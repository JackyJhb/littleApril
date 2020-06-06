#include "wifi_task.h"
#include "wifi.h"
#include "24c02.h"
#include "sccf.h"
#include "mqtt.h"
#include "includes.h"

OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
char bufWifi[1500];
char mqttStatus = MQTT_INIT;
uint16_t timer=0;


void heartBeat(void);
void connectToMQTTServer(void);
void disconnectFromMQTTServer(void);
void protocolAnalyze(char *buf,uint16_t len);

void connectToMQTTServer(void)
{
}

void disconnectFromMQTTServer(void)
{
}

void sendLogStream(void)
{
	uint16_t len;
	len = getLogDataPublish(bufWifi,ToServer,300);
	if (len)
	{
		sendDatas(bufWifi,len);
	}
}

void publishRealTimeData(void)
{
	uint16_t len;

	len = getDataPublish(bufWifi,ToServer,(char *)&dataStore.realtimeData,
											 sizeof(dataStore.realtimeData),ClientBroadcastRealTimeStatus);
	sendDatas(bufWifi,len);
}

void heartBeat(void)
{
	uint16_t len;
	len = getDataPingRespond(bufWifi);
	sendDatas(bufWifi,len);
}
void protocolAnalyze(char *buf,uint16_t len)
{
	uint32_t offset = 0;
	uint16_t len_data = 0;
	uint8_t order = *buf,rep_res=0x00;
	switch (order)
	{
		case ServerRequestAlarmThreshold:
			len_data = getDataPublish(bufWifi,ToServer,
																(char *)&dataStore.ctrlParameter.alarmThresholdOptions,
																sizeof(AlarmThresholdStore),
																ClientRespondAlarmThreshold);
			break;
		case ServerRequestSystemOptions:
			len_data = getDataPublish(bufWifi,ToServer,
																(char *)&dataStore.ctrlParameter.systemOptions,
																sizeof(SystemOptions),ClientRespondSystemOptions);
			break;
		case ServerRequestCoolDownGrade:
			len_data = getDataPublish(bufWifi,ToServer,
																(char *)dataStore.ctrlParameter.coolDownGrade,
																sizeof(dataStore.ctrlParameter.coolDownGrade),
																ClientRespondCoolDownGrade);
			break;
		case ServerRequestAmbientTemperatureWeek:
			len_data = getDataPublish(bufWifi,ToServer,
																(char *)dataStore.ctrlParameter.ambientTemperature,
																sizeof(dataStore.ctrlParameter.ambientTemperature),
																ClientRespondAmbientTemperatureWeek);
			break;
		case ServerRequestVentilateFansRunCtrl:
			len_data = getDataPublish(bufWifi,ToServer,
															  (char *)dataStore.ctrlParameter.ventilation.ventilateGrade,
																sizeof(dataStore.ctrlParameter.ventilation.ventilateGrade),
																ClientRespondVentilationFansRunCtrl);
			break;
		case ServerRequestVentilate:
			len_data = getDataPublish(bufWifi,ToServer,
																(char *)dataStore.ctrlParameter.ventilation.ventilationCoefficient,
																 sizeof(dataStore.ctrlParameter.ventilation.ventilationCoefficient),
																ClientRespondVentilate);
			break;
		case ServerSetRefAlarmThreshold:
			memcpy((char *)&dataStore.ctrlParameter.alarmThresholdOptions,(buf+1),sizeof(AlarmThresholdStore));
			offset = abs((char *)&dataStore.ctrlParameter - (char *)&dataStore.ctrlParameter.alarmThresholdOptions);
			AT24C02_Write(ADDR_CFG_FILE+offset,(uint8_t *)&dataStore.ctrlParameter.alarmThresholdOptions,sizeof(AlarmThresholdStore));
			len_data = getDataPublish(bufWifi,ToServer,&rep_res,sizeof(rep_res),ServerSetRefAlarmThreshold);
			break;
		case ServerSetRefSystemOptions:
			memcpy((char *)&dataStore.ctrlParameter.systemOptions,(buf+1),sizeof(SystemOptions));
			offset = abs((char *)&dataStore.ctrlParameter - (char *)&dataStore.ctrlParameter.systemOptions);
			AT24C02_Write(ADDR_CFG_FILE+offset,(uint8_t *)&dataStore.ctrlParameter.systemOptions,sizeof(SystemOptions));
			len_data = getDataPublish(bufWifi,ToServer,&rep_res,sizeof(rep_res),ServerSetRefSystemOptions);
			break;
		case ServerSetRefCoolDownGrade:
			memcpy((char *)dataStore.ctrlParameter.coolDownGrade,(buf+1),sizeof(dataStore.ctrlParameter.coolDownGrade));
			offset = abs((char *)&dataStore.ctrlParameter - (char *)dataStore.ctrlParameter.coolDownGrade);
			AT24C02_Write(ADDR_CFG_FILE+offset,(uint8_t *)dataStore.ctrlParameter.coolDownGrade,sizeof(dataStore.ctrlParameter.coolDownGrade));
			len_data = getDataPublish(bufWifi,ToServer,&rep_res,sizeof(rep_res),ServerSetRefCoolDownGrade);
			break;
		case ServerSetRefAmbientTemperatureWeek:
			memcpy((char *)dataStore.ctrlParameter.ambientTemperature,(buf+1),sizeof(dataStore.ctrlParameter.ambientTemperature));
			offset = abs((char *)&dataStore.ctrlParameter - (char *)dataStore.ctrlParameter.ambientTemperature);
			AT24C02_Write(ADDR_CFG_FILE+offset,(uint8_t *)dataStore.ctrlParameter.ambientTemperature,sizeof(dataStore.ctrlParameter.ambientTemperature));
			len_data = getDataPublish(bufWifi,ToServer,&rep_res,sizeof(rep_res),ServerSetRefAmbientTemperatureWeek);
			break;
		case ServerSetRefVentilateFansRunCtrl:
			memcpy((char *)dataStore.ctrlParameter.ventilation.ventilateGrade,(buf+1),sizeof(dataStore.ctrlParameter.ventilation.ventilateGrade));
			offset = abs((char *)&dataStore.ctrlParameter - (char *)dataStore.ctrlParameter.ventilation.ventilateGrade);
			AT24C02_Write(ADDR_CFG_FILE+offset,(uint8_t *)dataStore.ctrlParameter.ventilation.ventilateGrade,sizeof(dataStore.ctrlParameter.ventilation.ventilateGrade));
			len_data = getDataPublish(bufWifi,ToServer,&rep_res,sizeof(rep_res),ServerSetRefVentilateFansRunCtrl);
			break;
		case ServerSetRefVentilate:
			memcpy((char *)dataStore.ctrlParameter.ventilation.ventilationCoefficient,(buf+1),sizeof(dataStore.ctrlParameter.ventilation.ventilationCoefficient));
			offset = abs((char *)&dataStore.ctrlParameter - (char *)dataStore.ctrlParameter.ventilation.ventilationCoefficient);
			AT24C02_Write(ADDR_CFG_FILE+offset,(uint8_t *)dataStore.ctrlParameter.ventilation.ventilationCoefficient,sizeof(dataStore.ctrlParameter.ventilation.ventilationCoefficient));
			len_data = getDataPublish(bufWifi,ToServer,&rep_res,sizeof(rep_res),ServerSetRefVentilate);
			break;
		default:
			break;
	}
	if (len_data)
	{
		sendDatas(bufWifi,len_data);
	}
}
void ipdDeal(char *buf,uint16_t len)
{
	uint16_t real_len;
	char *ptr;
	switch (*((uint8_t *)buf))
	{
		case 0x20:         //Response to connect request
			mqttStatus = MQTT_SUBSCRIBE;
			break;
		case 0xD0:         //Heart beat respond
			mqttStatus = MQTT_CONNECTED;
			break;
		case 0x30:         //Datas arrivied
			if (buf[1]&0x80)
			{
				real_len = buf[2];
				real_len *= 128;
				real_len += buf[1] & 0x7F;
				ptr = buf+3+sizeof(SubscribeOrPublishTopic)+2;
			}
			else
			{
				real_len = buf[1];
				ptr = buf+2+sizeof(SubscribeOrPublishTopic)+2;
			}
			protocolAnalyze(ptr,real_len);
			break;
		case 0xE0:
			break;
		case 0x90:         //Response to subscribe
			mqttStatus = MQTT_CONNECTED;
			break;
		default:
			break;
	}
}

void WIFI_task(void *p_arg)
{
	OS_ERR err;
	uint16_t len,i;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	dataStore.realtimeData.netWorkStatus = NotFoundESP8266;
	while(1)
	{
		if (dataStore.realtimeData.netWorkStatus != ServerConnected)
		{
			WIFI_Server_Init();
			mqttStatus = MQTT_INIT;
			timer = 0;
		}
		else
		{
			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
			//receiveLogFromQPend();
			++timer;
			switch (mqttStatus)
			{
				case MQTT_INIT:				
					len = getDataConnect(bufWifi,
										dataStore.ctrlParameter.esp8266Options.mqttUserName,
										dataStore.ctrlParameter.esp8266Options.mqttPasswd);
					sendDatas(bufWifi,len);
					timer = 0;
					mqttStatus = MQTT_CONNECTING;
					break;
				case MQTT_CONNECTING:
					if (timer == 20)
					{
						mqttStatus = MQTT_INIT;
					}
					break;
				case MQTT_SUBSCRIBE:
					len = getDataSubscribe(bufWifi,FromServer,0x0001,0x00);
					sendDatas(bufWifi,len);
					mqttStatus = MQTT_SUBSCRIBING;
					timer = 0;
					break;
				case MQTT_SUBSCRIBING:
					if (timer == 20)
					{
						mqttStatus = MQTT_INIT;
					}
					break;
				case MQTT_CONNECTED:
					if (timer == 2100)
					{
						heartBeat();
						timer = 0x00;
					}
					else if ((timer % 200) == 0)
					{
						publishRealTimeData();
					}
					else if ((timer % 150) == 0)
					{
						//sendLogStream();
					}
					break;
				default:
					break;
			}
			if (getWifiRecDatas(bufWifi))
			{
				ipdDeal(bufWifi,len);
			}
		}
	}
}
