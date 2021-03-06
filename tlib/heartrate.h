
#ifndef _HEARTRATE_H_
#define _HEARTRATE_H_

#ifdef WIN32
	#pragma warning (disable: 4786 4996 4251)		// std library warnings
	#include <windows.h>
#endif

//lint -save -e40 -e32 -e30 -e36 -e145 -e49 -e39 -e119
#include <vector>
#include <deque>
//lint -restore

using namespace std;

#include <logger.h>
#include <smav.h>
#include <ini.h>

#include <beep2.h>

#include <signalstrengthmeter.h>
#include <lpfilter.h>

//extern "C" {

#define PTABSIZE 11

typedef struct  {
	double rawsig;
	double fsig;
	double minfsig;
	double maxfsig;
	double lastfsig;
	LPFilter *filter;
	double slope;
	double lastslope;
	double avgfsig;
	sMav *dcmav;
	double signalStrength;
	signalStrengthMeter *ssm;
	double threshold;
	int bufsize;
	unsigned char *buf;
	int bufptr;
} SIGDATA;


/**********************************************************************


**********************************************************************/


#ifdef WIN32
class __declspec(dllexport) Heartrate  {
#else
class Heartrate  {
#endif


	private:
		Beep2 *beep;
		int filterchangecounter;
		Ini *ini;
		BOOL debug;
		DWORD eOnTime;
		void restart(void);
		void insert(void);
		DWORD now;
		DWORD lastgoodpeaktime;
		double averageDownSlope;
		double beginDownSignal;
		DWORD beginDownSample;
		double totalppm;
		int center;
		unsigned long nptimer;
		unsigned long nvtimer;
		BOOL findpeak(void);
		deque<double> ptab;
		int downcount;
		int upcount;
		DWORD sample;
		DWORD thisPeak;
		DWORD lastPeak;
		DWORD period;
		double hz;
		double tperiod;
		unsigned char lastsignal;
		DWORD thisPeakTime, lastPeakTime;
		Logger *log;
		int bp;
		double rawppm;
		BOOL valid(void);

	public:

		Heartrate();
		~Heartrate();
		void input(unsigned short losig, unsigned short hisig);
		double filteredppm;
		static bool bini;

		void dump(void);
		void dumpPtab(void);

		double avgmin;
		unsigned long errorTimer;
		BOOL peak;

		SIGDATA *data;
		int gain;
		double peakval;
		double averageHeartRate;
		bool noiseflag;
		double filteredHz;
		char noisecode[16];
		DWORD pulses;
		DWORD validpulses;
		double lower_hr;					// lower alarm limit
		double upper_hr;					// upper alarm limit
		unsigned short minhr;			// lower alarm limit that the handlebar sends back to us
		unsigned short maxhr;			// upper alarm limit that the handlebar sends back to us
		double peakHR;
		double inZoneHr;
		DWORD inzonecount;
};

//}			// extern "C" {

#endif		// #ifndef _X_H_

