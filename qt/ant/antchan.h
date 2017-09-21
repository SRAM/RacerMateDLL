#ifndef _ANTCHAN_H_
#define _ANTCHAN_H_

#include <QtCore>

//#include "sensor.h"
#include "antdefs.h"

/****************************************************************************

****************************************************************************/

//#ifdef WIN32
//class __declspec(dllexport) ANTCHAN  {
//#else
class ANTCHAN  {
//#endif

public:

	QMap<unsigned short, SENSOR> sensors;				// sensor sn --> SENSOR

	int get_devtype(void) {
		return devtype;
	}
	unsigned short get_period(void) {
		return period;
	}
	enum CHANSTATE {
		UNASSIGNED,
		ASSIGNED,
		SEARCHING,
		TRACKING
	};


	CHANSTATE state=UNASSIGNED;
	static char statestr[4][32];
	unsigned char netnum = 0;
	unsigned char returned_type = 0;
	int number = -1;                                      // channel number
	unsigned char freq = 0;
	unsigned short period = 0;
	int devtype = 0;
	//char typestr[32] = { 0 };
	bool open = false;

public:
	ANTCHAN(void);
	~ANTCHAN(void);
	void reset(void);
};

#endif      // #ifndef _SENTRY_H_

