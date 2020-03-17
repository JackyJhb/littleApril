#ifndef WIFI_TASK_H
#define WIFI_TASK_H
#include "includes.h"

#define WIFI_TASK_PRIO		17
#define WIFI_STK_SIZE 		512

extern OS_TCB WIFITaskTCB;	
extern CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
extern void WIFI_task(void *p_arg);
#endif
