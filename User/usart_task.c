
#include "usart_task.h"
#include "usart.h"
#include "debug_config.h"
#include "task_monitor.h"

OS_TCB USARTTaskTCB;
CPU_STK USART_TASK_STK[USART_STK_SIZE];

void usart_task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE    msg_size;
	char * p_mem_blk;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	uart_byte_count = 0;
	enableWatchDog(USART_TASK_WD);
	while(1)
	{
		feedWatchDog(USART_TASK_WD);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_DLY,&err);
		if (++usart_timer >= 2 && uart_byte_count)
		{	
			if (RS485_receive_str[1] != 0x04)
			{
				uart_byte_count = uart_byte_count;
			}
			p_mem_blk = OSMemGet((OS_MEM      *)&mymem,
								(OS_ERR      *)&err);
			if (err == OS_ERR_NONE)
			{
				//发布任务消息到任务 USARTTaskTCB 
				memcpy(p_mem_blk,RS485_receive_str,uart_byte_count);
				OSTaskQPost ((OS_TCB      *)&TSTaskTCB,           //目标任务的控制块
							(void        *)p_mem_blk,             //消息内容的首地址
							(OS_MSG_SIZE  )uart_byte_count,       //消息长度
							(OS_OPT       )OS_OPT_POST_FIFO,      //发布到任务消息队列的入口端
							(OS_ERR      *)&err);                 //返回错误类型
				if (err != OS_ERR_NONE)
				{
					OSMemPut ((OS_MEM  *)&mymem,                            //指向内存管理对象
							(void    *)p_mem_blk,                                               //内存块的首地址
							(OS_ERR  *)&err);	                                      //返回错误类型
				}
			}
			else
			{
				OSMemPut ((OS_MEM  *)&mymem,                            //指向内存管理对象
							(void    *)p_mem_blk,                                               //内存块的首地址
							(OS_ERR  *)&err);	                                      //返回错误类型
			}
			uart_byte_count = 0;
			usart_timer = 0x00;
		}	
	}
}
