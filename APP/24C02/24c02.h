#ifndef __24C02_H
#define __24C02_H

#include "stm32f4xx.h"  
#include "system.h"


extern void AT24C02_Write(u8 WriteAddr,u8 *pBuffer,u8 WriteNum);	//��ָ����ַ��ʼд��ָ�����ȵ�����
extern void AT24C02_Read(u8 ReadAddr,u8 *pBuffer,u8 ReadNum);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

extern u8 AT24C02_Test(void);  //�������
extern void AT24C02_Init(void); //��ʼ��IIC

#endif
















