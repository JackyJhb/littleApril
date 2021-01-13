
#include "wifi.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 
#include "includes.h"
#include "sccf.h"
#include "protocol.h"
#include "mqtt.h"
#include "debug_config.h"
#include "circleBuffer.h"

char receiveBuf[WIFI_BUF_SIZE],tempLen[5];
volatile uint16_t wifiBytesCount;
//AT+CIFSR ??IP???
//AT+CIPSTART=0,"TCP","192.168.43.102",8080  TCP???????????
//
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
/*char routerName[]      = "niuniu";
char routerPasswd[]    = "niuniu20090317";
/*char serverIP[]        = "192.168.43.102";
char serverPort[]      = "8080";*/
//char routerName[]      = "abc888-1";
//char routerPasswd[]    = "abc12345";*/

char routerName[]      = "ChinaNet-mWvK";
char routerPasswd[]    = "jtpsehgi";

char serverIP[]        = "121.36.75.193";
//char serverIP[]        = "192.168.1.107";
char serverPort[]      = "16888";
char mqttUserName[]      = "rkkj2020";
char mqttUserPasswd[]    = "rkkj2020rkkj";
/*
->AT+CIPSTART=0,"TCP","192.168.43.102",8080
<-ERROR
	0,CLOSED
<-0,CONNECT
	OK
*/
void clearBuf(void);
void sendChar(uint8_t ch);
void sendChars(uint8_t *str, uint16_t strlen);
void sendStr(char *str);
void usartWifiInit(u32 bound);
uint8_t switchESP8266Mode(void);
uint8_t searchRouter(void);
uint8_t resetESP8266(void);
uint8_t connectToRouter(void);
uint8_t setMuxConnection(void);
uint8_t setServerMode(void);
uint8_t connectToServer(void);
uint8_t waitForAnswer(char *cmpSrcPtr,uint16_t delayms);
uint16_t sendDatas(char *buf,uint16_t len);
int16_t getWifiRecDatas(char *saveBufPtr);

int16_t getWifiRecDatas(char *saveBufPtr)
{
	OS_ERR err;
	uint16_t len_rec;
	char *ptrLenHead,tempLen[5],*ptrData,loopTimes=0x00;
	ptrLenHead = strstr(receiveBuf,"+IPD,0,");
	if (ptrLenHead != NULL)
	{
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
		memset(tempLen,0x00,5);
		ptrLenHead += 7;
		ptrData = strstr(receiveBuf,":");
		if (ptrData != NULL)
		{
			memcpy(tempLen,ptrLenHead,abs(ptrData-ptrLenHead));
			len_rec = atoi(tempLen);
			while (loopTimes++ < 40)
			{
				OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
				if (wifiBytesCount >= len_rec + abs(ptrData-receiveBuf))
				{
					loopTimes = 0x50;
					break;
				}
			}
			if (loopTimes != 0x50)
			{
				len_rec = 0x00;
			}
			++ptrData;
			memcpy(saveBufPtr,ptrData,len_rec);
			clearBuf();
			return len_rec;
		}
	}
	ptrLenHead = strstr(receiveBuf,"CLOSED");
	if (ptrLenHead != NULL)
	{
		dataStore.realtimeData.netWorkStatus = ConnectingToServer;
		return -1;
	}
	ptrLenHead = strstr(receiveBuf,"link is not valid");
	if (ptrLenHead != NULL)
	{
		dataStore.realtimeData.netWorkStatus = ConnectingToServer;
		return -2;
	}
	return 0;
}

void clearBuf(void)
{
	wifiBytesCount = 0;
	memset(receiveBuf,0x00,sizeof(receiveBuf));
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

void sendStr(char *str)
{
	uint16_t i = 0;
	while(*(str+i) != 0)
	{
		sendChar(*(str+i));
		++i;
	}
}

uint8_t searchESP8266(void)
{
	clearBuf();
	sendStr("AT\r\n");
	return waitForAnswer("OK",100);
}

uint8_t switchESP8266Mode(void)
{
	clearBuf();
	sendStr("AT+CWMODE=1\r\n");
	return waitForAnswer("OK",100);
}

uint8_t resetESP8266(void)
{
	clearBuf();
	sendStr("AT+RST\r\n");
	//return waitForAnswer("WIFI GOT IP",1000,10);
	return waitForAnswer("OK",1000);
}

uint8_t searchRouter(void)
{
	clearBuf();
	sendStr("AT+CWLAP\r\n");
	return waitForAnswer(dataStore.ctrlParameter.esp8266Options.routerName,1000); //4s
}

uint8_t connectToRouter(void)
{
	clearBuf();
	sendStr("AT+CWJAP=\"");
	sendStr(dataStore.ctrlParameter.esp8266Options.routerName);
	sendStr("\",\"");
	sendStr(dataStore.ctrlParameter.esp8266Options.routerPasswd);
	sendStr("\"\r\n");
	return waitForAnswer("WIFI GOT IP",1000); //6s
}
uint8_t setMuxConnection(void)
{
	clearBuf();
	sendStr("AT+CIPMUX=1\r\n");
	return waitForAnswer("OK",100);
}

uint8_t setServerMode(void)
{
	clearBuf();
	sendStr("AT+CIPSERVER=1\r\n");
	return waitForAnswer("OK",100);
}

uint8_t connectToServer(void)
{
	clearBuf();
	sendStr("AT+CIPSTART=0,\"TCP\",\"");
	sendStr(dataStore.ctrlParameter.esp8266Options.serverIP);
	sendStr("\",");
	sendStr(dataStore.ctrlParameter.esp8266Options.serverPort);
	sendStr("\r\n");
	return waitForAnswer("CONNECT",100);
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void WIFI_Server_Init(void)
{
	if ((dataStore.ctrlParameter.esp8266Options.setByUser != WIFI_REF_SET)
			&& (dataStore.realtimeData.netWorkStatus == NotFoundESP8266))
	{
		strcpy(dataStore.ctrlParameter.esp8266Options.routerName,routerName);
		strcpy(dataStore.ctrlParameter.esp8266Options.routerPasswd,routerPasswd);
		strcpy(dataStore.ctrlParameter.esp8266Options.serverIP,serverIP);
		strcpy(dataStore.ctrlParameter.esp8266Options.serverPort,serverPort);
		strcpy(dataStore.ctrlParameter.esp8266Options.mqttUserName,mqttUserName);
		strcpy(dataStore.ctrlParameter.esp8266Options.mqttPasswd,mqttUserPasswd);
	}
	do
	{
		switch (dataStore.realtimeData.netWorkStatus)
		{ //link is builded
			case NotFoundESP8266:
				//logPrintf(Info,"I:wifi Step0->Searching ESP8266\r\n");
				//if (searchESP8266() == NO_ERR)
				{
					dataStore.realtimeData.netWorkStatus = SettingCWMODE;
				}
				break;
			case SettingCWMODE:
				//logPrintf(Info,"I:wifi Step1->Switch mode of ESP8266\r\n");
				if (switchESP8266Mode() == NO_ERR)
				{
					dataStore.realtimeData.netWorkStatus = Reseting;
				}
				break;
			case Reseting:
				//logPrintf(Info,"I:wifi Step2->Reset ESP8266\r\n");
				if (resetESP8266() == NO_ERR)
				{
					dataStore.realtimeData.netWorkStatus = TurnOffEcho;
				}
				break;
			case TurnOffEcho:
				//logPrintf(Info,"I:wifi Step3->Reset ESP8266\r\n");
				if (resetESP8266() == NO_ERR)
				{
					dataStore.realtimeData.netWorkStatus = FoundRouter;
				}
			case FoundRouter:
				//logPrintf(Info,"I:wifi Step4->Search router\r\n");
				if (searchRouter() == NO_ERR)
				{
					dataStore.realtimeData.netWorkStatus = ConnectingToRouter;
				}
				break;
			case ConnectingToRouter:
				//logPrintf(Info,"I:wifi Step5->Connecct to router name:%s,passwd=%s\r\n",
				//		dataStore.ctrlParameter.esp8266Options.routerName,
				//		dataStore.ctrlParameter.esp8266Options.routerPasswd);
				if (connectToRouter() == NO_ERR)
				{
					dataStore.realtimeData.netWorkStatus = ConnectingToServer;
				}
				break;
			case ConnectingToServer:
				//logPrintf(Info,"I:wifi Step6->Set mux connection\r\n");
				if (setMuxConnection() != NO_ERR)
				{
					return;
				}
				//logPrintf(Info,"I:wifi Step7->Set server mode\r\n");
				if (setServerMode() != NO_ERR)
				{
					return;
				}
				//logPrintf(Info,"I:wifi Step8->Connecting to the service->IP = %s,port = %s\r\n",
				//			dataStore.ctrlParameter.esp8266Options.serverIP,
				//			dataStore.ctrlParameter.esp8266Options.serverPort);
				if (connectToServer() != NO_ERR)
				{
					return;
				}
				dataStore.realtimeData.netWorkStatus = ServerConnected;
				break;
			case ServerConnected:
				break;
			default:
				break;
			}
		}while (dataStore.realtimeData.netWorkStatus != ServerConnected);
}

void USART6_IRQHandler(void)  
{
	uint8_t rec_data;
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		rec_data =(u8)USART_ReceiveData(USART6);
		receiveBuf[wifiBytesCount]=rec_data;
		++wifiBytesCount;
		//if (wifiBytesCount > WIFI_BUF_SIZE)
			//wifiBytesCount = 0;
	}
}

uint16_t sendDatas(char *buf,uint16_t len)
{
	char temp[5];
	sendStr("AT+CIPSEND=0,");
	sprintf(temp,"%d",len);
	sendStr(temp);
	clearBuf();
	sendStr("\r\n");
	if (waitForAnswer(">",100) != 0)
		return 0;
	sendChars((uint8_t *)buf,len);
	/*if (waitForAnswer("SEND OK",100,3) != 0)
		return 0;
	clearBuf();*/
	return len;
}

uint8_t waitForAnswer(char *cmpSrcPtr,uint16_t delayms)
{
	OS_ERR err;
	uint16_t wait_timer = 0;
	do
	{
		if (wifiBytesCount > 0)
		{
			while (wifiBytesCount > strlen(receiveBuf))
				receiveBuf[strlen(receiveBuf)] = 0x01;
			if (strstr(receiveBuf,cmpSrcPtr) != NULL)
			{
				return 0;
			}
			if (strstr(receiveBuf,"link is not valid") != NULL)
			{
				dataStore.realtimeData.netWorkStatus = ConnectingToServer;
				return TCP_Link_Broken;
			}
			if (strstr(receiveBuf,"CLOSED") != NULL)
			{
				dataStore.realtimeData.netWorkStatus = ConnectingToServer;
				return TCP_Link_Broken;
			}
			if (strstr(receiveBuf,"link is builded") != NULL)
			{
				dataStore.realtimeData.netWorkStatus = Reseting;
				return Router_Broken;
			}
			if (strstr(receiveBuf,"WIFI GOT IP") != NULL)
			{
				dataStore.realtimeData.netWorkStatus = ConnectingToServer;
				return TCP_Link_Broken;
			}
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_DLY,&err);
	} while (++wait_timer < delayms);
	return 2;
}
