#include "stm32f4xx.h"
#include "rtc_task.h"
#include "debug_config.h"
#include "beep.h"
#include "sccf.h"
#include "task_monitor.h"
#include "bsp_gpio.h"
#include "circleBuffer.h"

OS_TCB RTCTaskTCB;
CPU_STK RTC_TASK_STK[RTC_STK_SIZE];

void rtc_task(void *p_arg)
{
	OS_ERR 			err;
	CPU_INT32U      cpu_clk_freq;
	CPU_TS          ts;
	static char a = 0x55;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	p_arg = p_arg;
	enableWatchDog(RTC_TASK_WD);
	while(1)
	{
		feedWatchDog(RTC_TASK_WD);
		OSTaskSemPend ((OS_TICK   )0,
						(OS_OPT    )OS_OPT_PEND_BLOCKING,
						(CPU_TS   *)&ts,
						(OS_ERR   *)&err);
		if (err != OS_ERR_NONE)
		{
			logPrintf(Error,"Error:RTC_task::QSTaskQPend() err = %d \r\n",err);
		}
		RTC_GetTimes(RTC_Format_BIN);
		dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Seconds = RTC_TimeStruct.RTC_Seconds;
		dataStore.realtimeData.year = RTC_DateStruct.RTC_Year;
		dataStore.realtimeData.month = RTC_DateStruct.RTC_Month;
		dataStore.realtimeData.day = RTC_DateStruct.RTC_Date;
		dataStore.realtimeData.hour = RTC_TimeStruct.RTC_Hours;
		dataStore.realtimeData.minute = RTC_TimeStruct.RTC_Minutes;
		dataStore.realtimeData.second = RTC_TimeStruct.RTC_Seconds;
	}
}
