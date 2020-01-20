
#include "usart_task.h"
#include "usart.h"
#include "debug_config.h"

OS_TCB USARTTaskTCB;
CPU_STK USART_TASK_STK[USART_STK_SIZE];

void Usart_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	char * pMsg;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	while(1)
	{
		pMsg = OSTaskQPend ((OS_TICK        )0,
							(OS_OPT         )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE   *)&msg_size,
							(CPU_TS        *)0,
							(OS_ERR       *)&err);

		OSMemPut ((OS_MEM  *)&mymem,
							(void    *)pMsg,
							(OS_ERR  *)&err);
	}
}
