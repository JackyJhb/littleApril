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
	kStore.smallWin.x0 = CLOSE_ABSOLUTE_ANGLE;
	kStore.smallWin.x1 = OPEN_ABSOLUTE_ANGLE;
	kStore.smallWin.y0 = SIDE_WIN_CLOSE;
	kStore.smallWin.y1 = SIDE_WIN_OPEN;
	getKValue(&kStore.smallWin);
	
	kStore.pressureSensor.x0 = 0.0f;
	kStore.pressureSensor.x1 = -30.0f;
	kStore.pressureSensor.y0 = 4.5f;
	kStore.pressureSensor.y1 = 0.5f;
	getKValue(&kStore.pressureSensor);
	enableWatchDog(WATCHER_TASK_WD);
	while (1)
	{
		//TODO: Getting the temperature and volatage data and save these values to array.
		feedWatchDog(WATCHER_TASK_WD);
		//Get_ADC_Value(&kStore);
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_DLY,&err);
	}
}
