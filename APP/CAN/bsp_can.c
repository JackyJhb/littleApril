#include "includes.h"
#include "bsp_can.h"
#include "debug_config.h"
#include "sccf.h"
#include "pro_wrapper.h"
#include "ventilation_task.h"
#include "circleBuffer.h"

CANStruct can1Struct;
CanRxMsg g_tCanRxMsg;
CanRxMsg  rx1Msg;
CanTxMsg TxMessage;
extern OS_SEM WaitForGyroSenRepSem,WaitForMagneticSenRepSem;

void CAN1_Init(void);

void CAN1_Init(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef       NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1);

	CAN_InitStructure.CAN_TTCM   = DISABLE;
	CAN_InitStructure.CAN_ABOM  = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
	CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1  = CAN_BS1_7tq;
	CAN_InitStructure.CAN_BS2  = CAN_BS2_6tq;
	CAN_InitStructure.CAN_Prescaler = 96;
	CAN_Init(CAN1, &CAN_InitStructure);
	//baundrate = 168000000/4/96/(1+7+6) = 31250

	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = (uint32_t)RELAY_CTL_SEND_ID<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow  = (((uint32_t)RELAY_CTL_SEND_ID<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/*CAN_FilterInitStructure.CAN_FilterNumber = 1;									//������1
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdList; 
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit; 		//32λ 
	//CAN_FilterInitStructure.CAN_FilterIdHigh = (uint32_t)ENV_PAR_SEND<<5;   //0x0581~0x0584
	//CAN_FilterInitStructure.CAN_FilterIdLow  = (((uint32_t)ENV_PAR_SEND<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;//0x0000; 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;						//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;						//���������0
	CAN_FilterInit(&CAN_FilterInitStructure);												//�˲�����ʼ��

	CAN_FilterInitStructure.CAN_FilterNumber = 2;									//������2
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdList; 
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit;		//32λ 
	//CAN_FilterInitStructure.CAN_FilterIdHigh = (uint32_t)MOTOR_RIGHT_QUICPSTOP_GET<<5;   //0x0581~0x0584
	//CAN_FilterInitStructure.CAN_FilterIdLow  = (((uint32_t)MOTOR_RIGHT_QUICPSTOP_GET<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;//0x0000; 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;						//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;						//���������0
	CAN_FilterInit(&CAN_FilterInitStructure);												//�˲�����ʼ��
	
	CAN_FilterInitStructure.CAN_FilterNumber = 3;									//������3
	CAN_FilterInitStructure.CAN_FilterMode   = CAN_FilterMode_IdList; 
	CAN_FilterInitStructure.CAN_FilterScale  = CAN_FilterScale_32bit;		//32λ 
	//CAN_FilterInitStructure.CAN_FilterIdHigh = (uint32_t)MOTOR_LEFT_QUICKSTOP_GET<<5;   //0x0581~0x0584
	//CAN_FilterInitStructure.CAN_FilterIdLow  = (((uint32_t)MOTOR_LEFT_QUICKSTOP_GET<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;//0x0000; 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;						//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;						//���������0
	CAN_FilterInit(&CAN_FilterInitStructure);												//�˲�����ʼ��
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.	

	CAN_FilterInitStructure.CAN_FilterNumber = 4;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;//CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	//CAN_FilterInitStructure.CAN_FilterIdHigh = GYRO_SENSOR_SEND_ID<<5; //�ŵ���������ID
	//CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)GYRO_SENSOR_SEND_ID<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF; 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);*/

	CAN_FilterInitStructure.CAN_FilterNumber = 5;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = ENV_PAR_SEND_ID<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)ENV_PAR_SEND_ID<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	CAN_ITConfig(CAN1,CAN_IT_FMP1,ENABLE);
	can1Struct.RxBuf_In  = 0;
	can1Struct.RxBuf_Out = 0;
	can1Struct.TxBuf_In  = 0;
	can1Struct.TxBuf_Out = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: can_ISR
*	����˵��: CAN�жϷ������. ��������� stm32f4xx_it.c�б�����
*	��    �Σ���
*	�� �� ֵ: ��
* �޸�˵����2017/11/20
*           1����if���ƽṹ�޸�Ϊswitch ... case ...�ṹ��
*           2������Ŷ���ߵ����������ݽ��շ��ʹ������̣�
*              ͨ����Ϣ���з��͸����߿��ƹ��̣�
*********************************************************************************************************
*/ 
void CAN1_RX0_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0) != RESET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &g_tCanRxMsg);
		CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
		switch (g_tCanRxMsg.StdId)
		{
			case RELAY_CTL_SEND_ID:
				break;
			case RELAY_CTL_RECEIVE_ID:
				break;
			default:
				break;
		}
	}
}

void CAN1_RX1_IRQHandler(void)
{
	OS_ERR   err;
	char *  p_mem_blk;
	OSIntEnter(); 	                                                                                            //�����ж�
	/*�����ж�*/
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP1) != RESET)
	{
		CAN_Receive(CAN1, CAN_FIFO1, &rx1Msg);
		logPrintf(Info,"Info:bsp_can.c::CAN1_RX1_IRQHandler->Free memory blocks total numbers is %d \r\n",
				((OS_MEM      *)&mymem)->NbrFree);
		#ifdef ENABLE_BLACK_BOX
		if (((OS_MEM      *)&mymem)->NbrFree < 10)
		{
			++dataStore.blackBox.memTooLowTimes;
		}
		#endif
		switch (rx1Msg.StdId)
		{
			case ENV_PAR_SEND_ID:
				if ((rx1Msg.Data[0] == SERVER_RELAY_CTRL) && (rx1Msg.Data[1] == 0x55))
				{
					p_mem_blk = OSMemGet((OS_MEM *)&mymem,(OS_ERR *)&err);  //���ڴ���� mem ��ȡһ���ڴ��
				
					if (err != OS_ERR_NONE)
					{
						logPrintf(Error,"Error:bsp_can.c::CAN1_RX1_IRQHandler->OSMemGet() err = %d \r\n",err);
						break;
					}
					memcpy(p_mem_blk,rx1Msg.Data,rx1Msg.DLC);
					OSTaskQPost((OS_TCB      *)&VentilationTaskTCB,
								(void        *)p_mem_blk,
								(OS_MSG_SIZE  )rx1Msg.DLC,
								(OS_OPT       )OS_OPT_POST_FIFO,
								(OS_ERR      *)&err);
					logPrintf(Info,"Info:bsp_can.c::CAN1_RX1_IRQHandler->Relay control respond post \r\n");
				}
				else
				{
					p_mem_blk = OSMemGet((OS_MEM *)&mymem,(OS_ERR *)&err);  //���ڴ���� mem ��ȡһ���ڴ��
					if (err != OS_ERR_NONE)
					{
						logPrintf(Error,"Error:bsp_can.c::CAN1_RX1_IRQHandler->OSMemGet() err = %d \r\n",err);
						break;
					}
					memcpy(p_mem_blk,rx1Msg.Data,rx1Msg.DLC);
					OSTaskQPost((OS_TCB      *)&EnvParameterTaskTCB,
								(void        *)p_mem_blk,
								(OS_MSG_SIZE  )rx1Msg.DLC,
								(OS_OPT       )OS_OPT_POST_FIFO,
								(OS_ERR      *)&err);
					logPrintf(Info,"Info:bsp_can.c::CAN1_RX1_IRQHandler->Env parameter ask respond post \r\n");
				}
				if (err != OS_ERR_NONE)
				{
					logPrintf(Error,"Error:bsp_can.c::CAN1_RX1_IRQHandler->OSTaskQPost() err = %d \r\n",err);
					OSMemPut ((OS_MEM *)&mymem,(void *)p_mem_blk,(OS_ERR *)&err);
				}
				break;
			case ENV_PAR_RECEIVE_ID:
				break;
			default:
				break;
		}
		CAN_ClearITPendingBit(CAN1,CAN_IT_FMP1);
		CAN_ClearFlag(CAN1,CAN_IT_FMP1);
		CAN_FIFORelease(CAN1,CAN_FIFO1);//
	} 
	OSIntExit();	        //�˳��ж�
}

void CAN1_TX_IRQHandler(void)
{
	 CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
	 if(CAN_GetITStatus(CAN1,CAN_IT_RQCP0) == SET)
	 {
		CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP0);
	 }	 
	 else if(CAN_GetITStatus(CAN1,CAN_IT_RQCP1) == SET)
	 {
		CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP1);
	 }
	 else if(CAN_GetITStatus(CAN1,CAN_IT_RQCP2) == SET)
	 {
		CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP2);
	 }
	 CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);
}
//CAN���ݷ���
u8 CAN1_Send_Msg(unsigned short int StdID,unsigned char *msg, unsigned char len,u16 time)
{
	OS_ERR      err;
	unsigned int  i = 0;
	TxMessage.StdId = StdID; // ��׼��ʶ��Ϊ0
	TxMessage.ExtId = 0x12;	 // ������չ��ʾ����29λ��
	TxMessage.IDE   = 0;	 // ʹ����չ��ʶ��
	TxMessage.RTR   = 0;	 // ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC   = len;

	for(i = 0; i < len; i++)
	{
			TxMessage.Data[i] = msg[i]; 
	}
	CAN_Transmit(CAN1, &TxMessage); 
	OSTimeDly(time, OS_OPT_TIME_DLY, &err);//���������ʱ�������ʱ��Ϊ0�����������ʱʱ���ڻ��յ�CAN RX�ж�

	return 0;
}
