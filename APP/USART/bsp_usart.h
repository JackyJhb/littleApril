/*
*********************************************************************************************************
*	                                  
*	模块名称 : 串口驱动模块    
*	文件名称 : bsp_usart.h
*	版    本 : V2.0
*	说    明 : 头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_USART_H
#define __BSP_USART_H

#define COM1   1
#define COM2   2
#define COM3   3
#define COM4   4
#define COM5   5
#define COM6   6
#define CAN    7
//队列数据缓存区大小
#define UART_BUF_LEN	0xff

typedef struct 
{
	unsigned char  RxBuf[UART_BUF_LEN+1];  //接收队列数据缓存区
	unsigned char  TxBuf[UART_BUF_LEN+1];  //发送队列数据缓存区
    volatile unsigned short RxBuf_In;	   //接收队列入列位置
	volatile unsigned short RxBuf_Out;	   //接收队列出列位置
	volatile unsigned short TxBuf_In;	   //发送队列入列位置
	volatile unsigned short TxBuf_Out;	   //发送队列出列位置
}UartStruct;

typedef struct 
{
    unsigned short       oneFrame;        //数据完整标志
    unsigned short       dataNum;         //收到的数据长度
    short                dataLeft;        //这条指令还剩下没传完的长度
    unsigned char        sended;          //是否已发送
    unsigned char        sendType;        //发送类型
    unsigned char        frmBuf[1024];    //pc传来的指令存放在这个数组里
}DipperdeFramer;


extern UartStruct Uart1Stu;
extern void bsp_InitUart(void);
extern void Uart_Printf(unsigned char UartID, const char *fmt,...);
extern void SetZigbee_BaudRate_38400(void);
extern void ProcessDataFormUart1(void);
extern void ProcessDataFormUart2(void);
extern void ProcessDataFormUart3(void);
extern int RFID_Process_TEK(unsigned char *buff,unsigned short len,unsigned short userDataLen);
extern int RFID_Process_BJF(unsigned char *buff,unsigned short len,unsigned short userDataLen);
extern unsigned int ReadUart(unsigned char UartID,unsigned char *buff,unsigned int length);
extern void WriteUart(unsigned char UartID,unsigned char *buff,unsigned int length);
#endif


