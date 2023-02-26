/*
 * PBTimerInt.c
 *
 * Created: 2/25/2023 12:47:28 PM
 *  Author: jpsteph
 */ 


#include <avr/interrupt.h>
#include "LED.h"
#include "clock.h"
#include <util/delay.h>
#include "uart.h"

volatile uint16_t tic_count0 = 0;
volatile uint16_t tic_count1 = 0;
volatile uint8_t button_count = 0;
volatile uint8_t timer0_done = 0;
volatile uint8_t timer1_done = 0;
volatile uint16_t timer1_tic_thres = 0;

//INT6 init for push button
void PBinterrupt_init(void) {
	//Set INT to rising edge
	EICRB |= (1<<ISC61)|(1<<ISC60);
	//Allow INT to trigger
	EIMSK |= (1<<INT6);
	sei();
}

void Timer0_init(void) {
	/* Timer clock = I/O clock / 1024 */
	TCCR0B = (1<<CS02)|(1<<CS00);
	/* Clear overflow flag */
	TIFR0 = 1<<TOV0;
	/* Enable Overflow Interrupt */
	TIMSK0 = 1<<TOIE0;
}

void Timer0_disable(void) {
	TCCR0B = (0<<CS02)|(0<<CS00);
}

void Timer0_enable(void) {
	TCCR0B = (1<<CS02)|(1<<CS00);
}

void Timer1_init(void) {
	/* Timer clock = I/O clock / 1024 */
	TCCR1B = (1<<CS12)|(1<<CS10);
	/* Clear overflow flag */
	TIFR1 = 1<<TOV1;
	/* Enable Overflow Interrupt */
	TIMSK1 = 1<<TOIE1;
	
}

void Timer1_disable(void) {
	TCCR1B = (0<<CS12)|(0<<CS10);
}

void Timer1_enable(void) {
	TCCR1B = (1<<CS12)|(1<<CS10);
}

void Button_Pressed(void) {
	_delay_ms(200);
	LEDBLINKSHORT();
	button_count++;
	tic_count0 = 0;
	if(button_count)
		Timer0_enable();
	
	if(button_count > 3) {
		LEDBLINKLONG();
		button_count = 0;
		tic_count0 = 0;
		Timer0_disable();
		USART_Transmit_String("RESET");
	}
}

uint8_t Is_Timer0_Done(void) {
	if(timer0_done) {
		Timer0_disable();
		USART_Transmit_String("TIMER DONE");
		LEDBLINKLONG();
		
		tic_count0 = 0;
		timer0_done = 0;
		
		if(button_count == 1) {
			button_count = 0;
			USART_Transmit_String("DEBUG MODE");
			return 1;
		}
		else if(button_count == 2) {
			button_count = 0;
			USART_Transmit_String("NORMAL MODE");
			return 2;
		}
		else if(button_count == 3) {
			button_count = 0;
			USART_Transmit_String("INITIAL MODE");
			return 3;
		}
		return 0;
	}
	return 0;
}


void Reset_Timer0_Status(void) {
	tic_count0 = 0;
	button_count = 0;
	timer0_done = 0;
		
}

void Reset_Timer1_Status(void) {
	tic_count1 = 0;
	timer1_tic_thres = 0;
	timer1_done = 0;
	
}

void Timer1_Handle(uint16_t tic_thres) {
	USART_Transmit_String("STARTING TIMER1 DELAY");
	timer1_tic_thres = tic_thres;
	Timer1_enable();
	while(timer1_done == 0);
	Timer1_disable();
	Reset_Timer1_Status();
	USART_Transmit_String("END DELAY");
}

void Stop_Timer0(void) {
	sei();
	_delay_ms(50);
	Timer0_disable();
	Reset_Timer0_Status();
}

ISR(INT6_vect)
{
	Button_Pressed();
}

ISR(TIMER0_OVF_vect)
{
	tic_count0++;
	USART_Transmit_Int16(tic_count0);  //debug
	if (tic_count0 > 100)
		timer0_done = 1;
}

ISR(TIMER1_OVF_vect)
{
	tic_count1++;
	//USART_Transmit_String("TIMER 1 TIC:");
	USART_Transmit_Int16(tic_count1);  //debug
	if (tic_count1 > timer1_tic_thres) {
	timer1_done = 1;
	//USART_Transmit_String("TIMER IS DONE");
	}
}