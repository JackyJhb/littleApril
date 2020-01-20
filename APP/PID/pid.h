#ifndef PID_TEMPERATURE_H
#define PID_TEMPERATURE_H

/*#define outdoorTemperature 36.2
#define TemperatureErr        3
#define ComfortTemperature 0.5
#define IncrementTemperature 1*/
#include "sccf.h"

typedef struct{
	PIDParameter ctrlRef;
	float integral;
	float actualTemperature;
	float errVal[3];
	int lastTime[3];
	int lastTimeFlag;
}PIDValue;

extern void initPID(PIDParameter *pid_ptr);
extern void acquireTemperature(int hour,int minute,int second,PIDValue *pid_ptr);
extern void pidControlTemperature(float set_temperature,float actual_temperature,uint8_t which_one);
extern void refrigeration(void);
extern float incrementPIDRealize(float actual_temperature,uint8_t which_one);


#endif
