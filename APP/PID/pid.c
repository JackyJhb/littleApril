#include "stm32f4xx.h"
#include "pid.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "includes.h"
#include "task_monitor.h"
#include "circleBuffer.h"
#include "task_monitor.h"

void pidControlTemperature(float set_temperature,float actual_temperature,uint8_t which_one)
{
	OS_ERR err;
	int8_t i,grade_nums=0,level = 0;
	static int8_t level_last = -1;
	static uint16_t cooding_down_fans = 0x0000;
	float temperature_difference,average_temperature;

	//dataStore.realtimeData.currentSetTemperature = (set_temperature+dataStore.ctrlParameter.systemOptions.deltaTemperature);
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->%d's area set temperature is %.2f.\r\n",which_one,dataStore.realtimeData.currentSetTemperature);
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->Real time temperature is %.2f\r\n",actual_temperature);
	
	if ((dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.systemOptions.startHeatingCondition) >= actual_temperature)
	{
		if (((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) == 0) &&
			(dataStore.realtimeData.boilerPipeTemperature >= dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature))
		{
				littleAprilHCWCtrl((1<<which_one),On);
				dataStore.realtimeData.heatingColdingStatus |= (1<<which_one);
				logPrintf(Info,"I:pid.c::pidControlTemperature()->Area %d is heating!\r\n",which_one);
		}
	}
		
	if ((dataStore.realtimeData.heatingColdingStatus & 0x0007) != 0)
	{
		if ((((dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.systemOptions.stopHeatingCondition) <= actual_temperature) 
			||
			(dataStore.realtimeData.boilerPipeTemperature <= dataStore.ctrlParameter.systemOptions.stopHeatingBoilerPipeTemperature))
		  &&
		  (dataStore.realtimeData.heatingColdingStatus & (1<<which_one)))
		{
			littleAprilHCWCtrl((1<<which_one),Off);
			dataStore.realtimeData.heatingColdingStatus &= ~(1<<which_one);
			logPrintf(Verbose,"V:pid.c::pidControlTemperature()->Fine tuning.Area %d heating stopped.\r\n",which_one);
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
		average_temperature += dataStore.realtimeData.insideTemperature[i/2][i%2];
	}
	average_temperature /= 6;

	temperature_difference = average_temperature - dataStore.realtimeData.currentSetTemperature;
	logPrintf(Debug,"D:pid.c::pidControlTemperature()->average temperature is %.2f,set temperature is %.2f,temperature_difference is %.2f,level_last is %d\r\n",
						average_temperature,
						dataStore.realtimeData.currentSetTemperature,
						temperature_difference,
						level_last);
	if (level_last != -1)
	{
		if (average_temperature < dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.systemOptions.stopColdingCondition)
		{
			level_last = -1;
			dataStore.realtimeData.isColding &= ~IS_COLDING;
			dataStore.realtimeData.isColding &= ~LEVEL_MASK;
			dataStore.realtimeData.workingVentilators = 0x0000;
			littleApril19FansCtrl(dataStore.realtimeData.workingVentilators,ENVCTRL_TASK_WD);
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
		if ((temperature_difference > (dataStore.realtimeData.currentSetTemperature + dataStore.ctrlParameter.systemOptions.stopColdingCondition)) &&
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
	
	dataStore.realtimeData.workingVentilators = 0x0000;
	cooding_down_fans =  dataStore.ctrlParameter.coolDownGrade[level].runningFansBits;
	dataStore.realtimeData.workingVentilators = cooding_down_fans;
	littleApril19FansCtrl(dataStore.realtimeData.workingVentilators,ENVCTRL_TASK_WD);
	dataStore.realtimeData.isColding |= IS_COLDING;
	dataStore.realtimeData.isColding &= ~LEVEL_MASK;
	dataStore.realtimeData.isColding |= level;
	level_last = level;
	logPrintf(Info,"I:pid.c::pidControlTemperature()->Calculate cool down grade result is %d!isColding=%d\r\n",
											level,
											dataStore.realtimeData.isColding );
}
