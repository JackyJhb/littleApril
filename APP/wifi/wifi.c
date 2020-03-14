
#include "wifi.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 

#include "includes.h"

/*********************************************************************************
**********************************************************************************
* 文件名称: lte.c                                                                *
* 文件简述：WIFI使用                            ·                                *
**********************************************************************************
*********************************************************************************/	

u8 receive_str[UART4_REC_NUM];     																			//接收缓存数组,最大USART_REC_LEN个字节 
u8 uart_bytes_count=0;
u8 APP_mode=0;          																								//APP控制模式  0：命令控制区  1：接收发送区

unsigned char MODE[]			="AT+CWMODE=3\r\n";
unsigned char Router[]			="AT+CWSAP=\"littleApril_wifi\",\"15940146108\",11,4\r\n";  //配置成路由器 名字为littleApril_wifi 密码15940146108
unsigned char RST[]				="AT+RST\r\n";
unsigned char M_Connection[]="AT+CIPMUX=1\r\n";
unsigned char SERVER[]		="AT+CIPSERVER=1,5000\r\n";											//端口号5000
unsigned char SEND[]			="AT+CIPSEND=\r\n";														//AT+CIPSEND= 发送数据

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

//使能ESP8266 就是置CH_PD为高
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
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)		//接收中断 
		{
				rec_data =(u8)USART_ReceiveData(USART6);         		//(USART1->DR) 读取接收到的数据

      /*  if(rec_data=='S')		  	                         //如果是S，表示是命令信息的起始位
				{
					uart_byte_count=0x01; 
				}

			else if(rec_data=='E')		                         //如果E，表示是命令信息传送的结束位
				{
					if(strcmp("open_led0",(char *)receive_str)==0)         LED0=0;	   //点亮LED0
					else if(strcmp("close_led0",(char *)receive_str)==0)   LED0=1;	   //关灭LED0
					
					else if(strcmp("open_led1",(char *)receive_str)==0)    LED1=0;	   //点亮LED1
					else if(strcmp("close_led1",(char *)receive_str)==0)   LED1=1;	   //关灭LED1
					
					else if(strcmp("open_led2",(char *)receive_str)==0)    LED2=0;	   //点亮LED2
					else if(strcmp("close_led2",(char *)receive_str)==0)   LED2=1;	   //关灭LED2
					
					else if(strcmp("open_beep",(char *)receive_str)==0)    BEEP=1; 	   //蜂鸣器响
					else if(strcmp("close_beep",(char *)receive_str)==0)   BEEP=0; 	   //蜂鸣器不响
					
					else if(strcmp("app_mode1",(char *)receive_str)==0)    
					    {
								APP_mode=0; 
					      LCD_DisplayString(30,140,16,"APP_mode: 0  ");
			          LCD_DisplayString(30,160,16,"Wait APP Control");
								LCD_Fill_onecolor(0,180,239,319,WHITE);
					    }//APP为状态控制区
					else if(strcmp("app_mode2",(char *)receive_str)==0)    
					    {
								APP_mode=1;
								LCD_DisplayString(30,140,16,"APP_mode: 1  ");
			          LCD_DisplayString(30,160,16,"Receive and send");
								LCD_Fill_onecolor(0,180,239,319,WHITE);
					    }//APP为接收发送区
					
					else if(receive_str[0]=='T')   //时间校准
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
					
				  if(APP_mode==1)   //APP为接收发送区时，显示APP发来的数据  数据一定要以S开头 E结尾
					  {
					    LCD_DisplayString(30,190,16,receive_str);	 //可以根据自己发送的数据 写相应的动作 按前面的编写方法
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


