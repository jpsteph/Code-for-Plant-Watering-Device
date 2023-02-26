/*
 * millis.h
 *
 * Created: 2/11/2023 1:04:22 PM
 *  Author: jpsteph
 */ 
#include "clock.h"

#ifndef MILLIS_H_
#define MILLIS_H_

// the CTC match value in OCR1A
#define CTC_MATCH_OVERFLOW ((F_CPU/1000)/8)

void millis_init();

unsigned long millis();

void delay(unsigned long delay_ms);
#endif /* MILLIS_H_ */