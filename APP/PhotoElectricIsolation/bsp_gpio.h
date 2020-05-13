/*
*********************************************************************************************************
*	                                  
*	ģ������ : GPIO��д����    
*	�ļ����� : bsp_gpio.h
*	��    �� : V2.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/

#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "stm32f4xx_gpio.h"

//#define ON  1
//#define OFF 0
typedef enum{
	On,
	Off
}OnOrOff;

typedef enum{
	HCWGroup0,
	FansGroup1,
	FansGroup2,
	Group3,
	Group4
}WhichGroup;

typedef enum{
	Heating_NO1        = 0x01,// GPIO_Pin_6       //C6
	Heating_NO2        = 0x02,//GPIO_Pin_0       //C0
	Heating_NO3 	   = 0x04,//GPIO_Pin_2       //C2
	Window_LEFT_OPEN   = 0x08,//GPIO_Pin_12      //B12
	Window_LEFT_CLOSE  = 0x10,//GPIO_Pin_13      //C13
	Window_RIGHT_OPEN  = 0x20,//GPIO_Pin_13      //B13
	Window_RIGHT_CLOSE = 0x40,//GPIO_Pin_3       //C3
	Colding            = 0x80 //GPIO_Pin_5       //B5
	
}WhichRelay;

extern void littleAprilIOInit(void);
extern void littleAprilHCWCtrl(WhichRelay whichOne,OnOrOff onOrOff);
extern void littleApril16FansCtrl(uint32_t relayCtrlGroup);

#endif


