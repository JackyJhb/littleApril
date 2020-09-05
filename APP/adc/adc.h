#ifndef _adc_H
#define _adc_H

#include "system.h"
#include "sccf.h"

#define SMALL_LEFT_WIN_CH            0
#define SMALL_RIGHT_WIN_CH          1
#define PRESSURE_CH                       3
#define BIG_LEFT_WIN_CH                 4
#define BIG_RIGHT_WIN_CH               5
#define RESERVED                              2

#define LEFT_WINDOW_ANGLE           1
#define RIGHT_WINDOW_ANGLE          2
#define ADC_VDD_33V                 3.3f

#define SMALL_WIN_MAX          60
#define SMALL_WIN_MIN            0

#define RHEOSTAT_ADC        		ADC1
#define RHEOSTAT_ADC_CLK    		RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC_DR_ADDR		((uint32_t)ADC1 + 0x4C)
#define RHEOSTAT_NOFCHANEL      	6
#define RHEOSTAT_ADC_DMA_CLK    	RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL	DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM		DMA2_Stream0

#define ADC_CONV_TIMES 5

void ADCx_Init(void);
extern void Get_ADC_Value(KStore *kStorePtr);

#endif
