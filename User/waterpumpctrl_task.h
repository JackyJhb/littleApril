#ifndef WATERPUMPCTRL_TASK_H
#define WATERPUMPCTRL_TASK_H
#include "includes.h"
#include "bsp_gpio.h"

#define WATERPUMP_TASK_PRIO		15
#define WATERPUMP_STK_SIZE 		128

extern OS_TCB WATERPUMPTaskTCB;	
extern CPU_STK WATERPUMP_TASK_STK[WATERPUMP_STK_SIZE];
extern void waterpump_task(void *p_arg);
#endif