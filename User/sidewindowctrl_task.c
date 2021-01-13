#include "sidewindowctrl_task.h"
#include "sccf.h"
#include "bsp_gpio.h"
#include "adc.h"
#include "task_monitor.h"
#include "circleBuffer.h"

#define CTRLMODELEFTMASK     0x8000
#define CTRLMODERIGHTMASK    0x0080
#define LEFT				 1
#define RIGHT				 0
#define MOTOR_RUNNING_LIMIT  4
#define MOTOR_STOPPED_LIMIT  4

#define REDUCE   1
#define INCREASE 2

#define REDUCE_STEP  1
#define INCREASE_STEP 1
OS_TCB SideWindowCtrlTaskTCB;
CPU_STK SIDEWINDOWCTRL_TASK_STK[SIDEWINDOWCTRL_STK_SIZE];

void adjustSmallWinAngleBase(uint8_t fansNumber,uint8_t reduceOrIncrease);
uint8_t isSmallWinWorking(uint8_t whichOne);
uint8_t calSetBits(uint32_t fansGroup);
void smallWinAngleTo(uint8_t whichOne,uint8_t angleTarget);
void windowCtrl(WindowAction mode);

void adjustSmallWinAngleBase(uint8_t fansNumber,uint8_t reduceOrIncrease)
{
	switch (reduceOrIncrease)
	{
		case REDUCE:
			if (dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[fansNumber] > REDUCE_STEP)
				dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[fansNumber] -= REDUCE_STEP;
			else
				dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[fansNumber] = SMALL_WIN_MIN;
			break;
		case INCREASE:
			if (dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[fansNumber] < (SMALL_WIN_MAX-INCREASE_STEP))
				dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[fansNumber] += INCREASE_STEP;
			else
				dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[fansNumber] = SMALL_WIN_MAX;
			break;
		default:
			break;
	}
	//Write to EEPROM
}

uint8_t isSmallWinWorking(uint8_t whichOne)
{
	uint8_t result = false;
	switch (whichOne)
	{
		case LEFT:
			if ((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_CLOSE) || (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_OPEN))
			{
				result = true;
			}
			break;
		case RIGHT:
			if ((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_CLOSE) || (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_OPEN))
			{
				result = true;
			}
			break;
		default:
			break;
	}
	return result;
}

uint8_t calSetBits(uint32_t fansGroup)
{
	uint8_t i,bits = 0;
	uint32_t temp = 1;
	for (i = 0;i < 19;i++)
	{
		if (fansGroup & temp)
			++bits;
		temp <<= 1;
	}
	logPrintf(Debug,"D:sidewindowctrl_task.c::calSetBits()->Total number of funs is %d.\r\n",bits);
	return bits;
}

uint8_t isWinWorking(void)
{
	if (dataStore.realtimeData.isSideWindowMotorRunning & WIN_WORKING_MASK)
		return true;
	else
		return false;
}

void smallWinAngleTo(uint8_t whichOne,uint8_t angleTarget)
{
	uint8_t realTimeAngleTemp;
	switch (whichOne)
	{
		case LEFT:
			realTimeAngleTemp = (dataStore.realtimeData.smallSideWindowsAngle & 0x7F00) >> 8;
			if ( (realTimeAngleTemp >= (angleTarget + MOTOR_RUNNING_LIMIT)) &&
				((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_CLOSE) == 0x0000) )
			{
				windowCtrl(SmallLeftClose);
				logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Left small window current angle is :%d,closing...\r\n",realTimeAngleTemp);
			}
			else if ((realTimeAngleTemp  + MOTOR_RUNNING_LIMIT) <= angleTarget &&
					((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_OPEN) == 0x0000)	)
			{
				windowCtrl(SmallLeftOpen);
				logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Left small window current angle is :%d,opening...\r\n",realTimeAngleTemp);
			}
			else
			{
				logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->isSideWindowMotorRunning is %d\r\n",dataStore.realtimeData.isSideWindowMotorRunning);
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_CLOSE)
				{
					if ((realTimeAngleTemp < angleTarget) || 
						(abs(realTimeAngleTemp - angleTarget) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SmallLeftStop);
						logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Left small window stop closing!\r\n");
					}
				}
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_LEFT_OPEN)
				{
					if ((realTimeAngleTemp > angleTarget) || 
						(abs(realTimeAngleTemp - angleTarget) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SmallLeftStop);
						logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Left small window stop opening!\r\n");
					}
				}
			}
			break;
		case RIGHT:
			realTimeAngleTemp = dataStore.realtimeData.smallSideWindowsAngle & 0x7F;
			if ( (realTimeAngleTemp >= (angleTarget + MOTOR_RUNNING_LIMIT)) &&
				((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_CLOSE) == 0x0000) )
			{
				windowCtrl(SmallRightClose);
				logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Right small window current angle is :%d,closing...\r\n",realTimeAngleTemp);
			}
			else if( ((realTimeAngleTemp  + MOTOR_RUNNING_LIMIT) <= angleTarget) &&
					((dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_OPEN) == 0x0000))
			{
				windowCtrl(SmallRightOpen);
				logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Right small window current angle is :%d,opening...\r\n",realTimeAngleTemp);
			}
			else
			{
				logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->isSideWindowMotorRunning is %d\r\n",dataStore.realtimeData.isSideWindowMotorRunning);
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_CLOSE)
				{
					if ((realTimeAngleTemp < angleTarget) || 
						(abs(realTimeAngleTemp - angleTarget) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SmallRightStop);
						logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Right small window stop closing!\r\n");
					}
				}
				if (dataStore.realtimeData.isSideWindowMotorRunning & SMALL_RIGHT_OPEN)
				{
					if ((realTimeAngleTemp > angleTarget) || 
						(abs(realTimeAngleTemp - angleTarget) < MOTOR_STOPPED_LIMIT))
					{
						windowCtrl(SmallRightStop);
						logPrintf(Debug,"D:sidewindowctrl_task.c::smallWinAngleTo()->Right small window stop opening!\r\n");
					}
				}
			}
			break;
		default:
			break;
	}
}

void windowCtrl(WindowAction action)
{
	switch (action)
	{
		case SmallLeftOpen:
			littleAprilHCWCtrl(Small_Left_CloseP,Off);
			littleAprilHCWCtrl(Small_Left_OpenP,On);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_CLOSE;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_LEFT_OPEN;
			break;
		case SmallLeftClose:
			littleAprilHCWCtrl(Small_Left_CloseP,On);
			littleAprilHCWCtrl(Small_Left_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_LEFT_CLOSE;
			break;
		case SmallLeftStop:
			littleAprilHCWCtrl(Small_Left_CloseP,Off);
			littleAprilHCWCtrl(Small_Left_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_LEFT_CLOSE;
			break;
		case SmallRightOpen:
			littleAprilHCWCtrl(Small_Right_CloseP,Off);
			littleAprilHCWCtrl(Small_Right_OpenP,On);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_CLOSE;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_RIGHT_OPEN;
			break;
		case SmallRightClose:
			littleAprilHCWCtrl(Small_Right_CloseP,On);
			littleAprilHCWCtrl(Small_Right_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning |= SMALL_RIGHT_CLOSE;
			break;
		case SmallRightStop:
			littleAprilHCWCtrl(Small_Right_CloseP,Off);
			littleAprilHCWCtrl(Small_Right_OpenP,Off);
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_OPEN;
			dataStore.realtimeData.isSideWindowMotorRunning &= ~SMALL_RIGHT_CLOSE;
			break;
		case BigLeftOpen:
			break;
		case BigLeftClose:
			break;
		case BigLeftStop:
			break;
		case BigRightOpen:
			break;
		case BigRightClose:
			break;
		case BigRightStop:
			break;
		default:
			break;
	}
}

void sidewindowctrl_task(void *p_arg)
{
	OS_ERR err;
	CPU_INT32U     cpu_clk_freq;
	uint8_t lastWorkingFanNumbers = 0,isPressureOk = false;
	uint8_t angleLR[2],pressureLR[2],totalWorkingFans,adjustWaitTime = 0;
	p_arg = p_arg;
	CPU_SR_ALLOC();
	cpu_clk_freq = BSP_CPU_ClkFreq();
	windowCtrl(SmallLeftStop);
	windowCtrl(SmallRightStop);
	OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_DLY,&err);
	enableWatchDog(SIDE_WINDOW_TASK_WD);
	while (1)
	{
		feedWatchDog(SIDE_WINDOW_TASK_WD);
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_DLY,&err);
		
		if ((dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle & CTRLMODELEFTMASK) ||
			(dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle & CTRLMODERIGHTMASK))
		{
			//Manual control
			angleLR[RIGHT] = dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F;
			logPrintf(Debug,"D:sidewindowctrl_task.c::sidewindowctrl_task()->Right small window arget angle is :%d\r\n",angleLR[RIGHT]);
			smallWinAngleTo(RIGHT,angleLR[RIGHT]);
			//Manual control
			angleLR[LEFT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle&0x7F00) >> 8;
			logPrintf(Debug,"D:sidewindowctrl_task.c::sidewindowctrl_task()->Left small window arget angle is :%d\r\n",angleLR[LEFT]);
			smallWinAngleTo(LEFT,angleLR[LEFT]);
		}
		else
		{
			//Auto control
			totalWorkingFans = calSetBits(dataStore.realtimeData.workingVentilators);
			if ((totalWorkingFans != 0) && (lastWorkingFanNumbers != totalWorkingFans))
			{
				lastWorkingFanNumbers = totalWorkingFans;
				isPressureOk = false;
				smallWinAngleTo(LEFT,dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[lastWorkingFanNumbers]);
				smallWinAngleTo(RIGHT,dataStore.ctrlParameter.negativePressureCtrlAngle.fansSmallWinOpenAngle[lastWorkingFanNumbers]);
			}
			++adjustWaitTime;
			if ((totalWorkingFans > 0) && (adjustWaitTime >= 80))
			{
				adjustWaitTime = 80;
				if ((isSmallWinWorking(LEFT) == false) && (isSmallWinWorking(RIGHT) == false) && (isPressureOk == false))
				{
					pressureLR[LEFT] = (dataStore.ctrlParameter.systemOptions.sideWindowDefaultAngle & 0x7F00)>>8;
					if (dataStore.realtimeData.pressureInside > (pressureLR[LEFT] + 3))
					{
						logPrintf(Debug,"D:sidewindowctrl_task.c::sidewindowctrl_task()->Right small window increase :%d\r\n",INCREASE);
						adjustSmallWinAngleBase(totalWorkingFans,INCREASE);
					}
					else if ((dataStore.realtimeData.pressureInside + 3)< pressureLR[LEFT])
					{
						logPrintf(Debug,"D:sidewindowctrl_task.c::sidewindowctrl_task()->Right small window reduce :%d\r\n",REDUCE);
						adjustSmallWinAngleBase(totalWorkingFans,REDUCE);
					}
					else
					{
						isPressureOk = true;
					}
				}
			}
			else if (totalWorkingFans == 0)
			{
				adjustWaitTime = 0x00;
			}
		}
	}
}
