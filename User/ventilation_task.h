#ifndef VENTILATION_TASK_H
#define VENTILATION_TASK_H

#include "includes.h"
#include "bsp_gpio.h"
#include "priolevel.h"

#define VENTILATION_STK_SIZE 		512

extern OS_TCB VentilationTaskTCB;
extern CPU_STK VENTILATION_TASK_STK[VENTILATION_STK_SIZE];
extern void ventilation_task(void *p_arg);
#endif
