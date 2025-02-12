#ifndef __SERIAL_H
#define __SERIAL_H


#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>


extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];
extern uint8_t pRxPacket;

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
uint8_t Serial_GetRxFlag(void);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
uint32_t Serial_Pow(uint32_t x, uint32_t y);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
int fputc(int ch, FILE *f);
void Serial_Printf(char *format, ...);
void Serial_SendPacket(void);
void Serial_RxPacket_Clear(void);

#endif
