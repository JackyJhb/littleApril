#ifndef CIRCLE_BUFFER_H
#define CIRCLE_BUFFER_H

#define CIRCLE_BUFFER_SIZE  2048
#define SINGLE_LOG_STR_LEN  512

typedef struct
{
    unsigned char  *buffer_ptr;
    unsigned short buffer_size;
    unsigned short rd_index;
    unsigned short wr_index;
}CircleBufferStruct;

extern int logPrintf(const char *format,...);
extern unsigned short circleBufferRead(unsigned char *pdest,unsigned short rd_len);
#endif
