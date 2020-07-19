#include "stm32f4xx.h"
#include <stdio.h>
#include "bsp_gpio.h"
#include "math.h"
#include "../Keyboard/bsp_button.h"
#include "debug_config.h"
#include "circleBuffer.h"
#include "task_monitor.h"

uint8_t hcwOutputCtrl,group3OutputCtrl;
uint16_t fansOutputCtrl;

typedef struct
{
	GPIO_TypeDef*		GPIOx;
	unsigned short		GPIO_Pin;
}GPIO_Index_Struct;

unsigned short InputGPIO[8]		= {0};							//72��IO������,��7��(0-6),0,1��ÿ��16����2-6��ÿ��8��

GPIO_Index_Struct littleAprilOutputGPIO[8] = {
	{GPIOG,GPIO_Pin_3},{GPIOG,GPIO_Pin_4},{GPIOB,GPIO_Pin_12},{GPIOB,GPIO_Pin_13},
	{GPIOC,GPIO_Pin_8},{GPIOC,GPIO_Pin_9},{GPIOC,GPIO_Pin_11},{GPIOC,GPIO_Pin_13},
};

GPIO_Index_Struct littleAprilFanOutputGPIOCS[5] = {
	{GPIOG,GPIO_Pin_5},{GPIOF,GPIO_Pin_6},{GPIOF,GPIO_Pin_7},{GPIOC,GPIO_Pin_0},{GPIOC,GPIO_Pin_2}
};

void littleAprilIOInit(void);
void clearOutput(void);
void littleAprilGroupOutput(WhichGroup whichGroup,uint8_t outputData);
void littleAprilGroup3Ctrl(Group3Define whichOne,OnOrOff onOrOff);

void littleAprilIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12 | GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOF,GPIO_Pin_6 | GPIO_Pin_7);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOG,GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	clearOutput();
}
void clearOutput(void)
{
	uint8_t i,j;
	hcwOutputCtrl = 0x00;
	fansOutputCtrl = 0x0000;
	group3OutputCtrl = 0x00;
	for (i = 0;i < 5;i++)
	{
		GPIO_SetBits(littleAprilFanOutputGPIOCS[i].GPIOx,
								 littleAprilFanOutputGPIOCS[i].GPIO_Pin);
		for (j = 0;j < 8;j++)
		{
			GPIO_ResetBits(littleAprilOutputGPIO[j].GPIOx,littleAprilOutputGPIO[j].GPIO_Pin);
		}
		GPIO_ResetBits(littleAprilFanOutputGPIOCS[i].GPIOx,
								 littleAprilFanOutputGPIOCS[i].GPIO_Pin);
	}
}
void littleAprilGroupOutput(WhichGroup whichGroup,uint8_t outputData)
{
	uint8_t i,temp;
	logPrintf(Verbose,"V:bsp_gpio.c::littleAprilGroupOutput()->%d is %d\r\n",whichGroup,outputData);
	GPIO_SetBits(littleAprilFanOutputGPIOCS[whichGroup].GPIOx,
								 littleAprilFanOutputGPIOCS[whichGroup].GPIO_Pin);
	for (i = 0;i < 8;i++)
	{
		temp = (1<<i);
		if (outputData & temp)
		{
			GPIO_SetBits(littleAprilOutputGPIO[i].GPIOx,littleAprilOutputGPIO[i].GPIO_Pin);
		}
		else
		{
			GPIO_ResetBits(littleAprilOutputGPIO[i].GPIOx,littleAprilOutputGPIO[i].GPIO_Pin);
		}
	}
	GPIO_ResetBits(littleAprilFanOutputGPIOCS[whichGroup].GPIOx,
								 littleAprilFanOutputGPIOCS[whichGroup].GPIO_Pin);
}

void littleAprilHCWCtrl(WhichRelay whichOne,OnOrOff onOrOff)
{
	if (onOrOff == On)
	{
		hcwOutputCtrl |= whichOne;
	}
	else
	{
		/* code */
		hcwOutputCtrl &= ~whichOne;
	}
	littleAprilGroupOutput(HCWGroup0,hcwOutputCtrl);
}

void littleApril16FansCtrl(uint32_t relayCtrlGroup,uint8_t whichTask)
{
	OS_ERR err;
	uint16_t currentFansOutput,i,j;
	currentFansOutput = fansOutputCtrl;
	fansOutputCtrl = relayCtrlGroup & 0xFFFF;
	if (currentFansOutput == fansOutputCtrl)
	{
		logPrintf(Verbose,"bsp_gpio.c::littleApril16FansCtrl()->State of output do not changed ,return!\r\n");
		return;
	}
	currentFansOutput &= fansOutputCtrl;
	for (i = 0;i < 16;i++)
	{
		j = (1 << i);
		if ((currentFansOutput & j) == 0)
		{
			if (fansOutputCtrl & j)
			{
				currentFansOutput |= j;
				OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_DLY,&err);
				feedWatchDog(whichTask);
			}
		}
		littleAprilGroupOutput(FansGroup1,currentFansOutput&0xFF);
		littleAprilGroupOutput(FansGroup2,(currentFansOutput&0xFF00)>>8);
	}
	logPrintf(Verbose,"currentFansOutput = %d,fansOutputCtrl = %d\r\n",currentFansOutput,fansOutputCtrl);
}

void littleAprilGroup3Ctrl(Group3Define whichOne,OnOrOff onOrOff)
{
	if (onOrOff == On)
	{
		group3OutputCtrl |= whichOne;
	}
	else
	{
		group3OutputCtrl &= ~whichOne;
	}
	littleAprilGroupOutput(Group3,group3OutputCtrl);
}