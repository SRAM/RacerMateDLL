#ifndef _DEVICE_H_
#define _DEVICE_H_

// in rm_common
#include <enums.h>							// TRAINER_IS_CLIENT, WIN_RS232, etc

#include <dlldefs.h>

// in tlib:
//#include "bike.h"							// needed for Bike::PARAMS?
#include "datasource.h"


#include "nv.h"

#define NPORTS 256

#ifdef LOGDLL
    //#define ERGVIDFILE "ergvid.log"
#endif

#ifndef WIN32
    #define INVALID_SOCKET -1
#endif


class DEVICE  {
    #ifdef _DEBUG
    private:
        int bp;
    #endif

    public:
        EnumDeviceType what;
		  TRAINER_COMMUNICATION_TYPE commtype;								// defined in glib_defines.h
        int comport_number;

		 // const char *id;
		char id[32];						// id known to dll, eg, "com1", "udp-6666"
		char udpkey[32];					// QT's version of the udp client key, eg, "192.168.1.40 43298"
		//const char *udpkey;

        //char portname[32];
        unsigned long when;				// time at which this data was taken
        NV nv;
        unsigned char last_control_byte;

        RIDER_DATA rd;

        bool beep_enabled;
        #ifdef WIN32
            HANDLE hthread;
        #endif

        Bike::PARAMS bike_params;
        dataSource *ds;					// use for non-getnextrecord() connections to the trainer
        EV *ev;								// creates a long-running thread until stopTrainer() is called
        Course *course;
        LoggingType logging_type;		// type of performance logging
        bool file_mode;
        float circumference;				// in cm
        //char url[256];
        //int tcp_port;

        /****************************************************************************

        ****************************************************************************/

        DEVICE(void)  {													// constructor
#ifdef _DEBUG
            bp = 0;
#endif
			//memset(udpkey, 0, sizeof(udpkey));
			//udpkey = NULL;
			memset(udpkey, 0, sizeof(udpkey));
			comport_number = -1;

			commtype = BAD_INPUT_TYPE;
            what = DEVICE_NOT_SCANNED;
            when = 0L;
            memset(&nv, 0, sizeof(nv));
            last_control_byte = 0;

			memset(id, 0, sizeof(id));

            beep_enabled = false;
            #ifdef WIN32
                hthread = 0;
            #endif

            memset(&bike_params, 0, sizeof(bike_params));
            ds = NULL;
            ev = NULL;
            course = NULL;
            logging_type = NO_LOGGING;
            file_mode = false;

            circumference = 0.0f;
            //tcp_port = -1;
            //memset(url, 0, sizeof(url));
            return;
        }									// DEVICE constructor

        /****************************************************************************
            copy constructor
        ****************************************************************************/
		  /*
        DEVICE (const DEVICE &copy) {
#ifdef _DEBUG
            bp = 1;
#endif
            return;
        }									// copy constructor
		  */

        /****************************************************************************
            destructor
        ****************************************************************************/
        ~DEVICE(void)  {
            return;
        }
};

#endif



