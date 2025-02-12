#ifndef	__ESP8266_H
#define	__ESP8266_H

#include "stm32f10x.h"                  // Device header

uint8_t SendCmd(char *cmd,char *Rcmd,uint16_t outtime);
void ESP8266_Init(void);
void GetTime_RecvData(void);
void ESP8266_SendData(char *data);
#endif


