/*
*********************************************************************************************************
*                                      
*    模块名称 : 串口驱动模块    
*    文件名称 : bsp_usart.c
*    版    本 : V2.0
*    说    明 : 实现printf和scanf函数重定向到串口1，即支持printf信息到USART1
*    	    	实现重定向，只需要添加2个函数:
*    	    	int fputc(int ch, FILE *f);
*    	    	int fgetc(FILE *f);
*    	    	对于KEIL MDK编译器，编译选项中需要在MicorLib前面打钩，否则不会有数据打印到USART1。
*
*********************************************************************************************************
*/
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "stm32f4xx_flash.h"
//#include "agv_get_rfid.h" 
#include "includes.h"					//ucos 使用	
#include "bsp_usart.h"
//#include "agv_path_magement.h"


UartStruct Uart1Stu;
UartStruct Uart2Stu;
UartStruct Uart3Stu;
UartStruct Uart4Stu;
UartStruct Uart5Stu;
UartStruct Uart6Stu;

DipperdeFramer RFID_ProcessBuff;
/*
*********************************************************************************************************
*    函 数 名: bsp_InitUart
*    功能说明: 初始化CPU的USART1串口硬件设备。未启用中断。
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
unsigned long GetBitFromByte(unsigned char bit_len,unsigned char byte_buf[],unsigned short *byte_offset,unsigned short *bit_offset)
{
	unsigned short byteOffset,bitOffset;
	unsigned long  data,tempdata;

	byteOffset = *byte_offset;
	bitOffset  = *bit_offset;
	
	data = 0;

	if((bit_len > 32) || (bitOffset == 0) || (bitOffset > 8))
		return 0;
	
	while(bit_len > 0)
	{
		if(bit_len <= bitOffset)
		{
			tempdata  = (byte_buf[byteOffset] >> (bitOffset - bit_len));
			tempdata &= ((0x01 << bit_len) - 1);
			bitOffset -= bit_len;
			data += tempdata;
			bit_len = 0;
		}
		else
		{
			tempdata = byte_buf[byteOffset] & ((0x01 << bitOffset) - 1);
			tempdata <<= (bit_len - bitOffset);
			bit_len -= bitOffset;
			byteOffset++;
			bitOffset = 8;
			data += tempdata;
		}
	}

	if(bitOffset == 0)
	{
		byteOffset++;
		bitOffset = 8;
	}
	*byte_offset = byteOffset;
	*bit_offset  = bitOffset;
	return data;
}

void Uart1_Init()
{
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE); //使能GPIOF时钟
    GPIO_ResetBits(GPIOF, GPIO_Pin_5);//RS485方向选择
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

	//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、    
    
    //USART1 初始化设置
	USART_InitStructure.USART_BaudRate   = 115200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity     = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 

//    USART_ClearFlag(USART1, USART_IT_TXE);
//	USART_ClearFlag(USART1, USART_FLAG_TC);

    Uart1Stu.RxBuf_In = 0;
    Uart1Stu.RxBuf_Out = 0;
    Uart1Stu.TxBuf_In = 0;
    Uart1Stu.TxBuf_Out = 0;
    
    USART_Cmd(USART1, ENABLE);  //使能串口1
}

void Uart2_Init()
{
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE); //使能GPIOF时钟
    GPIO_ResetBits(GPIOF, GPIO_Pin_4);//RS485方向选择
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOF, &GPIO_InitStructure);    
    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//使能USART2时钟
 
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2); //GPIOD5复用为USART2
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2); //GPIOD6复用为USART2
    	
	//USART2端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure);
    
	//Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、    
    
    //USART2 初始化设置
	USART_InitStructure.USART_BaudRate   = 19200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity     = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 

    USART_ClearFlag(USART2, USART_IT_TXE);
	USART_ClearFlag(USART2, USART_FLAG_TC);

    Uart2Stu.RxBuf_In = 0;
    Uart2Stu.RxBuf_Out = 0;
    Uart2Stu.TxBuf_In = 0;
    Uart2Stu.TxBuf_Out = 0;
    
    USART_Cmd(USART2, ENABLE);  //使能串口2
}

#if 0
void Uart3_Init()
{
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE); //使能GPIOF时钟
    GPIO_ResetBits(GPIOF, GPIO_Pin_3);//RS485方向选择
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOF, &GPIO_InitStructure);    
    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//使能USART3时钟
 
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3); //GPIOD8复用为USART3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3); //GPIOD9复用为USART3
    	
	//USART3端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure);
    
	//Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、    
    
    //USART3 初始化设置
	USART_InitStructure.USART_BaudRate   = 115200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity     = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 

    USART_ClearFlag(USART3, USART_IT_TXE);
	USART_ClearFlag(USART3, USART_FLAG_TC);

    Uart3Stu.RxBuf_In = 0;
    Uart3Stu.RxBuf_Out = 0;
    Uart3Stu.TxBuf_In = 0;
    Uart3Stu.TxBuf_Out = 0;
    
    USART_Cmd(USART3, ENABLE);  //使能串口3
}
#endif

void Uart6_Init()
{
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //使能GPIOG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART6时钟
 
	//串口6对应引脚复用映射
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOG9复用为USART6
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOG14复用为USART6
	
	//USART6端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //GPIOG9与GPIOG14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PG9，PG14

	//Usart6 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、    
    
    //USART6 初始化设置
	USART_InitStructure.USART_BaudRate   = 115200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity     = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART6, &USART_InitStructure); //初始化串口6
	
    USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART6, USART_IT_TXE, ENABLE); 

    USART_ClearFlag(USART6, USART_IT_TXE);
	USART_ClearFlag(USART6, USART_FLAG_TC);

    Uart6Stu.RxBuf_In = 0;
    Uart6Stu.RxBuf_Out = 0;
    Uart6Stu.TxBuf_In = 0;
    Uart6Stu.TxBuf_Out = 0;
    
    USART_Cmd(USART6, ENABLE);  //使能串口1
}

void bsp_InitUart(void)
{
    Uart1_Init();
    Uart2_Init();
 //   Uart3_Init();
    Uart6_Init();
}


void SetZigbee_BaudRate_38400()
{
    USART_InitTypeDef USART_InitStructure;
    USART_Cmd(UART4, DISABLE);
    USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
    USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
    USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    USART_Init(UART4,  &USART_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
    USART_Cmd(UART4, ENABLE);
}
/****************************************************************************
* 函数名: USART1_IRQHandler
* 功  能: USART1中断服务程序.
* 输  入: 无
* 输  出: 无
* 返  回: 无
*/
unsigned char k = 0;
uint8_t TestDispatchOrder = 255;
void USART1_IRQHandler(void)
{
//    unsigned int i;    
    unsigned char ch = 0;
	  //OS_ERR err;
    
    /*接收中断*/
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
    	/* Read one byte from the receive data register */
        ch = USART_ReceiveData(USART1);
		
		Uart1Stu.RxBuf[k] = ch;
		k++;
		if(Uart1Stu.RxBuf[k] == '$')
		{
			k = 0;
			memset(Uart1Stu.RxBuf,0,k+1);
		}
		
		TestDispatchOrder = ch;  
    }
			USART_ClearFlag(USART1,USART_FLAG_RXNE);
	
	
}
//void USART1_IRQHandler(void)
//{
//    unsigned int i;    
//    unsigned char ch;
//    
//    /*接收中断*/
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    { 
//    	/* Read one byte from the receive data register */
//        ch = USART_ReceiveData(USART1);
//    	if(((Uart1Stu.RxBuf_In + 1) & UART_BUF_LEN) == Uart1Stu.RxBuf_Out)
//    	{	
//            Uart1Stu.RxBuf_In = 0;
//			
//    	}
//    	else
//    	{

//            Uart1Stu.RxBuf[(Uart1Stu.RxBuf_In)] = ch;
//    	    Uart1Stu.RxBuf_In = (Uart1Stu.RxBuf_In + 1) & UART_BUF_LEN;
//    	}    	
//    }	    	    
//    /* 发送缓冲区空中断 */
//    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
//    { 
//        if(Uart1Stu.TxBuf_In != Uart1Stu.TxBuf_Out)
//        {
//            i = Uart1Stu.TxBuf_Out;
//            Uart1Stu.TxBuf_Out = (Uart1Stu.TxBuf_Out + 1) & UART_BUF_LEN;
//            ch = Uart1Stu.TxBuf[i];
//			SetRS485WriteCOM1();
//            USART_SendData(USART1,ch);
//        } 
//        else
//    	{
//    	    /* 禁止发送缓冲区空中断，使能发送完毕中断 */
//    	    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
//    	    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
//    	}            
//    }
//    /* 1个字节发送完毕的中断 */
//    else if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
//    {
//        if(Uart1Stu.TxBuf_In != Uart1Stu.TxBuf_Out)
//        {
//            i = Uart1Stu.TxBuf_Out;
//            Uart1Stu.TxBuf_Out = (Uart1Stu.TxBuf_Out + 1) & UART_BUF_LEN;
//            ch = Uart1Stu.TxBuf[i];
//			SetRS485WriteCOM1();
//            USART_SendData(USART1,ch);
//        } 
//        else
//    	{
//    	    /* 禁止发送缓冲区空中断，使能发送完毕中断 */
//    	    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
//			SetRS485ReadCOM1();
//    	}    
//    }     
//}


/****************************************************************************
* 函数名: USART2_IRQHandler
* 功  能: USART2中断服务程序.
* 输  入: 无
* 输  出: 无
* 返  回: 无
*/
void USART2_IRQHandler(void)
{
    unsigned int i;    
    unsigned char ch;
    
    /*接收中断*/
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    { 
    	/* Read one byte from the receive data register */
        ch = USART_ReceiveData(USART2);
    	if(((Uart2Stu.RxBuf_In + 1) & UART_BUF_LEN) == Uart2Stu.RxBuf_Out)
    	{
            
    	}
    	else
    	{
            Uart2Stu.RxBuf[(Uart2Stu.RxBuf_In)] = ch;
    	    Uart2Stu.RxBuf_In = (Uart2Stu.RxBuf_In + 1) & UART_BUF_LEN;
    	}    	
    }     	    	       
    /*发送缓冲区空中断 */
    if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
    { 
        if(Uart2Stu.TxBuf_In != Uart2Stu.TxBuf_Out)
        {
            i = Uart2Stu.TxBuf_Out;
            Uart2Stu.TxBuf_Out = (Uart2Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart2Stu.TxBuf[i];
//			SetRS485WriteCOM2();
            USART_SendData(USART2,ch);
        }
        else
    	{
    	    /* 禁止发送缓冲区空中断，使能发送完毕中断 */
    	    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    	    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    	}                 
    }
    /* 1个字节发送完毕的中断 */
    else if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
        if(Uart2Stu.TxBuf_In != Uart2Stu.TxBuf_Out)
        {
            i = Uart2Stu.TxBuf_Out;
            Uart2Stu.TxBuf_Out = (Uart2Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart2Stu.TxBuf[i];
//			SetRS485WriteCOM2();
            USART_SendData(USART2,ch);
        } 
        else
    	{
    	    /* 禁止发送缓冲区空中断，使能发送完毕中断 */
    	    USART_ITConfig(USART2, USART_IT_TC, DISABLE);
//			SetRS485ReadCOM2();
    	}    
    }       
}

/****************************************************************************
* 函数名: USART3_IRQHandler
* 功  能: USART3中断服务程序.
* 输  入: 无
* 输  出: 无
* 返  回: 无
*/

#if 0
void USART3_IRQHandler(void)
{
	unsigned int i;	
    unsigned char ch;
    /*接收中断*/
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{ 
		/* Read one byte from the receive data register */
        ch = USART_ReceiveData(USART3);
		if(((Uart3Stu.RxBuf_In + 1) & UART_BUF_LEN) == Uart3Stu.RxBuf_Out)
		{
            
		}
		else
		{
	        Uart3Stu.RxBuf[(Uart3Stu.RxBuf_In)] = ch;
		    Uart3Stu.RxBuf_In = (Uart3Stu.RxBuf_In + 1) & UART_BUF_LEN;
		}		
	} 					
    
	/*发送缓冲区空中断 */
	if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{ 
        if(Uart3Stu.TxBuf_In != Uart3Stu.TxBuf_Out)
        {
            i = Uart3Stu.TxBuf_Out;
            Uart3Stu.TxBuf_Out = (Uart3Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart3Stu.TxBuf[i];
//			SetRS485WriteCOM3();
            USART_SendData(USART3,ch);
        }
        else
		{
			/* 禁止发送缓冲区空中断，使能发送完毕中断 */
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
			USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		}                 
	}
	/* 1个字节发送完毕的中断 */
	else if (USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
        if(Uart3Stu.TxBuf_In != Uart3Stu.TxBuf_Out)
        {
            i = Uart3Stu.TxBuf_Out;
            Uart3Stu.TxBuf_Out = (Uart3Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart3Stu.TxBuf[i];
//			SetRS485WriteCOM1();
            USART_SendData(USART3,ch);
        } 
        else
		{
			/* 禁止发送缓冲区空中断，使能发送完毕中断 */
			USART_ITConfig(USART3, USART_IT_TC, DISABLE);
//			SetRS485ReadCOM3();
		}	
	}	   
}
#endif
/****************************************************************************
* 函数名: USART4_IRQHandler
* 功  能: USART4中断服务程序.
* 输  入: 无
* 输  出: 无
* 返  回: 无
*/
void UART4_IRQHandler(void)
{
	unsigned int i;	
    unsigned char ch;
    
    /*接收中断*/
    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{ 
		/* Read one byte from the receive data register */
        ch = USART_ReceiveData(UART4);
		if(((Uart4Stu.RxBuf_In + 1) & UART_BUF_LEN) == Uart4Stu.RxBuf_Out)
		{
            
		}
		else
		{
	        Uart4Stu.RxBuf[(Uart4Stu.RxBuf_In)] = ch;
		    Uart4Stu.RxBuf_In = (Uart4Stu.RxBuf_In + 1) & UART_BUF_LEN;
		}		
	} 					
    
	/*发送缓冲区空中断 */    
	if (USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
	{ 
        if(Uart4Stu.TxBuf_In != Uart4Stu.TxBuf_Out)
        {
            i = Uart4Stu.TxBuf_Out;
            Uart4Stu.TxBuf_Out = (Uart4Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart4Stu.TxBuf[i];
            USART_SendData(UART4,ch);
            
        }
        else
		{
			/* 禁止发送缓冲区空中断，使能发送完毕中断 */
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
			USART_ITConfig(UART4, USART_IT_TC, ENABLE);
		}                 
	}
	/* 1个字节发送完毕的中断 */
	else if (USART_GetITStatus(UART4, USART_IT_TC) != RESET)
	{
        if(Uart4Stu.TxBuf_In != Uart4Stu.TxBuf_Out)
        {
            i = Uart4Stu.TxBuf_Out;
            Uart4Stu.TxBuf_Out = (Uart4Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart4Stu.TxBuf[i];
            USART_SendData(UART4,ch);
        } 
        else
		{
			/* 禁止发送缓冲区空中断，使能发送完毕中断 */
			USART_ITConfig(UART4, USART_IT_TC, DISABLE);
		}	
	}	   
}

/****************************************************************************
* 函数名: USART5_IRQHandler
* 功  能: USART5中断服务程序.
* 输  入: 无
* 输  出: 无
* 返  回: 无
*/
void UART5_IRQHandler(void)
{
	unsigned int i;	
    unsigned char ch;
    
    /*接收中断*/
    if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{ 
		/* Read one byte from the receive data register */
        ch = USART_ReceiveData(UART5);
		if(((Uart5Stu.RxBuf_In + 1) & UART_BUF_LEN) == Uart5Stu.RxBuf_Out)
		{
            
		}
		else
		{
	        Uart5Stu.RxBuf[(Uart5Stu.RxBuf_In)] = ch;
		    Uart5Stu.RxBuf_In = (Uart5Stu.RxBuf_In + 1) & UART_BUF_LEN;
		}		
	} 					
    
	/*发送缓冲区空中断 */    
	if (USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{ 
        if(Uart5Stu.TxBuf_In != Uart5Stu.TxBuf_Out)
        {
            i = Uart5Stu.TxBuf_Out;
            Uart5Stu.TxBuf_Out = (Uart5Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart5Stu.TxBuf[i];
            USART_SendData(UART5,ch);
            
        }
        else
		{
			/* 禁止发送缓冲区空中断，使能发送完毕中断 */
			USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
			USART_ITConfig(UART5, USART_IT_TC, ENABLE);
		}                 
	}
	/* 1个字节发送完毕的中断 */
	else if (USART_GetITStatus(UART5, USART_IT_TC) != RESET)
	{
        if(Uart5Stu.TxBuf_In != Uart5Stu.TxBuf_Out)
        {
            i = Uart5Stu.TxBuf_Out;
            Uart5Stu.TxBuf_Out = (Uart5Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart5Stu.TxBuf[i];
            USART_SendData(UART5,ch);
        } 
        else
		{
			/* 禁止发送缓冲区空中断，使能发送完毕中断 */
			USART_ITConfig(UART5, USART_IT_TC, DISABLE);
		}	
	}	   
}

/****************************************************************************
* 函数名: USART6_IRQHandler
* 功  能: USART6中断服务程序.
* 输  入: 无
* 输  出: 无
* 返  回: 无
*/
void USART6_IRQHandler(void)
{
    unsigned int i;    
    unsigned char ch;
    
    /*接收中断*/
    if (USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    { 
    	/* Read one byte from the receive data register */
        ch = USART_ReceiveData(USART6);
    	if(((Uart6Stu.RxBuf_In + 1) & UART_BUF_LEN) == Uart6Stu.RxBuf_Out)
    	{
            
    	}
    	else
    	{
            Uart6Stu.RxBuf[(Uart6Stu.RxBuf_In)] = ch;
    	    Uart6Stu.RxBuf_In = (Uart6Stu.RxBuf_In + 1) & UART_BUF_LEN;
    	}    	
    }	    	    
    /* 发送缓冲区空中断 */
    if (USART_GetITStatus(USART6, USART_IT_TXE) != RESET)
    { 
        if(Uart6Stu.TxBuf_In != Uart6Stu.TxBuf_Out)
        {
            i = Uart6Stu.TxBuf_Out;
            Uart6Stu.TxBuf_Out = (Uart6Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart6Stu.TxBuf[i];
			//SetRS485WriteCOM1();
            USART_SendData(USART6,ch);
        } 
        else
    	{
    	    /* 禁止发送缓冲区空中断，使能发送完毕中断 */
    	    USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
    	    USART_ITConfig(USART6, USART_IT_TC, ENABLE);
    	}            
    }
    /* 1个字节发送完毕的中断 */
    else if (USART_GetITStatus(USART6, USART_IT_TC) != RESET)
    {
        if(Uart6Stu.TxBuf_In != Uart6Stu.TxBuf_Out)
        {
            i = Uart6Stu.TxBuf_Out;
            Uart6Stu.TxBuf_Out = (Uart6Stu.TxBuf_Out + 1) & UART_BUF_LEN;
            ch = Uart6Stu.TxBuf[i];
			//SetRS485WriteCOM1();
            USART_SendData(USART6,ch);
        } 
        else
    	{
    	    /* 禁止发送缓冲区空中断，使能发送完毕中断 */
    	    USART_ITConfig(USART6, USART_IT_TC, DISABLE);
			//SetRS485ReadCOM1();
    	}    
    }     
}

/****************************************************************************
* 函数名: ReadUart
* 功  能: 从指定的串口接收缓存区中取固定长度数据.
* 输  入: UartID-串口号，buff-数据存放位置， length-预取数据长度
* 输  出: 无
* 返  回: 实际获取的数据长度
*/
unsigned int ReadUart(unsigned char UartID,unsigned char *buff,unsigned int length)
{
    unsigned int i = 0;
    unsigned int count = 0;
    UartStruct *pUartStu;

    if(UartID == COM1)
        pUartStu = &Uart1Stu;
    else if(UartID == COM2)
        pUartStu = &Uart2Stu;
    else if(UartID == COM3)
        pUartStu = &Uart3Stu;
    else if(UartID == COM4)
        pUartStu = &Uart4Stu;
    else if(UartID == COM5)
        pUartStu = &Uart5Stu;    	
    else if(UartID == COM6)
        pUartStu = &Uart6Stu;    	
    else        
        return 0;

    for(i = 0; i < length; i++)
    {
    	if(pUartStu->RxBuf_In == pUartStu->RxBuf_Out)
    	    break;
        count += 1;
        buff[i] = pUartStu->RxBuf[(pUartStu->RxBuf_Out)];
    	pUartStu->RxBuf_Out = (pUartStu->RxBuf_Out + 1) & UART_BUF_LEN;    	
    }
    return count;
}

/****************************************************************************
* 函数名: WriteUart
* 功  能: 发送数据到指定的串口缓存区.
* 输  入: UartID-串口号，buff-需要发送的数据存放位置， length-需要发送的数据长度
* 输  出: 无
* 返  回: 无
*/
void WriteUart(unsigned char UartID,unsigned char *buff,unsigned int length)
{
    unsigned int i;    
    UartStruct *pUartStu;
    USART_TypeDef* USARTx;
    unsigned char write;

    if(UartID == COM1)
    {
        pUartStu = &Uart1Stu;
        USARTx = USART1;
    }
    else if(UartID == COM2)
    {
        pUartStu = &Uart2Stu;    	
        USARTx = USART2;
    }
    else if(UartID == COM3)
    {
        pUartStu = &Uart3Stu;    	
        USARTx = USART3;
    }
    else if(UartID == COM6)
    {
        pUartStu = &Uart4Stu;    	
        USARTx = USART6;
    }
    else if(UartID == COM4)
    {
        pUartStu = &Uart5Stu;    	
        USARTx = UART4;
    }
    else if(UartID == COM5)
    {
        pUartStu = &Uart6Stu;    	
        USARTx = UART5;
    }    
    else
        return; 

    if(pUartStu->TxBuf_In == pUartStu->TxBuf_Out)
    {
    	write = 1;
    }    
    else
    {
    	write = 0;
    }

    for(i = 0; i < length; i++)
    {
    	if(((pUartStu->TxBuf_In + 1) & UART_BUF_LEN) != pUartStu->TxBuf_Out)
    	{
    	    pUartStu->TxBuf[(pUartStu->TxBuf_In)] = buff[i];
    	    pUartStu->TxBuf_In = (pUartStu->TxBuf_In + 1) & UART_BUF_LEN;    	    
    	}
    	else
    	    break;
    }

    if(write)
    {
    	USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
    }               
}

void UartSend(unsigned char UartID,unsigned char *buff,unsigned int length)
{
    unsigned int i;
	unsigned int time;
    USART_TypeDef* USARTx;

    if(UartID == COM1)
        USARTx = USART1;
    else if(UartID == COM2)
        USARTx = USART2;
    else if(UartID == COM3)
        USARTx = USART3;
    else if(UartID == COM4)
        USARTx = UART4;
    else if(UartID == COM5)
        USARTx = UART5;
    else if(UartID == COM6)
        USARTx = USART6;    
    else
        return;
	
    for(i = 0; i < length; i++)
    {
        /* 写一个字节到USART1 */
        USART_SendData(USARTx, buff[i]);
    
        /* 等待发送结束 */
		time = 0;
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
        {
			time++;
			if(time > 100000)
			{
				break;
			}
		};
    }
}

//=================================================================================================================


////////////////////////////////////RFID读写,UART2/////////////////////////
//AA 09 09 68 00 00 01 1B 99
unsigned short int Get_CRC_16(unsigned char buf[],unsigned int len)
{
    unsigned short current_crc_value;
    int i, j;

    current_crc_value = 0xFFFF;

    for (i = 0; i < len; i++)
    {
        current_crc_value = current_crc_value ^ ((unsigned int)buf[i]);
        
    	for (j = 0; j < 8; j++)
        {
            if (current_crc_value & 0x0001)
            {
                current_crc_value = (current_crc_value >> 1) ^ 0x8408;
            }
            else
            {
                current_crc_value = (current_crc_value >> 1);
            }
        }
    }

    current_crc_value = ~current_crc_value;
    
    return current_crc_value;
}

//不考虑TAG地址
void RFID_WriteData(unsigned char cmd_code,unsigned cmd_index,unsigned char* userdata,unsigned char datalen)
{
    unsigned char sendBuf[256];
    unsigned short crc_value;
    unsigned char len;

    sendBuf[0] = 0xAA;    //Synchronization
    sendBuf[1] = 7 + datalen;
    sendBuf[2] = sendBuf[1];
    sendBuf[3] = cmd_code;
    sendBuf[4] = cmd_index;

    for(len = 0; len < datalen; len++)
    {
        sendBuf[5+len] = userdata[len];
    }
    crc_value = Get_CRC_16(sendBuf,5+datalen);

    sendBuf[5+datalen] = (unsigned char)crc_value; 
    sendBuf[6+datalen] = (unsigned char)(crc_value >> 8);
}
/****************************************************************************
* 函数名: RFID_DippFramer_TEK
* 功  能: 解析图尔克RFID读写器数据协议.
* 输  入: *pd-用户协议帧数据结构体参数，*pData-输入的协议数据参数
* 输  出: 无
* 返  回: 无
*/

void RFID_DippFramer_TEK(DipperdeFramer *pd,unsigned char *pData,unsigned short userDataLen)
{
    pd->oneFrame = 0;
    if((*pData == 0xAA) && (pd->dataNum == 0))
    {
        pd->frmBuf[0] = 0xAA;       
        pd->dataNum = 1;
    }
    else if((*pData >= 0x07) && (pd->dataNum == 1))
    {
        pd->frmBuf[1] = *pData;
        pd->dataNum = 2;
    }
    else if((*pData == pd->frmBuf[1]) && (pd->dataNum == 2))
    {
        pd->frmBuf[2] = *pData;
        pd->dataLeft  = *pData - 3;
        pd->dataNum = 3;
    }
    else if ((pd->dataNum >= 3) && (pd->dataNum <= 138))
    {
        pd->frmBuf[pd->dataNum] = *pData;
        pd->dataNum++;
        pd->dataLeft--;

        if(pd->dataLeft == 0)
        {
           pd->oneFrame = pd->dataNum;
           pd->dataNum = 0; 
        }                     
    }
    else
    {
       pd->oneFrame = 0;
       pd->dataNum = 0;
       pd->dataLeft = 0;
    }
}
/****************************************************************************
* 函数名: RFID_DippFramer_BJF
* 功  能: 解析倍加福RFID读写器数据协议.
* 输  入: *pd-用户协议帧数据结构体参数，*pData-输入的协议数据参数
* 输  出: 无
* 返  回: 无
*/
void RFID_DippFramer_BJF(DipperdeFramer *pd,unsigned char *pData,unsigned short userDataLen)
{
    static unsigned char SumCheck;

    pd->oneFrame = 0;
    if((*pData == 0x30) && (pd->dataNum == 0))
    {
        pd->frmBuf[0] = 0x30;
        pd->dataNum   = 1;
    	pd->dataLeft  = userDataLen+2;
    	SumCheck      = 0x30; 
    }
    else if(pd->dataLeft > 2)
    {
        SumCheck += *pData;
    	pd->frmBuf[pd->dataNum] = *pData;
    	pd->dataNum++;
        pd->dataLeft--;
    }
    else if(pd->dataLeft == 2)
    {    	
    	pd->frmBuf[pd->dataNum] = *pData;
    	pd->dataNum++;
        pd->dataLeft--;

    }
    else if((pd->dataLeft == 1) && (*pData == 0x03))
    {
        pd->frmBuf[pd->dataNum] = *pData;
        pd->dataNum++;
        pd->dataLeft--;
    	
        if((pd->dataLeft == 0) && (SumCheck == pd->frmBuf[pd->dataNum-2]))
        {
    	    pd->oneFrame = pd->dataNum;
    	    pd->dataNum = 0; 
        }                     
    }
    else
    {
       pd->oneFrame = 0;
       pd->dataNum = 0;
       pd->dataLeft = 0;
    }
}
//RFID数据处理
void RFID_ProcessUartCommand(unsigned char *pData)
{
  /*  unsigned short int MachineType;
	unsigned short int RFID_type;
	unsigned short int RFID_DataLen;

    //MachineType  = PLC_Get_D(D_MACHINE_TYPE);
    MachineType  = 24;//图尔克4个字节
	//MachineType  = 25;//倍加福4个字节
    RFID_type    = MachineType & 0x03;
	RFID_DataLen = ((MachineType >> 0x02 ) & 0x07) * 4;

	if(RFID_DataLen == 24)
	{
		RFID_DataLen = 4;
	}

    if(RFID_type == 0)
    {
    	RFID_DippFramer_TEK(&RFID_ProcessBuff,pData,RFID_DataLen);
    	if(RFID_ProcessBuff.oneFrame)
    	{
			RFIDValue = RFID_Process_TEK(RFID_ProcessBuff.frmBuf,RFID_ProcessBuff.oneFrame,RFID_DataLen);  
			printf("RFIDValue = %d\r\n",RFIDValue);			
    	    RFID_ProcessBuff.oneFrame = 0;    
    	}
    }
    else if(RFID_type == 1)
    {
    	RFID_DippFramer_BJF(&RFID_ProcessBuff,pData,RFID_DataLen);
    	if(RFID_ProcessBuff.oneFrame)
    	{ 
			RFIDValue = RFID_Process_BJF(RFID_ProcessBuff.frmBuf,RFID_ProcessBuff.oneFrame,RFID_DataLen);  	
    	    RFID_ProcessBuff.oneFrame = 0;    
    	}
    }*/
}

int RFID_Process_TEK(unsigned char *buff,unsigned short len,unsigned short userDataLen)
{
    unsigned char RFID_CmdData[1]={0x11};
	unsigned char avoidBuf[64];
    unsigned char type;
	unsigned char SumCheck;
	unsigned short byteOffset,bitOffset;
    unsigned short crc_value;
    unsigned short LandMarkID,UserData;
    unsigned char  datalen;
    unsigned short i;
    unsigned char  *pbuff;
    

    if(buff == 0 || len < 9)
        return 0;
    //RFID数据校验
    crc_value = Get_CRC_16(buff,len-2);
    if(crc_value != ((buff[len-1] << 8) | buff[len-2]))
        return 0;

    type = buff[3];
    switch(type)
    {
        case 0x00:
            break;

        case 0x48:
            break;

        case 0x49:
            break;

        case 0x60:
            break;

        case 0x61:
            break;

        case 0x62:
            break;

        case 0x67:
            break;

        case 0x68:

            datalen = buff[1];
            if(userDataLen + 7 != datalen)
                return 0;

			pbuff = buff + 5;

			if(userDataLen > 4)
			{
				//用户数据校验 
		        SumCheck = 0;
			    for(i = 0; i < userDataLen-1; i++)
			    {
				    SumCheck += pbuff[i];
			    }
				SumCheck &= 0x0f;
				byteOffset = userDataLen - 1;
				bitOffset  = 4;
			    if(GetBitFromByte(4,pbuff,&byteOffset,&bitOffset) != SumCheck)
				{
	    	    	//printf("rfid error\r\n");;
	    	    	//Uart_Printf(COM1,"$RFDI,%d,%d,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x*\r\n",Get_MachineID(),agvInfo.currentLandMarkID,SumCheck,buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11],buff[12],buff[13],buff[14],buff[15],buff[16],buff[17],buff[18],buff[19],buff[20],buff[21],buff[22],buff[23],buff[24],buff[25],buff[26],buff[27],buff[28],buff[29],buff[30],buff[31],buff[32],buff[33],buff[34],buff[35],buff[36]);   	    	
					return 0;
	    	    }    	 
			
				byteOffset = 0;
				bitOffset  = 8;
				UserData = GetBitFromByte(10,pbuff,&byteOffset,&bitOffset);	//获取用户数据
				LandMarkID = GetBitFromByte(11,pbuff,&byteOffset,&bitOffset);	//获取地标号
			}
			else
			{				
				byteOffset = 0;
				bitOffset  = 8;
				UserData = GetBitFromByte(10,pbuff,&byteOffset,&bitOffset);	//获取用户数据
				LandMarkID = GetBitFromByte(11,pbuff,&byteOffset,&bitOffset);	//获取地标号
			}
			break;

        case 0x69:
            break;

        case 0x70:
            break;

        case 0x71:
            break;

        case 0xAD:
            break;

        case 0xAF:
            break;

        case 0xBD:
            break;

        case 0xE0:
            break;

        case 0xE1:
            break;
    }
    return LandMarkID;    
}

int RFID_Process_BJF(unsigned char *buff,unsigned short len,unsigned short userDataLen)
{
	char BJF_12[] = {0x45,0x52,0x30,0x30,0x30,0x30,0x30,0x33,0xBA,0x03};//读取12个字节的命令
	char BJF_16[] = {0x45,0x52,0x30,0x30,0x30,0x30,0x30,0x34,0xBB,0x03};//读取16个字节的命令
	char BJF_20[] = {0x45,0x52,0x30,0x30,0x30,0x30,0x30,0x35,0xBC,0x03};//读取20个字节的命令

    unsigned char RFID_UserData[64];
//	unsigned char avoidBuf[64];
	unsigned char SumCheck;
	unsigned short byteOffset,bitOffset;
    unsigned short LandMarkID,UserData;
    unsigned short i;
    unsigned char  *pbuff;

    if(buff == 0 || len != userDataLen+3)
        return 0;
    //高低字节调换
    for(i = 0; i < userDataLen; i+=4)
    {
    	RFID_UserData[i+0] = buff[1+i+3];
    	RFID_UserData[i+1] = buff[1+i+2];
    	RFID_UserData[i+2] = buff[1+i+1];
    	RFID_UserData[i+3] = buff[1+i+0];
    }
	pbuff = RFID_UserData;
	if(userDataLen > 4)
	{			
		//用户数据校验 
		SumCheck = 0;
		for(i = 0; i < userDataLen-1; i++)
		{
		    SumCheck += RFID_UserData[i];
		}
		SumCheck &= 0x0f;
		byteOffset = userDataLen - 1;
		bitOffset  = 4;
		if(GetBitFromByte(4,RFID_UserData,&byteOffset,&bitOffset) != SumCheck)
		{
	        if(userDataLen == 12)
			{
				WriteUart(COM2,BJF_12,sizeof(BJF_12));
			}
			else if(userDataLen == 16)
			{
				WriteUart(COM2,BJF_16,sizeof(BJF_16));
			}
			else if(userDataLen == 20)
			{
				WriteUart(COM2,BJF_20,sizeof(BJF_20));
			}
	    	//Uart_Printf(COM1,"$RFDI,%d,%d,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x*\r\n",Get_MachineID(),agvInfo.currentLandMarkID,buff[5],buff[6],buff[7],buff[8],buff[9],buff[10],buff[11],buff[12],buff[13],buff[14],buff[15],buff[16],buff[17],buff[18],buff[19],buff[20],buff[21],buff[22],buff[23],buff[24],buff[25],buff[26],buff[27],buff[28],buff[29],buff[30],buff[31],buff[32],buff[33],buff[34],buff[35],buff[36]);
	    	return 0;
	    }                               
		byteOffset = 0;
		bitOffset  = 8;
		UserData = GetBitFromByte(10,pbuff,&byteOffset,&bitOffset);	//获取用户数据
		LandMarkID = GetBitFromByte(11,pbuff,&byteOffset,&bitOffset);	//获取地标号
    }
	else
	{
		byteOffset = 0;
		bitOffset  = 8;
		UserData = GetBitFromByte(10,pbuff,&byteOffset,&bitOffset);	//获取用户数据
		LandMarkID = GetBitFromByte(11,pbuff,&byteOffset,&bitOffset);	//获取地标号	
	}
    return LandMarkID;
}

void RFID_Read_LandMark_Info(unsigned char uart_id)
{
    // AA 09 09 68 01 00 07 F1 A6 //32个字节
    //unsigned char RFID_ReadCMD[] = {0xAA,0x09,0x09,0x68,0x00,0x00,0x0F,0x65,0x70};//主动读
    unsigned char RFID_ReadCMD[] = {0xAA,0x07,0x07,0xFF,0x00,0xFF,0x4D};    	  //复位
    
    WriteUart(uart_id,RFID_ReadCMD,sizeof(RFID_ReadCMD));    
}

//////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************************
** 函数名称:  Uart_Printf
** 函数功能:  串口接收数据
** 入口参数:  num 句柄 *buff 数据缓存
** 出口参数:  无
** 函数说明:
*********************************************************************************************************/
void Uart_Printf(unsigned char UartID, const char *fmt,...)
{
    va_list    ap;
    char string[1024];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    WriteUart(UartID,(unsigned char *)string,strlen(string));
    va_end(ap);
}


////////////////////modbus//////////////////////////////////
unsigned short GetCRC( unsigned char *dataPacket,unsigned int dataLenth )
{
    const unsigned char auchCRCHi[] = 
	{
    	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    	0x40
    } ;

    const char auchCRCLo[] = 
	{
    	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    	0x40
    };
    unsigned char CRCHi=0xFF;    	//CRC高位字节初始化
    unsigned char CRCLo=0xFF;    	//CRC低位字节初始化
    unsigned int  index=0;    	    //CRC查询表索引

    while (dataLenth--)
    {
    	index = CRCLo ^ *dataPacket++;/*计算CRC */
    	CRCLo = CRCHi ^ auchCRCHi[index];
    	CRCHi = auchCRCLo[index];
    }
    return (CRCHi << 8 | CRCLo) ;
}


//ConfigInfo ConfigInfo_1;
//ConfigInfo ConfigInfo_2;
void ProcessDataFormUart1()
{
	CPU_SR_ALLOC();
	u8 FrameHead[1];
//    unsigned int len;
	
	ReadUart(COM1,FrameHead,1);
    if(FrameHead[0] == ':')
    {
		unsigned char buff[16];
		ReadUart(COM1,buff,15);
//		if(len == 15)
//		{
//			ConfigInfo_1.Config.DriveMode = buff[0]-0x30;
//			ConfigInfo_1.Config.ControlMethod = buff[1]-0x30;
//			ConfigInfo_1.Config.DriveDirection = buff[2]-0x30;
//			ConfigInfo_1.Config.NavigationMode = buff[3]-0x30;
//			ConfigInfo_1.Config.CommunicationMode = buff[4]-0x30;
//			ConfigInfo_1.Config.RFID = buff[5]-0x30;
//			ConfigInfo_1.Config.LandmarkBytes = (buff[6]-0x30);
//			ConfigInfo_1.Config.WheelDiameter = ((buff[7]-0x30)*100) + ((buff[8]-0x30)*10) + ((buff[9]-0x30));
//			ConfigInfo_1.Config.WheelBase = ((buff[10]-0x30)*1000) + ((buff[11]-0x30)*100) + ((buff[12]-0x30)*10) + ((buff[13]-0x30));
//			ConfigInfo_1.Config.EncoderBit = buff[14]-0x30;
//			
//			OS_CRITICAL_ENTER();			//进入临界区
//			bsp_WriteCpuFlash(FLASH_SAVE_ADDR,ConfigInfo_1.Buffer,128);//写内存
//			bsp_ReadCpuFlash(FLASH_SAVE_ADDR,ConfigInfo_2.Buffer,128);//读内存
//			OS_CRITICAL_EXIT();	//退出临界区
//		}
    }
}

void ProcessDataFormUart2()
{
    unsigned char buff[64];
    unsigned int len;
    unsigned i;
    
    len = ReadUart(COM2,buff,64);
    if( len > 0)
    {
        for(i = 0; i < len; i++)
        {
            RFID_ProcessUartCommand(buff+i);         
        }
    }
}
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void  _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
