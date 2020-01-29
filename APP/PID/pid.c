#include "stm32f4xx.h"
#include "pid.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "includes.h"
#include "task_monitor.h"
PIDValue pid[3];

void acquireTemperature(int hour,int minute,int second,PIDValue *pid_ptr)
{
	//static int lastTime[3] = {0};
	static float Minute = 0;
	if (!pid_ptr->lastTimeFlag)
	{
		pid_ptr->lastTime[0] = hour;
		pid_ptr->lastTime[1] = minute;
		pid_ptr->lastTime[2] = second;
		pid_ptr->lastTimeFlag = 1;
	}
	else
	{
		if (pid_ptr->actualTemperature <= pid_ptr->ctrlRef.setTemperature)
		{
			Minute = (hour - pid_ptr->lastTime[0]) * 60 + (minute - pid_ptr->lastTime[1]) - (float)(second-pid_ptr->lastTime[2])/60.0;
			pid_ptr->actualTemperature = pid_ptr->actualTemperature + Minute * pid_ptr->ctrlRef.incrementTemperature;
			pid_ptr->lastTime[0] = hour;
			pid_ptr->lastTime[1] = minute;
			pid_ptr->lastTime[2] = second;
		}
		else
		{
			pid_ptr->lastTime[0] = hour;
			pid_ptr->lastTime[1] = minute;
			pid_ptr->lastTime[2] = second;
		}
	}
}

void initPID(PIDParameter *pid_ptr)
{
	uint8_t i;
	for (i = 0;i < 3;i++)
	{
		pid[i].ctrlRef.kP = pid_ptr->kP;//= 0.2;
		pid[i].ctrlRef.kI = pid_ptr->kI;//  = 0.015;
		pid[i].ctrlRef.kD = pid_ptr->kD;// = 0.2;
		pid[i].ctrlRef.setTemperature = pid_ptr->setTemperature;// = 23.0;
		pid[i].ctrlRef.temperatureErr = pid_ptr->temperatureErr;
		pid[i].lastTimeFlag = 0;
		pid[i].errVal[0] = 0;
		pid[i].errVal[1] = 0;
		pid[i].errVal[2] = 0;
		pid[i].integral = 0;
	}
}

float incrementPIDRealize(float actual_temperature,uint8_t which_one)
{
	float temp0,temp1,temp2,increment;
	pid[which_one].errVal[0] = pid[which_one].ctrlRef.setTemperature -actual_temperature;
	temp0 = pid[which_one].ctrlRef.kP * (pid[which_one].errVal[0] - pid[which_one].errVal[1]);
	temp1 = pid[which_one].ctrlRef.kI * pid[which_one].errVal[0];
	temp2 = pid[which_one].ctrlRef.kD * (pid[which_one].errVal[0] - 2 * pid[which_one].errVal[1] + pid[which_one].errVal[2]);
	increment = temp0 + temp1 + temp2;
	pid[which_one].errVal[1] = pid[which_one].errVal[0];
	pid[which_one].errVal[2] = pid[which_one].errVal[1];
	return increment;
}

void pidControlTemperature(float set_temperature,float actual_temperature,uint8_t which_one)
{
	OS_ERR err;
	uint8_t i,grade_nums=0,level = 0;
	//static uint8_t is_heating[3]={0};
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
		printf("Info:pid.c::pidControlTemperature()->average temperature is %.2f\r\n",average_temperature);
		#endif
	}
	dataStore.ctrlParameter.pidParameter.setTemperature = set_temperature;
	dataStore.realtimeData.currentSetTemperature = set_temperature;
	#ifdef ENABLE_OUTPUT_LOG
	printf("Info:pid.c::pidControlTemperature()->%d's area set temperature is %.2f.\r\n",which_one,dataStore.ctrlParameter.pidParameter.setTemperature);
	printf("Info:pid.c::pidControlTemperature()->Real time temperature is %.2f\r\n",actual_temperature);
	#endif
	
	//If the cooling process is not carried out,it's necessary to determine whether the heating operation is required.
	if (dataStore.realtimeData.isColding==false)
	{
		if ((dataStore.ctrlParameter.pidParameter.setTemperature + dataStore.ctrlParameter.systemOptions.startHeatingCondition) >= actual_temperature)
		{
			#ifdef ENABLE_OUTPUT_LOG
			printf("Warning:pid.c::pidControlTemperature()->%d's area's temperature is lower than setting,need to heating.\r\n",which_one);
			printf("Warning:pid.c::pidControlTemperature()->dataStore.realtimeData.boilerTemperature=%.2f.\r\n",dataStore.realtimeData.boilerTemperature);
			printf("Warning:pid.c::pidControlTemperature()->dataStore.ctrlParameter.systemOptions.startHeatingBoilerTemperature=%.2f.\r\n",dataStore.ctrlParameter.systemOptions.startHeatingBoilerTemperature);
			#endif
			//if (is_heating[which_one] &&
			if ((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) &&
				(dataStore.realtimeData.boilerTemperature <= dataStore.ctrlParameter.systemOptions.stopHeatingBoilerTemperature))
			{
					littleAprilIOCtrl(which_one,Off);
					//is_heating[which_one] = 0;
					dataStore.realtimeData.heatingColdingStatus &= ~(1<<which_one);
					#ifdef ENABLE_OUTPUT_LOG
					printf("Warning:pid.c::pidControlTemperature()->Boiler's temperature is %.2f,It's lower than setting's value %.2f.Heating paused.\r\n",
				           dataStore.realtimeData.boilerTemperature,
						   dataStore.ctrlParameter.systemOptions.stopHeatingBoilerTemperature);
					#endif
			}
			//if ((is_heating[which_one] == 0) &&
			if (((dataStore.realtimeData.heatingColdingStatus & (1<<which_one)) == 0) &&
				(dataStore.realtimeData.boilerTemperature >= dataStore.ctrlParameter.systemOptions.startHeatingBoilerTemperature))
			{
					littleAprilIOCtrl(which_one,On);
					//is_heating[which_one] = 1;
					dataStore.realtimeData.heatingColdingStatus |= (1<<which_one);
					#ifdef ENABLE_OUTPUT_LOG
					printf("Info:pid.c::pidControlTemperature()->Area %d is heating!\r\n",which_one);
					#endif
			}
		}
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:pid.c::pidControlTemperature()->which_one = %d,dataStore.realtimeData.heatingColdingStatus=%d!dataStore.ctrlParameter.systemOptions.startHeatingCondition = %.2f\r\n",
				which_one,
				dataStore.realtimeData.heatingColdingStatus,
				dataStore.ctrlParameter.systemOptions.startHeatingCondition);
		#endif
	}
	else if (which_one == 2)
	{
		if ((average_temperature < (dataStore.ctrlParameter.pidParameter.setTemperature + dataStore.ctrlParameter.systemOptions.stopColdingCondition)) && 
			cooding_down_fans)
		{
			cooding_down_fans = 0x0000;
			dataStore.realtimeData.workingVentilators = cooding_down_fans;
			littleAprilFanCtrl(dataStore.realtimeData.workingVentilators);
			//dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle;
			dataStore.realtimeData.isColding = false;
			#ifdef ENABLE_OUTPUT_LOG
			printf("Info:pid.c::pidControlTemperature()->Colding down is stopped!\r\n");
			#endif
		}
	}
	
	//if ((is_heating[0]==0) && (is_heating[1] == 0) && (is_heating[2] == 0))
	if ((dataStore.realtimeData.heatingColdingStatus & 0x0007) == 0)
	{
		if (which_one != 2)
			return;
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:pid.c::pidControlTemperature()->Outside temperature is %.2f,inside temperature is %.2f,threshold of temperature is %.2f.\r\n",
					dataStore.realtimeData.outsideTemperature,
					actual_temperature,
					dataStore.ctrlParameter.waterPumpStartTemperatureDifference);
		#endif
		temperature_difference = average_temperature - dataStore.ctrlParameter.pidParameter.setTemperature;
		if (dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference > temperature_difference)
			return;
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
		//TODO:Need to remove these part of code after integration side window control function.for_safty 20191229 
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:pid.c::pidControlTemperature()->Calculate cool down grade result is %d!\r\n",level);
		#endif
		if (dataStore.realtimeData.dayCycle >= 20)
		{
			++level;
			if (level >= 1)
				level = 1;
		}
		else 
		{
			if (level >= 1)
				level = 0;
		}
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:pid.c::pidControlTemperature()->Cool down grade is %d,set side windows angle to %d\r\n",level,
						dataStore.ctrlParameter.coolDownGrade[level].sideWindowOpenAngle);
		#endif
		dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.coolDownGrade[level].sideWindowOpenAngle;
		//TODO:According to the different case need to sellect difference side window opened anagle
		//if ((dataStore.realtimeData.realSideWindowsAngle[0] <= (dataStore.realtimeData.targetSideWindowsAngle+2)) &&
			//(dataStore.realtimeData.realSideWindowsAngle[1] <= (dataStore.realtimeData.targetSideWindowsAngle+2)))
		{
			#ifdef ENABLE_OUTPUT_LOG
			printf("Info:pid.c::pidControlTemperature()->Fans working history is %d \r\n",cooding_down_fans);
			#endif
			dataStore.realtimeData.workingVentilators = 0x0000;
			cooding_down_fans =  dataStore.ctrlParameter.coolDownGrade[level].runningFansBits;
			dataStore.realtimeData.workingVentilators = cooding_down_fans;
			littleAprilFanCtrl(dataStore.realtimeData.workingVentilators);
			#ifdef ENABLE_OUTPUT_LOG
			printf("Info:pid.c::pidControlTemperature()->Fans working list:");
			for (i = 0;i < 16;i++)
			{
				if (dataStore.realtimeData.workingVentilators & (1<<i))
				{
					printf("%d ",i);
				}
			}
			printf("\r\n");
			#endif
			dataStore.realtimeData.isColding = true;
		}
	}
	else
	{
		if (((dataStore.ctrlParameter.pidParameter.setTemperature + dataStore.ctrlParameter.systemOptions.stopHeatingCondition) <= actual_temperature) &&
			(dataStore.realtimeData.heatingColdingStatus & (1<<which_one)))
				//is_heating[which_one])
		{
			littleAprilIOCtrl(which_one,Off);
			//is_heating[which_one] = 0;
			dataStore.realtimeData.heatingColdingStatus &= ~(1<<which_one);
			littleAprilIOCtrl(Colding,Off);
			#ifdef ENABLE_OUTPUT_LOG
			printf("Info:pid.c::pidControlTemperature()->Fine tuning.Area %d heating stopped.\r\n",which_one);
			#endif
		}
		#ifdef ENABLE_OUTPUT_LOG
		printf("Info:pid.c::pidControlTemperature()->which_one = %d,dataStore.realtimeData.heatingColdingStatus=%d!dataStore.ctrlParameter.systemOptions.stopHeatingCondition = %.2f\r\n",
				which_one,
				dataStore.realtimeData.heatingColdingStatus,
				dataStore.ctrlParameter.systemOptions.stopHeatingCondition);
		#endif
	}
}
