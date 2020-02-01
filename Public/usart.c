#include "usart.h"	
#include "stdlib.h"
#include "debug_config.h"
#include "sccf.h"

//���ջ����� 	
//���ջ����� 	
uint8_t RS485_receive_str[128];   //���ջ���,���128���ֽ�.
uint8_t uart_byte_count=0,usart_timer;        //���յ������ݳ���


int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}


//��ʼ��IO ����1 
//bound:������
void USART1_Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10 ; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	char *   p_mem_blk,dat;
	OSIntEnter(); 	     //�����ж�
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		dat = USART_ReceiveData(USART1);
	} 
	OSIntExit();	        //�˳��ж�
} 	
										 
//��ʼ��IO ����2   bound:������	
void RS485_Init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);		//ʹ��USART2ʱ��

	//����2���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);		//GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);		//GPIOA3����ΪUSART2
	
	//USART2    
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;	//GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;				//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//���츴�����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				//����
	GPIO_Init(GPIOA,&GPIO_InitStructure);						//��ʼ��PA2��PA3
	
	//PG8���������485ģʽ����  
	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_8;					//GPIO8(����)GPIOG6(����)
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				//����
	GPIO_Init(GPIOG,&GPIO_InitStructure);						//��ʼ��PG8
	
	RS485_TX_EN=0;												//��ʼ��Ĭ��Ϊ����ģʽ	
	
	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;					//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); 					//��ʼ������2
	
	USART_Cmd(USART2, ENABLE);  								//ʹ�ܴ��� 2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//���������ж�
	
	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=13;		//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

//����2�����жϷ�����
void USART2_IRQHandler(void)
{
	OS_ERR   err;
	char *   p_mem_blk,dat;
	
	OSIntEnter(); 	     //�����ж�
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		usart_timer = 0x00;
		dat = (uint8_t)USART_ReceiveData(USART2);
		RS485_receive_str[uart_byte_count] = dat;
		++uart_byte_count;
		if (uart_byte_count > 100)
		{
			uart_byte_count = 0;
		}
	} 
	OSIntExit();	        //�˳��ж�
} 


/****************************************************************************
* ��    ��: void RS485_Send_Data(u8 *buf,u8 len)
* ��    �ܣ�RS485����len���ֽ�
* ��ڲ�����buf:�������׵�ַ
            len:���͵��ֽ��� 
* ���ز�������
* ˵    ����(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�����ݳ��Ȳ�Ҫ����512���ֽ�)       
****************************************************************************/	
void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t;
	if (len == 0x00)
	{
		return ;
	}
	RS485_TX_EN=1;			    //����Ϊ����ģʽ
  	for(t=0;t<len;t++)		//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //�ȴ����ͽ���		
    USART_SendData(USART2,buf[t]); //��������
	}	 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);   //�ȴ����ͽ���		
	uart_byte_count=0;	  
	RS485_TX_EN=0;				//����������Ϊ����ģʽ	
}
