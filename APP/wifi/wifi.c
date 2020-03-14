
#include "wifi.h"
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

void usartWifiInit(u32 bound)
{   
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	USART_DeInit(USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	USART_Cmd(USART6, ENABLE);
	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//ʹ��ESP8266 ������CH_PDΪ��
void ESP8266_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_SetBits(GPIOD, GPIO_Pin_8);
}

void sendChar(u8 ch)
{      
	while((USART6->SR&0x40)==0);  
    USART6->DR = (u8) ch;      
}

void sendChars(u8 *str, u16 strlen)
{ 
	u16 k= 0 ; 
	do
	{
		sendChar(*(str + k));
		k++;
	}
	while (k < strlen);
} 

void WIFI_Server_Init(void)
{
	OS_ERR err;
	usartWifiInit(115200);
	sendChars(MODE,sizeof(MODE));   
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	sendChars(Router,sizeof(Router));
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	sendChars(RST,sizeof(RST));
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	sendChars(M_Connection,sizeof(M_Connection));
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	sendChars(SERVER,sizeof(SERVER));
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
}

void USART6_IRQHandler(void)  
{
	u8 rec_data;
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)		//�����ж� 
		{
				rec_data =(u8)USART_ReceiveData(USART6);         		//(USART1->DR) ��ȡ���յ�������

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


