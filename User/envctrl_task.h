
#ifndef  ENVCTRL_TASK_H
#define ENVCTRL_TASK_H
#include "includes.h"
#include "priolevel.h"

#define INVIAL                      168
#define EnvParameter_STK_SIZE 		512
#define FILTER_TIMES          5
extern OS_TCB EnvParameterTaskTCB;
extern CPU_STK EnvParameter_TASK_STK[EnvParameter_STK_SIZE];
extern OS_MEM mymem;
//extern uint8_t ucArray [ 20 ] [ 40 ];
extern void EnvParameter_task(void *p_arg);
#endif
