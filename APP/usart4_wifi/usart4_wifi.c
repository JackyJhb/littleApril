
#include "usart4_wifi.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 

#include "includes.h"

/*********************************************************************************
**********************************************************************************
* �ļ�����: lte.c                                                                *
* �ļ�������WIFIʹ��                            ��                                *
**********************************************************************************
*********************************************************************************/	

u8 receive_str[UART4_REC_NUM];     																			//���ջ�������,���USART_REC_LEN���ֽ� 
u8 uart_bytes_count=0;
u8 APP_mode=0;          																								//APP����ģʽ  0�����������  1�����շ�����

unsigned char MODE[]			="AT+CWMODE=3\r\n";
unsigned char Router[]			="AT+CWSAP=\"littleApril_wifi\",\"15940146108\",11,4\r\n";  //���ó�·���� ����ΪlittleApril_wifi ����15940146108
unsigned char RST[]				="AT+RST\r\n";
unsigned char M_Connection[]="AT+CIPMUX=1\r\n";
unsigned char SERVER[]		="AT+CIPSERVER=1,5000\r\n";											//�˿ں�5000
unsigned char SEND[]			="AT+CIPSEND=\r\n";														//AT+CIPSEND= ��������

/****************************************************************************
* ��    ��: void uart4_init(u32 bound)
* ��    �ܣ�LTE_uart4��ʼ��
* ��ڲ�����bound��������   
* ���ز�������
* ˵    ���� 
****************************************************************************/
void uart4_init(u32 bound)
{   
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);										//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);										//ʹ��USART4ʱ�� 
	USART_DeInit(UART4);																										//��λ����4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);										//GPIOA0����ΪUSART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);										//GPIOA1����ΪUSART4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;												//GPIOA0��GPIOA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;															//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;													//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;														//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;															//����
	GPIO_Init(GPIOA,&GPIO_InitStructure);																			//��ʼ��PA9��PA10
	USART_InitStructure.USART_BaudRate = bound;																//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;									//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;												//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;													//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//�շ�ģʽ
	USART_Init(UART4, &USART_InitStructure);																		//��ʼ������1	
	USART_Cmd(UART4, ENABLE);																						//ʹ�ܴ���4 
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);														//��������ж�
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;														//����4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;													//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;															//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;														//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);																							//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

//ʹ��ESP8266 ������CH_PDΪ��
void ESP8266_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_1);
}

//����1����һ���ַ�
void uart4SendChar(u8 ch)
{      
	while((UART4->SR&0x40)==0);  
    UART4->DR = (u8) ch;      
}
/****************************************************************************
* ��    ��: void uart1SendChars(u8 *str, u16 strlen)
* ��    �ܣ�����1����һ�ַ���
* ��ڲ�����*str�����͵��ַ���
            strlen���ַ�������
* ���ز�������
* ˵    ���� 
****************************************************************************/
void uart4SendChars(u8 *str, u16 strlen)
{ 
	u16 k= 0 ; 
	do
	{
		uart4SendChar(*(str + k));
		k++;
	}   //ѭ������,ֱ���������   
	while (k < strlen);
} 

void WIFI_Server_Init(void)
{
	OS_ERR err;
	uart4_init(115200);																	//���ڳ�ʼ��������Ϊ115200  wifiģ��������õĲ�����Ϊ115200
	uart4SendChars(MODE,sizeof(MODE));   
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);				//��ʱ1s
	uart4SendChars(Router,sizeof(Router));										//����wifiģ���·���� ��Ӧ��·�����ָ�����
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);				//���ⲽ�ֻ����ߵ��ԾͿ����ѵ�����Ϊqiming_wifi��wifi
	uart4SendChars(RST,sizeof(RST));											//����ģ��
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);				//��ʱ1s
	uart4SendChars(M_Connection,sizeof(M_Connection));				//����������
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);				//��ʱ1s
	uart4SendChars(SERVER,sizeof(SERVER));								//���óɷ����� �� ���ö˿ں�5000  
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);				//���ⲽ���ֻ�APP�������õ�IP���˿ںžͿ���������  wifiģ���IPһ��̶�Ϊ192.168.4.1
}
//����1�жϷ������
void UART4_IRQHandler(void)  
{
	u8 rec_data;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)		//�����ж� 
		{
				rec_data =(u8)USART_ReceiveData(UART4);         		//(USART1->DR) ��ȡ���յ�������

      /*  if(rec_data=='S')		  	                         //�����S����ʾ��������Ϣ����ʼλ
				{
					uart_byte_count=0x01; 
				}

			else if(rec_data=='E')		                         //���E����ʾ��������Ϣ���͵Ľ���λ
				{
					if(strcmp("open_led0",(char *)receive_str)==0)         LED0=0;	   //����LED0
					else if(strcmp("close_led0",(char *)receive_str)==0)   LED0=1;	   //����LED0
					
					else if(strcmp("open_led1",(char *)receive_str)==0)    LED1=0;	   //����LED1
					else if(strcmp("close_led1",(char *)receive_str)==0)   LED1=1;	   //����LED1
					
					else if(strcmp("open_led2",(char *)receive_str)==0)    LED2=0;	   //����LED2
					else if(strcmp("close_led2",(char *)receive_str)==0)   LED2=1;	   //����LED2
					
					else if(strcmp("open_beep",(char *)receive_str)==0)    BEEP=1; 	   //��������
					else if(strcmp("close_beep",(char *)receive_str)==0)   BEEP=0; 	   //����������
					
					else if(strcmp("app_mode1",(char *)receive_str)==0)    
					    {
								APP_mode=0; 
					      LCD_DisplayString(30,140,16,"APP_mode: 0  ");
			          LCD_DisplayString(30,160,16,"Wait APP Control");
								LCD_Fill_onecolor(0,180,239,319,WHITE);
					    }//APPΪ״̬������
					else if(strcmp("app_mode2",(char *)receive_str)==0)    
					    {
								APP_mode=1;
								LCD_DisplayString(30,140,16,"APP_mode: 1  ");
			          LCD_DisplayString(30,160,16,"Receive and send");
								LCD_Fill_onecolor(0,180,239,319,WHITE);
					    }//APPΪ���շ�����
					
					else if(receive_str[0]=='T')   //ʱ��У׼
					    {						
								RTC_DateStruct.RTC_Year= (receive_str[3]-'0')*10 + (receive_str[4]-'0');						
								RTC_DateStruct.RTC_Month= (receive_str[5]-'0')*10 + (receive_str[6]-'0');							
								RTC_DateStruct.RTC_Date= (receive_str[7]-'0')*10 + (receive_str[8]-'0');							
								RTC_TimeStruct.RTC_Hours= (receive_str[9]-'0')*10 + (receive_str[10]-'0');						
								RTC_TimeStruct.RTC_Minutes= (receive_str[11]-'0')*10 + (receive_str[12]-'0');					
								RTC_TimeStruct.RTC_Seconds= (receive_str[13]-'0')*10 + (receive_str[14]-'0');		
		
								RTC_SetTimes(RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
								delay_ms(100);
								uart4SendChars("AT+CIPSEND=0,10\r\n",16);
					      delay_ms(500);
								uart4SendChars("correct_OK",10);
							}
					
				  if(APP_mode==1)   //APPΪ���շ�����ʱ����ʾAPP����������  ����һ��Ҫ��S��ͷ E��β
					  {
					    LCD_DisplayString(30,190,16,receive_str);	 //���Ը����Լ����͵����� д��Ӧ�Ķ��� ��ǰ��ı�д����
					  }
						
					for(uart_byte_count=0;uart_byte_count<32;uart_byte_count++)receive_str[uart_byte_count]=0x00;
					uart_byte_count=0;    
				}				  
			else if((uart_byte_count>0)&&(uart_byte_count<=UART4_REC_NUM))*/
				{
				   receive_str[uart_bytes_count-1]=rec_data;
				   uart_bytes_count++;
				}            
   } 
} 


