#ifndef _LINUX_FASTTIMER_H_
#define _LINUX_FASTTIMER_H_

//#include <stdio.h>
//#include <timeb.h>

/*
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
*/

#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>


#include <glib_defines.h>

/*************************************************************


*************************************************************/

class fastTimer  {
	 private:
		  int bp;
	//LARGE_INTEGER li;
	//__int64 basecount;
	//__int64 freq;
	//__int64 count;
	BOOL bstat;
	double seconds;
	double totalSeconds;
	fastTimer(const fastTimer&);
	fastTimer &operator = (const fastTimer&);		// unimplemented
	unsigned long cycles;
	char name[32];
	unsigned long ms;
	unsigned long average_counter;
	//qint64 totaldt, now, lastnow, dt, mindt, maxdt;
	double totaldt, now, dt, lastnow;
	double mindt, maxdt;
	unsigned long dw;
	float us;
		  //struct timeb timeb_start;
		  //struct timeb timeb_stop;
	char str[256];
	struct timezone tz;
	struct timeval tvstart;
	struct timeval tvstop;
		  double start_time;
		  double stop_time;

	 public:
	fastTimer(const char *_name=NULL);
	void reset(void);
	virtual ~fastTimer();
	void start(void);
	void stop(void);
		  void print(void);
	double getTotalSeconds(void);
	unsigned long getCycles(void);
	double getAverageSeconds(void);
	double getSeconds(void);
	unsigned long getms(void);
	//__int64 getFreq(void);
	void delay(unsigned long ms);
	void average(void);
	float getus(void);
	double getseconds(void);
};

#endif		// #ifndef _LINUX_FASTTIMER_H_

