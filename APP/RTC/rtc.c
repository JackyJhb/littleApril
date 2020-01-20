#include "rtc.h"
#include "rtc_task.h"
#include "sccf.h"

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
NVIC_InitTypeDef   NVIC_InitStructure;
uint8_t const month_amendBuf[12]={0,3,3,6,1,4,6,2,5,0,3,5};

uint8_t RTC_GetWeek(uint8_t wyear,uint8_t wmonth,uint8_t wday)
{	
	uint16_t middata;
	uint8_t yearL;
	yearL=wyear+100;
	middata=yearL+yearL/4;
	middata=middata%7;
	middata=middata+wday+month_amendBuf[wmonth-1];
	if (yearL%4==0&&wmonth<3)
		middata--;
	if((middata%7==0))
		return 7;
	return(middata%7);
}

ErrorStatus RTC_SetTimes(uint8_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t min,uint8_t sec)
{
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;

	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=RTC_GetWeek(year,month,date);
	//RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;

	if(hour>12)
		RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_PM;
	else 
		RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	
	return ( RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure) && RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure) );
}

void RTC_GetTimes(uint32_t RTC_Format)
{
	RTC_GetDate(RTC_Format,&RTC_DateStruct);
	RTC_GetTime(RTC_Format,&RTC_TimeStruct);
}

uint8_t RTC_Configuration(void)
{
	uint16_t wait=0;
	RTC_InitTypeDef RTC_InitStructure;
	RCC_LSEConfig(RCC_LSE_ON);
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
		wait++;
		delay_ms(5);
		if(wait>=500)
			return 1;
	}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
	RTC_SetTimes(20,1,19,20,29,30);
	return 0;
}

uint8_t RTC_InitConfig(void)
{
	uint8_t i=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)
	{
		i=RTC_Configuration();
		if(i==1)
			return 1;
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);
	}
	RTC_SetWakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);
	return 0;
}

void RTC_SetWakeUp(uint32_t wktime,uint16_t autodata)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_WakeUpCmd(DISABLE);
	RTC_WakeUpClockConfig(wktime);
	RTC_SetWakeUpCounter(autodata);
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	RTC_WakeUpCmd( ENABLE);

	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void RTC_SetAlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;

	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=1;
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay;
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);

	RTC_ClearITPendingBit(RTC_IT_ALRA);
	EXTI_ClearITPendingBit(EXTI_Line17);

	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);

	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void RTC_Alarm_IRQHandler(void)
{
	OSIntEnter(); 	   //进入中断
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);
		if (dataStore.realtimeData.realDataToSave.isStarted == REARING_STARTED)
		{
			++dataStore.realtimeData.dayCycle;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
	OSIntExit(); 	   //进入中断
}

void RTC_WKUP_IRQHandler(void)
{
	OS_ERR err;
	OSIntEnter(); 	   //进入中断
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)
	{
		RTC_ClearFlag(RTC_FLAG_WUTF);
		OSTaskSemPost((OS_TCB  *)&RTCTaskTCB,
							(OS_OPT   )OS_OPT_POST_NONE,
							(OS_ERR  *)&err);
	}
	EXTI_ClearITPendingBit(EXTI_Line22);	
	OSIntExit(); 	   //进入中断
}

void daysOfMonth(uint8_t *year,uint8_t *month,uint8_t *day)
{
	uint8_t nMmax = 12, nDmax = 0;
	switch (*month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			nDmax = 32;
			break;
		case 2:
			if ((*year % 400 == 0) || ((*year % 4 == 0) && (*year % 100 != 0)))
			{
				nDmax = 29;
			}
			else
			{
				nDmax = 28;
			}
			break;
		default:
			nDmax = 30;
			break;
	}
	if ((*day < nDmax) && (*month <= nMmax))
	{
		*day = *day+1;
	}
	else if ((*day == nDmax) && (*month != nMmax))
	{
		*day = 1;
		*month += 1;
	}
	else if ((*day == nDmax) && (*month == nMmax))
	{
		*day = *month = 1;
		*year += 1;
	}
		
}

uint16_t calDaysBettweenTwoDate(RTC_DateTypeDef *date,RTC_TimeTypeDef *time)
{
	uint32_t days = 0,seconds_start,seconds_now;
	RTC_DateTypeDef temp;
	temp.RTC_Date = date->RTC_Date;
	temp.RTC_Month = date->RTC_Month;
	temp.RTC_Year = date->RTC_Year;
	
	while(!((temp.RTC_Date == RTC_DateStruct.RTC_Date) && 
		   (temp.RTC_Month == RTC_DateStruct.RTC_Month) &&
	       (temp.RTC_Year == RTC_DateStruct.RTC_Year)))
	{
		daysOfMonth(&temp.RTC_Year,&temp.RTC_Month,&temp.RTC_Date);
		++days;
		if (days > 50)
		{
			//TODO:The number of the days is beyond the breeding cycle.
		}
	}
	seconds_start = time->RTC_Hours * 3600 + time->RTC_Minutes * 60 + time->RTC_Seconds;
	seconds_now = RTC_TimeStruct.RTC_Hours * 3600 + RTC_TimeStruct.RTC_Minutes * 60 + RTC_TimeStruct.RTC_Seconds;
	
	if (seconds_start < seconds_now)
	{
		++days;
	}
	time->RTC_Seconds += 2;
	return days;
}

uint8_t calHoursInOneDay(RTC_TimeTypeDef *time)
{
	uint32_t miu_start,miu_end,sub;
	uint8_t hours = 0;
	
	miu_start = time->RTC_Hours * 60 + time->RTC_Minutes;
	miu_end = RTC_TimeStruct.RTC_Hours * 60 + RTC_TimeStruct.RTC_Minutes;

	if (miu_end >= miu_start)
	{
		sub = miu_end - miu_start;
	}
	else
	{
		sub = 24 * 60 + miu_end - miu_start;
	}
	hours = sub / 60 + 1;
	if (hours > 24)
	{
		hours = 24;
	}
	return hours;
}
