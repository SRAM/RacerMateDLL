
#ifndef _QT_SMAV_H_
#define _QT_SMAV_H_
#ifdef WIN32
#include <windows.h>
#endif

namespace MYQT  {


#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

/**************************************************************************

**************************************************************************/

#ifdef WIN32
class __declspec(dllexport) qt_sMav  {
#else
class qt_sMav  {
#endif


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
		qt_sMav(int n, bool dosigma=false);
#else
		qt_sMav(int n);
#endif

		~qt_sMav();
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
}


#endif		// #ifndef _X_H_

