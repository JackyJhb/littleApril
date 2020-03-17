#ifndef __USART4_WIFI_H
#define __USART4_WIFI_H
#include "stdio.h"	
#include "stm32f4xx.h" 

#define UART4_REC_NUM  			1024
#define MAX_RETRY_TIMES     2

typedef enum {
	NO_ERR,
	Datas_Send_OK,
	Wifi_Disconnect,
	TCP_Link_Broken,
	Router_Broken
}ErrType;
extern char receiveBuf[UART4_REC_NUM];
extern char usartTimer,uartBytesCount;

extern void usartWifiInit(u32 bound);
extern void WIFI_Server_Init(void);
extern void sendDatas(char *buf,uint16_t len);

#endif


