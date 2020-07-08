#include "ds18b20.h"
//PB0\PB1

#define    DS18B20_DQ_H(GROUP,CH)            GPIO_SetBits(GROUP,CH) 
#define    DS18B20_DQ_L(GROUP,CH)            GPIO_ResetBits(GROUP,CH)
#define    DS18B20_DQ_ReadPin(GROUP,CH)  	   GPIO_ReadInputDataBit(GROUP,CH)

void DS18B20_DQ_DDR(uint8_t ddr,uint16_t whichOne);
int DS18B20_reset(uint16_t whichOne);
void DS18B20_Wbyte(uint8_t xbyte,uint16_t whichOne);
uint8_t DS18B20_Rbit(uint16_t whichOne);
uint8_t DS18B20_Rbyte(uint16_t whichOne);

void delays_us(uint32_t delay)
{
	uint16_t i = 0;
	while(delay--)
	{
		i = 30;
		while(i--);
	}
}

void DS18B20_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
}

void DS18B20_DQ_DDR(uint8_t ddr,uint16_t whichOne)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin		= whichOne;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;
	if (ddr)
	{
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	}
	GPIO_Init(GROUP_G,&GPIO_InitStructure);
	
	/*GPIO_InitStructure.GPIO_Pin		= CH2;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;
	if (ddr)
	{
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	}
	GPIO_Init(GROUP_G,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin		= CH3;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;
	if (ddr)
	{
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN;
	}
	GPIO_Init(GROUP_G,&GPIO_InitStructure);*/
}

int DS18B20_reset(uint16_t whichOne) 
{
    int  x = 0;
	GPIO_TypeDef *group;
	
	group = GROUP_G;
	DS18B20_DQ_DDR(1,whichOne);
	DS18B20_DQ_H(group,whichOne);
	delays_us(100);
	DS18B20_DQ_L(group,whichOne);
	delays_us(300);
	DS18B20_DQ_H(group,whichOne);
	
	DS18B20_DQ_DDR(0,whichOne);
	while(DS18B20_DQ_ReadPin(group,whichOne) == 1 && ++x < 0x100)
	{
		delays_us(1);
	}
	if (x >= 0x100)
	{
		return 1;
	}
	x = 0;
	while(DS18B20_DQ_ReadPin(group,whichOne) == 0 && ++x < 0x400)
	{
		delays_us(1);
	}
	if (x >= 0x400 )
	{
		return 1;
	}
    return 0;
}

void DS18B20_Wbyte(uint8_t xbyte,uint16_t whichOne)
{
	int8_t i ,x = 0;
	GPIO_TypeDef *group;
	
	group = GROUP_G;
	DS18B20_DQ_DDR(1,whichOne);
	for(i = 1; i <= 8; i++)
	{
		x = xbyte & 0x01;
		if(x)
		{
			DS18B20_DQ_L(group,whichOne);
			delays_us(2);
			DS18B20_DQ_H(group,whichOne);
			delays_us(60);
		}
		else
		{
			DS18B20_DQ_L(group,whichOne);
			delays_us(60);
			DS18B20_DQ_H(group,whichOne);
			delays_us(2);
		}
		xbyte = xbyte >> 1;
	}
}

uint8_t DS18B20_Rbit(uint16_t whichOne)
{
	uint8_t rbit = 0x00,x = 0;
	GPIO_TypeDef *group;
	
	group = GROUP_G;
	DS18B20_DQ_DDR(1,whichOne);
	DS18B20_DQ_L(group,whichOne);
	delays_us(2);
	DS18B20_DQ_H(group,whichOne);
	DS18B20_DQ_DDR(0,whichOne);
	delays_us(11);
	x = DS18B20_DQ_ReadPin(group,whichOne);
	if(x)
		rbit = 0x80;
	delays_us(50);
		return rbit;
}
 
uint8_t DS18B20_Rbyte(uint16_t whichOne)
{
    uint8_t rbyte = 0,i = 0, tempbit =0;
    for (i = 1; i <= 8; i++)
    {
        tempbit = DS18B20_Rbit(whichOne);
        rbyte = rbyte >> 1;
        rbyte = rbyte|tempbit;
    }
    return rbyte;
}

uint8_t ReadTemperature(float *temperature,uint16_t whichOne)
{
	uint8_t TempL,TempH;
	int fg;
	int data;
	float temper;
	if (DS18B20_reset(whichOne) == 1)
	{
		return 1;
	}
	DS18B20_Wbyte(0xCC,whichOne);
	DS18B20_Wbyte(0x44,whichOne);
	delays_us(10000);
	if (DS18B20_reset(whichOne) == 1)
	{
		return 1;
	}
	DS18B20_Wbyte(0xCC,whichOne);
	DS18B20_Wbyte(0xBE,whichOne);
	TempL = DS18B20_Rbyte(whichOne);
	TempH = DS18B20_Rbyte(whichOne);
	if(TempH > 0x70)
	{
		TempL = ~TempL;
		TempH = ~TempH;
		fg = 0;
	}
	else 
		fg = 1;
	data = TempH;
	data <<=  8;
	data += TempL;
	temper = (float)data*0.625/10.0;
	if ((temper == 85.00) || (temper >=100))
	{
		return 1;
	}
	if(fg)
	{
		*temperature = temper;
	}
	else
	{
		*temperature = -temper;
	}
	return 0;
}
