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
	Group0,
	Group1,
	Group2,
	Group3,
	Group4
}WhichGroup;

typedef enum{
	Heating_NO1        = 0x00,// GPIO_Pin_6       //C6
	Heating_NO2        = 0x01,//GPIO_Pin_0       //C0
	Heating_NO3 	   = 0x02,//GPIO_Pin_2       //C2
	Window_LEFT_OPEN   = 0x03,//GPIO_Pin_12      //B12
	Window_LEFT_CLOSE  = 0x04,//GPIO_Pin_13      //C13
	Window_RIGHT_OPEN  = 0x05,//GPIO_Pin_13      //B13
	Window_RIGHT_CLOSE = 0x06,//GPIO_Pin_3       //C3
	Colding            = 0x07 //GPIO_Pin_5       //B5
	
}WhichRelay;

extern void littleAprilIOInit(void);
extern void clearOutput(void);
extern void littleAprilGroupOutput(WhichGroup whichGroup,uint8_t outputData);
extern void littleAprilIOCtrl(WhichRelay whichOne,OnOrOff onOrOff);
extern void littleAprilFanCtrl(uint32_t relayCtrlGroup);

/*extern void bsp_InitGPIO(void);
extern void Updata_Input_GPIO(void);
extern unsigned short Get_Input_GPIOX_Group(unsigned char Group);
extern unsigned char Get_Input_GPIOX_Bit(unsigned char Group,unsigned char Bit);
extern void Update_Output_GPIO(void);
extern void Set_GPIOY_Group(unsigned char Group,unsigned short Data);
extern void Set_Output_GPIOY_Bit(unsigned char Group,unsigned char Bit);
extern void Reset_Output_GPIOY_Bit(unsigned char Group,unsigned char Bit);
extern unsigned char Get_Output_GPIOY_Bit(unsigned char Group,unsigned char Bit);

extern void LED_RUN_Init();
extern void LED_RUN_ON(void);
extern void LED_RUN_OFF(void);
extern void LED_CTR_ON(void);

extern void SPI_GPIO_Init(void);
extern void Init_Max7219(void);
extern void Write_Max7219(unsigned char address,unsigned char dat);
extern unsigned short Read_MCP3204(unsigned char channel);
extern void SetRS485ReadCOM1(void);
extern void SetRS485WriteCOM1(void);
extern void SetRS485ReadCOM2(void);
extern void SetRS485WriteCOM2(void);
extern void SetRS485ReadCOM3(void);
extern void SetRS485WriteCOM3(void);
extern void Init_AD5592(void);
extern void Write_AD5592(unsigned char who,unsigned short data);
extern unsigned short Read_AD5592(unsigned char who);
extern void SetDAC_Voltage(unsigned char DAC_ID,float voltage);
extern void Init_DAC(void);*/
#endif


