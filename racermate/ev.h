
#ifndef _EV_H_
#define _EV_H_

#define DO_TINYTHREAD

#ifdef WIN32
	//#include <windows.h>
	//#define DO_EXPORT
#else
	//#include <pthread.h>
#endif

#ifdef DO_TINYTHREAD
	#include <tinythread.h>
#endif

#include <tdefs.h>
#include <metadata.h>
#include <trainerdata.h>

#include <averagetimer.h>
#include <fasttimer.h>
#include <datasource.h>
#include <bike.h>

//#include <handler.h>

#include "dlldefs.h"
#include "levels.h"

#include <ssdata.h>


/*
#ifdef LEVEL
	XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#endif

#define DLL_FULL_ACCESS			65536
#define DLL_ERGVIDEO_ACCESS		32768
#define DLL_CANNONDALE_ACCESS	16384
#define DLL_MINIMUM_ACCESS		    1

#ifdef _DEBUG
	//#define EXTENDED_DLL
	//#define LEVEL	DLL_FULL_ACCESS
	#define LEVEL	DLL_CANNONDALE_ACCESS
	//#define LEVEL	DLL_ERGVIDEO_ACCESS
#else
	//#define EXTENDED_DLL						// allow for tts, motion sensor, plotter ENGINE only
	#define LEVEL DLL_MINIMUM_ACCESS
#endif
*/

//#ifdef EXTENDED_DLL
//#if LEVEL >= DLL_FULL_ACCESS
//	#include <rmp.h>
//#endif

/*
#ifdef _DEBUG
	#define LOGEV
#else
	#define LOGEV
#endif
*/

//#define EV_LOG_MPH

//#define EVLOGFILE "ev.log"



#define UPPER_SPEED 22.0f							// for velotron spindown
#define LOWER_SPEED 20.0f							// for velotron spindown
#define SPEEDUP_SPEED (UPPER_SPEED+1.0f)			// for velotron spindown

/*
struct TrainerData	{
	float kph;			// ALWAYS in KPH, application will metric convert. <0 on error
	float cadence;		// in RPM, any number <0 if sensor not connected or errored.
	float HR;			// in BPM, any number <0 if sensor not connected or errored.
	float Power;		// in Watts <0 on error
	//bool tdc;			// accumulated tdc
};
*/

/*
typedef struct {
	float ss;
	float lss;
	float rss;
	float lsplit;
	float rsplit;
} SSDATA;
*/


/*
struct SSDATA	{
	float ss;
	float lss;
	float rss;
	float lsplit;
	float rsplit;
};
*/

/*
struct SSD	{
	float ss;
	float lss;
	float rss;
	float lsplit;
	float rsplit;
	int lata;
	int rata;
	float *bars;								// 24 bars
	float *average_bars;						// 24 bars
	double MIN_SS_FORCE;
	double MAX_SS_FORCE;
	double *center_cos_rotated;					// 24
	double *center_sin_rotated;					// 24
	double maxlthrust;
	double maxrthrust;
};
*/

#ifdef WIN32
#else
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)
#endif


#define DO_EV_EVENT

/**************************************************************************************

************************************************************************************/

class EV  {

	private:
		//RACERMATE::Handler handler;

		char EVLOGFILE[260];
		//int tcp_port;
		//char url[256];
#ifdef WIN32
		static void CALLBACK EV::TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
#else
		//static void EV::TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
		//todo
#endif

		char gstr[256];
		unsigned long start_time;				// class start time
		unsigned long timeouts;
		bool lock;
		unsigned long thread_run_count;
		bool shiftflag;
		unsigned long max_lock_time;
		unsigned char last_control_byte;
		RIDER_DATA rd;
		int bp;
		Formula *formula;
#ifdef _DEBUG
		AverageTimer *at;
		fastTimer *ft;
#endif

		bool file_mode;
		bool running;
                char comstr[32];
#ifdef WIN32
		HANDLE stop_event;
#endif


#ifdef LOGEV
		FILE *logstream;
#endif

#ifdef EV_LOG_MPH
		FILE *mphstream;
#endif

#ifdef WIN32
	int timer_event;
#endif


#ifdef DO_TINYTHREAD
	bool contin;
	tthread::thread *thrd;
	tthread::mutex mut;
	static void serverthreadproc(void *);
	//static void clientthreadproc(void *);
#else
	#ifdef WIN32
		HANDLE hthread;
		static void threadproc(void *);
	#else
		pthread_t thread;
		static void *threadproc(void *);
		static void sig_alarm(int _sig);
		bool done;
		//bool freerun;
		void print_timer_res(void);
		int gettime_ms(struct timeval *t, struct timeval *tbegin);
	#endif
#endif

		Bike::GEARPAIR gp;

		RACERMATE::TrainerData td;
		SSDATA ssd;
		//METADATA meta;
		dataSource *ds;
		EnumDeviceType what;
#ifdef MAP
		const char *id;
		int realportnum;
#else
		int ix;
		int realportnum;
#endif
		char portname[32];

		float grade;
		char calstr[64];
		Bike *bike;
		Course *course;
		LoggingType logging_type;

		char ver[20];
		char cal[20];
		int start_collection_thread(void);

	public:

		//EV(int _ix, const char *_portname, EnumDeviceType _what, RIDER_DATA _rd, Bike::PARAMS *_params=NULL, Course *_course=NULL, LoggingType=NO_LOGGING);

#ifdef MAP
		EV(const char * _ix);
		//EV(const char * _realportnum, EnumDeviceType _what, RIDER_DATA _rd, Bike::PARAMS *_params, Course *_course, LoggingType, const char *_url, int _tcp_port);
		EV(const char * _realportnum, EnumDeviceType _what, RIDER_DATA _rd, Bike::PARAMS *_params, Course *_course, LoggingType);
	#else
		EV(int _ix);
		EV(int _realportnum, EnumDeviceType _what, RIDER_DATA _rd, Bike::PARAMS *_params, Course *_course, LoggingType, const char *_url, int _tcp_port);
#endif

		~EV(void);
		Bike::GEARPAIR get_gear_pair(void);
		Bike::GEARPAIR get_gear_indices_x(void);
		bool initialized;
		void set_file_mode_x(bool _mode) {
			file_mode = _mode;
		}
		void set_performance_file_name_x(const char *_perfname);
		inline bool get_shiftflag_x(void) { return shiftflag; }
		bool get_ant_stick_initialized(void);

		//void set_speed_cadence_sensor_params(float _circumference=0.0f);
		void set_circumference_x(float _circumference);
		//void set_use_speed_cadence_sensor_x(int _b);
		//void set_use_heartrate_sensor_x(int _b);


#ifdef DO_EXPORT
		void set_export(const char *_dbgfname, int _port);					// public
#endif

		Decoder *get_decoder_x(void)  {
			if (ds)  {
				return ds->decoder;
			}
			return NULL;
		}

		float get_slip_x(void)  {
			if (ds)  {
				return (float) ds->decoder->slip;
			}
			return 0.0f;
		}

		bool get_slipflag_x(void)  {
			if (ds)  {
				return ds->decoder->slipflag;
			}
			return false;
		}

		unsigned long get_finish_ms_x(void)  {
			if (ds)  {
				return ds->get_finish_ms_x();
			}
			return 0L;
		}

		/*
		float get_raw_rpm(void)  {
			if (ds)  {
				return ds->decoder->raw_rpm;
			}
			return false;
		}
		*/

		void clear_slipflag_x(void)  {
			if (ds)  {
				ds->decoder->slipflag = false;
			}
			return;
		}

//#ifdef WIN32
		int startCal_x(void);
		int endCal(void);
		int get_calibration_x(void);
//#endif

		int set_gear(int _front_index, int _rear_index);
		int set_ftp_x(float _ftp);
		RACERMATE::TrainerData *myread(void);
		RACERMATE::TrainerData *fastread(void);								// used for calibrating
		//inline bool get_finishEdge(void)  { if (ds) return ds->decoder->get_finishEdge(); else return false; }

		SSDATA *read_ss(void);
		void stop(void);
		EnumDeviceType getwhat(void)  { return what; }
		int set_watts(float _watts);
		int set_constant_force_x(float _force);
		int reset(void);
		int reset_averages(void);
		int set_wind_x(float);
		int set_draft_wind_x(float);
		int set_slope(float bike_kgs, float person_kgs, int drag_factor, float grade);			// float, int, float
		int set_hr_bounds_x(int LowBound, int HighBound, bool BeepEnabled);
		int get_keys(void);
		int get_accum_tdc_x(void);
		int get_status_bits_x(void);
		void clear_accum_tdc_x(void);
		int start_cal(void);
		int end_cal(unsigned char last_control_byte);
		int set_paused(bool _paused);
		const char *get_computrainer_cal(void);
		inline bool is_connected_x(void)  { return ds->connected; }
		int get_lower_hr_x(void);
		int get_upper_hr_x(void);
		int get_hr_beep_enabled_x(void);
		float *get_bars_x(void);
		float *get_average_bars_x(void);

		float get_calories_x(void);
		float get_x_np(void);
		float get_if_x(void);
		float get_tss_x(void);
		void calc_tp_x(unsigned long _ms, float _watts);
		float get_pp_x(void);
		int get_cal_time_x(void);
		void set_calibrating_x(bool b);
		void start(bool _b);
		void set_finished(bool _b);

		inline bool is_started_x(void)  { return ds->is_started_x(); }
		inline bool is_paused_x(void)  { return ds->is_paused_x(); }
		inline bool is_finished_x(void)  { return ds->is_finished_x(); }
		inline float get_watts_factor(void)  { return rd.watts_factor; }
		//inline bool get_finish_edge(void)  { return ds->get_finish_edge(); }

		static void (keydownfunc)(void *object, int index, int _key);
		static void (keyupfunc)  (void *object, int index, int _key);

		int send_current(unsigned short _current);
		void set_logging_x(LoggingType _logging_type);

#if LEVEL >= DLL_FULL_ACCESS
//		RMP *rmp;
		char perfname[256];
		bool loaded;

		inline dataSource *getds_x(void) { return ds; }
		METADATA *get_meta_x(void);
		//Decoder *get_decoder(void);
		unsigned char get_control_byte_x(void);
		Physics::MODE get_physics_mode_x(void);
		float get_manual_watts_x(void);
		float get_grade_x(void);
		Physics *get_physics_x(void);
		int get_packet_error1_x(int ix);
		int get_packet_error2_x(int ix);
		int get_packet_error3_x(int ix);
		int get_packet_error4_x(int ix);
		void set_manual_speed_x(float _manual_mph);
		const char *get_perf_file_name_x(void);
		unsigned long get_max_lock_time_x(void)  { return max_lock_time; }
		int load_x(const char *_fname);				// loads a performance file for a rmp device
#endif

#if LEVEL >= DLL_TRINERD_ACCESS
		int get_computrainer_mode_x(void);
#endif

};

#endif		// #ifndef _EV_H_
