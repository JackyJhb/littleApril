#ifndef SURFACE_H
#define SURFACE_H

#define MAIN_DATA_COUNTER 9

typedef enum{
    MainSurfaceLevel,
    SettingSurfaceLevel,
    TemperatureSurfaceLevel,
}SurfaceLevel;
typedef enum{
    NoDeviceFound,
    Stopped,
    Working
}DeviceStatus;

typedef struct{
    float curTemperature;
    float curHumidity;
    float illuminationIntensity;
    float curOxygenContent;
    float carbonDioxide;
    DeviceStatus heatingDeviceStatus;
    DeviceStatus coolingDeviceStatus;
    DeviceStatus ventilateDeviceStatus;
    float changeAirWindowAngle;
}MainData;

typedef struct{
    float upperLimit;
    float lowerLimit;
}TempCtrlRef;

typedef struct{
    float upperLimit;
    float lowerLimit;
}HumidityCtrlRef;

typedef struct{
    float upperLimit;
    float lowerLimit;
}OxygenContentCtrlRef;

extern void mainSurface(void);
extern void refreshMainSurface(MainData *mainDataPtr);
extern unsigned char systemSetting(char key_value);
extern unsigned char temperatureSetSurface(TempCtrlRef *tempCtrlRefPtr);
extern unsigned char humiditySetSurface(HumidityCtrlRef *humidityCtrlRefPtr);
extern unsigned char oxygenContentSetSurface(OxygenContentCtrlRef *oxygenContentCtrlRefPtr);
extern void messageBoxSurface(char *str);

#endif

