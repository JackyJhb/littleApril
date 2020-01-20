#include "stm32f4xx.h"
#include "lcd.h"
#include "font.h"
/*********************************************************************************
**********************************************************************************
* �ļ�����: lcd.c                                                                *
* �ļ�������LCD��ʾ��������                                                      *
* �������ڣ�2017.08.30                                                           *
* ��    ����V1.0                                                                 *
* ˵    ����                                                                     * 
**********************************************************************************
*********************************************************************************/	 

//��ʼ��LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 BRUSH_COLOR=BLACK;	//������ɫ
u16 BACK_COLOR=WHITE;  //����ɫ 

//����LCD������Ҫ����
u16  lcd_id;          //LCD ID
u8   dir_flag;        //�������������ƣ�0��������1������
u16  lcd_width;       //LCD�Ŀ��
u16  lcd_height;      //LCD�ĸ߶�
u16	 write_gramcmd;		//дgramָ��
u16  setxcmd;		      //����x����ָ��
u16  setycmd;		      //����y����ָ��

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);

void LCD_Init(void);													   	//��ʼ��
void LCD_DisplayOn(void);													//����ʾ
void LCD_DisplayOff(void);												//����ʾ
void LCD_Clear(u16 Color);	 											//����
void LCD_SetCursor(u16 Xpos, u16 Ypos);						//���ù��
void LCD_DrawPoint(u16 x,u16 y);									//����
void LCD_Color_DrawPoint(u16 x,u16 y,u16 color);	//��ɫ����
u16  LCD_GetPoint(u16 x,u16 y); 								  //���� 

void LCD_AUTOScan_Dir(u8 dir);					 
void LCD_Display_Dir(u8 dir);						 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height); 	

void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		//��䵥����ɫ
void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size);						      //��ʾһ���ַ�
void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����
void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 brushcolor,u16 backcolor); //��ʾ�Զ�������
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p);		           //��ʾһ��12/16/24�����ַ���
void LCD_DisplayString_color(u16 x,u16 y,u8 size,u8 *p,u16 brushcolor,u16 backcolor); //��ʾһ��12/16/24�����Զ�����ɫ���ַ���


void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn);
void LCD_ShowFontHZ24(u16 x,u16 y,u8 *cn);
void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic);

/****************************************************************************
* ��    ��: void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
* ��    �ܣ�LCDд�Ĵ���
* ��ڲ�����LCD_Reg: �Ĵ�����ַ
*           LCD_RegValue: Ҫд�������
* ���ز�������
* ˵    ����       
****************************************************************************/				   
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
{	
    LCD_CMD = LCD_Reg;		 //д��Ҫд�ļĴ������
    LCD_DATA = LCD_Value;  //��Ĵ���д�������
}
/****************************************************************************
* ��    ��: u16 LCD_ReadReg(u16 LCD_Reg)
* ��    �ܣ�LCD���Ĵ���
* ��ڲ�����LCD_Reg:�Ĵ�����ַ
* ���ز����������üĴ���������ֵ
* ˵    ����       
****************************************************************************/	
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
    LCD_CMD=LCD_Reg;		//д��Ҫ���ļĴ������
    delay_us(5);
    return LCD_DATA;		//���ض�����ֵ
}   
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_CMD=write_gramcmd;
}
//lcd��ʱ����
void lcdm_delay(u8 i)
{
    while(i--);
}
/****************************************************************************
* ��    ��: u16 LCD_BGRtoRGB(u16 bgr)
* ��    �ܣ�ͨ���ú�����GBRת��RGB
* ��ڲ�����bgr:GBR��ʽ����ɫֵ
* ���ز�����RGB��ʽ����ɫֵ
* ˵    ������ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ       
****************************************************************************/	
u16 LCD_BGRtoRGB(u16 bgr)
{
    u16  r,g,b,rgb;
    b=(bgr>>0)&0x1f;
    g=(bgr>>5)&0x3f;
    r=(bgr>>11)&0x1f;
    rgb=(b<<11)+(g<<5)+(r<<0);
    return(rgb);
} 
/****************************************************************************
* ��    ��: u16 LCD_GetPoint(u16 x,u16 y)
* ��    �ܣ���ȡĳ�����ɫֵ	
* ��ڲ�����x��x����
            y��y����
* ���ز������˵����ɫ
* ˵    ����     
****************************************************************************/
u16 LCD_GetPoint(u16 x,u16 y)
{
    vu16 r=0,g=0,b=0;

    if(x>=lcd_width||y>=lcd_height)
        return 0;	 //�����˷�Χ,ֱ�ӷ���
    LCD_SetCursor(x,y);
    if(lcd_id==0x9341)
        LCD_CMD=0X2E;   //9341 ���Ͷ�GRAMָ��
    else
        LCD_CMD=0x22;    //����IC���Ͷ�GRAMָ��
    if(LCD_DATA)
        r=0;
    lcdm_delay(2);
    r=LCD_DATA;  		  						 //ʵ��������ɫ

    if(lcd_id==0X9341)		//9341Ҫ��2�ζ���
    {
        lcdm_delay(2);
        b=LCD_DATA;
        g=r&0XFF;		 //����9341��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
        g<<=8;
    }

    if(lcd_id==0x9325)
        return r;
    else if(lcd_id==0x9341)
        return (((r>>11)<<11)|((g>>10)<<5)|(b>>11)); //ILI9341��Ҫ��ʽת��һ��
    else
        return LCD_BGRtoRGB(r);
}	

//LCD������ʾ
void LCD_DisplayOn(void)
{					   
    if(lcd_id==0x9341)
        LCD_CMD=0x29;
    else
        LCD_WriteReg(0x07,0x0173);  //������ʾ
}	

//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	
    if(lcd_id==0x9341)
        LCD_CMD=0x28;
    else
        LCD_WriteReg(0x07,0x0);    //�ر���ʾ
}   
/****************************************************************************
* ��    ��: void LCD_SetCursor(u16 Xaddr, u16 Yaddr)
* ��    �ܣ����ù��λ��
* ��ڲ�����x��x����
            y��y����
* ���ز�������
* ˵    ����     
****************************************************************************/
void LCD_SetCursor(u16 Xaddr, u16 Yaddr)
{	 
    if(lcd_id==0X9341)
    {
        LCD_CMD=setxcmd;
        LCD_DATA=(Xaddr>>8);
        LCD_DATA=(Xaddr&0XFF);
        LCD_CMD=setycmd;
        LCD_DATA=(Yaddr>>8);
        LCD_DATA=(Yaddr&0XFF);
    }
    else
    {
        if(dir_flag==1)Xaddr=lcd_width-1-Xaddr;//�������ǵ�תx,y����
        LCD_WriteReg(setxcmd, Xaddr);
        LCD_WriteReg(setycmd, Yaddr);
    }
} 
/****************************************************************************
* ��    ��: void LCD_AUTOScan_Dir(u8 dir)
* ��    �ܣ�����LCD���Զ�ɨ�跽��
* ��ڲ�����dir��ɨ�跽��
* ���ز�������
* ˵    ����     
****************************************************************************/  	   
void LCD_AUTOScan_Dir(u8 dir)
{
    u16 regval=0;
    u16 dirreg=0;
    u16 temp;

    if(dir_flag==1)//����ʱ
    {
        switch(dir)//����ת��
        {
        case 0:dir=6;break;
        case 1:dir=7;break;
        case 2:dir=4;break;
        case 3:dir=5;break;
        case 4:dir=1;break;
        case 5:dir=0;break;
        case 6:dir=3;break;
        case 7:dir=2;break;
        }
    }
    if(lcd_id==0x9341) //9341������
    {
        switch(dir)
        {
        case L2R_U2D://������,���ϵ���
            regval|=(0<<7)|(0<<6)|(0<<5);
            break;
        case L2R_D2U://������,���µ���
            regval|=(1<<7)|(0<<6)|(0<<5);
            break;
        case R2L_U2D://���ҵ���,���ϵ���
            regval|=(0<<7)|(1<<6)|(0<<5);
            break;
        case R2L_D2U://���ҵ���,���µ���
            regval|=(1<<7)|(1<<6)|(0<<5);
            break;
        case U2D_L2R://���ϵ���,������
            regval|=(0<<7)|(0<<6)|(1<<5);
            break;
        case U2D_R2L://���ϵ���,���ҵ���
            regval|=(0<<7)|(1<<6)|(1<<5);
            break;
        case D2U_L2R://���µ���,������
            regval|=(1<<7)|(0<<6)|(1<<5);
            break;
        case D2U_R2L://���µ���,���ҵ���
            regval|=(1<<7)|(1<<6)|(1<<5);
            break;
        }
        dirreg=0X36;
        regval|=0X08;
        LCD_WriteReg(dirreg,regval);

        if(regval&0X20)
        {
            if(lcd_width<lcd_height)//����X,Y
            {
                temp=lcd_width;
                lcd_width=lcd_height;
                lcd_height=temp;
            }
        }
        LCD_CMD=setxcmd;
        LCD_DATA=0;LCD_DATA=0;
        LCD_DATA=(lcd_width-1)>>8;LCD_DATA=(lcd_width-1)&0XFF;
        LCD_CMD=setycmd;
        LCD_DATA=0;LCD_DATA=0;
        LCD_DATA=(lcd_height-1)>>8;LCD_DATA=(lcd_height-1)&0XFF;
    }
    else
    {
        switch(dir)
        {
        case L2R_U2D://������,���ϵ���
            regval|=(1<<5)|(1<<4)|(0<<3);
            break;
        case L2R_D2U://������,���µ���
            regval|=(0<<5)|(1<<4)|(0<<3);
            break;
        case R2L_U2D://���ҵ���,���ϵ���
            regval|=(1<<5)|(0<<4)|(0<<3);
            break;
        case R2L_D2U://���ҵ���,���µ���
            regval|=(0<<5)|(0<<4)|(0<<3);
            break;
        case D2U_L2R://���µ���,������
            regval|=(0<<5)|(1<<4)|(1<<3);
            break;
        case D2U_R2L://���µ���,���ҵ���
            regval|=(0<<5)|(0<<4)|(1<<3);
            break;
        case U2D_L2R://���ϵ���,������
            regval|=(1<<5)|(1<<4)|(1<<3);
            break;
        case U2D_R2L://���ϵ���,���ҵ���
            regval|=(1<<5)|(0<<4)|(1<<3);
            break;
        }
        dirreg=0X03;
        regval|=1<<12;
        LCD_WriteReg(dirreg,regval);
    }
}
/****************************************************************************
* ��    ��: void LCD_Display_Dir(u8 dir)
* ��    �ܣ�����LCD��ʾ����
* ��ڲ�����dir: 0,����
                 1,����
* ���ز�������
* ˵    �����B
****************************************************************************/
void LCD_Display_Dir(u8 dir)
{
    if(dir==0)		  //����
    {
        dir_flag=0;
        lcd_width=240;
        lcd_height=320;
        if(lcd_id==0X9341)
        {
            write_gramcmd=0X2C;
            setxcmd=0X2A;
            setycmd=0X2B;
        }
        else
        {
            write_gramcmd=0x22;
            setxcmd=0x20;
            setycmd=0x21;
        }
    }
    else 				  //����
    {
        dir_flag=1;
        lcd_width=320;
        lcd_height=240;
        if(lcd_id==0X9341)
        {
            write_gramcmd=0X2C;
            setxcmd=0X2A;
            setycmd=0X2B;
        }
        else
        {
            write_gramcmd=0x22;
            setxcmd=0x21;
            setycmd=0x20;
        }
    }
    LCD_AUTOScan_Dir(INIT_SCAN_DIR);	//����ɨ�跽��
}	
/****************************************************************************
* ��    ��: void LCD_DrawPoint(u16 x,u16 y)
* ��    �ܣ����㣨�ڸõ�д�뻭�ʵ���ɫ��
* ��ڲ�����x��x����
            y��y����
* ���ز�������
* ˵    ���BRUSH_COLOR:�˵����ɫֵ
****************************************************************************/
void LCD_DrawPoint(u16 x,u16 y)
{
    LCD_SetCursor(x,y);		    //���ù��λ��
    LCD_WriteRAM_Prepare();	  //��ʼд��GRAM
    LCD_DATA=BRUSH_COLOR;
}
/****************************************************************************
* ��    ��: void LCD_Color_DrawPoint(u16 x,u16 y,u16 color)
* ��    �ܣ������õ����괦����Ӧ��ɫ���ڸõ�д���Զ�����ɫ��
* ��ڲ�����x��x����
            y��y����
            color �˵����ɫֵ
* ���ز�������
* ˵    ����color:д��˵����ɫֵ   UCGUI���øú���
****************************************************************************/
void LCD_Color_DrawPoint(u16 x,u16 y,u16 color)
{	       
    if(lcd_id==0X9341)
    {
        LCD_CMD=setxcmd;
        LCD_DATA=(x>>8);
        LCD_DATA=(x&0XFF);
        LCD_CMD=setycmd;
        LCD_DATA=(y>>8);
        LCD_DATA=(y&0XFF);
    }
    else
    {
        if(dir_flag==1)x=lcd_width-1-x;  //�������ǵ�תx,y����
        LCD_WriteReg(setxcmd,x);
        LCD_WriteReg(setycmd,y);
    }
    LCD_CMD=write_gramcmd;
    LCD_DATA=color;
}	 
/****************************************************************************
* ��    ��: void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
* ��    �ܣ����ô���,������û������굽�������Ͻ�(sx,sy)
* ��ڲ�����sx,sy:������ʼ����(���Ͻ�)
            width,height:���ڿ�Ⱥ͸߶�
* ���ز�������
* ˵    ���������С:width*height.�B
****************************************************************************/
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{   
    u8  h1reg,h2reg,v1reg,v2reg;
    u16 h1val,h2val,v1val,v2val;
    width=sx+width-1;
    height=sy+height-1;

    if(lcd_id==0X9341)
    {
        LCD_CMD=setxcmd;
        LCD_DATA=(sx>>8);
        LCD_DATA=(sx&0XFF);
        LCD_DATA=(width>>8);
        LCD_DATA=(width&0XFF);
        LCD_CMD=setycmd;
        LCD_DATA=(sy>>8);
        LCD_DATA=(sy&0XFF);
        LCD_DATA=(height>>8);
        LCD_DATA=(height&0XFF);
    }
    else
    {
        if(dir_flag==1)  //����
        {
            h1val=sy;
            h2val=height;
            v1val=lcd_width-width-1;
            v2val=lcd_width-sx-1;
        }else           //����
        {
            h1val=sx;
            h2val=width;
            v1val=sy;
            v2val=height;
        }
        h1reg=0X50;h2reg=0X51;     //ˮƽ���򴰿ڼĴ���
        v1reg=0X52;v2reg=0X53;     //��ֱ���򴰿ڼĴ���

        LCD_WriteReg(h1reg,h1val);
        LCD_WriteReg(h2reg,h2val);
        LCD_WriteReg(v1reg,v1val);
        LCD_WriteReg(v2reg,v2val);
        LCD_SetCursor(sx,sy);	      //���ù��λ��
    }
} 
//����FSMC
void LCD_FSMC_Config()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PD,PE,PF,PGʱ��
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//ʹ��FSMCʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        //PF10 �������,���Ʊ���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
    GPIO_Init(GPIOF, &GPIO_InitStructure);            //��ʼ�� //PF10 �������,���Ʊ���

    GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(7<<8)|(3<<14);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
    GPIO_Init(GPIOD, &GPIO_InitStructure);            //��ʼ��

    GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);         //PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
    GPIO_Init(GPIOE, &GPIO_InitStructure);            //��ʼ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //PG2,FSMC_A12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
    GPIO_Init(GPIOG, &GPIO_InitStructure);            //��ʼ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;        //PG12
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      //����
    GPIO_Init(GPIOG, &GPIO_InitStructure);            //��ʼ��

    GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12

    GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC); //PE7,AF12
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12

    GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);//PF12,AF12
    GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

    readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*16=96ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
    readWriteTiming.FSMC_DataSetupTime = 60;			 //���ݱ���ʱ��Ϊ60��HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA
    
    writeTiming.FSMC_AddressSetupTime =9;	     //��ַ����ʱ�䣨ADDSET��Ϊ9��HCLK =54ns
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨A
    writeTiming.FSMC_DataSetupTime = 8;		     //���ݱ���ʱ��Ϊ6ns*9��HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4; //ʹ��NE4 ��Ҳ�Ͷ�ӦBTCR[6],[7]��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // ���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  �洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // ��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // ʹ��BANK1

    delay_ms(50);
}

//��ʼ��lcd
void LCD_Init(void)
{ 			
    vu32 i=0;
    LCD_FSMC_Config();
    LCD_WriteReg(0x0000,0x0001);
    delay_ms(50);
    lcd_id = LCD_ReadReg(0x0000);    //��ȡLCD ID

    if(lcd_id<0XFF||lcd_id==0XFFFF||lcd_id==0X9300) //����ID����ȷ
    {
        //����9341 ID�Ķ�ȡ
        LCD_CMD=0XD3;
        lcd_id=LCD_DATA;
        lcd_id=LCD_DATA;
        lcd_id=LCD_DATA;   	//��ȡ93
        lcd_id<<=8;
        lcd_id|=LCD_DATA;  	//��ȡ41
    }
    if(lcd_id==0X9341) //�����9341,������WRʱ��Ϊ���
    {
        //��������дʱ����ƼĴ�����ʱ��
        FSMC_Bank1E->BWTR[6]&=~(0XF<<0); //��ַ����ʱ�䣨ADDSET������
        FSMC_Bank1E->BWTR[6]&=~(0XF<<8); //���ݱ���ʱ������
        FSMC_Bank1E->BWTR[6]|=3<<0;		   //��ַ����ʱ�䣨ADDSET��Ϊ3��HCLK =18ns
        FSMC_Bank1E->BWTR[6]|=2<<8;    	 //���ݱ���ʱ��Ϊ6ns*3��HCLK=18ns
    }
    if(lcd_id==0x9328) //ILI9328��ʼ��
    {
        LCD_WriteReg(0x00EC,0x108F);
        LCD_WriteReg(0x00EF,0x1234); //
        LCD_WriteReg(0x0001,0x0100);
        LCD_WriteReg(0x0002,0x0700); //��Դ����
        LCD_WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3) );
        LCD_WriteReg(0x0004,0x0000);
        LCD_WriteReg(0x0008,0x0202);
        LCD_WriteReg(0x0009,0x0000);
        LCD_WriteReg(0x000a,0x0000);//��ʾ����
        LCD_WriteReg(0x000c,0x0001);
        LCD_WriteReg(0x000d,0x0000);
        LCD_WriteReg(0x000f,0x0000);
        LCD_WriteReg(0x0010,0x0000); //��Դ����
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);
        LCD_WriteReg(0x0013,0x0000);
        LCD_WriteReg(0x0007,0x0001);
        delay_ms(50);
        LCD_WriteReg(0x0010,0x1490);
        LCD_WriteReg(0x0011,0x0227);
        delay_ms(50);
        LCD_WriteReg(0x0012,0x008A);
        delay_ms(50);
        LCD_WriteReg(0x0013,0x1a00);
        LCD_WriteReg(0x0029,0x0006);
        LCD_WriteReg(0x002b,0x000d);
        delay_ms(50);
        LCD_WriteReg(0x0020,0x0000);
        LCD_WriteReg(0x0021,0x0000);
        delay_ms(50);
        LCD_WriteReg(0x0030,0x0000);
        LCD_WriteReg(0x0031,0x0604);
        LCD_WriteReg(0x0032,0x0305);
        LCD_WriteReg(0x0035,0x0000);
        LCD_WriteReg(0x0036,0x0C09);
        LCD_WriteReg(0x0037,0x0204);
        LCD_WriteReg(0x0038,0x0301);
        LCD_WriteReg(0x0039,0x0707);
        LCD_WriteReg(0x003c,0x0000);
        LCD_WriteReg(0x003d,0x0a0a);
        delay_ms(50);
        LCD_WriteReg(0x0050,0x0000); //ˮƽGRAM��ʼλ��
        LCD_WriteReg(0x0051,0x00ef); //ˮƽGRAM��ֹλ��
        LCD_WriteReg(0x0052,0x0000); //��ֱGRAM��ʼλ��
        LCD_WriteReg(0x0053,0x013f); //��ֱGRAM��ֹλ��
        LCD_WriteReg(0x0060,0xa700);
        LCD_WriteReg(0x0061,0x0001);
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
        LCD_WriteReg(0x0090,0x0010);
        LCD_WriteReg(0x0092,0x0600);
        LCD_WriteReg(0x0007,0x0133);//������ʾ����
    }
    else if(lcd_id==0x9325)//9325
    {
        LCD_WriteReg(0x00E5,0x78F0);
        LCD_WriteReg(0x0001,0x0100);
        LCD_WriteReg(0x0002,0x0700);
        LCD_WriteReg(0x0003,0x1030);
        LCD_WriteReg(0x0004,0x0000);
        LCD_WriteReg(0x0008,0x0202);
        LCD_WriteReg(0x0009,0x0000);
        LCD_WriteReg(0x000A,0x0000);
        LCD_WriteReg(0x000C,0x0000);
        LCD_WriteReg(0x000D,0x0000);
        LCD_WriteReg(0x000F,0x0000);
        //power on sequence VGHVGL
        LCD_WriteReg(0x0010,0x0000);
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);
        LCD_WriteReg(0x0013,0x0000);
        LCD_WriteReg(0x0007,0x0000);
        LCD_WriteReg(0x0010,0x1690);
        LCD_WriteReg(0x0011,0x0227);
        LCD_WriteReg(0x0012,0x009D);
        LCD_WriteReg(0x0013,0x1900);
        LCD_WriteReg(0x0029,0x0025);
        LCD_WriteReg(0x002B,0x000D);
        LCD_WriteReg(0x0030,0x0007);
        LCD_WriteReg(0x0031,0x0303);
        LCD_WriteReg(0x0032,0x0003);
        LCD_WriteReg(0x0035,0x0206);
        LCD_WriteReg(0x0036,0x0008);
        LCD_WriteReg(0x0037,0x0406);
        LCD_WriteReg(0x0038,0x0304);
        LCD_WriteReg(0x0039,0x0007);
        LCD_WriteReg(0x003C,0x0602);
        LCD_WriteReg(0x003D,0x0008);
        LCD_WriteReg(0x0050,0x0000);
        LCD_WriteReg(0x0051,0x00EF);
        LCD_WriteReg(0x0052,0x0000);
        LCD_WriteReg(0x0053,0x013F);
        LCD_WriteReg(0x0060,0xA700);
        LCD_WriteReg(0x0061,0x0001);
        LCD_WriteReg(0x006A,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
        LCD_WriteReg(0x0090,0x0010);
        LCD_WriteReg(0x0092,0x0600);
        LCD_WriteReg(0x0007,0x0133);
        LCD_WriteReg(0x00,0x0022);//
    }
    else if(lcd_id==0X9341)	//9341��ʼ��
    {
        LCD_CMD=0xCF;
        LCD_DATA=0x00;
        LCD_DATA=0xC1;
        LCD_DATA=0X30;
        LCD_CMD=0xED;
        LCD_DATA=0x64;
        LCD_DATA=0x03;
        LCD_DATA=0X12;
        LCD_DATA=0X81;
        LCD_CMD=0xE8;
        LCD_DATA=0x85;
        LCD_DATA=0x10;
        LCD_DATA=0x7A;
        LCD_CMD=0xCB;
        LCD_DATA=0x39;
        LCD_DATA=0x2C;
        LCD_DATA=0x00;
        LCD_DATA=0x34;
        LCD_DATA=0x02;
        LCD_CMD=0xF7;
        LCD_DATA=0x20;
        LCD_CMD=0xEA;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_CMD=0xC0;
        LCD_DATA=0x1B;
        LCD_CMD=0xC1;
        LCD_DATA=0x01;
        LCD_CMD=0xC5;
        LCD_DATA=0x30;
        LCD_DATA=0x30;
        LCD_CMD=0xC7;
        LCD_DATA=0XB7;
        LCD_CMD=0x36;
        LCD_DATA=0x48;
        LCD_CMD=0x3A;
        LCD_DATA=0x55;
        LCD_CMD=0xB1;
        LCD_DATA=0x00;
        LCD_DATA=0x1A;
        LCD_CMD=0xB6;
        LCD_DATA=0x0A;
        LCD_DATA=0xA2;
        LCD_CMD=0xF2;
        LCD_DATA=0x00;
        LCD_CMD=0x26;
        LCD_DATA=0x01;
        LCD_CMD=0xE0;
        LCD_DATA=0x0F;
        LCD_DATA=0x2A;
        LCD_DATA=0x28;
        LCD_DATA=0x08;
        LCD_DATA=0x0E;
        LCD_DATA=0x08;
        LCD_DATA=0x54;
        LCD_DATA=0XA9;
        LCD_DATA=0x43;
        LCD_DATA=0x0A;
        LCD_DATA=0x0F;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_CMD=0XE1;
        LCD_DATA=0x00;
        LCD_DATA=0x15;
        LCD_DATA=0x17;
        LCD_DATA=0x07;
        LCD_DATA=0x11;
        LCD_DATA=0x06;
        LCD_DATA=0x2B;
        LCD_DATA=0x56;
        LCD_DATA=0x3C;
        LCD_DATA=0x05;
        LCD_DATA=0x10;
        LCD_DATA=0x0F;
        LCD_DATA=0x3F;
        LCD_DATA=0x3F;
        LCD_DATA=0x0F;
        LCD_CMD=0x2B;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_DATA=0x01;
        LCD_DATA=0x3f;
        LCD_CMD=0x2A;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_DATA=0x00;
        LCD_DATA=0xef;
        LCD_CMD=0x11;
        delay_ms(120);
        LCD_CMD=0x29;
    }
    LCD_Display_Dir(0);		 //��ʼ��Ϊ����
    LCD_BACK=1;					   //��������
    LCD_Clear(WHITE);
}

/****************************************************************************
* ��    ��: void LCD_Clear(u16 color)
* ��    �ܣ���������
* ��ڲ�����color: Ҫ���������ɫ
* ���ز�������
* ˵    �����B
****************************************************************************/
void LCD_Clear(u16 color)
{
    u32 i=0;
    u32 pointnum=0;

    pointnum=lcd_width*lcd_height; 	 //�õ�LCD�ܵ���
    LCD_SetCursor(0x00,0x00);	       //���ù��λ��
    LCD_WriteRAM_Prepare();     		 //��ʼд��GRAM
    for(i=0;i<pointnum;i++)
    {
        LCD_DATA=color;
    }
} 

/****************************************************************************
* ��    ��: void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
* ��    �ܣ���ָ����������䵥����ɫ
* ��ڲ�����(sx,sy),(ex,ey):�����ζԽ�����
            color:Ҫ������ɫ
* ���ز�������
* ˵    ���������СΪ:(ex-sx+1)*(ey-sy+1)  �B
****************************************************************************/
void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
    u16 i,j;
    u16 nlen=0;

    nlen=ex-sx+1;
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);      				  //���ù��λ��
        LCD_WriteRAM_Prepare();     			  //��ʼд��GRAM
        for(j=0;j<nlen;j++)LCD_DATA=color;	//���ù��λ��
    }
} 

/****************************************************************************
* ��    ��: void LCD_DisplayChar(u16 x,u16 y,u8 num,u8 size)
* ��    �ܣ���ָ��λ����ʾһ���ַ�
* ��ڲ�����x,y:��ʼ����
            word:Ҫ��ʾ���ַ�:abcdefg1234567890...
            size:�����С 12/16/24
* ���ز�������
* ˵    ����ȡ��ģ�ο�����ȡ��ģ��ʽ������ģȡģ����Ϊ�ȴ��ϵ��£��ٴ�����  �B
****************************************************************************/
void LCD_DisplayChar(u16 x,u16 y,u8 word,u8 size)
{  							  
    u8  bytenum,bytedata, a,b;
    u16 ymid=y;

    if(size==12) bytenum=12;        // �жϸ����������ֿ�������ռ���ֽ���
    else if(size==16) bytenum=16;
    else if(size==24) bytenum=36;
    else return;

    word=word-' ';  //�õ�ƫ�ƺ��ֵ ��Ϊ�ո�֮ǰ���ַ�û��font.h�е���������

    for(b=0;b<bytenum;b++)
    {
        if(size==12)bytedata=ascii_1206[word][b]; 	 	  //����1206����
        else if(size==16)bytedata=ascii_1608[word][b];	//����1608����
        else if(size==24)bytedata=ascii_2412[word][b];	//����2412����
        else return;								                  //û�е��ַ����飬û�ַ��ֿ�
        for(a=0;a<8;a++)
        {
            if(bytedata&0x80)LCD_Color_DrawPoint(x,y,BRUSH_COLOR);
            else LCD_Color_DrawPoint(x,y,BACK_COLOR);
            bytedata<<=1;
            y++;
            if(y>=lcd_height)return;		//�������˳�����
            if((y-ymid)==size)
            {
                y=ymid;
                x++;
                if(x>=lcd_width)return;	  //�������˳�����
                break;
            }
        }
    }
} 

//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
    u32 mid=1;
    while(n--)mid*=m;
    return mid;
}

/****************************************************************************
* ��    ��: void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* ��    �ܣ���ָ��λ����ʾһ������
* ��ڲ�����x,y:�������
            num:��ֵ(0~999999999);
            len:����(��Ҫ��ʾ��λ��)
            size:�����С
            mode: 0����λΪ0����ʾ
                  1����λΪ0��ʾ0
* ���ز�������
* ˵    ����  �B
****************************************************************************/
void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
    u8 t,numtemp;
    u8 end0=0;
    for(t=0;t<len;t++)
    {
        numtemp=(num/LCD_Pow(10,len-t-1))%10;
        if(end0==0&&t<(len-1))
        {
            if(numtemp==0)
            {
                if(mode)LCD_DisplayChar(x+(size/2)*t,y,'0',size);
                else LCD_DisplayChar(x+(size/2)*t,y,' ',size);
                continue;
            }else end0=1;

        }
        LCD_DisplayChar(x+(size/2)*t,y,numtemp+'0',size);
    }
} 

/****************************************************************************
* ��    ��: void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* ��    �ܣ���ָ��λ����ʾһ���Զ�����ɫ������
* ��ڲ�����x,y:�������
            num:��ֵ(0~999999999);
            len:����(��Ҫ��ʾ��λ��)
            size:�����С
            mode: 0����λΪ0����ʾ
                  1����λΪ0��ʾ0
            brushcolor���Զ��廭����ɫ
            backcolor�� �Զ��屳����ɫ
* ���ز�������
* ˵    ����  �B
****************************************************************************/
void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode,u16 brushcolor,u16 backcolor)
{
    u16 bh_color,bk_color;

    bh_color=BRUSH_COLOR;  //�ݴ滭����ɫ
    bk_color=BACK_COLOR;   //�ݴ汳����ɫ

    BRUSH_COLOR=brushcolor;
    BACK_COLOR=backcolor;

    LCD_DisplayNum(x,y,num,len,size,mode);

    BRUSH_COLOR=bh_color;   //���ı�ϵͳ��ɫ
    BACK_COLOR=bk_color;
}

/****************************************************************************
* ��    ��: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* ��    �ܣ���ʾ�ַ���
* ��ڲ�����x,y:�������
*           size:�����С
*           *p:�ַ�����ʼ��ַ	
* ���ز�������
* ˵    ����  �B
****************************************************************************/	  
void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
{         
    while((*p<='~')&&(*p>=' ')) //�ж��ǲ��ǷǷ��ַ�!
    {
        LCD_DisplayChar(x,y,*p,size);
        x+=size/2;
        if(x>=lcd_width) break;
        p++;
    }
}

/****************************************************************************
* ��    ��: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* ��    �ܣ���ʾ�Զ����ַ���
* ��ڲ�����x,y:�������
*           width,height:�����С  
*           size:�����С
*           *p:�ַ�����ʼ��ַ	
*           brushcolor���Զ��廭����ɫ
*           backcolor�� �Զ��屳����ɫ
* ���ز�������
* ˵    ����  �B
****************************************************************************/	  
void LCD_DisplayString_color(u16 x,u16 y,u8 size,u8 *p,u16 brushcolor,u16 backcolor)
{
    u16 bh_color,bk_color;

    bh_color=BRUSH_COLOR;  //�ݴ滭����ɫ
    bk_color=BACK_COLOR;   //�ݴ汳����ɫ

    BRUSH_COLOR=brushcolor;
    BACK_COLOR=backcolor;

    LCD_DisplayString(x,y,size,p);

    BRUSH_COLOR=bh_color;   //���ı�ϵͳ��ɫ
    BACK_COLOR=bk_color;
}


//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_DisplayChar(x,y,*p,size);
        x+=size/2;
        p++;
    }  
}



/****************************************************************************
*��������LCD_ShowFontHZ
*��  �룺x��������ʾ��X����
*      * y��������ʾ��Y����
*      * cn��Ҫ��ʾ�ĺ���
*      * wordColor�����ֵ���ɫ
*      * backColor��������ɫ
*��  ����
*��  �ܣ�д���ſ��庺��
****************************************************************************/
#if 0
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)	 
{  
	u8 i, j, wordNum;
	u16 color;
	while (*cn != '\0')
	{
		LCD_Set_Window(x, y, x+31, y+28);
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNumɨ���ֿ������
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSK��λ��
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							//LCD_WriteData_Color(FRONT_COLOR); 	
							LCD_DATA=BRUSH_COLOR;
						} 						
						else
						{
							//LCD_WriteData_Color(BACK_COLOR); 
							LCD_DATA=BRUSH_COLOR;
						} 
						color<<=1;
					}//for(j=0;j<8;j++)����
				}    
			}
		} //for (wordNum=0; wordNum<20; wordNum++)���� 	
		cn += 2;
		x += 32;
	}
}
#endif


#if 1
void LCD_ShowFontHZ24(u16 x,u16 y,u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<sizeof(CnChar24x24)/sizeof(struct Cn24CharTypeDef); wordNum++)
		{
			if ((CnChar24x24[wordNum].Index[0]==*cn)
			     &&(CnChar24x24[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<72; i++) 
				{	//MSK��λ��
					color=CnChar24x24[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_Color_DrawPoint(x,y,BRUSH_COLOR);
						} 						
						else
						{
							LCD_Color_DrawPoint(x,y,BACK_COLOR);
						} 
						color<<=1;
						x++;
						if((x-x0)==24)
						{
							x=x0;
							y++;
							if((y-y0)==24)
							{
								y=y0;
							}
						}
					}
				}	
			}		
		}
		cn += 2;
		x += 24;
		x0=x;
	}
}	

void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNumɨ���ֿ������
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSK��λ��
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_Color_DrawPoint(x,y,BRUSH_COLOR);
						} 						
						else
						{
							LCD_Color_DrawPoint(x,y,BACK_COLOR);
						} 
						color<<=1;
						x++;
						if((x-x0)==32)
						{
							x=x0;
							y++;
							if((y-y0)==29)
							{
								y=y0;
							}
						}
					}//for(j=0;j<8;j++)����
				}	
			}
			
		} //for (wordNum=0; wordNum<20; wordNum++)���� 	
		cn += 2;
		x += 32;
		x0=x;
	}
}	
#endif

void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic)
{
	u16 temp = 0;
	long tmp=0,num=0;
	LCD_Set_Window(x, y, x+wide-1, y+high-1);
	num = wide * high * 2 ;
	do
	{  
		temp = pic[tmp + 1];
		temp = temp << 8;
		temp = temp | pic[tmp];
		//LCD_WriteData_Color(temp);//�����ʾ
		LCD_DATA=BRUSH_COLOR;
		tmp += 2;
	}
	while(tmp < num);	
}

