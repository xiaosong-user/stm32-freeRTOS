#include "MyRTC.h"
#include <time.h>

uint16_t MyRTC_Time[] = {2024,12,4,21,34,50};


void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		RCC_LSEConfig(RCC_LSE_ON);
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);
		
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		
		RTC_SetPrescaler(32768 - 1);
		RTC_WaitForLastTask();
		
		MyRTX_SetTime();
		
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
	}
	else
	{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}

void MyRTX_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year = MyRTC_Time[0] - 1900;
	time_date.tm_mon = MyRTC_Time[1] - 1;
	time_date.tm_mday = MyRTC_Time[2];
	time_date.tm_hour = MyRTC_Time[3];
	time_date.tm_min = MyRTC_Time[4];
	time_date.tm_sec = MyRTC_Time[5];
	
	time_cnt = mktime(&time_date) - 8 * 60 * 60;
	
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

void MyRTC_ReadTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;
	
	time_date = *localtime(&time_cnt);
	
	
	MyRTC_Time[0] = time_date.tm_year + 1900;		
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
}

//	OLED_Clear();

//	/*???????*/
//	OLED_ShowString(0, 0, "Date:XXXX-XX-XX",OLED_8X16);
//	OLED_ShowString(0, 16, "Time:XX:XX:XX",OLED_8X16);
//	OLED_ShowString(0, 32, "CNT :",OLED_8X16);
//	OLED_ShowString(0, 48, "DIV :",OLED_8X16);
//	
//	
//	while(1)
//	{
//		MyRTC_ReadTime();
//		
//		OLED_ShowNum(40, 0, MyRTC_Time[0], 4,OLED_8X16);		//??MyRTC_Time???????,?
//		OLED_ShowNum(80, 0, MyRTC_Time[1], 2,OLED_8X16);		//?
//		OLED_ShowNum(104, 0, MyRTC_Time[2], 2,OLED_8X16);		//?
//		OLED_ShowNum(40, 16, MyRTC_Time[3], 2,OLED_8X16);		//?
//		OLED_ShowNum(64, 16, MyRTC_Time[4], 2,OLED_8X16);		//?
//		OLED_ShowNum(88, 16, MyRTC_Time[5], 2,OLED_8X16);		//?
//		
//		OLED_ShowNum(40, 32, RTC_GetCounter(), 10,OLED_8X16);	//??32??????
//		OLED_ShowNum(40, 48, RTC_GetDivider(), 10,OLED_8X16);	//???????
//		
//		OLED_Update();
//	}
