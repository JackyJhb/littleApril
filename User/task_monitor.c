#include "task_monitor.h"
#include "debug_config.h"
#include "sccf.h"
#include "circleBuffer.h"

OS_TCB MonitorTaskTCB;
CPU_STK MONITOR_TASK_STK[MONITOR_STK_SIZE];

static uint8_t feedWatchDogBuf[32];
static uint32_t enableWatchDogBits = 0x00;
void readSTM32UniqueID(void);
ResetSourceType getRstSrcType(void);
void enableWatchDog(uint8_t whichTask);
void disableWatchDog(uint8_t whichTask);
void feedWatchDog(uint8_t whichTask);
void watchDogInit(void);

void readSTM32UniqueID(void)
{
	uint8_t i;
	__IO uint8_t *addr = (__IO uint8_t *)(0x1FFF7A10);
	logPrintf(Info,"Info:task_monitor.c::readSTM32UniqueID()->\r\n");
	for (i = 0;i < STM32_UNIQUE_ID_SIZE;i++)
	{
		dataStore.realtimeData.stm32UniqueID[i] = *(addr+i);
		logPrintf(Info,"%d ",dataStore.realtimeData.stm32UniqueID[i]);
	}
	logPrintf(Info,"\r\n");
}

ResetSourceType getRstSrcType(void)
{
	ResetSourceType type;
	if (RCC->CSR & (1<<31))
	{
		type = LOWPOWER_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is LOWPOWER_RST!\r\n");
	}
	else if (RCC->CSR & (1<<30))
	{
		type = WWDG_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is WWDG_RST!\r\n");
	}
	else if (RCC->CSR & (1<<29))
	{
		type = IWDG_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is IWDG_RST!\r\n");
	}
	else if (RCC->CSR & (1<<28))
	{
		type = SOFTWARE_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is SOFTWARE_RST!\r\n");
	}
	else if (RCC->CSR & (1<<27))
	{
		type = POR_PDR_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is POR_PDR_RST!\r\n");
	}
	else if (RCC->CSR & (1<<26))
	{
		type = NRST_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is NRST_RST!\r\n");
	}
	else if (RCC->CSR & (1<<25))
	{
		type = RMVF_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is RMVF_RST!\r\n");
	}
	else
	{
		type = UNKNOWN_RST;
		logPrintf(Info,"Info:task_monitor.c::getRstSrcType()->System reset type is UNKNOWN_RST!\r\n");
	}
	RCC->CSR |= (1<<24);
	return type;
}

void watchDogInit(void)
{
	getRstSrcType();
	readSTM32UniqueID();
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(100);
	IWDG_Enable();
	logPrintf(Info,"Info:task_monitor.c::watchDogInit()->Watch dog was initialized!\r\n");
}

void enableWatchDog(uint8_t whichTask)
{
	uint16_t tmp = (1<<whichTask);
	enableWatchDogBits |= tmp;
	feedWatchDogBuf[whichTask] = 1;
}

void disableWatchDog(uint8_t whichTask)
{
	enableWatchDogBits &= ~(1<<whichTask);
}

void feedWatchDog(uint8_t whichTask)
{
	if ((enableWatchDogBits & (1<<whichTask)))
	{
		++feedWatchDogBuf[whichTask];
	}
}

uint8_t needToFeedWatchDog(void)
{
	uint8_t i,watch_dog_couter=0,dead_task_nums=0;
	for (i = 0;i < 16;i++)
	{
		if (enableWatchDogBits & (1<<i))
		{
			if (feedWatchDogBuf[i] == 0)
			{
				++dead_task_nums;
				logPrintf(Fatal,"F:task_monitor.c::needToFeedWatchDog()->");
				switch (i)
				{
					case RTC_TASK_WD:
						logPrintf(Fatal,"F:rtc_task was dead!!!\r\n");
						break;
					case ENVCTRL_TASK_WD:
						logPrintf(Fatal,"F:EnvParameter_task was dead!!!\r\n");
						break;
					case VENTILATION_TASK_WD:
						logPrintf(Fatal,"F:ventilation_task was dead!!!\r\n");
						break;
					case WATCHER_TASK_WD:
						logPrintf(Fatal,"F:watcher_task was dead!!!\r\n");
						break;
					case TS_TASK_WD:
						logPrintf(Fatal,"F:ts_task was dead!!!\r\n");
						break;
					case LED_TASK_WD:
						logPrintf(Fatal,"F:led1_task was dead!!!\r\n");
						break;
					case SIDE_WINDOW_TASK_WD:
						logPrintf(Fatal,"F:sidewindowctl_task was dead!!!\r\n");
						break;
					case ALARM_TASK_WD:
						logPrintf(Fatal,"F:alarm_task was dead!!!\r\n");
						break;
					case USART_TASK_WD:
						logPrintf(Fatal,"F:usart_task was dead!!!\r\n");
						break;
					case WATERPUMP_TASK_WD:
						logPrintf(Fatal,"F:waterpumpctrl_task was dead!!!\r\n");
						break;
					case BOILERCTRL_TASK_WD:
						logPrintf(Fatal,"F:boilerctrl_task was dead!!!\r\n");
						break;
					case LIGHTCTRL_TASK_WD:
						logPrintf(Fatal,"F:lightctrl_task was dead!!!\r\n");
						break;
					default:
						logPrintf(Fatal,"Unkown watch dog error occurred!!!\r\n");
						break;
				}
			}
			else
			{
				feedWatchDogBuf[i] = 0;
			}
		}
	}
	return dead_task_nums;
}

void monitor_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	uint8_t counter = 0;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	watchDogInit();
	while (1)
	{
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
		++counter;
		if (counter > 10)
		{
			counter = 0;
			if (needToFeedWatchDog())
			{
				logPrintf(Fatal,"F:task_monitor.c::monitor_task()->System will be reboot latter!\r\n");
				while(1)
				{
					OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
				}
			}
		}
		IWDG_ReloadCounter();
	}
}
