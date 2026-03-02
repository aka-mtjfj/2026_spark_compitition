#include "stm32f1xx.h" 
#ifndef __Menu_H
#define __Menu_H
uint16_t Menu1(void);
uint16_t Menu2_Timer(void);
uint16_t Menu2_Clock(void);
uint16_t Menu2_Setting(void);
extern uint32_t timer1_cnt,timer2_cnt,timer3_cnt;
extern uint32_t clock1_cnt,clock2_cnt,clock3_cnt;
extern uint8_t timer_yn[3];
extern uint8_t clock_yn[3];
extern uint8_t clock1_arr[3];
extern uint8_t clock2_arr[3];
extern uint8_t clock3_arr[3];
#endif
