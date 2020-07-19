#include "includes.h"
#include "timer.h"

void Timer9Init(uint16_t psc,uint16_t arr)
{
    TIM_TimeBaseInitTypeDef tim9TimeBaseInitTypeDef;
    NVIC_InitTypeDef nvicInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
    tim9TimeBaseInitTypeDef.TIM_ClockDivision = TIM_CKD_DIV1;
    tim9TimeBaseInitTypeDef.TIM_Period = arr;
    tim9TimeBaseInitTypeDef.TIM_Prescaler = psc - 1;
    tim9TimeBaseInitTypeDef.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM9,&tim9TimeBaseInitTypeDef);
    TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);

    nvicInitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    nvicInitStructure.NVIC_IRQChannelSubPriority = 0x03;
    nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInitStructure);
    
    TIM_Cmd(TIM9,ENABLE);
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM9,TIM_IT_Update) == SET)
    {
        GPIO_SetBits(GPIOE,GPIO_Pin_6);
    }
    TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
}
