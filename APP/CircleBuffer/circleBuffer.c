#include "circleBuffer.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

static char circleBufferArray[CIRCLE_BUFFER_SIZE];
static volatile CircleBufferStruct rb;
static volatile char initFlag = 0;
char singleLogStrBuf[SINGLE_LOG_STR_LEN];

int logPrintf(const char *format,...);
void circleBufferInit(void);
unsigned short circleBufferCount(void);
unsigned short circleBufferWrite(unsigned char *psrc,unsigned short wr_len);
unsigned short circleBufferRead(unsigned char *pdest,unsigned short rd_len);

int logPrintf(const char *format,...)
{
  /*va_list args;
  va_start(args,format);
  vsprintf(singleLogStrBuf,format,args);
	va_end(args);
  circleBufferWrite(singleLogStrBuf,strlen(singleLogStrBuf));*/
}

unsigned short circleBufferCount(void)
{
	unsigned short data_len;
  if (initFlag == 0x00)
  {
    circleBufferInit();
    initFlag = 1;
    return 0;
  }
  if(rb.wr_index >= rb.rd_index)
  {
		data_len = rb.wr_index - rb.rd_index;
  }
  else
  {
		data_len = ((rb.wr_index + rb.buffer_size) - rb.rd_index);
  }
	return data_len;
}

void circleBufferInit(void)
{
    /* initialize read and write index */
    rb.rd_index = 0;
    rb.wr_index = 0;
    /* set buffer pool and size */
    rb.buffer_ptr = circleBufferArray;
    rb.buffer_size = sizeof(circleBufferArray);
}

unsigned short circleBufferWrite(unsigned char *psrc,unsigned short wr_len)
{
    unsigned short free_count = 0;
    signed short over_flow_cnt = 0;
    if (initFlag == 0)
    {
      circleBufferInit();
      initFlag = 1;
    }

    free_count = rb.buffer_size - circleBufferCount();
    if(free_count > 0)
    {
        if(wr_len > free_count)
        {
          wr_len = free_count;
        }
        over_flow_cnt = rb.wr_index + wr_len - rb.buffer_size;
        if(over_flow_cnt <= 0)
        {
          memcpy((unsigned char *)&rb.buffer_ptr[rb.wr_index],psrc,wr_len);
          rb.wr_index += wr_len;
        }
        else
        {
          memcpy(rb.buffer_ptr + rb.wr_index ,psrc,wr_len - over_flow_cnt);
          memcpy(rb.buffer_ptr + rb.wr_index + (wr_len - over_flow_cnt) ,psrc,over_flow_cnt);
          rb.wr_index = over_flow_cnt;
        }
    }
    else
    {
        wr_len = 0;
    }
    return wr_len;
}

unsigned short circleBufferRead(unsigned char *pdest,unsigned short rd_len)
{
    unsigned short valid_count = 0;
    signed short over_flow_cnt = 0;

    if (initFlag == 0)
    {
      circleBufferInit();
      initFlag = 1;
      return 0;
    }
    valid_count = circleBufferCount();
    if(valid_count > 0)
    {
        if(rd_len > valid_count)
        {
          rd_len = valid_count;
        }
        over_flow_cnt = rb.rd_index + rd_len - rb.buffer_size;
        if(over_flow_cnt <= 0)
        {
          memcpy(pdest,(unsigned char *)&rb.buffer_ptr[rb.rd_index],rd_len);
          rb.rd_index += rd_len;
        }
        else
        {
          memcpy(pdest,(unsigned char *)&rb.buffer_ptr[rb.rd_index],rd_len - over_flow_cnt);
          memcpy(pdest + rd_len - over_flow_cnt,rb.buffer_ptr,over_flow_cnt);
          rb.rd_index = over_flow_cnt;
        }
    }
    else
    {
        rd_len = 0;
        pdest[0] = 0;
    }
    return rd_len;
}