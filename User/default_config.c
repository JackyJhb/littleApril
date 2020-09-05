#include "default_config.h"

#define COOL_DOWN_DEFAULT_LEVEL_1     0x100C0//0B000 0000 000  0000 0  1  0x00041;
#define COOL_DOWN_DEFAULT_LEVEL_2     0x200C0//0B000 0000 0000 0000 0110
#define COOL_DOWN_DEFAULT_LEVEL_3     0x401C0//0B000 0000 0000 0000 0111
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

const RealDataToSave realDataToSaveDefault = {
	REARING_STARTED,0x0000,5,8,7,20,8,41,45,24,INIT_KEY
};
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

const uint8_t humidityDefault[50];

const VentilateGrade ventilateGradeDefault[5] = {
{0x00020040,30,200,0.3},
{0x00050040,40,200,0.6},
{0x00040040,50,200,0.9},
{0x00030040,50,160,1.2},
{0x00070040,55,160,1.5}
};
/*#define SIDE_WIN_OPEN               1.036
#define SIDE_WIN_CLOSE              3.236
#define OPEN_ABSOLUTE_ANGLE         60
#define CLOSE_ABSOLUTE_ANGLE        0

	kStore.pressureSensor.x0 = 0.0f;
	kStore.pressureSensor.x1 = -30.0f;
	kStore.pressureSensor.y0 = 4.5f;
	kStore.pressureSensor.y1 = 0.5f;*/
const KStore sensorKOptionsDefault = {
	0.0,1.036,60,3.236,0.0,
	0.0,1.036,60,3.236,0.0,
	0.0,4.5,-30.0,0.5,0.0
};

const NegativePressureCtrlAngle negativePressureCtrlAngleDefault = {
	0,10,20,30,40,50,60,60,60,60,60,60,60,60,60,60,16,16,16,
	0,0,  0,  0,  0, 0,  0, 5, 10, 15,20,25,30,35,40,45,45,45,45
};