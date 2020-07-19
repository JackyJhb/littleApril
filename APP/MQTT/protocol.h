#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "stm32f4xx.h"

#define CODE_ID '0'

typedef enum {
    ToServer   = 'c',
    FromServer = 's'
}DataSource;

typedef enum {
	ServerRequestAlarmThreshold          			= 0x00,			//alarm threadhold
	ServerRequestSystemOptions           			= 0x01,			//control reference
	ServerRequestCoolDownGrade           			= 0x02,			//16 levels
	ServerRequestAmbientTemperatureWeek  			= 0x03,
	ServerRequestVentilateFansRunCtrl    			= 0x04,     //5 levels
	ServerRequestVentilate               			= 0x05,
	ServerRequestBlackBoxData									= 0x15,
	ServerRequestIlluminationStrength					= 0x17,

	ClientRespondAlarmThreshold			 					= 0x06,			//alarm threadhold
	ClientRespondSystemOptions			 					= 0x07,			//control reference
	ClientRespondCoolDownGrade			 					= 0x08,			//16 levels
	ClientRespondAmbientTemperatureWeek  			= 0x09,			//
	ClientRespondVentilationFansRunCtrl  			= 0x0A,			//5 levels
	ClientRespondVentilate				 						= 0x0B,
	ClientRespondBlackBoxData									= 0x16,
	ClientRespondIlluminationStrength					= 0x18,
	
	ServerSetRefAlarmThreshold			 					= 0x0C,
	ServerSetRefSystemOptions			 						= 0x0D,
	ServerSetRefCoolDownGrade			 						= 0x0E,
	ServerSetRefAmbientTemperatureWeek	 			= 0x0F,
	ServerSetRefVentilateFansRunCtrl	 				= 0x10,
	ServerSetRefVentilate				 							= 0x11,
	ServerSetIlluminationStrength							= 0x19,

	ClientBroadcastRealTimeStatus        			= 0x12,
	ClientBroadcastLog                        = 0x50,
	
	ServerEnableMQTTLog												= 0x13,
	ServerCtrlSystem													= 0x14
}PackageType;

typedef struct {
	char codeID;
	char sep0;
    char dataSource;
	char sep1;
	char deviceID[24];
}SubscribeOrPublishTopic;

typedef struct {
	char packageType;
}ReceiveMsgHeader;

#endif
