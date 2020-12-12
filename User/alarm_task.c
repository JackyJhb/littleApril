#include "system.h"
#include "alarm_task.h"
#include "debug_config.h"
#include "beep.h"
#include "sccf.h"
#include "task_monitor.h"
#include "rtc.h"
#include "stm32f4xx.h"                  // Device header
#include "circleBuffer.h"
#include "bsp_gpio.h"

OS_TCB AlarmTaskTCB;
CPU_STK ALARM_TASK_STK[ALARM_STK_SIZE];

void alarm_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	CPU_INT32U     cpu_clk_freq;
	char alarm_status,* pMsg,beep_status = 0x00,i;
	uint32_t alarm_bits = 0x00;
	float average_temperature;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	BEEP_Init();
	littleAprilGroup3Ctrl(Warning_Group3,On);
	OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_DLY,&err);
	littleAprilGroup3Ctrl(Warning_Group3,Off);
	enableWatchDog(ALARM_TASK_WD);
	while(1)
	{
		feedWatchDog(ALARM_TASK_WD);
		
		average_temperature = 0;
		for(i = 0;i < 6;i++)
		{
			average_temperature += dataStore.realtimeData.insideTemperature[i/2][i%2];
		}
		average_temperature /= 6;
		if (average_temperature > (dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureHighThreshold))
		{
			alarm_bits |= INSIDE_TEMPER_HIGH_ALARM;
		}
		else if (average_temperature < (dataStore.realtimeData.currentSetTemperature - dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureLowThreshold))
		{
			alarm_bits |= INSIDE_TEMPER_LOW_ALARM;
		}
		else
		{
			alarm_bits &= ~INSIDE_TEMPER_HIGH_ALARM;
			alarm_bits &= ~INSIDE_TEMPER_LOW_ALARM;
		}
		
		if (dataStore.realtimeData.boilerPipeTemperature < dataStore.ctrlParameter.alarmThresholdOptions.boilerTemperatureLowThreshold)
		{
			alarm_bits |= BOILER_TEMPER_LOW_ALARM;
		}
		else if (dataStore.realtimeData.boilerPipeTemperature > dataStore.ctrlParameter.alarmThresholdOptions.boilerTemperatureHighThreshold)
		{
			alarm_bits |= BOILER_TEMPER_LOW_ALARM;
		}
		else
		{
			alarm_bits &= ~BOILER_TEMPER_HIGH_ALARM;
			alarm_bits &= ~BOILER_TEMPER_LOW_ALARM;
		}
		
		#ifdef ENABLE_PIRACY_TRAP
		if ((RTC_DateStruct.RTC_Year > 20) || (RTC_DateStruct.RTC_Month > 7)) 
		{
			alarm_bits |= PIRACY_ALARM;
		}
		#endif
		
		for (i = 0;i < 3;i++)
		{
			if (dataStore.sensorStatusCounter.clientDeviceErrCounter[i] > ERR_TIMES_LIMIT)
			{
				alarm_bits |= (CLIENT_CONNECT_ERR<<i);
			}
			else
			{
				alarm_bits &= ~(CLIENT_CONNECT_ERR<<i);
			}
			
			if (dataStore.sensorStatusCounter.areaHeatingErrCounter[i] > ERR_TIMES_LIMIT)
			{
				alarm_bits |= (AREA_HEATING_ERR << i);
			}
			else
			{
				alarm_bits &= ~(AREA_HEATING_ERR << i);
			}
			
			if ((dataStore.sensorStatusCounter.areaLeftSensorErrCounter[i] >ERR_TIMES_LIMIT) ||
				(dataStore.sensorStatusCounter.areaRightSensorErrCounter[i] > ERR_TIMES_LIMIT))
			{
				alarm_bits |= (AREA_SENSOR_ERR << i);
			}
			else
			{
				alarm_bits &= ~(AREA_SENSOR_ERR << i);
			}
		}
		
		if (dataStore.sensorStatusCounter.outsideSensorErrCounter > ERR_TIMES_LIMIT)
		{
			alarm_bits |= OUTSIDE_SENSOR_ERR;
		}
		else
		{
			alarm_bits &= ~OUTSIDE_SENSOR_ERR;
		}
		
		if (dataStore.sensorStatusCounter.pipeSensorErrCounter > ERR_TIMES_LIMIT)
		{
			alarm_bits |= PIPE_SENSOR_ERR;
		}
		else
		{
			alarm_bits &= ~PIPE_SENSOR_ERR;
		}
		
		if (dataStore.sensorStatusCounter.boilerSensorErrCounter > ERR_TIMES_LIMIT)
		{
			alarm_bits |= BOILER_SENSOR_ERR;
		}
		else
		{
			alarm_bits &= ~BOILER_SENSOR_ERR;
		}
		
		if (dataStore.realtimeData.year == 20 && dataStore.realtimeData.month == 8 && dataStore.realtimeData.day == 27)
		{
			alarm_bits |= BAT_RTC_ERR;
		}
		
		dataStore.realtimeData.sensorErrStatus = alarm_bits;
		if (alarm_bits)
		{
			if (beep_status)
			{
				littleAprilGroup3Ctrl(Warning_Group3,On);
				logPrintf(Info,"I:alarm_task.c::alarm_task!Alarm code = %d\r\n",dataStore.realtimeData.sensorErrStatus);
				beep_status = 0x00;
			}
			else
			{
				littleAprilGroup3Ctrl(Warning_Group3,Off);
				beep_status = 0x01;
			}
		}
		else
		{
			littleAprilGroup3Ctrl(Warning_Group3,Off);
		}
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_DLY,&err);
	}
}