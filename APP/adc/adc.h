#ifndef _adc_H
#define _adc_H

#include "system.h"

#define LEFT_WINDOW_ANGLE           1
#define RIGHT_WINDOW_ANGLE          2
#define ADC_VDD_33V                 3.3
#define SIDE_WIN_OPEN               1.036
#define SIDE_WIN_CLOSE              3.236
#define OPEN_ABSOLUTE_ANGLE         60
#define CLOSE_ABSOLUTE_ANGLE        0

#define RHEOSTAT_ADC        		ADC1
#define RHEOSTAT_ADC_CLK    		RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_DR_ADDR		((uint32_t)ADC1 + 0x4C)
#define RHEOSTAT_NOFCHANEL      	4
#define RHEOSTAT_ADC_DMA_CLK    	RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL	DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM		DMA2_Stream0

void ADCx_Init(void);
void Get_ADC_Value(void);


#endif
