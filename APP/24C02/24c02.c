#include "24c02.h"
#include "ds18b20.h"
#include "includes.h"
#include "sccf.h"
#define SDA_IN()  {GPIOB->MODER&=~(3<<18);GPIOB->MODER|=0<<18;}
#define SDA_OUT() {GPIOB->MODER&=~(3<<18);GPIOB->MODER|=1<<18;}

#define IIC_SCL      PBout(8)
#define IIC_SDAOUT   PBout(9)
#define IIC_SDAIN    PBin(9)

//IIC相关函数
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t MCU_Wait_Ack(void);
void MCU_Send_Ack(void);
void MCU_NOAck(void);
void IIC_write_OneByte(uint8_t Senddata);
uint8_t IIC_Read_OneByte(uint8_t ack);
uint8_t AT24C02_ReadByte(uint16_t ReadAddr);

void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	IIC_SCL=1;
	IIC_SDAOUT=1;
}

void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDAOUT=1;
	IIC_SCL=1;
	delays_us(4);
 	IIC_SDAOUT=0;
	delays_us(4);
	IIC_SCL=0;
}	  

void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDAOUT=0;
 	delays_us(4);
	IIC_SCL=1;
  delays_us(4);
	IIC_SDAOUT=1;
}

uint8_t MCU_Wait_Ack(void)
{
	uint8_t WaitTime=0;
	SDA_IN();
	IIC_SDAOUT=1;
	delays_us(1);
	IIC_SCL=1;
	delays_us(1);
	while(IIC_SDAIN)
	{
		WaitTime++;
		if(WaitTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0; 
	return 0;
}

void MCU_Send_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDAOUT=0;
	delays_us(2);
	IIC_SCL=1;
	delays_us(2);
	IIC_SCL=0;
}

void MCU_NOAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDAOUT=1;
	delays_us(2);
	IIC_SCL=1;
	delays_us(2);
	IIC_SCL=0;
}

void IIC_write_OneByte(uint8_t Senddata)
{
	uint8_t t;
	SDA_OUT();
	IIC_SCL=0;
	for(t=0;t<8;t++)
	{
		IIC_SDAOUT=(Senddata&0x80)>>7;
		Senddata<<=1;
		delays_us(2);
		IIC_SCL=1;
		delays_us(2);
		IIC_SCL=0;
		delays_us(2);
	}
}

uint8_t IIC_Read_OneByte(uint8_t ack)
{
	uint8_t i,receivedata=0;
	SDA_IN();
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0;
		delays_us(2);
		IIC_SCL=1;
		receivedata<<=1;
		if(IIC_SDAIN)receivedata++;
			delays_us(1);
	}
	if (!ack)
		MCU_NOAck();
	else
		MCU_Send_Ack();
	return receivedata;
}

void AT24C02_Init(void)
{
	IIC_Init();
}

uint8_t AT24C02_ReadByte(uint16_t ReadAddr)
{
	uint8_t receivedata=0;													 

	IIC_Start();
	IIC_write_OneByte(0XA0);
	MCU_Wait_Ack();
	#ifdef AT24C128
	IIC_write_OneByte(ReadAddr/0x100);
	MCU_Wait_Ack();
	#endif
	IIC_write_OneByte(ReadAddr%0x100);
	MCU_Wait_Ack();
	IIC_Start();
	IIC_write_OneByte(0XA1);
	MCU_Wait_Ack();
  receivedata=IIC_Read_OneByte(0);
  IIC_Stop();
	return receivedata;
}

void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t ReadNum)
{
	while(ReadNum--)
	{
		*pBuffer++=AT24C02_ReadByte(ReadAddr++);	
	}
} 

void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t WriteNum)
{
	uint16_t i,len,addr=WriteAddr;
	#ifdef  AT24C128
	IIC_Start();  
	IIC_write_OneByte(0XA0);       //发送0XA0,写数据 	 
	MCU_Wait_Ack();	   
	IIC_write_OneByte(addr/0x100);       //发送高地址
	MCU_Wait_Ack();
	IIC_write_OneByte(addr%0x100); 
	MCU_Wait_Ack();
	for (len = 0;len < WriteNum;len++)
	{
		IIC_write_OneByte(*(pBuffer+len));  //发送字节					   
		MCU_Wait_Ack();
		++addr;
		if (addr%64 == 0)
		{
			IIC_Stop();                    //产生一个停止条件
			for (i = 0;i < 10;i++)
			{
				delays_us(1000);
			}
			IIC_Start();  
			IIC_write_OneByte(0XA0);       //发送0XA0,写数据 	 
			MCU_Wait_Ack();	   
			IIC_write_OneByte(addr/0x100);       //发送高地址
			MCU_Wait_Ack();
			IIC_write_OneByte(addr%0x100); 
			MCU_Wait_Ack();
		}
	}
  IIC_Stop();                    //产生一个停止条件
	for (i = 0;i < 10;i++)
	{
		delays_us(1000);
	}
	#else
	for (len = 0;len < WriteNum;len++)
	{
		IIC_Start();
		IIC_write_OneByte(0XA0);
		MCU_Wait_Ack();
		IIC_write_OneByte(addr%0x100);
		MCU_Wait_Ack();
		IIC_write_OneByte(*(pBuffer+len));
		MCU_Wait_Ack();
		++addr;
		IIC_Stop();
		for (i = 0;i < 10;i++)
		{
			delays_us(1000);
		}
	}
	#endif
}
