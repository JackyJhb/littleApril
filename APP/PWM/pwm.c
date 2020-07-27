#include "includes.h"
#include "pwm.h"
#include "circleBuffer.h"
extern OS_SEM zeroSem;
static CPU_TS ts_start = 0,ts_end = 0;
void SCR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE ,ENABLE); //ʹ�ܶ˿�Fʱ��

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT; //���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;//�ܽ�����F9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//�ٶ�Ϊ100M
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE,&GPIO_InitStructure); //��ʼ���ṹ��
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	 //TIM9ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //ʹ��PORTEʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOE6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //����
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //��ʼ��PE6

	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9); //GPIOE6����Ϊ��ʱ��9

	TIM_TimeBaseStructure.TIM_Prescaler=psc-1;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr-1;      //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);  //��ʼ����ʱ��5

	//��ʼ��TIM9 Channel1 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //�������:TIM����Ƚϼ��Ե�
	//TIM_OCInitStructure.TIM_Pulse = 3;
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);   //����Tָ���Ĳ�����ʼ������TIM9 4OC1

	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��TIM9��CCR1�ϵ�Ԥװ�ؼĴ���
  TIM_ARRPreloadConfig(TIM9,ENABLE);
	TIM_Cmd(TIM9, ENABLE);  //ʹ��TIM9
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
