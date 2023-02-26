/*
 * ADC.h
 *
 * Created: 2/9/2023 8:27:38 PM
 *  Author: jpsteph
 */ 


#ifndef ADC_H_
#define ADC_H_

void adc_init(void);

uint16_t adc_read(uint8_t ch);

#endif /* ADC_H_ */