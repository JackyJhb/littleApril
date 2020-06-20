#include "waterpumpctrl_task.h"
#include "stm32f4xx.h"
#include "sccf.h"
#include "debug_config.h"
#include "task_monitor.h"
#include "circleBuffer.h"

OS_TCB WATERPUMPTaskTCB;	
CPU_STK WATERPUMP_TASK_STK[WATERPUMP_STK_SIZE];

void waterpump_task(void *p_arg);

void waterpump_task(void *p_arg)
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	float average_temperature;
	uint8_t isRunning = false,pumpStatus = false,i;
	uint32_t counter = 0;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(WATERPUMP_TASK_WD);
	while(1)
	{
		feedWatchDog(WATERPUMP_TASK_WD);
		
		average_temperature = 0;
		for(i = 0;i < 6;i++)
		{
			average_temperature += *((float *)dataStore.realtimeData.insideTemperature +i);
		}
		average_temperature /= 6;
		
		if ((isRunning == true) && 
			(average_temperature < 
				dataStore.ctrlParameter.systemOptions.waterPumpStopTemperature))
		{
			isRunning = false;
			littleAprilHCWCtrl(Colding,Off);
			logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump disable,stopTemperature = %.1f!\r\n",
							dataStore.ctrlParameter.systemOptions.waterPumpStopTemperature);
		}
		else if ((isRunning == false) &&
			(average_temperature > 
				dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature))
		{
			isRunning = true;
			pumpStatus = true;
			counter = 0x00;
			littleAprilHCWCtrl(Colding,On);
			logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump enable,startTemperature = %.1f!\r\n",
							dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature);
		}
		
		if (isRunning == true)
		{
			++counter;
			if ((pumpStatus == false) && 
					(counter > dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime))
			{
				littleAprilHCWCtrl(Colding,On);
				counter = 0x00;
				pumpStatus = true;
				logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump working!Working delay=%f\r\n",
								dataStore.ctrlParameter.systemOptions.waterPumpRunningTime);
			}
			else if ((pumpStatus == true) && 
					(counter > dataStore.ctrlParameter.systemOptions.waterPumpRunningTime))
			{
				littleAprilHCWCtrl(Colding,Off);
				counter = 0x00;
				pumpStatus = false;
				logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump stopped!Stopped delay=%f\r\n",
								dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime);
			}
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}