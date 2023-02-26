/*
 * millis.c
 *
 * Created: 2/11/2023 1:04:11 PM
 *  Author: jpsteph
 */ 


#include "millis.h"
#include <avr/io.h>
#include <util/atomic.h>

volatile unsigned long timer1_millis;

void millis_init()
{
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);
	
	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = ((uint16_t) CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = (uint16_t) CTC_MATCH_OVERFLOW;

	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);
	
}

unsigned long millis()
{
	unsigned long millis_return;
	// ensure this cannnot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		millis_return = timer1_millis;
	}
	return millis_return;
}

//delays CPU operation by input using millis()
void delay(unsigned long delay_ms) {
	unsigned long while_delay_ms = millis() + delay_ms;
	while (millis() < while_delay_ms);
}

ISR (TIMER1_COMPA_vect)
{
	timer1_millis++;
}

