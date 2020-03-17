
#include "wifi.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 
#include "includes.h"
#include "sccf.h"
#include "protocol.h"

char receiveBuf[UART4_REC_NUM];
char bufSplit[10][100];
char usartTimer,uartBytesCount = 0;
//AT+CIFSR 查看IP地址
//AT+CIPSTART=0,"TCP","192.168.43.102",8080  TCP连接到服务器
//
char mode[]						= "AT+CWMODE=1\r\n";
char rst[]							= "AT+RST\r\n";
char connectToRouterOrder[] = "AT+CWJAP=\"";  
/*
->AT+CWJAP="Nokia 7","lianke611" 
<-WIFI DISCONNECT
	+CWJAP:3
	FAIL               //Wifi name error
<-+CWJAP:1           //Wifi passwd error
	FAIL
<-WIFI DISCONNECT
	WIFI CONNECTED
	WIFI GOT IP
	OK
*/

char muxConnection[]		= "AT+CIPMUX=1\r\n";
char server[]					= "AT+CIPSERVER=1\r\n";
char connectToServerOrder[] = "AT+CIPSTART=0,\"TCP\",\"";
char routerName[]      = "Nokia 7";
char routerPasswd[]    = "lianke611";
//char serverIP[]        = "121.36.75.193";
char serverIP[]        = "192.168.43.102";
//char serverPort[]      = "16888";
char serverPort[]      = "8080";
/*
->AT+CIPSTART=0,"TCP","192.168.43.102",8080
<-ERROR
	0,CLOSED
<-0,CONNECT
	OK
*/
char sendDatasOrder[]	= "AT+CIPSEND=0,";
char searchWifi[]			=	"AT+CWLAP\r\n";
char endCode[]        = "\r\n";

void sendChar(uint8_t ch);
void sendChars(uint8_t *str, uint16_t strlen);
void sendStr(char *str);
void split(char *src, const char *separator, char **dest, int *num);
void usartWifiInit(u32 bound);
uint8_t switchESP8266Mode(void);
uint8_t searchRouter(void);
uint8_t resetESP8266(void);
uint8_t connectToRouter(void);
uint8_t setMuxConnection(void);
uint8_t setServerMode(void);
uint8_t connectToServer(void);
uint8_t waitForAnswer(char *cmpSrcPtr,uint16_t delayms,uint8_t usartDelay);
void sendDatas(char *buf,uint16_t len);

void split(char *src, const char *separator, char **dest, int *num) 
{
	char *pNext;
	int count = 0;
	if (src == NULL || strlen(src) == 0)
		return; 
	if (separator == NULL || strlen(separator) == 0)
		return;
  pNext = (char *)strtok(src,separator);
	while(pNext != NULL)
	{
		*dest++ = pNext;
		++count;
		pNext = (char *)strtok(NULL,separator);
	}  
	*num = count;
}

void sendChar(uint8_t ch)
{      
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET)
	{
	}
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
void sendStr(char *str)
{
	char i = 0;
	while(*(str+i) != 0)
	{
		sendChar(*(str+i));
		++i;
	}
}

uint8_t searchESP8266(void)
{
	sendStr("ATE0\r\n");
	return waitForAnswer("OK",100,3);
}

uint8_t switchESP8266Mode(void)
{
	sendStr(mode);
	return waitForAnswer("OK",100,3);
}

uint8_t resetESP8266(void)
{
	sendStr(rst);
	return waitForAnswer("ready",600,3);
}

uint8_t searchRouter(void)
{
	sendStr(searchWifi);
	return waitForAnswer(dataStore.ctrlParameter.esp8266Options.routerName,500,5);
}

uint8_t connectToRouter(void)
{
	sendStr(connectToRouterOrder);
	sendStr(dataStore.ctrlParameter.esp8266Options.routerName);
	sendStr("\",\"");
	sendStr(dataStore.ctrlParameter.esp8266Options.routerPasswd);
	sendStr("\"\r\n");
	return waitForAnswer("OK",100,3);
}
uint8_t setMuxConnection(void)
{
	sendStr(muxConnection);
	return waitForAnswer("OK",100,3);
}

uint8_t setServerMode(void)
{
	sendStr(server);
	return waitForAnswer("OK",100,3);
}

uint8_t connectToServer(void)
{
	sendStr(connectToServerOrder);
	sendStr(dataStore.ctrlParameter.esp8266Options.serverIP);
	sendStr("\",");
	sendStr(dataStore.ctrlParameter.esp8266Options.serverPort);
	sendStr(endCode);
	return waitForAnswer("OK",500,5);
}

void usartWifiInit(u32 bound)
{   
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
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

uint8_t waitForAnswer(char *cmpSrcPtr,uint16_t delayms,uint8_t usartDelay)
{
	OS_ERR err;
	uint16_t wait_timer = 0;
	do
	{
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
		if (++usartTimer > usartDelay && uartBytesCount)
		{
			receiveBuf[uartBytesCount] = 0x00;
			if (strstr(receiveBuf,cmpSrcPtr) != NULL)
			{
				uartBytesCount = 0;
				return NO_ERR;
			}
			if (strstr(receiveBuf,"SEND OK") != NULL)
			{
				uartBytesCount = 0;
				return Datas_Send_OK;
			}
			if (strstr(receiveBuf,"WIFI DISCONNECT") != NULL)
			{
				uartBytesCount = 0;
				dataStore.realtimeData.netWorkStatus = ConnectingToServer;
				return Wifi_Disconnect;
			}
			if (strstr(receiveBuf,"link is not valid") != NULL)
			{
				uartBytesCount = 0;
				dataStore.realtimeData.netWorkStatus = ConnectingToServer;
				return TCP_Link_Broken;
			}
		}
	} while (++wait_timer < delayms);
	uartBytesCount = 0;
	return 1;
}

void WIFI_Server_Init(void)
{
	char i=0;
	if ((dataStore.ctrlParameter.esp8266Options.setByUser != WIFI_REF_SET)
			&& (dataStore.realtimeData.netWorkStatus == NotFoundESP8266))
	{
		strcpy(dataStore.ctrlParameter.esp8266Options.routerName,routerName);
		strcpy(dataStore.ctrlParameter.esp8266Options.routerPasswd,routerPasswd);
		strcpy(dataStore.ctrlParameter.esp8266Options.serverIP,serverIP);
		strcpy(dataStore.ctrlParameter.esp8266Options.serverPort,serverPort);
	}
	uartBytesCount = 0;
	switch (dataStore.realtimeData.netWorkStatus)
	{
		case NotFoundESP8266:
			while (searchESP8266() && ++i < MAX_RETRY_TIMES);
			if (i < MAX_RETRY_TIMES)
			{
				dataStore.realtimeData.netWorkStatus = SettingCWMODE;
			}
			break;
		case SettingCWMODE:
			while (switchESP8266Mode() && ++i < MAX_RETRY_TIMES);
			if (i < MAX_RETRY_TIMES)
			{
				dataStore.realtimeData.netWorkStatus = Reseting;
			}
			break;
		case Reseting:
			while (resetESP8266() && ++i < MAX_RETRY_TIMES);
			if (i < MAX_RETRY_TIMES)
			{
				//dataStore.realtimeData.netWorkStatus = FoundRouter;
				dataStore.realtimeData.netWorkStatus = ConnectingToRouter;
			}
			break;
		case FoundRouter:
			while (searchRouter() && ++i < MAX_RETRY_TIMES);
			if (i < MAX_RETRY_TIMES)
			{
				dataStore.realtimeData.netWorkStatus = ConnectingToRouter;
			}
			break;
		case ConnectingToRouter:
			while (connectToRouter() && ++i < MAX_RETRY_TIMES);
			if (i < MAX_RETRY_TIMES)
			{
				dataStore.realtimeData.netWorkStatus = ConnectingToServer;
			}
			break;
		case ConnectingToServer:
			while (setMuxConnection() && ++i < MAX_RETRY_TIMES);
			if (i == MAX_RETRY_TIMES)
			{
				//dataStore.realtimeData.netWorkStatus = Reseting;
			}
			i = 0;
			while (setServerMode() && ++i < MAX_RETRY_TIMES);
			if (i == MAX_RETRY_TIMES)
			{
				//dataStore.realtimeData.netWorkStatus = Reseting;
			}
			i = 0;
			while (connectToServer() && ++i < MAX_RETRY_TIMES);
			if (i == MAX_RETRY_TIMES)
			{
				//dataStore.realtimeData.netWorkStatus = Reseting;
			}
			i = 0;
			dataStore.realtimeData.netWorkStatus = ServerConnected;
			break;
		case ServerConnected:
			break;
		default:
			break;
	}
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

void sendDatas(char *buf,uint16_t len)
{
	char temp[5],i;
	sendStr(sendDatasOrder);
	sprintf(temp,"%d",len);
	sendStr(temp);
	sendStr("\r\n");
	waitForAnswer(">",100,3);
	for (i = 0;i < len;i++)
	{
		sendChar(*(buf+i));
	}
	waitForAnswer("SEND OK",100,3);
}

