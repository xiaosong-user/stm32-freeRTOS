#include "menu.h"
#include "Store.h"
#include "Beep.h"
#include "freertos_demo.h"
#include "Serov.h"
#include "as608.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Timer.h"
#include "MyRTC.h"


uint8_t TIM_Flag; //Enter键标志位
int PassWord;//密码
uint8_t PassFlag = 0;//修改密码标志位
SysPara AS608Para;//指纹模块AS608参数
int MenuNum = 0;//按键获得值
uint16_t ValidN;//储存指纹个数
uint8_t MainFlag;//主函数标志位


void Menu_RunMainMenu(void)
{
	static struct Option_Class Menu_StartOptionList[] = {
		{"<<<",Menu_Main},
		{"Unlock", Menu_RunUnlockMenu},	   // 开锁
		{"Setting", Menu_RunSettingMenu},  // 设置
		{".."}};

	Menu_RunMenu(Menu_StartOptionList);
}

void Menu_RunUnlockMenu(void)
{
	static struct Option_Class Menu_ToolsOptionList[] = {
		{"<<<"},
		{"密码解锁",Menu_PassCode},					   
		{"指纹解锁",Menu_FingerPrintMenu},				   
		{"IDCard", }, 
		{".."}};

	Menu_RunMenu(Menu_ToolsOptionList);
}

void Menu_FingerPrintMenu(void)
{
	static struct Option_Class Menu_ToolsOptionList[] = {
		{"<<<"},
		{"录入指纹",Add_FR},					   
		{"指纹开锁",press_FR},				   
		{"删除指纹",Del_FR}, 
		{".."}};

	Menu_RunMenu(Menu_ToolsOptionList);
}

void Menu_RunSettingMenu(void)
{
	static struct Option_Class Menu_ToolsOptionList[] = {
		{"<<<"},
		{"设备信息",Menu_Information},					   // 6-1 定时器定时中断

		{".."}};

	Menu_RunMenu(Menu_ToolsOptionList);
}

//主菜单
void Menu_Main(void)
{
	
	OLED_Clear();
	MainFlag = 1;
	Menu_PrintfOptionStr(32,0,64,16,OLED_8X16,"智能门锁");
	Menu_PrintfOptionStr(0,16,112,16,OLED_8X16,"  XXXX年XX月XX");
	OLED_ShowString(0, 32, "    XX:XX:XX",OLED_8X16);
	while(1)
	{
		
		MyRTC_ReadTime();
		
		OLED_ShowNum(16, 16, MyRTC_Time[0], 4,OLED_8X16);	
		OLED_ShowNum(64, 16, MyRTC_Time[1], 2,OLED_8X16);		
		OLED_ShowNum(96, 16, MyRTC_Time[2], 2,OLED_8X16);		
		OLED_ShowNum(32, 32, MyRTC_Time[3], 2,OLED_8X16);	
		OLED_ShowNum(56, 32, MyRTC_Time[4], 2,OLED_8X16);		
		OLED_ShowNum(80, 32, MyRTC_Time[5], 2,OLED_8X16);	
		
		OLED_Update();
		if(1 == KeyNum_Get())
		{
			Menu_RunMainMenu();
		}
		if(Menu_BackEvent())
			return;
		vTaskDelay(300);
	}
	
}


//设备显示功能函数

void Menu_Information(void)
{
	OLED_Clear();
	while(1)
	{

		OLED_ShowString(0, 0, "F_SIZE:",OLED_8X16);	//显示静态字符串
		OLED_ShowHexNum(56, 0, *((__IO uint16_t *)(0x1FFFF7E0)), 4,OLED_8X16);		//使用指针读取指定地址下的闪存容量寄存器
		
		OLED_ShowString(0, 16, "U_ID:",OLED_8X16);		//显示静态字符串
		OLED_ShowHexNum(40,16, *((__IO uint16_t *)(0x1FFFF7E8)), 4,OLED_8X16);		//使用指针读取指定地址下的产品唯一身份标识寄存器
		OLED_ShowHexNum(72, 16, *((__IO uint16_t *)(0x1FFFF7E8 + 0x02)), 4,OLED_8X16);
		OLED_ShowHexNum(0, 32, *((__IO uint32_t *)(0x1FFFF7E8 + 0x04)), 8,OLED_8X16);
		
		OLED_Update();
		
		if(Menu_BackEvent())
			return;
	}
}


//密码开门功能函数
void Menu_PassCode(void)
{		
	OLED_Clear();

	  while(1)
    {
		if(0 == PassFlag)OLED_Clear();
		Menu_PrintfOptionStr(0,0,128,16,OLED_8X16,"请输入密码:%04d",PassWord);
		OLED_Update();
		if(PassWord < 1000)
		{
			if(1 == KeyNum_Get())
				PassWord = PassWord * 10;
			else
				PassWord += Encoder_Get();
		}
		else//PassWord已经大于1000了，是四位数，现在是调整第四位数。如果不判断第四位智能为0。
		{
			if(1 == KeyNum_Get())
				TIM_Flag = 1;    //Enter键标志位
			else
				PassWord += Encoder_Get();
		}
		
		if(1 == PassFlag && 1 == TIM_Flag) //修改密码模式
		{

			PassFlag = 0;
			TIM_Flag = 0;
			Store_Clear();
			Store_Data[1] = PassWord;
			Store_Save();
			PassWord = 0;
		}
		else if( 333 == PassWord) //当PassWord为333时，进入管理员模式修改密码模式
		{
			Menu_PrintfOptionStr(0,16,128,16,OLED_8X16,"原来的密码:%04d",Store_Data[1]);
			Menu_PrintfOptionStr(0,32,48,16,OLED_8X16,"请修改");
			OLED_Update();
			PassWord = 0;   //密码清零
			PassFlag = 1;	//修改密码标志位
		}
		else if(0 == PassFlag && 1 == TIM_Flag)//输入密码模式
		{
			if(PassWord == Store_Data[1])
			{
				PassWord = 0;	
				TIM_Flag = 0;  //标志位
				Angle = 0;
				Beep_ON();//开锁提示
				Servo_SetAngle(Angle);
			}
			else
			{
				PassWord = 0;
				TIM_Flag = 0;
			}
		}
		
		if(Menu_BackEvent())
			return;
    }
}

//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
	Menu_PrintfOptionStr(0,0,strlen(EnsureMessage(ensure)),16,OLED_8X16,"%s\n",(u8*)EnsureMessage(ensure));
}

int GET_NUM(void)
{
	MenuNum += Encoder_Get();
	return MenuNum;
}

//录入指纹
void Add_FR(void)
{
	OLED_Clear();

	Menu_PrintfOptionStr(0,0,128,16,OLED_8X16,"与AS608模块握手.");	
	OLED_Update();
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		TIM4_Delayms(400);
		Menu_PrintfOptionStr(0,16,128,16,OLED_8X16,"未检测到模块!");	
		OLED_Update();
		TIM4_Delayms(800);
		Menu_PrintfOptionStr(0,32,128,16,OLED_8X16,"尝试连接模块.");
		OLED_Update();		
	}
	Menu_PrintfOptionStr(0,16,64,16,OLED_8X16,"通讯成功");
	OLED_Update();

	uint8_t ensure1=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure1!=0x00)
		ShowErrMessage(ensure1);//显示确认码错误信息	
		ensure1=PS_ReadSysPara(&AS608Para);  //读参数 
	if(ensure1==0x00)
	{
		Menu_PrintfOptionStr(0,32,80,16,OLED_8X16,"库容量:%d",AS608Para.PS_max - ValidN);
		
		Menu_PrintfOptionStr(0,48,80,16,OLED_8X16,"对比等级:%d",AS608Para.PS_level);
		OLED_Update();
	}
	else
		ShowErrMessage(ensure1);	
	u8 i,ensure ,processnum = 0;
	u16 ID;
	
	while(1)
	{
		if(1 == KeyNum_Get())
		{
			while(1)
			{
				switch (processnum)
				{
					case 0:
						i++;
						OLED_Clear();
						Menu_PrintfOptionStr(0,0,64,16,OLED_8X16,"请按指纹");
						OLED_Update();
						ensure=PS_GetImage();
						if(ensure==0x00) 
						{
							Beep_ON();
							ensure=PS_GenChar(CharBuffer1);//生成特征
							Beep_OFF();
							if(ensure==0x00)
							{
								Menu_PrintfOptionStr(0,0,64,16,OLED_8X16,"指纹正常");
								OLED_Update();
								i=0;
								processnum=1;//跳到第二步						
							}else ShowErrMessage(ensure);				
						}else ShowErrMessage(ensure);						
						break;
					
					case 1:
						i++;
						Menu_PrintfOptionStr(0,0,128,16,OLED_8X16,"请按再按一次指纹");
						OLED_Update();
						ensure=PS_GetImage();
						if(ensure==0x00) 
						{
							Beep_ON();
							ensure=PS_GenChar(CharBuffer2);//生成特征
							Beep_OFF();
							if(ensure==0x00)
							{
								OLED_Clear();
								Menu_PrintfOptionStr(0,0,64,16,OLED_8X16,"指纹正常");
								OLED_Update();
								i=0;
								processnum=2;//跳到第三步
							}else ShowErrMessage(ensure);	
						}else ShowErrMessage(ensure);		
						break;

					case 2:
						Menu_PrintfOptionStr(0,0,96,16,OLED_8X16,"对比两次指纹");
						OLED_Update();
						uint16_t score = 0;
						ensure=PS_Match(&score);
						if(ensure==0x00) 
						{
							OLED_Clear();
							Menu_PrintfOptionStr(0,0,64,16,OLED_8X16,"对比成功");
							OLED_Update();
							processnum=3;//跳到第四步
						}
						else 
						{
							Menu_PrintfOptionStr(0,0,128,16,OLED_8X16,"对比失败，请重新");
							OLED_Update();
							ShowErrMessage(ensure);
							i=0;
							processnum=0;//跳回第一步		
						}
						break;

					case 3:
						OLED_Clear();
						Menu_PrintfOptionStr(0,0,96,16,OLED_8X16,"对比两次指纹");
						OLED_Update();
						ensure=PS_RegModel();
						if(ensure==0x00) 
						{
							Menu_PrintfOptionStr(0,0,128,16,OLED_8X16,"生成指纹模板成功");
							processnum=4;//跳到第五步
						}else {processnum=0;ShowErrMessage(ensure);}
						break;
						
					case 4:	
						Menu_PrintfOptionStr(0,0,96,16,OLED_8X16,"请输入储存ID");
						Menu_PrintfOptionStr(0,16,96,16,OLED_8X16,"0=< ID <=299");
						OLED_Update();
						do
						{
							ID=GET_NUM();
							Menu_PrintfOptionStr(0,32,48,16,OLED_8X16,"ID:%d\n",GET_NUM());
							OLED_Update();
							if(KeyNum_Get())
							{
								MenuNum = 0;
								break;
							}
						}
						while(ID < AS608Para.PS_max );//输入ID必须小于模块容量最大的数值
						ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
						if(ensure==0x00) 
						{		
							OLED_Clear();
							Menu_PrintfOptionStr(0,0,96,16,OLED_8X16,"录入指纹成功");
							OLED_Update();					
							PS_ValidTempleteNum(&ValidN);//读库指纹个数
							Menu_PrintfOptionStr(0,16,128,16,OLED_8X16,"剩余指纹数:%d",(AS608Para.PS_max - ValidN));
							OLED_Update();
							TIM4_Delayms(1500);
						}else {processnum=0;ShowErrMessage(ensure);}
							processnum = 10;						
						break;
					default:
							return;	
								
				}

	
			}
		}		
	}
}
////刷指纹
void press_FR(void)
{
	OLED_Clear();

	Menu_PrintfOptionStr(0,0,128,16,OLED_8X16,"与AS608模块握手.");	
	OLED_Update();
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		TIM4_Delayms(400);
		Menu_PrintfOptionStr(0,16,128,16,OLED_8X16,"未检测到模块!");	
		OLED_Update();
		TIM4_Delayms(800);
		Menu_PrintfOptionStr(0,32,128,16,OLED_8X16,"尝试连接模块.");
		OLED_Update();		
	}
	Menu_PrintfOptionStr(0,16,64,16,OLED_8X16,"通讯成功");
	OLED_Update();

	uint8_t ensure1=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure1!=0x00)
		ShowErrMessage(ensure1);//显示确认码错误信息	
		ensure1=PS_ReadSysPara(&AS608Para);  //读参数 
	if(ensure1==0x00)
	{
		Menu_PrintfOptionStr(0,32,80,16,OLED_8X16,"库容量:%d",AS608Para.PS_max - ValidN);
		
		Menu_PrintfOptionStr(0,48,80,16,OLED_8X16,"对比等级:%d",AS608Para.PS_level);
		OLED_Update();
	}
	else
		ShowErrMessage(ensure1);
	
	SearchResult seach;
	u8 ensure;
	while(1)
	{
		OLED_Clear();
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 1)
		{
			ensure=PS_GetImage();
			if(ensure==0x00)//获取图像成功 
			{	
				Beep_ON();//打开蜂鸣器	
				ensure=PS_GenChar(CharBuffer1);
				if(ensure==0x00) //生成特征成功
				{	
					Angle = 0;	//改变舵机角度
					Servo_SetAngle(Angle);//操作舵机
					Beep_OFF();//关闭蜂鸣器	
					ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
					if(ensure==0x00)//搜索成功
					{	
						OLED_Clear();	
						Menu_PrintfOptionStr(0,0,80,16,OLED_8X16,"刷指纹成功");
						OLED_Update();
						Menu_PrintfOptionStr(0,16,64,16,OLED_8X16,"ID:%d",seach.pageID);
						Menu_PrintfOptionStr(0,32,104,16,OLED_8X16,"匹配得分:%d",seach.mathscore);
						OLED_Update();
						TIM4_Delayms(1000);
						return;
					}
					else 
						ShowErrMessage(ensure);					
			  }
				else
					ShowErrMessage(ensure);
			 Beep_OFF();//关闭蜂鸣器
			}
		}
	}
}

//删除指纹
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	OLED_Clear();
	Menu_PrintfOptionStr(0,0,64,16,OLED_8X16,"删除指纹");
	Menu_PrintfOptionStr(0,16,96,16,OLED_8X16,"请输入指纹ID");
	OLED_Update();
	TIM4_Delayms(50);
	do
	{
		num = GET_NUM();
		Menu_PrintfOptionStr(0,32,48,16,OLED_8X16,"ID:%d\n",GET_NUM());
		OLED_Update();
		if(KeyNum_Get())
		{
			MenuNum = 0;
			break;
		}
	}
	while(num <= 0xFFFF );//输入ID必须小于模块容量最大的数值
	
	if(num==0xFFFF)
		return;
	else if(num==20)
		ensure=PS_Empty();//清空指纹库
	else 
		ensure=PS_DeletChar(num,1);//删除单个指纹
	if(ensure==0)
	{
		OLED_Clear();
		Menu_PrintfOptionStr(0,0,64,16,OLED_8X16,"删除成功");	
		OLED_Update();		
	}
  else
	ShowErrMessage(ensure);	
	TIM4_Delayms(1200);
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	Menu_PrintfOptionStr(0,16,96,16,OLED_8X16,"指纹个数:%d",ValidN);	
	OLED_Update();
	while(!Menu_BackEvent());
	return;
}



