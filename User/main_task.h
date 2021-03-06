#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "includes.h"

#define START_TASK_PRIO		2
#define START_STK_SIZE 		512

extern OS_TCB StartTaskTCB;
extern CPU_STK START_TASK_STK[START_STK_SIZE];
extern void start_task(void *p_arg);
#endif
