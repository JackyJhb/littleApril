#include "adc.h"
#include "includes.h"
#include "sccf.h"

__IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL]={0};
void ADCx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);
//----------------------------------------------------------------------------------------
	
	DMA_DeInit(RHEOSTAT_ADC_DMA_STREAM);
	DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL;
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = RHEOSTAT_NOFCHANEL;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM,&DMA_InitStructure);
	DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM,ENABLE);
	
//----------------------------------------------------------------------------------------
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = RHEOSTAT_NOFCHANEL;
	ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_4,1,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_5,2,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_6,3,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_7,4,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_8,5,ADC_SampleTime_15Cycles);
	ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC,ENABLE);
	ADC_DMACmd(RHEOSTAT_ADC,ENABLE);
	ADC_Cmd(RHEOSTAT_ADC, ENABLE);
	ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

void Get_ADC_Value(void)
{
	uint16_t adc_value[2] = {0},i;
	//1.036V opened 3.232V closed
	float temp;
	//TODO:DMA get 5 times ADC values
	for (i = 0; i<5 ;i++)
	{
		while(DMA_GetFlagStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0) == RESET)
		{
			
		}
		adc_value[0] += ADC_ConvertedValue[0];
		adc_value[1] += ADC_ConvertedValue[1];
		DMA_ClearFlag(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0);
	}
	adc_value[0] /= 5;
	adc_value[1] /= 5;
	//temp = (ADC_ConvertedValue[0]*ADC_VDD_33V)/4096;
	temp = (adc_value[0]*ADC_VDD_33V)/4096;
	if (temp < SIDE_WIN_OPEN) 
	{
		dataStore.realtimeData.realSideWindowsAngle[0] = OPEN_ABSOLUTE_ANGLE;
	}
	else if (temp > SIDE_WIN_CLOSE)
	{
		dataStore.realtimeData.realSideWindowsAngle[0] = CLOSE_ABSOLUTE_ANGLE;
	}
	else
	{
		dataStore.realtimeData.realSideWindowsAngle[0] = ((ADC_VDD_33V - temp) * OPEN_ABSOLUTE_ANGLE)/(SIDE_WIN_CLOSE-SIDE_WIN_OPEN);
	}
	
	temp = (adc_value[1]*ADC_VDD_33V)/4096;
    if (temp < SIDE_WIN_OPEN)
	{
		dataStore.realtimeData.realSideWindowsAngle[1] = OPEN_ABSOLUTE_ANGLE;
	}
	else if (temp > SIDE_WIN_CLOSE)
	{
		dataStore.realtimeData.realSideWindowsAngle[1] = CLOSE_ABSOLUTE_ANGLE;
	}
	else
	{
		dataStore.realtimeData.realSideWindowsAngle[1] = ((ADC_VDD_33V - temp) * OPEN_ABSOLUTE_ANGLE)/(SIDE_WIN_CLOSE-SIDE_WIN_OPEN);
	}
} 
