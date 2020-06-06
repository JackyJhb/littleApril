#ifndef LED_TASK_H
#define LED_TASK_H
#include "includes.h"
#include "led.h"
#include "priolevel.h"

#define LED1_STK_SIZE 		128

extern OS_TCB Led1TaskTCB;
extern CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
extern void led1_task(void *p_arg);
#endif
