
#ifndef _TRAINER_H_
#define _TRAINER_H_

#include <errors.h>
#include <ergvid.h>

struct TRAINER  {
		TRAINER(void)  {
			keys = 0x40;
			#ifdef _DEBUG
				cb = 0;
			#endif

			what = DEVICE_NOT_SCANNED;
			watts = rpm = hr = mph = 0.0f;
			meta = NULL;
			memset(&gp, 0, sizeof(gp));
			memset(&td, 0, sizeof(td));
			mode = Physics::NOMODE;
			port = -1;
			ix = -1;
			fw = 0;
			cal = 0;
			calibrated = false;
			pos = -1;
			bike_kgs = 0.0f;
			person_kgs = 0.0f;
			drag_factor = 100;
			running = false;
		}

		#ifdef _DEBUG
		unsigned char cb;
		#endif

		bool running;
		int pos;
		bool calibrated;
		int fw;
		int cal;
		int port;
		int ix;
		Physics::MODE mode;
		float watts;
		float rpm, hr;
		float mph;
		int keys;
		TrainerData td;
		EnumDeviceType what;
		METADATA *meta;
		Bike::GEARPAIR gp;
		float bike_kgs;
		float person_kgs;
		int drag_factor;

		/*
		int i, ix, fw, status, port, mask, cal;
		DWORD now, last_display_time = 0L;
		char c;
		bool b;
		const char *cptr;
		bool flag = true;
		unsigned long display_count = 0L;
		FILE *logstream=NULL;
		*/

};


#endif



