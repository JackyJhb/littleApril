/******************************************************************************
Note:
	This part of code contains all the data store reptertory of this project.
The meaning of sccf is System Control Configuration File.
*******************************************************************************/
#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H

#include "system.h"
#include "stm32f4xx.h"
#include "24c02.h"
#include "sccf.h"

/*#define WATER_PUMP_START_TEMP_DIFF_DEFAULT  3.0
#define CHICK_NUMBERS_DEFAULT               30000
#define FAN_NUMBERS_DEFAULT                 19
#define HEN_HOUSE_VOLUME_DEFAULT            (84*16*(3.2+5.9))
#define STERE_PER_SECOND_DEFAULT            (38000/3600)

#define DAY_CYCLE_DEFAULT                   0x00
#define CYCLE_DAYS_DEFAULT                  50
#define START_RTC_YEAR_DEFAULT           		20
#define START_RTC_MONTH_DEFAULT             8
#define START_RTC_DATE_DEFAULT              7
#define START_RTC_WEEKDAY_DEFAULT          5
#define START_RTC_HOURS_DEFAULT             8
#define START_RTC_MINUTES_DEFAULT           41
#define START_RTC_SECONDS_DEFAULT           45*/

extern const RealDataToSave realDataToSaveDefault;
extern const CoolDownGrade coolDownGradeDefault[16];
extern const float ambientTemperatureDefault[50];
extern const LightingControl illuminationStrengthDefault[50];
extern const uint8_t humidityDefault[50];
extern const VentilateGrade ventilateGradeDefault[5];

extern const AlarmThresholdStore alarmThresholdOptionsDefault;
extern const SystemOptions systemOptionsDefault;
extern const KStore sensorKOptionsDefault;
extern const NegativePressureCtrlAngle negativePressureCtrlAngleDefault;

#endif
