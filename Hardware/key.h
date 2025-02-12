#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"                  // Device header

#define KEY1_PROT  GPIOA
#define KEY1_Pin   GPIO_Pin_0
#define KEY1_RCC   RCC_APB2Periph_GPIOA

void Key_Init(void);

uint8_t Key_Back_Get(void);


#endif
