#include "stm32f4xx.h"
#include "pid.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "includes.h"
#include "task_monitor.h"
#include "circleBuffer.h"

PIDValue pid[3];

void pidControlTemperature(float set_temperature,float actual_temperature,uint8_t which_one)
{
	OS_ERR err;
	int8_t i,grade_nums=0,level = 0;
	static int8_t level_last = -1;
	static uint16_t cooding_down_fans = 0x0000;
	float temperature_difference,average_temperature;
	
	dataStore.ctrlParameter.pidParameter.setTemperature = set_temperature;
	dataStore.realtimeData.currentSetTemperature = set_temperature;
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->%d's area set temperature is %.2f.\r\n",which_one,dataStore.ctrlParameter.pidParameter.setTemperature);
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->Real time temperature is %.2f\r\n",actual_temperature);
	
	if ((dataStore.ctrlParameter.pidParameter.setTemperature + dataStore.ctrlParameter.systemOptions.startHeatingCondition) >= actual_temperature)
	{
		#ifdef ENABLE_OUTPUT_LOG
		logPrintf(Warning,"W:pid.c::pidControlTemperature()->%d's area's temperature is lower than setting,need to heating.\r\n",which_one);
		logPrintf(Warning,"W:pid.c::pidControlTemperature()->dataStore.realtimeData.boilerPipeTemperature=%.2f.\r\n",dataStore.realtimeData.boilerPipeTemperature);
		logPrintf(Warning,"W:pid.c::pidControlTemperature()->dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature=%.2f.\r\n",dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature);
		#endif
		if ((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) &&
			(dataStore.realtimeData.boilerPipeTemperature <= dataStore.ctrlParameter.systemOptions.stopHeatingBoilerPipeTemperature))
		{
				littleAprilHCWCtrl((1<<which_one),Off);
				dataStore.realtimeData.heatingColdingStatus &= ~(1<<which_one);
				#ifdef ENABLE_OUTPUT_LOG
				logPrintf(Warning,"W:pid.c::pidControlTemperature()->Boiler's temperature is %.2f,It's lower than setting's value %.2f.Heating paused.\r\n",
				         dataStore.realtimeData.boilerPipeTemperature,
						  dataStore.ctrlParameter.systemOptions.stopHeatingBoilerPipeTemperature);
				#endif
		}
		if (((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) == 0) &&
			(dataStore.realtimeData.boilerPipeTemperature >= dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature))
		{
				littleAprilHCWCtrl((1<<which_one),On);
				//is_heating[which_one] = 1;
				dataStore.realtimeData.heatingColdingStatus |= (1<<which_one);
				#ifdef ENABLE_OUTPUT_LOG
				logPrintf(Info,"I:pid.c::pidControlTemperature()->Area %d is heating!\r\n",which_one);
				#endif
		}
	}
		
	if ((dataStore.realtimeData.heatingColdingStatus & 0x0007) != 0)
	{
		if (((dataStore.ctrlParameter.pidParameter.setTemperature + dataStore.ctrlParameter.systemOptions.stopHeatingCondition) <= actual_temperature) &&
			(dataStore.realtimeData.heatingColdingStatus & (1<<which_one)))
		{
			littleAprilHCWCtrl((1<<which_one),Off);
			dataStore.realtimeData.heatingColdingStatus &= ~(1<<which_one);
			littleAprilHCWCtrl(Colding,Off);
			#ifdef ENABLE_OUTPUT_LOG
			logPrintf(Verbose,"V:pid.c::pidControlTemperature()->Fine tuning.Area %d heating stopped.\r\n",which_one);
			#endif
		}
		logPrintf(Verbose,"V:pid.c::pidControlTemperature()->which_one = %d,dataStore.realtimeData.heatingColdingStatus=%d!dataStore.ctrlParameter.systemOptions.stopHeatingCondition = %.2f\r\n",
				which_one,
				dataStore.realtimeData.heatingColdingStatus,
				dataStore.ctrlParameter.systemOptions.stopHeatingCondition);
	}

	if (which_one != 2)
		return;

	average_temperature = 0;
	for(i = 0;i < 6;i++)
	{
		average_temperature += *((float *)dataStore.realtimeData.insideTemperature +i);
	}
	average_temperature /= 6;

	temperature_difference = average_temperature - dataStore.ctrlParameter.pidParameter.setTemperature;
	logPrintf(Debug,"D:pid.c::pidControlTemperature()->average temperature is %.2f,set temperature is %.2f,temperature_difference is %.2f,level_last is %d\r\n",
						average_temperature,
						dataStore.ctrlParameter.pidParameter.setTemperature,
						temperature_difference,
						level_last);
	if (level_last != -1)
	{
		if (dataStore.ctrlParameter.pidParameter.setTemperature >= 
				(average_temperature-dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference/2))
		{
			level_last = -1;
			dataStore.realtimeData.isColding = false;
			dataStore.realtimeData.workingVentilators = 0x0000;
			littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
			logPrintf(Debug,"D:pid.c::pidControlTemperature()->Stopped colding!\r\n");
			return;
		}
	}
	else
	{
		if (dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference > temperature_difference)
			return;
	}
	
	grade_nums = sizeof(dataStore.ctrlParameter.coolDownGrade)/sizeof(CoolDownGrade);
	if (temperature_difference <dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference)
	{
		level = 0;
	}
	else
	{
		for (i = 0;i < (grade_nums - 1);i++)
		{
			if ((temperature_difference >= dataStore.ctrlParameter.coolDownGrade[i].temperatureDifference) && 
				(temperature_difference < dataStore.ctrlParameter.coolDownGrade[i+1].temperatureDifference))
			{
				break;
			}
		}
		level = i;
	}
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->Level first is %d\r\n",level);
	if (level_last > level)
	{
		if ((temperature_difference > dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference/2) &&
					(temperature_difference <=((dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference + 
																		dataStore.ctrlParameter.coolDownGrade[1].temperatureDifference)/2)))
		{
			level = 0;
		}
		else if (temperature_difference > 
					(dataStore.ctrlParameter.coolDownGrade[level].temperatureDifference + 
					 dataStore.ctrlParameter.coolDownGrade[level+1].temperatureDifference)/2)
		{
			level += 1;
		}
	}
	logPrintf(Info,"I:pid.c::pidControlTemperature()->Calculate cool down grade result is %d!\r\n",level);
	dataStore.realtimeData.workingVentilators = 0x0000;
	cooding_down_fans =  dataStore.ctrlParameter.coolDownGrade[level].runningFansBits;
	dataStore.realtimeData.workingVentilators = cooding_down_fans;
	littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
	dataStore.realtimeData.isColding = true;
	level_last = level;
}
