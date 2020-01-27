#ifndef TS_TASK_H
#define TS_TASK_H

#include "includes.h"
#include "usart.h"

#define TS_TASK_PRIO		13
#define TS_STK_SIZE 		512
extern OS_TCB TSTaskTCB;
extern CPU_STK TS_TASK_STK[TS_STK_SIZE];
extern void ts_task(void *p_arg);

#endif
