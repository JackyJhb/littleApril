#include "lightctrl_task.h"
#include "task_monitor.h"
#include "bsp_gpio.h"
#include "sccf.h"
#include "exti.h"
#include "timer.h"
#include "led.h"

OS_TCB LightCtrlTaskTCB;
CPU_STK LIGHTCTRL_TASK_STK[LIGHTCTRL_STK_SIZE];
void lightctrl_task(void *p_arg);

void setPluseWidth(uint8_t pluseWidth);

void setPluseWidth(uint8_t pluseWidth)
{

}

void enLighting(uint8_t isEnable)
{
	if (isEnable)
	{
		littleAprilGroup3Ctrl(Lighting_Group3,On);
	}
	else
	{
		littleAprilGroup3Ctrl(Lighting_Group3,Off);
	}
}

void lightctrl_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	CPU_INT32U     cpu_clk_freq;
	uint8_t isLightingOff = 0,minutes,minutesLast,offTimeMinutes;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	ExtiInit();
	SCR_Init();
	Timer9Init(10,8400);
	enableWatchDog(LIGHTCTRL_TASK_WD);
	while(1)
	{
		feedWatchDog(LIGHTCTRL_TASK_WD);
		if (isLightingOff == 0)
		{
			if (dataStore.realtimeData.hour == dataStore.ctrlParameter.illuminationStrength[dataStore.realtimeData.dayCycle].lightingOffTime)
			{
				isLightingOff = 1;
				offTimeMinutes = dataStore.ctrlParameter.illuminationStrength[dataStore.realtimeData.dayCycle].lightingOffMinutes;
				minutesLast = dataStore.realtimeData.minute;
				minutes = 0x00;
				enLighting(ENABLE);
				setPluseWidth(dataStore.ctrlParameter.illuminationStrength[dataStore.realtimeData.dayCycle].pluseWidth);
			}
		}
		if ((isLightingOff) && (minutesLast != dataStore.realtimeData.minute))
		{
			++minutes;
			minutesLast = dataStore.realtimeData.minute;
			if (minutes >= offTimeMinutes)
			{
				isLightingOff = 0;
				enLighting(DISABLE);
			}
		}
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}
