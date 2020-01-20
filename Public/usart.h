#ifndef __usart_H
#define __usart_H
#include "includes.h"
#include "system.h"
#include "stdio.h"

extern uint8_t RS485_receive_str[128]; 	//接收缓冲,最大512个字节
extern uint8_t uart_byte_count;   			  //接收到的数据长度

//模式控制
#define RS485_TX_EN		PGout(8)	    //485模式控制.0,接收;1,发送.

														 
extern void RS485_Init(uint32_t bound);
extern void RS485_Send_Data(uint8_t *buf,uint8_t len);

extern OS_TCB	 USARTTaskTCB,TSTaskTCB;
extern OS_MEM  mymem;
extern void USART1_Init(uint32_t bound);
#endif


