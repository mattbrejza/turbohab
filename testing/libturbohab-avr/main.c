#include <avr/io.h>
#include <util/delay.h>
#include "libturbohab.h"


int main(void)
{

	DDRB |= (1<<7);
	DDRB |= (1<<0);
	while(1)
	{
		
		
		uint8_t input[125] = {0};
		uint8_t outsys[400] = {0};
		uint8_t outpar[400] = {0};
		
		PORTB = (1<<7);
		encode_turbo(input, outsys,outpar, 1000);		
		PORTB = 0;
		
		_delay_ms(1000);
	}


}