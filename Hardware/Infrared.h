#ifndef __INFRARED_H_
#define __INFRARED_H_

#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"

void BODY_HW_Init(void);
uint8_t BODY_HW_GetData(void);
void DetectData(uint8_t *value);

#endif
