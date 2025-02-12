#include "freertos_demo.h"
#include "OLED.h"
#include "MyRTC.h"
#include "Encoder.h"
#include "MyFLASH.h"
#include "Serov.h"
#include "Serial.h"
#include "ESP8266.h"
#include "string.h"
#include "Timer.h"
#include "Beep.h"
#include "Infrared.h"
#include "menu.h"


float Angle = 90;				//舵机角度	                                                         
char *RxString = NULL;					//接收手机指令字符串
uint8_t Num;   
					//密码	
uint8_t HW_Detect = 0;              //红外检测人体标志

#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

//系统运行
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

//舵机改变
#define TASK2_PRIO         4
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

//远程开锁
#define TASK3_PRIO         3
#define TASK3_STACK_SIZE   128
TaskHandle_t    task3_handler;
void task3( void * pvParameters );

////密码开锁
//#define TASK4_PRIO         2
//#define TASK4_STACK_SIZE   128
//TaskHandle_t    task4_handler;
//void task4( void * pvParameters );
/******************************************************************************************************/


void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (uint16_t               )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}


void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();             
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (uint16_t               )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (uint16_t               )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                
    xTaskCreate((TaskFunction_t         )   task3,
                (char *                 )   "task3",
                (uint16_t               )   TASK3_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK3_PRIO,
                (TaskHandle_t *         )   &task3_handler );
				
//	xTaskCreate((TaskFunction_t         )   task4,
//                (char *                 )   "task4",
//                (uint16_t               )   TASK4_STACK_SIZE,
//                (void *                 )   NULL,
//                (UBaseType_t            )   TASK4_PRIO,
//                (TaskHandle_t *         )   &task4_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();              
}

//运行正常测试
void task1( void * pvParameters )
{
	while(1)
	{
		Menu_Main();
	}
}

//舵机改变
void task2( void * pvParameters )
{

    while(1)
    {

		DetectData(&HW_Detect);//红外人体检测
		if(1 == HW_Detect)
		{
			Beep_ON();
			vTaskDelay(300);
			Beep_OFF();
		}
		if(0 == Angle)
		{	
			Angle = 90;
			vTaskDelay(300);
			Servo_SetAngle(Angle);
			Beep_OFF();
			
		}
		vTaskDelay(300);		
    }
}

//远程开锁
void task3( void * pvParameters )
{
	ESP8266_Init();
	uint8_t len;
    while(1)
    {
		if(Serial_GetRxFlag())
		{
			RxString = strstr((char *)Serial_RxPacket,"SERV+ON");

			if(RxString != NULL)
			{
				len = strlen(RxString);
				RxString[len - 2] = '\0';
				if(1 == MainFlag)
				{
					OLED_ShowString(16,48,RxString,OLED_8X16);
					MainFlag = 0;
				}
				else
					OLED_ShowString(100,0,"!!",OLED_8X16);
				OLED_Update();
				Num = 0;
				Angle = 0;
				Beep_ON();
				Servo_SetAngle(Angle);
				while(SendCmd("AT+CIPSENDEX=1,7\r\n",">",1000) != 1);//开启透传
				Serial_SendString("SERV+OK");
				Serial_RxPacket_Clear();  //接受包清除,需要在显示RxString后面，因为传递的是地址，内容会清空				
			}		
		}
     
        vTaskDelay(200);

    }
}
//
void task4( void * pvParameters )
{
    while(1)
    {
	

    }
}
