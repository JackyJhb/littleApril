#ifndef TASK_MONITOR_H
#define TASK_MONITOR_H

#include "includes.h"
#include "stm32f4xx.h"

#define RTC_TASK_WD         0
#define ENVCTRL_TASK_WD 	1
#define VENTILATION_TASK_WD 2
#define WATCHER_TASK_WD     3
#define TS_TASK_WD          4
#define LED_TASK_WD         5
#define SIDE_WINDOW_TASK_WD 6
#define ALARM_TASK_WD       7

#define STM32_UNIQUE_ID_SIZE 12

#define MONITOR_TASK_PRIO		3
#define MONITOR_STK_SIZE 		512
typedef enum{
	LOWPOWER_RST,
	WWDG_RST,
	IWDG_RST,
	SOFTWARE_RST,
	POR_PDR_RST,
	NRST_RST,
	RMVF_RST,
	UNKNOWN_RST
}ResetSourceType;


extern OS_TCB MonitorTaskTCB;
extern CPU_STK MONITOR_TASK_STK[MONITOR_STK_SIZE];

extern ResetSourceType getRstSrcType(void);
extern void enableWatchDog(uint8_t whichTask);
extern void disableWatchDog(uint8_t whichTask);
extern void feedWatchDog(uint8_t whichTask);
extern void monitor_task(void *p_arg);
extern void watchDogInit(void);

#endif
