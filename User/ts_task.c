#include "includes.h"
#include "ts_task.h"
#include "sccf.h"
#include "rtc.h"
#include "stm32_flash.h"
#include "24c02.h"
#include "circleBuffer.h"
#include "utility.h"

#define HMI_ID  0x07
#define ISME_ID 0x01//0xFF
OS_TCB TSTaskTCB;
CPU_STK TS_TASK_STK[TS_STK_SIZE];



void ts_task(void *p_arg)
{
	OS_ERR 			err;
	CPU_INT32U      cpu_clk_freq;
	uint16_t crc_result,rec_crc,addr_offset,read_len = 0,write_len = 0,send_len = 0,eeprom_addr;
	p_arg = p_arg;
	char *pMsg,msg_size,buf_rec[200],i;
	char *store,*header;
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
			logPrintf(Error,"E:ts_task.c::ts_task QSTaskQPend Error. err = %d \r\n",err);
		}
		if ((msg_size > 0) && (*pMsg == ISME_ID) && (err == OS_ERR_NONE))
		{
			memcpy(buf_rec,pMsg,msg_size);
			crc_result = crc16(pMsg,(msg_size-2));
			rec_crc = *(buf_rec+msg_size-2)*256 + *(buf_rec+msg_size-1);
			send_len = 0;
			if (crc_result == rec_crc)
			{
//				++dataStore.blackBox.requestTimes;
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
						if (addr_offset == 0x400)
						{
							header = (int8_t *)&eeprom_addr;
							addr_offset -= 0x400;
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
						else if (addr_offset >= 0x7FF && addr_offset < 0x8FF)
						{
							//header = (int8_t *)&dataStore.realtimeData.deltaTemperature;
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.deltaTemperature;
							*(buf_rec+3) = *(header + 3);
							*(buf_rec+4) = *(header + 2);
							*(buf_rec+5) = *(header + 1);
							*(buf_rec+6) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature;
							*(buf_rec+7) = *(header + 3);
							*(buf_rec+8) = *(header + 2);
							*(buf_rec+9) = *(header + 1);
							*(buf_rec+10) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.stopHeatingBoilerPipeTemperature;
							*(buf_rec+11) = *(header + 3);
							*(buf_rec+12) = *(header + 2);
							*(buf_rec+13) = *(header + 1);
							*(buf_rec+14) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.runningTimeOfVentilate;
							*(buf_rec+15) = *(header + 3);
							*(buf_rec+16) = *(header + 2);
							*(buf_rec+17) = *(header + 1);
							*(buf_rec+18) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.stoppedTimeOfVentilate;
							*(buf_rec+19) = *(header + 3);
							*(buf_rec+20) = *(header + 2);
							*(buf_rec+21) = *(header + 1);
							*(buf_rec+22) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference;
							*(buf_rec+23) = *(header + 3);
							*(buf_rec+24) = *(header + 2);
							*(buf_rec+25) = *(header + 1);
							*(buf_rec+26) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.coolDownGrade[1].temperatureDifference;
							*(buf_rec+27) = *(header + 3);
							*(buf_rec+28) = *(header + 2);
							*(buf_rec+29) = *(header + 1);
							*(buf_rec+30) = *(header + 0);
							
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature;
							*(buf_rec+31) = *(header + 3);
							*(buf_rec+32) = *(header + 2);
							*(buf_rec+33) = *(header + 1);
							*(buf_rec+34) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpStopTemperature;
							*(buf_rec+35) = *(header + 3);
							*(buf_rec+36) = *(header + 2);
							*(buf_rec+37) = *(header + 1);
							*(buf_rec+38) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpRunningTime;
							*(buf_rec+39) = *(header + 3);
							*(buf_rec+40) = *(header + 2);
							*(buf_rec+41) = *(header + 1);
							*(buf_rec+42) = *(header + 0);
							header = (int8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime;
							*(buf_rec+43) = *(header + 3);
							*(buf_rec+44) = *(header + 2);
							*(buf_rec+45) = *(header + 1);
							*(buf_rec+46) = *(header + 0);
							break;
						}
						else if (addr_offset >= 0xA00 && addr_offset < 0xBBB)
						{
							//header = (int8_t *)dataStore.ctrlParameter.esp8266Options.routerName;
							//for (i = 0;i < read_len;i++)
							{
								//*(buf_rec+3+i) = *(header + i);
							}
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
						read_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
						header = (int8_t *)(&dataStore.realtimeData);
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
						/*if (*(buf_rec + 5) == 0xFF)
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
						*(buf_rec+7+read_len) = (uint8_t)(crc_result & 0x00FF);*/
						break;
					case 6:     //Signal analog output
						switch (addr_offset)
						{
							case 0x400:
								eeprom_addr = *(buf_rec + 4)*256 + *(buf_rec + 5);
								/**(buf_rec + 2) = read_len;
								send_len = read_len + 5;
								crc_result = crc16(buf_rec,(read_len+3));
								*(buf_rec+3+read_len) = (uint8_t)(crc_result>>8);
								*(buf_rec+4+read_len) = (uint8_t)(crc_result & 0x00FF);*/
								RS485_Send_Data(buf_rec,send_len);
								break;
							case 0x11:
								dataStore.realtimeData.realDataToSave.isStarted = HEATING_STARTED;
								break;
							case 0x12:
								dataStore.realtimeData.realDataToSave.isStarted = REARING_STARTED;
								dataStore.realtimeData.dayCycle = 1;
								dataStore.realtimeData.realDataToSave.key = INIT_KEY;
								memcpy(&dataStore.realtimeData.realDataToSave.rtcDateStart,&RTC_DateStruct,sizeof(RTC_DateStruct));
								memcpy(&dataStore.realtimeData.realDataToSave.rtcTimeStart,&RTC_TimeStruct,sizeof(RTC_TimeStruct));
								sysCtrlConfigFileWrite(&dataStore.realtimeData.realDataToSave,sizeof(RealDataStore));
								/*dataStore.realtimeData.deltaTemperature = 0.0f;
								//OS_CRITICAL_ENTER();
								i = 0x89;
								AT24C02_Write(100,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
								AT24C02_Write(104,(uint8_t *)&i,sizeof(uint8_t));
								//OS_CRITICAL_EXIT();*/
								break;
							case 0x13:
								dataStore.realtimeData.realDataToSave.isStarted = REARING_STOPPED;
								break;
							case 0x14:
								break;
							case 0x15:
								//Dec temperature
								/*dataStore.realtimeData.deltaTemperature -= 0.05;
								i = 0x89;
								AT24C02_Write(100,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
								AT24C02_Write(104,(uint8_t *)&i,sizeof(uint8_t));
								AT24C02_Read(100,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));*/
								break;
							case 0x16:
								//Inc temperature
								/*dataStore.realtimeData.deltaTemperature += 0.05;
								i = 0x89;
								AT24C02_Write(100,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
								AT24C02_Write(104,(uint8_t *)&i,sizeof(uint8_t));*/
								break;
							case 0x7FF:
								break;
							default:
								break;
						}
						break;
					case 0x10:
						switch (addr_offset)
						{
							case 0x801:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.startHeatingBoilerPipeTemperature));
								read_len = write_len;
								break;
							case 0x803:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.stopHeatingBoilerPipeTemperature;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.stopHeatingBoilerPipeTemperature));
								read_len = write_len;
								break;
							case 0x805:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.runningTimeOfVentilate;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.runningTimeOfVentilate));				
								read_len = write_len;
								break;
							case 0x807:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.stoppedTimeOfVentilate;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.stoppedTimeOfVentilate));
								read_len = write_len;
								break;
							case 0x809:
								header = (uint8_t *)&dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.coolDownGrade[0].temperatureDifference));
								read_len = write_len;
								break;
							case 0x80B:
								header = (uint8_t *)&dataStore.ctrlParameter.coolDownGrade[1].temperatureDifference;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.coolDownGrade[1].temperatureDifference));
								read_len = write_len;
								break;
							case 0x80D:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.waterPumpStartTemperature));
								read_len = write_len;
								break;
							case 0x80F:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpStopTemperature;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.waterPumpStopTemperature));
								read_len = write_len;
								break;
							case 0x811:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpRunningTime;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.waterPumpRunningTime));
								read_len = write_len;
								break;
							case 0x813:
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - 
															(u8 *)&dataStore.ctrlParameter.systemOptions.waterPumpStoppedTime));
								read_len = write_len;
								break;
							case 0x7FF:
								//header = (uint8_t *)&dataStore.realtimeData.deltaTemperature;
								header = (uint8_t *)&dataStore.ctrlParameter.systemOptions.deltaTemperature;
								write_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
								eeprom_addr = ADDR_CFG_FILE+abs((char *)&dataStore.ctrlParameter - (char *)&dataStore.ctrlParameter.systemOptions.deltaTemperature);
								for (i=0;i<write_len;i++)
								{
									*(header + i) = *(buf_rec + 6 + write_len - i);
									*(buf_rec+3+i) = *(buf_rec+6+i);
								}
								/*i = 0x89;
								AT24C02_Write(100,(uint8_t *)&dataStore.realtimeData.deltaTemperature,sizeof(float));
								AT24C02_Write(104,(uint8_t *)&i,sizeof(uint8_t));*/
								read_len = write_len;
								break;
							case 0x0B02:
								header = dataStore.ctrlParameter.esp8266Options.routerPasswd;
								write_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
								write_len *= 2;
								for (i=0;i<write_len;i++)
								{
									*(header + i) = *(buf_rec + 7 + i);
									*(buf_rec+3+i) = *(buf_rec+7+i);
								}
								break;
							case 0x0AF0:
								header = dataStore.ctrlParameter.esp8266Options.routerName;
								eeprom_addr = ADDR_CFG_FILE+abs(((u8 *)&dataStore.ctrlParameter - dataStore.ctrlParameter.esp8266Options.routerName));
								for (i=0;i<write_len*2;i++)
								{
									*(header + i) = *(buf_rec + 7 + i);
									*(buf_rec+3+i) = *(buf_rec+7+i);
								}
								AT24C02_Write(eeprom_addr,header,write_len);
								*(buf_rec + 2) = read_len;
								send_len = read_len + 5;
								crc_result = crc16(buf_rec,(read_len+3));
								*(buf_rec+3+read_len) = (uint8_t)(crc_result>>8);
								*(buf_rec+4+read_len) = (uint8_t)(crc_result & 0x00FF);
								RS485_Send_Data(buf_rec,send_len);
								return ;
								break;
							default:
								return;
								break;
						}
						write_len = sizeof(uint16_t) * (*(buf_rec + 4) * 256 + *(buf_rec + 5));
						for (i=0;i<write_len;i++)
						{
							*(header + i) = *(buf_rec + 6 + write_len - i);
							*(buf_rec+3+i) = *(buf_rec+6+i);
						}
						AT24C02_Write(eeprom_addr,header,write_len);
						break;
					default:
						logPrintf(Info,"I:ts_task.c::ts_task->Receive unknown order.\r\n");
						//TODO:set the system's configure file to default values and write these values to EEPROM forever!
						break;
				}
				if (read_len > 0)
				{
					*(buf_rec + 2) = read_len;
					send_len = read_len + 5;
					crc_result = crc16(buf_rec,(read_len+3));
					*(buf_rec+3+read_len) = (uint8_t)(crc_result>>8);
					*(buf_rec+4+read_len) = (uint8_t)(crc_result & 0x00FF);
					RS485_Send_Data(buf_rec,send_len);
				}
			}
		}
		OSMemPut ((OS_MEM  *)&mymem,
							(void    *)pMsg,
							(OS_ERR  *)&err);
		if (err != OS_ERR_NONE)
		{
			logPrintf(Error,"E:ts_task.c::ts_task QSMemPut Error. err = %d \r\n",err);
		}
	}
}
