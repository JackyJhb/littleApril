
#include "sccf.h"
#include "mqtt.h"

char getDataFixedHead(char mesType,char dupFlag,char qosLevel,char retain);
int getDataPublish(char *buff,DataSource dataSrc,char *msg,int msgLen,char dataType);
int getDataSubscribe(char *buff,DataSource dataSrc,int num,char requestedQoS);
int getDataDisconnect(char *buff);
int getDataConnect(char *buff,char *userName,char *passwd);
int getDataPingRespond(char *buff);

char getDataFixedHead(char mesType,char dupFlag,char qosLevel,char retain)
{
	unsigned char dat = 0;
	dat = (mesType & 0x0f) << 4;
	dat |= (dupFlag & 0x01) << 3;
	dat |= (qosLevel & 0x03) << 1;
	dat |= (retain & 0x01);
	return dat;
}

int getDataPublish(char *buff,DataSource dataSrc,char *msg,int msgLen,char dataType)
{
	SubscribeOrPublishTopic *ptr;
	unsigned int i,len=0,lennum=0,offset=0;
	len = (sizeof(SubscribeOrPublishTopic) + msgLen + 2 + 1);
	if (len > 1460)
		return 0;
	if (len > 127)
		offset = 1;

	buff[0] = getDataFixedHead(MQTT_TypePUBLISH,0x00,0x00,0x00);
	len = sizeof(SubscribeOrPublishTopic);
	buff[2+offset] = len>>8;
	buff[3+offset] = len;
	ptr = (SubscribeOrPublishTopic *)(buff+4+offset);
	ptr->codeID = CODE_ID;
	ptr->sep0 = '/';
	ptr->dataSource = dataSrc;
	for(i = 0;i<STM32_UNIQUE_ID_SIZE;i++)
	{
		sprintf(ptr->deviceID+i*2,"%x",dataStore.realtimeData.stm32UniqueID[i]/16);
		sprintf(ptr->deviceID+i*2+1,"%x",dataStore.realtimeData.stm32UniqueID[i]%16);
	}
	ptr->sep1 = '/';
	lennum = len;
	len = msgLen;
	buff[4+lennum+offset] = dataType;
	/*for(i = 0;i<len;i++)
	{
		buff[4+i+lennum+offset + 1] = msg[i];
	}*/
	if (len > 0)
	{
		memcpy((buff+4+1+lennum+offset),msg,len);
	}
	lennum += len;
	lennum += 2;
	lennum += 1;
	if (offset)
	{
		buff[2] = lennum/128;
		buff[1] = (lennum % 128 | 0x80);
	}
	else
	{
		buff[1] = lennum;
	}
	return (buff[1]+2+offset);
}

int getDataDisconnect(char *buff)
{
	buff[0] = 0xe0;
	buff[1] = 0;
	return 2;
}

int getDataConnect(char *buff,char *userName,char *passwd)
{
	unsigned int i,len,lennum = 0;
	unsigned char *msg;
	buff[0] = getDataFixedHead(MQTT_TypeCONNECT,0,0,0);
	buff[2] = 0x00;
	buff[3] = 0x04;
	buff[4] = 'M';
	buff[5] = 'Q';
	buff[6] = 'T';
	buff[7] = 'T';
	buff[8] = 0x04;//Protocol Level
	buff[9] = 0 | (MQTT_StaCleanSession << 1) | (MQTT_StaWillFlag << 1)
							| (MQTT_StaWillQoS << 3) | (MQTT_StaWillRetain << 5) 
							|	(MQTT_StaPasswordFlag << 6) |(MQTT_StaUserNameFlag << 7);
	buff[10] = MQTT_KeepAlive >> 8;
	buff[11] = MQTT_KeepAlive;
	//len = strlen(MQTT_ClientIdentifier);
	len = STM32_UNIQUE_ID_SIZE*2;
	buff[12] = len >> 8;
	buff[13] = len;
	/*msg = MQTT_ClientIdentifier;
	for(i = 0;i<len;i++)
	{
		buff[14+i] =  msg[i];
	}*/
	for(i = 0;i<STM32_UNIQUE_ID_SIZE;i++)
	{
		sprintf(buff+14+i*2,"%x",dataStore.realtimeData.stm32UniqueID[i]/16);
		sprintf(buff+14+i*2+1,"%x",dataStore.realtimeData.stm32UniqueID[i]%16);
	}
	lennum += len;
	if(MQTT_StaWillFlag)
	{
		len = strlen(MQTT_WillTopic);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_WillTopic;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		len = strlen(MQTT_WillMessage);
		buff[12] = len >> 8;
		buff[13] = len;
		lennum += 2;
		msg = MQTT_WillMessage;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	if(MQTT_StaUserNameFlag)
	{
		len = strlen(userName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = userName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(passwd);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = passwd;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;
	return (buff[1]+2);
}

int getDataSubscribe(char *buff,DataSource dataSrc,int num,char requestedQoS)
{
	SubscribeOrPublishTopic *ptr;
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = sizeof(SubscribeOrPublishTopic);
	buff[2] = num>>8;
	buff[3] = num;
	buff[4] = len>>8;
	buff[5] = len;
	ptr = (SubscribeOrPublishTopic *)(buff+6);
	ptr->codeID = CODE_ID;
	ptr->sep0 = '/';
	ptr->dataSource = dataSrc;
	ptr->sep1 = '/';
	for(i = 0;i<STM32_UNIQUE_ID_SIZE;i++)
	{
		sprintf(ptr->deviceID+i*2,"%x",dataStore.realtimeData.stm32UniqueID[i]/16);
		sprintf(ptr->deviceID+i*2+1,"%x",dataStore.realtimeData.stm32UniqueID[i]%16);
	}
	lennum = len;
	buff[6 + lennum ] = requestedQoS;
	buff[1] = lennum + 5;
	return buff[1]+2;
}

int getDataPingRespond(char *buff)
{
	buff[0] = 0xc0;
	buff[1] = 0;
	return 2;
}
