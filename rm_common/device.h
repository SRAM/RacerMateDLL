#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <enums.h>							// TRAINER_IS_CLIENT, WIN_RS232, etc, in rm_common/
#include <rider_data.h>						// this includes tdefs.h from tlib
//#include <rmdefs.h>

#ifdef WIN32
#include <datasource.h>
#include <bike.h>
#endif


#include "public.h"
#include "nv.h"

#define NPORTS 256

//#ifndef WIN32
//    #define INVALID_SOCKET -1
//#endif


class DEVICE  {
    #ifdef _DEBUG
    private:
        int bp = 0;
    #endif

    public:
        EnumDeviceType what = DEVICE_NOT_SCANNED;
		TRAINER_COMMUNICATION_TYPE commtype  = BAD_INPUT_TYPE;								// defined in glib_defines.h
        int comport_number = -1;

		char id[32] = {0};						// id known to dll, eg, "com1", "udp-6666"
		char udpkey[32] = {0};					// QT's version of the udp client key, eg, "192.168.1.40 43298"

        unsigned long when = 0L;				// time at which this data was taken
		NV nv;
        unsigned char last_control_byte = 0;

        RIDER_DATA rd;

        bool beep_enabled = false;

        #ifdef WIN32
			HANDLE hthread = 0;
			Bike::PARAMS bike_params = {0};
			dataSource *ds = NULL;					// use for non-getnextrecord() connections to the trainer
			EV *ev = NULL;								// creates a long-running thread until stopTrainer() is called
			Course *course = NULL;
		#endif

        LoggingType logging_type = NO_LOGGING;		// type of performance logging
        bool file_mode = false;
        float circumference = 0.0f;				// in cm
};

#endif




