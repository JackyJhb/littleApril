#ifndef _adc_H
#define _adc_H

#include "system.h"

#define SMALL_LEFT_WIN_CH            0
#define SMALL_RIGHT_WIN_CH          1
#define PRESSURE_CH                       3
#define BIG_LEFT_WIN_CH                 4
#define BIG_RIGHT_WIN_CH               5
#define RESERVED                              2

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
#define RHEOSTAT_NOFCHANEL      	6
#define RHEOSTAT_ADC_DMA_CLK    	RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL	DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM		DMA2_Stream0

#define ADC_CONV_TIMES 5

typedef struct {
	float x0;
	float y0;
	float x1;
	float y1;
	float k;
}KStruct;

typedef struct {
	KStruct smallWin;
	KStruct bigWin;
	KStruct pressureSensor;
}KStore;

void ADCx_Init(void);
extern void Get_ADC_Value(KStore *kStorePtr);


#endif
