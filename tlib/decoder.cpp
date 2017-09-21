
#include <assert.h>
#include <memory.h>
#include <float.h>

#ifdef WIN32
#else
	#ifdef __MACH__
		#include <minmax.h>
	#else
		#include <limits.h>			// for INT_MAX
	#endif
#endif

#include <glib_defines.h>
#include <utils.h>
#include <minmax.h>
#include <decoder.h>
#include <comglobs.h>
#include <globals.h>
#include <oldsdirs.h>

#include <tglobs.h>

int Decoder::instances = 0;
int Decoder::abs_tick = 0;
bool Decoder::decoderlog = false;
bool Decoder::bini = false;

/*********************************************************
	constructor
*********************************************************/

Decoder::Decoder(int _segments, Course *_course, RIDER_DATA &_rd, int _id)  {

	instances++;
#ifdef _DEBUG
	if (instances>2)  {
		bp = 1;
	}
#endif

	//do_export = false;

	instance = instances;
#ifdef _DEBUG
	rel_tick = 0;
#endif

	abs_tick = 0;

	ant_speed = 0.0f;
	ant_cadence = -1.0f;
	ant_cadence_only = -1.0f;
	ant_wheel_rpm = -1.0f;
	ant_hr = -1.0f;

	armed = 0;
	armed_period = 0;
	armed_time = 0L;
	armed_rpm = 0.0f;
	packet_error3 = 0;
	packet_error4 = 0;

	ramp = 0;
	dly = MINDLY;

	shiftflag = false;
	ss = NULL;
	course = _course;
	rd = _rd;
	id = _id;
	do_drag = false;
	hrvalid = false;
	can_do_drag_factor = false;

	startTime = timeGetTime();
	pausedTime = 0;				// accumulates paused time
	pauseStartTime = 0;

	resets = 0;

#ifdef _DEBUG
	do_amplify = false;
#endif

	if (do_drag)  {
		//watts_factor = rd.watts_factor;		// user->get_watts_factor();
	}

	keydownfunc = NULL;			// function pointer to application level function
	keyupfunc = NULL;				// function pointer to application level function
	object = NULL;					// pointer to the object that has the functio pointer

	min_hr = INT_MAX;
	min_rpm = FLT_MAX;
	min_mph = FLT_MAX;
	min_watts = FLT_MAX;
	min_ss = FLT_MAX;
	max_ss = -FLT_MAX;
	min_lss = FLT_MAX;
	max_lss = -FLT_MAX;
	min_rss = FLT_MAX;
	max_rss = -FLT_MAX;
	min_lwatts = FLT_MAX;
	max_lwatts = -FLT_MAX;
	avg_lwatts = 0.0f;
	min_rwatts = FLT_MAX;
	max_rwatts = -FLT_MAX;
	avg_rwatts = 0.0f;
	min_lata = INT_MAX;
	max_lata = -INT_MAX;
	avg_lata = 0.0f;
	min_rata = INT_MAX;
	max_rata = -INT_MAX;
	avg_rata = 0.0f;
	total_lwatts = 0.0f;
	total_rwatts = 0.0f;
	min_pp = FLT_MAX;
	max_pp = -FLT_MAX;
	min_wpkg = FLT_MAX;					// watts per kg
	max_wpkg = -FLT_MAX;					// watts per kg
	avg_wpkg = 0.0f;
	total_wpkg = 0.0f;
	wpkg = 0.0f;

	memset(&mma, 0, sizeof(mma));

	accum_watts = 0.0f;								// used for ergvid
	accum_rpm = 0.0f;								// used for ergvid
	accum_tdc = 0;
	accum_hr = 0.0f;								// used for ergvid
	accum_kph = 0.0f;								// used for ergvid

	accum_watts_count = 0;							// used for ergvid
	accum_rpm_count = 0;							// used for ergvid
	accum_hr_count = 0;								// used for ergvid
	accum_kph_count = 0;							// used for ergvid

#ifdef WIN32
	ft = new fastTimer("Decoder");
#endif
	
	rawspeed = 0.0f;

	ss = new SS();

	logg = NULL;
	lastkeys = 0;
	keys = 0;
	accum_keys = 0;

#ifdef WIN32
	//avgtimer = NULL;
	at = new AverageTimer("SS");
#endif
	
	packets = 0;
	memset(&meta, 0, sizeof(meta));

	formula = NULL;
	formula = new Formula(rd.ftp);

	reset();

	drag_aerodynamic = rd.drag_aerodynamic;

	if (drag_aerodynamic <= 0.0)  {
		drag_aerodynamic = 8.0;
	}

	drag_tire = rd.drag_tire;

	if (drag_tire <= 0.0)  {
		drag_tire = .006;
	}

	if (decoderlog)  {
#ifdef _DEBUG
		if (id==0)  {
			bp = 3;
		}
#endif
		char str[256];
#ifdef DOGLOBS
		sprintf(str, "%s%sdecoder%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
		sprintf(str, "%s%sdecoder%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
		logg = new Logger(str);
		if (logg) logg->write(10,0,1,"decoder: started logging\n");
	}
	else  {
		logg = NULL;
	}

	kgs = 0;
	us = 0;
	lastus = 0;
	dus = 0;
	totalMS = 0;
	if (logg)  {
		logg->write(10,0,1,"decoder: 1\n");
		logg->write(10,0,1,"decoder: course = %s\n", course?"exists":"null");
	}

	/*
	if ((unsigned int)&course == 0xcdcdcdcd)  {
		//throw(fatalError(__FILE__, __LINE__));
		throw("xyzzy");
	}
	*/

	if (course)  {
		if (logg)  {
			logg->write(10,0,1,"decoder: 1a, id = %d\n", id);
		}
		total_miles = (float) (METERSTOMILES*course->get_total_meters());
		if (logg) logg->write(10,0,1,"decoder: 1b\n");
	}
	else  {
		total_miles = 0;
	}

	lbs = 0;
	logging = 0;
	dms = 0;

	if (logg) logg->write(10,0,1,"decoder: x\n");
	bp = 0;

}


/*********************************************************

*********************************************************/

Decoder::~Decoder(void)  {

	instances--;
#ifdef _DEBUG
	if (instances<0)  {
		bp = 1;
	}
#endif

	keydownfunc = NULL;			// function pointer to application level function
	keyupfunc = NULL;				// function pointer to application level function
	object = NULL;					// pointer to the object that has the functio pointer

	DEL(formula);

#ifdef WIN32
	DEL(ft);
	DEL(at);
#endif

#ifdef _DEBUG
	sprintf(string, "\n\nDecoder: received %ld packets\n\n", packets);
	OutputDebugString(string);
#endif

	DEL(logg);
	DEL(ss);
}




void Decoder::logmeta(void)  {
/*
	if (logg)
		logg->write("%4ld %6.2f %6.2f %8.2f\n", 
			meta.time, 
			meta.rpm, 
			meta.mph, 
			meta.watts);
*/
	return;
}

/*************************************************************************
	compute pulsepower
*************************************************************************/

void Decoder::computePP(void)  {

/*
computrainer:

   3 2 1 0		bits

	x x x x
   | | | |
   | | |  -------- 1 = blink, 0 = solid
	| |  ---------- 1 = 'E'
	|	------------ 1 = below limit
	 -------------- 1 = above limit

	if bits 2&3 set, no sensor is installed


*/

	if ( (meta.hrflags & 0x0c) == 0x0c)  {
		// return if bits 2 & 3 set (no sensor plugged in)
#ifdef _DEBUG
		bp = 1;
#endif
		return;
	}

	ppd = meta.hr * lbs;

	if (ppd > 0.0)  {
		meta.pp = 1000.0f * .734f * meta.watts * 60.0f / ppd;
	}
	else  {
		meta.pp = 0.0f;
	}

	return;
}							// computePP()

/*************************************************************************

*************************************************************************/

void Decoder::reset_averages(void)  {

	total_wpkg = 0.0f;
	totalLATA = 0.0f;
	totalRATA = 0.0f;

	totalMPH = 0.0f;
	totalWatts = 0.0f;
	totalRPM = 0.0f;
	total_pedal_rpm = 0.0f;
	totalHR = 0.0f;
	totalLSS = 0.0f;
	totalRSS = 0.0f;
	totalSS = 0.0f;
	totalPP = 0.0f;

	hravgcounter = 0;
	inzonecount = 0;
	averageCounter = 0;


	avg_lwatts = 0.0f;
	avg_rwatts = 0.0f;
	avg_lata = 0.0f;
	avg_rata = 0.0f;

	meta.averageMPH = 0.0f;
	meta.averageWatts = 0.0f;
	meta.averageRPM = 0.0f;
	meta.averageHR = 0.0f;
	meta.averagePP = 0.0f;
	meta.average_lss = 0.0f;
	meta.average_rss = 0.0f;
	meta.average_ss = 0.0f;


	max_ss = 0.0f;
	max_lss = 0.0f;
	max_rss =  0.0f;
	max_lwatts =  0.0f;
	max_rwatts =  0.0f;
	max_lata =  0;
	max_rata =  0;

	meta.peakMPH = 0.0f;
	meta.peakWatts = 0.0f;
	meta.peakHR = 0.0f;
	meta.peakRPM = 0.0f;
	meta.peakPP = 0.0f;

	if (ss)  {
		ss->reset_averages();
	}
	
	if (formula)  {
		formula->reset();
	}
}

/*********************************************************

*********************************************************/

void Decoder::reset(void)  {

	startMiles = 0.0f;
	//startTime = 0L;

	ssok = false;				// ss is now ok for finding max's & min's
	//ss_found = false;
	if (formula)  {
		formula->reset();
	}
	fill_in_mma();

	hrvalid = false;

	min_rpm = FLT_MAX;
	min_mph = FLT_MAX;
	min_watts = FLT_MAX;
	min_ss = FLT_MAX;
	min_lss = FLT_MAX;
	min_rss = FLT_MAX;
	min_lwatts = FLT_MAX;
	min_rwatts = FLT_MAX;
	min_lata = INT_MAX;
	min_rata = INT_MAX;

	max_ss = -FLT_MAX;
	max_lss = -FLT_MAX;
	max_rss = -FLT_MAX;
	max_lwatts = -FLT_MAX;
	max_rwatts = -FLT_MAX;
	max_lata = -INT_MAX;
	max_rata = -INT_MAX;

	avg_lwatts = 0.0f;
	avg_rwatts = 0.0f;
	avg_lata = 0.0f;
	avg_rata = 0.0f;


	resets++;

	rawspeed = 0.0f;
	meters = 0.0f;
	mps = 0.0f;
	wind = 0.0f;
	wpkg = 0.0f;

	dseconds = 0.0;
	lastseconds = 0.0;
	dt = 0.0;

	ppd = 0;

	hours = 0;
	minutes = 0;
	seconds = 0;
	tenths = 0;

	flags = 0;
	status = 0;
	ms = 0L;
	//period = 0L;
	losig = 0;
	hisig = 0;
	lastmph = 0;
	polarbit = false;

	hbstatus = 0;
	slip = 0;
	raw_rpm = 0.0f;
	slipflag = false;
	version = 0;

	draft_wind = 0;
	weight = 0;
	targetpower = 0;

	voltsCount = 0;
	supplyVoltage = 0;
	amps = 0;
	caltime = 0;

	started = false;
	finished = false;
	finishEdge = false;
	paused = false;

	reset_averages();

	memset(&meta, 0, sizeof(meta));				// tlm040115: added to make ergo charts reset correctly

#ifndef NEWUSER
        if (user)  {
         meta.minhr = (unsigned short)user->data.lower_hr;
         meta.maxhr = (unsigned short)user->data.upper_hr;
        }
        else  {
         meta.minhr = 40;
         meta.maxhr = 200;
        }
#else
	meta.minhr = (unsigned short)rd.lower_hr;
	meta.maxhr = (unsigned short)rd.upper_hr;
#endif


//#ifdef VELOTRON
	// probably want to reset ss here also, but only computrainer was listed in bug report
	//ss->reset();
//#else
//	ss->reset();
//#endif

	return;
}

/*********************************************************

*********************************************************/

void Decoder::fill_in_mma(void)  {

	// fill in this structure so that reports can still be printed after a reset:

	mma.calories = meta.calories;
	mma.meters = (float)(MILESTOMETERS * meta.miles);							// meters pedaled after started
	mma.ms = meta.time;								// miliseconds while started

	mma.min_hr = (float)min_hr;
	mma.max_hr = meta.peakHR;
	mma.avg_hr = meta.averageHR;

	mma.min_rpm = min_rpm;

	mma.max_rpm = meta.peakRPM;
	mma.avg_rpm = meta.averageRPM;
	mma.min_watts = min_watts;
	mma.max_watts = meta.peakWatts;
	mma.avg_watts = meta.averageWatts;
	mma.min_lwatts = min_lwatts;
	mma.max_lwatts = max_lwatts;
	mma.avg_lwatts = avg_lwatts;
	mma.min_rwatts = min_rwatts;
	mma.max_rwatts = max_rwatts;
	mma.avg_rwatts = avg_rwatts;

	mma.min_ss = min_ss;
	mma.max_ss = max_ss;
	mma.avg_ss = meta.average_ss;

	mma.min_lss = min_lss;
	mma.max_lss = max_lss;
	mma.avg_lss = meta.average_lss;
	mma.min_rss = min_rss;
	mma.max_rss = max_rss;
	mma.avg_rss = meta.average_rss;
	mma.min_lata = (float)min_lata;
	mma.max_lata = (float)max_lata;
	mma.avg_lata = avg_lata;
	mma.min_rata = (float)min_rata;
	mma.max_rata = (float)max_rata;
	mma.avg_rata = avg_rata;
	mma.min_mps = (float)(MPH_TO_METERS_PER_SEC * min_mph);
	mma.max_mps = (float)(MPH_TO_METERS_PER_SEC * meta.peakMPH);
	mma.avg_mps = (float)(MPH_TO_METERS_PER_SEC * meta.averageMPH);						// meters per second
	mma.min_pp = min_pp;
	mma.max_pp = meta.peakPP;
	mma.avg_pp = meta.averagePP;
	mma.min_wpkg = min_wpkg;
	mma.max_wpkg = max_wpkg;
	mma.avg_wpkg = avg_wpkg;

	return;
}


/*********************************************************
	sets mins, maxes to current data when start button is
	hit.
*********************************************************/

void Decoder::set_minmax(void)  {
#ifndef NEWUSER
	min_wpkg = (float) (meta.watts / (TOKGS*user->data.lbs) );
#else
	min_wpkg = (float) (meta.watts / (rd.pd.kgs) );
#endif

	min_lwatts = meta.lwatts;
	min_rwatts = meta.rwatts;
	min_watts = meta.watts;
	min_hr = (int)meta.hr;
	min_rpm = meta.rpm;
	min_ss = meta.ss;
	min_lss = meta.lss;
	min_rss = meta.rss;
	min_lata = meta.lata;
	min_rata = meta.rata;
	min_pp = meta.pp;

#ifndef NEWUSER
	max_wpkg = (float) (meta.watts / (TOKGS*user->data.lbs) );
#else
	max_wpkg = (float) (meta.watts / rd.pd.kgs );
#endif

	max_lwatts = meta.lwatts;
	max_rwatts = meta.rwatts;

	mma.max_watts = meta.watts;
	meta.peakWatts = meta.watts;

	mma.max_hr = meta.hr;
	meta.peakHR = meta.hr;

	mma.max_rpm = meta.rpm;
	meta.peakRPM = meta.rpm;

	max_ss = meta.ss;
	max_lss = meta.lss;
	max_rss = meta.rss;
	max_lata = meta.lata;
	max_rata = meta.rata;
	max_pp = meta.pp;

	return;
}

/**********************************************************************************

**********************************************************************************/

void Decoder::set_watts_factor(float _f)  {
	rd.watts_factor = _f;
	return;
}

/*************************************************************************
	entry:
		hrvalid is setup
		meta values setup
		wpkg setup

*************************************************************************/

void Decoder::peaks_and_averages(void)  {

#ifdef _DEBUG
	assert(started);
	assert(!finished);
	assert(!paused);
#endif

	//---------------------
	// compute averages:
	//---------------------

	totalMPH += meta.mph;
	totalWatts += meta.watts;
	totalRPM += meta.rpm;
	totalPP += meta.pp;
	total_lwatts += meta.lwatts;
	total_rwatts += meta.rwatts;
	totalLATA += meta.lata;
	totalRATA += meta.rata;
	total_wpkg += wpkg;

	averageCounter++;

	/*
		1)	A HR of zero is still used for In-Zone HR calculation, but now it's like it
			was before where if HR drops to zero the number begins to drop. If zero HR is
			seen, lock In-zone value and resume at that same number when HR resumes.

		2)	Zero HR should not change average HR.
	*/

/*
DWORD in_zone_ms;
DWORD above_zone_ms;
DWORD below_zone_ms;
DWORD last_zone_time;
DWORD zone_now;
*/

	if (hrvalid)  {
		if (meta.hr > 0.0f)  {
			//zone_now = timeGetTime();
			hravgcounter++;
			totalHR += meta.hr;
			meta.averageHR = (float) (totalHR / hravgcounter);

#ifndef NEWUSER
			if (meta.hr >= user->data.lower_hr && meta.hr <= user->data.upper_hr)  {
				inzonecount++;
				//in_zone_ms += dms;
			}
			else if (meta.hr > user->data.upper_hr)  {
				//above_zone_ms += dms;
			}
			else if (meta.hr < user->data.lower_hr)  {
				//below_zone_ms += dms;
			}
#else
			if (meta.hr >= rd.lower_hr && meta.hr <= rd.upper_hr)  {
				inzonecount++;
				//in_zone_ms += dms;
			}
			else if (meta.hr > rd.upper_hr)  {
				//above_zone_ms += dms;
			}
			else if (meta.hr < rd.lower_hr)  {
				//below_zone_ms += dms;
			}
#endif

			meta.inZoneHr = 100.0f * ((float)inzonecount/(float)hravgcounter);			// percent in zone
			//last_zone_time = zone_now;
		}
	}

	meta.averageMPH = (float) (totalMPH / averageCounter);
	meta.averageWatts = (float) (totalWatts / averageCounter);
	meta.averageRPM = (float) (totalRPM / averageCounter);
	meta.averagePP = (float) (totalPP / averageCounter);

	avg_lwatts = (float) total_lwatts / averageCounter;
	avg_rwatts = (float) total_rwatts / averageCounter;
	avg_lata = (float) totalLATA / averageCounter;
	avg_rata = (float) totalRATA / averageCounter;

	avg_wpkg = (float) (total_wpkg / averageCounter);

	//---------------------
	// compute maxima:
	//---------------------

	meta.peakMPH = max(meta.peakMPH, meta.mph);
	meta.peakWatts = max(meta.peakWatts, meta.watts);
	meta.peakHR = max(meta.peakHR, meta.hr);
	meta.peakRPM = max(meta.peakRPM, meta.rpm);

	max_wpkg = max(max_wpkg, wpkg);

	//---------------------
	// compute minima:
	//---------------------

	min_hr = min(min_hr, (int)(.5f + meta.hr) );
	min_rpm = min(min_rpm, meta.rpm);
	min_mph = min(min_mph, meta.mph);
	min_watts = min(min_watts, meta.watts);
	if (wpkg > 0.0f)  {
		min_wpkg = min(min_wpkg, wpkg);
	}
	else  {
		min_wpkg = 0.0f;
	}

	if (ssok)  {
		min_ss = min(min_ss, meta.ss);
		min_lss = min(min_lss, meta.lss);
		min_rss = min(min_rss, meta.rss);
		min_lwatts = min(min_lwatts, meta.lwatts);
		min_rwatts = min(min_rwatts, meta.rwatts);
		min_lata = min(min_lata, meta.lata);
		min_rata = min(min_rata, meta.rata);
		min_pp = min(min_pp, meta.pp);
		min_wpkg = min(min_wpkg, wpkg);

		max_ss = max(max_ss, meta.ss);
		max_lss = max(max_lss, meta.lss);
		max_rss = max(max_rss, meta.rss);
		max_lwatts = max(max_lwatts, meta.lwatts);
		max_rwatts = max(max_rwatts, meta.rwatts);
		max_lata = max(max_lata, meta.lata);
		max_rata = max(max_rata, meta.rata);
		max_pp = max(max_pp, meta.pp);
	}
	else  {
		if (meta.lss>0.0f)  {
			if (meta.rss>0.0f)  {
				if (meta.ss>0.0f)  {
					if (meta.lwatts > 0.0f)  {
						if (meta.rwatts > 0.0f)  {
							if (meta.lss < 99.999f)  {
								if (meta.rss < 99.999f)  {
									if (meta.ss < 99.999f)  {
										ssok = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// zone
	return;
}

/*************************************************************************

*************************************************************************/
/*
void Decoder::set_course(Course *_course)  {
	course = _course;
	init_course();
	return;
}
*/

/*************************************************************************

*************************************************************************/

void Decoder::init_course(void)  {
	/*
	if (course)  {
		switch(course->type)  {
			case Course::ERGO:
				totalLen = course->getTotalLen();		// totalLen is in minutes
				totalMS = (DWORD)(totalLen*60*1000);
				break;
			case Course::THREED:
				totalLen = (float)(course->getTotalLen() / 5280.0);		// get total len in miles
				break;
			case Course::NONERGO:
				totalLen = (float)(course->getTotalLen() / 5280.0);		// totalLen is in miles
				break;
			case Course::EMPTY:		// spinscan, manual charts, 3d have no course
				totalLen = 0;
				break;
			case Course::VIDEO:
				totalLen = course->getTotalMiles();
				break;
			default:
				throw(fatalError(__FILE__, __LINE__));
				break;
		}
	}
	else  {
	*/
		total_miles = 0;
		totalMS = 0;
	//}
	return;
}

/*************************************************************************

*************************************************************************/

void Decoder::log(int level, int printdepth, int reset, const char *format, ...)  {
	if (logg)
//		logg->write(level, printdepth, reset, format);
	return;
}


/**********************************************************************************

**********************************************************************************/

int Decoder::set_ftp(float _ftp)  {
	
	rd.ftp = _ftp;

	if (formula)  {
		formula->set_ftp(_ftp);
		return 0;
	}

	return 1;
}

/******************************************************************************************************************

******************************************************************************************************************/

void Decoder::compute_armed_rpm(void)  {

	#ifdef _DEBUG
		rel_tick++;
	#endif

	if (instance==1)  {
		abs_tick++;
		#ifdef _DEBUG
		bp = 8;
		#endif
	}
	#ifdef _DEBUG
	else  {
		bp = 7;
	}
	#endif

	period++;

	if(period > MAXDLY) {        // been 2.5 secs since a lh event
		period = MAXDLY;
		armed_period = armed_dwell = 0;                              // so jam results to zero
		armed_rpm = 0.0f;
		armed_time = 0L;                                // this is the phase, probably want to zero it
	}

	ramp =  (ramp < (MAXDLY)) ? ramp + 1 : MAXDLY;
	dly = (dly > (MINDLY)) ? dly - 1 : MINDLY;

	if(dly < ramp) {
		dly = ramp;
		armed = 1;
	}

	if(accum_tdc) {
		if(armed) {
			armed_time = abs_tick;                // absolute time for phase comparison
			armed_period = period;
			armed_dwell = last_hi_time;
			armed_rpm = 60.0f*1000.0f/(armed_period * TICK_PERIOD);  // I think this can never be div by zero
			armed = 0;
			period = 0;
		}
		last_hi_time = period;
		ramp = 0;
	}

	return;
}                                                        // compute_armed_rpm()


/**********************************************************************************
	used for ergvid
**********************************************************************************/

void Decoder::seed_avgs(void)  {

	accum_watts = meta.watts;
	accum_rpm = meta.rpm;

	accum_hr = meta.hr;
	accum_kph = (float) (TOKPH*meta.mph);

	accum_watts_count = 1;
	accum_rpm_count = 1;
	accum_hr_count = 1;
	accum_kph_count = 1;

	return;
}



/**********************************************************************************

**********************************************************************************/

void Decoder::set_speed(float _ant_speed)  {
	ant_speed = _ant_speed;
	return;
}

/**********************************************************************************
	entry:
		_ant_speed is in meters / hour
**********************************************************************************/

void Decoder::set_speed_cadence(float _ant_wheel_rpm, float _ant_speed_meters_per_hour, float _ant_cadence )  {

	ant_wheel_rpm = _ant_wheel_rpm;
	ant_speed = _ant_speed_meters_per_hour / 1000.0f;					// converting to km / hr
	ant_cadence = _ant_cadence;

	#ifdef _DEBUG
		//if (ant_speed > .1f)  {
		if (ant_cadence > .1f)  {
			bp = 1;
		}
	#endif
	return;
}

/**********************************************************************************
	entry:
		_ant_speed is in meters / hour
**********************************************************************************/

void Decoder::set_cadence(float _ant_cadence_only )  {

	ant_cadence_only = _ant_cadence_only;

	#ifdef _DEBUG
		//if (ant_speed > .1f)  {
		if (ant_cadence_only > .1f)  {
			bp = 1;
		}
	#endif
	return;
}


/**********************************************************************************

**********************************************************************************/

void Decoder::set_hr(float _ant_hr)  {
	ant_hr = _ant_hr;
#ifdef _DEBUG
#endif
	return;
}

