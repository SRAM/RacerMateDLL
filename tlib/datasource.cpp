
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
#endif

#include <assert.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#ifndef WIN32
	#include <minmax.h>
#endif

#if 0			//zzz
#include <globals.h>
#include <tdefs.h>
#include <utils.h>
#include <beepsound.h>

#include <tglobs.h>
#endif

#include <datasource.h>

bool dataSource::gblog = false;

/**********************************************************************
	constructor 1
**********************************************************************/

dataSource::dataSource(RIDER_DATA &_rd, Course *_course, Bike *_bike, LoggingType _logging_type)  {

	init();

	rd = _rd;
	course = _course;
	bike = _bike;
	logging_type = _logging_type;



	if (gblog)  {
		/*
		// let RTD or the subclasses create the log files! There will be multiple instances!!
		char str[256];
		sprintf(str, "%s%sds%d.log", dirs[DIR_DEBUG], FILESEPSTR, id);
		log = new Logger(str);													// subclasses may close this, delete it, and change the id number
		*/
		log = NULL;
	}
	else  {
		log = NULL;
	}

#ifdef _DEBUG
	if (log) log->write("dataSource: _DEBUG IS TRUE\n\n");
#else
	if (log) log->write("dataSource: _DEBUG IS FALSE\n\n");
#endif
	dataSource::reset();							// virtual call
}

/**********************************************************************
	constructor 2
**********************************************************************/

dataSource::dataSource(void)  {
	init();
	
}

/**********************************************************************
	constructor for video app
**********************************************************************/

//dataSource::dataSource(int _appcode, Course *_course)  {
#if 0
dataSource::dataSource(int _appcode)  {
	init();
	appcode = _appcode;
	//course = _course;
}
#endif


/**********************************************************************
	destructor
**********************************************************************/

dataSource::~dataSource()  {

	FCLOSE(export_stream);

	decoder = NULL;
	DEL(at);
	DEL(log);
#ifndef NEWUSER
	user = NULL;
#else
	//memset(&rd, 0, sizeof(rd));           // segfault, don't do this because of std::strings
#endif

	course = NULL;
	bike = NULL;

#if 0
	rd.pd.lastname.clear();
	rd.pd.firstname.clear();
	rd.pd.username.clear();
	rd.pd.email.clear();
	rd.pd.city.clear();
	rd.pd.country.clear();
	rd.pd.state.clear();
	rd.pd.zip.clear();
#endif
        

        bp = 0;
}




/*************************************************************************

*************************************************************************/

void dataSource::reset(void)  {
	if (decoder)  {
		//decoder->started = false;
		//decoder->finished = false;
		//decoder->reset();
	}
	return;
}

/*************************************************************************

*************************************************************************/

void dataSource::reset_averages(void)  {
	if (decoder)  {
		//decoder->started = false;
		//decoder->finished = false;
		decoder->reset_averages();
	}
	return;
}


/**********************************************************************

**********************************************************************/

void dataSource::beep(void)  {

#ifdef WIN32
	//PlaySound((const char *)beepsound, NULL, SND_ASYNC | SND_MEMORY);
#endif

	return;
}


/***********************************************************************************
	makes a METADATA packet from a VEL_LOGPACKET
***********************************************************************************/

void dataSource::makeMeta(void)  {
#if 1
	decoder->meta.miles = vlp.miles;								// float 			
	decoder->meta.time = vlp.ms;									// unsigned long
	decoder->meta.minutes = (float)decoder->meta.time / (1000.0f*60.0f);

	decoder->meta.rpm = vlp.rpm;									// unsigned char

	decoder->meta.pedalrpm = vlp.pedalrpm;						// unsigned char

	decoder->meta.averageRPM = vlp.avgrpm;						// unsigned char 	
	decoder->meta.peakRPM = vlp.maxrpm;							// unsigned char 	
	decoder->meta.hr = vlp.hr;										// unsigned char 	
	decoder->meta.averageHR = vlp.avghr;							// unsigned char 	
	decoder->meta.peakHR = vlp.maxhr;								// unsigned char 	
	decoder->meta.watts = vlp.watts;								// unsigned short 
	decoder->meta.averageWatts = vlp.avgwatts;					// unsigned short 
	decoder->meta.peakWatts = vlp.maxwatts;						// unsigned short 
	decoder->meta.mph = (float)vlp.mph/10.0f;					// unsigned short 
	decoder->meta.averageMPH = (float)vlp.avgmph/10.0f;		// unsigned short 
	decoder->meta.peakMPH = (float)vlp.maxmph/10.0f;			// unsigned short 
	decoder->meta.grade = (float)vlp.grade/10.0f;				// short 			
	decoder->meta.wind = (float)vlp.wind/10.0f;				// short 			
	decoder->meta.gear = vlp.gear;									// unsigned char 	
	decoder->meta.virtualFrontGear = vlp.frontgear;			// unsigned char 	
	decoder->meta.virtualRearGear = vlp.reargear;				// unsigned char 	

#ifdef _DEBUG
	if (decoder->meta.gear > 0)  {
		bp = 1;
	}
#endif

	decoder->meta.ss = vlp.ss;										// unsigned char 	
	decoder->meta.lss = vlp.lss;									// unsigned char 	
	decoder->meta.rss = vlp.rss;									// unsigned char 	
	decoder->meta.average_ss = vlp.avgss;						// unsigned char 	
	decoder->meta.average_lss = vlp.avglss;						// unsigned char 	
	decoder->meta.average_rss = vlp.avgrss;						// unsigned char 	

	decoder->meta.lwatts = vlp.lpower;							// unsigned char 	
	decoder->meta.rwatts = vlp.rpower;							// unsigned char 	
	decoder->meta.inZoneHr = vlp.inzone_hr;						// unsigned char 	
	decoder->meta.pp = vlp.pp;										// float 			
	decoder->meta.calories = vlp.calories;						// float 			

	// spinscan:

	decoder->meta.lata = vlp.lata;
	decoder->meta.rata = vlp.rata;

#ifdef WIN32
#ifdef VELOTRON
	decoder->ss->leftwatts = lp.lpower;
	decoder->ss->rightwatts = lp.rpower;
	decoder->ss->lata = lp.lata;
	decoder->ss->rata = lp.rata;
	decoder->ss->leftss = lp.lss;
	decoder->ss->rightss = lp.rss;
	decoder->ss->totalss = lp.ss;
	decoder->ss->average_lss = lp.avglss;
	decoder->ss->average_rss = lp.avgrss;
	decoder->ss->average_ss = lp.avgss;
#endif
#endif

	// tlm20040804+++++++++
	// for filemode spinscan:

	float thismax = -FLT_MAX;

	for(int i=0;i<24;i++)  {
		decoder->meta.bars[i] = vlp.bars[i];		//unsigned short, stored as 10 * torque
		thismax = MAX(thismax, (float)vlp.bars[i]);
	}

	thismax /= 10.0f;

	//------------------------------
	// check for rescaling here:
	//------------------------------

	if (thismax > MAX_SS_FORCE)  {
		belowcount = 0;
		abovecount++;
			
		decoder->meta.rescale = false;
		if (abovecount >= 10)  {
			MAX_SS_FORCE += 10.0;
			decoder->meta.rescale = true;
			abovecount = 0;
		}
	}
	else  {
		abovecount = 0;
			
		if (MAX_SS_FORCE > 10.0)  {
			decoder->meta.rescale = false;
			if (thismax < (MAX_SS_FORCE-10.0) )  {
				belowcount++;
				if (belowcount > 10)  {
					MAX_SS_FORCE -= 10.0;
					decoder->meta.rescale = true;
					belowcount = 0;
				}
			}
			else  {
				belowcount = 0;
			}
		}
	}

	// tlm20040804--------

	decoder->meta.tdc = vlp.tdc;
#endif

	return;
}								// makeMeta()

/**************************************************************************************

**************************************************************************************/

void dataSource::set_export(const char *_fname)  {

	if (_fname)  {
		strncpy(xprtname, _fname, sizeof(xprtname)-1);
		export_stream = fopen(xprtname, "wt");
	}

	return;
}

/**************************************************************************************

**************************************************************************************/

void dataSource::init(void)  {
	//manual = false;
	//calflag = false;

	//OutputDebugString("ds1");			// xxzz

	memset(xprtname, 0, sizeof(xprtname));
	export_stream = NULL;
	last_export_grade =	-10966.72f;			// std::numeric_limits<float>quiet_NaN;

	finish_ms = 0L;
	memset(started_date_time, 0, sizeof(started_date_time));

	recording = true;
	logging_type = NO_LOGGING;
	id = 0;
	registered = true;
	//mode = WINDLOAD;
	/*
	Physics::PHYSICS_WIND_LOAD_MODE,				// old WINDLOAD mode
	Physics::PHYSICS_CONSTANT_TORQUE_MODE,
	Physics::PHYSICS_CONSTANT_WATTS_MODE,			// old ERGO mode
	Physics::PHYSICS_CONSTANT_CURRENT_MODE,
	Physics::PHYSICS_NOMODE
	*/
	Physics::mode = Physics::WINDLOAD;

	device = DEVICE_NOT_SCANNED;
	course = NULL;
	memset(&vlp, 0, sizeof(vlp));

#ifdef WIN32
//	phwnd = 0;
#endif

	initialized = false;

	packet_error1 = 0L;
	packet_error2 = 0L;

	//bike = 0;
	bytes_out = 0;
	bytes_in  = 0;
	lastix = 0;
	//course = 0;
	file_size = 0;
	firstRecordOffset = 0;
	bp = 0;
	memset(string, 0, sizeof(string));
	memset(logfname, 0, sizeof(logfname));
	memset(export_name, 0, sizeof(export_name));
	lastbeeptime = 0;
	log = NULL;
	//DEL(log);
	log = NULL;

#if 1
	MIN_SS_FORCE = 0.0;				// 0 watts
	MAX_SS_FORCE = 10.0;				// 10 lbs
	abovecount = 0;
	belowcount = 0;
#endif

	//#ifdef _DEBUG
	//oktobreak = false;
	//#endif

	shutDownDelay = 0L;
	beepflag = false;
	anerobic_threshold = 0.0f;
	ppd = 0;
	//lbs = 0;
	eof = false;
	lasteof = false;
	eofedge = false;				// goes true once when we hit eof

	bof = false;
	lastbof = false;
	bofedge = false;				// goes true once when we hit bof
	decoder = NULL;				// children create the type of decoder that they need
	readRate = 0;
	nextReadTime = 0;
	lastReadTime = 0;
	records = 0L;
	connected = false;
	last_connected = false;
	connection_changed = false;
	bp = 0;
	resetTime = timeGetTime();
	at = NULL;
	xdelta = 0.0;
	left = 0;
	right = 0;
	cursorLeft = 0;
	cursorRight = 0;
	cursorWid = 0;
	winWid = 0;

	resetTime = timeGetTime();
	at = new AverageTimer("dataSource");
	
	return;
}


/**************************************************************************************

**************************************************************************************/

float dataSource::get_np(void)  { 
//	if (decoder) 
//		return decoder->formula->get_np();
//	else  {
		return 0.0f;
//	}
}

/**************************************************************************************

**************************************************************************************/

float dataSource::get_tss(void)  {
//	if (decoder)  {
//		return decoder->formula->get_tss();
//	}
//	else  {
		return 0.0f;
//	}
}

/**************************************************************************************

**************************************************************************************/

float dataSource::get_if(void)  {
	//if (decoder)  {
	//	return decoder->formula->get_if();
	//}
	//else  {
		return 0.0f;
	//}
}

/**************************************************************************************

**************************************************************************************/

float dataSource::get_avg_formula_watts(void)  {
	//if (decoder)  {
	//	return decoder->formula->get_avg_watts();
	//}
	//else  {
		return 0.0f;
	//}
}

/**************************************************************************************

**************************************************************************************/

int dataSource::set_x_ftp(float _ftp)  {

	rd.ftp = _ftp;

	if (decoder)  {
		decoder->set_ftp(_ftp);
		return 0;
	}

	return 1;
}

/**************************************************************************************

**************************************************************************************/

/*
void dataSource::set_course(Course *_course)  {
	course = _course;
	decoder->set_course(course);
}
*/


/**************************************************************************************

**************************************************************************************/
/*
void dataSource::set_manual_mph(float _manual_mph)  {
	if (decoder)  {
		decoder->set_manual_mph(_manual_mph);
	}
	return;
}
*/

/**************************************************************************************

**************************************************************************************/

void dataSource::set_logging_x(LoggingType _logging_type)  {
	logging_type = _logging_type;
	return;
}

/**************************************************************************************

**************************************************************************************/

void dataSource::clear_accum_tdc(void)  {
	decoder->accum_tdc = decoder->meta.tdc;
	return;
}

/**************************************************************************************

**************************************************************************************/

void dataSource::set_drag_factor(float _drag_factor)  {
	decoder->set_watts_factor(_drag_factor);
	rd.watts_factor = _drag_factor;
	return;
}

