
#include "ventilation_task.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "bsp_can.h"
#include "pro_wrapper.h"
#include "debug_config.h"
#include "task_monitor.h"
#include "default_config.h"
#include "rtc.h"
#include "circleBuffer.h"

OS_TCB VentilationTaskTCB;
CPU_STK VENTILATION_TASK_STK[VENTILATION_STK_SIZE];
char buf[20];

uint8_t getGrade(void)
{
	uint8_t i,grade=0;
	float average_temperature;
	average_temperature = 0;
	for(i = 0;i < 6;i++)
	{
		average_temperature += dataStore.realtimeData.insideTemperature[i/2][i%2];
	}
	average_temperature /= 6;
	logPrintf(Verbose,"V:average_temperature = %.2f\r\n",average_temperature);
	for (i =0;i < (sizeof(dataStore.ctrlParameter.ventilation.ventilateGrade)/sizeof(VentilateGrade));i++)
	{
		if (average_temperature < (dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.ventilation.ventilateGrade[i].gradeTemperature))
		{
			logPrintf(Verbose,"V:break at i = %d\r\n",i);
			grade = i;
			break;
		}
	}
	if (i >=5)
	{
		grade = 4;
	}
	logPrintf(Verbose,"V:ventilation_task.c::--->getGrade():grade=%d.\r\n",grade);
	return grade;
}

void ventilation_task(void *p_arg)
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	OS_MSG_SIZE    msg_size;
	char * pMsg,isFanWorking=false,level = 0,hours,isGetGradeCycle = true;
	float ventilation_volume = 0.0f,set_temperature;
	uint16_t ventilation_cycle_counter=0x0000,fan_work_seconds,work_timer_counter;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	cpu_clk_freq = BSP_CPU_ClkFreq();
	
	hours = calHoursInOneDay(&dataStore.realtimeData.realDataToSave.rtcTimeStart);
	set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle-1] - dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle];
	set_temperature = (set_temperature/24) * hours;
	set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle-1] - set_temperature;
	dataStore.realtimeData.currentSetTemperature = set_temperature;
	OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_DLY,&err);
	enableWatchDog(VENTILATION_TASK_WD);
	while(1)
	{
		feedWatchDog(VENTILATION_TASK_WD);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);		
		logPrintf(Debug,"D:ventilation_task.c::ventilation_task()->main loop.\r\n");
		if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
		{
			if ((dataStore.realtimeData.isColding & IS_COLDING) == IS_COLDING)
			{
				isGetGradeCycle = true;
				logPrintf(Verbose,"V:ventilation_task.c::ventilation_task()->do not use ventilation.\r\n");
			}
			else
			{
				if (isGetGradeCycle == true)
				{
					level = getGrade();
					dataStore.realtimeData.isColding &= ~IS_COLDING;
					dataStore.realtimeData.isColding &= ~LEVEL_MASK;
					dataStore.realtimeData.isColding |= level;
					isGetGradeCycle = false;
					isFanWorking = false;
					ventilation_cycle_counter = 0x0000;
					logPrintf(Debug,"D:ventilation_task.c::ventilation_task()->isColding=%d\r\n",dataStore.realtimeData.isColding);
				}
				if (isFanWorking == false)
				{
					++ventilation_cycle_counter;
					if (ventilation_cycle_counter >= 
								(dataStore.ctrlParameter.ventilation.ventilateGrade[level].ventilationCycle - 
								dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningTime))
					{
						fan_work_seconds = dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningTime;
						work_timer_counter = 0x00;
						if (fan_work_seconds > 0)
						{
							dataStore.realtimeData.workingVentilators = dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningFansBits;
							littleApril19FansCtrl(dataStore.realtimeData.workingVentilators,VENTILATION_TASK_WD);
						}
						isFanWorking = true;
						logPrintf(Verbose,"V: run time is %d,cycle is %d,temp = %.2f,run_bits = %d,level = %d\r\n",
													dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningTime,
													dataStore.ctrlParameter.ventilation.ventilateGrade[level].ventilationCycle,
													dataStore.ctrlParameter.ventilation.ventilateGrade[level].gradeTemperature,
													dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningFansBits,
													level);
					}
				}
				else
				{
					if (++work_timer_counter >= fan_work_seconds)
					{
						if (fan_work_seconds != dataStore.ctrlParameter.ventilation.ventilateGrade[level].ventilationCycle)
						{
							dataStore.realtimeData.workingVentilators = 0x00000000;
							littleApril19FansCtrl(dataStore.realtimeData.workingVentilators,VENTILATION_TASK_WD);
							logPrintf(Debug,"D:ventilation_task.c::ventilation_task()->Stop\r\n");
						}
						isGetGradeCycle = true;
						logPrintf(Debug,"D:ventilation_task.c::ventilation_task()->Next cycle!\r\n");
					}
				}
			}
		}
		else
		{
			dataStore.realtimeData.workingVentilators = 0x00000000;
			littleApril19FansCtrl(dataStore.realtimeData.workingVentilators,VENTILATION_TASK_WD);
		}
	}
}
