#ifndef PROTOCOL_H
#define PROTOCOL_H
typedef enum {
    RealDataClient,
    AlarmDataClient,
    ReferenceSetOKClient,
    ReferenceSetErrorClient,
    MQTTTest
}MessageType;

typedef enum {
    FromClient = 'C',
    FromServer = 'S'
}ClientOrServer;

typedef enum {
    ReferenceSetServer
}ServerOrder;

typedef struct{
    char clientDeviceID[12];
    char clientOrServer;
}SubscribeContent;

typedef struct{
	char a;
}RealDataPackage;

typedef struct{
	char b;
}AlarmDataPackage;

typedef struct {
    char msgType;

}ClientDataPackage;

#endif
