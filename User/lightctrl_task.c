#include "stm32f4xx.h"
#include "lightctrl_task.h"
#include "task_monitor.h"
#include "bsp_gpio.h"
#include "sccf.h"
#include "exti.h"
#include "pwm.h"
#include "led.h"
#include "circleBuffer.h"
#include "ds18b20.h"

extern OS_SEM zeroSem;
uint8_t isLighting = true;
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
	uint16_t offCounter,lastMiniute;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	ExtiInit();
	SCR_Init();
	Timer9Init(1000,1680);     //500KHz/2.5K 200Hz
	//enableWatchDog(LIGHTCTRL_TASK_WD);
	while(1)
	{
		//feedWatchDog(LIGHTCTRL_TASK_WD);
		OSSemPend(&zeroSem,200,OS_OPT_PEND_BLOCKING,0,&err);
		if (err != OS_ERR_NONE)
		{
			logPrintf(Error,"E:lightctrl_task->lightctrl_task()::Wait Sem error :%d!\r\n",err);
		}
		else
		{
			delays_us(30);
			GPIO_ResetBits(GPIOE,GPIO_Pin_6);
		}
		
		if (isLighting == true)
		{
			if (dataStore.ctrlParameter.illuminationStrength[dataStore.realtimeData.dayCycle].lightingOffTime ==
					dataStore.realtimeData.hour)
			{
				if (dataStore.realtimeData.minute == 0x14)
				{
					isLighting = false;
					offCounter = 0;
					lastMiniute = dataStore.realtimeData.minute;
				}
			}
		}
		else
		{
			if (lastMiniute != dataStore.realtimeData.minute)
			{
				++offCounter;
				lastMiniute = dataStore.realtimeData.minute;
				if (offCounter >= dataStore.ctrlParameter.illuminationStrength[dataStore.realtimeData.dayCycle].lightingOffMinutes)
					isLighting = true;
			}
		}
		
	}
}
