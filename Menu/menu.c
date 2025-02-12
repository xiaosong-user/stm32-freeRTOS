#include "Menu.h"
#include "Encoder.h"
#include "key.h"
#include "Timer.h"


/*菜单全局属性*/
struct MenuProperty Menu_Global = {
	.Cursor_Actual_X = 0,  // 当前光标位置X
	.Cursor_Actual_Y = 63, // 当前光标位置Y
	.Cursor_Actual_W = 0,  // 当前光标尺寸宽
	.Cursor_Actual_H = 0,  // 当前光标尺寸高

	.CursorStyle = reverse,	 // 光标风格;
	.Cursor_ActSpeed = 0.15, // 光标动画速度系数;
	.Slide_ActSpeed = 2,	 // 滚动动画速度系数;

	.Font_Width = 8,	// 字体宽度 8 或 6
	.Font_Height = 16,	// 字体高度
	.Line_Height = 16,	// 行高
	.Layout_Margin = 0, // 页边距

	.Window_X = 0,	 // 窗口位置X
	.Window_Y = 0,	 // 窗口位置Y
	.Window_W = 128, // 窗口宽度
	.Window_H = 64,	 // 窗口高度
};

int8_t Menu_RollEvent(void) // 菜单滚动
{
	return Encoder_Get(); // 旋钮编码器PA8,PA9;
}
int8_t Menu_EnterEvent(void) // 菜单确认
{
	return KeyNum_Get(); // 确认键接到PB14;
}
int8_t Menu_BackEvent(void) // 菜单返回
{
	return Key_Back_Get(); // 
}

/**
 * 函    数：菜单运行
 * 参    数：选项列表
 * 返 回 值：
 * 说    明：把选项列表显示出来,并根据按键事件执行相应操作
 */
void Menu_RunMenu(struct Option_Class *Option_List)
{

	int8_t Roll_Event = 0;		// 记录菜单滚动事件
	int8_t Show_i = 0;			// 显示起始下标
	int8_t Show_i_previous = 4; // 显示起始下标的前一个状态(用于比较)
	int8_t Show_offset;			// 显示Y轴的偏移
	int8_t Cat_i = 1;			// 选中下标默认为1,(因为Option_List[0]为"<<<")
	int8_t Cur_i = 0;			// 光标下标默认为0

	int8_t Option_MaxNum = 0;
	for (Option_MaxNum = 0; Option_List[Option_MaxNum].String[0] != '.'; Option_MaxNum++) // 计算选项列表长度
	{
		// Option_List[Option_MaxNum].StrLen = Menu_GetOptionStrLen(Option_List[Option_MaxNum].String); // 顺手计算选项名字长度
	}
	Option_MaxNum--;

	/*光标下标限制等于窗口高度减去上下页边距再除以行高*/
	int8_t Cur_i_Ceiling = (Menu_Global.Window_H - Menu_Global.Layout_Margin * 2) / Menu_Global.Line_Height; // 计算光标限制位置;

	/**********************************************************/

	while (1)
	{

		if (Menu_EnterEvent())
		{
			/*如果功能不为空则执行功能,否则返回*/
			if (Option_List[Cat_i].func)
			{
				Option_List[Cat_i].func();
			}
			else
			{
				return;
			}
		}
		if (Menu_BackEvent())
		{
			return;
		}

		/*根据按键事件更改选中下标和光标下标*/
		Roll_Event = Menu_RollEvent();
		if (Roll_Event)
		{
			/*更新下标*/
			Cur_i += Roll_Event;
			Cat_i += Roll_Event;
			/*限制选中下标*/
			if (Cat_i > Option_MaxNum)
			{
				Cat_i = Option_MaxNum;
			}
			if (Cat_i < 0)
			{
				Cat_i = 0;
			}
			/*限制光标下标*/
			if (Cur_i >= Cur_i_Ceiling)
			{
				Cur_i = Cur_i_Ceiling - 1;
			}
			if (Cur_i > Option_MaxNum)
			{
				Cur_i = Option_MaxNum;
			}
			if (Cur_i < 0) // 踩坑记录: (Cur_i >= Cur_i_Ceiling) Cur_i_Ceiling 有可能是负数, 如果放在(Cur_i < 0)后面判断, 则 Cur_i 会变成负数, 造成程序卡死; 结论: 如果进行位置限制判断, 变量判断应该放在前,常量判断应该放在后;
			{
				Cur_i = 0;
			}
		}

		/**********************************************************/

		OLED_Clear();

		/*计算显示起始下标*/
		Show_i = Cat_i - Cur_i;

		if (1) // 增加显示偏移量实现平滑移动
		{
			if (Show_i - Show_i_previous) // 如果下标有偏移
			{
				Show_offset = (Show_i - Show_i_previous) * Menu_Global.Line_Height; // 计算显示偏移量
				Show_i_previous = Show_i;
			}
			if (Show_offset)
			{
				Show_offset /= Menu_Global.Slide_ActSpeed; // 显示偏移量逐渐归零
			}
		}

		for (int8_t i = -1; i < Cur_i_Ceiling + 1; i++) // 遍历显示选项(遍历 i 从 -1 开始到 Cur_i_Ceiling + 1 结束, 是为了菜单滚动的时候首行和尾行不会有空白);
		{
			if (Show_i + i < 0)
			{
				continue;
			}
			if (Show_i + i > Option_MaxNum)
			{
				break;
			}

			/*菜单格式化打印函数会返回打印的字符串长度*/
			Option_List[Show_i + i].StrLen =
				/*使用格式化字符串打印, 支持添加一个(float)变量*/
				Menu_PrintfOptionStr(
					/*显示从窗口X起点, 加上页边距*/
					2 + Menu_Global.Window_X + Menu_Global.Layout_Margin,
					/*显示从窗口Y起点, 加上页边距, 加上行偏移, 加上显示偏移, 加上垂直居中*/
					Menu_Global.Window_Y + (Menu_Global.Layout_Margin) + (i * Menu_Global.Line_Height) + (Show_offset) + ((Menu_Global.Line_Height - Menu_Global.Font_Height) / 2),
					/*显示宽度范围减去双倍(左右)页边距*/
					Menu_Global.Window_W - Menu_Global.Layout_Margin * 2,
					/*显示高度就是行高(或字高)*/
					Menu_Global.Line_Height,
					/*显示字符的宽度*/
					Menu_Global.Font_Width,
					/*要显示的字符串*/
					Option_List[Show_i + i].String,
					/*可选变量*/
					*Option_List[Show_i + i].Variable);
		}

		// 调用显示光标函数
		Menu_ShowCursor(Menu_Global.Window_X + Menu_Global.Layout_Margin,									  // 光标左起点加上页边距
						Menu_Global.Window_Y + Menu_Global.Layout_Margin + (Cur_i * Menu_Global.Line_Height), // 光标上起点加上光标行偏移
						4 + Option_List[Cat_i].StrLen * Menu_Global.Font_Width + Menu_Global.Layout_Margin,	  // 光标宽度等于字符串长度乘以字符宽度加上页边距(跟随字符串)
																											  // Menu_Global.Window_W - Menu_Global.Layout_Margin * 2,// 光标宽度等于窗口宽度减去左右页边距(等长)(二选一注释)
						Menu_Global.Line_Height,															  // 光标高度就是行高
						Menu_Global.CursorStyle,															  // 光标状态
						Menu_Global.Cursor_ActSpeed);														  // 光标速度

		//OLED_DrawRectangle(Menu_Global.Window_X, Menu_Global.Window_Y, Menu_Global.Window_W, Menu_Global.Window_H, 0); // 显示窗口边框,根据个人喜好选择

		/**********************************************************/
		/*调试信息*/

//		OLED_ShowSignedNum(110, 48, Cur_i, 2, OLED_6X8);
//		OLED_ShowSignedNum(110, 56, Cat_i, 2, OLED_6X8);

//		 int delay = 1000000; while(delay--);
		/**********************************************************/
		OLED_Update();
	}
}

/**
 * 函    数：计算选项字符串长度;
 * 参    数：String 计算长度的字符串
 * 返 回 值：字符串长度
 * 说    明：英文一个长度,中文两个长度;
 */
uint8_t Menu_GetOptionStrLen(char *String)
{
	uint8_t i = 0, len = 0;
	while (String[i] != '\0') // 遍历字符串的每个字符
	{
		if (String[i] > '~') // 如果不属于英文字符长度加2
		{
			len += 2;
			i += 3;
		}
		else // 属于英文字符长度加1
		{
			len += 1;
			i += 1;
		}
	}
	return len;
}

/**
 * 函    数：使用printf函数打印格式化字符串(增加范围打印)
 * 参    数：X 指定格式化字符串左上角的横坐标，范围：0~127
 * 参    数：Y 指定格式化字符串左上角的纵坐标，范围：0~63
 * 参    数：Width  范围：0~127
 * 参    数：Height 范围：0~63
 * 参    数：FontSize 指定字体大小
 *           范围：OLED_8X16		宽8像素，高16像素
 *                 OLED_6X8		宽6像素，高8像素
 * 参    数：format 指定要显示的格式化字符串，范围：ASCII码可见字符组成的字符串
 * 参    数：... 格式化字符串参数列表
 * 返 回 值：*****************打印字符串的长度******************
 * 说    明：调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
 */
uint8_t Menu_PrintfOptionStr(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, uint8_t FontSize, char *format, ...)
{
	char String[30];												   // 定义字符数组
	va_list arg;													   // 定义可变参数列表数据类型的变量arg
	va_start(arg, format);											   // 从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);									   // 使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);													   // 结束变量arg
	return OLED_ShowStringArea(X, Y, Width, Height, String, FontSize); // OLED显示字符数组（字符串）并返回字符串
}

/**
 * 函    数：平滑偏移
 * 参    数：Actual_Value 当前数值
 * 参    数：Target_Value 目标数值
 * 参    数：Act_Speed 偏移速度
 * 返 回 值: 当前数值偏移后的值
 * 说    明：按照格式写入参数, 每次循环调用, 当前值都向目标值靠近一定距离
 */
float Menu_CurveMigration(float Actual_Value, float Target_Value, float Act_Speed)
{
	if ((Target_Value - Actual_Value) > 1)
	{
		Actual_Value += (Target_Value - Actual_Value) * Act_Speed + 1;
	}
	else if ((Target_Value - Actual_Value) < -1)
	{
		Actual_Value += (Target_Value - Actual_Value) * Act_Speed - 1;
	}
	else
	{
		Actual_Value = Target_Value;
	}

	return Actual_Value;
}

/**
 * 函    数：菜单平滑显示光标
 * 参    数：Target_Cur_X 光标目标X位置
 * 参    数：Target_Cur_Y 光标目标Y位置
 * 参    数：Target_Cur_W 光标目标宽度
 * 参    数：Target_Cur_H 光标目标高度
 * 参    数：Cur_Act_Speed 光标移动速度范围: 0 < Cur_Act_Speed <=1
 * 返 回 值：无
 * 说    明：输入光标目标位置和尺寸, 光标会在多次打印中逐渐接近目标位置和尺寸;
 */
void Menu_ShowCursor(float Target_Cur_X, float Target_Cur_Y, float Target_Cur_W, float Target_Cur_H, enum CursorStyle CurStyle, float Cur_Act_Speed)
{
	Menu_Global.Cursor_Actual_X = Menu_CurveMigration(Menu_Global.Cursor_Actual_X, Target_Cur_X, Cur_Act_Speed);
	Menu_Global.Cursor_Actual_Y = Menu_CurveMigration(Menu_Global.Cursor_Actual_Y, Target_Cur_Y, Cur_Act_Speed);
	Menu_Global.Cursor_Actual_W = Menu_CurveMigration(Menu_Global.Cursor_Actual_W, Target_Cur_W, Cur_Act_Speed);
	Menu_Global.Cursor_Actual_H = Menu_CurveMigration(Menu_Global.Cursor_Actual_H, Target_Cur_H, Cur_Act_Speed);

	if (CurStyle == reverse)
	{
		OLED_ReverseArea(Menu_Global.Cursor_Actual_X, Menu_Global.Cursor_Actual_Y, Menu_Global.Cursor_Actual_W, Menu_Global.Cursor_Actual_H); // 反相光标
	}
	else if (CurStyle == mouse)
	{
		OLED_ShowString(Menu_Global.Cursor_Actual_X + Menu_Global.Cursor_Actual_W, Menu_Global.Cursor_Actual_Y + (Menu_Global.Cursor_Actual_H - 6) / 2, "<-", OLED_6X8); // 尾巴光标
	}
	else if (CurStyle == frame)
	{
		OLED_DrawRectangle(Menu_Global.Cursor_Actual_X, Menu_Global.Cursor_Actual_Y, Menu_Global.Cursor_Actual_W, Menu_Global.Cursor_Actual_H, 0); // 矩形光标
	}
}



