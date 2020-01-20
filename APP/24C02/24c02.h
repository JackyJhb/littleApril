#ifndef __24C02_H
#define __24C02_H

#include "stm32f4xx.h"  
#include "system.h"


extern void AT24C02_Write(u8 WriteAddr,u8 *pBuffer,u8 WriteNum);	//从指定地址开始写入指定长度的数据
extern void AT24C02_Read(u8 ReadAddr,u8 *pBuffer,u8 ReadNum);   	//从指定地址开始读出指定长度的数据

extern u8 AT24C02_Test(void);  //检查器件
extern void AT24C02_Init(void); //初始化IIC

#endif
















