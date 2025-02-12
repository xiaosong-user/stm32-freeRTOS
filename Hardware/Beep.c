#include "Beep.h"



void Beep_Init(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}



void Beep_ON(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_9); 
}


void Beep_OFF(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_9); 
}

void Beep_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_9) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_9);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);					//则设置PA1引脚为低电平
	}
}

