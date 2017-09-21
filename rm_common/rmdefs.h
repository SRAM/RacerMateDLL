#ifndef _RMDEFS_H_
#define _RMDEFS_H_

//#define INTID


enum LoggingType {
	NO_LOGGING,
	RAW_PERF_LOGGING,
	THREE_D_PERF_LOGGING,
	RM1_PERF_LOGGING
};


#define NOMAP
#ifdef MAXCLIENTS
xxxxxxxxxxxxxxxxxxxxxxxxxx
#else
	#define MAXCLIENTS 8
	#define MULTICLIENT

		enum  {
			SECT_DEVNUM = 1,
			SECT_FAST,
			SECT_SLOW,
			SECT_SS,
			SECT_IDLE,
			SECT_HBCTL,
			SECT_EMPTY,
			SECT_RANDOM=64,
			SECT_HERE=255
		};

#endif

	//#define TEST_WITH_NO_APP

	#define CT_MODE_WIND				0x2c
	#define CT_MODE_WIND_PAUSE		0x28
	#define CT_MODE_RFTEST			0x1C
	#define CT_MODE_ERGO				0x14
	#define CT_MODE_ERGO_PAUSE		0x10

#endif

