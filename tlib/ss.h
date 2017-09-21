
#ifndef _SS_H_
#define _SS_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <logger.h>
#include <smav.h>
#include <averagetimer.h>
#include <lpfilter.h>
#include <twopolefilter.h>
#include <tdefs.h>
#include <metadata.h>




class SS {
	friend class computrainerDecoder;
	friend class Rider;
	friend class Polar;
	friend class Bars;

	public:
		typedef struct  {
			float raw;
			float fval;							// same as meta.bars[i]
			float a;
			float beginning_degrees;
			float center_degrees;

			float beginning_radians;			// for computing lata & rata
			float center_radians;				// for computing lata & rata

			float beginning_sin;
			float beginning_cos;
			float beginning_sin_rotated;		// rotated
			float beginning_cos_rotated;		// rotated

			float center_sin;
			float center_cos;
			float center_sin_rotated;			// rotated
			float center_cos_rotated;			// rotated

			short color;
		} THRUST;

		typedef struct {
			float ss;
			float lss;
			float rss;
			float lsplit;
			float rsplit;
			int lata;
			int rata;

			float *bars;								// pointer to thrust.fval
			float *average_bars;						// 24 bars
			float MIN_SS_FORCE;
			float MAX_SS_FORCE;
			float *center_cos_rotated;					// 24
			float *center_sin_rotated;					// 24
			float maxlthrust;
			float maxrthrust;
		} SSD;

		#define CUTOFF_RPM 30
		#define NBARS 24

		SS(void);
		~SS();
		void do_ten_hz_filter(METADATA *_meta, bool _started, bool _finished, bool _paused);
		//void do_ten_hz_filter(float *_fbars, bool _started, bool _finished, bool _paused);
		inline bool get_rescale(void) { return rescale; }
		void start(void);
		void reset(void);
		void set_degperseg(double _dps);
		float run(double _watts, double _force, double _rpm, METADATA *_meta);
		double get_max_force(void)  { return MAX_SS_FORCE; }
		void set_max_force(float f)  { MAX_SS_FORCE = f; }
		inline float *get_x_bars2(void)  { return bars2; }
		void reset_averages(void);
		void setraw(float *_bars);

	private:
		bool reset_flag;
		double accum_ss;
		double accum_lss;
		double accum_rss;
		DWORD lastavgtime;
		unsigned long averageCounter;
		LPFilter *wattsFilter;
		AverageTimer *at;
		void filter(void);
		void load(double rpm);
		int bp;
		float *lastTwoRevForces;
		void init(void);
		double averageForce;
		BOOL peddlingTimeout;
		BOOL primed;
		int inptr;
		float thisforce;
		float *force;
		unsigned long loadcount;
		BOOL firstValidTDC;
		int middle;
		int *counts;
		DWORD ssrec;
		float TCD;
		float TCN;
		sMav *rpmMav;
		//DWORD lastFilterTime;
		twoPoleFilter **tpf;
		float average_lss;
		float average_rss;
		float average_ss;
		float bars2[24];
		double total_bars2[24];
		unsigned long bars2_count[24];
		float MIN_SS_FORCE;
		float MAX_SS_FORCE;
		int abovecount, belowcount;
		void set_reset(bool _b)  { reset_flag = _b; }
		Logger *log;
		void snapshot(char *_fname);
		int SSShift;
		inline int get_shift(void) { return SSShift; }
		void set_shift(int _i) { SSShift = _i; }
		float minlthrust, minrthrust;
		float totalss;
		float leftss;
		float rightss;
		float leftwatts;
		float rightwatts;
		float lata;								//leftAta;
		float rata;								//rightAta;
		int ilata;
		int irata;
		float *filteredTorques;
		float *thisRevForces;
		THRUST *thrust;
		float this_rev_max;
		float maxlthrust;
		float maxrthrust;
		bool metric;
		double degperseg;
		bool rescale;
		int size;
};

#endif		// #ifndef _X_H_

