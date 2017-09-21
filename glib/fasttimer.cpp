
#ifdef WIN32
#pragma warning(disable:4996)

#include <stdio.h>
#include <fasttimer.h>


/*************************************************************

  how to test:

	__int64 start, end, freq, dt;
	//DWORD microseconds;
	double seconds;
	fastTimer *ft = new fastTimer();
	freq = ft->getFreq();
	start = ft->read();
	Sleep(1000);
	end = ft->read();
	dt = end - start;
	seconds = (double)(end - start) / (double)freq;
	DEL(ft);

*************************************************************/

//----------------------------------------------------
//
//----------------------------------------------------

fastTimer::fastTimer(char *_name)  {
	if (_name)  {
		strncpy(name, _name, sizeof(name)-1);
	}
	else  {
		memset(name, 0, sizeof(name));
	}

	bstat = QueryPerformanceFrequency(&li);
	freq = li.QuadPart;

	reset();
}

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::reset(void)  {
	lastnow = 0;
	totaldt = 0;
	mindt = 0;
	maxdt = 0;
	cycles = 0;
	average_counter = 0L;
	totalSeconds = 0.0;
	start();							// sets basecount
	return;
}

//----------------------------------------------------
//
//----------------------------------------------------

fastTimer::~fastTimer() {

#ifdef _DEBUG

	if (cycles>0)  {
		if (name[0])  {
			sprintf(str, "%s: Average dt = %.6lf milliseconds (((((((((((((((((((((((((((((((((((((((\n", name, (1000.0 * totalSeconds) / (double)cycles);
		}
		else  {
			sprintf(str, "Average dt = %.6lf milliseconds\n", (1000.0 * totalSeconds) / (double)cycles);
		}
		OutputDebugString(str);
	}
	else  {
		sprintf(str, "%s: stop() not called\n", name);
	}


#if 0
	if (average_counter==0)  {
		sprintf(str, "%s: average() not called\n", name);
		//OutputDebugString(str);
	}
	else  {
		double d = (double)totaldt / (double)average_counter;
		d /= (double)freq;			// compute seconds
		d *= 1000;						// compute milliseconds
		sprintf(str, "%s: Average time between calls to average() is %.4lf milliseconds (((((((((((((((((((((((((((((((\n", name, d);
		OutputDebugString(str);
		double mind, maxd;

		mind = (double)mindt;
		mind /= (double)freq;
		mind *= 1000;

		maxd = (double)maxdt;
		maxd /= (double)freq;
		maxd *= 1000;

		sprintf(str, "min time between calls to average() is %.4lf milliseconds (((((((((((((((((((((((((((((((\n", mind);
		OutputDebugString(str);

		sprintf(str, "max time between calls to average() is %.4lf milliseconds (((((((((((((((((((((((((((((((\n", maxd);
		OutputDebugString(str);
	}
#endif
#endif


}

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::start(void)  {
	QueryPerformanceCounter(&li);
	basecount = li.QuadPart;
	return;
}


//----------------------------------------------------
//
//----------------------------------------------------

double fastTimer::stop(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	totalSeconds += seconds;
	cycles++;
	return seconds;
}

//----------------------------------------------------
//
//----------------------------------------------------

double fastTimer::getTotalSeconds(void)  {
	return totalSeconds;
}

//----------------------------------------------------
//
//----------------------------------------------------

unsigned long fastTimer::getCycles(void)  {
	return cycles;
}

//----------------------------------------------------
//
//----------------------------------------------------

_int64 fastTimer::read(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	return count;
}

//----------------------------------------------------
// returns average time between start & stop calls
//----------------------------------------------------

double fastTimer::getAverageSeconds(void)  {
	if (cycles==0)  {
		return 0.0;
	}
	return (totalSeconds / cycles);
}

//----------------------------------------------------
// returns the seconds since the timer was created,
// or reset
//----------------------------------------------------

double fastTimer::getSeconds(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	return seconds;
}

//----------------------------------------------------
// returns the milliseconds since the timer was created,
// or reset
//----------------------------------------------------

unsigned long fastTimer::getms(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	ms = (unsigned long) (.5 + seconds * 1000.0);
	return ms;
}

//----------------------------------------------------
//
//----------------------------------------------------

__int64 fastTimer::getFreq(void)  {
	return freq;
}

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::delay(DWORD ms)  {
	__int64 now, begin;
	double seconds;

	begin = read();

	while(1)  {
		now = read();
		seconds = (now - begin) / (double)freq;
		if (seconds >= (double)ms/1000.0)  {
			break;
		}
	}

	return;
}

//------------------------------------------
//	measures the average time between calls to
// average()
//------------------------------------------

void fastTimer::average(void)  {

	//now = read();
	QueryPerformanceCounter(&li);
	now = li.QuadPart;

	if (lastnow > 0)  {
		dt = now - lastnow;
		lastnow = now;
		totaldt += dt;
		average_counter++;
		if (dt > maxdt)  {
			maxdt = dt;
		}
		if (dt < mindt)  {
			mindt = dt;
		}
	}
	else  {
		maxdt = 0;
		mindt = 1000000000;
		lastnow = now;
		return;						// don't count the first one
	}
}

//-----------------------------------------------------
//	simulates timeGetTime()
// returns the number of milliseconds since reset()
// note: DO NOT USE AS A DROP IN REPLACEMENT FOR timeGetTime()!!!
//-----------------------------------------------------

/*
same as getms()
//DWORD timeGetTime(void)  {
DWORD getTime(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	//seconds = (double)(count - basecount) / (double)freq;
	dw = (DWORD) (.5 + 1000*(count - basecount) / (double)freq);
	return dw;
}
*/

float fastTimer::getus(void)  {				// returns the microseconds since reset()
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	//seconds = (double)(count - basecount) / (double)freq;
	us = (float) ((count - basecount) / (double)freq);
	return us;
}

double fastTimer::getseconds(void)  {				// returns the seconds since reset()
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	//seconds = (float) (((count - basecount) / (double)freq)/1000000.0);
	return seconds;
}

#else
	// linux version
#include <fasttimer.h>
#include <string.h>
#include <stdio.h>

/*************************************************************

*************************************************************/

//----------------------------------------------------
//
//----------------------------------------------------

fastTimer::fastTimer(const char *_name)  {

	if (_name)  {
		strncpy(name, _name, sizeof(name)-1);
	}
	else  {
		memset(name, 0, sizeof(name));
	}
	reset();
}

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::reset(void)  {
	lastnow = 0;
	totaldt = 0;
	mindt = 0;
	maxdt = 0;
	cycles = 0;
	average_counter = 0L;
	totalSeconds = 0.0;
	start();							// sets basecount
	return;
}

//----------------------------------------------------
//
//----------------------------------------------------

fastTimer::~fastTimer() {

	if (cycles>0)  {
		if (name[0])  {
			sprintf(str, "%s: Average dt = %.6f milliseconds (((((((((((((((((((((((((((((((((((((((\r\n", name, (1000.0 * totalSeconds) / (double)cycles);
		}
		else  {
			sprintf(str, "Average dt = %.6f milliseconds\r\n", (1000.0 * totalSeconds) / (double)cycles);
		}
		OutputDebugString(str);
		sprintf(str, "cycles = %ld\r\n", cycles);
		OutputDebugString(str);
	}
	else  {
		//sprintf(str, "%s: stop() not called\n", name);
	}


/*
	if (average_counter==0)  {
		sprintf(str, "%s: average() not called\n", name);
		//OutputDebugString(str);
	}
	else  {
		#if 0
		double d = (double)totaldt / (double)average_counter;
		d /= (double)freq;			// compute seconds
		d *= 1000;						// compute milliseconds
		sprintf(str, "%s: Average time between calls to average() is %.4lf milliseconds (((((((((((((((((((((((((((((((\n", name, d);
		OutputDebugString(str);
		double mind, maxd;

		mind = (double)mindt;
		mind /= (double)freq;
		mind *= 1000;

		maxd = (double)maxdt;
		maxd /= (double)freq;
		maxd *= 1000;

		sprintf(str, "min time between calls to average() is %.4lf milliseconds (((((((((((((((((((((((((((((((\n", mind);
		OutputDebugString(str);

		sprintf(str, "max time between calls to average() is %.4lf milliseconds (((((((((((((((((((((((((((((((\n", maxd);
		OutputDebugString(str);
		#endif
	}
*/


}

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::start(void)  {
	gettimeofday(&tvstart, &tz);
	localtime(&tvstart.tv_sec);
	start_time = tvstart.tv_sec + tvstart.tv_usec/1000000.0;		// double
	return;
}


//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::stop(void)  {

	gettimeofday(&tvstop, &tz);
	localtime(&tvstop.tv_sec);
	stop_time = tvstop.tv_sec + tvstop.tv_usec/1000000.0;

	seconds = stop_time - start_time;
	totalSeconds += seconds;					// double
	cycles++;
}


//----------------------------------------------------
//
//----------------------------------------------------

double fastTimer::getTotalSeconds(void)  {
	return totalSeconds;
}

//----------------------------------------------------
//
//----------------------------------------------------

unsigned long fastTimer::getCycles(void)  {
	return cycles;
}

#ifdef WIN32

//----------------------------------------------------
//
//----------------------------------------------------

_int64 fastTimer::read(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	return 0;
}
#endif


//----------------------------------------------------
// returns average time between start & stop calls
//----------------------------------------------------

double fastTimer::getAverageSeconds(void)  {
	if (cycles==0)  {
		return 0.0;
	}
	return (totalSeconds / cycles);
}

//----------------------------------------------------
// returns the seconds since the timer was created,
// or reset
//----------------------------------------------------

double fastTimer::getSeconds(void)  {
	/*
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	*/
	return seconds;
}

//----------------------------------------------------
// returns the milliseconds since the timer was created,
// or reset
//----------------------------------------------------

unsigned long fastTimer::getms(void)  {
	/*
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	ms = (unsigned long) (.5 + seconds * 1000.0);
	*/
	return ms;
}

//----------------------------------------------------
//
//----------------------------------------------------

//__int64 fastTimer::getFreq(void)  {
//	return freq;
//}

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::delay(DWORD ms)  {
#if 0
	__int64 now, begin;
	double seconds;

	begin = read();

	while(1)  {
		now = read();
		seconds = (now - begin) / (double)freq;
		if (seconds >= (double)ms/1000.0)  {
			break;
		}
	}
#endif

	return;
}

//------------------------------------------
//	measures the average time between calls to
// average()
//------------------------------------------

void fastTimer::average(void)  {
	/*
	//now = read();
	QueryPerformanceCounter(&li);
	now = li.QuadPart;
	*/
	if (lastnow > 0)  {
		dt = now - lastnow;
		lastnow = now;
		totaldt += dt;
		average_counter++;
		if (dt > maxdt)  {
			maxdt = dt;
		}
		if (dt < mindt)  {
			mindt = dt;
		}
	}
	else  {
		maxdt = 0;
		mindt = 1000000000;
		lastnow = now;
		return;						// don't count the first one
	}
}

//-----------------------------------------------------
//	simulates timeGetTime()
// returns the number of milliseconds since reset()
// note: DO NOT USE AS A DROP IN REPLACEMENT FOR timeGetTime()!!!
//-----------------------------------------------------

/*
same as getms()
//DWORD timeGetTime(void)  {
DWORD getTime(void)  {
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	//seconds = (double)(count - basecount) / (double)freq;
	dw = (DWORD) (.5 + 1000*(count - basecount) / (double)freq);
	return dw;
}
*/

float fastTimer::getus(void)  {				// returns the microseconds since reset()
	/*
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	//seconds = (double)(count - basecount) / (double)freq;
	us = (float) ((count - basecount) / (double)freq);
	*/
	return us;
}

double fastTimer::getseconds(void)  {				// returns the seconds since reset()
	/*
	QueryPerformanceCounter(&li);
	count = li.QuadPart;
	seconds = (double)(count - basecount) / (double)freq;
	//seconds = (float) (((count - basecount) / (double)freq)/1000000.0);
	*/
	return seconds;
}



#endif

//----------------------------------------------------
//
//----------------------------------------------------

void fastTimer::print(void)  {

	if (cycles>0)  {
		if (name[0])  {
			sprintf(str, "%s: Average dt = %.6f milliseconds (((((((((((((((((((((((((((((((((((((((\n", name, (1000.0 * totalSeconds) / (double)cycles);
		}
		else  {
			sprintf(str, "Average dt = %.6f milliseconds\n", (1000.0 * totalSeconds) / (double)cycles);
		}
	}

	// timeb_start;
	// timeb_stop;

	OutputDebugString(str);

	return;
}


