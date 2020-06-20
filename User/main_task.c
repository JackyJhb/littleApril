#include "main_task.h"
#include "usart_task.h"
#include "led_task.h"
#include "envctrl_task.h"
#include "wifi_task.h"
#include "ts_task.h"
#include "rtc_task.h"
#include "ventilation_task.h"
#include "watcher_task.h"
#include "task_monitor.h"
#include "sidewindowctrl_task.h"
#include "alarm_task.h"
#include "waterpumpctrl_task.h"
#include "circleBuffer.h"
#include "boilerctrl_task.h"
#include "lightctrl_task.h"

OS_MEM mymem;
uint8_t ucArray [ 20 ] [ 40];
OS_TCB StartTaskTCB;
CPU_STK START_TASK_STK[START_STK_SIZE];

void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
	cpu_clk_freq = BSP_CPU_ClkFreq();
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;
    OS_CPU_SysTickInit(cnts);
    Mem_Init();       

#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);     
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN
	OSSchedRoundRobinCfg(DEF_ENABLED,0,&err);
#endif

	OS_CRITICAL_ENTER();
    loggerBufferInit();
	OSMemCreate ((OS_MEM      *)&mymem,
				(CPU_CHAR    *)"mymem",
				(void        *)ucArray,
				(OS_MEM_QTY   )20,
				(OS_MEM_SIZE  )40,
				(OS_ERR      *)&err);
                

	OSTaskCreate((OS_TCB 	* )&VentilationTaskTCB,		
				 (CPU_CHAR	* )"Ventilation task", 		
                 (OS_TASK_PTR )ventilation_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )VENTILATION_TASK_PRIO,     
                 (CPU_STK   * )&VENTILATION_TASK_STK[0],	
                 (CPU_STK_SIZE)VENTILATION_STK_SIZE/10,	
                 (CPU_STK_SIZE)VENTILATION_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"Led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

	OSTaskCreate((OS_TCB 	* )&USARTTaskTCB,		
				 (CPU_CHAR	* )"Usart task", 		
                 (OS_TASK_PTR )usart_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )USART_TASK_PRIO,     
                 (CPU_STK   * )&USART_TASK_STK[0],	
                 (CPU_STK_SIZE)USART_STK_SIZE/10,	
                 (CPU_STK_SIZE)USART_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

	OSTaskCreate((OS_TCB 	* )&SideWindowCtrlTaskTCB,		
				 (CPU_CHAR	* )"Side Window Ctrl task", 		
                 (OS_TASK_PTR )sidewindowctrl_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SIDEWINDOWCTRL_TASK_PRIO,     
                 (CPU_STK   * )&SIDEWINDOWCTRL_TASK_STK[0],	
                 (CPU_STK_SIZE)SIDEWINDOWCTRL_STK_SIZE/10,	
                 (CPU_STK_SIZE)SIDEWINDOWCTRL_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OSTaskCreate((OS_TCB 	* )&WATERPUMPTaskTCB,		
				 (CPU_CHAR	* )"Waterpump task", 		
                 (OS_TASK_PTR )waterpump_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )WATERPUMP_TASK_PRIO,     
                 (CPU_STK   * )&WATERPUMP_TASK_STK[0],	
                 (CPU_STK_SIZE)WATERPUMP_STK_SIZE/10,	
                 (CPU_STK_SIZE)WATERPUMP_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);			 

	OSTaskCreate((OS_TCB 	* )&EnvParameterTaskTCB,		
				 (CPU_CHAR	* )"EnvParameter task", 		
                 (OS_TASK_PTR )EnvParameter_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )EnvParameter_TASK_PRIO,     
                 (CPU_STK   * )&EnvParameter_TASK_STK[0],	
                 (CPU_STK_SIZE)EnvParameter_STK_SIZE/10,	
                 (CPU_STK_SIZE)EnvParameter_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OSTaskCreate((OS_TCB 	* )&WIFITaskTCB,		
				 (CPU_CHAR	* )"Wifi task", 		
                 (OS_TASK_PTR )WIFI_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )WIFI_TASK_PRIO,     
                 (CPU_STK   * )&WIFI_TASK_STK[0],	
                 (CPU_STK_SIZE)WIFI_STK_SIZE/10,	
                 (CPU_STK_SIZE)WIFI_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
								 
	OSTaskCreate((OS_TCB 	* )&AlarmTaskTCB,		
				 (CPU_CHAR	* )"Alarm task", 		
                 (OS_TASK_PTR )alarm_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ALARM_TASK_PRIO,     
                 (CPU_STK   * )&ALARM_TASK_STK[0],	
                 (CPU_STK_SIZE)ALARM_STK_SIZE/10,	
                 (CPU_STK_SIZE)ALARM_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OSTaskCreate((OS_TCB 	* )&MonitorTaskTCB,		
				 (CPU_CHAR	* )"Monitor task", 		
                 (OS_TASK_PTR )monitor_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )MONITOR_TASK_PRIO,     
                 (CPU_STK   * )&MONITOR_TASK_STK[0],	
                 (CPU_STK_SIZE)MONITOR_STK_SIZE/10,	
                 (CPU_STK_SIZE)MONITOR_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OSTaskCreate((OS_TCB 	* )&TSTaskTCB,		
				 (CPU_CHAR	* )"TS task", 		
                 (OS_TASK_PTR )ts_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TS_TASK_PRIO,     
                 (CPU_STK   * )&TS_TASK_STK[0],	
                 (CPU_STK_SIZE)TS_STK_SIZE/10,	
                 (CPU_STK_SIZE)TS_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OSTaskCreate((OS_TCB 	* )&RTCTaskTCB,		
				 (CPU_CHAR	* )"RTC task", 		
                 (OS_TASK_PTR )rtc_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )RTC_TASK_PRIO,     
                 (CPU_STK   * )&RTC_TASK_STK[0],	
                 (CPU_STK_SIZE)RTC_STK_SIZE/10,	
                 (CPU_STK_SIZE)RTC_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
				 
	OSTaskCreate((OS_TCB 	* )&WatcherTaskTCB,		
				 (CPU_CHAR	* )"Watcher task", 		
                 (OS_TASK_PTR )watcher_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )WATCHER_TASK_PRIO,     
                 (CPU_STK   * )&WATCHER_TASK_STK[0],	
                 (CPU_STK_SIZE)WATCHER_STK_SIZE/10,	
                 (CPU_STK_SIZE)WATCHER_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

    OSTaskCreate((OS_TCB 	* )&BoilerCtrlTaskTCB,		
				 (CPU_CHAR	* )"Boiler control task", 		
                 (OS_TASK_PTR )boilerctrl_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )BOILERCTRL_TASK_PRIO,     
                 (CPU_STK   * )&BOILERCTRL_TASK_STK[0],	
                 (CPU_STK_SIZE)BOILERCTRL_STK_SIZE/10,	
                 (CPU_STK_SIZE)BOILERCTRL_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

    OSTaskCreate((OS_TCB 	* )&LightCtrlTaskTCB,		
				 (CPU_CHAR	* )"Light control task", 		
                 (OS_TASK_PTR )lightctrl_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LIGHTCTRL_TASK_PRIO,     
                 (CPU_STK   * )&LIGHTCTRL_TASK_STK[0],	
                 (CPU_STK_SIZE)LIGHTCTRL_STK_SIZE/10,	
                 (CPU_STK_SIZE)LIGHTCTRL_STK_SIZE,		
                 (OS_MSG_QTY  )50,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);
	OS_CRITICAL_EXIT();
}
