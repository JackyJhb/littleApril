#ifndef SIDE_WINDOW_CTRL_TASK_H
#define SIDE_WINDOW_CTRL_TASK_H

#include "includes.h"
#include "stm32f4xx.h"
#include "priolevel.h"

#define SIDEWINDOWCTRL_STK_SIZE 		512

typedef enum {
    SmallLeftOpen,
    SmallLeftClose,
    SmallLeftStop,
    SmallRightOpen,
    SmallRightClose,
    SmallRightStop,
    BigLeftOpen,
    BigLeftClose,
    BigLeftStop,
    BigRightOpen,
    BigRightClose,
    BigRightStop
}WindowAction;

#define SMALL_LEFT_OPEN   ((uint32_t)0x00000001)
#define SMALL_LEFT_CLOSE  ((uint32_t)0x00000002)
#define SMALL_RIGHT_OPEN  ((uint32_t)0x00000004)
#define SMALL_RIGHT_CLOSE ((uint32_t)0x00000008)
#define BIG_LEFT_OPEN     ((uint32_t)0x00000010)
#define BIG_LEFT_CLOSE    ((uint32_t)0x00000020)
#define BIG_RIGHT_OPEN    ((uint32_t)0x00000040)
#define BIG_RIGHT_CLOSE   ((uint32_t)0x00000080)

#define WIN_WORKING_MASK ((uint32_t)0x00000FF)

extern OS_TCB SideWindowCtrlTaskTCB;
extern CPU_STK SIDEWINDOWCTRL_TASK_STK[SIDEWINDOWCTRL_STK_SIZE];
extern void sidewindowctrl_task(void *p_arg);
#endif
