#include "wifi_task.h"
#include "usart4_wifi.h"
#include "24c02.h"

OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];

void WIFI_task(void *p_arg)
{
	OS_ERR err;
	char wifi_status = 1;
	p_arg = p_arg;
	ESP8266_init();
	WIFI_Server_Init();
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
	}
}