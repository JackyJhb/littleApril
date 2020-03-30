#ifndef PROTOCOL_H
#define PROTOCOL_H
typedef enum {
    FromClient = 'c',
    FromServer = 's'
}DataSource;

typedef enum {
	WarnningDataClient 	= 'w',
	RealDataClient  		= 'r',
	MaunalOperateClient = 'm',
	AutoOperateClient   = 'a'
}DataTypeClient;

typedef enum {
	WarnningRefSet    	= 'w',
	VentilateRefSet 		= 'v',
	ColdDownRefSet      = 'c',
	HeatingRefSet       = 'h'
}DataTypeServer;


typedef struct {
    char dataSource;
		char deviceID[12];
    char dataType;
}SubscribeOrPublishTopic;


#endif
