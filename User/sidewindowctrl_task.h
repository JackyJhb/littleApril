#ifndef SIDE_WINDOW_CTRL_TASK_H
#define SIDE_WINDOW_CTRL_TASK_H

#include "includes.h"
#include "stm32f4xx.h"
#include "priolevel.h"

#define SIDEWINDOWCTRL_STK_SIZE 		512

extern OS_TCB SideWindowCtrlTaskTCB;
extern CPU_STK SIDEWINDOWCTRL_TASK_STK[SIDEWINDOWCTRL_STK_SIZE];
extern void sidewindowctrl_task(void *p_arg);
#endif
