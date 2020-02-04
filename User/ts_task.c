#include "includes.h"
#include "ts_task.h"
#include "sccf.h"
#include "rtc.h"
#include "stm32_flash.h"
#include "24c02.h"

#define HMI_ID  0x07
#define ISME_ID 0xFF
OS_TCB TSTaskTCB;
CPU_STK TS_TASK_STK[TS_STK_SIZE];

/* Table Of CRC Values for high-order unsigned char */
unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};

/* Table of CRC values for low-order unsigned char    */
unsigned char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

uint16_t crc16(uint8_t *puchMsg,uint8_t usDataLen)
{
	uint8_t uchCRCHi = 0xFF;
	uint8_t uchCRCLo = 0xFF;
	uint16_t uIndex;
	while (usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	return (((unsigned int)(uchCRCHi) << 8) | uchCRCLo);
}

void ts_task(void *p_arg)
{
	OS_ERR 			err;
	CPU_INT32U      cpu_clk_freq;
	uint16_t crc_result,rec_crc,addr_offset,read_len = 0,write_len = 0,send_len = 0;
	p_arg = p_arg;
	char *pMsg,msg_size,buf_rec[200],i,*header;
	static char isFirst = 1;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	while(1)
	{
		pMsg = OSTaskQPend ((OS_TICK        )0,
							(OS_OPT         )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE   *)&msg_size,
							(CPU_TS        *)0,
							(OS_ERR       *)&err);
		if (err != OS_ERR_NONE)
		{
			#ifdef ENABLE_OUTPUT_LOG
			printf("Error:ts_task.c::ts_task QSTaskQPend Error. err = %d \r\n",err);
			#endif
		}
		if ((msg_size > 0) && (*pMsg == ISME_ID) && (err == OS_ERR_NONE))
		{
			memcpy(buf_rec,pMsg,msg_size);
			crc_result = crc16(pMsg,(msg_size-2));
			rec_crc = *(buf_rec+msg_size-2)*256 + *(buf_rec+msg_size-1);
			send_len = 0;
			if (crc_result == rec_crc)
			{
				++dataStore.blackBox.requestTimes;
				addr_offset = *(buf_rec + 2) * 256 + *(buf_rec + 3);
				switch (*(buf_rec+1))
				{
					case 1://Multiple digital output
						addr_offset = (addr_offset - 1)/8;
						read_len = (*(buf_rec + 4) * 256 + *(buf_rec + 5))/8;
						if ((*(buf_rec + 4) * 256 + *(buf_rec + 5))%8 != 0)
							++read_len;
						for (i = 0;i < read_len;i++)
						{
							*(buf_rec + 3 + i) = *(((uint8_t *) &dataStore.realtimeData.workingVentilators) + i + addr_offset);
						}
						break;
					case 2: //Multiple digital input
						//TODO Set started or stopped status and set alarm time if started was set.
						read_len = (*(buf_rec + 4) * 256 + *(buf_rec + 5))/8;
						if ((*(buf_rec + 4) * 256 + *(buf_rec + 5))%8 != 0)
							++read_len;
						for (i = 0;i < read_len;i++)
						{
							*(buf_rec + 3 + i) = *(((uint8_t *) &dataStore.realtimeData.heatingColdingStatus) + i);
						}
						break;
					case 3:		//Multiple analogs input			
						read_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
						if (addr_offset <= 0x1B)
						{
							header = (int8_t *)&dataStore.blackBox;
						}
						else if (addr_offset >= 0x32 && addr_offset < 0x1FF)
						{
							header = (int8_t *)&dataStore.realtimeData;
							addr_offset -= 0x32;
						}
						else if (addr_offset >= 0x1FF && addr_offset < 0x3FF)
						{
							header = (int8_t *)&dataStore.ctrlParameter.alarmThresholdOptions;
							addr_offset -= 0x1FF;
						}
						else
						{
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions;
							addr_offset -= 0x3FF;
						}									
						for (i = 0;i < read_len;i++)
						{
							if (i % 2 == 0)
							{
								*(buf_rec+3+i) = *(header + i + addr_offset*2 + 1);
							}
							else
							{
								*(buf_rec+3+i) = *(header + i + addr_offset*2 - 1);
							}
							//*(buf_rec+3+i) = *(header+(i/4)*4+(3-i%4));
						}
						break;
					case 4:     //Multiple analogs input
						//addr_offset = *(buf_rec + 2) * 256 + *(buf_rec + 3);						
						read_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
						header = (int8_t *)(&dataStore.realtimeData);
						dataStore.realtimeData.realDataToSave.cycleDays = 49;
						dataStore.realtimeData.realSideWindowsAngle[0] = 20;
						dataStore.realtimeData.realSideWindowsAngle[1] = 22;
						dataStore.realtimeData.targetSideWindowsAngle = 21;
						dataStore.realtimeData.humidityInside[0] = 78;
						dataStore.realtimeData.humidityInside[1] = 79;
						dataStore.realtimeData.humidityInside[2] = 80;
						dataStore.realtimeData.insideTemperature[0][0] = 26.6;
						dataStore.realtimeData.insideTemperature[0][1] = 26.8;
						dataStore.realtimeData.insideTemperature[1][0] = 27.7;
						dataStore.realtimeData.insideTemperature[1][1] = 27.9;
						dataStore.realtimeData.insideTemperature[2][0] = 28.8;
						dataStore.realtimeData.insideTemperature[2][1] = 29.0;
						dataStore.realtimeData.outsideTemperature = -22.2;
						dataStore.realtimeData.boilerTemperature = 55;
						dataStore.realtimeData.pressureInside = 10;
						dataStore.realtimeData.currentSetTemperature = 25.8;
						if (addr_offset == 255)
						{
							for (i = 0;i < read_len;i++)
							{
								if (i % 2 == 0)
								{
									*(buf_rec+3+i) = 0x00;
								}
								else
								{
									*(buf_rec+3+i) = dataStore.realtimeData.dayCycle & 0x00FF;
								}
							}
						}
						else
						{
							for (i = 0;i < read_len;i++)
							{
								if (i % 2 == 0)
								{
									*(buf_rec+3+i) = *(header + i + 1);
								}
								else
								{
									*(buf_rec+3+i) = *(header + i - 1);
								}
							}
						}
						break;
					case 5:		//Signal digital output
						//addr_offset = *(buf_rec + 2) * 256 + *(buf_rec + 3);
						if (*(buf_rec + 5) == 0xFF)
						{
							dataStore.realtimeData.realDataToSave.isStarted = REARING_STARTED;
						}
						else
						{
							dataStore.realtimeData.realDataToSave.isStarted = REARING_STOPPED;
						}
						send_len = 8;
						crc_result = crc16(buf_rec,6);
						*(buf_rec+6+read_len) = (uint8_t)(crc_result>>8);
						*(buf_rec+7+read_len) = (uint8_t)(crc_result & 0x00FF);
						break;
					case 6:     //Signal analog output
						//addr_offset = *(buf_rec + 2) * 256 + *(buf_rec + 3);
						switch (addr_offset)
						{
							case 0x11:
								dataStore.realtimeData.realDataToSave.isStarted = HEATING_STARTED;
								break;
							case 0x12:
								dataStore.realtimeData.realDataToSave.isStarted = REARING_STARTED;
								dataStore.realtimeData.realDataToSave.cycleDays = 49;
								dataStore.realtimeData.dayCycle = 0;
								dataStore.realtimeData.realDataToSave.key = INIT_KEY;
								memcpy(&dataStore.realtimeData.realDataToSave.rtcDateStart,&RTC_DateStruct,sizeof(RTC_DateStruct));
								memcpy(&dataStore.realtimeData.realDataToSave.rtcTimeStart,&RTC_TimeStruct,sizeof(RTC_TimeStruct));
								if (sysCtrlConfigFileWrite(&dataStore.realtimeData.realDataToSave,sizeof(RealDataStore)))
								{
									#ifdef ENABLE_OUTPUT_LOG
									printf("Error:ts_task.c::ts_task -> Rearing started,write config files to AT24C02 failed.\r\n");
									#endif
								}
								break;
							case 0x13:
								dataStore.realtimeData.realDataToSave.isStarted = REARING_STOPPED;
								break;
							case 0x14:
								break;
							case 0x15:
								//Dec temperature
								dataStore.realtimeData.deltaTemperature -= 0.05;
								//OS_CRITICAL_ENTER();
								i = 0x89;
								AT24C02_Write(250,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
								AT24C02_Write(254,(uint8_t *)&i,sizeof(uint8_t));
								//OS_CRITICAL_ENTER();
								break;
							case 0x16:
								//Inc temperature
								dataStore.realtimeData.deltaTemperature += 0.05;
								//OS_CRITICAL_ENTER();
								i = 0x89;
								AT24C02_Write(250,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
								AT24C02_Write(254,(uint8_t *)&i,sizeof(uint8_t));
								//OS_CRITICAL_ENTER();
								break;
							case 0x17:
								--dataStore.realtimeData.deltaActionCycle;
								break;
							case 0x18:
								++dataStore.realtimeData.deltaActionCycle;
								break;
							case 0x19:
								if (isFirst)
								{
									isFirst = 0;
									--dataStore.realtimeData.deltaActionTimeSpan;
								}
								else
								{
									isFirst = 1;
								}
								break;
							case 0x1A:
								if (isFirst)
								{
									isFirst = 0;
									++dataStore.realtimeData.deltaActionTimeSpan;
								}
								else
								{
									isFirst = 1;
								}
								break;
							case 0x7FF:
								break;
							default:
								break;
						}
						break;
					case 0x10:
						addr_offset -= 0x3FF;
						header = (uint8_t *)&dataStore.ctrlParameter.systemOptions + addr_offset*2;
						write_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
						for (i=0;i<write_len;i++)
						{
							*(header + i) = *(buf_rec + 6 + write_len - i);
							*(buf_rec+3+i) = *(buf_rec+6+i);
						}
						read_len = write_len;
						//OS_CRITICAL_ENTER();
						i = 0x89;
						dataStore.realtimeData.deltaTemperature = 1.99;
						AT24C02_Write(250,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
						AT24C02_Write(254,(uint8_t *)&i,sizeof(uint8_t));
						//OS_CRITICAL_EXIT();
						break;
					default:
						//TODO:set the system's configure file to default values and write these values to EEPROM forever!
						break;
				}
				*(buf_rec + 2) = read_len;
				send_len = read_len + 5;
				crc_result = crc16(buf_rec,(read_len+3));
				*(buf_rec+3+read_len) = (uint8_t)(crc_result>>8);
				*(buf_rec+4+read_len) = (uint8_t)(crc_result & 0x00FF);
				RS485_Send_Data(buf_rec,send_len);
			}
		}
		OSMemPut ((OS_MEM  *)&mymem,
							(void    *)pMsg,
							(OS_ERR  *)&err);
		if (err != OS_ERR_NONE)
		{
			#ifdef ENABLE_OUTPUT_LOG
			printf("Error:ts_task.c::ts_task QSMemPut Error. err = %d \r\n",err);
			#endif
		}
	}
}
