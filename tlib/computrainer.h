#ifndef _COMPUTRAINER_H_
#define _COMPUTRAINER_H_

#ifdef WIN32
	#include <windows.h>
#endif

//#include <rmdefs.h>

#include <rtd.h>
#include <computrainer_decoder.h>

/************************************************************************************************************

************************************************************************************************************/

class Computrainer : public RTD {
	public:
		enum COMPUTRAINER_MODE  {							// make sure this is in sync with 'computrainer_modes'
			HB_WIND_RUN = 0x2c,
			HB_WIND_PAUSE = 0x28,
			HB_RFTEST = 0x1c,
			HB_ERGO_RUN = 0x14,
			HB_ERGO_PAUSE = 0x10
		};

		enum COMPUTRAINER_KEYS  {
			RESET = 0x01,
			F1 = 0x02,
			F2 = 0x04,
			PLS = 0x10,								// name conflict in cregexp.h for 'PLUS' which cause strange compiler error
			F3 = 0x08,
			MINUS = 0x20
		};

	private:
#ifdef _DEBUG
		FILE *ctstream;
		int gulps;
#endif

		int lastGrade_i;
		int lastWind_i;
		unsigned char txbuf[6];
		unsigned char pkt_mask[6];
		unsigned char is_signed;

		float manualWattsStep;
		float manualWatts;
		bool done_finished;
		computrainerDecoder::RAW_COMPUTRAINER_LOGPACKET lp;
		long file_size;
		void destroy(void);
		void init(void);
		unsigned char packet[16+1];
		DWORD skips;
		unsigned char tx_select;
		unsigned char control_byte;
		DWORD lastWriteTime;
		int rr;
		//void makelogpacket(void);
		int constructor;								// which constructuor we're using

	public:
		Computrainer( const char *_port, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type, const char *_url, int _tcp_port );
		Computrainer( Serial *port, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type=NO_LOGGING );
	#ifdef HELO_WRLD
		Computrainer( int _ix, RACERMATE::Ctsrv *, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type=NO_LOGGING );
	#else
		Computrainer(int _ix, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type = NO_LOGGING);
	#endif
		Computrainer( Client *, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type=NO_LOGGING );
		~Computrainer(void);

		virtual RESULT get_result(void);
		virtual inline float getConstantWatts(void)  { return manualWatts; }
		virtual inline Physics::MODE get_physics_mode_x(void)  {	return Physics::mode; }									// defined in datasource.h
		virtual inline unsigned char get_computrainer_mode_x(void)  {	return control_byte; }									// defined in datasource.h
		virtual inline unsigned char get_control_byte_x(void) { return control_byte; }
		virtual void setkgs(float _person_kgs);
		virtual void set_grade(float grade);
		virtual void set_hr_bounds(int LowBound, int HighBound, bool BeepEnabled);
		virtual int get_keys(void);
		virtual int get_accum_keys(void);
		virtual int get_accum_tdc(void);
		virtual int get_status_bits(void);

		virtual unsigned char start_cal(void);
		virtual void end_cal(unsigned char last_control_byte);

		virtual int getNextRecord(void);
		inline float getManualWatts(void)  { return manualWatts; }
		virtual int updateHardware(bool _force=false);
		int getPreviousRecord(DWORD _delay);
		void gradeUp(void);
		void gradeDown(void);
		double getGrade(void);
		virtual void pause(void);
		void resume(void);
		void start(void);
		virtual void reset();
		void setConstantWatts(float _constantWatts);
		void dorr(void);

#ifdef WIN32
		virtual void finish(void);
		virtual char * save(bool lastperf=false, bool _showDialog=true);
#endif

		virtual void flush(void);
		virtual unsigned char getControlByte(void)  {
		return control_byte;
	}

};

#endif		//#ifndef _COMPUTRAINER_H_


