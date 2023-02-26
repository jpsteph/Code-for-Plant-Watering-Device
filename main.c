/*
 * ADCmoisturesensor.c
 *
 * Created: 2/9/2023 8:25:06 PM
 * Author : jpsteph
 */ 

#include <avr/io.h>
#include "uart.h"
#include "ADC.h"
#include "clock.h"
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <string.h>
#include "LED.h"
#include "PBTimerInt.h"

#define PUMP 5
#define PUMPINIT() DDRB |= (1 << PUMP);\
					PORTB |= (1 << PUMP);
#define PUMPON() PORTB &= ~(1 << PUMP)
#define PUMPOFF() PORTB |= (1 << PUMP)


int main(void)
{
	int8_t button_status = 0;
	int8_t system_status = 0;
	int8_t debug_flag = 0;
	char rxbuffer[50];
	int8_t filledbuffsize;
	float adcnum;
	float sens_limit = 3.3;
	
	USART_Init(9600);
	USART_Transmit_String("USART INIT");
	adc_init();
	LEDINIT();
	PUMPINIT();
	
	PBinterrupt_init();
	Timer0_init();
	Timer0_disable();
	Timer1_init();
	Timer1_disable();
	
	//debug cmds
	char adc[] = "adc";
	//char adcOFF[] = "adcoff";
	char pumpON[] = "pumpon";
	char pumpOFF[] = "pumpoff";
	char endDBG[] = "endbug";
	
	set_sleep_mode (SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	
    /* Replace with your application code */
    while (1) 
    {
		sleep_disable();
		
		button_status = Is_Timer0_Done();
		if(button_status != 0)
			system_status = button_status;
			
		//debug mode
		if(system_status == 1)
		{
			filledbuffsize = USART_Receive_String(rxbuffer);
		
			//USART_Transmit_String2(rxbuffer, filledbuffsize - 1);
			USART_Transmit_String("");
		
			debug_flag = USART_cmd(rxbuffer, adc, strlen(adc));
			if(debug_flag) {
				//number will range from 1.5 to 3
				for(int i = 0; i < 5; i++) {
				adcnum = ((float) adc_read(0)) * 3.3 / 1023;
				USART_Transmit_Float(adcnum);
				_delay_ms(500);
				}
			}
		
			debug_flag = USART_cmd(rxbuffer, pumpON, strlen(pumpON));
			if(debug_flag) {
				PUMPON();
				cli();
			}
		 
			debug_flag = USART_cmd(rxbuffer, pumpOFF, strlen(pumpOFF));
			if(debug_flag) {
				PUMPOFF();
				sei();
				_delay_ms(50);
				Timer0_disable();
				Reset_Timer0_Status();
			}
			
			debug_flag = USART_cmd(rxbuffer, endDBG, strlen(endDBG));
			if(debug_flag) {
				system_status = 0;
				USART_Transmit_String("ENDING DEBUG MODE");
			}
	 	 
			 LEDBLINK();
			 debug_flag = 0;
		}
		
		if(system_status == 2) {
			uint8_t is_dry = 0;
		
			cli();
			USART_Transmit_String("ACTIVATING NORMAL OPERATION");
			adcnum = ((float) adc_read(0)) * 3.3 / 1023;
			USART_Transmit_Float(adcnum);
			if(adcnum > sens_limit) {
				PUMPON();
				is_dry = 1;
			}
			
			while(is_dry) {
				_delay_ms(100);
				adcnum = ((float) adc_read(0)) * 3.3 / 1023;
				USART_Transmit_Float(adcnum);
				if(adcnum < sens_limit) {
					is_dry = 0;
				}
			}
			
			PUMPOFF();
			system_status = 2;
			USART_Transmit_String("EXITING NORMAL OPERATION");
			Stop_Timer0();
			Timer1_Handle(50);
		}
		
		//config routine
		if(system_status == 3)  {
			float adc_high;
			float adc_low;
			float adc_low_old = 100;
			uint8_t setting_up = 1; 
			cli();
			
			while(setting_up == 1) {
			adc_high = ((float) adc_read(0)) * 3.3 / 1023;
			PUMPON();
			_delay_ms(5000);
			
			PUMPOFF();
			
			adc_low = ((float) adc_read(0)) * 3.3 / 1023;
			if(adc_low_old == 100) //first time case
				adc_low_old = adc_low;
			
			USART_Transmit_String("ADC HIGH VALUE:");
			USART_Transmit_Float(adc_high);
			USART_Transmit_String("ADC LOW VALUE:");
			USART_Transmit_Float(adc_low);
			USART_Transmit_String("ADC OLD LOW VALUE:");
			USART_Transmit_Float(adc_low_old);
			
			USART_Transmit_String("ADC HIGH TO LOW VALUE:");
			USART_Transmit_Float(adc_high - adc_low);
			USART_Transmit_String("ADC LOW DIFF BTW CYCLES:");
			USART_Transmit_Float(adc_low_old - adc_low);
			USART_Transmit_String("");
			USART_Transmit_String("");
			
			_delay_ms(20000);
			
			if((adc_high - adc_low) < 0.09) {		//wet soil wont change sensor adc value that much 
				if((adc_low_old - adc_low) < 0.03) {		//if soil is wet adc value after watering should become saturater after a few pump cycles 
					sens_limit = adc_low + 0.5;
					setting_up = 0;
					system_status = 2; //go to normal opertion 
				}
			}
			adc_low_old = adc_low;
			}
			USART_Transmit_String("SET UP FINISHED");
			USART_Transmit_String("SENSOR THRESHOLD WILL BE:");
			USART_Transmit_Float(sens_limit);
			Stop_Timer0();
		}
		
		
		sleep_enable();
	
	}
}
