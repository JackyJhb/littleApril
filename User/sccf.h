/******************************************************************************
Note:
	This part of code contains all the data store reptertory of this project.
The meaning of sccf is System Control Configuration File.
*******************************************************************************/
#ifndef SCCF_H
#define SCCF_H

#include "system.h"
#include "stm32f4xx.h"
#include "24c02.h"
#include "debug_config.h"

#define AT24C128
#define INIT_KEY        0x2222
#define INIT_KEY_FLASH  0x5555

#define ADDR_RTD_FILE   0x0000      //Real time data
#define ADDR_CFG_FILE   0x0400      //Config file data

#define REARING_STARTED 0xA6
#define REARING_STOPPED 0xA8
#define HEATING_STARTED 0x68
#define WIFI_REF_SET    0x18

#define STM32_UNIQUE_ID_SIZE 12

#pragma pack(1)
typedef enum{
	NotFoundESP8266 = 0,
	SettingCWMODE = 1,
	Reseting = 2,
	TurnOffEcho = 3,
	FoundRouter = 4,
	ConnectingToRouter = 5,
	RouterError = 6,
	PasswdError = 7,
	ConnectingToServer = 8,
	ServerConnected = 9,
	ServerDisconnected = 10
}NetWorkStatus;

typedef struct {
	char setByUser;
	char routerName[21];
	char routerPasswd[21];
	char serverIP[21];
	char serverPort[7];
	char mqttUserName[21];
	char mqttPasswd[21];
	char ip[21];
}ESP8266Options;

typedef struct{
	float temperatureDifference;
	uint32_t runningFansBits;
}CoolDownGrade;

typedef struct{
	uint32_t runningFansBits;
}VentilateGrade;

typedef struct{
	float setTemperature;
	float temperatureErr;
	float incrementTemperature;
	float kP;
	float kI;
	float kD;
}PIDParameter;

#ifndef ENABLE_USER_SET
typedef struct {
	uint32_t demarcationDay;
	float   cmfPerMinute;
	float	ventilationCycle;
}VentilationCoefficient;
#else
typedef struct {
	uint32_t runningTime;
	float	ventilationCycle;
	uint8_t grade;
}VentilationCoefficient;
#endif

typedef struct {
	float henhouseVolume;
	float sterePerSecondOfFanRate;
	float fanNumbers;
	uint32_t chickNumbers;
	uint32_t workTimeLimit; //20 + 450 + 20
	#ifndef ENABLE_USER_SET
	VentilationCoefficient ventilationCoefficient[8];
	#else
	VentilationCoefficient ventilationCoefficient[50];
	#endif
	VentilateGrade ventilateGrade[5];
}Ventilation;

typedef struct{
	float henhouseTemperatureHighThreshold;
	float henhouseTemperatureLowThreshold;
	float boilerTemperatureHighThreshold;
	float boilerTemperatureLowThreshold;
	float humidityHighThreshold;
	float humidityLowThreshold;
	float barometricPressureLowThreshold;
}AlarmThresholdStore;

typedef struct {
	float startHeatingCondition;
	float stopHeatingCondition;
	float stopColdingCondition;
	float barometricPressureCondition;
	float startHeatingBoilerTemperature;
	float stopHeatingBoilerTemperature;
	float startFanShaftTemperature;
	float stopFanShaftTemperature;
	float startCirculatingPumpTemperature;
	float stopCirculatingPumpTemperature;
	float runningTimeOfVentilate;
	float stoppedTimeOfVentilate;
	uint16_t sideWindowDefaultAngle;
	
	float waterPumpStartTemperature;
	float waterPumpStopTemperature;
	float waterPumpRunningTime;
	float waterPumpStoppedTime;
}SystemOptions;

typedef struct{
	uint32_t keyCtrlParameter;
	float ambientTemperature[50];
	float illuminationStrength[50];
	uint32_t humidity[50];
	Ventilation ventilation;
	CoolDownGrade coolDownGrade[16];
	PIDParameter pidParameter;
	float waterPumpStartTemperatureDifference;
	AlarmThresholdStore alarmThresholdOptions;
	SystemOptions systemOptions;
	ESP8266Options esp8266Options;
}ControlParameterStore;

typedef struct{
	uint16_t isStarted;
	uint16_t cycleDays;           //
	RTC_DateTypeDef rtcDateStart; //Week Month Date Year
	RTC_TimeTypeDef rtcTimeStart; //Hour Minute Second H12/H24
	uint16_t key;
}RealDataToSave;

typedef struct{
	RealDataToSave realDataToSave;
	uint16_t realSideWindowsAngle[2];
	uint16_t targetSideWindowsAngle;
	uint16_t humidityInside[3];
	float 	 insideTemperature[3][2];
	float    outsideTemperature;
	float    boilerTemperature;
	float 	 illumination[3];
	float    pressureInside;
	uint16_t dayCycle;
	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t hour;
	uint16_t minute;
	uint16_t second;
	float currentSetTemperature;
	float deltaTemperature;
	short deltaActionCycle;
	short deltaActionTimeSpan;
	uint16_t volatageABC[3];
	uint16_t workingVentilators;
	uint16_t heatingColdingStatus;
	uint16_t isSideWindowMotorRunning;
	uint32_t sensorErrStatus;
	uint16_t isColding;
	
	NetWorkStatus netWorkStatus;
	uint8_t stm32UniqueID[STM32_UNIQUE_ID_SIZE];
}RealDataStore;

#ifdef ENABLE_BLACK_BOX
typedef struct{
	uint16_t communicateTimeOutErrTimes[4];
	uint16_t memAllocErrTimes;
	uint16_t memTooLowTimes;
	uint16_t boilerTemperatureTooLowErrTimes;
	uint16_t devIDErrTimes;
	uint16_t sensorErrTimes[3][4];
	uint16_t heatingTimes[3];
	uint16_t coldingTimes[2];
	uint16_t rebootTimes;
	uint16_t requestTimes;
	uint16_t ventilationTimes[16];
}BlackBoxStore;
#endif

typedef struct{
	ControlParameterStore ctrlParameter;
	RealDataStore         realtimeData;
	#ifdef ENABLE_BLACK_BOX
	BlackBoxStore         blackBox;
	#endif
}DataStore;

#pragma pack()
extern DataStore dataStore;
extern uint8_t saveControlParameters(uint32_t *ptr,uint32_t size);
extern void readControlParameters(uint32_t *ptr,uint32_t size);
extern uint8_t sysCtrlConfigFileInit(void);
extern uint8_t sysCtrlConfigFileWrite(RealDataToSave *ptr,uint16_t len);
#endif
