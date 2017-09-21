
#ifdef WIN32
	#define STRICT
	#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996)					// for vista strncpy_s
	#include <windows.h>
	#include <process.h>
	#include <mmsystem.h>
#endif

/**********************************************************************

**********************************************************************/

#include <float.h>
#include <math.h>

#include <algorithm>

#include <glib_defines.h>
#include <heartrate.h>
//#include <tglobs.h>

bool Heartrate::bini = false;

//extern "C" {

/**********************************************************************

**********************************************************************/

Heartrate::Heartrate()  {
	int i;
	int j;

	if (bini)  {
		ini = new Ini("hr.ini");
		debug = ini->getBool("options", "debug", "false", false);
	}
	else  {
		ini = NULL;
		debug = false;
	}


	for(i=0;i<PTABSIZE;i++)  {
		ptab.push_back(0.0);
	}

	restart();

	data = new SIGDATA[2];

	inZoneHr = 0.0;
	inzonecount = 0;
	minhr = 0;														// unsigned short
	maxhr = 0;			// unsigned short

	//displayfilter = new LPFilter(40);						// display filtering
	avgmin = 0.0;
	peak = false;
	peakval = 0.0;
	//totalHz = 0.0;
	totalppm = 0.0;
	averageHeartRate = 0.0;
	center = PTABSIZE / 2;
	lower_hr = 0.0;
	upper_hr = 0.0;

	sample = 0;		// total number of 5 ms samples
	filterchangecounter = 0;
	gain = 1;					// 0 is low gain, 1 is high gain

	for(i=1;i<2;i++)  {
		data[i].lastfsig = 128.0;
		data[i].maxfsig = 0.0;
		data[i].minfsig = 255.0;
		data[i].filter = new LPFilter(80);			// 40 60 80 90 100 200
		data[i].lastslope = 0.0;
		data[i].dcmav = new sMav(400) ;								// computes dc value over last 2 seconds
		data[i].ssm = new signalStrengthMeter(300);

		data[i].signalStrength = 0.0;
		data[i].avgfsig = 0.0;

		if(debug)  {
			data[i].bufsize = 4096;
			data[i].bufptr = 0;
			data[i].buf = new unsigned char[data[i].bufsize];
			for(j=0;j<data[i].bufsize;j++)  {
				data[i].buf[j] = 128;
			}
		}
	}



	errorTimer = 0;
	lastPeak = 0;

	log = NULL;

	if (debug)  {
		log = new Logger("hr.log");
		log->write(0,0,0,"%10ld: started\n", timeGetTime());
	}

	beep = NULL;
	averageDownSlope = 0;
	beginDownSignal = 0;
	tperiod = 0;
	rawppm = 0;
	peakHR = 0;
}

/**********************************************************************

**********************************************************************/

Heartrate::~Heartrate()  {
	int i;

	if (debug)  {
		for(i=1;i<2;i++)  {
			log->write(10,0,0,"avgfsig %d = %.2lf\n", i, data[i].avgfsig);
		}
	}


	for(i=1;i<2;i++)  {
		delete data[i].filter;
		delete data[i].dcmav;
		delete data[i].ssm;

		if (debug)  {
			delete[] data[i].buf;
		}
	}

	delete[] data;

	DEL(ini);
	DEL(beep);
	DEL(log);
}


/**********************************************************************
  data comes in every .005 sec

  exit:
	filteredHz is set

**********************************************************************/

void Heartrate::input(unsigned short losig, unsigned short hisig)  {
	int i;

	nptimer++;
	sample++;
	peak = false;

	data[0].rawsig = losig;
	data[1].rawsig = hisig;

	if (sample < 20)  {
		for(i=1;i<2;i++)  {
			data[i].fsig = data[i].filter->calc((double)data[i].rawsig);

			if (debug)  {
				data[i].buf[data[i].bufptr] = (unsigned char)(.5+data[i].rawsig);
				data[i].bufptr++;
				if (data[i].bufptr==data[i].bufsize) data[i].bufptr = 0;
			}
		}

		lastPeak = sample;
		return;
	}


	for(i=1;i<2;i++)  {				// only maintain high gain signal
		data[i].fsig = data[i].filter->calc((double)data[i].rawsig);
		data[i].avgfsig = data[i].dcmav->compute((float)data[i].fsig);

		if (debug)  {
			data[i].buf[data[i].bufptr] = (unsigned char) (.5 + data[i].fsig);
			data[i].bufptr++;
			if (data[i].bufptr==data[i].bufsize) data[i].bufptr = 0;
		}

		data[i].signalStrength = data[i].ssm->compute(data[i].fsig);



	}




	if (findpeak())  {					// uses data[gain].fsig

		// a peak has been found, but may not be valid

		pulses++;							// count peaks
		peak = true;
		nptimer = 0;

		period = thisPeak - lastPeak;
		tperiod = .005*period;

		if (tperiod > 0.0)  {
			hz = 1.0/tperiod;
			rawppm = 60.0*hz;
		}
		else  {
			hz = rawppm = 0.0;
		}

		lastPeak = thisPeak;


		if (valid())  {
			validpulses++;
			nvtimer = 0;
			noiseflag = false;
			lastgoodpeaktime = timeGetTime();
			
			insert();

			if (validpulses >= PTABSIZE)  {
				filteredHz = ptab[center];
				filteredppm = 60*filteredHz;

			}

			if (filteredppm==0)  {
				bp = 1;
			}

			if (filteredppm > peakval)  {
				peakval = filteredppm;
			}

			totalppm += filteredppm;
			averageHeartRate = totalppm / sample;
		}
		else  {										// pulse, but not valid
			if (!noiseflag)  {
				if (pulses >= PTABSIZE)  {
					now = timeGetTime();
					if (now - lastgoodpeaktime > 8000)  {
						noiseflag = true;
						if (debug) log->write(0,0,0,"\n%10ld: NOISEFLAG after 8 seconds of no valid pulse\n", now);
						eOnTime = now;
					}
				}
			}
			else  {
				now = timeGetTime();
				if ( (now - eOnTime) >= 8000)  {
					restart();
				}
			}
		}
	}							// if (findpeak()
	else  {
		// no pulse found
		if (nptimer == 200*8)  {					// no pulse for 5 seconds?
			if (debug)  {
			}
			restart();
		}
	}

	for(i=1;i<2;i++)  {
		data[i].lastfsig = data[i].fsig;
	}

	return;
}




/**********************************************************************

  called every 5 ms

	entry:
		fsig is setup

  exit:
	returns true if a (bottom) peak has been found

**********************************************************************/

BOOL Heartrate::findpeak(void)  {

	data[gain].slope = data[gain].fsig - data[gain].lastfsig;

	data[gain].threshold = data[gain].avgfsig - 1.0;

	if (data[gain].slope > 0)  {
		upcount++;

		if (data[gain].lastslope <= 0)  {
			if (data[gain].fsig < data[gain].threshold )  {
				if (downcount > 20)  {
					data[gain].lastslope = data[gain].slope;
					downcount = 0;

					thisPeak = sample;
					return TRUE;
				}
			}
		}

		downcount = 0;
	}
	else  {
		downcount++;
		upcount = 0;
	}

	data[gain].lastslope = data[gain].slope;
	return FALSE;

}

/**********************************************************************
	dumps the continuously running buffer when the 'b' key is hit
**********************************************************************/


void Heartrate::dump(void)  {
	int i, j;

	if (!debug) return;

	for(i=1;i<2;i++)  {
		if (data[i].buf && log)  {

			log->write(10,0,0,"--------------------\n");

			for(j=0;j<data[i].bufsize;j++)  {
				log->write(10, 0, 0, "%d\n", (int)data[i].buf[data[i].bufptr]);
				data[i].bufptr++;
				if (data[i].bufptr==data[i].bufsize) data[i].bufptr = 0;
			}

			log->write(10,0,0,"--------------------\n");
		}
	}


	return;
}

/**********************************************************************

**********************************************************************/

void Heartrate::dumpPtab(void)  {
	int i;

	if (!debug) return;

	for(i=0; i<PTABSIZE; i++)  {
		if (i==center)  {
			log->write(0,0,0,"(%6.2lf) ", 60*ptab[i]);
		}
		else  {
			log->write(0,0,0,"%6.2lf ", 60*ptab[i]);
		}
	}
	log->write(0,0,0,"\n");
	return;
}


/**********************************************************************

	called when a peak has been found about every 500 ms. (more or less)

	entry:
		data[].fsig
		data[].slope
		data[].rawsig

**********************************************************************/

BOOL Heartrate::valid(void)  {
	double diff;

	if (rawppm > 250)  {
		strcpy(noisecode, "E");			// E>
		if (debug) log->write(0,0,0,"%10ld: noise: rawppm > 250 ... %.02lf\n", timeGetTime(), rawppm);
		return FALSE;
	}
	else if (rawppm < 40 )  {
		if (pulses > PTABSIZE)  {
			strcpy(noisecode, "E");			// E<
		}
		if (debug) log->write(0,0,0,"%10ld: noise: rawppm < 40 ... %.02lf\n", timeGetTime(), rawppm);
		return FALSE;
	}
	else if (pulses>PTABSIZE)  {
		if (filteredppm != 0)  {
			diff = rawppm - filteredppm;
			if (fabs(diff) > 15)  {
				strcpy(noisecode, "E");
				if (debug) log->write(0,0,0,"%10ld  noise (diff): rawppm = %.02lf, filteredppm = %.02lf\n", timeGetTime(), rawppm, filteredppm);
				return FALSE;
			}
		}
	}

	return TRUE;

}


/**********************************************************************
	inserts hz into ptab/sorttab and computes filteredHz
**********************************************************************/

void Heartrate::insert(void)  {
	static int toggle = 0;

	if (hz > ptab[center])  {				// shift down
		ptab.pop_front();
		ptab.push_back(hz);
	}
	else if (hz < ptab[center])  {		// shift up
		ptab.pop_back();
		ptab.push_front(hz);
	}
	else  {				// hz == ptab[center]
		toggle ^= 1;
		if (toggle)  {
			ptab.pop_front();
			ptab.push_front(hz);
		}
		else  {
			ptab.pop_back();
			ptab.push_back(hz);
		}
	}

	sort(ptab.begin(), ptab.end());
	if (debug) log->write(0,0,0,"%10ld  %6.2lf (%.2lf) -> ", timeGetTime(), 60*hz, data[1].signalStrength);
	dumpPtab();

	return;
}


/****************************************************************************************


****************************************************************************************/

void Heartrate::restart(void)  {
	int i;

	downcount = 0;
	upcount = 0;
	hz = 0.0;
	nptimer = 0;
	nvtimer = 0;

	filteredHz = 0;
	filteredppm = 0;
	noiseflag = false;
	noisecode[0] = 0;
	lastgoodpeaktime = timeGetTime();
	pulses = 0;
	validpulses = 0;
	for(i=0;i<PTABSIZE;i++)  {
		ptab[i] = 0.0;
	}

	return;
}

//}			// extern "C" {

