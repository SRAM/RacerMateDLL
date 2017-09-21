#ifndef TMR_H
#define TMR_H

#include "comdefs.h"


namespace MYNAMESPACE  {

/*************************************************************

*************************************************************/

class X_EXPORT Tmr  {

	 public:
		//Tmr();
		Tmr(const char *_name=NULL);
		~Tmr();
		void start(void);
		void stop(void);
		void update(void);

	 private:
		  char name[32];
		  qint64 startms;
		  qint64 endms;
		  qint64 dms;
		  qint64 minms;
		  qint64 maxms;

		  double seconds;
		  double totalSeconds;
		  unsigned long cycles;
#ifdef _DEBUG
		  char str[256];
#endif

		  // used by average timer:
		  qint64 atnow;
		  qint64 atdt;
		  qint64 atlastTime;
		  qint64 attotaldt;
		  qint64 atcount;
		  qint64 atmindt;
		  qint64 atmaxdt;

};

}
#endif // TMR_H
