//The MIT License (MIT)
//
//Copyright (c) 2014 Matthew Brejza
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <inttypes.h>
#include <stdint.h>


#define BYTES_PREAMBLE 4

#define BYTES_SYNC_WORD 4
//if <4 bytes, pad the bottom the sync word
#define SYNC_WORD 0x9A52E716


typedef struct Qpp_state_s{
	uint16_t df2;
	uint16_t f,g;
	uint16_t len;
	uint16_t k;
} Qpp_state_t;

typedef struct Sbi_state_s{
	uint8_t nd;
	uint8_t rows;
	uint8_t row;
	uint8_t col;
} Sbi_state_t;

//bits [9:0] = f2
//bits [18:10] = f1
//bits [25:19] = index

#define INT_C_40 ((uint32_t)(1<<19) | (3<<10) | (10))
#define INT_C_416 ((uint32_t)(48<<19) | (25<<10) | (52))

void encode_turbo(uint8_t *input, uint8_t *output_sys, uint8_t *output_par, uint16_t interleaver_len, uint32_t int_coeff);
uint16_t crc_xmodem_update (uint16_t crc, uint8_t data);
uint16_t channel_encode(uint8_t *input, uint8_t *output, uint16_t interleaver_len, uint32_t int_coeff, uint8_t rate);

