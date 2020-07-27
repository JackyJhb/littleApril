#ifndef __PWM_H
#define __PWM_H	 
#include "stm32f4xx.h"

extern void SCR_Init(void);
extern void Timer9Init(uint16_t arr,uint16_t psc);
extern void Timer9PWMInit(uint16_t arr,uint32_t psc);

#endif

















