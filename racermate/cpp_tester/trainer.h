#ifndef _TRAINER_H_
#define _TRAINER_H_

#include <public.h>
#include <bike.h>
#include <enums.h>
#include <dll.h>							// for SSDATA

class Trainer  {
	private:
		char gteststring[2048];
		TRAINER_COMMUNICATION_TYPE comtype;
		static int Chainrings[MAX_FRONT_GEARS];
		static int cogset[MAX_REAR_GEARS];
		float wheeldiameter;						// in mm, 27.0 inches
		int ActualChainring;						// physical number of teeth on velotron front gear
		int Actualcog;								// physical number of teeth on velotron rear gear
		int bp;

	public:
		Trainer(void);
		int init(const char * _ix);

		const char * appkey;							// known to highest level app, "UDP-6666", "COM1", etc
		char udpkey[64];								// used for udp only, eg, "192.168.1.40 6666"

		int iport;									// used for rs232 only
		char keyboard_keys_string[128] = { 0 };

		SSDATA ssdata;
		int keys;
		int manwatts;

		float wind;											// kph
		float draft_wind;									// kph

		int get_cal(void);

		bool calmode;								// convenience variable
		int drag_factor;
		EnumDeviceType what;
		float person_kgs;
		float bike_kgs;
		int slope;									// %grade * 100
		int fw;
		int cal;
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
		int status_bits;

};

#endif


