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
	if (which_one == 2)
	{
		average_temperature = 0;
		for(i = 0;i < 6;i++)
		{
			average_temperature += *((float *)dataStore.realtimeData.insideTemperature +i);
		}
		average_temperature /= 6;
		#ifdef ENABLE_OUTPUT_LOG
		logPrintf(Verbose,"V:pid.c::pidControlTemperature()->average temperature is %.2f\r\n",average_temperature);
		#endif
	}
	dataStore.ctrlParameter.pidParameter.setTemperature = set_temperature;
	dataStore.realtimeData.currentSetTemperature = set_temperature;
	#ifdef ENABLE_OUTPUT_LOG
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->%d's area set temperature is %.2f.\r\n",which_one,dataStore.ctrlParameter.pidParameter.setTemperature);
	printf("Info:pid.c::pidControlTemperature()->Real time temperature is %.2f\r\n",actual_temperature);
	#endif
	
	if ((dataStore.ctrlParameter.pidParameter.setTemperature + dataStore.ctrlParameter.systemOptions.startHeatingCondition) >= actual_temperature)
	{
		#ifdef ENABLE_OUTPUT_LOG
		logPrintf(Warning,"W:pid.c::pidControlTemperature()->%d's area's temperature is lower than setting,need to heating.\r\n",which_one);
		logPrintf(Warning,"W:pid.c::pidControlTemperature()->dataStore.realtimeData.boilerTemperature=%.2f.\r\n",dataStore.realtimeData.boilerTemperature);
		logPrintf(Warning,"W:pid.c::pidControlTemperature()->dataStore.ctrlParameter.systemOptions.startHeatingBoilerTemperature=%.2f.\r\n",dataStore.ctrlParameter.systemOptions.startHeatingBoilerTemperature);
		#endif
		if ((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) &&
			(dataStore.realtimeData.boilerTemperature <= dataStore.ctrlParameter.systemOptions.stopHeatingBoilerTemperature))
		{
				littleAprilHCWCtrl((1<<which_one),Off);
				dataStore.realtimeData.heatingColdingStatus &= ~(1<<which_one);
				#ifdef ENABLE_OUTPUT_LOG
				logPrintf(Warning,"W:pid.c::pidControlTemperature()->Boiler's temperature is %.2f,It's lower than setting's value %.2f.Heating paused.\r\n",
				         dataStore.realtimeData.boilerTemperature,
						  dataStore.ctrlParameter.systemOptions.stopHeatingBoilerTemperature);
				#endif
		}
		if (((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) == 0) &&
			(dataStore.realtimeData.boilerTemperature >= dataStore.ctrlParameter.systemOptions.startHeatingBoilerTemperature))
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
		#ifdef ENABLE_OUTPUT_LOG
		logPrintf(Verbose,"V:pid.c::pidControlTemperature()->which_one = %d,dataStore.realtimeData.heatingColdingStatus=%d!dataStore.ctrlParameter.systemOptions.stopHeatingCondition = %.2f\r\n",
				which_one,
				dataStore.realtimeData.heatingColdingStatus,
				dataStore.ctrlParameter.systemOptions.stopHeatingCondition);
		#endif
	}

	if (which_one != 2)
		return;
	#ifdef ENABLE_OUTPUT_LOG
	logPrintf(Verbose,"V:pid.c::pidControlTemperature()->Outside temperature is %.2f,inside temperature is %.2f,threshold of temperature is %.2f.\r\n",
				dataStore.realtimeData.outsideTemperature,
				actual_temperature,
				dataStore.ctrlParameter.waterPumpStartTemperatureDifference);
	#endif
	temperature_difference = average_temperature - dataStore.ctrlParameter.pidParameter.setTemperature;
	if (level_last != -1)
	{
		if (dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference/2 > temperature_difference)
		{
			level_last = -1;
			dataStore.realtimeData.isColding = false;
			dataStore.realtimeData.workingVentilators = 0x0000;
			littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
			return;
		}
	}
	else
	{
		if (dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference > temperature_difference)
			return;
	}
	
	grade_nums = sizeof(dataStore.ctrlParameter.coolDownGrade)/sizeof(CoolDownGrade);
	level = 0;
	for (i = 0;i < (grade_nums - 1);i++)
	{
		if ((temperature_difference >= dataStore.ctrlParameter.coolDownGrade[i].temperatureDifference) && 
			(temperature_difference < dataStore.ctrlParameter.coolDownGrade[i+1].temperatureDifference))
		{
			level = i;
			break;
		}
		if (i == (grade_nums-2))
		{
			level = grade_nums -1;
			#ifdef ENABLE_OUTPUT_LOG
			printf("Info:pid.c::pidControlTemperature()->Cool down grade is %d that is too high,it's terrible!\r\n",level);
			#endif
		}
	}
	#ifdef ENABLE_OUTPUT_LOG
	printf("Info:pid.c::pidControlTemperature()->Calculate cool down grade result is %d!\r\n",level);
	#endif
	if ((level_last > level) && 
		(temperature_difference > 
					(dataStore.ctrlParameter.coolDownGrade[level].temperatureDifference + 
					 dataStore.ctrlParameter.coolDownGrade[level_last].temperatureDifference)/2))
	{
		return ;
	}
	dataStore.realtimeData.workingVentilators = 0x0000;
	cooding_down_fans =  dataStore.ctrlParameter.coolDownGrade[level].runningFansBits;
	dataStore.realtimeData.workingVentilators = cooding_down_fans;
	littleApril16FansCtrl(dataStore.realtimeData.workingVentilators);
	dataStore.realtimeData.isColding = true;
	level_last = level;
}
