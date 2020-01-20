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


#define COOL_DOWN_DEFAULT_LEVEL_1     0x00001//0B000 0000 000  0000 0  1  0x00041;
#define COOL_DOWN_DEFAULT_LEVEL_2     0x00006//0B000 0000 0000 0000 0110
#define COOL_DOWN_DEFAULT_LEVEL_3     0x00007//0B000 0000 0000 0000 0111
#define COOL_DOWN_DEFAULT_LEVEL_4     0x00047//0B000 0001 1000 0001 0110
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

#define HEATING_START_BOILER_DEFAULT        55.0
#define WATER_PUMP_START_TEMP_DIFF_DEFAULT  2.0
#define CHICK_NUMBERS_DEFAULT               30000
#define FAN_NUMBERS_DEFAULT                 19
#define HEN_HOUSE_VOLUME_DEFAULT            (84*16*(3.2+5.9))
#define STERE_PER_SECOND_DEFAULT            (38000/3600)

#define DAY_CYCLE_DEFAULT                   0x00
#define CYCLE_DAYS_DEFAULT                  49
#define START_RTC_YEAR_DEFAULT           	20
#define START_RTC_MONTH_DEFAULT             1
#define START_RTC_DATE_DEFAULT              2
#define START_RTC_WEEKDAY_DEFAULT           4
#define START_RTC_HOURS_DEFAULT             10
#define START_RTC_MINUTES_DEFAULT           41
#define START_RTC_SECONDS_DEFAULT           45

const CoolDownGrade coolDownGradeDefault[16] = {
	{0.5,COOL_DOWN_DEFAULT_LEVEL_1,10},
	{1.0,COOL_DOWN_DEFAULT_LEVEL_2,20},
	{1.5,COOL_DOWN_DEFAULT_LEVEL_3,30},
	{2.0,COOL_DOWN_DEFAULT_LEVEL_4,40},
	{2.5,COOL_DOWN_DEFAULT_LEVEL_5,50},
	{1.8,COOL_DOWN_DEFAULT_LEVEL_6,60},
	{2.1,COOL_DOWN_DEFAULT_LEVEL_7,70},
	{2.4,COOL_DOWN_DEFAULT_LEVEL_8,80},
	{2.7,COOL_DOWN_DEFAULT_LEVEL_9,90},
	{3.0,COOL_DOWN_DEFAULT_LEVEL_10,90},
	{3.3,COOL_DOWN_DEFAULT_LEVEL_11,90},
	{3.6,COOL_DOWN_DEFAULT_LEVEL_12,90},
	{3.9,COOL_DOWN_DEFAULT_LEVEL_13,90},
	{4.2,COOL_DOWN_DEFAULT_LEVEL_14,90},
	{4.5,COOL_DOWN_DEFAULT_LEVEL_15,90},
	{4.8,COOL_DOWN_DEFAULT_LEVEL_16,90}
};

const float ambientTemperatureDefault[50] = {
	35.0,
	35.0,34.3,33.6,32.9,32.2,31.5,30.8,
	30.0,29.8,29.6,29.4,29.2,29.0,28.8,
	28.6,28.3,28.0,27.7,27.4,27.1,26.8,
	26.6,26.3,26.0,25.7,25.4,25.1,24.8,
	24.5,24.2,23.9,23.6,23.3,23.0,22.7,
	22.4,22.1,21.8,21.5,21.4,21.0,21.0,
	21.0,21.0,21.0,21.0,21.0,21.0,21.0
};
const float illuminationStrengthDefault[50] = {
	288,289,290,291,292,288,289,290,291,292,
	288,289,290,291,292,288,289,290,291,292,
	288,289,290,291,292,288,289,290,291,292,
	288,289,290,291,292,288,289,290,291,292,
	288,289,290,291,292,288,289,290,291,292
};
#ifndef ENABLE_USER_SET
const VentilationCoefficient ventilationCoefficientDefault[8] = {
	{7 ,0.00283168,180},
	{14,0.0070792 ,180},
	{21,0.00991088,180},
	{28,0.0141584 ,180},
	{35,0.01840592,180},
	{42,0.01982176,180},
	{49,0.02406928,180},
	{56,0.02406928,180}
}
#else
const VentilationCoefficient ventilationCoefficientDefault[50] = {
	{0,180},//
	{0,300},//0  1
	{10,300},//10
	{15,300},//15
	{20,300},//20
	{25,300},//25
	{30,300},//30
	{35,300},//35
	{40,300},//40
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45
	{45,300},//45 20 days
	{45,240},//45 2\3
	{45,240},//45
	{45,240},//45
	{45,240},//45
	{45,240},//45
	{45,240},//45
	{45,240},//45 27 days
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45 34 days
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180},//45
	{45,180} //45
};
#endif
const uint8_t humidityDefault[50];

const AlarmThresholdStore alarmThresholdStoreDefault = {
	1.0,1.0,80.0,40.0,80,48,-20.0
};

#endif
