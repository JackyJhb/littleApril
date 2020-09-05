#ifndef PID_TEMPERATURE_H
#define PID_TEMPERATURE_H

#include "sccf.h"

extern void pidControlTemperature(float set_temperature,float actual_temperature,uint8_t which_one);
#endif
