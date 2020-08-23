#include "default_config.h"
const SystemOptions systemOptionsDefault = {
	-0.3,//float startHeatingCondition;
	0.5,//float stopHeatingCondition;
	0.6,//float stopColdingCondition;
	0.0,//float deltaTemperature;
	58,//float startHeatingBoilerPipeTemperature;
	57,//float stopHeatingBoilerPipeTemperature;
	55,//float startFanShaftTemperature;
	57,//float stopFanShaftTemperature;
	65,//float startCirculatingPumpTemperature;
	60,//float stopCirculatingPumpTemperature;
	40,//
	60,
	15,//uint16_t sideWindowDefaultAngle;
	15,
	35.0,
	30.0,
	30.0,
	30.0
};
const AlarmThresholdStore alarmThresholdOptionsDefault = {
	1.5,1.0,80.0,40.0,80,48,-10.0
};
const CoolDownGrade coolDownGradeDefault[16] = {
	{1.6,COOL_DOWN_DEFAULT_LEVEL_1},
	{2.5,COOL_DOWN_DEFAULT_LEVEL_2},
	{3.0,COOL_DOWN_DEFAULT_LEVEL_3},
	{3.2,COOL_DOWN_DEFAULT_LEVEL_4},
	{3.6,COOL_DOWN_DEFAULT_LEVEL_5},
	{4.2,COOL_DOWN_DEFAULT_LEVEL_6},
	{4.8,COOL_DOWN_DEFAULT_LEVEL_7},
	{5.5,COOL_DOWN_DEFAULT_LEVEL_8},
	{5.7,COOL_DOWN_DEFAULT_LEVEL_9},
	{5.9,COOL_DOWN_DEFAULT_LEVEL_10},
	{6.1,COOL_DOWN_DEFAULT_LEVEL_11},
	{6.3,COOL_DOWN_DEFAULT_LEVEL_12},
	{6.5,COOL_DOWN_DEFAULT_LEVEL_13},
	{6.7,COOL_DOWN_DEFAULT_LEVEL_14},
	{6.9,COOL_DOWN_DEFAULT_LEVEL_15},
	{7.1,COOL_DOWN_DEFAULT_LEVEL_16}
};

const float ambientTemperatureDefault[50] = {
	34.5,33.8,33.1,32.4,31.7,31.0,30.3,  //-0.5
	29.8,29.6,29.4,29.2,29.0,28.8,28.6,  //-0.2
	28.4,28.2,28.0,27.8,27.6,27.4,27.2,  //-0.2
	26.9,26.6,26.3,26.0,25.7,25.4,25.1,  //-0.4
	24.8,24.5,24.2,23.9,23.6,23.3,23.0,  //-0.4
	22.7,22.4,22.1,21.8,21.5,21.2,20.8,  //-0.4
	20.5,20.2,20.0,20.0,20.0,20.0,20.0,
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

/*const VentilationCoefficient ventilationCoefficientDefault[50] = {
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
};*/
const uint8_t humidityDefault[50];

const VentilateGrade ventilateGradeDefault[5] = {
{0x0040,40,200,0.3},
{0x0040,45,200,0.6},
{0x0040,50,200,0.9},
{0x0040,50,160,1.2},
{0x0040,55,160,1.5}
};
