
#include "wifi.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 

#include "includes.h"

uint8_t APP_mode=0;
uint8_t receiveBuf[UART4_REC_NUM];
uint8_t usartTimer,uartBytesCount = 0;

uint8_t mode[]						= "AT+CWMODE=1";
uint8_t rst[]							= "AT+RST";
uint8_t connectToRouterOrder[] = "AT+CWJAP=,";
uint8_t routerName[]      = "\"Nokia 7\"";
uint8_t routerPasswd[]    = "\"lianke611\"";
uint8_t muxConnection[]		= "AT+CIPMUX=1";
uint8_t server[]					= "AT+CIPSERVER=1";
uint8_t connectToServerOrder[] = "AT+CIPSTART=0,\"TCP\",,";
uint8_t serverIP[]        = "\"121.36.75.193\"";
uint8_t serverPort[]      = "16888";
uint8_t SendLen[]					= "AT+CIPSEND=";
uint8_t SearchWifi[]			=	"AT+CWLAP";
uint8_t endCode[]         = "\r\n";

void sendChar(uint8_t ch);
void sendChars(uint8_t *str, uint16_t strlen);

uint8_t switchESP8266Mode(uint8_t *mode);
uint8_t searchWifi(uint8_t *wifiName,uint16_t delayms);
uint8_t connectToRouter(char *routerName,char *passwd);
uint8_t connectToServer(uint8_t *ip,uint16_t *port);
uint8_t waitForAnswer(char *cmpSrcPtr,uint16_t delayms);

uint8_t switchESP8266Mode(uint8_t *mode)
{
	sendChars(mode,sizeof(mode));
	sendChars(endCode,sizeof(endCode));
	return waitForAnswer("OK",100);
}

uint8_t searchWifi(uint8_t *wifiName,uint16_t delayms)
{
	sendChars(SearchWifi,sizeof(SearchWifi));
	sendChars(endCode,sizeof(endCode));
}

void usartWifiInit(u32 bound)
{   
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	//USART_DeInit(USART6);
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

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

void sendChar(uint8_t ch)
{      
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
    USART_SendData(USART6,ch);
}

void sendChars(uint8_t *str, uint16_t strlen)
{ 
	uint16_t k = 0;
	do
	{
		sendChar(*(str + k));
		k++;
	}
	while (k < strlen);
}

uint8_t waitForAnswer(char *cmpSrcPtr,uint16_t delayms)
{
	OS_ERR err;
	uint16_t wait_timer = 0;
	do
	{
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
		if (++usartTimer > 2 && uartBytesCount)
		{
			uartBytesCount = 0;
			if (strstr(receiveBuf,cmpSrcPtr) != NULL)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	} while (++wait_timer < delayms);
	if (wait_timer == delayms)
	{
		return 2;
	}
}

void WIFI_Server_Init(void)
{
	OS_ERR err;
	usartWifiInit(115200);
	sendChars(MODE,sizeof(MODE));
	waitForAnswer("OK",100);
	sendChars(Router,sizeof(Router));
	waitForAnswer("OK",100);
	sendChars(RST,sizeof(RST));
	waitForAnswer("OK",100);
	sendChars(M_Connection,sizeof(M_Connection));
	waitForAnswer("OK",100);
	sendChars(SERVER,sizeof(SERVER));
	waitForAnswer("OK",100);
}

void USART6_IRQHandler(void)  
{
	uint8_t rec_data;
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		rec_data =(u8)USART_ReceiveData(USART6);
		usartTimer = 0x00;
		receiveBuf[uartBytesCount]=rec_data;
		if (++uartBytesCount > sizeof(receiveBuf))
		{
			uartBytesCount = 0;
		}        
	}
}


