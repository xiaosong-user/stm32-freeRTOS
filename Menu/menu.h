#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "OLED.h"
#include "Encoder.h"


extern uint8_t MainFlag;

struct Option_Class
{
	char *String;		// 选项名字
	void (*func)(void); // 函数指针
	float *Variable;	// 可选变量
	uint8_t StrLen;		// 由于中文占三个字节,用strlen计算名字长度不再准确,故需额外储存名字长度
};

enum CursorStyle//光标类型
{
	reverse,/*反向*/
	mouse,/*鼠标*/
	frame,/*框架*/
};

struct MenuProperty//菜单属性
{
	float Cursor_Actual_X;		  // 当前光标位置X
	float Cursor_Actual_Y;		  // 当前光标位置Y
	float Cursor_Actual_W;		  // 当前光标尺寸宽
	float Cursor_Actual_H;		  // 当前光标尺寸高
	enum CursorStyle CursorStyle; // 光标风格;
	float Cursor_ActSpeed;		  // 光标动画速度系数;
	float Slide_ActSpeed;		  // 滚动动画速度系数;
	int8_t Font_Width;			  // 字体宽度
	int8_t Font_Height;			  // 字体宽度
	int8_t Line_Height;			  // 行高
	int8_t Layout_Margin;		  // 页边距

	int8_t Window_X;				// 窗口位置X
	int8_t Window_Y;                // 窗口位置Y
	uint8_t Window_W;               // 窗口宽度
	uint8_t Window_H;               // 窗口高度
};

//菜单函数	
void Menu_RunMainMenu(void);//主菜单
void Menu_RunUnlockMenu(void);//解锁菜单
void Menu_RunSettingMenu(void);//设置菜单
void Menu_Information(void);//显示设备信息函数
void Menu_PassCode(void);//输入密码函数
void Menu_Main(void);//主函数
void Menu_FingerPrintMenu(void);//指纹菜单
void ShowErrMessage(u8 ensure);//错误码显示函数
void Add_FR(void);//录入指纹函数
void press_FR(void);//刷指纹函数
void Del_FR(void);//删除指纹函数


int8_t Menu_RollEvent(void); // 菜单滚动
int8_t Menu_EnterEvent(void); // 菜单确认
int8_t Menu_BackEvent(void); // 菜单返回


void Menu_RunMenu(struct Option_Class *Option_List);
uint8_t Menu_GetOptionStrLen(char *String);
uint8_t Menu_PrintfOptionStr(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, uint8_t FontSize, char *format, ...);
float Menu_CurveMigration(float Actual_Value, float Target_Value, float Act_Speed);
void Menu_ShowCursor(float Target_Cur_X, float Target_Cur_Y, float Target_Cur_W, float Target_Cur_H, enum CursorStyle CurStyle, float Cur_Act_Speed);


#endif








// typedef struct
//{
//	uint8_t current;					//当前页面的索引号
//	
//	uint8_t up;               			//向上翻索引号
//	
//	uint8_t down;             			//向下翻索引号
//	
//	uint8_t enter;             			//确认索引号
//	
//	void (*current_operation)(void);  	//函数
//	
//} key_table;

//void Menu_switch(void);

//extern uint8_t func_index;  

