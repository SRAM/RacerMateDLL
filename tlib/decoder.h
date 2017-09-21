
#ifndef _DECODER_H_
#define _DECODER_H_


#ifdef WIN32
	#include <windows.h>
#endif

#include <glib_defines.h>
#include <tdefs.h>
#include <metadata.h>
#include <rider_data.h>
#include <logger.h>

#include <averagetimer.h>
#include <fasttimer.h>
#include <course.h>
#include <bike.h>
//#include <aunt.h>

#ifdef NEWUSER
    //#include <rider.h>
#else
    #include <user.h>
#endif
#include <ss.h>
//lint -save -e40 -e32 -e30 -e36 -e145 -e49 -e39 -e119
#include <formula.h>
//lint -restore

typedef struct  {
	float min_hr; float max_hr; float avg_hr;
	float min_rpm; float max_rpm; float avg_rpm;
	float min_mps; float max_mps; float avg_mps;							// meters per second
	float min_pp; float max_pp; float avg_pp;
	float min_wpkg; float max_wpkg; float avg_wpkg;

	float min_watts; float max_watts; float avg_watts;
	float min_lwatts; float max_lwatts; float avg_lwatts;
	float min_rwatts; float max_rwatts; float avg_rwatts;

	float min_ss; float max_ss; float avg_ss;
	float min_lss; float max_lss; float avg_lss;
	float min_rss; float max_rss; float avg_rss;

	float min_lata; float max_lata; float avg_lata;
	float min_rata; float max_rata; float avg_rata;

	float calories;
	float meters;							// meters pedaled after started
	DWORD ms;								// miliseconds while started

} MINMAXAVG;


	class Decoder  {
	friend class VIDPERF;

	protected:
		//int rpm_invalid;					// counter for rmp not connected
		//int hr_invalid;						// counter for HR not connected

		//bool do_export;						// flag for cannondale to export slope to a file in real time

		void compute_armed_rpm(void);
		static int abs_tick;
#ifdef _DEBUG
		int rel_tick;
#endif

		static int instances;
		int instance;

		float TICK_PERIOD;
		int MINDLY;
		int MAXDLY;
		int dly;
		int ramp;
		int period;
		int last_hi_time;
		int armed;

		//these are static results per invocation, forced to zero after 2.5 seconds with no l/h edge

		float armed_rpm;
		int armed_period;            // "ticks" (hb packets of TICK_PERIOD ms) from last armed lh xsn to this one
		int armed_dwell;            // ticks from previous lh event to last accum_tdc==true  while armed==0, dwell time

		//this is used as a flag by the phase calc, so it needs to be writable by the phase calc function
		unsigned int armed_time;            // abs_tick value at this armed hl xsn for phase

		bool shiftflag;

		DWORD startTime;
		DWORD pausedTime;				// accumulates paused time
		DWORD pauseStartTime;

		unsigned short version;
		bool do_drag;
		float startMiles;
		bool can_do_drag_factor;
		//float watts_factor;
		//bool ss_found;
		virtual void set_minmax(void);
		virtual void peaks_and_averages(void);
		bool hrvalid;

		#ifdef LOGKEYS
		#error "LOGKEYS is defined"
		#endif

		#define LOGKEYS

		int id;

		int min_hr;
		float min_pp;
		float max_pp;
		float min_wpkg;
		float max_wpkg;
		float avg_wpkg;
		float min_rpm;
		float min_mph;
		float min_watts;
		float min_ss;
		float max_ss;
		float min_lss;
		float max_lss;
		float min_rss;
		float max_rss;
		float min_lwatts;
		float max_lwatts;
		float avg_lwatts;
		float min_rwatts;
		float max_rwatts;
		float avg_rwatts;
		int min_lata;
		int max_lata;
		float avg_lata;
		int min_rata;
		int max_rata;
		float avg_rata;
		float wpkg;								// watts per kg
		float kgs;


		float us, lastus, dus;
		//static int resets;
		int resets;
#ifdef WIN32
		fastTimer *ft;
#endif
		DWORD totalMS;
		float total_miles;
		float lbs;
		void computePP(void);
		double totalMPH;
		double totalWatts;
		double totalRPM;
		double totalHR;
		float total_pedal_rpm;
		float totalLSS;
		float totalRSS;
		float totalSS;
		float totalPP;
		float totalLATA;
		float totalRATA;
		float total_lwatts;
		float total_rwatts;
		float total_wpkg;


		DWORD hravgcounter;
		DWORD inzonecount;
		DWORD averageCounter;
		void logmeta(void);
		Course *course;
#ifndef NEWUSER
		User *user;
#endif

		BOOL logging;
#ifdef WIN32
		AverageTimer *at;
#endif
		char string[256];
		Logger *logg;
		bool started;
		bool finished;
		bool finishEdge;
		bool paused;

	public:
		Decoder(int _segments, Course *_course, RIDER_DATA &_rd, int _id=0);
		virtual ~Decoder();
		static bool decoderlog;
		static bool bini;
		RIDER_DATA rd;
		Formula *formula;
		//int bad_parity_count;
		int packet_error3;					// keys byte parity bit
		int packet_error4;
		int set_ftp(float _ftp);
		float get_max_ss(void)  { return max_ss; }
		//static inline int get_instances(void)  { return instances; }
		//static void reset_instances(void)  { instances = 0; abs_tick = 0; }

		virtual void set_manual_mph(float _manual_mph)  { return; }

		virtual double inline get_measured_ergo_watts(void)  { return 0.0; }
		virtual Bike::GEARPAIR get_gear_pair(void) { Bike::GEARPAIR gp = {0}; return gp; }
		virtual Bike::GEARPAIR get_gear_indices(void) { Bike::GEARPAIR gp = {0}; return gp; }

		inline Formula *get_formula(void)  { return formula; }

		void seed_avgs(void);							// used for ergvid
		float accum_watts;								// used for ergvid
		float accum_rpm;								// used for ergvid
		float accum_hr;									// used for ergvid
		float accum_kph;								// used for ergvid
		int accum_tdc;
		inline int get_armed(void)  { return armed; }
		inline float get_armed_rpm(void)  {
			return armed_rpm;
		}
		inline int get_armed_period(void)  { return armed_period; }
		inline int get_armed_dwell(void)  { return armed_dwell; }
		inline unsigned long get_armed_time(void)		{ return armed_time; }

		int accum_watts_count;							// used for ergvid
		int accum_rpm_count;							// used for ergvid
		int accum_hr_count;								// used for ergvid
		int accum_kph_count;							// used for ergvid

		MINMAXAVG mma;				// saved peaks and averages that don't get reset so a report can be printed after reset



		void setlbs(float _lbs)  { lbs = _lbs; }
		void set_pause_start_time(DWORD _ms)  { pauseStartTime = _ms; }
		inline DWORD get_pause_start_time(void)  { return pauseStartTime; }
		inline DWORD get_start_time(void)  { return startTime; }
		inline DWORD get_paused_time(void) { return pausedTime; }
		void set_paused_time(DWORD dw) {
			pausedTime = dw;
#ifdef _DEBUG
			if (dw>0)  {
				bp = 1;
			}
#endif
		}
		virtual void gradeUp(void) { return; }
		virtual void gradeDown(void) { return; }
		inline bool get_shiftflag(void)  { return shiftflag; }

		void log(int level, int printdepth, int reset, const char *format, ...);

		inline bool get_finishEdge(void)  { return finishEdge; }
		void set_finishEdge(bool b)  { finishEdge = b; }

		float get_total_miles(void)  { return total_miles; }

		void set_finished(bool b)  { finished = b; }
		inline bool get_finished(void)  { return finished; }

		inline DWORD get_totalMS(void)  { return totalMS; }

		virtual void set_weight_lbs(double _d)  {
			rd.pd.kgs = (float)(POUNDSTOKGS * _d);
			return;
		}
		virtual void set_weight_kgs(double _d)  {
			rd.pd.kgs = (float)_d;
			return;
		}

#ifndef NEWUSER
		virtual void set_lower_hr(float f)  { user->data.lower_hr = (float) f; }
		virtual void set_upper_hr(float f)  { user->data.upper_hr = (float) f; }
#else
		virtual void set_lower_hr(float f)  { rd.lower_hr = (float) f; }
		virtual void set_upper_hr(float f)  { rd.upper_hr = (float) f; }
#endif


		//void set_export(bool _b)  { do_export = _b; }

		void setid(int _id)  { id = _id; }
		void set_version(unsigned short us) { version = us; };
		inline unsigned short get_version(void)  { return version; }
		void set_startMiles(float _f)  { startMiles = _f; }
		inline float get_startMiles(void)  { return startMiles; }

#ifdef _DEBUG
		bool do_amplify;
		virtual void toggle_amplifier(void)  { return; }
#endif


		virtual int decode(unsigned char *packet, DWORD _ms=0)  {
			bp = 1;
			return 0;
		}

		bool get_can_do_drag_factor(void)  {
			return can_do_drag_factor;
		}

		int get_resets(void)  {
			return resets;
		}

		void fill_in_mma(void);


		#define DECODER_FUNC_PTR

		//virtual void set_grade(float _grade)  { return; }

		void set_id(int _id)  {
			id = _id;
		}
		void *object;
		void (*keydownfunc)(void *object, int index, int _key);	// function pointer to application level function
		void (*keyupfunc)  (void *object, int index, int _key);	// function pointer to application level function

		virtual void inc_speed(void) { return; }
		virtual void dec_speed(void) { return; }

		virtual void update(void) { return; }					// added so demoSource's demoDecoder could do peaks, avgs, integration
		virtual void update(float) { return; }					// used by compupacer
		virtual void integrate(void) { return; }		// added for 3d software to stop jerking

		unsigned char lastkeys;
		unsigned char keys;
		unsigned char accum_keys;
		virtual void reset(void);
		//virtual void reset_averages(void);

		inline bool get_started(void)  { return started; }
		void set_started(bool _b)  { started = _b; }

		inline bool get_paused(void)  { return paused; }
		void set_paused(bool _paused)  { paused = _paused; }
		virtual bool get_physics_pause(void)  { return false; }
		virtual void set_physics_pause(bool _b)  { return; }


		void set_total_miles(float _total_miles)  {
			total_miles = _total_miles;
			return;
		}

#ifdef NEWMETA
		virtual void setDistance(float _meters)  {				// for compatibility with 3d
			meters = _meters;
			meta.meters;

			//meta.feet = (float)METERSTOFEET * meters;
			//meta.miles = (float)METERSTOMILES * meters;
			return;
		}
#else
		virtual void setDistance(float _meters)  {				// for compatibility with 3d
			meters = _meters;
			meta.feet = (float)METERSTOFEET * meters;
			meta.miles = (float)METERSTOMILES * meters;
			return;
		}
#endif


		int bp;
		unsigned char flags;
		int status;
		double dt;
		double dseconds;						// seconds as a double
		double lastseconds;

		unsigned long dms;
		unsigned long ms;

		// note: the object that creates the decoder must point these pointers at
		// reall objects!!
		SS *ss;

		METADATA meta;
		DWORD packets;
		float rawspeed;						// for 3d compatibility
		float meters;							// for 3d compatibility
		double mps;								// meters per second

		float ant_speed;						// kph
		float ant_cadence;
		float ant_cadence_only;
		float ant_wheel_rpm;
		float ant_hr;

		double wind;						// in kph
		double draft_wind;					// in kph

		unsigned short losig;			// 10 bit data now, raw signal from velotron
		unsigned short hisig;			// 10 bit data now, raw signal from velotron
		bool hrPluggedIn;
		double lastmph;
		int polarbit;

		unsigned short hbstatus;
		unsigned short slip;
		bool slipflag;
		float raw_rpm;

		double weight;
		double drag_aerodynamic;
		double drag_tire;
		double targetpower;
		unsigned short voltsCount;
		double supplyVoltage;
		double amps;
		DWORD caltime;
		int hours;
		int minutes;
		int seconds;
		int tenths;
		void reset_averages(void);
		bool ssok;

	private:
		float ppd;
		void init_course(void);

	public:
		inline float get_watts_factor(void)  { return rd.watts_factor; }
		virtual void set_watts_factor(float _f);


		void set_speed(float _ant_speed);

		void set_speed_cadence(float _ant_wheel_rpm, float _ant_speed_meters_per_hour, float _ant_cadence );
		void set_cadence(float _ant_cadence_only );
		void set_hr(float _ant_hr);

};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _X_H_

