#include "stm32f10x.h"                  // Device header
#include "Timer.h"

int16_t Encoder_Count,B_level,Cnt;
uint8_t KeyNum;


void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

}

int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp = Encoder_Count;
	Encoder_Count = 0;
	return Temp;
}

uint8_t KeyNum_Get(void)
{
	int16_t Temp;
	Temp = KeyNum;
	KeyNum = 0;
	return Temp;
}


void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		TIM4_Delayms(10);
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) ==0)
		{
			KeyNum = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}

	else if (EXTI_GetITStatus(EXTI_Line10) == SET)
	{ 
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 0 && Cnt==0)//A相下降沿触发第一次中断
		{
			Cnt++;//计数值加一，表示已经触发了第一次中断
			B_level=0;//读取B相电平，若为高电平则B_level置1，反之保持0
			if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 1)
			{
				B_level=1;
			}
		}
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 1 && Cnt==1)//A相上升沿触发第二次中断
		{
			Cnt=0;//计数清零
			if(B_level==1 && GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 0) 
			{
				Encoder_Count++;//正转
			}
			if(B_level==0 && GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12) == 1)
			{
				Encoder_Count--;//反转
			}	 
		}	
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
}

//void TIM4_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
//	{
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//	}
//}
