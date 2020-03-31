#include "wifi_task.h"
#include "wifi.h"
#include "24c02.h"
#include "sccf.h"
#include "mqtt.h"

OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
char bufWifi[120];

void heartBeat(void);
void connectToMQTTServer(void);
void disconnectFromMQTTServer(void);

void connectToMQTTServer(void)
{
}

void disconnectFromMQTTServer(void)
{
}



void heartBeat(void)
{
}

void WIFI_task(void *p_arg)
{
	OS_ERR err;
	char mqttStatus = MQTT_INIT;;
	uint16_t len;
	p_arg = p_arg;
	dataStore.realtimeData.netWorkStatus = NotFoundESP8266;
	usartWifiInit(115200);
	while(1)
	{
		#ifdef ENABLE_WIFI_LOG
		printf("Info:wifi dataStore.realtimeData.netWorkStatus = %d\r\n",
						dataStore.realtimeData.netWorkStatus);
		#endif
		if (dataStore.realtimeData.netWorkStatus != ServerConnected)
		{
			WIFI_Server_Init();
			mqttStatus = MQTT_INIT;
		}
		else
		{
			
			#ifdef ENABLE_WIFI_LOG
			printf("Info:wifi dataStore.realtimeData.netWorkStatus = %d\r\n",
							dataStore.realtimeData.netWorkStatus);
			#endif
			
			switch (mqttStatus)
			{
				case MQTT_INIT:
					len = getDataConnect(bufWifi);
					if (sendDatas(bufWifi,len) == 0)
						break;
					#ifdef ENABLE_WIFI_LOG
					printf("Info:wifi MQTT connecting... \r\n");
					#endif
					len = wifiReceiveData(bufWifi,200);
					//if ((len > 0) && (bufWifi[0] == 0x20))
					{
						mqttStatus = MQTT_CONNECTED;
						#ifdef ENABLE_WIFI_LOG
						printf("Info:wifi MQTT connected... \r\n");
						#endif
					}
					break;
				case MQTT_DISCONNECTED:
					//len = getDataConnect(bufWifi);
					//mqttStatus = MQTT_CONNECTING;
					//sendDatas(bufWifi,len);
					break;
				case MQTT_CONNECTED:
					OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_DLY,&err);
					len = getDataPingRespond(bufWifi);
					sendDatas(bufWifi,len);
					len = wifiReceiveData(bufWifi,100);
					break;
				default:
					len = wifiReceiveData(bufWifi,0);
					if (len)
					{
						sendDatas(bufWifi,len);
					}
					break;
			}
			//OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_DLY,&err);
		}
	}
}
