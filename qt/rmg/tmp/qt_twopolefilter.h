
#ifndef _QT_TWOPOLEFILTER_H_
#define _QT_TWOPOLEFILTER_H_

#ifdef WIN32
	#include <windows.h>
#endif
namespace MYQT  {

#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

#ifdef WIN32
class __declspec(dllexport) qt_twoPoleFilter  {
#else
class qt_twoPoleFilter  {
#endif


	public:
		typedef struct  {
			float fval;
			float a;
		} STATE;

		double n;
		qt_twoPoleFilter(int _n);
		virtual ~qt_twoPoleFilter(void);
		double calc(double in);
		void setState(double _fval, double _a);
		void getState(double *_fval, double *_a);
		void reset(void);

	private:
		double fval;
		double a;
		double k;
		qt_twoPoleFilter(const qt_twoPoleFilter&);
		qt_twoPoleFilter &operator = (const qt_twoPoleFilter&);		// unimplemented

};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif
}


#endif		// #ifndef _X_H_
