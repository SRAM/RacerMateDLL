
#ifndef _RIDER_DATA_H_
#define _RIDER_DATA_H_


#include <person.h>
//#include <tdefs.h>
//#include <datasource.h>

//int RIDER_DATA::instances = 0;
//int rider_instances = 0;

//struct RIDER_DATA  {
class  RIDER_DATA  {
    public:
		PERSON_DATA pd;
		float lower_hr;
		float upper_hr;
		int hr_beep_enabled;
		int draft_enable;
		float drag_aerodynamic;
		float drag_tire;
		int cal;												// unsigned short for computrainer
		float watts_factor;
		float ftp;
		int metric;

#if 0
		// added these for multi_tts:
		int port;
		int enabled;
		int pos;
		int fw;
		int ix;						// just port-1 for convenience
		EnumDeviceType what;
		int keys;
		dataSource *ds;
#endif

		RIDER_DATA(void)  {									// constructor
			pd.age = 25.0;
			pd.year = 0;
			pd.month = 0;
			pd.day = 0;
			pd.kgs = 150.0*TOKGS;
			pd.cm = 0.0f;
			pd.sex = 'M';
			//strcpy(pd.firstname, "");
			//strcpy(pd.lastname, "");
			//pd.firstname = "";
			//pd.lastname = "";

			lower_hr = 60.0f;;
			upper_hr = 70.0f;
			draft_enable = false;
			drag_tire = .006f;
			cal = 0x0000;							// unsigned short
			watts_factor = 1.0f;
			ftp = 1.0f;
			drag_aerodynamic = 8.0f * watts_factor;
			hr_beep_enabled = false;
			metric = 0;
			//rider_instances++;

#if 0
			port = -1;
			enabled = 0;
			pos = -1;
			fw = 0;
			ix = -1;
			what = DEVICE_NOT_SCANNED;
			keys = 0x40;
			ds = NULL;

#endif
		}

		~RIDER_DATA()  {					// destructor
            ftp = 0.0f;
        }


#if 1
	RIDER_DATA(const RIDER_DATA& copy)  {					// copy constructor
		pd = copy.pd;
		lower_hr = copy.lower_hr;
		upper_hr = copy.upper_hr;
		hr_beep_enabled = copy.hr_beep_enabled;
		draft_enable = copy.draft_enable;
		drag_aerodynamic = copy.drag_aerodynamic;
		drag_tire = copy.drag_tire;
		cal = copy.cal;
		watts_factor = copy.watts_factor;
		ftp = copy.ftp;
		metric = copy.metric;

#if 0
		port = copy.port;
		enabled = copy.enabled;
		pos = copy.pos;
		fw = copy.fw;
		ix = copy.ix;
		what = copy.what;
		keys = copy.keys;
		ds = copy.ds;
#endif

		//site = copy.site.c_str();
		//city = copy.city.c_str();
		//state = copy.state.c_str();
	}
#endif

	/*
	void write(FILE *_stream)  {
		return;
	}
	*/
};
//}  RIDER_DATA;




#endif		// #ifndef _RIDER_DATA_H_

