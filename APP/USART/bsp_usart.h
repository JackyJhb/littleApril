/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��    
*	�ļ����� : bsp_usart.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
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
//�������ݻ�������С
#define UART_BUF_LEN	0xff

typedef struct 
{
	unsigned char  RxBuf[UART_BUF_LEN+1];  //���ն������ݻ�����
	unsigned char  TxBuf[UART_BUF_LEN+1];  //���Ͷ������ݻ�����
    volatile unsigned short RxBuf_In;	   //���ն�������λ��
	volatile unsigned short RxBuf_Out;	   //���ն��г���λ��
	volatile unsigned short TxBuf_In;	   //���Ͷ�������λ��
	volatile unsigned short TxBuf_Out;	   //���Ͷ��г���λ��
}UartStruct;

typedef struct 
{
    unsigned short       oneFrame;        //����������־
    unsigned short       dataNum;         //�յ������ݳ���
    short                dataLeft;        //����ָ�ʣ��û����ĳ���
    unsigned char        sended;          //�Ƿ��ѷ���
    unsigned char        sendType;        //��������
    unsigned char        frmBuf[1024];    //pc������ָ���������������
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


