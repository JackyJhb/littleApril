#ifndef RTC_TASK_H
#define RTC_TASK_H
#include "includes.h"
#include "rtc.h"
#include "priolevel.h"

#define RTC_STK_SIZE 		128

extern OS_TCB RTCTaskTCB;	
extern CPU_STK RTC_TASK_STK[RTC_STK_SIZE];
extern void rtc_task(void *p_arg);
#endif
