#include "24c02.h"
#include "ds18b20.h"
#include "SysTick.h"
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
	
//EEPROM24c02��غ���
uint8_t AT24C02_ReadByte(uint8_t ReadAddr);							     //ָ����ַ��ȡһ���ֽ�
void AT24C02_WriteByte(uint8_t WriteAddr,uint8_t DataToWrite);		//ָ����ַд��һ���ֽ�
u32 Buf_4Byte(uint8_t *pBuffer,u32 Date_4Byte,uint8_t Byte_num,uint8_t mode);

//IIC��ӦIO�ڵĳ�ʼ��
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
/*******************************************************************************
*************************����ΪIO��ģ��IICͨ��**********************************
*******************************************************************************/
//IIC��ʼ�ź�
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

/****************************************************************************
* ��    ��: uint8_t MCU_Wait_Ack(void)
* ��    �ܣ�MCU�ȴ����豸Ӧ���źŵ���
* ��ڲ�������
* ���ز�����1:����Ӧ��ʧ��  0:����Ӧ��ɹ�
* ˵    ����  �B
****************************************************************************/
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

/****************************************************************************
* ��    ��: uint8_t void MCU_Send_Ack(void)
* ��    �ܣ�MCU����ACKӦ��,��֪24cxx
* ��ڲ�������
* ���ز�����
* ˵    ����  �B
****************************************************************************/
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

/****************************************************************************
* ��    ��: uint8_t void MCU_Send_Ack(void)
* ��    �ܣ�MCU������ACKӦ��	
* ��ڲ�������
* ���ز�����
* ˵    ����  �B
****************************************************************************/  
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

/****************************************************************************
* ��    ��: void IIC_write_OneByte(uint8_t Senddata)
* ��    �ܣ�IICдһ���ֽڵ�EEPROM	
* ��ڲ�����Senddata:д���8λ����
* ���ز�����
* ˵    ����  �B
****************************************************************************/	  
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

/****************************************************************************
* ��    ��: void IIC_Read_OneByte(uint8_t Senddata)
* ��    �ܣ�IIC��ȡһ���ֽ�
* ��ڲ�����ack=1������ACK��ack=0������nACK 
* ���ز�����������8λ����
* ˵    ����  �B
****************************************************************************/	  
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
/*******************************IO��ģ��IIC*************************************
*******************************************************************************/


/*******************************************************************************
*************************����ΪEEPROM24C02��д����******************************
*******************************************************************************/
//��ʼ��24c02��IIC�ӿ�
void AT24C02_Init(void)
{
	IIC_Init();  //IIC��ʼ��
}

/****************************************************************************
* ��    ��: uint8_t AT24C02_ReadByte(uint8_t ReadAddr)
* ��    �ܣ���AT24C02ָ����ַ����һ������
* ��ڲ�����ReadAddr��Ҫ��ȡ�������ڵĵ�ַ
* ���ز�����������8λ����
* ˵    ����  �B
****************************************************************************/
uint8_t AT24C02_ReadByte(uint8_t ReadAddr)
{				  
	uint8_t receivedata=0;		  	    																 
  
	IIC_Start();  
	IIC_write_OneByte(0XA0);           //����������ַ0XA0
	MCU_Wait_Ack();
  IIC_write_OneByte(ReadAddr);       //���͵͵�ַ
	MCU_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_write_OneByte(0XA1);           //�������ģʽ			   
	MCU_Wait_Ack();	 
  receivedata=IIC_Read_OneByte(0);		   
  IIC_Stop();                    //����һ��ֹͣ����	    
	
	return receivedata;
}

/****************************************************************************
* ��    ��: uint8_t AT24C02_WriteByte(uint8_t WriteAddr,uint8_t WriteData)
* ��    �ܣ���AT24C02ָ����ַд��һ������
* ��ڲ�����WriteAddr��Ҫд���������ڵĵ�ַ
            WriteData: Ҫд�������
* ���ز����� 
* ˵    ����  �B
****************************************************************************/
void AT24C02_WriteByte(uint8_t WriteAddr,uint8_t WriteData)
{				
  IIC_Start();  
	IIC_write_OneByte(0XA0);       //����0XA0,д���� 	 
	MCU_Wait_Ack();	   
  IIC_write_OneByte(WriteAddr);  //���͵͵�ַ
	MCU_Wait_Ack(); 	 										  		   
	IIC_write_OneByte(WriteData);  //�����ֽ�							   
	MCU_Wait_Ack();  		    	   
  IIC_Stop();                    //����һ��ֹͣ���� 
	delay_ms(10);	 
}

/****************************************************************************
* ��    ��: uint8_t AT24C02_Test(void)
* ��    �ܣ�����AT24C02�Ƿ�����
* ��ڲ�������
* ���ز���������1:���ʧ��
            ����0:���ɹ� 
* ˵    ����  �B
****************************************************************************/
uint8_t AT24C02_Test(void)
{
	uint8_t Testdata;
	Testdata=AT24C02_ReadByte(255); //����������ԣ�����ֵ�����ٴ�д��	   
	if(Testdata==0XAB)return 0;		   
	else                             
	{
		AT24C02_WriteByte(255,0XAB);
	  Testdata=AT24C02_ReadByte(255);	  
		if(Testdata==0XAB)return 0;
	}
	return 1;											  
}

/****************************************************************************
* ��    ��: u32 Buf_4Byte(uint8_t *pBuffer,u32 Date_4Byte,uint8_t Byte_num,uint8_t mode)
* ��    �ܣ���λ�����ֽڻ�ת
* ��ڲ�����mode��1:��λ��ת�ֳ��ֽ�   0:�ֽںϲ���һ����λ��
            Byte_num����Ҫת�����ֽ���
            *pBuffer���ֽڽ���������ֽ���������
            Date_4Byte����λ����
* ���ز�����modeΪ0ʱ�����ض�λ��
* ˵    ����Byte_num���Ϊ4���ֽڣ��ú����ں���Ĵ�����У��ʱ��ȡУ��ֵ���õ�
****************************************************************************/
u32 Buf_4Byte(uint8_t *pBuffer,u32 Date_4Byte,uint8_t Byte_num,uint8_t mode)
{
   uint8_t i; u32 middata=0;
	if(mode)    //��λ��ת�ֳ��ֽ�
	 {
	   for(i=0;i<Byte_num;i++)
	     {
	       *pBuffer++ =(Date_4Byte>>(8*i))&0xff;
	     }
			return 0; 
	 } 
	 else       //�ֽںϲ���һ����λ��
	 {
	    Date_4Byte=0;
		  pBuffer+=(Byte_num-1);
		  for(i=0;i<Byte_num;i++)
	      { 		
		      middata<<=8;
		      middata+= *pBuffer--;			   
	      }
			return middata;	
	 }
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
void AT24C02_Read(uint8_t ReadAddr,uint8_t *pBuffer,uint8_t ReadNum)
{
	while(ReadNum--)
	{
		*pBuffer++=AT24C02_ReadByte(ReadAddr++);	
	}
} 

/****************************************************************************
* ��    ��: void AT24C02_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint8_t WriteNum)
* ��    �ܣ���AT24C02�����ָ����ַ��ʼд��ָ������������
* ��ڲ�����WriteAddr :��ʼд��ĵ�ַ  0~255
            pBuffer  :���������׵�ַ
            WriteNum:Ҫд�����ݵĸ���
* ���ز�����
* ˵    ����  �B
****************************************************************************/
void AT24C02_Write(uint8_t WriteAddr,uint8_t *pBuffer,uint8_t WriteNum)
{
	while(WriteNum--)
	{
		AT24C02_WriteByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








