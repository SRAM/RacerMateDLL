
#ifndef _LPFILTER_H_
#define _LPFILTER_H_

#ifdef WIN32
	#include <windows.h>
#endif

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
#endif

#ifdef WIN32
class __declspec(dllexport) LPFilter  {
#else
class LPFilter  {
#endif


	private:
		double fval;
		LPFilter(const LPFilter&);
		LPFilter &operator = (const LPFilter&);		// unimplemented

	public:
		double n;
		LPFilter(void);
		LPFilter(int _n);
		virtual ~LPFilter(void);

#ifdef _DEBUG
		double calc(double in, int _id=-1);
		void setfval(double _fval);
#else
		double calc(double in);
		void setfval(double _fval);
#endif

		void setTC(double _n);
		double getTC(void);
		double getfval(void)  {return fval;}
		void reset(void);
};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _X_H_

