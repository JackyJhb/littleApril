#include "circleBuffer.h"
#include "debug_config.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "wifi_task.h"
#include "includes.h"

#ifdef ENABLE_WIFI_LOG
OS_MEM logMalloc;
uint8_t logArray[LOG_BUFFER_HEIGHT][LOG_BUFFER_WIDTH];
char loggerBufferArray[CIRCLE_BUFFER_SIZE];
LoggerBufferStruct rb;
static char initFlag = 0;
#else
char bufPrintf[512];
#endif

LogLevel logLevel = Debug;    

void loggerBufferInit(void);
int logPrintf(LogLevel level,const char *format,...);
uint16_t receiveLogFromQPend(void);
uint16_t loggerBufferRead(uint8_t *pdest,uint16_t rd_len);
uint16_t loggerBufferCount(void);

void loggerBufferInit(void)
{
  #ifdef ENABLE_WIFI_LOG
	OS_ERR err;
  /* initialize read and write index */
  rb.rd_index = 0;
  rb.wr_index = 0;
  /* set buffer pool and size */
  rb.buffer_ptr = loggerBufferArray;
  rb.buffer_size = sizeof(loggerBufferArray);

  OSMemCreate ((OS_MEM      *)&logMalloc,
				       (CPU_CHAR    *)"logMalloc",
				       (void        *)logArray,
				       (OS_MEM_QTY   )LOG_BUFFER_HEIGHT,
				       (OS_MEM_SIZE  )LOG_BUFFER_WIDTH,
				       (OS_ERR      *)&err);
  #endif
}

void setLogLevel(LogLevel level)
{
  #ifdef ENABLE_OUTPUT_LOG
  logLevel = level;
  #endif
}

int logPrintf(LogLevel level,const char *format,...)
{
  char *p_mem_blk;
  va_list args;
	#ifndef ENABLE_OUTPUT_LOG
	return 0;
	#endif
	OS_ERR          err;
  #ifdef ENABLE_WIFI_LOG
  uint16_t len;
  if (level > logLevel)
    return 0;
  p_mem_blk = OSMemGet((OS_MEM *)&logMalloc,(OS_ERR *)&err);  //???????? mem ??????????
	if (err != OS_ERR_NONE)
	{
		return 0;
  }
  va_start(args,format);
	vsprintf(p_mem_blk,format,args);
	va_end(args);
  len = strlen(p_mem_blk);
  OSTaskQPost((OS_TCB      *)&WIFITaskTCB,
							(void        *)p_mem_blk,
							(OS_MSG_SIZE  )len,
							(OS_OPT       )OS_OPT_POST_FIFO,
							(OS_ERR      *)&err);
  return len;
  #else
  if (level > logLevel)
    return 0;
  va_start(args,format);
	vsprintf(bufPrintf,format,args);
	va_end(args);
	printf(bufPrintf);
	return 0;
	#endif
}

uint16_t loggerBufferCount(void)
{
  #ifdef ENABLE_WIFI_LOG
	unsigned short data_len;
  if(rb.wr_index >= rb.rd_index)
  {
		data_len = rb.wr_index - rb.rd_index;
  }
  else
  {
		data_len = ((rb.wr_index + rb.buffer_size) - rb.rd_index);
  }
	return data_len;
  #else
  return 0;
  #endif
}

uint16_t receiveLogFromQPend(void)
{
  #ifdef ENABLE_WIFI_LOG
  OS_ERR          err;
  uint16_t free_count = 0,msg_len;
  int16_t over_flow_cnt = 0;
  char *pMsg;

  pMsg = OSTaskQPend ((OS_TICK        )0,
							(OS_OPT         )OS_OPT_PEND_BLOCKING,
							(OS_MSG_SIZE   *)&msg_len,
							(CPU_TS        *)0,
							(OS_ERR       *)&err);
  if ((pMsg == NULL) || err != OS_ERR_NONE)
    return 0;
  free_count = rb.buffer_size - loggerBufferCount();
  if(free_count > 0)
  {
    if(msg_len > free_count)
    {
      msg_len = free_count;
    }
    over_flow_cnt = rb.wr_index + msg_len - rb.buffer_size;
    if(over_flow_cnt <= 0)
    {
      memcpy((unsigned char *)&rb.buffer_ptr[rb.wr_index],pMsg,msg_len);
      rb.wr_index += msg_len;
    }
    else
    {
      memcpy(rb.buffer_ptr + rb.wr_index ,pMsg,msg_len - over_flow_cnt);
      //memcpy(rb.buffer_ptr + rb.wr_index + (msg_len - over_flow_cnt) ,pMsg,over_flow_cnt);
      memcpy(rb.buffer_ptr,pMsg+ (msg_len - over_flow_cnt),over_flow_cnt);
      rb.wr_index = over_flow_cnt;
    }
  }
  else
  {
    msg_len = 0;
  }
  OSMemPut ((OS_MEM  *)&logMalloc,
					(void    *)pMsg,
					(OS_ERR  *)&err);
  return msg_len;
  #else
  return 0;
  #endif
}

uint16_t loggerBufferRead(uint8_t *pdest,uint16_t rd_len)
{
  #ifdef ENABLE_WIFI_LOG
  uint16_t valid_count = 0;
  int16_t over_flow_cnt = 0;
  receiveLogFromQPend();
  valid_count = loggerBufferCount();
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
  #else
  return 0;
  #endif
}