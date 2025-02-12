#include "Serial.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "MyRTC.h"
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  发送AT指令，检查AT指令的返回，判断AT指令是否发送成功
  * @param  cmd:发送的AT指令
  * @param  Rcmd:AT指令对应的返回值
  * @param  outtime:ESP8266超时回复判断
  * @retval 
  */
uint8_t SendCmd(char *cmd,char *Rcmd,uint16_t outtime)
{
  uint8_t ACK_Flag = 0;
  
  //发送AT指令之前要清空
  
	Serial_RxPacket_Clear();
  
	while(outtime) 
	{
		Serial_SendString(cmd);//发送AT指令 -- > 才会进入串口2中断服务
		vTaskDelay(10);
		if(Serial_GetRxFlag() == 1)
		{
		  if(strstr((char *)Serial_RxPacket,Rcmd) != NULL)
		  {
			ACK_Flag = 1;
			break;
		  }
		}
		outtime--;
	}
	return ACK_Flag;
}

void ESP8266_Init(void)
{
	Serial_SendString("ATE0\r\n");
	while(SendCmd("AT+CWMODE_CUR=3\r\n","OK",1000) != 1);
	while(SendCmd("AT+CIPMUX=1\r\n","OK",1000) != 1);
	while(SendCmd("AT+CWJAP_CUR=\"xiaosong\",\"12345678\"\r\n","OK",1000) != 1);
	//必须加上1标号，不然连不上，可能0标号被抢占。
	while(SendCmd("AT+CIPSTART=1,\"TCP\",\"192.168.4.1\",8080\r\n","OK",1000)!=1);
}

void ESP8266_SendData(char *data)
{
	while(SendCmd("AT+CIPSENDEX=1,19\r\n",">",1000) != 1);//开启透传
	Serial_SendString(data);
}


/**
  * @brief  解析JSON数据
  * @param  
  * @retval 
  */
void GetTime_RecvData(void)
{
  char Val[20] = {0};
  char tep[20] = {0};
  uint8_t i = 0;
  uint32_t timeget = 0;
//  long long timeget = 0;
  if(1 == Serial_GetRxFlag())
  {
    if(pRxPacket > 50)//判断是否是 下发指令的回传的指令 而不是上传时系统默认下发的 //防止有影响
    {
    char *addr =  strstr((char *)(Serial_RxPacket+6),"\"t\"");
    addr+=5;
    while(*(addr + i) != '"')
    {
      Val[i] = *( addr+i);
      i++;
    }
//    sscanf(Val, "%lld", &timeget);  // 存储类型为long long int
//    printf("%lld\r\n",timeget);
    
    for(i = 0;i<10;i++) //这种要去掉3位 才能使用atoi转换
    {
      tep[i] = Val[i];
    }
    timeget = atoi(tep);
	Serial_RxPacket_Clear();
    MyRTC_Init();
    
    }
  }
}
