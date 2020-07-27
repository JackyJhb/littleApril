#include "includes.h"
#include "pwm.h"
#include "circleBuffer.h"
extern OS_SEM zeroSem;
static CPU_TS ts_start = 0,ts_end = 0;
void SCR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE ,ENABLE); //使能端口F时钟

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT; //输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;//管脚设置F9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//速度为100M
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化结构体
	GPIO_ResetBits(GPIOE,GPIO_Pin_5|GPIO_Pin_6);
}

void Timer9Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef tim9TimeBaseInitTypeDef;
    NVIC_InitTypeDef nvicInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
    tim9TimeBaseInitTypeDef.TIM_ClockDivision = TIM_CKD_DIV1;
    tim9TimeBaseInitTypeDef.TIM_Period = arr-1;
    tim9TimeBaseInitTypeDef.TIM_Prescaler = psc - 1;
    tim9TimeBaseInitTypeDef.TIM_CounterMode = TIM_CounterMode_Up;
		tim9TimeBaseInitTypeDef.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM9,&tim9TimeBaseInitTypeDef);
    TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);

    nvicInitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    nvicInitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    nvicInitStructure.NVIC_IRQChannelSubPriority = 0x03;
    nvicInitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInitStructure);
    //TIM_Cmd(TIM9,ENABLE);
}

void Timer9PWMInit(uint16_t arr,uint32_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	 //TIM9时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //使能PORTE时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOE6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //上拉
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //初始化PE6

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9); //GPIOE6复用为定时器9

	TIM_TimeBaseStructure.TIM_Prescaler=psc-1;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr-1;      //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);  //初始化定时器5

	//初始化TIM9 Channel1 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //输出极性:TIM输出比较极性低
	//TIM_OCInitStructure.TIM_Pulse = 3;
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);   //根据T指定的参数初始化外设TIM9 4OC1

	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM9在CCR1上的预装载寄存器
  TIM_ARRPreloadConfig(TIM9,ENABLE);
	TIM_Cmd(TIM9, ENABLE);  //使能TIM9
	TIM_CtrlPWMOutputs(TIM9,ENABLE);
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
	OS_ERR err;
	/*ts_end = OS_TS_GET();
	logPrintf(Verbose,"V:TIM1_BRK_TIM9_IRQHandler->%dns.\r\n",
				(ts_end-ts_start));
	ts_start = ts_end;*/
	OSIntEnter();
	if (TIM_GetITStatus(TIM9,TIM_IT_Update) == SET)
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_6);
		OSSemPost(&zeroSem,OS_OPT_POST_1,&err);
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
	TIM_Cmd(TIM9,DISABLE);
	//logPrintf(Error,"E:|\r\n");
	OSIntExit();
}
