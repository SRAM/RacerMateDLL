
#ifndef _SIMDECODER_H_
#define _SIMDECODER_H_

/****************************************************************************


****************************************************************************/


//#include <windows.h>

//#include <config.h>

#include <course.h>
#include <decoder.h>
#ifndef NEWUSER
	#include <user.h>
#else
#endif


#include <lpfilter.h>

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
class __declspec(dllexport) simDecoder : public Decoder  {
	#else
class simDecoder : public Decoder  {
	#endif
#else
class simDecoder : public Decoder  {
#endif

	friend class Simulator;
	friend class Rider2;

	public:
		#define SPEED_FUNCTION 1				// 0 = random, 1 = constant, 2 = mathematical function
		#ifdef _DEBUG
			virtual void inc_speed(void);
			virtual void dec_speed(void);
			double dmph;
		#endif
		static void set_speeds(double _mphs[8]);
		static void set_speeds2(double mphs[8], int index[8], int n);
		#if SPEED_FUNCTION==0			// random
			static double mphs[8];		// base speed for random mphs
			double base_mph;
		#elif SPEED_FUNCTION==1			// constant speed
			static double mphs[8];
			double constant_mph;
		#elif SPEED_FUNCTION==2			// sine wave, eg
		#endif
		virtual void set_manual_mph(float _manual_mph);


	private:
		#define DT .030295900					// determined empirically from avgtimer->update()

		static int n_sim_decoders;
		int instance;
		bool manual;
		float manual_mph;

		double miles;
		//double mph;
		DWORD startTime;
		DWORD perfTime;


		int mode;
		DWORD total_ms;
		float min_mph;
		float max_mph;

		float min_watts;
		float max_watts;

		float min_hr;
		float max_hr;

		float min_rpm;
		float max_rpm;

		DWORD lastIntegrateTime;
		DWORD lastnow;
		bool firststart;

		void integrate(void);
		int lastGrade_i;

		LPFilter *mphFilter;
		LPFilter *wattsFilter;
		LPFilter *rpmFilter;
		LPFilter *hrFilter;

	public:
#ifndef NEWUSER
		simDecoder(Course *_course=NULL, User *_user=NULL);
#else
		simDecoder(Course *_course, RIDER_DATA &_rd);
#endif

		~simDecoder();
		virtual int decode(unsigned char *packet, DWORD _ms=0);
		virtual void reset(void);

};

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		}									//extern "C" {
	#endif
#endif

#endif		// #ifndef _SIMDECODER_H_

