#include "boilerctrl_task.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "task_monitor.h"

OS_TCB BoilerCtrlTaskTCB;
CPU_STK BOILERCTRL_TASK_STK[BOILERCTRL_STK_SIZE];
void boilerctrl_task(void *p_arg);

void boilerctrl_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	CPU_INT32U     cpu_clk_freq;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(BOILERCTRL_TASK_WD);
	while(1)
	{
		feedWatchDog(BOILERCTRL_TASK_WD);
		if (dataStore.realtimeData.boilerInsideTemperature > dataStore.ctrlParameter.systemOptions.stopFanShaftTemperature)
		{
			littleAprilGroup3Ctrl(FanShaft_Group3,Off);
		}
		if (dataStore.realtimeData.boilerInsideTemperature < dataStore.ctrlParameter.systemOptions.startFanShaftTemperature)
		{
			littleAprilGroup3Ctrl(FanShaft_Group3,On);
		}

		if (dataStore.realtimeData.boilerInsideTemperature > dataStore.ctrlParameter.systemOptions.startCirculatingPumpTemperature)
		{
			littleAprilGroup3Ctrl(CirculatePump_Group3,On);
		}

		if (dataStore.realtimeData.boilerInsideTemperature < dataStore.ctrlParameter.systemOptions.stopCirculatingPumpTemperature)
		{
			littleAprilGroup3Ctrl(CirculatePump_Group3,Off);
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}
