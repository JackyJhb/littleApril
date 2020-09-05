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
#define MOTOR_RUNNING_LIMIT  3
#define MOTOR_STOPPED_LIMIT  1

OS_TCB SideWindowCtrlTaskTCB;
CPU_STK SIDEWINDOWCTRL_TASK_STK[SIDEWINDOWCTRL_STK_SIZE];

void windowCtrl(uint8_t mode)
{
	switch (mode)
	{
		case SMALL_LEFT_OPEN:
			littleAprilHCWCtrl(Small_Left_CloseP,Off);
			littleAprilHCWCtrl(Small_Left_OpenP,On);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_CLOSE;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_LEFT_OPEN;
			break;
		case SMALL_LEFT_CLOSE:
			littleAprilHCWCtrl(Small_Left_CloseP,On);
			littleAprilHCWCtrl(Small_Left_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_LEFT_CLOSE;
			break;
		case SMALL_LEFT_STOP:
			littleAprilHCWCtrl(Small_Left_CloseP,Off);
			littleAprilHCWCtrl(Small_Left_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_CLOSE;
			break;
		case SMALL_RIGHT_OPEN:
			littleAprilHCWCtrl(Small_Right_CloseP,Off);
			littleAprilHCWCtrl(Small_Right_OpenP,On);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_CLOSE;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_RIGHT_OPEN;
			break;
		case SMALL_RIGHT_CLOSE:
			littleAprilHCWCtrl(Small_Right_CloseP,On);
			littleAprilHCWCtrl(Small_Right_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_RIGHT_CLOSE;
			break;
		case SMALL_RIGHT_STOP:
			littleAprilHCWCtrl(Small_Right_CloseP,Off);
			littleAprilHCWCtrl(Small_Right_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_CLOSE;
			break;
		case BIG_LEFT_OPEN:
			break;
		case BIG_LEFT_CLOSE:
			break;
		case BIG_LEFT_STOP:
			break;
		case BIG_RIGHT_OPEN:
			break;
		case BIG_RIGHT_CLOSE:
			break;
		case BIG_RIGHT_STOP:
			break;
		default:
			break;
	}
}

void sidewindowctrl_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	uint8_t angleLR[2],pressureLR[2],realTimeAngleTemp;
	static uint16_t last_side_window_angle = 360;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	windowCtrl(SMALL_LEFT_STOP);
	windowCtrl(SMALL_RIGHT_STOP);
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_DLY,&err);
	enableWatchDog(SIDE_WINDOW_TASK_WD);
	while (1)
	{
		feedWatchDog(SIDE_WINDOW_TASK_WD);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_DLY,&err);
		if (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle & CTRLMODELEFTMASK)
		{
			//Manual control
			angleLR[LEFT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F00) >> 8;
			realTimeAngleTemp = (dataStore.realtimeData.smallSideWindowsAngle & 0x7F00) >> 8;
			if ( (realTimeAngleTemp > (angleLR[LEFT] + MOTOR_RUNNING_LIMIT)) &&
				((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_CLOSE) == 0x0000) )
			{
				windowCtrl(SMALL_LEFT_CLOSE);
			}
			else if ( ((realTimeAngleTemp  + MOTOR_RUNNING_LIMIT) < angleLR[LEFT]) &&
					((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_OPEN) == 0x0000)	)
			{
				windowCtrl(SMALL_LEFT_OPEN);
			}
			else
			{
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_CLOSE)
				{
					if ((realTimeAngleTemp < angleLR[LEFT]) || 
						(abs(realTimeAngleTemp - angleLR[LEFT]) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SMALL_LEFT_STOP);
					}
				}
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_OPEN)
				{
					if ((realTimeAngleTemp > angleLR[LEFT]) || 
						(abs(realTimeAngleTemp - angleLR[LEFT]) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SMALL_LEFT_STOP);
					}
				}
			}
		}
		else
		{
			//Auto control
			pressureLR[LEFT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F00)>>8;
			if (dataStore.realtimeData.pressureInside > dataStore.ctrlParameter.alarmThresholdOptions.barometricPressureLowThreshold)
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
			realTimeAngleTemp = dataStore.realtimeData.smallSideWindowsAngle & 0x7F;
			if ( (realTimeAngleTemp > (angleLR[RIGHT] + MOTOR_RUNNING_LIMIT)) &&
				((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_CLOSE) == 0x0000) )
			{
				windowCtrl(SMALL_RIGHT_CLOSE);
			}
			else if( ((realTimeAngleTemp  + MOTOR_RUNNING_LIMIT) < angleLR[RIGHT]) &&
					((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_OPEN) == 0x0000))
			{
				windowCtrl(SMALL_RIGHT_OPEN);
			}
			else
			{
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_CLOSE)
				{
					if ((realTimeAngleTemp < angleLR[RIGHT]) || 
						(abs(realTimeAngleTemp - angleLR[RIGHT]) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SMALL_RIGHT_STOP);
					}
				}
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_OPEN)
				{
					if ((realTimeAngleTemp > angleLR[RIGHT]) || 
						(abs(realTimeAngleTemp - angleLR[RIGHT]) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SMALL_RIGHT_STOP);
					}
				}
			}
		}
		else
		{
			//Auto control
			pressureLR[RIGHT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F);
			if (dataStore.realtimeData.pressureInside > dataStore.ctrlParameter.alarmThresholdOptions.barometricPressureLowThreshold)
			{
				//Open left window
			}
			else
			{
				//Close left window
			}
		}
	}
}
