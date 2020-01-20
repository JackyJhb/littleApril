/*
*********************************************************************************************************
*	                                  
*	模块名称 : 按键驱动模块    
*	文件名称 : bsp_button.h
*	版    本 : V2.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

/* 按键滤波时间50ms, 单位10ms
 只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
*/
#define BUTTON_FILTER_TIME 	5
#define BUTTON_LONG_TIME 	100		/* 持续1秒，认为长按事件 */

/*
	每个按键对应1个全局的结构体变量。
	其成员变量是实现滤波和多种按键状态所必须的
*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	unsigned char (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	unsigned char Count;			/* 滤波器计数器 */
	unsigned char FilterTime;		/* 滤波时间(最大255,表示2550ms) */
	unsigned short LongCount;		/* 长按计数器 */
	unsigned short LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	unsigned char  State;			/* 按键当前状态（按下还是弹起） */
	unsigned char KeyCodeUp;		/* 按键弹起的键值代码, 0表示不检测按键弹起 */
	unsigned char KeyCodeDown;	/* 按键按下的键值代码, 0表示不检测按键按下 */
	unsigned char KeyCodeLong;	/* 按键长按的键值代码, 0表示不检测长按 */
	unsigned char RepeatSpeed;	/* 连续按键周期 */
	unsigned char RepeatCount;	/* 连续按键计数器 */
}BUTTON_T;

/* 定义键值代码
	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2)	编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_DOWN_OK,			/* OK键按下 */
	KEY_UP_OK,			    /* OK键弹起 */
	KEY_LONG_OK,			/* OK键长按 */

	KEY_DOWN_CANCEL,		/* CANCEL键按下 */
	KEY_UP_CANCEL,			/* CANCEL键弹起 */
	KEY_LONG_CANCEL,		/* CANCEL键长按 */

	KEY_DOWN_UP,			/* Up键按下 */
	KEY_UP_UP,			    /* Up键弹起 */
	KEY_LONG_UP,			/* Up键长按 */

	KEY_DOWN_DOWN,			/* Down键按下 */
	KEY_UP_DOWN,			/* Down键弹起 */
	KEY_LONG_DOWN,			/* Down键长按 */

	KEY_DOWN_LEFT,			/* LEFT键按下 */
	KEY_UP_LEFT,			/* LEFT键弹起 */
	KEY_LONG_LEFT,			/* LEFT键长按 */
	
	KEY_DOWN_RIGHT,	   
    KEY_UP_RIGHT,
    KEY_LONG_RIGHT
}KEY_ENUM;

/* 按键ID */
enum
{
	KID_OK = 0,
	KID_CANCEL,
	KID_LEFT,
    KID_RIGHT,    
	KID_UP,
	KID_DOWN
};

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	20
typedef struct
{
	unsigned char Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	unsigned char Read;					    /* 缓冲区读指针 */
	unsigned char Write;					/* 缓冲区写指针 */
}KEY_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitButton(void);
void bsp_PutKey(unsigned char _KeyCode);
unsigned char bsp_GetKey(void);
void bsp_KeyPro(void);
unsigned char bsp_KeyState(unsigned char _ucKeyID);
unsigned char IsKeyDownOK(void);
unsigned char IsKeyDownCancel(void);
unsigned char IsKeyDownLeft(void);
unsigned char IsKeyDownRight(void);
unsigned char IsKeyDownUp(void);
unsigned char IsKeyDownDown(void);
#endif


