#include "wifi_task.h"
#include "wifi.h"
#include "24c02.h"
#include "sccf.h"

OS_TCB WIFITaskTCB;
CPU_STK WIFI_TASK_STK[WIFI_STK_SIZE];

void WIFI_task(void *p_arg)
{
	OS_ERR err;
	char buf[] = "I just want to fuck her.It's so easy!I think she is very crazy now.It is wanderful!!!";
	p_arg = p_arg;
	dataStore.realtimeData.netWorkStatus = NotFoundESP8266;
	usartWifiInit(115200);
	while(1)
	{
		if (dataStore.realtimeData.netWorkStatus != ServerConnected)
		{
			WIFI_Server_Init();
		}
		else
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_DLY,&err);
			sendDatas(buf,sizeof(buf));
		}
	}
}
