#ifndef __USART2_H
#define __USART2_H 

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

#define USART_REC_LEN 200
extern uint8_t USART1_RxPacket[];
extern uint8_t USART1_RxFlag;


void USART3_RxPacket_Clear(void);
void usart1_init(u32 bound);
void USART1_SendByte(uint8_t Byte);
uint8_t USART1_GetRxFlag(void);
void USART1_SendArray(uint8_t *Array, uint16_t Length);
void USART1_SendString(char *String);
uint32_t USART1_Pow(uint32_t x, uint32_t y);
void USART1_SendNumber(uint32_t Number, uint8_t Length);
void USART1_Printf(char *format, ...);
#endif
