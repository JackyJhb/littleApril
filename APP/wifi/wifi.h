#ifndef __USART4_WIFI_H
#define __USART4_WIFI_H
#include "stdio.h"	
#include "stm32f4xx.h" 

#define WIFI_BUF_SIZE  			1024
#define MAX_RETRY_TIMES     2

typedef enum {
	NO_ERR = 0,
	Datas_Send_OK,
	Wifi_Disconnect,
	TCP_Link_Broken,
	Router_Broken
}ErrType;

extern void usartWifiInit(u32 bound);
extern void WIFI_Server_Init(void);
extern uint16_t sendDatas(char *buf,uint16_t len);
extern int16_t getWifiRecDatas(char *saveBufPtr);

#endif


