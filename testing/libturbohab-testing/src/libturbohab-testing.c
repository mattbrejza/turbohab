/*
 ============================================================================
 Name        : libturbohab-testing.c
 Author      : Matt Brejza
 Version     :
 Copyright   : Copyright Matt Brejza 2014
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "libturbohab.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>


int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	int i;







/*
	//Qpp_state_t state;
	Sbi_state_t state_sbi;




	uint16_t interleaver1inv[40] = {};
	uint16_t interleaver2[40] = {};
	int i;

	subblock_interleaver_init(&state_sbi,2,24);


	interleaver1inv[0] = subblock_interleaver_addr_inv(&state_sbi,0);
	interleaver2[0] = subblock_interleaver2_reset(&state_sbi);

	//interleaver[0] = interleaver_init(&state,3, 10, 40);

	for (i = 1; i < 35; i++){
	//	interleaver[i] = interleaver_next(&state);
		interleaver1inv[i] = subblock_interleaver_addr_inv(&state_sbi,i);
		interleaver2[i] = subblock_interleaver2_next(&state_sbi);
	}
	for (i = 35; i < 40; i++){
	//	interleaver[i] = interleaver_next(&state);
		interleaver1inv[i] = subblock_interleaver_addr_inv(&state_sbi,i);
		interleaver2[i] = subblock_interleaver2_next(&state_sbi);
	}
*/

	//now test the encoder

	//uint8_t input[] = {78, 142, 143, 148, 122, 0, 0, 0, 0, 0, 0};
	//uint8_t answers[] = {188, 215, 35, 204, 144, 192};
	//uint8_t answerp[] = {136, 6, 164, 184, 166, 194, 151, 178, 181, 129, 182};

	//0
	//uint8_t input[] = {185,53,191,129,157,0,0,0};
	//uint8_t answers[] = {63,132,206,160,255,160,0,0};
	//uint8_t answerp[] = {207,186,123,62,87,234,119,171,40,29,42,0,0};

	//1
	//uint8_t input[] = {129,99,102,144,208,0,0,0};
	//uint8_t answers[] = {3,180,140,149,200,208,0,0};
	//uint8_t answerp[] = {4,110,210,92,246,124,200,214,58,121,83,0,0};

	//4
	//uint8_t input[] = {254,79,143,142,185,0,0,0};
	//uint8_t answers[] = {255,158,99,201,85,128,0,0};
	//uint8_t answerp[] = {12,152,18,74,120,197,132,94,59,182,206,0,0};

	//4 w/ checksum
	uint8_t input[] = {254,79,143,0,0,0,0,0};
	uint8_t answers[] = {182,22,66,201,81,0,0,0};
	uint8_t answerp[] = {44,72,207,90,59,196,193,114,189,14,254,0,0};


	uint8_t outsys[10] = {0};
	uint8_t outpar[27] = {0};
	uint8_t outall[400] = {0xFF};

	uint8_t input2[] = {0x87, 0x00, 0xa4, 0x4a, 0x4f, 0x45, 0x59, 0x01, 0xcd, 0x28, 0xf5, 0x02, 0xcd, 0xf0, 0x8e, 0x03, 0x93, 0xce, 0x1e, 0x5b, 0x92, 0x16, 0xd2, 0xff, 0x2c, 0x8c, 0x5c, 0x10, 0x04, 0x05, 0x05, 0x03, 0x0a, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc9, 0x1e};

	uint16_t ll = channel_encode(input2,outall,328,INT_C_328,0);



//	encode_turbo(input, outsys,outpar, 40,INT_C_40);
	uint16_t l = channel_encode(input,outall,40,INT_C_40,0);

	uint8_t errors=0;

	for (i = 0; i < 11; i++)
	{
		if (answerp[i] != outpar[i])
			errors++;
	}
	for (i = 0; i < 6; i++)
	{
		if (answers[i] != outsys[i])
			errors++;
	}

	return EXIT_SUCCESS;
}
