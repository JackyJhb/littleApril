#include "stm32f4xx.h"
#include <stdio.h>
#include "bsp_gpio.h"
#include "math.h"
#include "../Keyboard/bsp_button.h"
#include "debug_config.h"
//#include "bsp_timer.h"

/*#define  DECODE_MODE			0x09  
#define  INTENSITY					0x0A  
#define  SCAN_LIMIT				0x0B  
#define  SHUT_DOWN				0x0C  
#define  DISPLAY_TEST			0x0F 

#define DA1_72V						0
#define DA2_5V						1
#define DA3_5V						2
#define DA3_NULL					3*/

typedef struct
{
	GPIO_TypeDef*		GPIOx;
	unsigned short		GPIO_Pin;
}GPIO_Index_Struct;

unsigned short InputGPIO[8]		= {0};							//72��IO������,��7��(0-6),0,1��ÿ��16����2-6��ÿ��8��
/*unsigned short OutputGPIO[5]	= {0xff,0xff,0xff,0xff,0xff};	//40��IO�����,��5��(0-4)��ÿ��8��

void delay(unsigned int time)
{
	while(time--);
}

GPIO_Index_Struct InputGPIO_CS[10] =
{
	{GPIOG,GPIO_Pin_8},{GPIOG,GPIO_Pin_7},{GPIOG,GPIO_Pin_2},{GPIOD,GPIO_Pin_11},{GPIOD,GPIO_Pin_0},
	{GPIOD,GPIO_Pin_2},{GPIOB,GPIO_Pin_6},{GPIOB,GPIO_Pin_7},{GPIOE,GPIO_Pin_2},{GPIOD,GPIO_Pin_1},
};

GPIO_Index_Struct InputGPIO_BUS[8] = 
{
	{GPIOG,GPIO_Pin_6},{GPIOG,GPIO_Pin_5},{GPIOG,GPIO_Pin_4},{GPIOG,GPIO_Pin_3},
	{GPIOB,GPIO_Pin_8},{GPIOB,GPIO_Pin_9},{GPIOE,GPIO_Pin_0},{GPIOE,GPIO_Pin_1},
};

unsigned char InputGPIO_Mapping[5][8] = 
{
	{0x00,0x02,0x04,0x06,0x10,0x12,0x14,0x16},
	{0x01,0x03,0x05,0x07,0x11,0x13,0x15,0x17},
	{0x20,0x22,0x24,0x26,0x30,0x32,0x34,0x36},
	{0x21,0x23,0x25,0x27,0x31,0x33,0x35,0x37},
	{0x40,0x42,0x44,0x46,0x41,0x43,0x45,0x47},
};

GPIO_Index_Struct OutputGPIO_CS[5] = 
{
	{GPIOD,GPIO_Pin_10},{GPIOE,GPIO_Pin_12},{GPIOE,GPIO_Pin_10},{GPIOF,GPIO_Pin_15},{GPIOF,GPIO_Pin_13},
};

GPIO_Index_Struct OutputGPIO_BUS[8] = 
{
	{GPIOE,GPIO_Pin_9},{GPIOE,GPIO_Pin_8},{GPIOE,GPIO_Pin_7},{GPIOB,GPIO_Pin_15},
	{GPIOB,GPIO_Pin_14},{GPIOB,GPIO_Pin_12},{GPIOG,GPIO_Pin_1},{GPIOG,GPIO_Pin_0},
};

unsigned char OutputGPIO_Mapping[5][8] = 
{
	{0x00,0x02,0x04,0x06,0x10,0x12,0x14,0x16},
	{0x01,0x03,0x05,0x07,0x11,0x13,0x15,0x17},
	{0x20,0x22,0x24,0x26,0x30,0x32,0x34,0x36},
	{0x21,0x23,0x25,0x27,0x31,0x33,0x35,0x37},
	{0x40,0x42,0x44,0x46,0x41,0x43,0x45,0x47},
};*/
/*GPIO_Index_Struct littleAprilOutputGPIO[8] = {
	{GPIOC,GPIO_Pin_6},{GPIOC,GPIO_Pin_0},{GPIOC,GPIO_Pin_2},{GPIOB,GPIO_Pin_5},
	{GPIOB,GPIO_Pin_12},{GPIOB,GPIO_Pin_13},{GPIOC,GPIO_Pin_13},{GPIOC,GPIO_Pin_3},
};*/

/*GPIO_Index_Struct littleAprilFanOutputGPIO[16] = {
	{GPIOF,GPIO_Pin_6},{GPIOE,GPIO_Pin_6},{GPIOE,GPIO_Pin_5},{GPIOC,GPIO_Pin_11},
	{GPIOC,GPIO_Pin_9},{GPIOC,GPIO_Pin_8},{GPIOC,GPIO_Pin_7},{GPIOF,GPIO_Pin_7},
	{GPIOF,GPIO_Pin_8},{GPIOC,GPIO_Pin_4},{GPIOC,GPIO_Pin_5},{GPIOF,GPIO_Pin_9},
	{GPIOB,GPIO_Pin_1},{GPIOG,GPIO_Pin_3},{GPIOG,GPIO_Pin_4},{GPIOG,GPIO_Pin_5},

};*/
GPIO_Index_Struct littleAprilOutputGPIO[8] = {
	{GPIOG,GPIO_Pin_3},{GPIOG,GPIO_Pin_4},{GPIOB,GPIO_Pin_12},{GPIOB,GPIO_Pin_13},
	{GPIOC,GPIO_Pin_8},{GPIOC,GPIO_Pin_9},{GPIOC,GPIO_Pin_11},{GPIOC,GPIO_Pin_13},
};
#ifdef NEW_CIRCURIT_BOARD
GPIO_Index_Struct littleAprilFanOutputGPIOCS[5] = {
	{GPIOG,GPIO_Pin_5},{GPIOF,GPIO_Pin_6},{GPIOF,GPIO_Pin_7},{GPIOC,GPIO_Pin_0},{GPIOC,GPIO_Pin_2}
};
#else
GPIO_Index_Struct littleAprilFanOutputGPIO[16] = {
	{GPIOG,GPIO_Pin_5},{GPIOF,GPIO_Pin_6},{GPIOF,GPIO_Pin_7},{GPIOE,GPIO_Pin_5}/*E*/,
	{GPIOE,GPIO_Pin_6}/*E*/,{GPIOC,GPIO_Pin_0},{GPIOC,GPIO_Pin_2},{GPIOC,GPIO_Pin_3},
	{GPIOF,GPIO_Pin_4}/*E*/,{GPIOC,GPIO_Pin_5},{GPIOC,GPIO_Pin_6},{GPIOC,GPIO_Pin_7},
	{GPIOB,GPIO_Pin_5},{GPIOB,GPIO_Pin_1},{GPIOE,GPIO_Pin_15},{GPIOF,GPIO_Pin_9},
};
#endif

void littleAprilIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | 
								GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | 
				   GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_15);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOF,GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOG,GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
}

void littleAprilIOCtrl(WhichRelay whichOne,OnOrOff onOrOff)
{
	#ifdef ENABLE_OUTPUT_LOG
	printf("Info:bsp_gpio.c::littleAprilIOCtrl()->%d is %d\r\n",whichOne,onOrOff);
	#endif
	if (onOrOff == On)
	{
		GPIO_SetBits(littleAprilOutputGPIO[whichOne].GPIOx,littleAprilOutputGPIO[whichOne].GPIO_Pin);
	}
	else
	{
		GPIO_ResetBits(littleAprilOutputGPIO[whichOne].GPIOx,littleAprilOutputGPIO[whichOne].GPIO_Pin);
	}
}

void littleAprilFanCtrl(uint32_t relayCtrlGroup)
{
	char i;
	uint32_t temp;
	#ifdef ENABLE_OUTPUT_LOG
	printf("Info:bsp_gpio.c::littleAprilFanCtrl()->relayCtrlGroup is %d\r\n",relayCtrlGroup);
	#endif
	for (i = 0;i < 16;i++)
	{
		temp = (1 << i);
		if (relayCtrlGroup & temp)
		{
			GPIO_SetBits(littleAprilFanOutputGPIO[i].GPIOx,littleAprilFanOutputGPIO[i].GPIO_Pin);
		}
		else
		{	
			GPIO_ResetBits(littleAprilFanOutputGPIO[i].GPIOx,littleAprilFanOutputGPIO[i].GPIO_Pin);
		}
	}
}

/*//////////////////////////////////�ӿں���//////////////////////////////////////////////////
////X������ӿ�////
void X_Input_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    //X������//////////////////////////////////////////////////////
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG,ENABLE);
    //ƬѡGPIO
	GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;												//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_11);
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;												//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_SetBits(GPIOE, GPIO_Pin_2);
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;												//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);    
	
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;												//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	//X��������GPIO
	GPIO_SetBits(GPIOG, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;													//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;													//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;													//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_DOWN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void Updata_Input_GPIO()
{
	int i,j;
	unsigned char ingpio[10];
	unsigned char offset1,offset2;
	for(i = 0; i < 10; i++)
	{
		GPIO_ResetBits(InputGPIO_CS[i].GPIOx,InputGPIO_CS[i].GPIO_Pin);      //Ƭѡ����
		ingpio[i] = 0;
		for(j = 0; j < 8; j++)
		{
			ingpio[i] >>= 1;
			if(GPIO_ReadInputDataBit(InputGPIO_BUS[j].GPIOx,InputGPIO_BUS[j].GPIO_Pin) == 0)
			{
				ingpio[i] |= 0x80;
			}
		}
		GPIO_SetBits(InputGPIO_CS[i].GPIOx,InputGPIO_CS[i].GPIO_Pin);         //
	}
	InputGPIO[0] = ((ingpio[1] << 8) & 0xff00) + (ingpio[0] & 0x00ff);
	InputGPIO[1] = ((ingpio[3] << 8) & 0xff00) + (ingpio[2] & 0x00ff);
	unsigned char InputGPIO_Mapping[5][8] = 
{
	{0x00,0x02,0x04,0x06,0x10,0x12,0x14,0x16},
	{0x01,0x03,0x05,0x07,0x11,0x13,0x15,0x17},
	{0x20,0x22,0x24,0x26,0x30,0x32,0x34,0x36},
	{0x21,0x23,0x25,0x27,0x31,0x33,0x35,0x37},
	{0x40,0x42,0x44,0x46,0x41,0x43,0x45,0x47},
};
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 8; j++)
		{
			offset1 = InputGPIO_Mapping[i][j] >> 4;
			offset2 = InputGPIO_Mapping[i][j] & 0x0f;
			if((ingpio[4+offset1] >> offset2) & 0x01)
			{
				InputGPIO[2+i] |= (0x01 << j);
			}
			else
			{
				InputGPIO[2+i] &= ~(0x01 << j);
			}
		}
	}   
	InputGPIO[7] = ingpio[9];
}

unsigned short Get_Input_GPIOX_Group(unsigned char Group)
{
	if(Group > 7)
		return 0;
	else
		return InputGPIO[Group];
}

unsigned char Get_Input_GPIOX_Bit(unsigned char Group,unsigned char Bit)
{
	if(Group > 7 || Bit > 15)
		return 0;
	if(InputGPIO[Group] & (0x01 << Bit))
		return 1;
	else
		return 0;
}

////Y������ӿ�//////
void Y_Output_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOF,ENABLE);
	//ƬѡGPIO
	GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	//Y���������
	GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin			= GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode		= GPIO_Mode_OUT;					//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd		= GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void Update_Output_GPIO()
{
    unsigned int i,j;
    unsigned char outgpio[5];
    unsigned char offset1,offset2;
    
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 8; j++)
        {
            offset1 = OutputGPIO_Mapping[i][j] >> 4;
            offset2 = OutputGPIO_Mapping[i][j] & 0x0f;
            
            if(Get_Output_GPIOY_Bit(i,j))
            {
                outgpio[offset1] |= (0x01 << offset2);
            }
            else
            {
                outgpio[offset1] &= ~(0x01 << offset2);
            }
        }
    }    
    
    for(i = 0; i < 5; i++)
    {
    	for(j = 0; j < 8; j++)
    	{
		    if((outgpio[i] >> j) & 0x01)
			{
			    GPIO_ResetBits(OutputGPIO_BUS[j].GPIOx,OutputGPIO_BUS[j].GPIO_Pin);
			}
			else
			{
				GPIO_SetBits(OutputGPIO_BUS[j].GPIOx,OutputGPIO_BUS[j].GPIO_Pin);
			}
        }
		GPIO_SetBits(OutputGPIO_CS[i].GPIOx,OutputGPIO_CS[i].GPIO_Pin);
		GPIO_ResetBits(OutputGPIO_CS[i].GPIOx,OutputGPIO_CS[i].GPIO_Pin);
    }
}

//GPIO �������
void Set_GPIOY_Group(unsigned char Group,unsigned short Data)
{
    if((Group > 4))
        return;
    OutputGPIO[Group] = Data;
}

void Set_Output_GPIOY_Bit(unsigned char Group,unsigned char Bit)
{
    if(Group > 4 || Bit > 15)
        return;
    OutputGPIO[Group] |= (0x01 << Bit);
}

void Reset_Output_GPIOY_Bit(unsigned char Group,unsigned char Bit)
{
    if((Group > 4) || (Bit > 15))
        return;
    OutputGPIO[Group] &= ~(0x01 << Bit);
}

unsigned char Get_Output_GPIOY_Bit(unsigned char Group,unsigned char Bit)
{
    if(Group > 4 || Bit > 15)
        return 0;

    if(OutputGPIO[Group] & (0x01 << Bit))
        return 1;
    else
        return 0;    
}
void LED_RUN_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
 
    GPIO_ResetBits(GPIOD,GPIO_Pin_4); 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);    
}
//LEDRUN����
void LED_RUN_ON()
{
    GPIO_SetBits(GPIOD,GPIO_Pin_4);
}

void LED_RUN_OFF()
{
    GPIO_ResetBits(GPIOD,GPIO_Pin_4);
}
//////////////////////////////////////////////////////////
//RS485�������
void RS485_DIR_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
    GPIO_ResetBits(GPIOF,GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);     
}

void SetRS485ReadCOM1()
{
    GPIO_ResetBits(GPIOF,GPIO_Pin_5);	
}

void SetRS485WriteCOM1()
{
    GPIO_SetBits(GPIOF,GPIO_Pin_5);	
}

void SetRS485ReadCOM2()
{
    GPIO_ResetBits(GPIOF,GPIO_Pin_4);	
}

void SetRS485WriteCOM2()
{
    GPIO_SetBits(GPIOF,GPIO_Pin_4);	
}

void SetRS485ReadCOM3()
{
    GPIO_ResetBits(GPIOF,GPIO_Pin_3);	
}

void SetRS485WriteCOM3()
{
    GPIO_SetBits(GPIOF,GPIO_Pin_3);	
}
///////////////////////////////ģ��I2C////////////////////////////////

//--------------------------------------------------------------
// Prototype      : void I2C_Start(void)
// Calls          : Delay_5us()
// Description    : Start Singnal
//--------------------------------------------------------------
void Set_SCL_H()
{
    GPIO_SetBits(GPIOB,  GPIO_Pin_6);
}

void Set_SCL_L()
{
    GPIO_ResetBits(GPIOB,  GPIO_Pin_6);
}

void Set_SDA_Output()
{
    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Set_SDA_Input()
{
    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Set_SDA_H()
{
    GPIO_SetBits(GPIOB,  GPIO_Pin_7);
}

void Set_SDA_L()
{
    GPIO_ResetBits(GPIOB,  GPIO_Pin_7);
}

unsigned char Get_SDA_Input()
{
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
        return 1;
    else
        return 0;
}

void I2C_Start(void)
{
    Set_SDA_Output();
    Set_SDA_H();                    
    Set_SCL_H();                                   
    Set_SDA_L();                                 
    Set_SCL_L();                    
}


//--------------------------------------------------------------
// Prototype      : void I2C_Stop(void)
// Calls          : Delay_5us()
// Description    : Stop Singnal
//-------------------------------------------------------------- 
void I2C_Stop(void)
{
    Set_SDA_L();                   
    Set_SCL_H();                                 
    Set_SDA_H();                                
}

//--------------------------------------------------------------
// Prototype      : void I2C_SendACK(uchar ack);
// Calls          : Delay_5us()
// Parameters     : bit ack:1-noack, 0-ack
// Description    : Master device send ACK to slave device.
//--------------------------------------------------------------
void I2C_SendACK(unsigned char ack)
{
    if(ack == 0)
    {
      Set_SDA_L(); 
    }  
    else
    {
      Set_SDA_H(); 
    }
            
    Set_SCL_H();                   
    
    Set_SCL_L();                         
}

//--------------------------------------------------------------
// Prototype      : uchar I2C_SendByte(uchar sendDAT)
// Calls          : Delay_5us()
// Parameters     : uchar sendDAT---data to be send
// Return Value   : CY--slave ack (1---noack,0---ack)
// Description    : Send one byte to I2C
//--------------------------------------------------------------
unsigned char I2C_SendByte(unsigned char sendDAT)
{
    unsigned char i,revack;

    for (i = 0 ; i < 8; i++)         
    {
        Set_SCL_L();                 

        if(sendDAT & 0x80)             // write data
        {
            Set_SDA_H();
        }
        else   
        {
            Set_SDA_L();
        }
    
        sendDAT <<=  1;
    
        Set_SCL_H();                    
    } 
 
    Set_SCL_L();                 
    
    Set_SDA_Input();
    
    Set_SCL_H();    
    
    revack = Get_SDA_Input();
   
    Set_SCL_L();    
       
    Set_SDA_Output(); 
    
    return revack;
}


//--------------------------------------------------------------
// Prototype      : uchar I2C_RecvByte()
// Calls          : Delay_5us()
// Parameters     : none
// Return Value   : revDAT- received data
// Description    : Receive one byte from I2C
//--------------------------------------------------------------
unsigned char I2C_RecvByte()
{
    unsigned char i;
    unsigned char revDAT = 0;
	Set_SDA_Input();
	for (i = 0; i < 8; i++)
	{
		revDAT <<= 1;
		Set_SCL_H();
		if(Get_SDA_Input())
		{
			revDAT |= 0x01;
		}
		else
		{
			revDAT &= 0xfe;
		}
		Set_SCL_L();                  
	}
	Set_SDA_Output();
	return revDAT;
}

void Write_24C16(unsigned short int regadd,unsigned char data)
{
	unsigned char addr;
	addr = 0xA0 | ((regadd >> 7) & 0x0e);
	I2C_Start();
	I2C_SendByte(addr);     // Device Addr + Write (operation) 
	addr = regadd&0x00ff;
	I2C_SendByte(addr);
	I2C_SendByte(data);
	I2C_Stop();
}

unsigned char Read_24C16(unsigned short int regadd)
{
	unsigned char revdata;
	unsigned char addr;
	I2C_Start();
	addr = 0xA0 | ((regadd >> 7) & 0x0e);            
	I2C_SendByte(addr);     // Device Addr + Write (operation)
	addr = regadd&0x00ff;
	I2C_SendByte(addr); 
	I2C_Start();
	addr = 0xA1 | ((regadd >> 7) & 0x0e);
	I2C_SendByte(addr);     // Device Addr + Read (operation)
	revdata = I2C_RecvByte();
	I2C_SendACK(1);
	I2C_Stop();
	return revdata;
}


/////////////////////////////////////////////////////////////////////////////////////
void SPI_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);    
    //SCLK
    GPIO_ResetBits(GPIOB,GPIO_Pin_3); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
    //MOSI
    GPIO_ResetBits(GPIOB,GPIO_Pin_5); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
}

void Set_SPI_SCLK_H()
{
    GPIO_SetBits(GPIOB,  GPIO_Pin_3);
}

void Set_SPI_SCLK_L()
{
    GPIO_ResetBits(GPIOB,  GPIO_Pin_3);
}

void Set_SPI_MOSI_H()
{
    GPIO_SetBits(GPIOB,  GPIO_Pin_5);
}

void Set_SPI_MOSI_L()
{
    GPIO_ResetBits(GPIOB,  GPIO_Pin_5);
}

unsigned char Get_SPI_MISO()
{
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4))
        return 1;
    else
        return 0;
}

void SPI_WriteByte(unsigned char data)
{
    unsigned char i,temp;

    for(i = 0; i < 8; i++)
    {
        Set_SPI_SCLK_L();
        delay(100);
        temp = data & 0x80;
        data = data << 1;
        if(temp)
            Set_SPI_MOSI_H();
        else
            Set_SPI_MOSI_L();
        delay(100);
        Set_SPI_SCLK_H();
        delay(100);         
    }
}

void Reset_AD5592()
{
	GPIO_ResetBits(GPIOE,  GPIO_Pin_4);
    GPIO_ResetBits(GPIOF,  GPIO_Pin_1);
	delay(1000);
	GPIO_SetBits(GPIOE,  GPIO_Pin_4);
    GPIO_SetBits(GPIOF,  GPIO_Pin_1);
	delay(1000);
}

void Set_AD5592_1_Enable()
{
    GPIO_ResetBits(GPIOF,  GPIO_Pin_0);
}

void Set_AD5592_1_Disable()
{
    GPIO_SetBits(GPIOF,  GPIO_Pin_0);
}

void Set_AD5592_2_Enable()
{
    GPIO_ResetBits(GPIOE,  GPIO_Pin_3);
}

void Set_AD5592_2_Disable()
{
    GPIO_SetBits(GPIOE,  GPIO_Pin_3);
}


void Write_AD5592(unsigned char who,unsigned short data) 
{         
    unsigned char i;      
    
	Set_SPI_SCLK_L();
	delay(20);
    if(who == 0)
    {
        Set_AD5592_1_Enable();
	}
    else
    {
        Set_AD5592_2_Enable();
    }
    delay(20);
    for(i = 0; i < 16; i++)
    {            
        Set_SPI_SCLK_H();
		delay(20);

        if(data & 0x8000)
        {
            Set_SPI_MOSI_H();
        }
        else
        {
            Set_SPI_MOSI_L();
        }
        data <<= 1;
        delay(20);
        Set_SPI_SCLK_L();	  //�½���д��
		delay(20);
    }
	Set_SPI_SCLK_H();
    if(who == 0)
    {
        Set_AD5592_1_Disable();
	}
    else
    {
        Set_AD5592_2_Disable();
    }    
	delay(50);    
} 
//0011 1000 0101 0011 0x3853
unsigned short Read_AD5592(unsigned char who) 
{         
    unsigned char i;
	unsigned short data;
	
	data = 0;
	      
    Set_SPI_SCLK_L();
	delay(20);
    if(who == 0)
    {
        Set_AD5592_1_Enable();
	}
    else
    {
        Set_AD5592_2_Enable();
    }
	delay(20);
    for(i = 0; i < 16; i++)
    {            
        data <<= 1;
		Set_SPI_SCLK_H();
		delay(20);
		if(Get_SPI_MISO())
		{
			data += 1;
		}      
        Set_SPI_SCLK_L();
		delay(20);	
    }
	Set_SPI_SCLK_H();
    if(who == 0)
    {
        Set_AD5592_1_Disable();
	}
    else
    {
        Set_AD5592_2_Disable();
    }
	delay(50);

	return data;    
}
//0x200F IO0-IO3 AD		 //����AD
//0x28F0 IO4-IO7 DA		 //����DA
//0x8000 | (((daIndex&0x03)+0x04)<<12) | (data&0x0FFF) //д12λ���ݵ�DA
void Init_AD5592(void)       
{
	GPIO_InitTypeDef GPIO_InitStructure;
    //Ƭѡ����λ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF,ENABLE);    

    GPIO_SetBits(GPIOE,GPIO_Pin_3 | GPIO_Pin_4); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_SetBits(GPIOF,GPIO_Pin_0 | GPIO_Pin_1); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    

    Reset_AD5592();
    
	Write_AD5592(0,0x20C3);  //AD���� 0,1,6,7        
    Write_AD5592(0,0x283C);  //DA���� 2,3,4,5
	Write_AD5592(0,0x3802);
	Write_AD5592(0,0x5A00);  //�ο���ѹ�������
	Write_AD5592(0,0x18B0);  //DA�����ѹ��Χ���ã�2���ο���ѹ���,AD�����ѹ��Χ���ã�2���ο���ѹ	        
    Write_AD5592(0,0x97FF);
	Write_AD5592(0,0xa7FF);
	Write_AD5592(0,0xb7FF);
	Write_AD5592(0,0xc7FF);

    //Write_AD5592(0,0x12C3);

	Write_AD5592(1,0x20C3);  //AD���� 0,1,6,7        
    Write_AD5592(1,0x283C);  //DA���� 2,3,4,5
	Write_AD5592(1,0x3802);
	Write_AD5592(1,0x5A00);  //�ο���ѹ�������
	Write_AD5592(1,0x18B0);  //DA�����ѹ��Χ���ã�2���ο���ѹ���,AD�����ѹ��Χ���ã�2���ο���ѹ	        
    Write_AD5592(1,0x97FF);
	Write_AD5592(1,0xa7FF);
	Write_AD5592(1,0xb7FF);
	Write_AD5592(1,0xc7FF); 

    //Write_AD5592(1,0x12C3);
}


void Set_MAX7219_Enable()
{
    GPIO_ResetBits(GPIOF,  GPIO_Pin_2);
}

void Set_MAX7219_Disable()
{
    GPIO_SetBits(GPIOF,  GPIO_Pin_2);
}

void Write_Max7219(unsigned char address,unsigned char dat) 
{         
    unsigned char i;      
    
    Set_MAX7219_Enable();				  

    for(i = 0; i < 8; i++)
    {            
        Set_SPI_SCLK_L();
        delay(100);
        if(address & 0x80)
        {
            Set_SPI_MOSI_H();
        }
        else
        {
            Set_SPI_MOSI_L();
        }
        delay(100);
        address <<= 1;
        
        Set_SPI_SCLK_H();
        delay(100);
    }
        
    for(i = 0; i < 8; i++)                    
    {            
        Set_SPI_SCLK_L();
        delay(100);
        if(dat&0x80)
        {
            Set_SPI_MOSI_H();
        }
        else
        {
            Set_SPI_MOSI_L();
        }                   
        dat <<= 1;
        delay(100);           
        Set_SPI_SCLK_H();
        delay(100);
    }
    Set_SPI_SCLK_L();      
    Set_MAX7219_Disable();    
} 

void Init_Max7219(void)       
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//Ƭѡ����λ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);    
	GPIO_SetBits(GPIOF,GPIO_Pin_2); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);    
	Write_Max7219(SHUT_DOWN, 0x01);          
	Write_Max7219(DISPLAY_TEST, 0x00);        
	Write_Max7219(DECODE_MODE, 0x00);         
	Write_Max7219(SCAN_LIMIT, 0x03);     //SCAN LIMIT 0~7 0xX0~0xX7   
	Write_Max7219(INTENSITY, 0x01);       
}

void SetDAC_Voltage(unsigned char DAC_ID,float voltage)
{
	unsigned int DA_Data;	
	if(DAC_ID > 8)
		return;
	if(voltage < -10.0f)
	{
		voltage = -10.0f;
	}
	else if(voltage > 10.0f)
	{
		voltage = 10.0f;
	}
	voltage += 10.0f;
	DA_Data = (unsigned int)((2047.0f * voltage) / 10.0f);
	DA_Data = 4096 - DA_Data;
	switch(DAC_ID)
	{
		case 1:
			Write_AD5592(0,0xB000+(DA_Data&0xfff));
			//		printf("�ٶ�= %lf\r\n",voltage);
			break;
		case 2:
			Write_AD5592(0,0xC000+(DA_Data&0xfff));
			break;
		case 3:
			Write_AD5592(0,0xA000+(DA_Data&0xfff));
			break;
		case 4:		
			Write_AD5592(0,0xD000+(DA_Data&0xfff));
			break;
		case 5:
			Write_AD5592(1,0xB000+(DA_Data&0xfff));
			break;
		case 6:
			Write_AD5592(1,0xC000+(DA_Data&0xfff));
			break;
		case 7:
			Write_AD5592(1,0xA000+(DA_Data&0xfff));
			break;
		case 8:		
			Write_AD5592(1,0xD000+(DA_Data&0xfff));
			break;    
	}
}

void Init_DAC()
{
	Init_AD5592();
	SetDAC_Voltage(1,0.0);
	SetDAC_Voltage(2,0.0);
	SetDAC_Voltage(3,0.0);
	SetDAC_Voltage(4,0.0);
	SetDAC_Voltage(5,0.0);
	SetDAC_Voltage(6,0.0);
	SetDAC_Voltage(7,0.0);
	SetDAC_Voltage(8,0.0); 
}

void bsp_InitGPIO(void)
{
	X_Input_Init();
	Y_Output_Init();
}*/
