#ifndef _RMCONST_H_
#define _RMCONST_H_

#ifndef UDP_SERVER_SERIAL_PORT_BASE
	static const int UDP_SERVER_SERIAL_PORT_BASE = 221;						// 221 - 236
#endif

/*
static const unsigned char MODE_WIND = 0x2c;
static const unsigned char MODE_WIND_PAUSE = 0x28;
static const unsigned char MODE_RFTEST = 0x1C;
static const unsigned char MODE_ERGO = 0x14;
static const unsigned char MODE_ERGO_PAUSE = 0x10;
*/


#ifndef KEY1
	static const unsigned char KEY1 = 0x02;
	static const unsigned char KEY2 = 0x04;
	static const unsigned char KEY3 = 0x08;
	static const unsigned char KEY4 = 0x10;
	static const unsigned char KEY5 = 0x20;
	static const unsigned char KEY6 = 0x40;
#endif


#endif


