/*
*********************************************************************************************************
*	                                  
*	ģ������ : CAN������ʾ����
*	�ļ����� : bsp_can.h
*	��    �� : V1.1
*	˵    �� : ͷ�ļ�
* �޸�˵�� ��2017.11.20 ע�͵����õĵ�������
*********************************************************************************************************
*/
#ifndef _BSP_CAN_H
#define _BSP_CAN_H
#include "includes.h"
#include "stm32f4xx_can.h"

#define RELAY_CTL_RECEIVE_ID             0x408         //�̵�������ָ�������ID
#define RELAY_CTL_SEND_ID                 0x418        //�̵�������״̬��������ID

#define ENV_PAR_RECEIVE_ID                0x508       //���βɼ�ָ���������ID
#define ENV_PAR_SEND_ID                   0x518       //���βɼ������������ID

#define CAN_BUF_LEN	0x7ff
typedef struct
{
	CanTxMsg g_tCanTxMsg;	//���ڷ���
	CanRxMsg g_tCanRxMsg;	// ���ڽ��� 
	unsigned char  RxBuf[CAN_BUF_LEN+1];
	unsigned char  TxBuf[CAN_BUF_LEN+1];
  volatile unsigned short RxBuf_In;
	volatile unsigned short RxBuf_Out;
	volatile unsigned short TxBuf_In;
	volatile unsigned short TxBuf_Out;	
}CANStruct;


extern OS_TCB  EnvParameterTaskTCB;
extern OS_MEM  mymem;
/* ���ⲿ���õĺ������� */

extern void CAN1_Init(void);
//extern void SendCanMsg(void);  //2017.11.20ע�͵�
//extern void ProcessDataFormCAN(void);
extern u8 CAN1_Send_Msg(unsigned short int StdID,unsigned char *msg, unsigned char len,u16 time);

#endif


