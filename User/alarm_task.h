#ifndef ALARM_TASK_H
#define ALARM_TASK_H

#include "includes.h"
#include "priolevel.h"

/*#define AREA1_TEMPERATURE_ALARM 			0x01
#define AREA2_TEMPERATURE_ALARM				0x02
#define AREA3_TEMPERATURE_ALARM				0x04
#define BOILER_TEMPERATURE_ALARM            0x08
#define AREA1_HUMIDITY_ALARM                0x10
#define AREA2_HUMIDITY_ALARM                0x20
#define AREA3_HUMIDITY_ALARM                0x40*/
#define INSIDE_TEMPER_HIGH_ALARM           0x00000001
#define INSIDE_TEMPER_LOW_ALARM            0x00000002
#define BOILER_TEMPER_HIGH_ALARM          0x00000004
#define BOILER_TEMPER_LOW_ALARM           0x00000008

#define BOILER_HEATING_ALARM                    0x00000010
#define CLIENT_CONNECT_ERR                      0x00000020
//#define CLIENT2_CONNECT_ERR                      0x00000040
//#define CLIENT3_CONNECT_ERR                      0x00000080

#define PUMP_CIRCULATE_ALARM                   0x00000100
#define FAN_SHAFT_ALARM                              0x00000200
#define AREA_HEATING_ERR                          0x00000400
//#define AREA2_HEATING_ERR                          0x00000800

//#define AREA3_HEATING_ERR                          0x00001000
#define OUTSIDE_SENSOR_ERR                       0x00002000
#define PIPE_SENSOR_ERR															 0x00004000
#define BOILER_SENSOR_ERR												 0x00008000

#define AREA_SENSOR_ERR													 0x00010000
//#define AREA2_SENSOR_ERR                          0x00020000
//#define AREA3_SENSOR_ERR                          0x00040000
#define LEFT_SMWIN_ERR                               0x00080000

#define RIGHT_SMWIN_ERR                            0x00100000
#define LEFT_BGWIN_ERR                              0x00200000
#define RIGHT_BGWIN_ERR                            0x00400000

#define PIRACY_ALARM                  									0x80000000


#define ERR_TIMES_LIMIT                                10

#define ALARM_STK_SIZE 		512

extern OS_TCB AlarmTaskTCB;
extern CPU_STK ALARM_TASK_STK[ALARM_STK_SIZE];
extern void alarm_task(void *p_arg);
#endif
