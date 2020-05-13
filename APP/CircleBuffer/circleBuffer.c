#include "circleBuffer.h"
#include "string.h"

static int circleBufferCount(CircleBufferStruct *rb)
{
    if(rb->wr_index >= rb->rd_index)
    {
        return (rb->wr_index - rb->rd_index);
    }
    else
    {
        return ((rb->wr_index + rb->buffer_size) - rb->rd_index);
    }
}

void circleBufferInit(CircleBufferStruct *rb,unsigned char *pool,unsigned short size)
{
    /* initialize read and write index */
    rb->rd_index = 0;
    rb->wr_index = 0;
    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = size;
}

unsigned short circleBufferWrite(CircleBufferStruct *rb,unsigned char *psrc,unsigned short wr_len)
{
    unsigned short free_count = 0;
    signed short over_flow_cnt = 0;

    free_count = rb->buffer_size - circleBufferCount(rb);
    if(free_count > 0)
    {
        if(wr_len > free_count)
        {
          wr_len = free_count;
        }
        over_flow_cnt = rb->wr_index + wr_len - rb->buffer_size;
        if(over_flow_cnt <= 0)
        {
          memcpy((unsigned char *)&rb->buffer_ptr[rb->wr_index],psrc,wr_len);
          rb->wr_index += wr_len;
        }
        else
        {
          memcpy(rb->buffer_ptr + rb->wr_index ,psrc,wr_len - over_flow_cnt);
          memcpy(rb->buffer_ptr + rb->wr_index + (wr_len - over_flow_cnt) ,psrc,over_flow_cnt);
          rb->wr_index = over_flow_cnt;
        }
    }
    else
    {
        wr_len = 0;
    }
    return wr_len;
}

unsigned short circleBufferRead(CircleBufferStruct *rb,unsigned char *pdest,unsigned short rd_len)
{
    unsigned short valid_count = 0;
    signed short over_flow_cnt = 0;

    valid_count = circleBufferCount(rb);
    if(valid_count > 0)
    {
        if(rd_len > valid_count)
        {
          rd_len = valid_count;
        }
        over_flow_cnt = rb->rd_index + rd_len - rb->buffer_size;
        if(over_flow_cnt <= 0)
        {
          memcpy(pdest,(unsigned char *)&rb->buffer_ptr[rb->rd_index],rd_len);
          rb->rd_index += rd_len;
        }
        else
        {
          memcpy(pdest,(unsigned char *)&rb->buffer_ptr[rb->rd_index],rd_len - over_flow_cnt);
          memcpy(pdest + rd_len - over_flow_cnt,rb->buffer_ptr,over_flow_cnt);
          rb->rd_index = over_flow_cnt;
        }
    }
    else
    {
        rd_len = 0;
        pdest[0] = 0;
    }
     
    return rd_len;
}