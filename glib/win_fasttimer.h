#pragma once
#pragma warning(disable:4996)
//#undef UNICODE

#include <windows.h>
//#include <stdio.h>

#include <glib_defines.h>

/*
#ifdef PROBLEMDEPENDENCY2
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT __declspec(dllimport)
#endif
*/


/*************************************************************

  how to test:

	__int64 start, end, freq, dt;
	//DWORD microseconds;
	double seconds;
	fastTimer *ft = new fastTimer();
	freq = ft->getFreq();
	start = ft->read();
	Sleep(1000);
	end = ft->read();
	dt = end - start;
	seconds = (double)(end - start) / (double)freq;
	DEL(ft);

*************************************************************/

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
	class __declspec(dllexport) fastTimer  {
	//class DLLEXPORT fastTimer  {

#else
class fastTimer  {
#endif


	private:
		LARGE_INTEGER li;
		__int64 basecount;
		__int64 freq;
		__int64 count;
		BOOL bstat;
		double seconds;
		double totalSeconds;
		//fastTimer(const fastTimer&) {return;}
		fastTimer(const fastTimer&);
		fastTimer &operator = (const fastTimer&);		// unimplemented
		unsigned long cycles;
		char name[32];
		unsigned long ms;
		unsigned long average_counter;
		__int64 totaldt, now, lastnow, dt, mindt, maxdt;
		//double totaldt, now, dt, lastnow;
		DWORD dw;
		float us;
		char str[256];

	public:
		fastTimer(char *_name=NULL);
		void reset(void);
		void print(void);
		virtual ~fastTimer();
		void start(void);
		double stop(void);
		double getTotalSeconds(void);
		unsigned long getCycles(void);
		_int64 read(void);
		double getAverageSeconds(void);
		double getSeconds(void);
		unsigned long getms(void);
		__int64 getFreq(void);
		void delay(DWORD ms);
		void average(void);
		float getus(void);
		double getseconds(void);
};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

