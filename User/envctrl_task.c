#include "stdlib.h"
#include "envctrl_task.h"
#include "bsp_can.h"
#include "bsp_gpio.h"
#include "bsp_button.h"
#include "pid.h"
#include "pro_wrapper.h"
#include "debug_config.h"
#include "sccf.h"
#include "rtc.h"
#include "task_monitor.h"
#include "ds18b20.h"
#include "adc.h"
#include "circleBuffer.h"

OS_TCB EnvParameterTaskTCB;
CPU_STK EnvParameter_TASK_STK[EnvParameter_STK_SIZE];

void currentTargetTemperature(void);
static void updateTemperatureData(void);
static void temperatureCtrl(uint8_t dev_id);
static void huimidityCtrl(uint8_t dev_id);
static void illuminancyCtrl(uint8_t dev_id);

static float bufTemperature[3][FILTER_TIMES];
static char loopCounter[3] = {0x00,0x00,0x00};

float getMid(float *buf,char len)
{
	char i;
	if (len < 3)
		return 0.0;
	float max = -128.0,min = +168.0,av=0.0;
	for (i = 0;i < len;i++)
	{
		if (*(buf + i) < min)
			min = *(buf + i);
		if (*(buf + i) > max)
			max = *(buf + i);
		av += *(buf +  i);
	}
	av -= max;
	av -= min;
	return av /= (len - 2);
}

void updateTemperatureData(void)
{
	float temp;
	if (ReadTemperature(&temp,CH1))
	{
		if (dataStore.sensorStatusCounter.outsideSensorErrCounter < 100)
		{
			++dataStore.sensorStatusCounter.outsideSensorErrCounter;
		}
		logPrintf(Error,"E:envctrl_task.c::updateTemperatureData()->get outside temperature error!\r\n");
	}
	else
	{
		dataStore.sensorStatusCounter.outsideSensorErrCounter = 0x00;
		if (loopCounter[OUTSIDE] < FILTER_TIMES)
		{
			bufTemperature[OUTSIDE][loopCounter[OUTSIDE]] = temp;
			++loopCounter[OUTSIDE];
			if (loopCounter[OUTSIDE] == FILTER_TIMES)
			{
				dataStore.realtimeData.outsideTemperature = getMid(&bufTemperature[OUTSIDE][0],FILTER_TIMES);
				loopCounter[OUTSIDE] = 0x00;
			}
		}
		else
		{
			loopCounter[OUTSIDE] = 0x00;
		}
	}
	if (ReadTemperature(&temp,CH2))
	{
		if (dataStore.sensorStatusCounter.pipeSensorErrCounter < 100)
		{
			++dataStore.sensorStatusCounter.pipeSensorErrCounter;
		}
		logPrintf(Error,"E:envctrl_task.c::updateTemperatureData()->get pipe temperature error!\r\n");
	}
	else
	{
		dataStore.sensorStatusCounter.pipeSensorErrCounter = 0x00;
		if (loopCounter[BOILER_PIPE] < FILTER_TIMES)
		{
			bufTemperature[BOILER_PIPE][loopCounter[BOILER_PIPE]] = temp;
			++loopCounter[BOILER_PIPE];
			if (loopCounter[BOILER_PIPE] == FILTER_TIMES)
			{
				dataStore.realtimeData.boilerPipeTemperature = getMid(&bufTemperature[BOILER_PIPE][0],FILTER_TIMES);
				loopCounter[BOILER_PIPE] = 0x00;
			}
		}
		else
		{
			loopCounter[BOILER_PIPE] = 0x00;
		}
	}
	if (ReadTemperature(&temp,CH3))
	{
		if (dataStore.sensorStatusCounter.boilerSensorErrCounter < 100)
		{
			++dataStore.sensorStatusCounter.boilerSensorErrCounter;
		}
		logPrintf(Error,"E:envctrl_task.c::updateTemperatureData()->get boiler inside temperature error!\r\n");
	}
	else
	{
		dataStore.sensorStatusCounter.boilerSensorErrCounter = 0x00;
		if (loopCounter[BOILER_INSIDE] < FILTER_TIMES)
		{
			bufTemperature[BOILER_INSIDE][loopCounter[BOILER_INSIDE]] = temp;
			++loopCounter[BOILER_INSIDE];
			if (loopCounter[BOILER_INSIDE] == FILTER_TIMES)
			{
				dataStore.realtimeData.boilerInsideTemperature = getMid(&bufTemperature[BOILER_INSIDE][0],FILTER_TIMES);
				dataStore.realtimeData.boilerInsideTemperature += 7.0f;
				loopCounter[BOILER_INSIDE] = 0x00;
			}
		}
		else
		{
			loopCounter[BOILER_INSIDE] = 0x00;
		}
	}
}

void currentTargetTemperature(void)
{
	float set_temperature;
	uint8_t hours;
	hours = calHoursInOneDay(&dataStore.realtimeData.realDataToSave.rtcTimeStart);
	if (dataStore.realtimeData.dayCycle < 50)
	{
			set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle-1] - dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle];
			set_temperature = (set_temperature/24) * hours;
			set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle-1] - set_temperature;
			logPrintf(Verbose,"V:envctrl_task.c::temperatureCtrl()->hours:%d,base_temperature:%.2f,target_temperature:%.2f,set_temperature:%.2f\r\n",
						hours,
						dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle - 1],
						dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle],
						set_temperature);
	}
	dataStore.realtimeData.currentSetTemperature = (set_temperature+dataStore.ctrlParameter.systemOptions.deltaTemperature);
}

void temperatureCtrl(uint8_t dev_id)
{
	float temp=0.0f/*,set_temperature*/;
	uint8_t valid_nums = 0/*,hours*/;
	//CPU_SR_ALLOC();
			
	if (dataStore.realtimeData.insideTemperature[dev_id][0] != INVIAL)
	{
		temp = dataStore.realtimeData.insideTemperature[dev_id][0];
		++valid_nums;
	}
	if (dataStore.realtimeData.insideTemperature[dev_id][1] != INVIAL)
	{
		temp += dataStore.realtimeData.insideTemperature[dev_id][1];
		++valid_nums;
	}
	if (valid_nums != 0)
	{
		temp /= valid_nums;
		if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
		{
			pidControlTemperature(dataStore.realtimeData.currentSetTemperature,temp,dev_id);
		}
		else if (dataStore.realtimeData.realDataToSave.isStarted == HEATING_STARTED)
		{
			dataStore.realtimeData.currentSetTemperature = (dataStore.ctrlParameter.ambientTemperature[0]+dataStore.ctrlParameter.systemOptions.deltaTemperature);
			pidControlTemperature(dataStore.realtimeData.currentSetTemperature,temp,dev_id);
		}
	}
	else
	{
		//TODO: Both of DS18B20 don't work anymore!It's a criticl error!
		logPrintf(Error,"E:envctrl_task.c::temperatureCtrl()->Dev_id = %d ,DS18B20 destoried!\r\n",dev_id);
	}	
}

void huimidityCtrl(uint8_t dev_id)
{
	float ave_temperature = 0;
	if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
	{
		switch (dev_id)
		{
			case 0x00:
			case 0x01:
			case 0x02:
				break;
			default:
				break;
		}
	}
}

void illuminancyCtrl(uint8_t dev_id)
{
	if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
	{
		switch (dev_id)
		{
			case 0x00:
				break;
			case 0x01:
				break;
			case 0x02:
				break;
			default:
				break;
		}
	}
}

void EnvParameter_task(void *p_arg)
{
	OS_ERR          err;
	CPU_TS_TMR      ts_int;
	CPU_INT32U      cpu_clk_freq;
	OS_MSG_SIZE     msg_size;
	char * pMsg,ask_status = SERVER_REQ_TEMPERATURE,ask_dev_id = 0x00,ch0_err = 0,ch1_err = 0,counter_left = 0,counter_right=0;
	ServerOrder *order_ptr;
	unsigned char buf[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uint32_t sequenceID = 0x00000000;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	cpu_clk_freq = BSP_CPU_ClkFreq();    
	CAN1_Init();
	order_ptr = (ServerOrder *)buf;
	enableWatchDog(ENVCTRL_TASK_WD);
	dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle;
	while(1)
	{
		if (((OS_MEM      *)&mymem)->NbrFree > 15)
		{
			logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->device id = %d\r\n",ask_dev_id);
			feedWatchDog(ENVCTRL_TASK_WD);
		}
		else
		{
			logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->mymem is too low NbrFree = %d\r\n",((OS_MEM      *)&mymem)->NbrFree);
		}
		if (ask_dev_id == 0x03)
		{
			logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->System days of cycle:%d\r\n",dataStore.realtimeData.dayCycle);
			logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->20%d.%d.%d--->",
								RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
			logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->%d:%d:%d\r\n",
								RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
			logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->System start date:%d year %d month %d day\r\n",
				dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_Year,
				dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_Month,
				dataStore.realtimeData.realDataToSave.rtcDateStart.RTC_Date);
			logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->System start time:%d hour %d minute %d second\r\n",
				dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Hours,
				dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Minutes,
				dataStore.realtimeData.realDataToSave.rtcTimeStart.RTC_Seconds);
			++sequenceID;
			currentTargetTemperature();
			OS_CRITICAL_ENTER();
			updateTemperatureData();
			OS_CRITICAL_EXIT();
			ask_dev_id = 0x00;
		}
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_DLY,&err);
		feedWatchDog(ENVCTRL_TASK_WD);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
		feedWatchDog(ENVCTRL_TASK_WD);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_DLY,&err);
		feedWatchDog(ENVCTRL_TASK_WD);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
		feedWatchDog(ENVCTRL_TASK_WD);
		order_ptr->type = ask_status;
		pMsg = OSTaskQPend ((OS_TICK        )0,
							(OS_OPT         )OS_OPT_PEND_NON_BLOCKING,
							(OS_MSG_SIZE   *)&msg_size,
							(CPU_TS        *)0,
							(OS_ERR       *)&err);
		if (pMsg == NULL)
		{
			CAN1_Send_Msg(ENV_PAR_RECEIVE_ID|ask_dev_id,buf,sizeof(ServerOrder),10);
			logPrintf(Debug,"D:envctrl_task.c::EnvParameter_task()->Ask for temperature dev_id=%d\r\n",ask_dev_id);
			pMsg = OSTaskQPend ((OS_TICK        )3000,
							(OS_OPT         )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE   *)&msg_size,
							(CPU_TS        *)0,
							(OS_ERR       *)&err);
		}
		
		if (err != OS_ERR_NONE)
		{
			logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->QSTaskQPend() Dev_id:%d , wait for requests temperature respond err = %d ",ask_dev_id,err);
			#ifdef ENABLE_BLACK_BOX
			if (err == OS_ERR_TIMEOUT)
			{
				++dataStore.blackBox.communicateTimeOutErrTimes[ask_dev_id];
				if (dataStore.sensorStatusCounter.clientDeviceErrCounter[ask_dev_id] < 100)
				{
					++dataStore.sensorStatusCounter.clientDeviceErrCounter[ask_dev_id];
				}
			}
			else
			{
				++dataStore.blackBox.memAllocErrTimes;
			}
			#endif
		}

		if (pMsg)
		{
			if (((DataPackage *)pMsg)->dev_id > 0x02)
			{
				logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->CAN receive data->unknown dev_id = %d",((DataPackage *)pMsg)->dev_id);
			}
			else
			{
				dataStore.sensorStatusCounter.clientDeviceErrCounter[ask_dev_id] = 0x00;
				switch (((DataPackage *)pMsg)->type)
				{
					case SERVER_REQ_TEMPERATURE:
						//Filter temperature value that out of range or not normal.
						if ((((((DataPackage *)pMsg)->err & DS18B20_LEFT_ERR) != DS18B20_LEFT_ERR) && 
							((float)((DataPackage *)pMsg)->leftTemperature/100 > 10.0) &&
							((float)((DataPackage *)pMsg)->leftTemperature/100 < 50.0)) || (counter_left >= 3)) {
							counter_left = 0;
							dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][0] = (float)((DataPackage *)pMsg)->leftTemperature/100;
							dataStore.sensorStatusCounter.areaLeftSensorErrCounter[(((DataPackage *)pMsg)->dev_id)] = 0x00;
							ch0_err = 0;
						} else {
							if ((((DataPackage *)pMsg)->err & DS18B20_LEFT_ERR) != DS18B20_LEFT_ERR) {
								++counter_left;
							}
							#ifdef ENABLE_BLACK_BOX
							++dataStore.blackBox.sensorErrTimes[((DataPackage *)pMsg)->dev_id][0];
							#endif
							ch0_err = 1;
							if (dataStore.sensorStatusCounter.areaLeftSensorErrCounter[(((DataPackage *)pMsg)->dev_id)] < 100) {
								++dataStore.sensorStatusCounter.areaLeftSensorErrCounter[(((DataPackage *)pMsg)->dev_id)];
							}
						}
						if ((((((DataPackage *)pMsg)->err & DS18B20_RIGHT_ERR) != DS18B20_RIGHT_ERR) && 
							((float)((DataPackage *)pMsg)->rightTemperature/100 > 10.0) &&
							((float)((DataPackage *)pMsg)->rightTemperature/100 < 50.0)) || (counter_right >= 3)) {
							counter_right = 0;
							dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][1] = (float)((DataPackage *)pMsg)->rightTemperature/100;
							dataStore.sensorStatusCounter.areaRightSensorErrCounter[(((DataPackage *)pMsg)->dev_id)] = 0x00;
							ch1_err = 0;
						} else {
							if ((((DataPackage *)pMsg)->err & DS18B20_RIGHT_ERR) != DS18B20_RIGHT_ERR) {
								++counter_right;
							}
							#ifdef ENABLE_BLACK_BOX
							++dataStore.blackBox.sensorErrTimes[((DataPackage *)pMsg)->dev_id][1];
							#endif
							if (dataStore.sensorStatusCounter.areaRightSensorErrCounter[(((DataPackage *)pMsg)->dev_id)] < 100)
							{
								++dataStore.sensorStatusCounter.areaRightSensorErrCounter[(((DataPackage *)pMsg)->dev_id)];
							}
							ch1_err = 1;
						}
						if ((ch0_err == 1) && (ch1_err == 0))
						{
							dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][0]  = 
													dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][1] ;
						}
						if ((ch0_err == 0) && (ch1_err == 1))
						{
							dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][1]  = 
													dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][0] ;
						}
						
						if (sequenceID > 10)
						{
							//Do temperaure control action after get more than 10 times client devices datas.
							temperatureCtrl(((DataPackage *)pMsg)->dev_id);
						}
								
						if ((((DataPackage *)pMsg)->err & HUMIDITY_SENSOR_OUTSIDE_ERR) != HUMIDITY_SENSOR_OUTSIDE_ERR)
						{
							dataStore.realtimeData.humidityInside[((DataPackage *)pMsg)->dev_id] = ((DataPackage *)pMsg)->humidity;
						}
						else
						{
							dataStore.realtimeData.humidityInside[((DataPackage *)pMsg)->dev_id] = INVIAL;
							#ifdef ENABLE_BLACK_BOX
							++dataStore.blackBox.sensorErrTimes[((DataPackage *)pMsg)->dev_id][2];
							#endif
						}
						huimidityCtrl(((DataPackage *)pMsg)->dev_id);

						logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->CAN receive data::");
						logPrintf(Verbose,"V:dev_id = %d,tempCH0 = %.2f,tempCH1 = %.2f,humidity = %d,sequenceID = %ld\r\n",((DataPackage *)pMsg)->dev_id,
								(float)((DataPackage *)pMsg)->leftTemperature/100,
								(float)((DataPackage *)pMsg)->rightTemperature/100,
								sequenceID,
								dataStore.realtimeData.humidityInside[((DataPackage *)pMsg)->dev_id]);
						logPrintf(Verbose,"V:using temperature:CH0 =%.2f,CH1 = %.2f\r\n",
								dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][0],
								dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][1]);
						ask_status = SERVER_REQ_TEMPERATURE;
						break;
					case SERVER_REQ_ILLUMINANCY:
						logPrintf (Verbose,"D:envctrl_task.c::EnvParameter_task()->CAN receive data::\r\n");
						//TODO
						if ((((DataPackage *)pMsg)->err & ILLUMINANCY_SENSOR_ERR) != ILLUMINANCY_SENSOR_ERR)
						{
							illuminancyCtrl(ask_dev_id);
						}
						else
						{
							//TODO
							#ifdef ENABLE_BLACK_BOX
							++dataStore.blackBox.sensorErrTimes[((DataPackage *)pMsg)->dev_id][3];
							#endif
						}
						ask_status = SERVER_REQ_TEMPERATURE;
						break;
					default:
						break;
				}
			}
			OSMemPut ((OS_MEM  *)&mymem,(void    *)pMsg,(OS_ERR  *)&err);
			if (err != OS_ERR_NONE)
			{
				logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->OSMemPut() err = %d\r\n",err);
			}
		}
		++ask_dev_id;
	}
}
