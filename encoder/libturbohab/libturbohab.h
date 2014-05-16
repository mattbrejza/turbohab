#include <inttypes.h>
#include <stdint.h>


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
	uint8_t null_lookup[32];
} Sbi_state_t;


void encode(uint8_t *input, uint8_t *output_sys, uint8_t *output_par, uint16_t interleaver_len);
uint8_t urc_lte_termination(uint8_t *input, uint8_t *output, uint16_t total_bits );
uint8_t urc_lte_termination_io(uint8_t *io, uint16_t total_bits );
uint16_t interleaver_next(Qpp_state_t *st);
uint16_t interleaver_init(Qpp_state_t *st,uint16_t f1, uint16_t f2, uint16_t len);
uint16_t subblock_interleaver_addr_inv_nulls(Sbi_state_t *st, uint16_t i);
uint16_t subblock_interleaver_addr_inv(Sbi_state_t *st, uint16_t i);
uint16_t subblock_interleaver1_reset(Sbi_state_t *st);
uint16_t subblock_interleaver1_isnull_reset(Sbi_state_t *st);
uint16_t subblock_interleaver1_next(Sbi_state_t *st);
uint8_t subblock_interleaver1_isnull_next(Sbi_state_t *st);
uint16_t subblock_interleaver2_reset(Sbi_state_t *st);
uint16_t subblock_interleaver2_isnull_reset(Sbi_state_t *st);
uint16_t subblock_interleaver2_next(Sbi_state_t *st);
uint8_t subblock_interleaver2_isnull_next(Sbi_state_t *st);
void subblock_interleaver_init(Sbi_state_t *st, uint8_t rows, uint8_t nd);
uint8_t subblock_interleaver12_isnull_next(Sbi_state_t *st);
uint16_t subblock_interleaver12_isnull_reset(Sbi_state_t *st);
uint8_t mask2index(uint8_t input);
