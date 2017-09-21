
#ifndef _FORMULA_H_
#define _FORMULA_H_

#ifdef WIN32
	#pragma warning(disable: 4251)
	#include <windows.h>
#endif

//lint -save -e40 -e32 -e30 -e36 -e145 -e49 -e39 -e119
#include <deque>
//lint -restore

#include <glib_defines.h>
//#include <config.h>

#include <averagetimer.h>


typedef struct  {
	unsigned long ms;
	float watts;
} FORMULA_PAIR;


#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
class __declspec(dllexport) Formula  {

#else
class Formula  {
#endif

	public:
		float np;
		float IF;
		float tss;

	private:

		//static int count;								// instances;
		char str[256] = { 0 };

		double fourth_total = 0.0;
		unsigned long fourth_count = 0L;
		float fourth_avg = 0.0f;

		AverageTimer *t = NULL;
		int bp = 0;
		//int id = 0;											// instance number, 0, or 1
		std::deque<FORMULA_PAIR> dq;
		double total_watts = 0.0;
		float avg_watts = 0.0f;
#ifdef _DEBUG
		int maxdq = 0;
#endif
		float ftp = 0.0f;

	public:
		Formula(float _ftp=0.0f);
		virtual ~Formula();
		void reset(void);
		void calc(float watts);

		void set_ftp(float _ftp)  { ftp = _ftp; }
		float get_ftp(void)  { return ftp; }

#ifdef _DEBUG
		float get_maxdq(void)  { return (float)maxdq; }
#endif

		inline float get_x_np(void)  { return np; }
		inline float get_x_tss(void)  { return tss; }
		inline float get_x_if(void)  { return IF; }
		float get_avg_watts(void)  { return avg_watts; }
};
#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif
#endif

