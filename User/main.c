#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyRTC.h"
#include "Encoder.h"
#include "MyFLASH.h"
#include "Store.h"
#include "Serov.h"
#include "Serial.h"
#include "ESP8266.h"
#include "string.h"
#include "Timer.h"
#include "freertos_demo.h"
#include "Beep.h"
#include "Infrared.h"
#include "menu.h"
#include "usart1.h"


/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0             X轴           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */





int main(void)
{       
	OLED_Init(); 
	MyRTC_Init();
	Servo_Init();
	Serial_Init();
//	USART_Cmd(USART2,DISABLE); 
//	USART_Cmd(USART3,DISABLE);
	Store_Init();
	Encoder_Init();
	Beep_Init();
	BODY_HW_Init();
	usart1_init(57600);
	
	freertos_demo();
	
}




/************手机控制开锁*****************************
int main(void)
{       
	OLED_Init(); 
	usart2_init(9600);
	Serial_Init();
	ESP8266_Init();
	Timer_Init();
	Servo_Init();
	while(1)
	{
		Angle = 90;
		OLED_ShowString(0,16,"Ready",OLED_8X16);
		Delay_ms(200);
		OLED_ShowString(0,16,"     ",OLED_8X16);
		Delay_ms(200);
		Servo_SetAngle(Angle);			//设置舵机的角度为角度变量
		OLED_ShowString(0,32,"Servo is closing",OLED_8X16);
		if(Serial_GetRxFlag())
		{
			RxString = strstr((char *)Serial_RxPacket,"ORDER");
			if(RxString != NULL)
			{
				OLED_ShowString(0,0,RxString,OLED_8X16);
				Num = 0;
				Angle = 0;
				Servo_SetAngle(Angle);			//设置舵机的角度为角度变量
				OLED_ShowString(0,32,"Servo is Opening",OLED_8X16);
				ESP8266_SendData("DATA:SERV+OK\r\n");
				Serial_RxPacket_Clear();
			}		
		}		
	}
	
}

*****************手机控制开锁************************************/




/**************舵机**********************
		Num = KeyNum_Get();

		if(1 == Num)
		{
			Num = 0;
			Angle = 0;
		}
		Servo_SetAngle(Angle);			//设置舵机的角度为角度变量
		OLED_ShowNum(48, 0, Angle, 3,OLED_8X16);	//OLED显示角度变量
		Delay_ms(1000);
		Angle = 90;
		Servo_SetAngle(Angle);			//设置舵机的角度为角度变量+
**************舵机**********************/	


/**Power停止模式****************************
int main(void)
{
	uint16_t Num = 0;
	OLED_Init(); 
	Encoder_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	OLED_Clear();
	OLED_ShowString(0,0,"SYSCLK:",OLED_8X16);
	OLED_ShowNum(56,0,SystemCoreClock,8,OLED_8X16);
	while(1)
	{
		Num +=  KeyNum_Get();
		OLED_ShowNum(0,32,Num,4,OLED_8X16);
		OLED_ShowString(0,16,"Runing",OLED_8X16);
		Delay_ms(500);
		OLED_ShowString(0,16,"      ",OLED_8X16);
		Delay_ms(500);
		PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
		SystemInit();
	}
}
******Power停止模式***********************/

