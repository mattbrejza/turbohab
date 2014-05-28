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

#define BYTES_POST_SYNC_WORD 1

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
// ((uint32_t)(1<<19) | (3<<10) | (10))
#define INT_C_40      527370
#define INT_C_88      1578006
#define INT_C_112     2139220
#define INT_C_136     2630690
#define INT_C_160     3167352
#define INT_C_184     3728430
#define INT_C_208     4222004
#define INT_C_232     4805690
#define INT_C_256     5258272
#define INT_C_280     5872850
#define INT_C_304     6329420
#define INT_C_328     6837330
#define INT_C_352     7361580
#define INT_C_376     7910494
#define INT_C_400     8543272
#define INT_C_424     8965226
#define INT_C_448     9467048
#define INT_C_472     9991286
#define INT_C_496     10646590
#define INT_C_528     11027522
#define INT_C_576     11600992
#define INT_C_624     12100842
#define INT_C_672     12627196
#define INT_C_720     13188216
#define INT_C_768     13853744
#define INT_C_816     14285926
#define INT_C_864     14697520
#define INT_C_912     15234162
#define INT_C_960     15758396
#define INT_C_1008    16309332
#define INT_C_1088    16952524
#define INT_C_1184    17321034
#define INT_C_1280    18029808
#define INT_C_1376    18371670
#define INT_C_1472    18920540
#define INT_C_1568    19411996
#define INT_C_1664    20110440
#define INT_C_1760    20474990
#define INT_C_1856    21030004
#define INT_C_1952    21556834
#define INT_C_2048    22051904
#define INT_C_2240    22758820
#define INT_C_2432    23340488
#define INT_C_2624    23620772
#define INT_C_2816    24161368
#define INT_C_3008    24802492
#define INT_C_3200    25279728
#define INT_C_3392    25742548
#define INT_C_3584    26273104
#define INT_C_3776    26922220
#define INT_C_3968    27647224
#define INT_C_4160    27821186
#define INT_C_4352    28800408
#define INT_C_4544    29201550
#define INT_C_4736    29433276
#define INT_C_4928    29924814
#define INT_C_5120    30448720
#define INT_C_5312    30975142
#define INT_C_5504    31478870
#define INT_C_5696    32027826
#define INT_C_5888    32836792
#define INT_C_6144    33299936

void encode_turbo(uint8_t *input, uint8_t *output_sys, uint8_t *output_par, uint16_t interleaver_len, uint32_t int_coeff);
uint16_t crc_xmodem_update (uint16_t crc, uint8_t data);
uint16_t channel_encode(uint8_t *input, uint8_t *output, uint16_t interleaver_len, uint32_t int_coeff, uint8_t rate);

