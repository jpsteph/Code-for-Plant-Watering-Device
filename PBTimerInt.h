/*
 * PBTimerInt.h
 *
 * Created: 2/25/2023 12:47:54 PM
 *  Author: jpsteph
 */ 


#ifndef PBTIMERINT_H_
#define PBTIMERINT_H_
void PBinterrupt_init(void);

void Timer0_init(void);

void Timer0_disable(void);

void Timer0_enable(void);

void Timer1_init(void);

void Timer1_disable(void);

void Timer1_enable(void);

void Button_Pressed(void);

uint8_t Is_Timer0_Done(void);

void Reset_Timer0_Status(void);

void Reset_Timer1_Status(void);

void Timer1_Handle(uint16_t tic_thres);

void Stop_Timer0(void);

#endif /* PBTIMERINT_H_ */