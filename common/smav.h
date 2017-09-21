
#ifndef _SMAV_H_
#define _SMAV_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include "comdefs.h"

#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

/**************************************************************************

**************************************************************************/

class X_EXPORT sMav  {

	private:
		#define DOSIGMA
		float *raw;								// raw data
		float *fval;							// filtered values

#ifdef DOSIGMA
		bool dosigma;
		double *squares;
		double sigma;
		double squares_sum;
#endif

#ifdef _DEBUG
		int bp;
		unsigned long count;
#endif

		int len;
		float sum;
		int k;
		int shiftlen;
		float curval;

	public:
#ifdef DOSIGMA
		sMav(int n, bool dosigma=false);
#else
		sMav(int n);
#endif

		~sMav();
		float compute(float input);
		float first(void);
		float last(void);
		void reset(float val=0.0f);		// seeds the moving average
		void setShift(int n=0);					// sets the shift value from 0 to len-1
		inline float get_curval(void)  { return curval; }
#ifdef DOSIGMA
		inline double get_sigma(void)  { return sigma; }
#endif

};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _X_H_

