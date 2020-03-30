#include "default_config.h"
const SystemOptions systemOptionsDefault = {
	-0.3,//float startHeatingCondition;
	0.15,//float stopHeatingCondition;
	0.15,//float stopColdingCondition;
	-20,//float barometricPressureCondition;
	58,//float startHeatingBoilerTemperature;
	55,//float stopHeatingBoilerTemperature;
	60,//float startFanShaftTemperature;
	65,//float stopFanShaftTemperature;
	60,//float startCirculatingPumpTemperature;
	55,//float stopCirculatingPumpTemperature;
	70,//
	110,
	15//uint16_t sideWindowDefaultAngle;
};
const AlarmThresholdStore alarmThresholdOptionsDefault = {
	1.0,1.0,80.0,40.0,80,48,-20.0
};
const CoolDownGrade coolDownGradeDefault[16] = {
	{1.0,COOL_DOWN_DEFAULT_LEVEL_1,30},
	{1.5,COOL_DOWN_DEFAULT_LEVEL_3,15},
	{2.0,COOL_DOWN_DEFAULT_LEVEL_3,60},
	{2.5,COOL_DOWN_DEFAULT_LEVEL_4,40},
	{3.0,COOL_DOWN_DEFAULT_LEVEL_5,50},
	{3.5,COOL_DOWN_DEFAULT_LEVEL_6,60},
	{4.0,COOL_DOWN_DEFAULT_LEVEL_7,70},
	{4.5,COOL_DOWN_DEFAULT_LEVEL_8,80},
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
	34.0,33.5,33.0,32.5,32.0,31.5,31.0,  //-0.5
	30.5,30.3,30.1,29.9,29.7,29.5,29.3,  //-0.2
	29.1,28.9,28.7,28.5,28.3,28.1,27.9,  //-0.2
	27.7,27.3,26.9,26.5,26.1,25.7,25.3,  //-0.4
	24.9,24.5,24.1,23.7,23.3,22.9,22.5,  //-0.4
	22.1,21.7,21.3,20.9,20.5,20.1,20.0,  //-0.4
	20.0,20.0,20.0,20.0,20.0,20.0,20.0,
	20.0
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
	{0,180},//0  1
	{30,180},//1
	{30,180},//2
	{50,180},//3
	{55,180},//4
	{55,180},//5
	{55,180},//6
	{55,180},//7
	{55,180},//8
	{60,180},//9
	{65,180},//10
	{65,180},//11
	{60,240},//12
	{60,240},//13
	{60,240},//14
	{60,240},//15
	{60,240},//16
	{60,240},//17
	{60,240},//18
	{60,240},//19
	{60,240},//20
	{60,240},//21
	{60,240},//22
	{70,220},//23
	{70,220},//24
	{70,220},//25
	{70,220},//26 days
	{70,220},//45
	{70,220},//45
	{70,220},//45
	{70,180},//45
	{70,180},//45
	{70,180},//45
	{70,180},//45 34 days
	{70,180},//45
	{70,180},//45
	{70,180},//45
	{70,180},//45
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
	{45,180}//
};
#endif
const uint8_t humidityDefault[50];
