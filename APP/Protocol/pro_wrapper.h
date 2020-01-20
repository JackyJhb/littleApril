#ifndef  PRO_WRAPPER_H
#define  PRO_WRAPPER_H
#include "stdint.h"
#pragma pack(1)
//Server order define

#define 	SERVER_REQ_TEMPERATURE          0xBB             //请求\响应温度
#define 	SERVER_REQ_HUIMIDITY_INSIDE     0x65             //请求\响应内部湿度（移除）
#define 	SERVER_REQ_HUIMIDITY_OUTSIDE    0x56             //请求\响应外部湿度（移除）
#define 	SERVER_REQ_ILLUMINANCY          0x73             //请求\响应光照强度
#define 	SERVER_RELAY_CTRL               0xAA             //请求\响应继电器控制输出
#define 	SERVER_REQ_OUTPUTSTATUS         0xC6             //请求\响应输出状态获取


#define 	ERR_NONE                        0x00
#define 	DS18B20_LEFT_ERR                0x01
#define     DS18B20_RIGHT_ERR               0x02
#define 	HUMIDITY_SENSOR_OUTSIDE_ERR     0x04
#define 	HUMIDITY_SENSOR_INSIDE_ERR      0x08
#define 	ILLUMINANCY_SENSOR_ERR          0x10
#define 	RELAY_CTRL_ERR                  0x20

typedef struct{
	uint8_t type;
}ServerOrder;

typedef struct{
	uint16_t relayBits;
	uint16_t workFor;
}RelayCtrlData;

typedef struct{
	ServerOrder   order;
	RelayCtrlData ctrlData;
}RelayCtrlPackage;

typedef struct{
	uint8_t type;
	uint8_t dev_id;
	int16_t leftTemperature;
	int16_t rightTemperature;
	uint8_t humidity;
	uint8_t err;
}DataPackage;
#pragma pack()

#endif
