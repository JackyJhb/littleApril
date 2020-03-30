

#include "mqtt.h"
#include "protocol.h"

unsigned char getDataFixedHead(unsigned char mesType,unsigned char dupFlag,unsigned char qosLevel,unsigned char retain);
unsigned int getDataPublish(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);		 	
unsigned int getDataSubscribe(unsigned char *buff,const char *dat,unsigned int num,unsigned char requestedQoS);
unsigned int getDataDisconnect(unsigned char *buff);
unsigned int getDataConnect(unsigned char *buff);
unsigned int getDataPingRespond(unsigned char *buff);

unsigned char getDataFixedHead(unsigned char mesType,unsigned char dupFlag,unsigned char qosLevel,unsigned char retain)
{
    unsigned char dat = 0;
	dat = (mesType & 0x0f) << 4;
	dat |= (dupFlag & 0x01) << 3;
	dat |= (qosLevel & 0x03) << 1;
	dat |= (retain & 0x01);
	return dat;
}

unsigned int getDataPublish(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)
{
	unsigned int i,len=0,lennum=0;
	buff[0] = getDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);
	len = strlen(topic);
	buff[2] = len>>8;
	buff[3] = len;
	for(i = 0;i<len;i++)
	{
		buff[4+i] = topic[i];
	}
	lennum = len;
	len = strlen(msg);
	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2;
	return (lennum+1);
}

unsigned int getDataDisconnect(unsigned char *buff)
{
	buff[0] = 0xe0;
	buff[1] = 0;
	return 2;
}

unsigned int getDataConnect(unsigned char *buff)
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
	len = strlen(MQTT_ClientIdentifier);
	buff[12] = len >> 8;
	buff[13] = len;
	msg = MQTT_ClientIdentifier;
	for(i = 0;i<len;i++)
	{
		buff[14+i] =  msg[i];
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
		len = strlen(MQTT_UserName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_UserName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(MQTT_Password);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_Password;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;
	return (buff[1]+2);
}

unsigned int getDataSubscribe(unsigned char *buff,const char *dat,unsigned int num,unsigned char requestedQoS)
{
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = strlen(dat);
	buff[2] = num>>8;
	buff[3] = num;
	buff[4] = len>>8;
	buff[5] = len;
	for(i = 0;i<len;i++)
	{
		buff[6+i] = dat[i];
	}
	lennum = len;
	buff[6 + lennum ] = requestedQoS;
	buff[1] = lennum + 5;
	return buff[1]+2;
}

unsigned int getDataPingRespond(unsigned char *buff)
{
	buff[0] = 0xc0;
	buff[1] = 0;
	return 2;
}
