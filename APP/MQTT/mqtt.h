#ifndef MQTT_H
#define MQTT_H

#include <string.h>
#define		MQTT_TypeCONNECT           1
#define		MQTT_TypeCONNACK           2 
#define		MQTT_TypePUBLISH           3
#define		MQTT_TypePUBACK            4 
#define		MQTT_TypePUBREC            5
#define		MQTT_TypePUBREL            6 
#define		MQTT_TypePUBCOMP           7
#define		MQTT_TypeSUBSCRIBE         8
#define		MQTT_TypeSUBACK            9
#define		MQTT_TypeUNSUBSCRIBE       10 
#define		MQTT_TypeUNSUBACK          11
#define		MQTT_TypePINGREQ           12
#define		MQTT_TypePINGRESP          13
#define		MQTT_TypeDISCONNECT        14
 
#define		MQTT_StaCleanSession        1
#define		MQTT_StaWillFlag            0
#define		MQTT_StaWillQoS             0
#define		MQTT_StaWillRetain          0
#define		MQTT_StaUserNameFlag        1
#define		MQTT_StaPasswordFlag        1
#define		MQTT_KeepAlive              60
#define		MQTT_ClientIdentifier       "littleApril_NO1"
#define		MQTT_WillTopic              ""
#define		MQTT_WillMessage            ""
#define		MQTT_UserName               "rkkj2020"
#define		MQTT_Password               "rkkj2020rkkj"

#define 	MQTT_INIT                   0
#define   MQTT_DISCONNECTED           1
#define   MQTT_CONNECTING             2
#define   MQTT_CONNECTED              3         
 
extern unsigned char getDataFixedHead(unsigned char mesType,unsigned char dupFlag,unsigned char qosLevel,unsigned char retain);
extern unsigned int getDataPublish(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg);		 	
extern unsigned int getDataSubscribe(unsigned char *buff,const char *dat,unsigned int num,unsigned char requestedQoS);
extern unsigned int getDataDisconnect(unsigned char *buff);
extern unsigned int getDataConnect(unsigned char *buff);
extern unsigned int getDataPingRespond(unsigned char *buff);

#endif
