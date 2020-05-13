#ifndef CIRCLE_BUFFER_H
#define CIRCLE_BUFFER_H

typedef struct
{
    unsigned char  *buffer_ptr;
    unsigned short buffer_size;
    unsigned short rd_index;
    unsigned short wr_index;
}CircleBufferStruct;

extern void circleBufferInit(CircleBufferStruct *rb,unsigned char *pool,unsigned short size);
extern unsigned short circleBufferWrite(CircleBufferStruct *rb,unsigned char *pdest,unsigned short rd_len);
extern unsigned short circleBufferRead(CircleBufferStruct *rb,unsigned char *pdest,unsigned short rd_len);
#endif
