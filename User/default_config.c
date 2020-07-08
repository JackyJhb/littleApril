#include "default_config.h"
const SystemOptions systemOptionsDefault = {
	-0.3,//float startHeatingCondition;
	0.15,//float stopHeatingCondition;
	0.15,//float stopColdingCondition;
	-20,//float barometricPressureCondition;
	58,//float startHeatingBoilerPipeTemperature;
	55,//float stopHeatingBoilerPipeTemperature;
	60,//float startFanShaftTemperature;
	65,//float stopFanShaftTemperature;
	60,//float startCirculatingPumpTemperature;
	55,//float stopCirculatingPumpTemperature;
	70,//
	110,
	15,//uint16_t sideWindowDefaultAngle;
	
	3.0,
	2.0,
	30,
	30
};
const AlarmThresholdStore alarmThresholdOptionsDefault = {
	1.0,1.0,80.0,40.0,80,48,-20.0
};
const CoolDownGrade coolDownGradeDefault[16] = {
	{1.0,COOL_DOWN_DEFAULT_LEVEL_1},
	{1.5,COOL_DOWN_DEFAULT_LEVEL_2},
	{2.0,COOL_DOWN_DEFAULT_LEVEL_3},
	{2.5,COOL_DOWN_DEFAULT_LEVEL_4},
	{3.0,COOL_DOWN_DEFAULT_LEVEL_5},
	{3.5,COOL_DOWN_DEFAULT_LEVEL_6},
	{4.0,COOL_DOWN_DEFAULT_LEVEL_7},
	{4.5,COOL_DOWN_DEFAULT_LEVEL_8},
	{2.7,COOL_DOWN_DEFAULT_LEVEL_9},
	{3.0,COOL_DOWN_DEFAULT_LEVEL_10},
	{3.3,COOL_DOWN_DEFAULT_LEVEL_11},
	{3.6,COOL_DOWN_DEFAULT_LEVEL_12},
	{3.9,COOL_DOWN_DEFAULT_LEVEL_13},
	{4.2,COOL_DOWN_DEFAULT_LEVEL_14},
	{4.5,COOL_DOWN_DEFAULT_LEVEL_15},
	{4.8,COOL_DOWN_DEFAULT_LEVEL_16}
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
const LightingControl illuminationStrengthDefault[50] = {
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},
	{20,360,100},

	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},
	{20,240,80},

	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},
	{20,200,60},

	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},
	{20,160,40},

	{20,60,50},
	{20,60,60},
	{20,60,70},
	{20,60,80},
	{20,60,90},
	{20,60,100},
	{20,60,100},
	{20,60,100},
	{20,60,100},
	{20,60,100},
};

const VentilationCoefficient ventilationCoefficientDefault[50] = {
	{0,180,0},//0  1
	{30,180,0},//1
	{30,180,0},//2
	{50,180,0},//3
	{55,180,0},//4
	{55,180,0},//5
	{55,180,0},//6
	{55,180,0},//7
	{55,180,0},//8
	{60,180,0},//9
	{65,180,0},//10
	{65,180,0},//11
	{60,240,0},//12
	{60,240,0},//13
	{60,240,0},//14
	{60,240,0},//15
	{60,240,0},//16
	{60,240,1},//17
	{60,240,1},//18
	{60,240,1},//19
	{60,240,1},//20
	{60,240,1},//21
	{60,240,1},//22
	{70,220,1},//23
	{70,220,1},//24
	{70,220,1},//25
	{70,220,1},//26 days
	{70,220,1},//45
	{70,220,1},//45
	{70,220,1},//45
	{70,180,1},//45
	{70,180,1},//45
	{70,180,1},//45
	{70,180,1},//45 34 days
	{70,180,2},//45
	{70,180,2},//45
	{70,180,2},//45
	{70,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2},//45
	{45,180,2}//
};
const uint8_t humidityDefault[50];

const VentilateGrade ventilateGradeDefault[5] = {0x01,0x06,0x07,0x17,0x67};
