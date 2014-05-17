#include <inttypes.h>
#include <stdint.h>
#include "libturbohab.h"
#include <string.h>

//to save on flash, combine interleaver1 and interleaver2 with a parameter

static uint8_t colPermPatInv[] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,
        1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};


//output_sys needs to have interleaver_len + 4 bits
//output_sys needs to have 2 * ceil(interleaver_len+4 / 32)*32 bits
void encode_turbo(uint8_t *input, uint8_t *output_sys, uint8_t *output_par, uint16_t interleaver_len)
{
    //does termination

    //work out output length
    //C = 32
    uint16_t D = interleaver_len + 4;            //number of bits per stream d0,d1,d2
    uint8_t rows,mask,current_byte,termination_l,termination_u;
    uint16_t i,addr;
    uint8_t *ptr;
    uint16_t bytes_per_stream;                    //used to clear memory
    uint8_t temp;

    //interleaver bit/addr storage
    uint16_t termuen_2_addr;
    uint8_t termuun_1_addr;


    //calculate rows = ceil(D/32)
    if ((D & 0x1F) == 0)
        rows = D >> 5;
    else
        rows = ( D >> 5 ) + 1;

    if ((D & 0x7) == 0)
        bytes_per_stream = D >> 3;
    else
        bytes_per_stream = ( D >> 3 ) + 1;


    //interleave the input into the output buffer (will be overwritten)
    Qpp_state_t state_qpp;
    interleaver_init(&state_qpp,3, 10, interleaver_len);   //<<<<<<<<<<<<<<<<<<<<<<<< change
    addr = 0;
    mask = 0x80;
    ptr = output_sys;
    for (i = 0; i < interleaver_len; i++)
    {
        if (*(input+(addr>>3)) & (1 << (7-(addr&0x7))))
            *ptr |= mask;

        mask = mask >> 1;
        if (mask == 0)
        {
            mask = 0x80;
            ptr++;
        }
        addr = interleaver_next(&state_qpp);
    }


    /////////////////////////////////////
    ////////// lower encoder ////////////
    /////////////////////////////////////

    //lower URC code (place into parity 1 spot for now)
    termination_l = urc_lte_termination_io(output_sys,interleaver_len);

    Sbi_state_t sbi_state;
    subblock_interleaver_init(&sbi_state,rows,(rows*32)-(interleaver_len+4));

    //fill in the termination we know (see 5.1.3.2.2 in ETSI TS 136 212 V11.4.0 (2014-01) )
    addr = interleaver_len + 2;
    if (termination_l & (1<<2))
        *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));
    addr++;
    if (termination_l & (1<<5))
        *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));

    //interleave lower parity into its final spot
    //is interleaved into alternate spots in the parity output
    addr = subblock_interleaver_reset(&sbi_state,1,2);
    mask = 0x40;
    ptr = output_par;
    temp = 0;
    for (i = 0; i < rows*32; i++)
    {
        if (addr >= interleaver_len)            //handle termination differently
        {
            if (addr > interleaver_len+1){  //last two termination bits known
                if (*(output_sys+(addr>>3)) & (1 << (7-(addr&0x7))))
                    *ptr |= mask;
            }
            else if (addr == interleaver_len)    //save addresses for later of bits we dont have
                termuun_1_addr = (temp<<3) + (mask2index(mask));
            else // (addr == interleaver_len+1)
                termuen_2_addr = (temp<<3) + (mask2index(mask));
        }
        else if (*(output_sys+(addr>>3)) & (1 << (7-(addr&0x7))))
            *ptr |= mask;

        mask = mask >> 2;
        if (mask == 0)
        {
            mask = 0x40;
            ptr++;
            temp++;
        }
        addr = subblock_interleaver_next(&sbi_state,1,2);
    }

    /////////////////////////////////////
    ////////// upper encoder ////////////
    /////////////////////////////////////

    //upper URC code (place into systematic spot for now)
    memset((void*)output_sys,0,bytes_per_stream);
    termination_u = urc_lte_termination(input,output_sys,interleaver_len);

    //fill in the termination
    addr = interleaver_len;
    if (termination_u & (1<<7))
        *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));
    addr++;
    if (termination_u & (1<<1))
        *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));
    addr++;
    if (termination_l & (1<<7))
        *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));
    addr++;
    if (termination_l & (1<<1))
        *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));

    //interleave upper parity into its final spot
    mask = 0x80;
    ptr = output_sys;
    current_byte = *ptr;
    for (i = 0; i < interleaver_len+4; i++)
    {
        addr = subblock_interleaver_addr_inv_nulls(&sbi_state,i)<<1;

        if (current_byte & mask)
            *(output_par+(addr>>3)) |= (1 << (7-(addr&0x7)));

        mask = mask >> 1;

        if (mask == 0)
        {
            mask = 0x80;
            ptr++;
            current_byte = *ptr;
        }
    }


    //add the termination into the d2 stream now we know it
    if (termination_u & (1<<2))
        *(output_par+(termuun_1_addr>>3)) |= (1 << (7-(termuun_1_addr&0x7)));
    if (termination_u & (1<<5))
        *(output_par+(termuen_2_addr>>3)) |= (1 << (7-(termuen_2_addr&0x7)));

    /////////////////////////////////////
    /////// systematic interleaver //////
    /////////////////////////////////////

    //fill in the termination
    temp = 0;
    if (termination_u & (1<<3))
        temp |= (1 << 0);
    if (termination_u & (1<<6))
        temp |= (1 << 1);
    if (termination_l & (1<<3))
        temp |= (1 << 2);
    if (termination_l & (1<<6))
        temp |= (1 << 3);


    //interleave the systematic into its final slot
    memset(output_sys,0,bytes_per_stream);


    addr = subblock_interleaver_reset(&sbi_state,0,1);
    mask = 0x80;
    ptr = output_sys;
    for (i = 0; i < interleaver_len+4; i++)
    {
        if (addr >= interleaver_len)            //handle termination differently
        {
            if ((addr == interleaver_len) && (temp&(1<<0)))
                *ptr |= mask;
            else if((addr == interleaver_len+1) && (temp&(1<<1)))
                *ptr |= mask;
            else if((addr == interleaver_len+2) && (temp&(1<<2)))
                *ptr |= mask;
            else if((addr == interleaver_len+3) && (temp&(1<<3)))
                *ptr |= mask;
        }
        else if (*(input+(addr>>3)) & (1 << (7-(addr&0x7))))
            *ptr |= mask;

        mask = mask >> 1;
        if (mask == 0)
        {
            mask = 0x80;
            ptr++;
        }
        addr = subblock_interleaver_next(&sbi_state,0,1);
    }








    /*
    for (i = 0; i < interleaver_len; i++)
    {
        addr = subblock_interleaver_addr_inv(&sbi_state,i);

        if (current_byte & mask)
     *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));

        mask = mask >> 1;

        if (mask == 0)
        {
            mask = 0x80;
            ptr++;
            current_byte = *ptr;
        }
    }
    for (i = 0; i < 4; i++)
    {
        addr = subblock_interleaver_addr_inv(&sbi_state,interleaver_len+i);
        if (temp & 1)
     *(output_sys+(addr>>3)) |= (1 << (7-(addr&0x7)));
        temp >>= 1;
    }
     */

    /////////////////////////////////////
    //////////// null removal ///////////
    /////////////////////////////////////

    //remove nulls from parity buffer
    uint8_t input_mask = 0x80;
    uint8_t output_mask = 0x80;
    uint8_t *input_ptr = output_par;
    uint8_t *output_ptr = output_par;
    temp = *input_ptr;
    *output_ptr = 0;
    addr = subblock_interleaver12_isnull_reset(&sbi_state);
    for (i = 0; i < rows*32; i++)
    {

        //stream1
        if (!(addr & 0x1))
        {
            if (temp & input_mask)
                *output_ptr |= output_mask;
            output_mask >>= 1;
        }
        input_mask >>= 1;
        if (input_mask == 0)
        {
            input_mask = 0x80;
            input_ptr++;
            temp = *input_ptr;
            *input_ptr = 0;
        }
        if (output_mask == 0)
        {
            output_mask = 0x80;
            output_ptr++;
        }



        //stream2
        if (!(addr & 0x2))
        {
            if (temp & input_mask)
                *output_ptr |= output_mask;
            output_mask >>= 1;
        }
        input_mask >>= 1;

        addr = subblock_interleaver12_isnull_next(&sbi_state);

        if (input_mask == 0)
        {
            input_mask = 0x80;
            input_ptr++;
            temp = *input_ptr;
            *input_ptr = 0;
        }
        if (output_mask == 0)
        {
            output_mask = 0x80;
            output_ptr++;
        }
    }
}


//returns termination. out[7:5] contains CODED termination, out[3:1] contains UNCODED termination (first bit MSB)
static uint8_t urc_lte_termination(uint8_t *input, uint8_t *output, uint16_t total_bits )
{
    uint16_t i;
    uint8_t state = 0;
    uint8_t state_n = 0;
    uint8_t io_mask = 0x80;
    *output = 0x00;
    for (i = 0; i < total_bits; i++)
    {
        state_n = state << 1;
        if (*input & io_mask)
            state_n |= ((state >> 1) & 1) ^ ((state >> 2) & 1) ^ 1;
        else
            state_n |= ((state >> 1) & 1) ^ ((state >> 2) & 1);

        if ((state_n & 1) ^ (state & 1) ^ ((state >> 2) & 1))
            *output |= io_mask;

        state = state_n;

        io_mask = io_mask >> 1;
        if (io_mask == 0)
        {
            io_mask = 0x80;
            output++;
            input++;
            *output = 0x00;
        }
    }

    //termination lookup style
    switch (state&0x7)
    {
    case 0: return 0x00;
    case 1: return 0xA6;
    case 2: return 0x4C;
    case 3: return 0xEA;
    case 4: return 0x88;
    case 5: return 0x2E;
    case 6: return 0xC4;
    default: return 0x62;
    }

}

//version which writes the output over the top of the input buffer
static uint8_t urc_lte_termination_io(uint8_t *io, uint16_t total_bits )
{
    uint16_t i;
    uint8_t state = 0;
    uint8_t state_n = 0;
    uint8_t io_mask = 0x80;
    uint8_t buff = 0;

    for (i = 0; i < total_bits; i++)
    {
        state_n = state << 1;
        if (*io & io_mask)
            state_n |= ((state >> 1) & 1) ^ ((state >> 2) & 1) ^ 1;
        else
            state_n |= ((state >> 1) & 1) ^ ((state >> 2) & 1);

        if ((state_n & 1) ^ (state & 1) ^ ((state >> 2) & 1))
            buff |= io_mask;

        state = state_n;

        io_mask = io_mask >> 1;
        if (io_mask == 0)
        {
            io_mask = 0x80;
            *io = buff;
            io++;
            buff = 0x00;
        }
    }

    //termination lookup style
    switch (state&0x7)
    {
    case 0: return 0x00;
    case 1: return 0xA6;
    case 2: return 0x4C;
    case 3: return 0xEA;
    case 4: return 0x88;
    case 5: return 0x2E;
    case 6: return 0xC4;
    default: return 0x62;
    }

}

//internal interleaver
static uint16_t interleaver_next(Qpp_state_t *st)
{

    st->f = st->g + st->f;
    st->g += st->df2;

    if (st->g >= st->len)
        st->g -= st->len;

    if (st->f >= st->len)
        st->f -= st->len;

    return st->f;

}

//initalise the internal interleaver
static uint16_t interleaver_init(Qpp_state_t *st,uint16_t f1, uint16_t f2, uint16_t len)
{
    st->df2 = f2 << 1;
    st->g = f1+f2;
    st->f = 0;
    st->k = 0;
    st->len = len;
    return 0;
}



//does not remove null value addresses
static uint16_t subblock_interleaver_addr_inv_nulls(Sbi_state_t *st, uint16_t i)
{
    i = i + st->nd;
    uint8_t col = colPermPatInv[i & 0x1F];
    uint8_t row = i >> 5;

    return st->rows*col + row;

}

//resets and outputs item0 of subblock interleaver 2
static uint16_t subblock_interleaver_reset(Sbi_state_t *st, uint8_t inc_nulls, uint8_t interleaver)
{
    st->row = 0;
    st->col = 0;

    if (st->nd > interleaver-1){
        if (inc_nulls)
            return 0;
        else
            return subblock_interleaver_next(st, inc_nulls,interleaver);
    }
    else
        return interleaver-st->nd-1;

}

//resets and outputs the item0 of isnull. Indictates null values
static uint16_t subblock_interleaver12_isnull_reset(Sbi_state_t *st)
{
    st->row = 0;
    st->col = 0;
    if (st->nd > 1)   //inter2
        return 0x3;
    else if (st->nd > 0)   //inter1
        return 0x1;
    else
        return 0x0;

}

//will return 0 for <null> if inc_nulls is set.
//interleaver is 1 or 2 depending on what subblock interleaver is used
//this function is slow compared to subblock_interleaver_addr_inv_nulls (12.4us vs 1.25us @ 16MHz)
static uint16_t subblock_interleaver_next(Sbi_state_t *st, uint8_t inc_nulls, uint8_t interleaver)
{
    uint16_t out;
    interleaver--;

    do{

        st->row++;
        if (st->row == st->rows)
        {
            st->row = 0;
            st->col++;
        }

        out = colPermPatInv[st->col] + 32*st->row + interleaver;
        if (out >= (uint16_t)(st->rows*32))
            out = out - st->rows*32;


    } while(!(inc_nulls) && (out < st->nd));

    if(out < st->nd)
        return 0;
    else
        return out-st->nd;

}


//out[0] is for interleaver1, out[1] for interleaver2
static uint8_t subblock_interleaver12_isnull_next(Sbi_state_t *st)
{
    uint16_t addr;
    uint8_t out = 0;

    st->row++;
    if (st->row == st->rows)
    {
        st->row = 0;
        st->col++;
    }

    addr = colPermPatInv[st->col] + 32*st->row;
    if (addr >= (uint16_t)(st->rows*32))
        addr = addr - st->rows*32;

    if(addr < st->nd)
        out = 1;


    addr = colPermPatInv[st->col] + 32*st->row + 1;
    if (addr >= (uint16_t)(st->rows*32))
        addr = addr - st->rows*32;

    if(addr < st->nd)
        out |= (1<<1);

    return out;
}



static void subblock_interleaver_init(Sbi_state_t *st, uint8_t rows, uint8_t nd)
{

    st->nd = nd;
    st->rows = rows;
    st->row = 0;
    st->col = 0;

}

static uint8_t mask2index(uint8_t input)
{
    uint8_t i;
    for(i = 0; i < 9; i++)
    {
        if (input == 0x80)
            return i;
        input <<= 1;
    }
    return 0;
}
