
#ifdef WIN32
	#pragma warning(disable:4996)			// strncpy_s
#endif




#ifdef WIN32
	#include <glib_defines.h>		// for DWORD and/or MIN/MAX
#else
	#include <string.h>
	#include <utils.h>					// for timeGetTime()
	#include <minmax.h>
#endif

#include <averagetimer.h>


/****************************************************************************
	timing debug class. Used to determine the average time between a point
	in a program where update() is called. The destructor prints the average
	time between calls to the debugger screen.
****************************************************************************/

AverageTimer::AverageTimer(void)  {
	reset();
	//totaldt = 0;
	//count = 0;
	//lastTime = 0;
	strcpy(name, "Default");
}

/******************************************************************************

******************************************************************************/

AverageTimer::AverageTimer(const char *_name)  {
	reset();
	//totaldt = 0;
	//count = 0;
	//lastTime = 0;
	strncpy(name, _name, 31);
}

/******************************************************************************

******************************************************************************/

AverageTimer::~AverageTimer(void)  {

#ifdef WIN32
	#ifdef _DEBUG
		char str[256];
	#endif
#else
		char str[256];
#endif

	if (count==0)  {
#ifdef WIN32
		#ifdef _DEBUG
		sprintf(str, "%s: update not called ((((((((((((((((((((((((((((((((((((((((((\r\n", name);
		OutputDebugString(str);
		#endif
#else
		sprintf(str, "%s: update not called ((((((((((((((((((((((((((((((((((((((((((\r\n", name);
		OutputDebugString(str);
#endif
	}
	else  {
#ifdef WIN32
	#ifdef _DEBUG
		sprintf(str, "%s: Average time between calls is %.6lf milliseconds (((((((((((((((((((((((((((((((((((((((\r\n", name, (double)totaldt / (double)count);
		OutputDebugString(str);
		sprintf(str, "%s: mindt %ld ms, maxdt = %ld ms (((((((((((((((((((((((((((((((((((((((\r\n", name, mindt, maxdt);
		OutputDebugString(str);
	#endif
#else
	//#ifdef _DEBUG
		sprintf(str, "%s: Average time between calls is %.6f milliseconds\r\n", name, (double)totaldt / (double)count);
		OutputDebugString(str);
		sprintf(str, "%s: mindt %ld ms, maxdt = %ld ms\r\n", name, mindt, maxdt);
		OutputDebugString(str);
	//#endif
#endif
	}


}


/******************************************************************************

******************************************************************************/

void AverageTimer::update(void)  {

//#ifdef WIN32
	now = timeGetTime();

	if (lastTime == 0)  {
		lastTime = now;
		return;						// don't count the first one
	}

	dt = now - lastTime;
	lastTime = now;
	totaldt += dt;
	count++;

	maxdt = MAX(maxdt, dt);
	mindt = MIN(mindt, dt);

//#endif

}

/******************************************************************************
	returns the average ms for this timer
******************************************************************************/

double AverageTimer::getAverage(void)  {
	if (count==0)  {
		return 0;
	}
	else  {
		return (double)totaldt / (double)count;
	}
}

/******************************************************************************

******************************************************************************/

double AverageTimer::getms(void)  {
	if (count>0)  {
		return ((double)totaldt / (double)count);
	}
	else  {
		return 0.0;
	}
}

/******************************************************************************

******************************************************************************/

void AverageTimer::reset(void)  {
	totaldt = 0;
	count = 0;
	lastTime = 0;
	maxdt = 0;
	mindt = 0xffffffff;
	now = 0;
	dt = 0;
	return;
}


