#include "system.h"
#include "SysTick.h"
#include "main_task.h"

#include "bsp_button.h"
#include "bsp_gpio.h"
#include "bsp_can.h"
#include "led.h"
#include "beep.h"
#include "exti.h"
#include "usart.h"
#include "includes.h"
#include "rtc.h"
#include "adc.h"
#include "ds18b20.h"

#include "debug_config.h"
#include "sccf.h"
#include "task_monitor.h"
int main()
{  	
	OS_ERR err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//SysTick_Init();
	LED_Init();
	BEEP_Init();
	RS485_Init(115200);
	USART1_Init(115200);
	AT24C02_Init();
	littleAprilIOInit();
	getRstSrcType();
	while (RTC_InitConfig());
	RTC_GetTimes(RTC_Format_BIN);
	while ((err = sysCtrlConfigFileInit()))
	{
		#ifdef ENABLE_OUTPUT_LOG
		printf("Error:main.c::main() -> sysCtrlConfigFileInit error occurred! Error code is %d\r\n",err);
		#endif
	}
	if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
	{
		dataStore.realtimeData.dayCycle = calDaysBettweenTwoDate(&dataStore.realtimeData.realDataToSave.rtcDateStart,
																 &dataStore.realtimeData.realDataToSave.rtcTimeStart);
		RTC_SetAlarmA(dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_WeekDay,
					dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Hours,
					dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Minutes,
					dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Seconds);
		#if defined(ENABLE_OUTPUT_LOG) || defined(ENABLE_BASE_LOG)
		printf("#############################################################\r\n");
		printf("#                      littleApril core                     #\r\n");
		printf("#Branch:beta                                                #\r\n");
		printf("#Version:V1.1.10 (0308E)                                    #\r\n");
		printf("#Change date:2020/03/08                                     #\r\n");
		printf("#Ventilation cycle changed.                                 #\r\n");
		printf("#############################################################\r\n");
		printf("Info:main.c::System start date:%d year %d month %d day\r\n",
				dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_Year,
				dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_Month,
				dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_Date);
		printf("Info:main.c::System start time:%d hour %d minute %d second\r\n",
				dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Hours,
				dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Minutes,
				dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Seconds);
		printf("Info:main.c::System days of cycle:%d\r\n",dataStore.realtimeData.dayCycle);
		printf("#############################################################\r\n");
		#endif
	}
	//TODO: Need to add some initilizate code in here to do some necessary jobs for ADC devices and Ds18B20 sensors.
	ADCx_Init();
	DS18B20_GPIO_Init();
	watchDogInit();
	
	OSInit(&err);

	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,
				 (CPU_CHAR	* )"start task",
                 (OS_TASK_PTR )start_task,
                 (void		* )0,
                 (OS_PRIO	  )START_TASK_PRIO,
                 (CPU_STK   * )&START_TASK_STK[0],
                 (CPU_STK_SIZE)START_STK_SIZE/10,
                 (CPU_STK_SIZE)START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OSStart(&err);
	while(1);
}