#ifndef TMR_H
#define TMR_H

#include <comdefs.h>


namespace RACERMATE  {

/*************************************************************

*************************************************************/

class X_EXPORT Tmr  {

	 public:
		//Tmr();
		Tmr(const char *_name=NULL);
		~Tmr();
		void start(void);
		int stop(void);
		void update(void);
		int getdms()  { return (int)dms; }
#ifdef _DEBUG
		void setdbg(void)  {
			dbg = true;
		}
#endif

	 private:

		  char name[32];
		  qint64 startms = 0;
		  qint64 endms = 0;
		  qint64 dms = 0;
		  qint64 minms = 9999999999;
		  qint64 maxms = 0;

		  double seconds = 0;
		  double totalSeconds = 0;
		  unsigned long cycles = 0;
#ifdef _DEBUG
		  char str[256] = {0};
		  bool dbg = false;
#endif

		  // used by average timer:
		  qint64 atnow = 0;
		  qint64 atdt = 0;
		  qint64 atlastTime = 0;
		  qint64 attotaldt = 0;
		  qint64 atcount = 0;
		  qint64 atmindt  = 9999999999;
		  qint64 atmaxdt = 0;

};

}
#endif // TMR_H
