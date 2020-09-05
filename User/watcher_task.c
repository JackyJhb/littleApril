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

void getKValue(KStruct *k)
{
	k->k = (k->y1 - k->y0)/(k->x1 - k->x0);
}

void watcher_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	KStore kStore;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	getKValue(&dataStore.ctrlParameter.sensorKOptions.smallWin);
	getKValue(&dataStore.ctrlParameter.sensorKOptions.bigWin);
	getKValue(&dataStore.ctrlParameter.sensorKOptions.pressureSensor);

	enableWatchDog(WATCHER_TASK_WD);
	while (1)
	{
		feedWatchDog(WATCHER_TASK_WD);
		Get_ADC_Value(&dataStore.ctrlParameter.sensorKOptions);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}
