#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"   

#define LED1_PROT  GPIOC
#define LED1_Pin   GPIO_Pin_8
#define LED1_RCC   RCC_APB2Periph_GPIOC


void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);

#endif
