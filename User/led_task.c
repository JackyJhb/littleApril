#include "stm32f4xx.h"
#include "led_task.h"
#include "debug_config.h"
#include "beep.h"
#include "bsp_gpio.h"
#include "sccf.h"
#include "task_monitor.h"
#include "circleBuffer.h"

OS_TCB Led1TaskTCB;
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];

void led1_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	char led_status = 1;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(LED_TASK_WD);
	while(1)
	{
		if (led_status)
		{
			//GPIO_SetBits(GPIOE,GPIO_Pin_3);
			GPIO_SetBits(GPIOF,GPIO_Pin_10);
			OSTimeDlyHMSM(0,0,2,900,OS_OPT_TIME_DLY,&err);
			feedWatchDog(LED_TASK_WD);
			dataStore.blackBox.rebootTimes = 25;
			logPrintf(Info,"-------------------------------------------\r\n");
			logPrintf(Info,"I:BlackBox store information lists:\r\n");
			logPrintf(Info,"I:rebootTimes:%d\r\n",dataStore.blackBox.rebootTimes);
			logPrintf(Info,"I:communicateTimeOutErrTimes: %d %d %d %d\r\n",
					dataStore.blackBox.communicateTimeOutErrTimes[0],
					dataStore.blackBox.communicateTimeOutErrTimes[1],
					dataStore.blackBox.communicateTimeOutErrTimes[2],
					dataStore.blackBox.communicateTimeOutErrTimes[3]);
			logPrintf(Info,"I:memAllocErrTimes:%d\r\n",dataStore.blackBox.memAllocErrTimes);
			logPrintf(Info,"I:memTooLowTimes:%d\r\n",dataStore.blackBox.memTooLowTimes);
			logPrintf(Info,"I:sensorErrTimes:%d %d %d %d \r\n%d %d %d %d\r\n%d %d %d %d\r\n",
					dataStore.blackBox.sensorErrTimes[0][0],dataStore.blackBox.sensorErrTimes[0][1],dataStore.blackBox.sensorErrTimes[0][2],dataStore.blackBox.sensorErrTimes[0][3],
					dataStore.blackBox.sensorErrTimes[1][0],dataStore.blackBox.sensorErrTimes[1][1],dataStore.blackBox.sensorErrTimes[1][2],dataStore.blackBox.sensorErrTimes[1][3],
					dataStore.blackBox.sensorErrTimes[2][0],dataStore.blackBox.sensorErrTimes[2][1],dataStore.blackBox.sensorErrTimes[2][2],dataStore.blackBox.sensorErrTimes[2][3]);
			logPrintf(Info,"-------------------------------------------\r\n");
		}
		else
		{
			//GPIO_ResetBits(GPIOE,GPIO_Pin_3);
			GPIO_ResetBits(GPIOF,GPIO_Pin_10);
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY,&err);
		}
		led_status = !led_status;
	}
}