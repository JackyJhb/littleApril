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
	OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_DLY,&err);
	enableWatchDog(WATERPUMP_TASK_WD);
	while(1)
	{
		feedWatchDog(WATERPUMP_TASK_WD);
		average_temperature = 0;
		for(i = 0;i < 6;i++)
		{
			average_temperature += dataStore.realtimeData.insideTemperature[i/2][i%2];
		}
		average_temperature /= 6;
		if (isRunning == false)
		{
			if (average_temperature > 
				dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature)
			{
				isRunning = true;
				pumpStatus = true;
				counter = 0x00;
				dataStore.realtimeData.heatingColdingStatus |= Colding;
				littleAprilHCWCtrl(Colding,On);
				dataStore.realtimeData.isSideWindowMotorRunning |= COLDING_PUMP_WORKING;
				logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump enable,startTemperature = %.1f!\r\n",
							dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature);
			}
			else
			{
				pumpStatus = false;
				counter = 0x00;
				dataStore.realtimeData.heatingColdingStatus &= ~Colding;
				littleAprilHCWCtrl(Colding,Off);
				dataStore.realtimeData.isSideWindowMotorRunning &= ~COLDING_PUMP_WORKING;
				logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump disable,stopTemperature = %.1f!\r\n",
							dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature);
			}
		}
		
		if (isRunning == true)
		{
			++counter;
			if ((pumpStatus == false) && 
					(counter > dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime))
			{
				counter = 0x00;
				isRunning = false;
				logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump working!Working delay=%f\r\n",
								dataStore.ctrlParameter.systemOptions.waterPumpRunningTime);
			}
			else if ((pumpStatus == true) && 
					(counter > dataStore.ctrlParameter.systemOptions.waterPumpRunningTime))
			{
				if (dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime != 0)
				{
					dataStore.realtimeData.heatingColdingStatus &= ~Colding;
					littleAprilHCWCtrl(Colding,Off);
					dataStore.realtimeData.isSideWindowMotorRunning &= ~COLDING_PUMP_WORKING;
					pumpStatus = false;
				}
				counter = 0x00;
				logPrintf(Info,"I:waterpumpctrl_task.c::waterpump_task()->Pump stopped!Stopped delay=%f\r\n",
								dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime);
			}
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}