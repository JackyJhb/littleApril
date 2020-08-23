#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

#define ENABLE_OUTPUT_LOG
#define ENABLE_BLACK_BOX
#define ENABLE_PIRACY_TRAP

#ifdef ENABLE_OUTPUT_LOG
    //#define ENABLE_WIFI_LOG
#endif

#define true  1
#define false 0
	
#define IS_COLDING 0x8000
#define LEVEL_MASK 0x7FFF

#endif
