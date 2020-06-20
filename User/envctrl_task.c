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

static void updateData(void);
static void temperatureCtrl(uint8_t dev_id);
static void huimidityCtrl(uint8_t dev_id);
static void illuminancyCtrl(uint8_t dev_id);

void updateData(void)
{
	if (ReadTemperature(&dataStore.realtimeData.outsideTemperature,CH1))
	{
		logPrintf(Error,"E:envctrl_task.c::updateData()->get outside temperature error!\r\n");
	}
	if (ReadTemperature(&dataStore.realtimeData.boilerPipeTemperature,CH2))
	{
		logPrintf(Error,"E:envctrl_task.c::updateData()->get boiler temperature error!\r\n");
	}
}

void temperatureCtrl(uint8_t dev_id)
{
	float temp=0.0f,set_temperature;
	uint8_t valid_nums = 0,hours;
	CPU_SR_ALLOC();
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
		//This function that called is main PID control function.
		if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
		{
			hours = calHoursInOneDay(&dataStore.realtimeData.realDataToSave.rtcTimeStart);
			if (dataStore.realtimeData.dayCycle > 0)
			{
					set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle - 1] - dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle];
					set_temperature = (set_temperature/24) * hours;
					set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle - 1] - set_temperature;
					logPrintf(Verbose,"V:envctrl_task.c::temperatureCtrl()->hours:%d,base_temperature:%.2f,target_temperature:%.2f,set_temperature:%.2f\r\n",
								hours,
								dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle - 1],
								dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle],
								set_temperature);
			}
			else
			{
				set_temperature = dataStore.ctrlParameter.ambientTemperature[dataStore.realtimeData.dayCycle];
			}
			pidControlTemperature((set_temperature+dataStore.realtimeData.deltaTemperature),temp,dev_id);
		}
		else if (dataStore.realtimeData.realDataToSave.isStarted == HEATING_STARTED)
		{
			pidControlTemperature(dataStore.ctrlParameter.ambientTemperature[0],temp,dev_id);
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
	char * pMsg,ask_status = SERVER_REQ_TEMPERATURE,ask_dev_id = 0x00;
	ServerOrder *order_ptr;
	#ifdef ENABLE_OUTPUT_LOG
	unsigned int can_err_times = 0;
	#endif
	unsigned char buf[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	CPU_SR_ALLOC();
	p_arg = p_arg;
	cpu_clk_freq = BSP_CPU_ClkFreq();    
	CAN1_Init();
	order_ptr = (ServerOrder *)buf;
	enableWatchDog(ENVCTRL_TASK_WD);
	#ifdef ENABLE_PIRACY_TRAP
	if ((RTC_DateStruct.RTC_Year > 20) || (RTC_DateStruct.RTC_Month > 7)) 
	{
		while(1);
	}
	#endif
	dataStore.realtimeData.targetSideWindowsAngle = dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle;
	while(1)
	{
		if (((OS_MEM      *)&mymem)->NbrFree > 15)
		{
			feedWatchDog(ENVCTRL_TASK_WD);
		}
		if (ask_dev_id == 0x03)
		{
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
			logPrintf(Verbose,"V:envctrl_task.c::EnvParameter_task()->System days of cycle:%d\r\n",dataStore.realtimeData.dayCycle);
			OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
			ask_dev_id = 0x00;
		}
		OS_CRITICAL_ENTER();
		updateData();
		OS_CRITICAL_EXIT();
		order_ptr->type = ask_status;
		CAN1_Send_Msg(ENV_PAR_RECEIVE_ID|ask_dev_id,buf,sizeof(ServerOrder),0);
		logPrintf(Debug,"D:envctrl_task.c::EnvParameter_task()->Ask for temperature dev_id=%d\r\n",ask_dev_id);
		pMsg = OSTaskQPend ((OS_TICK        )3000,
							(OS_OPT         )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE   *)&msg_size,
							(CPU_TS        *)0,
							(OS_ERR       *)&err);
		if (err != OS_ERR_NONE)
		{
			logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->QSTaskQPend() Dev_id:%d , wait for requests temperature respond err = %d ",ask_dev_id,err);
			#ifdef ENABLE_BLACK_BOX
			if (err == OS_ERR_TIMEOUT)
			{
				++dataStore.blackBox.communicateTimeOutErrTimes[ask_dev_id];
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
				switch (((DataPackage *)pMsg)->type)
				{
					case SERVER_REQ_TEMPERATURE:
						#ifdef ENABLE_BLACK_BOX
						if (((DataPackage *)pMsg)->dev_id > 0x02)
						{
							++dataStore.blackBox.devIDErrTimes;
							break;
						}
						#endif
						dataStore.realtimeData.sensorErrStatus = ((DataPackage *)pMsg)->err;
						if (((((DataPackage *)pMsg)->err & DS18B20_LEFT_ERR) != DS18B20_LEFT_ERR) && 
							((float)((DataPackage *)pMsg)->leftTemperature/100 > -20.0) &&
							((float)((DataPackage *)pMsg)->leftTemperature/100 < 50.0))
						{
							dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][0] = (float)((DataPackage *)pMsg)->leftTemperature/100;
						}
						else
						{
							#ifdef ENABLE_BLACK_BOX
							++dataStore.blackBox.sensorErrTimes[((DataPackage *)pMsg)->dev_id][0];
							#endif
						}
						if (((((DataPackage *)pMsg)->err & DS18B20_RIGHT_ERR) != DS18B20_RIGHT_ERR) && 
							((float)((DataPackage *)pMsg)->rightTemperature/100 > -20.0) &&
							((float)((DataPackage *)pMsg)->rightTemperature/100 < 50.0))
						{
							dataStore.realtimeData.insideTemperature[(((DataPackage *)pMsg)->dev_id)][1] = (float)((DataPackage *)pMsg)->rightTemperature/100;
						}
						else
						{
							#ifdef ENABLE_BLACK_BOX
							++dataStore.blackBox.sensorErrTimes[((DataPackage *)pMsg)->dev_id][1];
							#endif
						}
						temperatureCtrl(((DataPackage *)pMsg)->dev_id);
								
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

						logPrintf(Verbose,"D:envctrl_task.c::EnvParameter_task()->CAN receive data::");
						logPrintf(Verbose,"dev_id = %d,tempCH0 = %f,tempCH1 = %f,humidity = %d\r\n",((DataPackage *)pMsg)->dev_id,
								dataStore.realtimeData.insideTemperature[((DataPackage *)pMsg)->dev_id][0],
								dataStore.realtimeData.insideTemperature[((DataPackage *)pMsg)->dev_id][1],
								dataStore.realtimeData.humidityInside[((DataPackage *)pMsg)->dev_id]);
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
			OSMemPut ((OS_MEM  *)&mymem,
					(void    *)pMsg,
					(OS_ERR  *)&err);
			if (err != OS_ERR_NONE)
			{
				logPrintf(Error,"E:envctrl_task.c::EnvParameter_task()->OSMemPut() err = %d\r\n",err);
			}
		}
		++ask_dev_id;
	}
}
