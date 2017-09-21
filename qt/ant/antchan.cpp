
#ifdef WIN32
	#pragma warning(disable:4996)       // strncpy_s
#endif

#include <memory.h>

#include "antchan.h"

char ANTCHAN::statestr[4][32] = {
	"STATUS_UNASSIGNED_CHANNEL",
	"STATUS_ASSIGNED_CHANNEL",
	"STATUS_SEARCHING_CHANNEL",
	"STATUS_TRACKING_CHANNEL"
};

/****************************************************************************

****************************************************************************/

ANTCHAN::ANTCHAN(void) {
}

/******************************************************************************

******************************************************************************/

ANTCHAN::~ANTCHAN(void) {

}


/****************************************************************************

****************************************************************************/

void ANTCHAN::reset(void) {
	state = UNASSIGNED;
	netnum = 0;
	returned_type = 0;
	number = -1;                                   // channel number
	freq = 0;
	period = 0;
	devtype = 0;
	//typestr[32] = { 0 };
	open = false;
	return;
}

