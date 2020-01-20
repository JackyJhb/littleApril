#ifndef  PRO_WRAPPER_H
#define  PRO_WRAPPER_H
#include "stdint.h"
#pragma pack(1)
//Server order define

#define 	SERVER_REQ_TEMPERATURE          0xBB             //����\��Ӧ�¶�
#define 	SERVER_REQ_HUIMIDITY_INSIDE     0x65             //����\��Ӧ�ڲ�ʪ�ȣ��Ƴ���
#define 	SERVER_REQ_HUIMIDITY_OUTSIDE    0x56             //����\��Ӧ�ⲿʪ�ȣ��Ƴ���
#define 	SERVER_REQ_ILLUMINANCY          0x73             //����\��Ӧ����ǿ��
#define 	SERVER_RELAY_CTRL               0xAA             //����\��Ӧ�̵����������
#define 	SERVER_REQ_OUTPUTSTATUS         0xC6             //����\��Ӧ���״̬��ȡ


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
