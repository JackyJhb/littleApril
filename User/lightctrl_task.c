#include "lightctrl_task.h"
#include "task_monitor.h"

OS_TCB LightCtrlTaskTCB;
CPU_STK LIGHTCTRL_TASK_STK[LIGHTCTRL_STK_SIZE];
void lightctrl_task(void *p_arg);

void lightctrl_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	CPU_INT32U     cpu_clk_freq;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	enableWatchDog(LIGHTCTRL_TASK_WD);
	while(1)
	{
		feedWatchDog(LIGHTCTRL_TASK_WD);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	}
}
