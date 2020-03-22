
#include "ventilation_task.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "bsp_can.h"
#include "pro_wrapper.h"
#include "debug_config.h"
#include "task_monitor.h"
#include "default_config.h"
#include "rtc.h"

OS_TCB VentilationTaskTCB;
CPU_STK VENTILATION_TASK_STK[VENTILATION_STK_SIZE];
char buf[20];

void ventilation_task(void *p_arg)
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	OS_MSG_SIZE    msg_size;
	char * pMsg,isFanWorking=0,level;
	float ventilation_volume = 0.0f,cmf_per_minute;
	uint16_t ventilation_cycle_counter = 0x00F0,ventilation_cycle = 0,fan_work_seconds,work_timer_counter;
	#ifndef ENABLE_USER_SET
	uint8_t fan_numbers;
	#endif
	CPU_SR_ALLOC();
	p_arg = p_arg;
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(VENTILATION_TASK_WD);
	while(1)
	{
		feedWatchDog(VENTILATION_TASK_WD);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);		
		if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
		{
			OS_CRITICAL_ENTER();
			if (dataStore.realtimeData.dayCycle > 49)
			{
				dataStore.realtimeData.dayCycle = 49;
			}
			#ifndef ENABLE_USER_SET
			ventilation_cycle = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle/7].ventilationCycle;
			#else
			//ventilation_cycle = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].ventilationCycle;
			ventilation_cycle = dataStore.ctrlParameter.systemOptions.runningTimeOfVentilate +
													dataStore.ctrlParameter.systemOptions.stoppedTimeOfVentilate;
			#endif
			OS_CRITICAL_EXIT();
			if (dataStore.realtimeData.isColding == true)
			{
				ventilation_cycle_counter = dataStore.ctrlParameter.systemOptions.runningTimeOfVentilate;
				isFanWorking = false;
				#ifdef ENABLE_OUTPUT_LOG
				printf("-------------->isColding now,do not use ventilation.\r\n");
				#endif
			}
			++ventilation_cycle_counter;
			if ((ventilation_cycle_counter >= ventilation_cycle) && 
					(dataStore.realtimeData.isColding == false))
			{
				if (isFanWorking == false)
				{
					//fan_work_seconds = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].runningTime +
										//dataStore.realtimeData.deltaActionTimeSpan;
					fan_work_seconds = dataStore.ctrlParameter.systemOptions.runningTimeOfVentilate;
					#ifdef ENABLE_OUTPUT_LOG
						printf("############Manual control ventilation###########\r\n");
						printf("Info:ventilation_task.c::ventilation_task :\r\n");
						printf("dayCycle = %d\r\n",dataStore.realtimeData.dayCycle);
						printf("fan_work_seconds = %d\r\n",fan_work_seconds);
						printf("ventilation_cycle = %d\r\n",ventilation_cycle);
						printf("deltaActionTimeSpan = %d\r\n",dataStore.realtimeData.deltaActionTimeSpan);
						printf("###############################################\r\n");
					#endif
					
					work_timer_counter = 0x00;
					if (dataStore.realtimeData.dayCycle < 19)
					{
						level = 0;
					}
					else
					{
						level = 1;
					}
					dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.coolDownGrade[level].sideWindowOpenAngle;
					if (fan_work_seconds > 0)
					{
						littleAprilFanCtrl(dataStore.ctrlParameter.coolDownGrade[level].runningFansBits);
						isFanWorking = true;
					}
				}
				else
				{
					if (++work_timer_counter >= fan_work_seconds)
					{
						ventilation_cycle_counter = fan_work_seconds;
						if (dataStore.realtimeData.dayCycle < 19)
						{
							dataStore.realtimeData.workingVentilators &= (~COOL_DOWN_DEFAULT_LEVEL_1);
						}
						else
						{
							dataStore.realtimeData.workingVentilators &= (~COOL_DOWN_DEFAULT_LEVEL_2);
						}
						littleAprilFanCtrl(dataStore.realtimeData.workingVentilators);
						isFanWorking = false;
						//dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle;
						#ifdef ENABLE_OUTPUT_LOG
						printf("Info:ventilation_task.c::ventilation_task ventilators stopped working.\r\n");
						#endif
					}
				}
			}
		}
	}
}
