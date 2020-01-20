#ifndef __USART4_WIFI_H
#define __USART4_WIFI_H
#include "stdio.h"	
#include "stm32f4xx.h" 

//////////////////////////////////////////////////////////////////////////////////	 

extern u8 APP_mode;

#define UART4_REC_NUM  			200  	//�����������ֽ��� 200
extern u8 uart_bytes_count;          //uart_byte_countҪС��USART_REC_LEN
extern u8 receive_str[UART4_REC_NUM]; 

void ESP8266_init(void);
void uart4_init(u32 bound);
void WIFI_Server_Init(void);


void uart4SendChars(u8 *str, u16 strlen);

#endif


