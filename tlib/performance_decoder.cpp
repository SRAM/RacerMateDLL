#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#ifndef WIN32
	#include <minmax.h>
#endif

#include <utils.h>

#include <performance_decoder.h>

/*********************************************************

*********************************************************/

performanceDecoder::performanceDecoder(Course *_course, RIDER_DATA &_rd) : Decoder(48, _course, _rd, 0)  {

	unsigned short seed;
	seed = getRandomNumber();
	srand(seed);

	manual = false;
	manual_mph = 0.0f;
	startTime = 0L;
	perfTime = 0L;
	miles = 0.0;			// for integrating, to eliminate roundoff errors using meta.miles (float)
	meta.mph = 0.0;				// for integrating, to eliminate roundoff errors using meta.mph (float)

	min_mph = frand(11.5f, 15.2f);
	max_mph = frand(29.0f, 35.2f);

	min_watts = frand(90.3f, 115.2f);
	max_watts = frand(294.1f, 435.7f);

	min_hr = frand(90.3f, 115.2f);
	max_hr = frand(145.9f, 178.3f);

	min_rpm = frand(80.6f, 91.2f);
	max_rpm = frand(124.9f, 145.4f);

	mphFilter = new LPFilter(10);
	rpmFilter = new LPFilter(12);
	wattsFilter = new LPFilter(15);
	hrFilter = new LPFilter(30);

	reset();
}



/*********************************************************

*********************************************************/

performanceDecoder::~performanceDecoder(void)  {
	DEL(mphFilter);
	DEL(rpmFilter);
	DEL(wattsFilter);
	DEL(hrFilter);
}

/**********************************************************************

**********************************************************************/

int performanceDecoder::decode(unsigned char *_packet, DWORD _ms)  {

	float f;



	if (finished)  {
		meta.grade = 0.0f;
		meta.wind = 0.0f;
		return 0;
	}


	//----------
	// mph
	//----------

	if (manual)  {
		//if (manual_mph > 0.0f)  {
		//	bp = 22;
		//}
		meta.mph = manual_mph;
	}
	else  {
		meta.mph = 0.0f;
		/*
		#if SPEED_FUNCTION==0										// random
			if (started)  {
				f = frand(-.5f, .5f);
			}
			else  {
				f = frand(-.10f, .10f);
			}
			f = mphFilter->calc(f);

			meta.mph += f;
			if (meta.mph<min_mph)  {
				meta.mph = min_mph;
			}
			if (meta.mph > max_mph)  {
				meta.mph = max_mph;
			}
			mph = meta.mph;

		#elif SPEED_FUNCTION==1										// constant
			#ifdef _DEBUG
				meta.mph = (float) (constant_mph + dmph);
				if (meta.mph < 0.0f) {
					meta.mph = 0.0f;
				}
				mph = constant_mph + dmph;
				if (mph < 0.0) {
					mph = 0.0f;
				}
			#else
				meta.mph = (float)constant_mph;
				mph = constant_mph;
			#endif
		#elif SPEED_FUNCTION==2										// mathematical function

		#endif
		*/

	}

	accum_kph += (float)(TOKPH*meta.mph);
	accum_kph_count++;

	//----------
	// watts
	//----------

	if (started)  {
		f = frand(-1.0f, 1.0f);
	}
	else  {
		f = frand(-.10f, .10f);
	}
	f = (float)wattsFilter->calc(f);

	meta.watts += f;
	if (meta.watts<min_watts)  {
		meta.watts = min_watts;
	}
	if (meta.watts > max_watts)  {
		meta.watts = max_watts;
	}

	accum_watts += meta.watts;
	accum_watts_count++;

	//----------
	// rpm
	//----------

	if (started)  {
		f = frand(-1.0f, 1.0f);
	}
	else  {
		f = frand(-.10f, .10f);
	}
	f = (float)rpmFilter->calc(f);

	meta.rpm += f;
	if (meta.rpm<min_rpm)  {
		meta.rpm = min_rpm;
	}
	if (meta.rpm > max_rpm)  {
		meta.rpm = max_rpm;
	}
	meta.pedalrpm = meta.rpm;

	accum_rpm += meta.rpm;
	accum_rpm_count++;

	//----------
	// hr
	//----------

	if (started)  {
		f = frand(-1.0f, 1.0f);
	}
	else  {
		f = frand(-.1f, .1f);
	}
	f = (float)hrFilter->calc(f);

	meta.hr += f;

	if (meta.hr<min_hr)  {
		meta.hr = min_hr;
	}
	if (meta.hr > max_hr)  {
		meta.hr = max_hr;
	}

	accum_hr += meta.hr;
	accum_hr_count++;

#ifdef VELOTRON
	meta.virtualFrontGear = 18;
	meta.virtualRearGear = 72;
	meta.gear = 93;
#endif

	//----------
	// calories
	//----------

	if (started)  {
		f = frand(0.0f, .01f);
	}
	else  {
		f = 0.0f;
	}
	meta.calories += f;


	computePP();


	//--------------
	// time:
	//--------------

	DWORD now;


	if (started && !finished)  {
		if (firststart)  {
			timeBeginPeriod(1);
			startTime = lastnow = timeGetTime();
			timeEndPeriod(1);
			firststart = false;
			meta.time = 0L;
		}
		else  {
			timeBeginPeriod(1);
			now = timeGetTime();
			timeEndPeriod(1);
			dms = now - lastnow;
			lastnow = now;
			meta.time += dms;
		}
	}

	#ifdef _DEBUG
	//avgTimer->update();			// .1 ms
	#endif

	timeBeginPeriod(1);
	now = timeGetTime();
	timeEndPeriod(1);

	if ((now-lastIntegrateTime) < 28)  {								// 1000/35 = 28 ms (taken from 3d software)
		return 0;
	}

	#ifdef _DEBUG
		//if (started)  {
		//	bp = 1;
		//}
	#endif

	dt = (now - lastIntegrateTime) / 1000.0;
	lastIntegrateTime = now;

	#ifdef _DEBUG
	//avgTimer->update();			// 33.3 ms for both mmtimer and fasttimer
	#endif

	integrate();

	#ifdef _DEBUG
	//avgTimer->update();			// 1000 ms
	#endif


	return 0;
}						// decode()




/**********************************************************************
	called for each decoded packet
**********************************************************************/

void performanceDecoder::integrate(void)  {

	if (!started)  {
		return;
	}

	if (finished)  {
		return;
	}

	if (paused)  {
		return;
	}

	//--------------------------------
	// compute averages:
	//--------------------------------

	averageCounter++;

	totalMPH += meta.mph;
	totalWatts += meta.watts;
	totalRPM += meta.rpm;
	totalHR += meta.hr;
	totalPP += meta.pp;

	meta.averageMPH = (float)(totalMPH / averageCounter);
	meta.averageWatts = (float)(totalWatts / averageCounter);
	meta.averageRPM = (float)(totalRPM / averageCounter);
	meta.averageHR = (float)(totalHR / averageCounter);
	meta.averagePP = (float)(totalPP / averageCounter);

	//--------------------------------
	// compute peaks
	//--------------------------------

	meta.peakMPH = MAX(meta.peakMPH, meta.mph);
	meta.peakWatts = MAX(meta.peakWatts, meta.watts);
	meta.peakRPM = MAX(meta.peakRPM, meta.rpm);
	meta.peakHR = MAX(meta.peakHR, meta.hr);
	meta.peakPP = MAX(meta.peakPP, meta.pp);

	//--------------------------------
	// compute the miles
	//--------------------------------

	//meta.miles += (float) (meta.mph*(dt/3600.0));		// integrate the speed to get miles
	//miles += meta.mph*(dt/3600.0);							// integrate the speed to get miles
	miles += meta.mph*(dt/3600.0);									// integrate the speed to get miles
	meta.miles = (float)miles;
	meters = (float) (MILESTOMETERS * meta.miles);

#ifdef _DEBUG
	if (meta.miles > 0.0f)  {
		bp = 1;
	}
#endif

	if (mode==0)  {
		if (miles >= total_miles)  {
			miles = total_miles;
			meta.miles = total_miles;	// tlm20030812, last logging is EXACTLY totalLen
			if (!finished)  {
#ifdef _DEBUG
				timeBeginPeriod(1);
				DWORD _now = timeGetTime();
				timeEndPeriod(1);
				perfTime = _now - startTime;
				bp = 1;
#endif
				finished = true;
				finishEdge = true;
			}
		}
	}
	else if (mode==1)  {						// ergo mode is time-based
		if (meta.time >= total_ms)  {
			if (!finished)  {
				finished = true;
				finishEdge = true;
			}
		}
	}

	// get the grade at this miles
	
	if (course)  {
		meta.grade = course->get_grade(meta.miles, &lastGrade_i);
	}
	else  {
		meta.grade = 0.0f;
	}


	return;
}										// integrate()

/**********************************************************************

**********************************************************************/

void performanceDecoder::reset(void)  {

	startTime = 0L;
	perfTime = 0L;
	miles = 0.0;			// for integrating, to eliminate roundoff errors using meta.miles (float)
	meta.mph = 0.0;				// for integrating, to eliminate roundoff errors using meta.mph (float)

	lastIntegrateTime = 0;
	lastGrade_i = 0;
	lastnow = 0;

	packets = 0;

	Decoder::reset();			// sets runSwitch to FALSE, dt to 0

//#ifdef _DEBUG
//	if (resets==2)  {
//		bp = 1;
//	}
//#endif

	meta.runSwitch = 1;
	meta.lata = 90;
	meta.rata = 90;

	//dt = DT;
	dt = 0;
	firststart = true;

	return;
}

