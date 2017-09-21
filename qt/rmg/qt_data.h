#ifndef _QT_DATA_H_
#define _QT_DATA_H_

#include <comdefs.h>
#include <rmdefs.h>


namespace RACERMATE  {

class X_EXPORT QT_DATA  {
	public:
		QT_DATA(void);
#ifdef INTID
		int id;
#else
		QString key;
#endif
		float mph;
		float watts;
		float rpm;
		float lata;
		float rata;
		float miles;
		float ss;
		float lss;
		float rss;
		float lwatts;
		float rwatts;
		float avgss;
		float avglss;
		float avgrss;
		float hr;
		float cals;

		unsigned short hrflags;

#ifdef MULTICLIENT
		unsigned short devnum;
#endif

		void reset(void);

	private:
		void init(void);

};
}

#endif


