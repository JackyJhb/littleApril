#include "task_monitor.h"
#include "debug_config.h"
OS_TCB MonitorTaskTCB;
CPU_STK MONITOR_TASK_STK[MONITOR_STK_SIZE];

static uint8_t feedWatchDogBuf[16];
static uint16_t enableWatchDogBits = 0x00;
static uint8_t stm32UniqueID[STM32_UNIQUE_ID_SIZE] = {0};

void readSTM32UniqueID(void);
ResetSourceType getRstSrcType(void);
void enableWatchDog(uint8_t whichTask);
void disableWatchDog(uint8_t whichTask);
void feedWatchDog(uint8_t whichTask);
void watchDogInit(void);

void readSTM32UniqueID(void)
{
	uint8_t i;
	vu8 *addr = (vu8*)(0x1ffff7a10);
	#ifdef ENABLE_OUTPUT_LOG
	printf("Info:task_monitor.c::readSTM32UniqueID()->\r\n");
	#endif
	for (i = 0;i < STM32_UNIQUE_ID_SIZE;i++)
	{
		stm32UniqueID[i] = *addr;
		++addr;
		#ifdef ENABLE_OUTPUT_LOG
		printf("%d ",stm32UniqueID[i]);
		#endif
	}
	#ifdef ENABLE_OUTPUT_LOG
	printf("\r\n");
	#endif
}

ResetSourceType getRstSrcType(void)
{
	ResetSourceType type;
	if (RCC->CSR & (1<<31))
	{
		type = LOWPOWER_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is LOWPOWER_RST!\r\n");
		#endif
	}
	else if (RCC->CSR & (1<<30))
	{
		type = WWDG_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is WWDG_RST!\r\n");
		#endif
	}
	else if (RCC->CSR & (1<<29))
	{
		type = IWDG_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is IWDG_RST!\r\n");
		#endif
	}
	else if (RCC->CSR & (1<<28))
	{
		type = SOFTWARE_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is SOFTWARE_RST!\r\n");
		#endif
	}
	else if (RCC->CSR & (1<<27))
	{
		type = POR_PDR_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is POR_PDR_RST!\r\n");
		#endif
	}
	else if (RCC->CSR & (1<<26))
	{
		type = NRST_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is NRST_RST!\r\n");
		#endif
	}
	else if (RCC->CSR & (1<<25))
	{
		type = RMVF_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is RMVF_RST!\r\n");
		#endif
	}
	else
	{
		type = UNKNOWN_RST;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:task_monitor.c::getRstSrcType()->System reset type is UNKNOWN_RST!\r\n");
		#endif
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
	#ifdef ENABLE_OUTPUT_LOG
	printf("Info:task_monitor.c::watchDogInit()->Watch dog was initialized!\r\n");
	#endif
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
				#ifdef ENABLE_OUTPUT_LOG
				printf("Fatal:task_monitor.c::needToFeedWatchDog()->");
				switch (i)
				{
					case RTC_TASK_WD:
						printf("rtc_task was dead!!!\r\n");
						break;
					case ENVCTRL_TASK_WD:
						printf("EnvParameter_task was dead!!!\r\n");
						break;
					case VENTILATION_TASK_WD:
						printf("ventilation_task was dead!!!\r\n");
						break;
					case WATCHER_TASK_WD:
						printf("watcher_task was dead!!!\r\n");
						break;
					case TS_TASK_WD:
						printf("ts_task was dead!!!\r\n");
						break;
					case LED_TASK_WD:
						printf("led1_task was dead!!!\r\n");
						break;
					case SIDE_WINDOW_TASK_WD:
						printf("SIDE_WINDOW_TASK_WD was dead!!!\r\n");
						break;
					case USART_TASK_WD:
						printf("USART_TASK_WD was dead!!!\r\n");
						break;
					case WATERPUMP_TASK_WD:
						printf("WATERPUMP_TASK_WD was dead!!!\r\n");
						break;
					default:
						printf("Unkown watch dog error occurred!!!\r\n");
						break;
				}
				#endif
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
				#ifdef ENABLE_OUTPUT_LOG
				printf("Fatal:task_monitor.c::monitor_task()->System will be reboot latter!\r\n");
				#endif
				while(1);
			}
		}
		IWDG_ReloadCounter();
	}
}
