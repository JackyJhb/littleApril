#ifndef USART_TASK_H
#define USART_TASK_H
#include "includes.h"

#define USART_TASK_PRIO		14
#define USART_STK_SIZE 		512

extern OS_TCB USARTTaskTCB;
extern CPU_STK USART_TASK_STK[USART_STK_SIZE];
extern void Usart_task(void *p_arg);
#endif
