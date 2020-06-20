#ifndef LIGHT_CTRL_H
#define LIGHT_CTRL_H

#include "includes.h"
#include "priolevel.h"

#define LIGHTCTRL_STK_SIZE 		512

extern OS_TCB LightCtrlTaskTCB;
extern CPU_STK LIGHTCTRL_TASK_STK[LIGHTCTRL_STK_SIZE];
extern void lightctrl_task(void *p_arg);

#endif
