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
	char alarm_status,* pMsg,beep_status = 0x00;
	uint32_t alarm_bits = 0x00;
	float average_temperature;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	BEEP_Init();
	enableWatchDog(ALARM_TASK_WD);
	while(1)
	{
		feedWatchDog(ALARM_TASK_WD);
		average_temperature = (dataStore.realtimeData.insideTemperature[0][0] + dataStore.realtimeData.insideTemperature[0][1])/2;
		if (average_temperature > (dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureHighThreshold) || 
			average_temperature < (dataStore.realtimeData.currentSetTemperature - dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureLowThreshold))
		{
			alarm_bits |= AREA1_TEMPERATURE_ALARM;
		}
		else
		{
			alarm_bits &= ~AREA1_TEMPERATURE_ALARM;
		}
		
		average_temperature = (dataStore.realtimeData.insideTemperature[1][0] + dataStore.realtimeData.insideTemperature[1][1])/2;
		if (average_temperature > (dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureHighThreshold) || 
			average_temperature < (dataStore.realtimeData.currentSetTemperature - dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureLowThreshold))
		{
			alarm_bits |= AREA2_TEMPERATURE_ALARM;
		}
		else
		{
			alarm_bits &= ~AREA2_TEMPERATURE_ALARM;
		}
		
		average_temperature = (dataStore.realtimeData.insideTemperature[2][0] + dataStore.realtimeData.insideTemperature[2][1])/2;
		if (average_temperature > (dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureHighThreshold) || 
			average_temperature < (dataStore.realtimeData.currentSetTemperature - dataStore.ctrlParameter.alarmThresholdOptions.henhouseTemperatureLowThreshold))
		{
			alarm_bits |= AREA3_TEMPERATURE_ALARM;
		}
		else
		{
			alarm_bits &= ~AREA3_TEMPERATURE_ALARM;
		}
		
		if ((dataStore.realtimeData.boilerPipeTemperature < dataStore.ctrlParameter.alarmThresholdOptions.boilerTemperatureLowThreshold) || 
			(dataStore.realtimeData.boilerPipeTemperature > dataStore.ctrlParameter.alarmThresholdOptions.boilerTemperatureHighThreshold))
		{
			alarm_bits |= BOILER_TEMPERATURE_ALARM;
		}
		else
		{
			alarm_bits &= ~BOILER_TEMPERATURE_ALARM;
		}
		
		if ((dataStore.realtimeData.humidityInside[1] > dataStore.ctrlParameter.alarmThresholdOptions.humidityHighThreshold) ||
			(dataStore.realtimeData.humidityInside[1] < dataStore.ctrlParameter.alarmThresholdOptions.humidityLowThreshold) )
		{
			alarm_bits |= AREA2_HUMIDITY_ALARM;
		}
		else
		{
			alarm_bits &= ~AREA2_HUMIDITY_ALARM;
		}
		
		#ifdef ENABLE_PIRACY_TRAP
		if ((RTC_DateStruct.RTC_Year > 20) || (RTC_DateStruct.RTC_Month > 7)) 
		{
			alarm_bits |= AREA2_HUMIDITY_ALARM;
		}
		#endif
		
		if (alarm_bits)
		{
			if (beep_status)
			{
				//GPIO_ResetBits(BEEP_Port,BEEP_Pin);
				littleAprilGroup3Ctrl(Warning_Group3,On);
				logPrintf(Info,"I:alarm_task.c::alarm_task!Alarm code = %d\r\n",beep_status);
				beep_status = 0x00;
			}
			else
			{
				//GPIO_SetBits(BEEP_Port,BEEP_Pin);
				littleAprilGroup3Ctrl(Warning_Group3,Off);
				beep_status = 0x01;
			}
		}
		else
		{
			//GPIO_SetBits(BEEP_Port,BEEP_Pin);
			littleAprilGroup3Ctrl(Warning_Group3,Off);
		}
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_DLY,&err);
	}
}