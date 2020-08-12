#include "exti.h"
#include "includes.h"
#include "sccf.h"
#include "circleBuffer.h"
extern OS_SEM zeroSem;
extern uint16_t lightLevel;
extern uint8_t isLighting;

void ExtiInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF,EXTI_PinSource3);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI3_IRQHandler(void)
{
	OS_ERR err;
	uint16_t level = 0;
	if(EXTI_GetITStatus(EXTI_Line3)==1)
	{
		if (isLighting == true)
		{
			level = dataStore.ctrlParameter.illuminationStrength[dataStore.realtimeData.dayCycle-1].pluseWidth*10;
			if (level > 740)
			{
				GPIO_SetBits(GPIOE,GPIO_Pin_6);
			}
			else if (level < 1)
			{
				GPIO_ResetBits(GPIOE,GPIO_Pin_6);
			}
			else
			{
				TIM_SetCounter(TIM9,level);  //
				TIM_Cmd(TIM9,ENABLE);
			}
		}
		else
		{
			GPIO_ResetBits(GPIOE,GPIO_Pin_6);
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}
