
#ifndef _DATASOURCE_H_
#define _DATASOURCE_H_

#ifdef WIN32
	#include <windows.h>
#endif

#if 1			//zzz
#include <public.h>
#include <tdefs.h>
#include <logger.h>
#include <glib_defines.h>
#include <tdefs.h>
#include <rmdefs.h>

#include <decoder.h>
#include <averagetimer.h>
#ifdef WIN32
	#include <timeout.h>
#endif
#include <course.h>
#include <physics.h>
#include <bike.h>
#endif

enum DSTATE {
	HAVE_DATA,
	NO_DATA_AVAILABLE
};



class dataSource {

	friend class Mbox;
	 friend class RPT;
	 friend class Rider;

	public:
		Decoder *decoder;
		virtual int set_constant_force_x(float _force)  { return 0; }
		void set_logging_x(LoggingType _logging_type);
		virtual const char *get_perf_file_name_x(void) { return NULL; }
		virtual inline int get_calibration_x(void) { return 0; }
		virtual void set_performance_file_name_x(const char *_perfname) { return; }
		unsigned long get_finish_ms_x(void)  {
			if (decoder)  {
				finish_ms = decoder->meta.time;
			}
			else  {
				finish_ms = 0L;
			}
			return finish_ms;

		}


		virtual void set_manual_mph(float _manual_mph) { return; }
		virtual RESULT get_result(void)  { return result; }

		void set_recording(bool b) { recording = b; }
		inline bool get_recording(void) { return recording; }
		void set_export(const char *fname);

	private:
		// needed for velotron (keep for now for dll / prolific)
		double MIN_SS_FORCE;
		double MAX_SS_FORCE;
		int abovecount;
		int belowcount;
		char logfname[32];
		void init(void);

	protected:

		// export stuff for cannnondale:
		//bool do_export;						// flag for cannondale to export slope to a file in real time
		char xprtname[260];
		FILE *export_stream;
		float last_export_grade;

		unsigned long finish_ms;
		char started_date_time[32];
#ifdef WIN32
		Timeout timeout;
#endif
		bool recording;
		RESULT result;
		LoggingType logging_type;
		//bool manual;
		int id;
		RIDER_DATA rd;
		EnumDeviceType device;
		bool initialized;
		bool registered;
		unsigned long bytes_out;
		unsigned long bytes_in;
		bool beepflag;
		float anerobic_threshold;
		VEL_LOGPACKET vlp;
		void makeMeta(void);
		void beep(void);
		unsigned long lastbeeptime;
		float ppd;
		Logger *log;
		char string[256];
		AverageTimer *at;
		DWORD resetTime;
		int bp;
		int lastix;
		bool dsmetric;				// the metric menu state, now using the global gMetric!!!
		long file_size;
		long firstRecordOffset;
		bool last_connected;
		unsigned long shutDownDelay;
		char export_name[256];
		Course *course;
		Bike *bike;
		//MODE mode;
		/*
enum MODE  {
	PHYSICS_WIND_LOAD_MODE,					// old WINDLOAD mode
	PHYSICS_CONSTANT_TORQUE_MODE,
	PHYSICS_CONSTANT_WATTS_MODE,			// old ERGO mode
	PHYSICS_CONSTANT_CURRENT_MODE
};
		*/
		//Physics::MODE mode;

	public:
		dataSource(RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type);
		dataSource(void);						// mfd constructor
		static bool gblog;
		virtual ~dataSource(void);
		inline bool is_initialized(void)  { return initialized; }
		Course *get_course(void)  { return course; }
		//bool calflag;

#ifdef WIN32
		virtual void keydown(WPARAM wparam) { return; }
#endif

		inline int get_lower_hr_x(void)  { return ROUND(rd.lower_hr); }
		inline int get_upper_hr_x(void)  { return ROUND(rd.upper_hr); }
		inline int get_hr_beep_enabled_x(void)  { return rd.hr_beep_enabled?1:0; }
		int set_x_ftp(float _ftp);
		inline bool is_paused_x(void)  {
			return decoder->get_paused();
		}
		inline bool is_started_x(void)  {
			return decoder->get_started();
		}
		inline bool is_finished_x(void)  {
			return decoder->get_finished();
		}

		virtual Bike::GEARPAIR get_gear_indices_x(void) { Bike::GEARPAIR gp={0}; return gp; }
		virtual int get_x_accum_tdc(void)  { return 0; }
		virtual int get_x_status_bits(void)  { return 0; }
		virtual void startCal_x(void) { return; }
		virtual void set_x_wind(float _mph)  {
			//float mph = (float)(KPHTOMPH * _kph);
			decoder->meta.wind = _mph;
			decoder->wind = _mph;
			return;
		}
		virtual Physics::MODE get_physics_mode_x(void) { return Physics::NOMODE; }
		virtual unsigned char get_computrainer_mode_x(void) { return 0xff; }
		virtual unsigned char get_control_byte_x(void)  { return 0xff; }
		virtual Physics* get_physics_x(void)  { return NULL; }





		virtual Bike::GEARPAIR get_gear_pair(void) { Bike::GEARPAIR gp={0}; return gp; }
		unsigned long packet_error1;
		unsigned long packet_error2;

		virtual int addcourse(const char *_control_file=NULL)  { return 0; }
		virtual char *getvidbase(void)  { return NULL; }
		virtual char *getvidfile(void)  { return NULL; }
		inline virtual bool is_realtime(void)  { return false; }

		virtual void setkgs(float _person_kgs) { return; }
		//virtual void set_drag_factor(float _drag_factor) { return; }
		void set_drag_factor(float _drag_factor);

		virtual void set_grade(float grade) { return; }
		//Q_UNUSED
		virtual void set_hr_bounds(int LowBound, int HighBound, bool BeepEnabled) { return; }


		virtual int get_keys(void)  { return 0x40; }					// return the 'lost connection' bit
		virtual int get_accum_keys(void)  { return 0x40; }					// return the 'lost connection' bit
		void clear_accum_tdc(void);

		#ifdef _DEBUG
			virtual void dump(void) { return; }
		#endif

		void sr(bool _r)  { registered = _r; }
		float get_np(void);
		float get_tss(void);
		float get_if(void);
		float get_avg_formula_watts(void);

		inline RIDER_DATA *get_rider_data(void)  {
			return &rd;
		}

		bool get_metric(void)  {
			return dsmetric;
		}
		inline unsigned long get_bytes_in(void)  {return bytes_in;}
		inline unsigned long get_bytes_out(void)  {return bytes_out;}

		void set_total_miles(float _total_miles)  {			// miles or minutes
			decoder->set_total_miles(_total_miles);
			return;
		}

		inline void flush_keys(void)  {
			decoder->meta.keys = 0;
			decoder->meta.keydown = 0;
			decoder->meta.keyup = 0;
			decoder->lastkeys = 0;
			return;
		}

		int readRate;
		bool connected;
		bool connection_changed;
		virtual void setTimeout(unsigned long timeout)  {
			return;
		}
		virtual char * getFirstName(void) { return NULL; }
		virtual char * getLastName(void) { return NULL; }

		virtual void start(void)  {
			return;
		}
		virtual void finish(void)  {
			return;
		}
		virtual int myexport(char *_fname) {return -1;}		// unimplemented by default
		DWORD records;				// 0 for realtime


		virtual int getNextRecord(void) { return 1; }							// 1 = no data, 0 = new data
		virtual int getPreviousRecord(DWORD _delay) { return -1; }
		virtual int getRecord(int k) {return -1;}
		virtual int get_nRecords(void) {return (int)records;}
		virtual bool out_of_data(void) { return false; }
		virtual BOOL packetsAvailable(void) { return FALSE; }
		virtual int seek(long rec) { return -1; }
		virtual int updateHardware(bool _force=false) { return 0; }
		virtual long getOffset(void) { return -1; }
		virtual void remap(RECT physrect, float mRecsToPhys, float bRecsToPhys) { return; }

		virtual unsigned char start_cal(void) { return 0x00; }
		virtual void end_cal(unsigned char last_control_byte) { return; };
		//virtual void startCal(unsigned short picCurrentCount) { return; }

		virtual void emergencyStop(int flag) { return; }
		virtual void gradeUp(void) { return; }
		virtual void gradeDown(void) { return; }
		virtual void reset(void);
		virtual void reset_averages(void);
		virtual double getGrade(void) { return 0.0; }
		virtual int read(int k) { return 0; }						// read record k (file mode only)
		virtual int jumpmiles(float _miles) { return 0; };
		virtual void pause(void) {
			decoder->set_paused(true);
			return;
		}
		inline bool get_finish_edge(void)  {
			return decoder->get_finishEdge();
		}
		virtual void resume(void) {
			decoder->set_paused(false);
			return;
		}

		virtual void test(void) { return; }					// general purpose routing for testing, doesn't do anything
																		// in particular, can do anything the I want the child
																		// class to do.
		virtual double getTotalMiles(void)  {return 0.0; /*return totalMiles*/}		// hack for .3dp files  embedded course length did not match the pinfo header course length.
		virtual unsigned char getControlByte(void)  {
			return 0;
		}
		virtual float get_start_miles(void)  { return 0.0f; }
		inline bool hrbeep_enabled(void)  {
			return beepflag;
		}

		//----------------
		// setters:
		//----------------

		void setAnerobicThreshold(float _f)  {
			anerobic_threshold = _f;
		}
		void setFinished(void)  {
			decoder->set_finished(true);
			return;
		}
		void set_hrbeep(bool _b)  {
			beepflag = _b;
		}

		virtual void setConstantCurrent(float _constantCurrent)  { return; }
		virtual void setConstantWatts(float _constantWatts)  { return; }



		char *get_export_name(void)  {
			return export_name;
		}

		float getAT(void)  {
			return anerobic_threshold;
		}



		virtual void set_draft_wind(float _mph)  {
			//float mph = (float)(KPHTOMPH * _kph);
			decoder->draft_wind = _mph;
			return;
		}
		virtual float get_draft_wind(void)  {							// returns mph
			//return 0.0f;
			return (float)decoder->draft_wind;
		}
		virtual float get_wind(void)  {									// returns mph
			//return 0.0f;
			return (float)decoder->wind;
		}




		virtual bool is_perf(void)  { return false; }

		float getRPM(void)  {
			return decoder->meta.rpm;
		}
		virtual double getWatts(void)  { return 0.0; }
		virtual float getConstantWatts(void)  { return 0.0f; }

		float gethr(void)  {
			return decoder->meta.hr;
		}

		virtual void clear_averages(void)  {
			return;
		}

		virtual void flush(void)  { return; }
		DWORD nextReadTime;
		DWORD lastReadTime;

		double xdelta;
		long left;
		long right;
		long cursorLeft;
		long cursorRight;
		long cursorWid;
		long winWid;

		bool eof;
		bool lasteof;
		bool eofedge;				// goes true once when we hit eof
		bool bof;
		bool lastbof;
		bool bofedge;				// goes true once when we hit bof

		virtual char * save(bool lastperf=false, bool _showDialog=true) { return NULL; }
		virtual void setPhysicsMode(int)  { return; }
		virtual void setShutDownDelay(unsigned long _ms)  { return; }
};

#endif		// #ifndef _X_H_

