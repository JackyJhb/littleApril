#include "adc.h"
#include "includes.h"
#include "sccf.h"
#include "circleBuffer.h"

__IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL*ADC_CONV_TIMES]={0};
void Get_ADC_Value(KStore *kStorePtr);

static float resultBuf[RHEOSTAT_NOFCHANEL][ADC_CONV_TIMES];
static uint8_t convTimes=0;

float getAveValue(float *buf,char len)
{
	float max=-100.00,min=100.00,total = 0.00;
	uint8_t i;
	for (i = 0;i < len;i++)
	{
		if (*(buf+i) > max)
			max = *(buf+i);
		if (*(buf+i) < min)
			min = *(buf +i);
		total += *(buf+i);
	}
	
	total -= (max + min);
	total /= (len - 2);
	return total;
}

void ADCx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;// |GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
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
	DMA_InitStructure.DMA_BufferSize = RHEOSTAT_NOFCHANEL*ADC_CONV_TIMES;
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
	
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_3,1,ADC_SampleTime_15Cycles); //New
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_4,2,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_5,3,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_6,4,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_8,5,ADC_SampleTime_15Cycles);
	ADC_RegularChannelConfig(RHEOSTAT_ADC,ADC_Channel_9,6,ADC_SampleTime_15Cycles);
	
	ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC,ENABLE);
	ADC_DMACmd(RHEOSTAT_ADC,ENABLE);
	ADC_Cmd(RHEOSTAT_ADC, ENABLE);
	ADC_SoftwareStartConv(RHEOSTAT_ADC);
}

void Get_ADC_Value(KStore *kStorePtr)
{
	uint16_t adc_value[RHEOSTAT_NOFCHANEL] = {0};
	uint8_t i,j;
	float temp;

	while(DMA_GetFlagStatus(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0) == RESET);
	DMA_ClearFlag(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TCIF0);
	for (i = 0;i < ADC_CONV_TIMES;i++)
	{
		for (j = 0;j < RHEOSTAT_NOFCHANEL;j++)
		{
			adc_value[j] += ADC_ConvertedValue[i*RHEOSTAT_NOFCHANEL + j];
		}
	}
	for (j = 0;j < RHEOSTAT_NOFCHANEL;j++)
	{
		adc_value[j] /= ADC_CONV_TIMES;
	}
	
	//1
	//Because pressure sensor output 0~4.5V voltage and ADC Vref is 3.3V,so need to ...
	temp = (float)(adc_value[PRESSURE_CH] * ADC_VDD_33V)/4095-kStorePtr->pressureSensor.y0;
	temp /= kStorePtr->pressureSensor.k;
	temp += kStorePtr->pressureSensor.x0;
	resultBuf[PRESSURE_CH][convTimes] = temp;
	if (convTimes == 4)
	{
		temp = getAveValue(&resultBuf[PRESSURE_CH][0],sizeof(resultBuf[PRESSURE_CH])/sizeof(float));
		if ((uint8_t)(temp + 0.5) > (uint8_t)temp)
			temp += 1;
		i = (uint8_t)temp;
		dataStore.realtimeData.pressureInside = i;
	}
	
	//2
	temp =(float)(adc_value[SMALL_LEFT_WIN_CH] * ADC_VDD_33V)/4095-kStorePtr->smallWin.y0;
	temp /= kStorePtr->smallWin.k;
	temp += kStorePtr->smallWin.x0;
	resultBuf[SMALL_LEFT_WIN_CH][convTimes] = temp;
	if (convTimes == 4)
	{
		temp = getAveValue(&resultBuf[SMALL_LEFT_WIN_CH][0] , sizeof(resultBuf[SMALL_LEFT_WIN_CH])/sizeof(float));
		if ((uint8_t)(temp + 0.5) > (uint8_t)temp)
			temp += 1;
		i = (uint8_t)temp;
		dataStore.realtimeData.smallSideWindowsAngle &= 0x00FF;
		dataStore.realtimeData.smallSideWindowsAngle |= (i<<8);
	}

	//3
	temp =(float)(adc_value[SMALL_RIGHT_WIN_CH] * ADC_VDD_33V)/4095-kStorePtr->smallWin.y0;
	temp /= kStorePtr->smallWin.k;
	temp += kStorePtr->smallWin.x0;
	resultBuf[SMALL_RIGHT_WIN_CH][convTimes] = temp;
	if (convTimes == 4)
	{
		temp = getAveValue(&resultBuf[SMALL_RIGHT_WIN_CH][0] , sizeof(resultBuf[SMALL_RIGHT_WIN_CH])/sizeof(float));
		if ((uint8_t)(temp + 0.5) > (uint8_t)temp)
			temp += 1;
		i = (uint8_t)temp;
		dataStore.realtimeData.smallSideWindowsAngle &= 0xFF00;
		dataStore.realtimeData.smallSideWindowsAngle |= i;
	}
	
	//4
	temp =(float)(adc_value[BIG_LEFT_WIN_CH] * ADC_VDD_33V)/4095-kStorePtr->bigWin.y0;
	temp /= kStorePtr->bigWin.k;
	temp += kStorePtr->bigWin.x0;
	resultBuf[BIG_LEFT_WIN_CH][convTimes] = temp;
	if (convTimes == 4)
	{
		temp = getAveValue(&resultBuf[BIG_LEFT_WIN_CH][0] , sizeof(resultBuf[BIG_LEFT_WIN_CH])/sizeof(float));
		if ((uint8_t)(temp + 0.5) > (uint8_t)temp)
			temp += 1;
		i = (uint8_t)temp;
		dataStore.realtimeData.bigSideWindowsAngle &= 0x00FF;
		dataStore.realtimeData.bigSideWindowsAngle |= (i<<8);
	}
	
	//5
	temp =(float)(adc_value[BIG_RIGHT_WIN_CH] * ADC_VDD_33V)/4095-kStorePtr->bigWin.y0;
	temp /= kStorePtr->bigWin.k;
	temp += kStorePtr->bigWin.x0;
	resultBuf[BIG_RIGHT_WIN_CH][convTimes] = temp;
	if (convTimes == 4)
	{
		temp = getAveValue(&resultBuf[BIG_RIGHT_WIN_CH][0] , sizeof(resultBuf[BIG_RIGHT_WIN_CH])/sizeof(float));
		if ((uint8_t)(temp + 0.5) > (uint8_t)temp)
			temp += 1;
		i = (uint8_t)temp;
		dataStore.realtimeData.bigSideWindowsAngle &= 0xFF00;
		dataStore.realtimeData.bigSideWindowsAngle |= i;
	}

	if (++convTimes == ADC_CONV_TIMES)
		convTimes = 0;
} 
