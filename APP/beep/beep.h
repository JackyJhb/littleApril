#ifndef _beep_H
#define _beep_H

#include "system.h"
#define BEEP_ON  0
#define BEEP_OFF 1
#define BEEP_Pin (GPIO_Pin_8)  //����BEEP�ܽ�
#define BEEP_Port (GPIOF) //����BEEP�˿�

#define Beep PFout(8)  //BEEP PF8
void BEEP_Init(void);


#endif
