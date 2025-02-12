#ifndef __TIMER_H
#define __TIMER_H

void Timer_Init(void);
void TIM4_Delayms(u16 xms);
void TIM7_Int_Init(u16 arr,u16 psc);
#endif
