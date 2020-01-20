#ifndef _ds18b20_H
#define _ds18b20_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#define    CH1        GPIO_Pin_4
#define    GROUP_E    GPIOE 

#define    CH2        GPIO_Pin_15
#define    GROUP_G    GPIOG


extern void DS18B20_GPIO_Init(void);
extern uint8_t ReadTemperature(float *temperature,uint16_t whichOne);
extern void delays_us(uint32_t delay);
#endif
