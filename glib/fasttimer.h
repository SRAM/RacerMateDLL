
#ifndef _FASTTIMER_H_
#define _FASTTIMER_H_

#ifdef WIN32
	#include <win_fasttimer.h>
#else
	#include <linux_fasttimer.h>
#endif

#endif			// #ifndef _FASTTIMER_H_

