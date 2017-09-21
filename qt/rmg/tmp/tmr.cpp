#include <string.h>

//#include <QDebug>

#include "tmr.h"

namespace RACERMATE  {

/*************************************************************

*************************************************************/

Tmr::Tmr(const char * _name)  {

	 if (_name==NULL)  {
		  strcpy(name, "tmr");
	 }
	 else  {
		  strncpy(name, _name, sizeof(name)-1);
	 }

	 /*
	 startms = 0;
	 endms = 0;
	 dms = 0;
	 seconds = 0.0;
	 totalSeconds = 0.0;
	 cycles = 0;
	 //minms = std::numeric_limits<qint64>::max();
	 */

	 /*
	 minms = 9999999999999;
	 maxms = 0;

	 atcount = 0;
	 attotaldt = 0;
	 atdt = 0;
	 atmindt = 9999999999;
	 atmaxdt = 0;
	 atnow = 0;
	 atlastTime = 0;
	*/

	 startms = 0;

	 return;
}

/*************************************************************

*************************************************************/
/*
Tmr::Tmr()  {
	return;
}
*/

/*************************************************************

*************************************************************/

Tmr::~Tmr()  {

//	bool qt = false;

#ifdef QT_CORE_LIB
//	qt = true;
#endif

#ifdef _DEBUG
	if (dbg)  {
		if (cycles>0)  {
			  if (name[0])  {
					sprintf(str, "%s: Average dt = %.6f milliseconds (((((((((((((((((((((((((((((((((((((((", name, (1000.0 * totalSeconds) / (double)cycles);
			  }
			  else  {
					sprintf(str, "Average dt = %.6f milliseconds", (1000.0 * totalSeconds) / (double)cycles);
			  }

			  qDebug() << str;
			  sprintf(str, "cycles = %ld", cycles);
			  qDebug() << str;
			  qDebug() << "min = " << minms << "ms";
			  qDebug() << "max = " << maxms << "ms";

		 }
		 else  {
			  //sprintf(str, "%s: stop() not called\n", name);
		 }
	}

	 if (atcount==0)  {
		  //sprintf(str, "%s: update not called ((((((((((((((((((((((((((((((((((((((((((", name);
		  //qDebug() << str;
	 }
	 else  {
		  sprintf(str, "%s: Average time between calls is %.6f milliseconds", name, (double)attotaldt / (double)atcount);
		  qDebug() << str;
		  sprintf(str, "%s: mindt %lld ms, maxdt = %lld ms", name, atmindt, atmaxdt);
		  qDebug() << str;
	 }

#endif

}



//----------------------------------------------------
//
//----------------------------------------------------

void Tmr::start(void)  {
	 startms = QDateTime::currentMSecsSinceEpoch();
	 return;
}


//----------------------------------------------------
//
//----------------------------------------------------

int Tmr::stop(void)  {
	 endms = QDateTime::currentMSecsSinceEpoch();
	 dms = endms - startms;
	 seconds = (double)(dms) / 1000.0;
	 totalSeconds += seconds;

	 minms = qMin(dms, minms);
	 maxms = qMax(dms, maxms);

	 cycles++;
	 return (int)dms;
}

/******************************************************************************

******************************************************************************/

void Tmr::update(void)  {

	 //now = timeGetTime();

	 atnow = QDateTime::currentMSecsSinceEpoch();


	 if (atlastTime == 0)  {
		  atlastTime = atnow;
		  return;						// don't count the first one
	 }

	 atdt = atnow - atlastTime;
	 atlastTime = atnow;
	 attotaldt += atdt;
	 atcount++;

	 atmaxdt = qMax(atmaxdt, atdt);
	 atmindt = qMin(atmindt, atdt);


}

}
