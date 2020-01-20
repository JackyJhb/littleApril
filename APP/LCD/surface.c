#include "surface.h"
#include "string.h"
#include "key.h"

#include "lcd.h"

#define START_X_POS         10            //列起始位置
#define START_Y_POS         10            //行起始位置
#define HALF_X_POS      		160          //半行起始位置
#define PIXEL_PER_LINE 		 24            //单行行宽


extern const unsigned char WorkingStatusIcon[];
extern const unsigned char StoppedStatusIcon[];
extern const unsigned char ErrorStatusIcon[] ;

void mainSurface(void)
{
    unsigned int i;
    BRUSH_COLOR=BLACK;
    BACK_COLOR = WHITE;
    const char buf[][11] = {
        "当前温度：",
        "当前湿度：",
        "光照强度：",
        "氧气浓度：",
        "二氧化碳：",
        "加热装置：",
        "制冷装置：",
        "通风设备：",
        "换气通道：",
    };
    for (i = 0;i<9;i++)
    {
        LCD_ShowFontHZ24(START_X_POS,START_Y_POS+PIXEL_PER_LINE*i,(u8 *)&buf[i][0]);
        //LCD_DrawRectangle(START_X_POS,START_Y_POS+PIXEL_PER_LINE*i,START_X_POS+5*24+180,START_Y_POS+PIXEL_PER_LINE*i+24);
    }
}

void refreshMainSurface(MainData *mainDataPtr)
{
    char buf[50];
    int i;
    memset(buf,0,sizeof(buf));

    BRUSH_COLOR=BLUE;
    BACK_COLOR = WHITE;
    for (i = 0;i < MAIN_DATA_COUNTER;i++)
    {
        switch (i)
        {
        case 0:
            sprintf(buf,"%.2f",mainDataPtr->curTemperature);
            break;
        case 1:
            sprintf(buf,"%.2f",mainDataPtr->curHumidity);
            break;
        case 2:
            sprintf(buf,"%.2f",mainDataPtr->illuminationIntensity);
            break;
        case 3:
            sprintf(buf,"%.2f",mainDataPtr->curOxygenContent);
            break;
        case 4:
            sprintf(buf,"%.2f",mainDataPtr->carbonDioxide);
            break;
        case 5:
            switch (mainDataPtr->heatingDeviceStatus)
            {
            case NoDeviceFound:
                sprintf(buf,"%s","NoDeviceFound");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)ErrorStatusIcon);
                break;
            case Stopped:
                sprintf(buf,"%s","Stopped");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)StoppedStatusIcon);
                break;
            case Working:
                sprintf(buf,"%s","Working");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)WorkingStatusIcon);
                break;
            default:
                break;
            }
            break;
        case 6:
            switch (mainDataPtr->coolingDeviceStatus)
            {
            case NoDeviceFound:
                sprintf(buf,"%s","NoDeviceFound");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)ErrorStatusIcon);
                break;
            case Stopped:
                sprintf(buf,"%s","Stopped");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)StoppedStatusIcon);
                break;
            case Working:
                sprintf(buf,"%s","Working");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)WorkingStatusIcon);
                break;
            default:
                break;
            }
            break;
        case 7:
            switch (mainDataPtr->ventilateDeviceStatus)
            {
            case NoDeviceFound:
                sprintf(buf,"%s","NoDeviceFound");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)ErrorStatusIcon);
                break;
            case Stopped:
                sprintf(buf,"%s","Stopped");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)StoppedStatusIcon);
                break;
            case Working:
                sprintf(buf,"%s","Working");
                LCD_ShowPicture(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,46,18,(u8 *)WorkingStatusIcon);
                break;
            default:
                break;
            }
            break;
        case 8:
            sprintf(buf,"%.2f",mainDataPtr->changeAirWindowAngle);
            break;
        default:
            break;
        }
        if (i != 5 && i != 6 && i != 7)
        {
            LCD_ShowString(HALF_X_POS,START_Y_POS+PIXEL_PER_LINE*i+5,100,20,16,buf);
        }
    }
}
unsigned char systemSetting(char key_value)
{
    char i;
    static char pos = -1;
    const char buf[][11] = {
        "温度设置",
        "湿度设置",
        "光强设置",
        "二氧化碳",
        "加热设置",
        "制冷设置",
        "通风设置",
        "换气设置",
    };
    if (key_value == KEY_DOWN)
    {
        ++pos;
        if (pos > 7)
            pos = 0;
    }
    else if (key_value == KEY_UP)
    {
        if (pos == 0)
            pos = 7;
        else
            --pos;
    }
    else if (key_value == KEY_RIGHT)
    {
        pos = 0;
        return 0;
    }
    else
    {
        i = pos;
        pos = 0;
        return (i+1);
    }
    for (i = 0;i<8;i++)
    {
        if (i ==pos)
        {
            BRUSH_COLOR=WHITE;
            BACK_COLOR = BLACK;
        }
        else
        {
            BRUSH_COLOR=BLACK;
            BACK_COLOR = WHITE;
        }
        LCD_ShowFontHZ24(START_X_POS+50,START_Y_POS+(PIXEL_PER_LINE+5)*i,(u8 *)&buf[i][0]);
    }
    return 188;
}

unsigned char temperatureSetSurface(TempCtrlRef *tempCtrlRefPtr)
{
}

unsigned char humiditySetSurface(HumidityCtrlRef *humidityCtrlRefPtr)
{
}

unsigned char oxygenContentSetSurface(OxygenContentCtrlRef *oxygenContentCtrlRefPtr)
{
}

void messageBoxSurface(char *str)
{
}
