#include "Infrared.h"
#include "Beep.h"
#include "ESP8266.h"
#include "LED.h"

void BODY_HW_Init(void)
{
		RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOD, ENABLE );	// 打开连接 传感器DO 的单片机引脚端口时钟
	
		GPIO_InitTypeDef GPIO_InitStructure;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;			// 配置连接 传感器DO 的单片机引脚模式
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			// 设置为下拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOD, &GPIO_InitStructure);				// 初始化 
	
}



uint8_t BODY_HW_GetData(void)
{
	return GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
}

void DetectData(uint8_t *value)//红外触发发给主机信息
{
//	static  uint8_t flag_In = 0;//防止重复发送
	if(BODY_HW_GetData() && (0 == *value))//可防止重复进入
	{
		ESP8266_SendData("DATA:door+mei ren");
		*value = 1;
//		if(flag_In == 0)
//		{
			
//			flag_In = 1;
//		}
	
	}
	else if(!BODY_HW_GetData() && (1 == *value))
	{
		ESP8266_SendData("DATA:DOOR+you ren");
		*value = 0;
//		if(flag_In == 1)
//		{
			
//			flag_In = 0;
//		}
	}
}

