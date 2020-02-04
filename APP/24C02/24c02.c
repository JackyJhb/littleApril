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
void IIC_Init(void);          //初始化IIC的IO口				 
void IIC_Start(void);				  //发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
uint8_t MCU_Wait_Ack(void); 				//IIC等待ACK信号
void MCU_Send_Ack(void);					  //IIC发送ACK信号
void MCU_NOAck(void);				  //IIC不发送ACK信号
void IIC_write_OneByte(uint8_t Senddata);
uint8_t IIC_Read_OneByte(uint8_t ack);

uint8_t AT24C02_ReadByte(uint16_t ReadAddr);							     //指定地址读取一个字节

void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //使能GPIOB时钟
  //GPIOB8,B9初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;     //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);                 //初始化IO
	IIC_SCL=1;
	IIC_SDAOUT=1;
}

void IIC_Start(void)
{
	SDA_OUT();     //配置SDA线为输出
	IIC_SDAOUT=1;	  	  
	IIC_SCL=1;
	delays_us(4);
 	IIC_SDAOUT=0;
	delays_us(4);
	IIC_SCL=0;     //准备发送或接收数据 
}	  

//IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();    //配置SDA线为输出
	IIC_SCL=0;
	IIC_SDAOUT=0; 
 	delays_us(4);
	IIC_SCL=1;
  delays_us(4);				
	IIC_SDAOUT=1; //发送I2C总线结束信号				   	
}

uint8_t MCU_Wait_Ack(void)
{
	uint8_t WaitTime=0;
	SDA_IN();      //配置SDA线为输入  
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
	IIC_SCL=0;    //拉低时钟开始数据传输
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
	SDA_IN();       //配置SDA线为输入  
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
        MCU_NOAck();//发送nACK
    else
        MCU_Send_Ack(); //发送ACK   
    return receivedata;
}

//初始化24c02的IIC接口
void AT24C02_Init(void)
{
	IIC_Init();  //IIC初始化
}

uint8_t AT24C02_ReadByte(uint16_t ReadAddr)
{				  
	uint8_t receivedata=0;		  	    																 
  
	IIC_Start();  
	IIC_write_OneByte(0XA0);           //发送器件地址0XA0
	MCU_Wait_Ack();
	#ifdef AT24C128
	IIC_write_OneByte(ReadAddr/0x100);       //发送高地址
	MCU_Wait_Ack();
	#endif
	IIC_write_OneByte(ReadAddr%0x100); 
	MCU_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_write_OneByte(0XA1);           //进入接收模式			   
	MCU_Wait_Ack();	 
  receivedata=IIC_Read_OneByte(0);		   
  IIC_Stop();                    //产生一个停止条件	    
	
	return receivedata;
}

/****************************************************************************
* 名    称: void AT24C02_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint8_t ReadNum)
* 功    能：从AT24C02里面的指定地址开始读出指定个数的数据
* 入口参数：ReadAddr :开始读出的地址  0~255
            pBuffer  :数据数组首地址
            ReadNum:要读出数据的个数
* 返回参数：
* 说    明：  
****************************************************************************/
void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t ReadNum)
{
	while(ReadNum--)
	{
		*pBuffer++=AT24C02_ReadByte(ReadAddr++);	
	}
} 

/****************************************************************************
* 名    称: void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,
*                              uint16_t WriteNum)
* 功    能：从AT24C02里面的指定地址开始写入指定个数的数据
* 入口参数：WriteAddr :开始写入的地址  0~255
            pBuffer  :数据数组首地址
            WriteNum:要写入数据的个数
* 返回参数：
* 说    明：  
****************************************************************************/
void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t WriteNum)
{
	uint16_t i,len,addr=WriteAddr;
	/*while(WriteNum--)
	{
		AT24C02_WriteByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}*/
	IIC_Start();  
	IIC_write_OneByte(0XA0);       //发送0XA0,写数据 	 
	MCU_Wait_Ack();	   
	#ifdef AT24C128
	IIC_write_OneByte(addr/0x100);       //发送高地址
	MCU_Wait_Ack();
	#endif
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
			#ifdef AT24C128
			IIC_write_OneByte(addr/0x100);       //发送高地址
			MCU_Wait_Ack();
			#endif
			IIC_write_OneByte(addr%0x100); 
			MCU_Wait_Ack();
		}
	}
  IIC_Stop();                    //产生一个停止条件
	for (i = 0;i < 10;i++)
	{
		delays_us(1000);
	}
}
