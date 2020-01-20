#ifndef __LCD_H
#define __LCD_H		
#include "system.h"	 

//////////////////////////////////////////////////////////////////////////////////	 

//LCD驱动重要参数集
extern  u16  lcd_id;         //LCD ID
extern  u8   dir_flag;       //横屏还是竖屏控制：0，竖屏；1，横屏。	
extern  u16  lcd_width;      //LCD 宽度
extern  u16  lcd_height;     //LCD 高度
extern  u16	 write_gramcmd;	 //写gram指令
extern	u16  setxcmd;		     //设置x坐标指令
extern	u16  setycmd;		     //设置y坐标指令	 

//LCD的画笔颜色和背景色	   
extern u16  BRUSH_COLOR;//默认红色    
extern u16  BACK_COLOR; //背景颜色.默认为白色


//////////////////////////////////////////////////////////////////////////////////	 
//-----------------LCD背光端口定义---------------- 
#define	LCD_BACK PFout(10)  //LCD背光    	PF10 	    


//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 
//A12作为数据命令区分线 
//注意设置时STM32内部会右移一位对其		    
#define  CMD_BASE        ((u32)(0x6C000000 | 0x00001FFE))
#define  DATA_BASE       ((u32)(0x6C000000 | 0x00002000))

#define LCD_CMD       ( * (u16 *) CMD_BASE )
#define LCD_DATA      ( * (u16 *) DATA_BASE)

//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

#define INIT_SCAN_DIR  L2R_U2D   //设置初始化扫描方向

//颜色值定义
#define WHITE        0xFFFF
#define BLACK        0x0000	  
#define BLUE         0x001F 
#define GREEN        0x07E0
#define BRED         0XF81F
#define GRED 			   0XFFE0
#define GBLUE			   0X07FF
#define BROWN 			 0XBC40  
#define BRRED 			 0XFC07  
#define GRAY  			 0X8430  
#define RED          0xF800
#define MAGENTA      0xF81F
#define CYAN         0x7FFF
#define YELLOW       0xFFE0

extern void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value);
extern u16 LCD_ReadReg(u16 LCD_Reg);
extern void LCD_WriteRAM_Prepare(void);

extern void LCD_Init(void);													   	//初始化
extern void LCD_DisplayOn(void);													//开显示
extern void LCD_DisplayOff(void);												//关显示
extern void LCD_Clear(u16 Color);	 											//清屏
extern void LCD_SetCursor(u16 Xpos, u16 Ypos);						//设置光标
extern void LCD_DrawPoint(u16 x,u16 y);									//画点
extern void LCD_Color_DrawPoint(u16 x,u16 y,u16 color);	//颜色画点
extern u16  LCD_GetPoint(u16 x,u16 y); 								  //读点 

extern void LCD_AUTOScan_Dir(u8 dir);					 
extern void LCD_Display_Dir(u8 dir);						 
extern void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height); 	

extern void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		//填充单个颜色
extern void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size);						      //显示一个字符
extern void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//显示 数字
extern void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 brushcolor,u16 backcolor); //显示自定义数字
extern void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p);		           //显示一个12/16/24字体字符串
extern void LCD_DisplayString_color(u16 x,u16 y,u8 size,u8 *p,u16 brushcolor,u16 backcolor); //显示一个12/16/24字体自定义颜色的字符串

extern void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
extern void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn);
extern void LCD_ShowFontHZ24(u16 x,u16 y,u8 *cn);
extern void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic);


#endif  





