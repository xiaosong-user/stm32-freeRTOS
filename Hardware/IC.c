#include "stm32f10x.h"
#include "IC.h"

void IC_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			//开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);			//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//将PA0引脚初始化为上拉输入
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;               //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
	
	/*输入捕获初始化*/
	TIM_ICInitTypeDef TIM_ICInitStructure;							//定义结构体变量
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;				//选择配置定时器通道1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;							//输入滤波器参数，可以过滤信号抖动
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;		//极性，选择为上升沿触发捕获
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;			//捕获预分频，选择不分频，每次信号都触发捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//输入信号交叉，选择直通，不交叉
	TIM_PWMIConfig(TIM2,&TIM_ICInitStructure);			//TIM_PWMIConfig自动配置剩下的TI2通道为相反的属性，置TIM2的输入捕获通道
	
	/*选择触发源及从模式
	显然，这里的PWM输入模式正是利用2个捕捉通道针对同一信号进行的捕捉,
	只是分别针对上升沿和下降沿进行捕捉。要注意的是，利用上述PWM输入模
	式方法对外部输入信号的频率和占空比进行测量，只限于TI1或TI2通道。因
	为只有TI1FP1和TI2FP2接到了从模式控制器。这点，资料上有讲得清楚，
	有人有时无视了这点，将待测信号接到TI3或TI4后使用上述方法自然就不灵光了。
	要使用STM32内部正交编码功能的话，只能使用TI1和TI2两个输入通道。所以，应用时不要随便乱接。
	参考:https://blog.csdn.net/zoe512622789/article/details/54378948
	*/
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);					//触发源选择TI1FP1,只能选择TI1和TI2
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);					//从模式选择复位
																	//即TI1产生上升沿时，会触发CNT归零
	
	/*TIM使能*/
	TIM_Cmd(TIM2, ENABLE);			//使能TIM2，定时器开始运行
}

/**
  * 函    数：获取输入捕获的频率
  * 参    数：无
  * 返 回 值：捕获得到的频率
  */
uint32_t IC_GetFreq(void)
{
	return (1000000 / (TIM_GetCapture1(TIM2) + 1));		//测周法得到频率fx = fc / N，这里不执行+1的操作也可
}

uint16_t IC_GetDuty(void)
{
	return ((TIM_GetCapture2(TIM2)+1) * 100) /(TIM_GetCapture1(TIM2) + 1);
}
	

