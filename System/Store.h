#ifndef __STORE_H
#define __STORE_H

#include "stm32f10x.h"                  // Device header

extern uint16_t Store_Data[];

void Store_Init(void);
void Store_Save(void);
void Store_Clear(void);

#endif
