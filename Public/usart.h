#ifndef __usart_H
#define __usart_H
#include "includes.h"
#include "system.h"
#include "stdio.h"

extern uint8_t RS485_receive_str[128]; 	//���ջ���,���512���ֽ�
extern uint8_t uart_byte_count;   			  //���յ������ݳ���

//ģʽ����
#define RS485_TX_EN		PGout(8)	    //485ģʽ����.0,����;1,����.

														 
extern void RS485_Init(uint32_t bound);
extern void RS485_Send_Data(uint8_t *buf,uint8_t len);

extern OS_TCB	 USARTTaskTCB,TSTaskTCB;
extern OS_MEM  mymem;
extern void USART1_Init(uint32_t bound);
#endif


