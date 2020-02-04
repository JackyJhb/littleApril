#include "24c02.h"
#include "ds18b20.h"
#include "includes.h"
#include "sccf.h"
#define SDA_IN()  {GPIOB->MODER&=~(3<<18);GPIOB->MODER|=0<<18;}
#define SDA_OUT() {GPIOB->MODER&=~(3<<18);GPIOB->MODER|=1<<18;}
 
#define IIC_SCL      PBout(8)
#define IIC_SDAOUT   PBout(9)
#define IIC_SDAIN    PBin(9)

//IIC��غ���
void IIC_Init(void);          //��ʼ��IIC��IO��				 
void IIC_Start(void);				  //����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
uint8_t MCU_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MCU_Send_Ack(void);					  //IIC����ACK�ź�
void MCU_NOAck(void);				  //IIC������ACK�ź�
void IIC_write_OneByte(uint8_t Senddata);
uint8_t IIC_Read_OneByte(uint8_t ack);

uint8_t AT24C02_ReadByte(uint16_t ReadAddr);							     //ָ����ַ��ȡһ���ֽ�

void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
  //GPIOB8,B9��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;          //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;     //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);                 //��ʼ��IO
	IIC_SCL=1;
	IIC_SDAOUT=1;
}

void IIC_Start(void)
{
	SDA_OUT();     //����SDA��Ϊ���
	IIC_SDAOUT=1;	  	  
	IIC_SCL=1;
	delays_us(4);
 	IIC_SDAOUT=0;
	delays_us(4);
	IIC_SCL=0;     //׼�����ͻ�������� 
}	  

//IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();    //����SDA��Ϊ���
	IIC_SCL=0;
	IIC_SDAOUT=0; 
 	delays_us(4);
	IIC_SCL=1;
  delays_us(4);				
	IIC_SDAOUT=1; //����I2C���߽����ź�				   	
}

uint8_t MCU_Wait_Ack(void)
{
	uint8_t WaitTime=0;
	SDA_IN();      //����SDA��Ϊ����  
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
	IIC_SCL=0;    //����ʱ�ӿ�ʼ���ݴ���
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
	SDA_IN();       //����SDA��Ϊ����  
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
        MCU_NOAck();//����nACK
    else
        MCU_Send_Ack(); //����ACK   
    return receivedata;
}

//��ʼ��24c02��IIC�ӿ�
void AT24C02_Init(void)
{
	IIC_Init();  //IIC��ʼ��
}

uint8_t AT24C02_ReadByte(uint16_t ReadAddr)
{				  
	uint8_t receivedata=0;		  	    																 
  
	IIC_Start();  
	IIC_write_OneByte(0XA0);           //����������ַ0XA0
	MCU_Wait_Ack();
	#ifdef AT24C128
	IIC_write_OneByte(ReadAddr/0x100);       //���͸ߵ�ַ
	MCU_Wait_Ack();
	#endif
	IIC_write_OneByte(ReadAddr%0x100); 
	MCU_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_write_OneByte(0XA1);           //�������ģʽ			   
	MCU_Wait_Ack();	 
  receivedata=IIC_Read_OneByte(0);		   
  IIC_Stop();                    //����һ��ֹͣ����	    
	
	return receivedata;
}

/****************************************************************************
* ��    ��: void AT24C02_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint8_t ReadNum)
* ��    �ܣ���AT24C02�����ָ����ַ��ʼ����ָ������������
* ��ڲ�����ReadAddr :��ʼ�����ĵ�ַ  0~255
            pBuffer  :���������׵�ַ
            ReadNum:Ҫ�������ݵĸ���
* ���ز�����
* ˵    ����  �B
****************************************************************************/
void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t ReadNum)
{
	while(ReadNum--)
	{
		*pBuffer++=AT24C02_ReadByte(ReadAddr++);	
	}
} 

/****************************************************************************
* ��    ��: void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,
*                              uint16_t WriteNum)
* ��    �ܣ���AT24C02�����ָ����ַ��ʼд��ָ������������
* ��ڲ�����WriteAddr :��ʼд��ĵ�ַ  0~255
            pBuffer  :���������׵�ַ
            WriteNum:Ҫд�����ݵĸ���
* ���ز�����
* ˵    ����  �B
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
	IIC_write_OneByte(0XA0);       //����0XA0,д���� 	 
	MCU_Wait_Ack();	   
	#ifdef AT24C128
	IIC_write_OneByte(addr/0x100);       //���͸ߵ�ַ
	MCU_Wait_Ack();
	#endif
	IIC_write_OneByte(addr%0x100); 
	MCU_Wait_Ack();
	for (len = 0;len < WriteNum;len++)
	{
		IIC_write_OneByte(*(pBuffer+len));  //�����ֽ�					   
		MCU_Wait_Ack();
		++addr;
		if (addr%64 == 0)
		{
			IIC_Stop();                    //����һ��ֹͣ����
			for (i = 0;i < 10;i++)
			{
				delays_us(1000);
			}
			IIC_Start();  
			IIC_write_OneByte(0XA0);       //����0XA0,д���� 	 
			MCU_Wait_Ack();	   
			#ifdef AT24C128
			IIC_write_OneByte(addr/0x100);       //���͸ߵ�ַ
			MCU_Wait_Ack();
			#endif
			IIC_write_OneByte(addr%0x100); 
			MCU_Wait_Ack();
		}
	}
  IIC_Stop();                    //����һ��ֹͣ����
	for (i = 0;i < 10;i++)
	{
		delays_us(1000);
	}
}
