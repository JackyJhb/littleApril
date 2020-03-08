#include "usart.h"	
#include "stdlib.h"
#include "debug_config.h"
#include "sccf.h"

//接收缓存区 	
//接收缓存区 	
uint8_t RS485_receive_str[128];   //接收缓冲,最大128个字节.
uint8_t uart_byte_count=0,usart_timer;        //接收到的数据长度


int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}


//初始化IO 串口1 
//bound:波特率
void USART1_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10 ; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	char *   p_mem_blk,dat;
	OSIntEnter(); 	     //进入中断
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		dat = USART_ReceiveData(USART1);
	} 
	OSIntExit();	        //退出中断
} 	
										 
//初始化IO 串口2   bound:波特率	
void RS485_Init(u32 bound)
{  	 
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);		//使能USART2时钟

	//串口2引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);		//GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);		//GPIOA3复用为USART2
	
	//USART2    
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;	//GPIOA2与GPIOA3
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;				//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);						//初始化PA2，PA3
	
	//PG8推挽输出，485模式控制  
	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_8;					//GPIO8(普中)GPIOG6(极智)
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				//上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure);						//初始化PG8
	
	RS485_TX_EN=0;												//初始化默认为接收模式	
	
	//USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;					//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); 					//初始化串口2
	
	USART_Cmd(USART2, ENABLE);  								//使能串口 2	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//开启接受中断
	
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=13;		//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								//根据指定的参数初始化VIC寄存器、
}

//串口2接收中断服务函数
void USART2_IRQHandler(void)
{
	OS_ERR   err;
	char *   p_mem_blk,dat;
	
	OSIntEnter(); 	     //进入中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		usart_timer = 0x00;
		dat = (uint8_t)USART_ReceiveData(USART2);
		RS485_receive_str[uart_byte_count] = dat;
		++uart_byte_count;
		if (uart_byte_count > 100)
		{
			uart_byte_count = 0;
		}
	} 
	OSIntExit();	        //退出中断
} 


/****************************************************************************
* 名    称: void RS485_Send_Data(u8 *buf,u8 len)
* 功    能：RS485发送len个字节
* 入口参数：buf:发送区首地址
            len:发送的字节数 
* 返回参数：无
* 说    明：(为了和本代码的接收匹配,这里建议数据长度不要超过512个字节)       
****************************************************************************/	
void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t;
	if (len == 0x00)
	{
		return ;
	}
	RS485_TX_EN=1;			    //设置为发送模式
  	for(t=0;t<len;t++)		//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //等待发送结束		
    USART_SendData(USART2,buf[t]); //发送数据
	}	 
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);   //等待发送结束		
	uart_byte_count=0;	  
	RS485_TX_EN=0;				//发送完设置为接收模式	
}
