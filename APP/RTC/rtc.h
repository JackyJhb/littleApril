#ifndef __RTC_H
#define __RTC_H	 
#include "stm32f4xx.h" 
#include "includes.h"


//////////////////////////////////////////////////////////////////////////////////	 

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
	
uint8_t RTC_InitConfig(void);						//RTC��ʼ��

uint8_t RTC_GetWeek(uint8_t wyear,uint8_t wmonth,uint8_t wday);
ErrorStatus RTC_SetTimes(uint8_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t min,uint8_t sec);
void RTC_GetTimes(uint32_t RTC_Format);

void RTC_SetAlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec);	//��������ʱ��(����������,24Сʱ��)
void RTC_SetWakeUp(uint32_t wktime,uint16_t autodata);				//���û��Ѷ�ʱ����ʱ��
uint16_t calDaysBettweenTwoDate(RTC_DateTypeDef *date,RTC_TimeTypeDef *time);
uint8_t calHoursInOneDay(RTC_TimeTypeDef *time);

#endif

















