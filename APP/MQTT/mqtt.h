#ifndef MQTT_H
#define MQTT_H

#include <string.h>
#include "protocol.h"

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
        
 
extern char getDataFixedHead(char mesType,char dupFlag,char qosLevel,char retain);
extern int getDataPublish(char *buff,DataSource dataSrc,char *msg,char msgLen,char dataType);
extern int getDataSubscribe(char *buff,DataSource dataSrc,int num,char requestedQoS);
extern int getDataDisconnect(char *buff);
extern int getDataConnect(char *buff,char *userName,char *passwd);
extern int getDataPingRespond(char *buff);

#endif
