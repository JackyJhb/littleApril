
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
				//����������Ϣ������ USARTTaskTCB 
				memcpy(p_mem_blk,RS485_receive_str,uart_byte_count);
				OSTaskQPost ((OS_TCB      *)&TSTaskTCB,           //Ŀ������Ŀ��ƿ�
							(void        *)p_mem_blk,             //��Ϣ���ݵ��׵�ַ
							(OS_MSG_SIZE  )uart_byte_count,       //��Ϣ����
							(OS_OPT       )OS_OPT_POST_FIFO,      //������������Ϣ���е���ڶ�
							(OS_ERR      *)&err);                 //���ش�������
				if (err != OS_ERR_NONE)
				{
					OSMemPut ((OS_MEM  *)&mymem,                            //ָ���ڴ�������
							(void    *)p_mem_blk,                                               //�ڴ����׵�ַ
							(OS_ERR  *)&err);	                                      //���ش�������
				}
			}
			else
			{
				OSMemPut ((OS_MEM  *)&mymem,                            //ָ���ڴ�������
							(void    *)p_mem_blk,                                               //�ڴ����׵�ַ
							(OS_ERR  *)&err);	                                      //���ش�������
			}
			uart_byte_count = 0;
			usart_timer = 0x00;
		}	
	}
}
