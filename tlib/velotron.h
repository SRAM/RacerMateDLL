
#ifndef _VELOTRONRTD_H_
#define _VELOTRONRTD_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <tdefs.h>
#include <averagetimer.h>
#include <signalstrengthmeter.h>
#include <serial.h>
#include <physics.h>
#include <heartrate.h>
#include <velotron_decoder.h>
#include <rtd.h>

#ifdef _DEBUG
	//#define LOG_MPH
#endif

class Velotron : public RTD {
	friend class Calibrator;
	friend class EV;

	private:
		#define RXSIZE GRXSIZE					// 128
		#define UPDATE_MS 47						// was 50, but made 47 due to beat problems
		DWORD logtime;
		DWORD LOGTIME;							// how often we should
		VEL_LOGPACKET lp;
		bool shiftflag;
#ifdef _DEBUG
	#ifdef LOG_MPH
		FILE *logstream;
	#endif
#endif

		void setPicCurrent(unsigned short _currentCountToPic);
		bool rpmNotification;
		float polarsig;
		double polarStrength;
		double elStrength;
		void init(void);
		int front_index_save;
		int rear_index_save;

		LPFilter *rpmFilter;					// rpm filter for ergo mode since it doesn't come from physics' rpmFilter
		LPFilter *mphFilter;
		LPFilter *polarFilter;
		signalStrengthMeter *earLobeSSM;
		signalStrengthMeter *polarSSM;
		Heartrate *hr;
		BOOL packetsAvailable(void);
		unsigned char code[PACKETLEN];
		unsigned char packet[PACKETLEN];
		void destroy(void);
		DWORD syncErrors;
		DWORD checksumErrors;
		unsigned short rxinptr, rxoutptr;
		unsigned char *rxq;
		unsigned char b[3];
		unsigned short lastPicCurrentCount;
		DWORD version;
		unsigned long timeout;

	public:
		Velotron(const char *_port, RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type=NO_LOGGING);
		virtual ~Velotron(void);
		Serial *get_port(void)  { return port; }
		virtual Bike::GEARPAIR get_gear_pair(void);
		virtual Bike::GEARPAIR get_gear_indices(void);
		virtual void set_grade(float grade);
		virtual inline Physics::MODE get_physics_mode(void)  {	return Physics::mode; }									// defined in datasource.h
		virtual void setkgs(float _person_kgs);
		virtual int set_constant_force(float _force);
		virtual inline int get_calibration(void)  { return (int)decoder->caltime; }

		double getDisplayedWatts(void);
		void setmiles(double _miles);
		void set_start_miles(double _start_miles);
		virtual int getNextRecord(void);
		int getPreviousRecord(DWORD _delay);
		virtual int updateHardware(bool _force=false);

		//virtual void startCal(unsigned short picCurrentCount);
		virtual void startCal(void);

		virtual unsigned char start_cal(void);
		virtual void end_cal(unsigned char last_control_byte);

		//bool calibrating;

		Physics *physics;
		void emergencyStop(int flag);
		void setGrade(double _grade);
		void gradeUp(void);
		void gradeDown(void);

		virtual void set_wind(float _kph);
		virtual void set_draft_wind(float _kph);


		virtual void reset(void);
		virtual void setTimeout(unsigned long _timeout)  {
			timeout = _timeout;
			return;
		}
		virtual void set_hr_bounds(int LowBound, int HighBound, bool BeepEnabled);

		void reset_rxq(void)  {
			memset(rxq, 0, RXSIZE*PACKETLEN);
			rxinptr = rxoutptr = 0;
			return;
		}
		void kill_keys(void);

		//--------------------
		// public getters:
		//--------------------

		double getGrade(void)  { return physics->getGrade(); }
		virtual double getWatts(void);
		virtual float getConstantWatts(void);

		void pause(void);
		void resume(void);
		void start(void);
		void setPhysicsMode(Physics::MODE);
		void setConstantForce(double _constantForce);
		void setConstantWatts(float _manualWatts);
		void setConstantCurrent(float _constantCurrent);
		virtual void flush(void);
		virtual int get_keys(void);
		virtual int get_accum_keys(void);
		virtual int get_accum_tdc(void);
		virtual Physics* get_physics_x(void);

};

#endif		// #ifndef _VELOTRONRTD_H_


