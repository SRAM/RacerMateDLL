
#ifndef _TWOPOLEFILTER_H_
#define _TWOPOLEFILTER_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include "comdefs.h"

#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

class X_EXPORT twoPoleFilter  {

	public:
		typedef struct  {
			float fval;
			float a;
		} STATE;

		double n;
		twoPoleFilter(int _n);
		virtual ~twoPoleFilter(void);
		double calc(double in);
		void setState(double _fval, double _a);
		void getState(double *_fval, double *_a);
		void reset(void);

	private:
		double fval;
		double a;
		double k;
		twoPoleFilter(const twoPoleFilter&);
		twoPoleFilter &operator = (const twoPoleFilter&);		// unimplemented

};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _X_H_
