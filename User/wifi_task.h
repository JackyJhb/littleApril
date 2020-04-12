#ifndef WIFI_TASK_H
#define WIFI_TASK_H
#include "includes.h"

#define WIFI_TASK_PRIO		17
#define WIFI_STK_SIZE 		512

#define   MQTT_INIT                   0
#define   MQTT_CONNECTING             1
#define   MQTT_SUBSCRIBE              2
#define   MQTT_SUBSCRIBING            3
#define   MQTT_CONNECTING             4
#define   MQTT_CONNECTED              5 

extern OS_TCB WIFITaskTCB;	
extern CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];
extern void WIFI_task(void *p_arg);
#endif
