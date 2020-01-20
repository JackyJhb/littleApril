/*
*********************************************************************************************************
*	                                  
*	模块名称 : CAN网络演示程序。
*	文件名称 : bsp_can.h
*	版    本 : V1.1
*	说    明 : 头文件
* 修改说明 ：2017.11.20 注释掉无用的导出函数
*********************************************************************************************************
*/
#ifndef _BSP_CAN_H
#define _BSP_CAN_H
#include "includes.h"
#include "stm32f4xx_can.h"

#define RELAY_CTL_RECEIVE_ID             0x408         //继电器控制指令发送邮箱ID
#define RELAY_CTL_SEND_ID                 0x418        //继电器控制状态发送邮箱ID

#define ENV_PAR_RECEIVE_ID                0x508       //环参采集指令接受邮箱ID
#define ENV_PAR_SEND_ID                   0x518       //环参采集结果发送邮箱ID

#define CAN_BUF_LEN	0x7ff
typedef struct
{
	CanTxMsg g_tCanTxMsg;	//用于发送
	CanRxMsg g_tCanRxMsg;	// 用于接收 
	unsigned char  RxBuf[CAN_BUF_LEN+1];
	unsigned char  TxBuf[CAN_BUF_LEN+1];
  volatile unsigned short RxBuf_In;
	volatile unsigned short RxBuf_Out;
	volatile unsigned short TxBuf_In;
	volatile unsigned short TxBuf_Out;	
}CANStruct;


extern OS_TCB  EnvParameterTaskTCB;
extern OS_MEM  mymem;
/* 供外部调用的函数声明 */

extern void CAN1_Init(void);
//extern void SendCanMsg(void);  //2017.11.20注释掉
//extern void ProcessDataFormCAN(void);
extern u8 CAN1_Send_Msg(unsigned short int StdID,unsigned char *msg, unsigned char len,u16 time);

#endif


