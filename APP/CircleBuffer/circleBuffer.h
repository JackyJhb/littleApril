#ifndef LOGGER_MANAGMENT_H
#define LOGGER_MANAGMENT_H

#include "includes.h"
#include "debug_config.h"

#define CIRCLE_BUFFER_SIZE  2048
#define LOG_BUFFER_HEIGHT   30
#define LOG_BUFFER_WIDTH    256

typedef enum {
    Fatal   = 0x00,
    Error   = 0x01,
    Warning = 0x02,
    Debug   = 0x03,
    Info    = 0x04,
    Verbose = 0x05
}LogLevel;

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
extern void setLogLevel(LogLevel level);
extern int logPrintf(LogLevel level,const char *format,...);
extern uint16_t receiveLogFromQPend(void);
extern uint16_t loggerBufferRead(uint8_t *pdest,uint16_t rd_len);
extern uint16_t loggerBufferCount(void);
#endif
