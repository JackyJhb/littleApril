#include "sidewindowctrl_task.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "adc.h"
#include "task_monitor.h"
#include "circleBuffer.h"

#define THRESHOLD  0

#define CTRLMODELEFTMASK   0x8000
#define CTRLMODERIGHTMASK 0x0080
#define LEFT		1
#define RIGHT	0

OS_TCB SideWindowCtrlTaskTCB;
CPU_STK SIDEWINDOWCTRL_TASK_STK[SIDEWINDOWCTRL_STK_SIZE];

void sidewindowctrl_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	uint8_t angleLR[2],pressureLR[2];
	static uint16_t last_side_window_angle = 360;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	littleAprilHCWCtrl(Window_RIGHT_CLOSE,Off);
	littleAprilHCWCtrl(Window_RIGHT_OPEN,Off);
	littleAprilHCWCtrl(Window_LEFT_CLOSE,Off);
	littleAprilHCWCtrl(Window_LEFT_OPEN,Off);
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	enableWatchDog(SIDE_WINDOW_TASK_WD);
	
	while (1)
	{
		feedWatchDog(SIDE_WINDOW_TASK_WD);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
		Get_ADC_Value();
		if (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle & CTRLMODELEFTMASK)
		{
			//Manual control
			angleLR[LEFT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F00) >> 8;
		}
		else
		{
			//Auto control
			pressureLR[LEFT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F00)>>8;
			if (dataStore.realtimeData.pressureInside > pressureLR[LEFT])
			{
				//Open left window
			}
			else
			{
				//Close left window
			}
		}
		
		if (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle & CTRLMODERIGHTMASK)
		{
			//Manual control
			angleLR[RIGHT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F) ;
		}
		else
		{
			//Auto control
			pressureLR[RIGHT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F);
			if (dataStore.realtimeData.pressureInside > pressureLR[RIGHT])
			{
				//Open left window
			}
			else
			{
				//Close left window
			}
		}
		
	/*	if (last_side_window_angle != dataStore.realtimeData.targetSideWindowsAngle)
		{
			logPrintf(Info,"I:sidewindowctrl_task.c::sidewindowCtrl_task()->Changed angle from %d to %d .\r\n",
							last_side_window_angle,
							dataStore.realtimeData.targetSideWindowsAngle);
			dataStore.realtimeData.isSideWindowMotorRunning = 0x03;
			last_side_window_angle = dataStore.realtimeData.targetSideWindowsAngle;
		}
		if (dataStore.realtimeData.isSideWindowMotorRunning & 0x01)
		{
			if (dataStore.realtimeData.realSideWindowsAngle[0] <= dataStore.realtimeData.targetSideWindowsAngle - 1)
			{
				//littleAprilIOCtrl(Window_LEFT_CLOSE,On);
				//littleAprilIOCtrl(Window_LEFT_OPEN,Off);
				#ifdef ENABLE_OUTPUT_LOG
				//printf("Left side window is opening.\r\n");
				#endif
			}
			else if (dataStore.realtimeData.realSideWindowsAngle[0] >= dataStore.realtimeData.targetSideWindowsAngle + 1)
			{
				//littleAprilIOCtrl(Window_LEFT_CLOSE,Off);
				//littleAprilIOCtrl(Window_LEFT_OPEN,On);
				#ifdef ENABLE_OUTPUT_LOG
				//printf("Left side window is closing.\r\n");
				#endif
			}
			else
			{
				dataStore.realtimeData.isSideWindowMotorRunning &= 0xFE;
				//littleAprilIOCtrl(Window_LEFT_CLOSE,Off);
				//littleAprilIOCtrl(Window_LEFT_OPEN,Off);
				#ifdef ENABLE_OUTPUT_LOG
				//printf("Left side window is stopped.\r\n");
				#endif
			}
		}
		if (dataStore.realtimeData.isSideWindowMotorRunning & 0x02)
		{
			if (dataStore.realtimeData.realSideWindowsAngle[1] <= dataStore.realtimeData.targetSideWindowsAngle - 1)
			{
				//littleAprilIOCtrl(Window_RIGHT_CLOSE,Off);
				//littleAprilIOCtrl(Window_RIGHT_OPEN,On);
				#ifdef ENABLE_OUTPUT_LOG
				//printf("Right side window is opening.\r\n");
				#endif
			}
			else if (dataStore.realtimeData.realSideWindowsAngle[1] >= dataStore.realtimeData.targetSideWindowsAngle + 1)
			{
				//littleAprilIOCtrl(Window_RIGHT_OPEN,Off);
				//littleAprilIOCtrl(Window_RIGHT_CLOSE,On);
				#ifdef ENABLE_OUTPUT_LOG
				//printf("Right side window is closing.\r\n");
				#endif
			}
			else
			{
				dataStore.realtimeData.isSideWindowMotorRunning &= 0xFD;
				//littleAprilIOCtrl(Window_RIGHT_CLOSE,Off);
				//littleAprilIOCtrl(Window_RIGHT_OPEN,Off);
				#ifdef ENABLE_OUTPUT_LOG
				//printf("Right side window is stopped.\r\n");
				#endif
			}
		}*/
	}
}
