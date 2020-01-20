#include "stm32f4xx.h"
#include "watcher_task.h"
#include "sccf.h"
#include "adc.h"
#include "ds18b20.h"
#include "debug_config.h"
#include "beep.h"
#include "task_monitor.h"

OS_TCB WatcherTaskTCB;
CPU_STK WATCHER_TASK_STK[WATCHER_STK_SIZE];

void watcher_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(WATCHER_TASK_WD);
	while (1)
	{
		//TODO: Getting the temperature and volatage data and save these values to array.
		feedWatchDog(WATCHER_TASK_WD);
		Get_ADC_Value();
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_DLY,&err);
	}
}
