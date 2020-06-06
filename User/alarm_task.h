#ifndef ALARM_TASK_H
#define ALARM_TASK_H

#include "includes.h"
#include "priolevel.h"

#define AREA1_TEMPERATURE_ALARM 			0x01
#define AREA2_TEMPERATURE_ALARM				0x02
#define AREA3_TEMPERATURE_ALARM				0x04
#define BOILER_TEMPERATURE_ALARM            0x08
#define AREA1_HUMIDITY_ALARM                0x10
#define AREA2_HUMIDITY_ALARM                0x20
#define AREA3_HUMIDITY_ALARM                0x40

#define PIRACY_ALARM                  0x80000000

#define ALARM_STK_SIZE 		512

extern OS_TCB AlarmTaskTCB;
extern CPU_STK ALARM_TASK_STK[ALARM_STK_SIZE];
extern void alarm_task(void *p_arg);
#endif
