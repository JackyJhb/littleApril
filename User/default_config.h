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


#define COOL_DOWN_DEFAULT_LEVEL_1     0x0C0C0//0x00040//0B000 0000 000  0000 0  1  0x00041;
#define COOL_DOWN_DEFAULT_LEVEL_2     0x0C0C0//0B000 0000 0000 0000 0110
#define COOL_DOWN_DEFAULT_LEVEL_3     0x040C0//0B000 0000 0000 0000 0111
#define COOL_DOWN_DEFAULT_LEVEL_4     0x0C0C0//0B000 0001 1000 0001 0110
#define COOL_DOWN_DEFAULT_LEVEL_5     0x000C7//0B010 0001 1000 0001 1000
#define COOL_DOWN_DEFAULT_LEVEL_6     0x03C18//0B000 0011 1100 0001 1000
#define COOL_DOWN_DEFAULT_LEVEL_7     0x23C18//0B010 0011 1100 0001 1000
#define COOL_DOWN_DEFAULT_LEVEL_8     0x03C3C//0B000 0011 1100 0011 1100
#define COOL_DOWN_DEFAULT_LEVEL_9     0x23C3C//0B010 0011 1100 0011 1100
#define COOL_DOWN_DEFAULT_LEVEL_10    0x07E3C//0B000 0111 1110 0011 1100
#define COOL_DOWN_DEFAULT_LEVEL_11    0x27E3C//0B010 0111 1110 0011 1100
#define COOL_DOWN_DEFAULT_LEVEL_12    0x07E7E//0B000 0111 1110 0111 1110
#define COOL_DOWN_DEFAULT_LEVEL_13    0x27E7E//0B010 0111 1110 0111 1110
#define COOL_DOWN_DEFAULT_LEVEL_14    0x0FF7E//0B000 1111 1111 0111 1110
#define COOL_DOWN_DEFAULT_LEVEL_15    0x2FF7E//0B010 1111 1111 0111 1110
#define COOL_DOWN_DEFAULT_LEVEL_16    0x7FFFF//0B111 1111 1111 1111 1111

#define WATER_PUMP_START_TEMP_DIFF_DEFAULT  3.0
#define CHICK_NUMBERS_DEFAULT               30000
#define FAN_NUMBERS_DEFAULT                 19
#define HEN_HOUSE_VOLUME_DEFAULT            (84*16*(3.2+5.9))
#define STERE_PER_SECOND_DEFAULT            (38000/3600)

#define DAY_CYCLE_DEFAULT                   0x00
#define CYCLE_DAYS_DEFAULT                  49
#define START_RTC_YEAR_DEFAULT           		20
#define START_RTC_MONTH_DEFAULT             4
#define START_RTC_DATE_DEFAULT              21
#define START_RTC_WEEKDAY_DEFAULT           2
#define START_RTC_HOURS_DEFAULT             6
#define START_RTC_MINUTES_DEFAULT           00
#define START_RTC_SECONDS_DEFAULT           00

extern const CoolDownGrade coolDownGradeDefault[16];
extern const float ambientTemperatureDefault[50];
extern const float illuminationStrengthDefault[50];
#ifndef ENABLE_USER_SET
extern const VentilationCoefficient ventilationCoefficientDefault[8];
#else
extern const VentilationCoefficient ventilationCoefficientDefault[50] ;
#endif
extern const uint8_t humidityDefault[50];

extern const AlarmThresholdStore alarmThresholdOptionsDefault;
extern const SystemOptions systemOptionsDefault;

#endif
