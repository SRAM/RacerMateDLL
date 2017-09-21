#ifndef _TRAINER_H_
#define _TRAINER_H_

#include <public.h>
#include <bike.h>
#include <enums.h>
#include <dll.h>							// for SSDATA

class Trainer  {
	private:
		char gteststring[2048];
		int iport;
		char portname[64];
		TRAINER_COMMUNICATION_TYPE comtype;
		static int Chainrings[MAX_FRONT_GEARS];
		static int cogset[MAX_REAR_GEARS];
		float wheeldiameter;						// in mm, 27.0 inches
		int ActualChainring;						// physical number of teeth on velotron front gear
		int Actualcog;								// physical number of teeth on velotron rear gear
		int broadcast_port;
		int listen_port;
		int bp;

	public:
		Trainer(void);
		int init(const char * _ix, int _broadcast_port, int _listen_port);

		SSDATA ssdata;
		int keys;
		char keystr[128];
		int manwatts;

		float wind;											// kph
		float draft_wind;									// kph

		/*
		int iwind_kph;
		int idraft_wind_kph;

		int iwind_mph;
		int idraft_wind_mph;

		float wind_kph;
		float draft_wind_kph;

		float wind_mph;
		float draft_wind_mph;
		*/

		int get_cal(void);

		bool calmode;								// convenience variable
		int drag_factor;
		EnumDeviceType what;
		float person_kgs;
		float bike_kgs;
		int slope;									// %grade * 100
		int fw;
		int cal;
		const char * ix;
		int fix;										// index of velotron front gear (0-2)
		int rix;										// index of velotron rear gear (0-9)
		int maxfix;									// highest non-zero index for front gear
		int maxrix;									// highest non-zero index for rear gear
		float watts;
		float rpm;
		float hr;
		float mph;
		float *bars;
		float *average_bars;

};

#endif


