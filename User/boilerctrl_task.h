#ifndef BOILERCTRL_TASK_H
#define BOILERCTRL_TASK_H

#include "includes.h"
#include "priolevel.h"

#define BOILERCTRL_STK_SIZE 		512
#define STOPPED                     0
#define RUNNING                     1

extern OS_TCB BoilerCtrlTaskTCB;
extern CPU_STK BOILERCTRL_TASK_STK[BOILERCTRL_STK_SIZE];
extern void boilerctrl_task(void *p_arg);

#endif
