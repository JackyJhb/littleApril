
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
	uint16_t ventilation_cycle_counter,fan_work_seconds,work_timer_counter;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(VENTILATION_TASK_WD);
	ventilation_cycle_counter = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].runningTime;
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
			OS_CRITICAL_EXIT();
			if (dataStore.realtimeData.isColding == true)
			{
				ventilation_cycle_counter = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].runningTime;
				isFanWorking = false;
				//logPrintf("-------------->isColding now,do not use ventilation.\r\n");
			}
			++ventilation_cycle_counter;
			if (ventilation_cycle_counter >= dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].ventilationCycle)
			{
				level = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].grade;
				if (isFanWorking == false)
				{
					fan_work_seconds = dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].runningTime;
					//logPrintf("############Manual control ventilation###########\r\n");
					//logPrintf("Info:ventilation_task.c::ventilation_task :\r\n");
					//logPrintf("dayCycle = %d\r\n",dataStore.realtimeData.dayCycle);
					//logPrintf("fan_work_seconds = %d\r\n",fan_work_seconds);
					//logPrintf("ventilation_cycle = %d\r\n",dataStore.ctrlParameter.ventilation.ventilationCoefficient[dataStore.realtimeData.dayCycle].ventilationCycle);
					//logPrintf("###############################################\r\n");
					work_timer_counter = 0x00;
					//dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.coolDownGrade[level].sideWindowOpenAngle;
					dataStore.realtimeData.workingVentilators = dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningFansBits;
					littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
					isFanWorking = true;
				}
				else
				{
					if (++work_timer_counter >= fan_work_seconds)
					{
						ventilation_cycle_counter = fan_work_seconds;
						dataStore.realtimeData.workingVentilators &= 
						          (~dataStore.ctrlParameter.ventilation.ventilateGrade[level].runningFansBits);
						littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
						isFanWorking = false;
						//dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle;
						//logPrintf("Info:ventilation_task.c::ventilation_task ventilators stopped working.\r\n");
					}
				}
			}
		}
		else
		{
			dataStore.realtimeData.workingVentilators = 0x0000;
			littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
		}
	}
}
