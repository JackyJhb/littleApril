#ifndef WATCHER_TASK_H
#define WATCHER_TASK_H

#include "includes.h"

#define WATCHER_TASK_PRIO	16
#define WATCHER_STK_SIZE 		512
extern OS_TCB WatcherTaskTCB;
extern CPU_STK WATCHER_TASK_STK[WATCHER_STK_SIZE];

extern void watcher_task(void *p_arg);
#endif
