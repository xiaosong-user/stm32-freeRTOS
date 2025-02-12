#include "usart1.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "Timer.h"

uint8_t USART1_RxPacket[USART_REC_LEN];
uint8_t USART1_RxFlag;
uint8_t pRxPacket3;
void usart1_init(u32 bound)
{  	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);
	
	
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	
		/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	USART_Cmd(USART3,ENABLE);                   //使能串口 
  
}

void USART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART3,Byte);
	while(RESET == USART_GetFlagStatus(USART3,USART_FLAG_TXE));
}


/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void USART1_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		USART1_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：接收数组清空
  * 参    数：无
  * 返 回 值：无
  */
void USART3_RxPacket_Clear(void)
{
	memset(USART1_RxPacket,0,USART_REC_LEN);
	pRxPacket3 = 0;
}



/**
  * 函    数：获取串口接收标志位
  * 参    数：无
  * 返 回 值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，读取后标志位自动清零
  */
uint8_t USART1_GetRxFlag(void)
{
	if(pRxPacket3 > 0)
	{
		TIM4_Delayms(20);
		USART1_RxFlag = 1;
	}
	if (USART1_RxFlag == 1)			//如果标志位为1
	{
		USART1_RxFlag = 0;
		return 1;					//则返回1，并自动清零标志位
	}
	return 0;						//如果标志位为0，则返回0
}


void USART3_IRQHandler(void)                	//串口1中断服务程序
{

	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)		//判断是否是USART1的接收事件触发的中断
	{
		uint8_t RxData = USART_ReceiveData(USART3);
		USART1_RxPacket[pRxPacket3] = RxData;
		pRxPacket3 ++;	
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		//清除标志位
	}

}


 
