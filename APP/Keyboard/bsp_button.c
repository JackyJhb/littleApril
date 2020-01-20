/*
*********************************************************************************************************
*	                                  
*	模块名称 : 按键驱动模块
*	文件名称 : bsp_button.c
*	版    本 : V2.0
*	说    明 : 实现按键的检测，具有软件滤波机制，可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*				(5) 组合键
*
*********************************************************************************************************
*/

#include "stm32f4xx.h"
#include <stdio.h>

#include "bsp_button.h"
#include "../PhotoElectricIsolation/bsp_gpio.h"

static BUTTON_T s_BtnOK;		                                        /* 确定 键 */
static BUTTON_T s_BtnCancel;                                       	    /* 取消 键 */
static BUTTON_T s_BtnLeft;	                                            /* 左     键 */
static BUTTON_T s_BtnRight;	                                            /* 右     键 */
static BUTTON_T s_BtnUp;		                                        /* 上     键 */
static BUTTON_T s_BtnDown;		                                        /* 下     键 */

static KEY_FIFO_T s_Key;		                                        /* 按键FIFO变量,结构体 */

static void bsp_InitButtonVar(void);
static void bsp_InitButtonHard(void);
static void bsp_DetectButton(BUTTON_T *_pBtn);

/*
	定义函数判断按键是否按下，返回值1 表示按下，0表示未按下
*/

extern unsigned short InputGPIO[];
uint8_t IsKeyDownOK(void) 		{if (InputGPIO[7] & 0x08) return 1; return 0;}
uint8_t IsKeyDownCancel(void) 	{if (InputGPIO[7] & 0x04) return 1; return 0;}
uint8_t IsKeyDownUp(void) 		{if (InputGPIO[7] & 0x20) return 1; return 0;}
uint8_t IsKeyDownDown(void) 	{if (InputGPIO[7] & 0x10) return 1; return 0;}
uint8_t IsKeyDownLeft(void) 	{if (InputGPIO[7] & 0x02) return 1; return 0;}
uint8_t IsKeyDownRight(void) 	{if (InputGPIO[7] & 0x01) return 1; return 0;}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitButton
*	功能说明: 初始化按键
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitButton(void)
{
	bsp_InitButtonVar();		/* 初始化按键变量 */
	bsp_InitButtonHard();		/* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参：_KeyCode : 按键代码
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_Key.Buf[s_Key.Write] = _KeyCode;

	if (++s_Key.Write  >= KEY_FIFO_SIZE)
	{
		s_Key.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;
	if (s_Key.Read == s_Key.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_Key.Buf[s_Key.Read];
		if (++s_Key.Read >= KEY_FIFO_SIZE)
		{
			s_Key.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyState
*	功能说明: 读取按键的状态
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t bsp_KeyState(uint8_t _ucKeyID)
{
	uint8_t ucState = 0;
	switch (_ucKeyID)
	{
		case KID_OK:
			ucState = s_BtnOK.State;
			break;
		case KID_CANCEL:
			ucState = s_BtnCancel.State;
			break;
		case KID_UP:
			ucState = s_BtnUp.State;
			break;
		case KID_DOWN:
			ucState = s_BtnUp.State;
			break;
		case KID_LEFT:
			ucState = s_BtnLeft.State;
			break;
		case KID_RIGHT:
			ucState = s_BtnRight.State;
			break;
	}
	return ucState;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitButtonHard
*	功能说明: 初始化按键硬件
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitButtonHard(void)
{

}
	
/*
*********************************************************************************************************
*	函 数 名: bsp_InitButtonVar
*	功能说明: 初始化按键变量
*	形    参：strName : 例程名称字符串
*			  strDate : 例程发布日期
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitButtonVar(void)
{
    /* 对按键FIFO读写指针清零 */
	s_Key.Read = 0;
	s_Key.Write = 0;
	/* 初始化OK按键变量，支持按下、弹起、长按 */
	s_BtnOK.IsKeyDownFunc = IsKeyDownOK;							/* 判断按键按下的函数 */
	s_BtnOK.FilterTime = BUTTON_FILTER_TIME;						/* 按键滤波时间 */
	s_BtnOK.LongTime = BUTTON_LONG_TIME;							/* 长按时间 */
	s_BtnOK.Count = s_BtnOK.FilterTime / 2;							/* 计数器设置为滤波时间的一半 */
	s_BtnOK.State = 0;												/* 按键缺省状态，0为未按下 */
	s_BtnOK.KeyCodeDown = KEY_DOWN_OK;								/* 按键按下的键值代码 */
	s_BtnOK.KeyCodeUp   = KEY_UP_OK;								/* 按键弹起的键值代码 */
	s_BtnOK.KeyCodeLong = KEY_LONG_OK;								/* 按键被持续按下的键值代码 */
	s_BtnOK.RepeatSpeed = 0;										/* 按键连发的速度，0表示不支持连发 */
	s_BtnOK.RepeatCount = 0;										/* 连发计数器 */	
	/* 初始化Left按键变量，支持按下 */
	s_BtnLeft.IsKeyDownFunc = IsKeyDownLeft;						/* 判断按键按下的函数 */
	s_BtnLeft.FilterTime = BUTTON_FILTER_TIME;						/* 按键滤波时间 */
	s_BtnLeft.LongTime = BUTTON_LONG_TIME;							/* 长按时间 */
	s_BtnLeft.Count = s_BtnLeft.FilterTime / 2;						/* 计数器设置为滤波时间的一半 */
	s_BtnLeft.State = 0;											/* 按键缺省状态，0为未按下 */
	s_BtnLeft.KeyCodeDown = KEY_DOWN_LEFT;							/* 按键按下的键值代码 */
	s_BtnLeft.KeyCodeUp = KEY_UP_LEFT;								/* 按键弹起的键值代码，0表示不检测 */
	s_BtnLeft.KeyCodeLong = KEY_LONG_LEFT;							/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnLeft.RepeatSpeed = 0;										/* 按键连发的速度，0表示不支持连发 */
	s_BtnLeft.RepeatCount = 0;										/* 连发计数器 */
	/* 初始化Right按键变量，支持按下 */
	s_BtnRight.IsKeyDownFunc = IsKeyDownRight;						/* 判断按键按下的函数 */
	s_BtnRight.FilterTime = BUTTON_FILTER_TIME;						/* 按键滤波时间 */
	s_BtnRight.LongTime = BUTTON_LONG_TIME;							/* 长按时间, 0表示不检测 */
	s_BtnRight.Count = s_BtnRight.FilterTime / 2;					/* 计数器设置为滤波时间的一半 */
	s_BtnRight.State = 0;																	/* 按键缺省状态，0为未按下 */
	s_BtnRight.KeyCodeDown = KEY_DOWN_RIGHT;				/* 按键按下的键值代码 */
	s_BtnRight.KeyCodeUp   = KEY_UP_RIGHT;							/* 按键弹起的键值代码 */
	s_BtnRight.KeyCodeLong = KEY_LONG_RIGHT;					/* 按键被持续按下的键值代码 */
	s_BtnRight.RepeatSpeed = 2;													/* 按键连发的速度，0表示不支持连发 */
	s_BtnRight.RepeatCount = 0;														/* 连发计数器 */	
	/* 初始化UP按键变量，支持按下 */
	s_BtnUp.IsKeyDownFunc = IsKeyDownUp;								/* 判断按键按下的函数 */
	s_BtnUp.FilterTime = BUTTON_FILTER_TIME;						/* 按键滤波时间 */
	s_BtnUp.LongTime = BUTTON_LONG_TIME;							/* 长按时间 */
	s_BtnUp.Count = s_BtnUp.FilterTime / 2;									/* 计数器设置为滤波时间的一半 */
	s_BtnUp.State = 0;																		/* 按键缺省状态，0为未按下 */
	s_BtnUp.KeyCodeDown = KEY_DOWN_UP;							/* 按键按下的键值代码 */
	s_BtnUp.KeyCodeUp   = KEY_UP_UP;									/* 按键弹起的键值代码，0表示不检测 */
	s_BtnUp.KeyCodeLong = KEY_LONG_UP;								/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnUp.RepeatSpeed = 2;														/* 按键连发的速度，0表示不支持连发 */
	s_BtnUp.RepeatCount = 0;															/* 连发计数器 */
	/* 初始化DOWN按键变量，支持按下、连发（周期10ms） */
	s_BtnDown.IsKeyDownFunc = IsKeyDownDown;					/* 判断按键按下的函数 */
	s_BtnDown.FilterTime = BUTTON_FILTER_TIME;					/* 按键滤波时间 */
	s_BtnDown.LongTime = BUTTON_LONG_TIME;					/* 长按时间 */
	s_BtnDown.Count = s_BtnDown.FilterTime / 2;						/* 计数器设置为滤波时间的一半 */
	s_BtnDown.State = 0;																	/* 按键缺省状态，0为未按下 */
	s_BtnDown.KeyCodeDown = KEY_DOWN_DOWN;				/* 按键按下的键值代码 */
	s_BtnDown.KeyCodeUp = KEY_UP_DOWN;							/* 按键弹起的键值代码，0表示不检测 */
	s_BtnDown.KeyCodeLong = KEY_LONG_DOWN;					/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnDown.RepeatSpeed = 2;													/* 按键连发的速度，0表示不支持连发 */
	s_BtnDown.RepeatCount = 0;													/* 连发计数器 */
	/* 初始化DOWN按键变量，支持按下、连发（周期10ms） */
	s_BtnCancel.IsKeyDownFunc = IsKeyDownCancel;				/* 判断按键按下的函数 */
	s_BtnCancel.FilterTime = BUTTON_FILTER_TIME;				/* 按键滤波时间 */
	s_BtnCancel.LongTime = BUTTON_LONG_TIME;					/* 长按时间 */
	s_BtnCancel.Count = s_BtnCancel.FilterTime / 2;					/* 计数器设置为滤波时间的一半 */
	s_BtnCancel.State = 0;																/* 按键缺省状态，0为未按下 */
	s_BtnCancel.KeyCodeDown = KEY_DOWN_CANCEL;		/* 按键按下的键值代码 */
	s_BtnCancel.KeyCodeUp = KEY_UP_CANCEL;					/* 按键弹起的键值代码，0表示不检测 */
	s_BtnCancel.KeyCodeLong = KEY_LONG_CANCEL;			/* 按键被持续按下的键值代码，0表示不检测 */
	s_BtnCancel.RepeatSpeed = 2;													/* 按键连发的速度，0表示不支持连发 */
	s_BtnCancel.RepeatCount = 0;													/* 连发计数器 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectButton
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	形    参：按键结构变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectButton(BUTTON_T *_pBtn)
{
	/* 如果没有初始化按键函数，则报错
	if (_pBtn->IsKeyDownFunc == 0)
	{
		printf("Fault : DetectButton(), _pBtn->IsKeyDownFunc undefine");
	}
	*/

	if (_pBtn->IsKeyDownFunc())
	{
		if (_pBtn->Count < _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count < 2 * _pBtn->FilterTime)
		{
			_pBtn->Count++;
		}
		else
		{
			if (_pBtn->State == 0)
			{
				_pBtn->State = 1;
				/* 发送按钮按下的消息 */
				if (_pBtn->KeyCodeDown > 0)
				{
					/* 键值放入按键FIFO */
					bsp_PutKey(_pBtn->KeyCodeDown);
				}
			}
			if (_pBtn->LongTime > 0)
			{
				if (_pBtn->LongCount < _pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++_pBtn->LongCount == _pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
						bsp_PutKey(_pBtn->KeyCodeLong);						
					}
				}
				else
				{
					if (_pBtn->RepeatSpeed > 0)
					{
						if (++_pBtn->RepeatCount >= _pBtn->RepeatSpeed)
						{
							_pBtn->RepeatCount = 0;
							/* 常按键后，每隔10ms发送1个按键 */
							bsp_PutKey(_pBtn->KeyCodeDown);														
						}
					}
				}
			}
		}
	}
	else
	{
		if(_pBtn->Count > _pBtn->FilterTime)
		{
			_pBtn->Count = _pBtn->FilterTime;
		}
		else if(_pBtn->Count != 0)
		{
			_pBtn->Count--;
		}
		else
		{
			if (_pBtn->State == 1)
			{
				_pBtn->State = 0;
				/* 发送按钮弹起的消息 */
				if (_pBtn->KeyCodeUp > 0)
				{
					/* 键值放入按键FIFO */
					bsp_PutKey(_pBtn->KeyCodeUp);			
				}
			}
		}

		_pBtn->LongCount = 0;
		_pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyPro
*	功能说明: 检测所有按键。非阻塞状态，必须被周期性的调用。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyPro(void)
{
	bsp_DetectButton(&s_BtnOK);												/* OK 键 */
	bsp_DetectButton(&s_BtnCancel);										/* Cancel 键 */
	bsp_DetectButton(&s_BtnUp);												/* UP 键 */
	bsp_DetectButton(&s_BtnDown);											/* DOWN 键 */
	bsp_DetectButton(&s_BtnLeft);												/* Left 键 */
	bsp_DetectButton(&s_BtnRight);											/* Right 键 */
}
