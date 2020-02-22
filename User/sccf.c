#include "sccf.h"
#include "string.h"
#include "24c02.h"
#include "stm32_flash.h"
#include "default_config.h"
//86x8x(3.2+5.9) = 6260.8m^3
//10.56m^3 ��ֻ����
//������ȫ����19ֻ����ȫ������30s���

//
DataStore dataStore;
uint8_t sysCtrlConfigFileWrite(RealDataToSave *ptr,uint16_t len);
uint8_t saveControlParameters(uint32_t *ptr,uint32_t size);
void readControlParameters(uint32_t *ptr,uint32_t size);
uint8_t setControlParametersToDefault(void);


uint8_t saveControlParameters(uint32_t *ptr,uint32_t size)
{
	int res;
	char i = 0;
	ControlParameterStore temp;
	do
	{
		stm32FlashWrite(ADDR_FLASH_SECTOR_11,(uint32_t *)&dataStore.ctrlParameter,size);
		stm32FlashRead(ADDR_FLASH_SECTOR_11,(uint32_t *)&temp,size);
		res = memcmp(&temp,&dataStore.ctrlParameter,sizeof(ControlParameterStore));
		if (++i>5)
		{
			return 1;
		}
	}while(res != 0);
	return 0;
}

void readControlParameters(uint32_t *ptr,uint32_t size)
{
	stm32FlashRead(ADDR_FLASH_SECTOR_11,ptr,size);
}

uint8_t setControlParametersToDefault(void)
{
	memcpy(dataStore.ctrlParameter.ambientTemperature,ambientTemperatureDefault,sizeof(ambientTemperatureDefault));
	memcpy(dataStore.ctrlParameter.humidity,humidityDefault,sizeof(humidityDefault));
	memcpy(dataStore.ctrlParameter.illuminationStrength,illuminationStrengthDefault,sizeof(illuminationStrengthDefault));
	memcpy(dataStore.ctrlParameter.ventilation.ventilationCoefficient,ventilationCoefficientDefault,sizeof(ventilationCoefficientDefault));
	memcpy(dataStore.ctrlParameter.coolDownGrade,coolDownGradeDefault,sizeof(coolDownGradeDefault));
	memcpy(&dataStore.ctrlParameter.alarmThresholdOptions,&alarmThresholdOptionsDefault,sizeof(AlarmThresholdStore));
	memcpy(&dataStore.ctrlParameter.systemOptions,&systemOptionsDefault,sizeof(SystemOptions));
	
	dataStore.ctrlParameter.waterPumpStartTemperatureDifference = WATER_PUMP_START_TEMP_DIFF_DEFAULT;
	dataStore.ctrlParameter.ventilation.chickNumbers = CHICK_NUMBERS_DEFAULT;
	dataStore.ctrlParameter.ventilation.fanNumbers = FAN_NUMBERS_DEFAULT;
	dataStore.ctrlParameter.ventilation.henhouseVolume = HEN_HOUSE_VOLUME_DEFAULT;
	dataStore.ctrlParameter.ventilation.sterePerSecondOfFanRate = STERE_PER_SECOND_DEFAULT;
	dataStore.ctrlParameter.keyCtrlParameter = INIT_KEY_FLASH;

	AT24C02_Write(ADDR_CFG_FILE,(u8 *)&dataStore.ctrlParameter,sizeof(dataStore.ctrlParameter));
	return 0;
}

uint8_t writeCtrlConfigFile(void * ptr,unsigned int size)
{
	AT24C02_Write(ADDR_RTD_FILE,(u8 *)ptr,size);
	return 0;
}

uint8_t readCtrlConfigFile(void *ptr,unsigned int size)
{
	uint8_t temp;
	
	AT24C02_Read(ADDR_RTD_FILE,(u8 *)ptr,size);
	//*
	AT24C02_Read(254,&temp,sizeof(uint8_t));
	if (temp == 0x89)
	{
		AT24C02_Read(250,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
	}
	else
	{
		dataStore.realtimeData.deltaTemperature = 0.0f;
	}
	//*
	return 0;
}

void persistConfigFileToDefault(RealDataStore *ptr)
{
	ptr->realDataToSave.isStarted = REARING_STARTED;
	ptr->realDataToSave.cycleDays = CYCLE_DAYS_DEFAULT;
	ptr->realDataToSave.rtcDateStart.RTC_Year    = START_RTC_YEAR_DEFAULT;
	ptr->realDataToSave.rtcDateStart.RTC_Month   = START_RTC_MONTH_DEFAULT;
	ptr->realDataToSave.rtcDateStart.RTC_Date    = START_RTC_DATE_DEFAULT;
	ptr->realDataToSave.rtcDateStart.RTC_WeekDay = START_RTC_WEEKDAY_DEFAULT;
	ptr->realDataToSave.rtcTimeStart.RTC_Hours   = START_RTC_HOURS_DEFAULT;
	ptr->realDataToSave.rtcTimeStart.RTC_Minutes = START_RTC_MINUTES_DEFAULT;
	ptr->realDataToSave.rtcTimeStart.RTC_Seconds = START_RTC_SECONDS_DEFAULT;
}

uint8_t sysCtrlConfigFileInit(void)
{
	uint8_t err_code = 0;
	AT24C02_Init();
	memset(&dataStore.realtimeData,0x00,sizeof(RealDataStore));
	readCtrlConfigFile(&dataStore.realtimeData.realDataToSave,sizeof(RealDataToSave));
	if (dataStore.realtimeData.realDataToSave.key != INIT_KEY)
	{
		persistConfigFileToDefault(&dataStore.realtimeData);
		dataStore.realtimeData.realDataToSave.key = INIT_KEY;
		err_code = sysCtrlConfigFileWrite(&dataStore.realtimeData.realDataToSave,sizeof(RealDataToSave));
	}
	dataStore.realtimeData.isColding = false;

	AT24C02_Read(ADDR_CFG_FILE,(u8 *)&dataStore.ctrlParameter,sizeof(dataStore.ctrlParameter));
	if (dataStore.ctrlParameter.keyCtrlParameter != INIT_KEY_FLASH)
	{
		memset(&dataStore.ctrlParameter,0x00,sizeof(ControlParameterStore));
		err_code = setControlParametersToDefault();
	}
	return err_code;
}

uint8_t sysCtrlConfigFileWrite(RealDataToSave *ptr,uint16_t len)
{
	int res,j=0;
	RealDataToSave test;
	AT24C02_Init();
	do
	{
		writeCtrlConfigFile(ptr,sizeof(RealDataToSave));
		readCtrlConfigFile(&test,sizeof(RealDataToSave));
		res = memcmp(ptr,&test,sizeof(RealDataToSave));
		++j;
		if (j >= 5)
		{
			return 2;
		}
	}while (res != 0);
	return 0;
}
