#ifndef LOGGER_MANAGMENT_H
#define LOGGER_MANAGMENT_H

#include "includes.h"
#include "debug_config.h"

#define CIRCLE_BUFFER_SIZE  2048
#define LOG_BUFFER_HEIGHT   30
#define LOG_BUFFER_WIDTH    256

typedef struct
{
    unsigned char  *buffer_ptr;
    unsigned short buffer_size;
    unsigned short rd_index;
    unsigned short wr_index;
}LoggerBufferStruct;

#ifdef ENABLE_WIFI_LOG
extern OS_MEM logMalloc;
extern uint8_t logArray[LOG_BUFFER_HEIGHT][LOG_BUFFER_WIDTH];
#endif
extern void loggerBufferInit(void);
extern int logPrintf(const char *format,...);
extern uint16_t receiveLogFromQPend(void);
extern uint16_t loggerBufferRead(uint8_t *pdest,uint16_t rd_len);
extern uint16_t loggerBufferCount(void);
#endif
