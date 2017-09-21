
#ifndef _AUNT_H_
#define _AUNT_H_

#ifdef WIN32
	//#define DO_ANT				// crashes in windows 10?
#endif

#ifdef DO_ANT

#include <types.h>				// used to be dsi_types.h
#include <antdefines.h>
#include <antmessage.h>
#include <dsi_framer_ant.hpp>
#include <dsi_thread.h>
#include <dsi_serial_generic.hpp>


#ifdef _DEBUG
	//#define ANT_LOG
#endif


#ifdef ANT
zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
#endif


/************************************************************************************************************

************************************************************************************************************/

#define CHANNEL_TYPE_MASTER   (0)
#define CHANNEL_TYPE_SLAVE		(1)
#define CHANNEL_TYPE_INVALID	(2)

#define ANT_CONSOLE_PRINT

#define CHANBUF_SIZE   (MESG_MAX_SIZE_VALUE)     // Channel event buffer size, assumes worst case extended message size
#define RBUF_SIZE      (MESG_MAX_SIZE_VALUE)     // Protocol response buffer size

#define MESSAGE_TIMEOUT 3300
#define ENABLE_EXTENDED_MESSAGES			// Un - comment to enable extended messages.
#define ENABLE_EXTENDED_MESSAGES

#define ANT_HR 0
#define ANT_C 1
#define ANT_SC 2

#define ANT_INVALID_SENSOR -1;
#define HR_DEV_TYPE	120						// heart rate
#define SC_DEV_TYPE	121						// speed cadence
#define C_DEV_TYPE	122						// cadence

class ANT  {
	private:
		int init(void);

	protected:
		int reset(void);
#ifdef _DEBUG
		int bp;
#endif
	public:
		ANT(void);
		~ANT() {}
		DWORD inline get_decode_time(void)  { return decode_time; }

		float accum;
		unsigned long accum_count;

		DWORD last_open_attempt_time;
		DWORD last_status_request_time;
		DWORD last_data_time;
		DWORD channel_open_time;
		bool initialized;
		bool decoding_data;
		DWORD lastnow;
		UCHAR chan_num;				// 0
		UCHAR dev_type;				// 151
		UCHAR freq;						// 57
		USHORT period;					// 8086
		UCHAR trans_type;				// 0
		UCHAR chan_type;				// CHANNEL_TYPE_SLAVE, Channel type as chosen by user (master or slave), 0 = slave?
		char name[24];

		UCHAR page;
		UCHAR previous_event_count;
		USHORT previous_time_1024;
		USHORT time_1024;
		UCHAR event_count;
		ULONG elapsed_time2;			// Cumulative operating time (elapsed time since battery insertion) in 2 second resolution	
		UCHAR mfgr_id;					// Manufacturing ID
		USHORT sernum;					// Serial number
		UCHAR hw_version;				// Hardware version
		UCHAR sw_version;				// Software version
		UCHAR model;				   // Model number
		UCHAR no_event_count;		// Successive transmissions with no new cadence events
		ULONG acum_event_count;		// Cumulative heart beat event count
		USHORT rr_interval_1024;	// R-R interval (1/1024 seconds), conversion to ms is performed for data display
		UCHAR channel_status;

		bool inline tracking(void)  { return channel_status==STATUS_TRACKING_CHANNEL; }

	protected:
		DWORD decode_time;

};											// class ANT


class PAGE_SENSOR : public ANT  {

	public:
		//float get_val(void);						//  { return (float)val; }
		void reset_accum(void)  { accum = 0.0f; accum_count = 0L;	/* val = 0; */ }

		//unsigned char val;										// heartrate or cadence
		//unsigned char calculated_val;

	private:
		int init(void);

	protected:
		#define PAGE0				((UCHAR) 0)
		#define PAGE1				((UCHAR) 1)
		#define PAGE2				((UCHAR) 2)
		#define PAGE3				((UCHAR) 3)
		#define PAGE4				((UCHAR) 4)
		#define INVALID_PAGE		((UCHAR) 0)
		#define TOGGLE_MASK		((UCHAR) 0x80)

		enum statePage  {
			STD0_PAGE = 0,
			STD1_PAGE = 1,
			INIT_PAGE = 2,
			EXT_PAGE = 3
		};

		PAGE_SENSOR(void);
		~PAGE_SENSOR()  {}
		int reset(void);

		ULONG ulAcumTime1024;				// Cumulative time (1/1024 seconds), conversion to s is performed for data display
		UCHAR page;
		//UCHAR previous_event_count;
		USHORT previous_time_1024;
		USHORT time_1024;
		UCHAR event_count;
		ULONG elapsed_time2;					// Cumulative operating time (elapsed time since battery insertion) in 2 second resolution	
		UCHAR mfgr_id;							// Manufacturing ID
		USHORT sernum;							// Serial number
		UCHAR hw_version;						// Hardware version
		UCHAR sw_version;						// Software version
		UCHAR model;							// Model number
		UCHAR no_event_count;				// Successive transmissions with no new cadence events
		ULONG acum_event_count;				// Cumulative heart beat event count

};													// class PAGE_SENSOR


class CAD : public PAGE_SENSOR  {
	public:
		CAD(void);
		~CAD() {}
		int reset(void);
		int decode(void);												// decoder used by heart rate and cadence-only
		float get_val(void);						//  { return (float)val; }
		unsigned char val;											// cadence
		//unsigned char calculated_val;

	private:
		int init(void);
		USHORT previous_event_count;
};																		// class CAD

class HR : public PAGE_SENSOR  {
	public:
		HR(void) { reset(); }
		~HR() {}
		int reset(void);
		int decode(void);												// decoder used by heart rate and cadence-only
		unsigned char val;											// heartrate directly from sensor
		unsigned char calculated_val;
		float get_val(void);						//  { return (float)val; }

	private:
		UCHAR previous_event_count;
		USHORT rr_interval_1024;			// R-R interval (1/1024 seconds), conversion to ms is performed for data display

};																	// class HR


class SC : public ANT  {
	private:
		enum BSCState {
			BSC_INIT = 0,
			BSC_ACTIVE = 1,
		};

	public:
		SC(void) { reset(); }
		~SC() {}
		int decode(void);
		int reset(void);
		void set_circum(UCHAR _cm);
		float inline get_wheel_rpm(void)  { return wheel_rpm; }
		float inline get_speed(void)  { return speed; }						// Instantaneous speed (meters/h)
		float inline get_cadence(void) { return cadence; }
		void inline set_cadence(float _f) { cadence = _f; }

	private:
		float cadence;
		USHORT event_diff;
		USHORT time_diff;

		UCHAR circumference_cm;						// wheel circumference (cm)
		float wheel_rpm;
		float speed;

		USHORT speed_event_count;					// Bike speed event count (wheel revolutions)
		USHORT speed_previous_event_count;		// Bike speed previous event count
		USHORT speed_time;							// Time of last bike speed event (1/1024s)
		USHORT speed_previous_time;				// Time of previous bike speed event (1/1024s)

		ULONG speed_acum_event_count;				// Cumulative bike speed event count (wheel revolutions)
		ULONG speed_acum_time;						// Cumulative time (1/1024 seconds)

		USHORT cad_event_count;						// Bike cadence event count (pedal revolutions)
		USHORT cad_previous_event_count;			// Bike cadence previous event count
		USHORT cad_time;								// Time of last bike cadence event (1/1024s)
		USHORT cad_previous_time;					// Time of previous bike cadence event (1/1024s)

		ULONG cad_acum_event_count;				// Cumulative bike cadence event count (pedal revolutions)
		ULONG cad_acum_time;							// Cumulative time (1/1024 seconds)
		ULONG cad_distance;								// Cumulative distance (cm)

		BSCState cad_state;								// state for speed cadence sensor
		bool cad_coasting;									// coasting flag
		bool cad_stopping;
		UCHAR ucNoSpdEventCount;					// counter for successive transmissions with no new speed events
		UCHAR ucNoCadEventCount;					// counter for successive transmissions with no new cadence events
};												// class SC


extern SC sc;
extern CAD cad;
extern HR hr;

// variables:

extern bool asi;
extern bool sensors_initialized;
extern bool ant_closed;
extern bool do_ant_logging;
extern bool log_raw;

// functions:
extern int ant_init(void (*_ant_keyfunc)(void)=NULL );
extern int ant_start_sc(void);
extern int ant_start_cad(void);
extern int ant_start_heartrate(void);
extern void ant_set_circumference(float _mm);		//  { circumference_cm = ROUND(_mm/10.0f); }
extern int ant_close(void);

#endif					// #ifdef DO_ANT
#endif					// #ifndef _AUNT_H_
