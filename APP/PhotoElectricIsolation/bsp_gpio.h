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
	Fan17_Group3      	 = 0x01,
	Fan18_Group3 		 = 0x02,
	Fan19_Group3	     = 0x04,
	CirculatePump_Group3 = 0x08,
	FanShaft_Group3    	 = 0x10,
	Warning_Group3	 	 = 0x20,
	Big_Left_OpenP	 = 0x40,
	Big_Left_CloseP	 = 0x80
}Group3Define;

typedef enum{
	Big_Right_OpenP = 0x01,
	Big_Right_CloseP = 0x02,
	ReservedP0 = 0x04,
	ReservedP1 = 0x08,
	ReservedP2 = 0x10,
	ReservedP3 = 0x20,
	ReservedP4 = 0x40,
	ReservedP5 = 0x80
}Group4Define;

typedef enum{
	Heating_NO1        = 0x01,
	Heating_NO2        = 0x02,
	Heating_NO3 	   = 0x04,
	Small_Left_OpenP   = 0x08,
	Small_Left_CloseP  = 0x10,
	Small_Right_OpenP  = 0x20,
	Small_Right_CloseP = 0x40,
	Colding            = 0x80 
}WhichRelay;

extern void littleAprilIOInit(void);
extern void littleAprilGroupOutput(WhichGroup whichGroup,uint8_t outputData);
extern void littleAprilHCWCtrl(WhichRelay whichOne,OnOrOff onOrOff);
extern void littleApril19FansCtrl(uint32_t relayCtrlGroup,uint8_t whichTask);
extern void littleAprilGroup3Ctrl(Group3Define whichOne,OnOrOff onOrOff);
extern void littleAprilGroup4Ctrl(Group4Define whichOne,OnOrOff onOrOff);

#endif
