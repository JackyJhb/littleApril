#ifndef SIDE_WINDOW_CTRL_TASK_H
#define SIDE_WINDOW_CTRL_TASK_H

#include "includes.h"
#include "stm32f4xx.h"
#include "priolevel.h"

#define SIDEWINDOWCTRL_STK_SIZE 		512

#define SMALL_LEFT_CLOSE  0x01
#define SMALL_LEFT_OPEN 0x02
#define SMALL_LEFT_STOP 0x03
#define SMALL_RIGHT_CLOSE 0x04
#define SMALL_RIGHT_OPEN  0x05
#define SMALL_RIGHT_STOP 0x06

#define BIG_LEFT_CLOSE  0x07
#define BIG_LEFT_OPEN 0x08
#define BIG_LEFT_STOP 0x09
#define BIG_RIGHT_CLOSE 0x0A
#define BIG_RIGHT_OPEN  0x0B
#define BIG_RIGHT_STOP 0x0C

extern OS_TCB SideWindowCtrlTaskTCB;
extern CPU_STK SIDEWINDOWCTRL_TASK_STK[SIDEWINDOWCTRL_STK_SIZE];
extern void sidewindowctrl_task(void *p_arg);
#endif
