#ifndef WIFI_TASK_H
#define WIFI_TASK_H
#include "includes.h"
#include "priolevel.h"

#define WIFI_STK_SIZE 		512

#define   MQTT_INIT                   0
#define   MQTT_CONNECTING             1
#define   MQTT_SUBSCRIBE              2
#define   MQTT_SUBSCRIBING            3
#define   MQTT_CONNECTING             4
#define   MQTT_CONNECTED              5
#define 	MQTT_DISCONNECT             6

extern OS_TCB WIFITaskTCB;
extern OS_MEM logMalloc;
extern CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
extern void WIFI_task(void *p_arg);
#endif
