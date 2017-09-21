
#ifdef WIN32
    #define STRICT
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <mmsystem.h>
    #include <assert.h>
    #include <math.h>
    #include <process.h>
	#include <shlobj.h>
#else
    #include <assert.h>
    #include <ctype.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <stdint.h>
    #include <stdio.h>
     #include <stdlib.h>
    #include <string.h>
    #include <sys/time.h>
    //#include <pthread.h>

//    #include <sys/timerfd.h>

    #include <sys/types.h>
    #include <sys/uio.h>
    #include <termios.h>
    #include <time.h>
    #include <unistd.h>

    #include <sys/types.h>
    #include <math.h>
    #include <sys/un.h>
    #include <sys/ioctl.h>

    //#include <unistd.h>
#endif

#include <fatalerror.h>
#include <err.h>

//#include <fileselector.h>
#include <round.h>
#include <computrainer.h>
#include <simulator.h>
#include <performance.h>
#include <velotron.h>

#include <tglobs.h>

#include "dll_globals.h"
#include "ev.h"

#ifdef LOGEV
#ifdef WIN32
    #pragma message("LOGEV is defined !!!!!!!!!!!!!!!!!!!!!")
#endif
#endif

/**********************************************************************
    constructor
**********************************************************************/

#if 1
EV::EV(int _ix)  {
    int status;

    //int ii = portname_to_ix["COM231"];

    //const char *cptr = ix_to_portname[_ix];

    //ix = _ix;
    //strncpy(portname, _portname, sizeof(portname)-1);

#ifdef WIN32
    //realportnum = _ix + 1;
#else
#endif

    ix = _ix;
    what = devices[_ix].what;
    course = devices[_ix].course;
    logging_type = devices[_ix].logging_type;
    rd = devices[_ix].rd;
    strncpy(url, devices[_ix].url, sizeof(url)-1);
    tcp_port = devices[_ix].tcp_port;

    max_lock_time = 0L;
    thread_run_count = 0L;
    shiftflag = false;
    timeouts = 0L;
    start_time = 0L;


//#ifdef EXTENDED_DLL
//#if LEVEL > 1
#if LEVEL >= DLL_FULL_ACCESS
    if (what==DEVICE_RMP)  {
        rmp = new RMP();
    }
    else  {
        rmp = NULL;
    }
    memset(perfname, 0, sizeof(perfname));
    loaded = false;
#endif


#ifdef EV_LOG_MPH
    mphstream = fopen("evmph.txt", "wt");
#endif

#ifndef WIN32
    #ifndef DO_TINYTHREAD
        done = false;
    #endif
#endif

    bp = 0;
    gp.front = 0;
    gp.rear = 0;
    formula = NULL;

    //int n = 0;

    /*
    if (strstr(_portname, "COM"))  {
        n = sscanf(_portname, "COM%d", &realportnum);
    }
    if (n != 1)  {
        if (!strstr(_portname, "/dev/"))  {
            throw(fatalError(__FILE__, __LINE__));
        }
    }
    */

    if (&devices[_ix].bike_params)  {
        if (what==DEVICE_VELOTRON)  {
            bike = new Bike(realportnum, &devices[_ix].bike_params);
            bike->set_gear(devices[_ix].bike_params.front_ix, devices[_ix].bike_params.rear_ix);					// go into default gear
        }
        else  {
            bike = NULL;
        }
    }
    else  {
        bike = NULL;
    }

#ifdef DO_TINYTHREAD
    contin = false;
    //mut = 0;
    thrd = 0;
#else
    #ifdef WIN32
        hthread = NULL;
        stop_event = 0;
    #else
        thread = 0;
    #endif
#endif

    initialized = false;

    //person_kgs = 68.0f;
    //drag_factor = 100;				// 0 to 1000%
    rd.watts_factor = 1.0f;

    grade = 0.0f;
    memset(calstr, 0, sizeof(calstr));
    memset(&ssd, 0, sizeof(ssd));

#ifdef _DEBUG
    at = new AverageTimer("ev");
    ft = new fastTimer("evft");
#endif

	 memset(EVLOGFILE, 0, sizeof(EVLOGFILE));

#ifdef LOGEV
	 sprintf(EVLOGFILE, "%s\\ev%d.log", personal_path, ix);
	 unlink(EVLOGFILE);
    logstream = fopen(EVLOGFILE, "wt");
    fprintf(logstream, "ok1\n");
    fclose(logstream);
#endif

	 if (what == DEVICE_COMPUTRAINER) {
		 //sprintf(comstr, "COM%s", portname);
		 strncpy(comstr, devices[_ix].portname, sizeof(comstr) - 1);
#ifdef LOGEV
		logstream = fopen(EVLOGFILE, "a+t");
		fprintf(logstream, "creating Computrainer on %s, url = %s, tcp_port = %d...\n", comstr, url, tcp_port);
		fclose(logstream);
#endif

		 // from internal.cpp @ 2272:   ds = new Computrainer(_client, rd, &course, &bike);

		//if (server) {
		//	ds = new Computrainer(_ix, server, rd, course, NULL, logging_type);
		//}
		//else {
			ds = new Computrainer(comstr, rd, course, NULL, logging_type, url, tcp_port);		// comstr is class member, 32 bytes, computrainer constructor 1, rtd constructor 1
		//}

#ifdef _DEBUG
    #ifdef DO_TCP_CLIENT
        if (realportnum >= CLIENT_SERIAL_PORT_BASE && realportnum <= CLIENT_SERIAL_PORT_BASE+16)  {
        //if (!strcmp(comstr, CLIENT_SERIAL_PORT_STR))  {
                bp = 3;
        }
    #endif
#endif

        ds->setShutDownDelay(10);

        /*
        float diameter = 700.0f;									// mm
        float circumference = (float) (PI * diameter);		// mm

        set_speed_cadence_sensor_params(circumference);
        //set_use_heartrate_sensor_x();
        */

        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "created Computrainer...\n");
            fclose(logstream);
        #endif
    }
    else if (what==DEVICE_VELOTRON)  {
        strncpy(comstr, devices[_ix].portname, sizeof(comstr)-1);
        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "creating Velotron...\n");
            fclose(logstream);
        #endif

        ds = new Velotron(comstr, rd, NULL, bike, logging_type);

        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "ok3\n");
            fclose(logstream);
        #endif
    }
    else if (what==DEVICE_SIMULATOR)  {
        ds = new Simulator(rd, false, course);
    }
    else if (what==DEVICE_RMP)  {
        ds = new Performance(rd, false);
    }
    else  {
        return;
    }

	if (!ds->is_initialized() )  {
		#ifdef LOGEV
			logstream = fopen(EVLOGFILE, "a+t");
			fprintf(logstream, "error: ds not initialized\n");
         fclose(logstream);
		#endif

		 DEL(ds);
        return;
    }

	#ifdef LOGEV
		logstream = fopen(EVLOGFILE, "a+t");
		fprintf(logstream, "ds was initialized\n");
      fclose(logstream);
	#endif

    ds->decoder->keydownfunc = keydownfunc;
    ds->decoder->keyupfunc = keyupfunc;
    ds->decoder->object = this;
    ds->decoder->set_id(ix+1);

    //float f = (float)drag_factor / 100.0f;
    float f = rd.watts_factor;

    ds->set_drag_factor(f);

    ds->set_grade(0.0f);								// sets computrainer::control_byte and ds->mode

    ds->decoder->set_version(0);							// unsigned short

    formula = ds->decoder->get_formula();

#ifdef WIN32
    timer_event = timeSetEvent(100, 1, TimerProc, (DWORD_PTR)this, TIME_PERIODIC);		// generates accurate 100.0 ms interrupt to run 10 hz ss filter
#endif

    status = start_collection_thread();
    if (status != 0)  {
        return;
    }

    initialized = true;

#ifdef LOGEV
	logstream = fopen(EVLOGFILE, "a+t");
    fprintf(logstream, "evx\n");
    fclose(logstream);
#endif

}								// constructor
#endif


/**********************************************************************
    _portnum starts at 1
    defaults to windload mode
    constructor
**********************************************************************/

#if 0
EV::EV(int _ix, const char *_portname, EnumDeviceType _what, RIDER_DATA _rd, Bike::PARAMS *_params, Course *_course, LoggingType _logging_type)  {
#else
EV::EV(int _realportnum, EnumDeviceType _what, RIDER_DATA _rd, Bike::PARAMS *_params, Course *_course, LoggingType _logging_type, const char *_url, int _tcp_port)  {
	ix = _realportnum - 1;

	realportnum = _realportnum;
    tcp_port = _tcp_port;
    strncpy(url, _url, sizeof(url)-1);
    //char _portname[256];
#endif

    int status;


#ifdef WIN32
	 //strcpy(_portname, devices[ix].portname);
#else
#endif

    what = _what;
    course = _course;
    logging_type = _logging_type;
    rd = _rd;

    max_lock_time = 0L;
    thread_run_count = 0L;
    shiftflag = false;
    timeouts = 0L;
    start_time = 0L;


//#ifdef EXTENDED_DLL
//#if LEVEL > 1
#if LEVEL >= DLL_FULL_ACCESS
    if (what==DEVICE_RMP)  {
        rmp = new RMP();
    }
    else  {
        rmp = NULL;
    }
    memset(perfname, 0, sizeof(perfname));
    loaded = false;
#endif


#ifdef EV_LOG_MPH
    mphstream = fopen("evmph.txt", "wt");
#endif

#ifndef WIN32
    #ifndef DO_TINYTHREAD
        done = false;
    #endif
#endif

    bp = 0;
    gp.front = 0;
    gp.rear = 0;
    formula = NULL;

    int n = 0;

	if (strstr(devices[ix].portname, "COM"))  {
		//n = sscanf(_portname, "COM%d", &realportnum);
	}
	//if (n != 1)  {
	else if (!strstr(devices[ix].portname, "/dev/"))  {
		throw(fatalError(__FILE__, __LINE__));
	}

    if (_params)  {
//#ifdef WIN32
#if 1
        bike = new Bike(realportnum, _params);
#else
#endif
        bike->set_gear(_params->front_ix, _params->rear_ix);					// go into default gear
    }
    else  {
        bike = NULL;
    }

#ifdef DO_TINYTHREAD
    contin = false;
    //mut = 0;
    thrd = 0;
#else
    #ifdef WIN32
        hthread = NULL;
        stop_event = 0;
    #else
        thread = 0;
    #endif
#endif

    initialized = false;

    //person_kgs = 68.0f;
    //drag_factor = 100;				// 0 to 1000%
    rd.watts_factor = 1.0f;

    grade = 0.0f;
    memset(calstr, 0, sizeof(calstr));
    memset(&ssd, 0, sizeof(ssd));

#ifdef _DEBUG
    at = new AverageTimer("ev");
    ft = new fastTimer("evft");
#endif

#ifdef LOGEV
	//logstream = fopen(EVLOGFILE, "wt");
	unlink(EVLOGFILE);
	logstream = fopen(EVLOGFILE, "a+t");
	fprintf(logstream, "ok1\n");
	fclose(logstream);
#endif

    if (what==DEVICE_COMPUTRAINER)  {
//#ifdef WIN32
        //sprintf(comstr, "COM%s", portname);
        strncpy(comstr, devices[ix].portname, sizeof(comstr)-1);
        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "creating Computrainer on %s...\n", comstr);
            fclose(logstream);
        #endif

        // from internal.cpp @ 2272:   ds = new Computrainer(_client, rd, &course, &bike);

        ds = new Computrainer(comstr, rd, course, NULL, logging_type, url, tcp_port);		// comstr is class member, 32 bytes, computrainer constructor 1, rtd constructor 1

#ifdef _DEBUG
    #ifdef DO_TCP_CLIENT
        if (realportnum >= CLIENT_SERIAL_PORT_BASE && realportnum <= CLIENT_SERIAL_PORT_BASE+16)  {
        //if (!strcmp(comstr, CLIENT_SERIAL_PORT_STR))  {
                bp = 3;
        }
    #endif
#endif

        ds->setShutDownDelay(10);

        /*
        float diameter = 700.0f;									// mm
        float circumference = (float) (PI * diameter);		// mm

        set_speed_cadence_sensor_params(circumference);
        //set_use_heartrate_sensor_x();
        */

        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "created Computrainer...\n");
            fclose(logstream);
        #endif
    }
    else if (what==DEVICE_VELOTRON)  {
        strncpy(comstr, devices[ix].portname, sizeof(comstr)-1);
        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "creating Velotron...\n");
            fclose(logstream);
        #endif

        ds = new Velotron(comstr, rd, NULL, bike, logging_type);

        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "ok3\n");
            fclose(logstream);
        #endif
    }
    else if (what==DEVICE_SIMULATOR)  {
        ds = new Simulator(rd, false, course);
    }
    else if (what==DEVICE_RMP)  {
        ds = new Performance(rd, false);
    }

    else  {
        return;
    }

    if (!ds->is_initialized() )  {
        DEL(ds);
        return;
    }

    ds->decoder->keydownfunc = keydownfunc;
    ds->decoder->keyupfunc = keyupfunc;
    ds->decoder->object = this;
    ds->decoder->set_id(ix+1);

    //float f = (float)drag_factor / 100.0f;
    float f = rd.watts_factor;

    ds->set_drag_factor(f);

    ds->set_grade(0.0f);								// sets computrainer::control_byte and ds->mode

    ds->decoder->set_version(0);							// unsigned short

    formula = ds->decoder->get_formula();

#ifdef WIN32
    timer_event = timeSetEvent(100, 1, TimerProc, (DWORD_PTR)this, TIME_PERIODIC);		// generates accurate 100.0 ms interrupt to run 10 hz ss filter
#endif

    status = start_collection_thread();
    if (status != 0)  {
        return;
    }

    initialized = true;

#ifdef LOGEV
	logstream = fopen(EVLOGFILE, "a+t");
    fprintf(logstream, "evx\n");
    fclose(logstream);
#endif

}								// constructor

/**********************************************************************
    destructor
**********************************************************************/

EV::~EV()  {

#ifdef LOGEV
	logstream = fopen(EVLOGFILE, "a+t");
    fprintf(logstream, "EV::destructor\n");
    fclose(logstream);
#endif

#ifdef EV_LOG_MPH
    FCLOSE(mphstream);
#endif

#ifdef WIN32
    timeKillEvent(timer_event);
#endif

    stop();

//#ifdef EXTENDED_DLL
#if LEVEL >= DLL_FULL_ACCESS
    DEL(rmp);
#endif

    bp = 0;

#ifdef DO_TINYTHREAD
	 // thread stopped in stop(), but.. just to make sure:
    contin = FALSE;
    DWORD start = timeGetTime();

    while(running)  {
        if ((timeGetTime() - start) >= 2000)  {
            break;
        }
        bp++;
    }

    if (thrd)  {
        thrd->join();
        DEL(thrd);
    }
#else
    #ifdef WIN32
        hthread = NULL;
    #endif
#endif

#ifdef _DEBUG
    DEL(at);
    DEL(ft);
    char str[32];
    sprintf(str, "timeouts = %ld\r\n", timeouts);
    OutputDebugString(str);
#endif


    DEL(ds);
    DEL(bike);


#ifdef LOGEV
	logstream = fopen(EVLOGFILE, "a+t");
    fprintf(logstream, "EV::destructor finished\r\n");
    FCLOSE(logstream);

{
		/*
		SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, personal_path);					// "C:\Users\larry\Documents"
		strcat(personal_path, "\\dll");
		if (!direxists(personal_path))  {
			CreateDirectory(personal_path, NULL);
		}
		*/

		SYSTEMTIME x;
		GetLocalTime(&x);
		char fname[64];
		char dest[260];
		strcpy(dest, personal_path);
		sprintf(fname,"\\%04d-%02d-%02d_%02d-%02d-%02d-ev.log",x.wYear, x.wMonth, x.wDay, x.wHour, x.wMinute, x.wSecond);
		strcat(dest, fname);

		if (exists(EVLOGFILE))  {
			BOOL b = CopyFile(EVLOGFILE, dest, FALSE);					// existing, new
			if (!b)  {
				Err *err = new Err();							// The filename, directory name, or volume label syntax is incorrect
				OutputDebugString(err->getString());
				OutputDebugString("\n");
				DEL(err);
			}
		}
}

#endif


}

/*************************************************************************

*************************************************************************/

void EV::set_performance_file_name_x(const char *_perfname)  {
    ds->set_performance_file_name_x(_perfname);
    return;
}


/*************************************************************************

*************************************************************************/

void EV::stop(void)  {

    if (ds)  {
        ds->setConstantWatts(0.0f);			// physics->mode = PHYSICS_WINDLOAD
        ds->set_grade(0.0f);				// physics->mode = PHYSICS_WINDLOAD
        ds->setkgs(0.0f);
        ds->setConstantCurrent(0.0f);		// physics->mode = PHYSICS_CONSTANT_CURRENT_MODE
    }

    //unsigned long start_time;

#ifdef WIN32
	#ifdef DO_TINYTHREAD

		if (contin)  {
			#ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
				fprintf(logstream, "setting stop flag: %08lx\r\n", (unsigned long)stop_event);
				fflush(logstream);
				fclose(logstream);
			#endif
			contin = false;

			start_time = timeGetTime();

			while(running)  {
				if ( (timeGetTime()-start_time)>= 1000)  {
					#ifdef LOGEV
						logstream = fopen(EVLOGFILE, "a+t");
						fprintf(logstream, "stopping thread timed out\r\n");
						fflush(logstream);
						fclose(logstream);
					#endif
					break;
				}
				Sleep(20);
			}

	      running = false;
			#ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
				fprintf(logstream, "thread stopped\r\n");
				fflush(logstream);
				fclose(logstream);
			#endif
		}

		// THE DESTRUCTOR JOINS AND DELETES THE THRD!!!!!!!!!!!!!!!!!!!!

	#else					// #ifdef DO_TINYTHREAD
    if (stop_event != 0)  {
        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "setting stop event: %08lx\r\n", stop_event);
				fflush(logstream);
            fclose(logstream);
        #endif

        SetEvent(stop_event);

        #ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "stop event set, running = %s, stop_event = %08lx\r\n", running?"true":"false", stop_event);
				fflush(logstream);
            fclose(logstream);
        #endif

        start_time = timeGetTime();

        while(running)  {
            if ( (timeGetTime()-start_time)>= 1000)  {
                #ifdef LOGEV
							logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(logstream, "stopping thread timed out\r\n");
							fflush(logstream);
                    fclose(logstream);
                #endif
                break;
            }
            Sleep(20);
        }

        running = false;
        stop_event = 0;

			#ifdef LOGEV
				logstream = fopen(EVLOGFILE, "a+t");
            fprintf(logstream, "thread stopped\r\n");
				fflush(logstream);
            fclose(logstream);
			#endif
		}

		done = true;
	#endif							// #ifdef DO_TINYTHREAD
#endif								// #ifdef WIN32

    return;
}



/*************************************************************************

*************************************************************************/

int EV::set_watts(float _watts)  {
    ds->setConstantWatts(_watts);
#ifdef _DEBUG
    //oktobreak = true;
#endif

    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::reset(void)  {
    ds->reset();
    ds->setConstantWatts(0.0f);				// added this for velotron
    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::reset_averages(void)  {
    ds->reset_averages();
    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::set_wind_x(float _kph)  {
    float mph = (float)(KPHTOMPH * _kph);
    ds->set_x_wind(mph);
    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::set_draft_wind_x(float _kph)  {
    float mph = (float)(KPHTOMPH * _kph);
    ds->set_draft_wind(mph);
    return 0;
}


/*************************************************************************

*************************************************************************/


int EV::set_slope(float _bike_kgs, float _person_kgs, int _drag_factor, float _grade)  {

    if (_person_kgs != (float)rd.pd.kgs)  {
        if (!bike)  {
            //ds->setkgs(_person_kgs + _bike_kgs);
            ds->setkgs(_person_kgs + _bike_kgs);
            rd.pd.kgs = _person_kgs;
        }
        else  {
            ds->setkgs(_person_kgs);
            rd.pd.kgs = _person_kgs;
        }
    }

    if (bike)  {
        if (_bike_kgs != bike->get_kgs() )  {
            bike->set_kgs(_bike_kgs);
        }
    }

    int drag_factor = ROUND(100.0f*rd.watts_factor);

    if (_drag_factor != drag_factor)  {					// 0 to 1000 ==> 0.0f to 10.0f
        //float f = rd.watts_factor;
        float f = (float)_drag_factor / 100.0f;
        ds->set_drag_factor(f);
        rd.watts_factor = f;
    }

    if (_grade != grade)  {
        grade = _grade;
        ds->set_grade(grade);
    }

    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::set_hr_bounds_x(int LowBound, int HighBound, bool BeepEnabled)  {
    rd.lower_hr = (float)LowBound;
    rd.upper_hr = (float)HighBound;

    ds->set_hr_bounds(LowBound, HighBound, BeepEnabled);

    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::set_ftp_x(float _ftp)  {
    rd.ftp = _ftp;

    ds->set_x_ftp(rd.ftp);

    return 0;
}

/*************************************************************************

*************************************************************************/

void EV::clear_accum_tdc_x(void)  {
    ds->clear_accum_tdc();
    return;
}

/*************************************************************************

*************************************************************************/

int EV::get_accum_tdc_x(void)  {
    return ds->get_x_accum_tdc();
}

/*************************************************************************

*************************************************************************/

int EV::get_status_bits_x(void)  {
    return ds->get_x_status_bits();
}

/*************************************************************************

*************************************************************************/

int EV::get_keys(void)  {
    return ds->get_accum_keys();
}


/*************************************************************************

*************************************************************************/

RACERMATE::TrainerData * EV::myread(void)  {

    //while(lock) {}


    // watts

    if (ds->decoder->accum_watts_count==0)  {
        td.Power = ds->decoder->accum_watts;
    }
    else  {
        td.Power = ds->decoder->accum_watts / ds->decoder->accum_watts_count;
    }

#ifdef _DEBUG
    if (td.Power > .001f)  {
        bp = 4;
    }
#endif

    //------------------------------------
    // cadence
    //------------------------------------

    if ( (ds->decoder->meta.rpmflags & RPM_VALID) != RPM_VALID )  {
        if (what==DEVICE_COMPUTRAINER)  {
#ifdef DO_ANT
            if (asi)  {
                if (cad.decoding_data)  {
                    td.cadence = cad.get_val();
                }
                else if (sc.decoding_data)  {
                    td.cadence = sc.get_cadence();
                }
                else  {
                    td.cadence = -1.0f;
                }
            }
#else
            td.cadence = -1.0f;
#endif

        }
        else  {
            td.cadence = 0.0f;							// velotron, no cadence sensor plugged in
        }
    }
    else  {
        if (ds->decoder->meta.pedalrpm<=.00001f)  {
            td.cadence = 0.0f;
        }
        else  {
            if (ds->decoder->accum_rpm_count==0)  {
                td.cadence = ds->decoder->accum_rpm;
            }
            else  {
                td.cadence = ds->decoder->accum_rpm / ds->decoder->accum_rpm_count;
            }
        }
    }

    //-------------------------------------
    // hr
    //-------------------------------------

    if ( (ds->decoder->meta.hrflags&0x0c)==0x0c)  {				// if bits 2&3 set, no sensor is installed
#ifdef DO_ANT
        if (asi)  {
            td.HR = hr.get_val();

            /*
            float f;
            f = hr.get_val();
            ds->decoder->accum_hr += f;
            ds->decoder->accum_hr_count++;

            //td.HR = hr.get_val();
            //hr.reset_accum();
            //td.HR = ds->decoder->ant_hr;
            if (ds->decoder->accum_hr_count==0)  {
                td.HR = ds->decoder->accum_hr;
            }
            else  {
                td.HR = ds->decoder->accum_hr / ds->decoder->accum_hr_count;
            }
            */

        }
        else  {
            td.HR = -1.0f;
        }
#else
        td.HR = -1.0f;
#endif

    }
    else  {
        if (ds->decoder->accum_hr_count==0)  {
            td.HR = ds->decoder->accum_hr;
        }
        else  {
            td.HR = ds->decoder->accum_hr / ds->decoder->accum_hr_count;
        }
    }

    //-------------------------------------
    // speed (never use and speed sensor)
    //-------------------------------------

    if (ds->decoder->accum_kph_count==0)  {
        td.kph = ds->decoder->accum_kph;
    }
    else  {
        td.kph = ds->decoder->accum_kph / ds->decoder->accum_kph_count;
    }

    ds->decoder->seed_avgs();				// reset avgs

#ifdef DO_ANT
    //hr.seed_avgs();
    hr.reset_accum();
    cad.reset_accum();
#endif


    /*
    if (ds->decoder->get_finishEdge())  {
        //finishEdge = true;
        //ds->setFinished();
    }
    */


    return &td;
}					// myread()

/*************************************************************************

*************************************************************************/

RACERMATE::TrainerData * EV::fastread(void)  {

#if 1
    unsigned long dt=0L;
    unsigned long start = timeGetTime();

    while(lock) {
        dt = timeGetTime() - start;
        if (dt>=500L)  {
            break;
        }
    }
    if (dt > max_lock_time)  {
        max_lock_time = dt;
    }

#else
    DWORD now = timeGetTime();
    while(lock) {
        if ((now-start_time)>2000L)  {
            throw(fatalError(__FILE__, __LINE__));
        }
    }
#endif

    td.Power = ds->decoder->meta.watts;
    td.cadence = ds->decoder->meta.rpm;

    if ( (ds->decoder->meta.hrflags&0x0c)==0x0c)  {
#ifdef DO_ANT
        if (asi)  {
            td.HR = hr.get_val();
            #ifdef _DEBUG
                static int nzs = 0;
                if (td.HR < .1f)  {
                    nzs++;
                }
                else  {
                    if (nzs > 20)  {
                        bp = 2;
                    }
                    nzs = 0;
                }
            #endif
            //td.HR = ds->decoder->ant_hr;			// no hr sensor installed, so see if there is an ANT heartrate sensor
        }
#endif

    }
    else  {
        //td.HR = ds->decoder->meta.hr;
        if (ds->decoder->accum_hr_count==0)  {
            td.HR = ds->decoder->accum_hr;
        }
        else  {
            td.HR = ds->decoder->accum_hr / ds->decoder->accum_hr_count;
        }
    }

    td.kph = (float) (TOKPH*ds->decoder->meta.mph);

#ifdef EV_LOG_MPH
    fprintf(mphstream, "%10ld %.2f\n", timeGetTime(), ds->decoder->meta.mph);					// jumps
#endif

    return &td;
}					// fastread()


/*************************************************************************

*************************************************************************/

int EV::set_paused(bool _pause)  {

    if (_pause)  {
        ds->pause();
    }
    else  {
        ds->resume();
    }

    return 0;
}



#define STOP_EVENT 0
#define NEVENTS 1


#ifdef DO_TINYTHREAD

    /**********************************************************************
        copied from server.cpp
    **********************************************************************/
    //xxx
#if 1
    void EV::serverthreadproc(void *tmp)  {
        //int bp = 0;
        //EnumDeviceType what;
        int status;
        DWORD now, start_time;
    #ifdef _DEBUG
        //DWORD last_display_time = 0L;
        bool reached = false;
        unsigned long cnt = 0L;
        //float prev_wind = 0.0f;
    #endif


        EV *ev = static_cast<EV *>(tmp);
        //bp = 1;
        //what = ev->what;
        ev->lock = false;

        //ev->log(true, "\nlistening for connections on port %d\n\n", server->listen_port);

        ev->running = true;

        while(ev->contin)  {
            start_time = timeGetTime();

            ev->lock = true;

            while(1)  {
                now = timeGetTime();
                if ( (now-start_time) >= 500)  {
                    break;
                }
                /*
                #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(ev->logstream, "EV:: calling gnr\n");
                    fclose(ev->logstream);
                #endif
                */

                if (ev->ds)  {
                    status = ev->ds->getNextRecord();			// returns 0 if there is more data
                }

                /*
                #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(ev->logstream, "EV:: gnr status = %d\n", status);
                    fclose(ev->logstream);
                #endif
                */

                if (status!=0)  {
                    ev->timeouts++;
                    break;
                }
            }

            ev->lock = false;


            #ifdef EV_LOG_MPH
            //fprintf(ev->mphstream, "%10ld %.2f\n", timeGetTime(), ev->ds->decoder->meta.mph);				// doesn't jump
            #endif

            #ifdef _DEBUG
                //ev->at->update();					// 10.00 ms
            #endif

            if (ev->ds)  {
                ev->ds->updateHardware();								// takes .0017 ms
            }
            //dw = WaitForMultipleObjects(NEVENTS, hArray, FALSE, 10);

            Sleep(10);
            ev->thread_run_count++;

            #ifdef _DEBUG
            //ev->at->update();					// 10.00 ms
            #endif

    #ifdef _DEBUG
            cnt++;
            if (ev->ds)  {
                if (ev->ds->decoder)  {
                    if (ev->ds->decoder->meta.wind > .1f)  {
                        reached = true;										// cnt = 11
                    }
                    if (reached)  {
                        if (ev->ds->decoder->meta.wind < .01f)  {
                            //bp = 2;											// cnt = 51
                        }
                    }
                }
            }
    #endif

        }

        ev->running = false;
        //printf("threadx\n");

        return;
    }										// serverthreadproc()
#endif			// #if 0

#else												// #if DO_TINYTHREAD
    #ifdef WIN32

    /*************************************************************************

    *************************************************************************/

    void EV::threadProc(void *tmp)  {
        int status;
        DWORD dw;
        int bp = 0;
        HANDLE hArray[NEVENTS];
        BOOL b;
        EnumDeviceType what;
        float mph;
        DWORD now, start_time;
    #ifdef _DEBUG
        DWORD last_display_time = 0L;
        bool reached = false;
        unsigned long cnt = 0L;
        float prev_wind = 0.0f;
    #endif

        EV *ev = static_cast<EV *>(tmp);

        #ifdef _DEBUG
            if (ev->ds==NULL)  {
                bp = 4;
            }
        #endif

        #ifdef LOGEV
				ev->logstream = fopen(EVLOGFILE, "a+t");
            fprintf(ev->logstream, "EV:: threadproc start\n");
            fclose(ev->logstream);
        #endif

        hArray[STOP_EVENT] = ev->stop_event;

        what = ev->what;
        ev->running = true;
        ev->lock = false;


        while(1)  {
            start_time = timeGetTime();

            ev->lock = true;

            while(1)  {
                now = timeGetTime();
                if ( (now-start_time) >= 500)  {
                    break;
                }
                /*
                #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(ev->logstream, "EV:: calling gnr\n");
                    fclose(ev->logstream);
                #endif
                */

                status = ev->ds->getNextRecord();			// returns 0 if there is more data

                /*
                #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(ev->logstream, "EV:: gnr status = %d\n", status);
                    fclose(ev->logstream);
                #endif
                */

                if (status!=0)  {
                    ev->timeouts++;
                    break;
                }
            }

            ev->lock = false;


            #ifdef EV_LOG_MPH
            //fprintf(ev->mphstream, "%10ld %.2f\n", timeGetTime(), ev->ds->decoder->meta.mph);				// doesn't jump
            #endif

            ev->ds->updateHardware();								// takes .0017 ms
            dw = WaitForMultipleObjects(NEVENTS, hArray, FALSE, 10);

            ev->thread_run_count++;

            switch(dw)  {
                case WAIT_TIMEOUT:			// The time-out interval elapsed, and the object's state is nonsignaled.
                    bp = 1;
                    //ev->at->update();		// 10.74 ms for 10 ms timeout, 9.7 ms for 9 ms timeout, 1.95 ms for 1 ms timeout
                    break;

                case WAIT_OBJECT_0 + STOP_EVENT:  {			// stop_event
                    goto finis;
                }

                case WAIT_ABANDONED:			// The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled
                    bp = 3;
                    break;

                default:
                    bp = 4;
                    break;
            }									// switch()
            //ev->at->update();					// 10.74 ms with 10 ms timeout, .0011 ms with no timeout
    #ifdef _DEBUG
            cnt++;
            if (ev->ds->decoder->meta.wind > .1f)  {
                reached = true;										// cnt = 11
            }
            if (reached)  {
                if (ev->ds->decoder->meta.wind < .01f)  {
                    bp = 2;											// cnt = 51
                }
            }
    #endif
        }							// while(1)


    finis:
        #ifdef LOGEV
            ev->running = false;
				ev->logstream = fopen(EVLOGFILE, "a+t");
            fprintf(ev->logstream, "EV:: out of threadproc loop\n");
            fclose(ev->logstream);
        #endif

        //CloseHandle(ev->stop_event);
        ev->stop_event = 0;
        //_endthread();						// return invokes _endthread() anyway

        return;
    }						// void EV::threadProc(void *tmp)  {

    #else									// LINUX VERSION

    /**********************************************************************

    **********************************************************************/

    void EV::sig_alarm(int _sig) {
        //printf("sig_alarm\n");
        #ifdef _DEBUG
        //	OGL::at->update();				// .9999 ms
        #endif
        //alarm_sig = _sig;
        return;
    }

    /*************************************************************************
     this is the data collection thread
    *************************************************************************/

    void *EV::threadProc(void *tmp)  {
         //EnumDeviceType what;
         DWORD start_time;
         DWORD now;
         int status;

        /*
         int nready;
         struct timeval tp_begin;
         //struct timeval tp;
         int tmrfd = -1;
         fd_set readset;
         fd_set rset;
         //int msnow;												// time since start, milliseconds, from gettime_ms()
         ssize_t s;
         uint64_t exp;
         int  tick=0;
         //AverageTimer at("tp");
         */


         EV *ev = static_cast<EV *>(tmp);

         //what = ev->what;
         ev->running = true;
         ev->lock = false;

    #if 0
         struct itimerspec new_value;

         //init interval timer for timerfd
         //10 mS timeout
         new_value.it_value.tv_sec = 0;
         new_value.it_value.tv_nsec = 10000000;
         new_value.it_interval.tv_sec = 0;
         new_value.it_interval.tv_nsec = 10000000;	// 10 ms
         tmrfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

         if (tmrfd == -1) {
        handle_error("timerfd_create");
         }

         if (timerfd_settime(tmrfd, 0, &new_value, NULL) == -1) {
              handle_error("timerfd_settime");
         }

         //set up select triggers
         FD_ZERO(&readset);
         //FD_SET(STDIN_FILENO, &readset);
         //	FD_SET(cc[hbar].fd, &readset);
         FD_SET(tmrfd, &readset);
         gettimeofday(&tp_begin, NULL);
         //ev->print_timer_res();

         // wait for events

         while (!ev->done)  {

              start_time = timeGetTime();
        ev->lock = true;

              while(1)  {
                    now = timeGetTime();
                    if ( (now-start_time) >= 500)  {
                         break;
                    }

                    status = ev->ds->getNextRecord();			// returns 0 if there is more data
                    if (status!=0)  {
                         ev->timeouts++;
            break;
                    }
        }

              ev->lock = false;

        ev->ds->updateHardware();


              // now sleep for 10 ms:

              rset = readset;
              nready = select(32, &rset, NULL, NULL, NULL);
              //msnow = ev->gettime_ms(&tp, &tp_begin);

              if (nready <= 0) {
                    ev->bp = 7;
                    break;
              }

              if (FD_ISSET(tmrfd, &rset)) {
                    //have to read from the timer fd to clear the select flag

                    s = read(tmrfd, (void *)&exp, (size_t)sizeof(uint64_t));
                    if (s != sizeof(uint64_t)) {
                         handle_error("read");
                    }

                    tick +=1;
                    if (tick >= 100) {
                         tick = 0;
                         //at.update();							// 1000.000000 ms
                    }
              }                           // if (FD_ISSET(tmrfd, &rset)) {
        //at.update();								// 10.000 ms
         }							// while (!done)


    #else

         bool run = true;

         while(run)  {
              start_time = timeGetTime();

        ev->lock = true;

              while(1)  {
                    now = timeGetTime();
                    if ( (now-start_time) >= 500)  {
                         break;
                    }

                    status = ev->ds->getNextRecord();			// returns 0 if there is more data
                    if (status!=0)  {
                         ev->timeouts++;
            break;
                    }
        }

        ev->lock = false;


        #ifdef EV_LOG_MPH
                    //fprintf(ev->mphstream, "%10ld %.2f\n", timeGetTime(), ev->ds->decoder->meta.mph);				// doesn't jump
        #endif

        ev->ds->updateHardware();								// takes .0017 ms

              /*
        dw = WaitForMultipleObjects(NEVENTS, hArray, FALSE, 10);
        ev->thread_run_count++;
        switch(dw)  {
                    case WAIT_TIMEOUT:			// The time-out interval elapsed, and the object's state is nonsignaled.
                         bp = 1;
            //ev->at->update();		// 10.74 ms for 10 ms timeout, 9.7 ms for 9 ms timeout, 1.95 ms for 1 ms timeout
                         break;
                    case WAIT_OBJECT_0 + STOP_EVENT:  {			// stop_event
                         goto finis;
                    }
                    case WAIT_ABANDONED:			// The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled
                         bp = 3;
                         break;

                    default:
                         bp = 4;
            break;
        }									// switch()
              */
              //ev->at->update();					// 10.74 ms with 10 ms timeout, .0011 ms with no timeout
         }							// while(1)
    #endif

         return NULL;
    }                           // void *EV::threadProc(void *tmp)  {

    /*******************************************************************************/
    /*******************************************************************************/
    // return elapsed time in milliseconds as integer
    int EV::gettime_ms(struct timeval *t, struct timeval *tbegin) {
        gettimeofday(t,NULL);
        return (int)((t->tv_sec-tbegin->tv_sec)*1000L + (t->tv_usec-tbegin->tv_usec)/1000L);
    }


    /*******************************************************************************/
    /*******************************************************************************/

    // stuff for testing timers
    void EV::print_timer_res(void) {

        return;

        /*
        struct timespec curr;
        int nsecs;

        if (clock_getres(CLOCK_MONOTONIC, &curr) == -1) {
            handle_error("clock_getres");
        }

        nsecs = curr.tv_sec*1000000000 + curr.tv_nsec;
        //fprintf(logfile,"CLOCK_MONOTONIC resolution is  %d nsecs\n\n", nsecs);
        //printf("CLOCK_MONOTONIC resolution is  %d nsecs\n\n", nsecs);
        */

    }


    #endif

#endif			// #ifndef DO_TINYTHREAD

/*************************************************************************

*************************************************************************/

Bike::GEARPAIR EV::get_gear_pair(void)  {
    gp = ds->get_gear_pair();
    return gp;
}

/*************************************************************************

*************************************************************************/

Bike::GEARPAIR EV::get_gear_indices_x(void)  {
    gp = ds->get_gear_indices_x();
    return gp;
}

/**********************************************************************

**********************************************************************/

void EV::keydownfunc(void *object, int id, int key)  {
    //int i;
    //int bp = 0;
    //bool dbg = true;

    EV *ev = (EV *) object;

    switch(key)  {
        case 1:
            break;
        case 2:
            break;
        case 3:										// up arrow
            if (ev->shiftflag)  {
                //bp = 1;
            }
            else  {
                if (ev->bike)  {						// computrainer threads don't have a bike
                    ev->bike->upkeydown();
                }
            }
            break;
        case 4:
            ev->shiftflag = true;
            break;
        case 5:
            break;
        case 6:										// down arrow
            if (ev->shiftflag)  {
                //bp = 2;
            }
            else  {
                if (ev->bike)  {						// computrainer threads don't have a bike
                    ev->bike->downkeydown();
                }
            }
            break;
        case 0:
            if (ev->bike)  {						// computrainer threads don't have a bike
                if ( ev->bike->run() )  {
                    ev->ds->decoder->meta.virtualFrontGear = ev->bike->state.gp.front;
                    ev->ds->decoder->meta.gear = (unsigned char) (.5 + ev->bike->state.gear_inches);
                    ev->ds->decoder->meta.virtualRearGear = ev->bike->state.gp.rear;
                }
            }
            break;
        default:
            break;
    }
    return;
}


/**********************************************************************

**********************************************************************/

void EV::keyupfunc(void *object, int id, int key)  {
    //int bp = 0;

    EV *ev = (EV *) object;

    switch(key)  {
        case 1:
            break;
        case 2:
            break;
        case 3:  {
            if (ev->shiftflag)  {
            }
            else  {
                if (ev->bike)  {						// computrainer threads don't have a bike
                    if (ev->bike->upkeyup())  {
                        ev->ds->decoder->meta.gear = (unsigned char) (.5 + ev->bike->state.gear_inches);
                        ev->ds->decoder->meta.virtualRearGear = ev->bike->state.gp.rear;
                    }
                }
            }
            break;
        }
        case 4:
            ev->shiftflag = false;
            break;
        case 5:
            break;
        case 6:  {
            if (ev->shiftflag)  {
            }
            else  {
                if (ev->bike)  {						// computrainer threads don't have a bike
                    if (ev->bike->downkeyup())  {
                        ev->ds->decoder->meta.gear = (unsigned char) (.5 + ev->bike->state.gear_inches);
                        ev->ds->decoder->meta.virtualRearGear = ev->bike->state.gp.rear;
                    }
                }
            }
            break;
        }

        case 0:
            break;

        default:
            break;
    }							// switch(key)

}								// keyupfunc()

/**********************************************************************

**********************************************************************/

int EV::set_gear(int _front_index, int _rear_index)  {
    bike->set_gear(_front_index, _rear_index);
    return 0;
}


/*************************************************************************

*************************************************************************/

int EV::get_lower_hr_x(void)  {
    int k;
    k = ds->get_lower_hr_x();
    return k;
}

/*************************************************************************

*************************************************************************/

int EV::get_upper_hr_x(void)  {
    int k;
    k = ds->get_upper_hr_x();
    return k;
}

/*************************************************************************

*************************************************************************/

int EV::get_hr_beep_enabled_x(void)  {
    int k;
    k = ds->get_hr_beep_enabled_x();
    return k;
}


/*************************************************************************

*************************************************************************/


float *EV::get_bars_x(void)  {

    return ds->decoder->meta.bars;
}

/*************************************************************************

*************************************************************************/


float *EV::get_average_bars_x(void)  {

    return ds->decoder->ss->get_x_bars2();
}

/*************************************************************************

*************************************************************************/

int EV::set_constant_force_x(float _force)  {
    ds->set_constant_force_x(_force);
    return 0;
}
/*************************************************************************

*************************************************************************/

SSDATA * EV::read_ss(void)  {
    ssd.ss = ds->decoder->meta.ss;
    ssd.lss = ds->decoder->meta.lss;
    ssd.rss = ds->decoder->meta.rss;
    ssd.lsplit = ds->decoder->meta.lwatts;
    ssd.rsplit = ds->decoder->meta.rwatts;

    return &ssd;
}					// read_ss()

/*************************************************************************

*************************************************************************/

float EV::get_calories_x(void)  {
    return ds->decoder->meta.calories;
}

/*************************************************************************

*************************************************************************/

float EV::get_x_np(void)  {
    if (formula)  {
        return formula->get_x_np();
    }
    return -1.0f;
}

/*************************************************************************

*************************************************************************/

float EV::get_if_x(void)  {
    if (formula)  {
        return formula->get_x_if();
    }
    //return -FLT_MAX;
    return -1.0f;
}

/*************************************************************************

*************************************************************************/

float EV::get_tss_x(void)  {
    if (formula)  {
        return formula->get_x_tss();
    }
    return -1.0f;
}

/*************************************************************************

*************************************************************************/

float EV::get_pp_x(void)  {
    return ds->decoder->meta.pp;
}

/*************************************************************************

*************************************************************************/

int EV::get_cal_time_x(void)  {
    return (int)ds->decoder->caltime;
}





//#ifdef EXTENDED_DLL
#if LEVEL >= DLL_FULL_ACCESS

/*************************************************************************

*************************************************************************/

const char *EV::get_perf_file_name_x(void)  {
    if (!ds)  {
        return NULL;
    }

    const char *cptr = ds->get_perf_file_name_x();
    return cptr;
}


/*************************************************************************

*************************************************************************/

METADATA * EV::get_meta_x(void)  {
    return &ds->decoder->meta;
}					// get_meta()

/*************************************************************************

*************************************************************************/
/*
METADATA * EV::get_decoder(void)  {
    return &ds->decoder;
}					// get_decoder()
*/

/*************************************************************************

*************************************************************************/

unsigned char EV::get_control_byte_x(void)  {
    unsigned char cb;

    cb = ds->get_control_byte_x();

    return cb;
}

/*************************************************************************

*************************************************************************/

Physics::MODE EV::get_physics_mode_x(void)  {
    Physics::MODE mode = Physics::NOMODE;

    mode = ds->get_physics_mode_x();

    return mode;
}

/*************************************************************************

*************************************************************************/

float EV::get_manual_watts_x(void)  {
    float watts = 0.0f;

    watts = ds->getConstantWatts();

    return watts;
}

/*************************************************************************

*************************************************************************/

float EV::get_grade_x(void)  {
    float grade = 0.0f;

    grade = (float)ds->getGrade();

    return grade;
}

/*************************************************************************

*************************************************************************/

Physics *EV::get_physics_x(void)  {
    return ds->get_physics_x();
}

/*************************************************************************

*************************************************************************/

int EV::get_packet_error1_x(int ix)  {
    return (int)ds->packet_error1;
}

/*************************************************************************

*************************************************************************/

int EV::get_packet_error2_x(int ix)  {
    return (int)ds->packet_error2;
}

/*************************************************************************

*************************************************************************/

int EV::get_packet_error3_x(int ix)  {
    return ds->decoder->packet_error3;
}

/*************************************************************************

*************************************************************************/

int EV::get_packet_error4_x(int ix)  {
    return ds->decoder->packet_error4;
}

/*************************************************************************

*************************************************************************/

void EV::set_manual_speed_x(float _manual_mph)  {
    ds->set_manual_mph(_manual_mph);
    return;
}
#endif				// #if LEVEL

#ifdef WIN32

/**********************************************************************
    interrupts at 100 ms rate
**********************************************************************/

void CALLBACK EV::TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)  {

    if (dwUser==NULL)  {
        return;
    }
    EV *ev = (EV *)dwUser;

	 //if (ev->ser)
#ifdef _DEBUG
    //ev->at->update();						// 100.00 ms
#endif
    ev->ds->decoder->ss->do_ten_hz_filter(&ev->ds->decoder->meta, ev->ds->is_started_x(), ev->ds->is_finished_x(), ev->ds->is_paused_x());			// computes ss peaks and averages

    return;
}

#else
            // todo

#endif		// #ifdef WIN32


/*************************************************************************

*************************************************************************/

int EV::start_cal(void)  {
    unsigned char c;
    c = ds->start_cal();				// bike needs to go into gear 93
    return c;
}

/*************************************************************************

*************************************************************************/

int EV::end_cal(unsigned char _last_control_byte)  {
    /*
    enum COMPUTRAINER_MODE  {
        HB_WIND_RUN = 0x2c,
        HB_WIND_PAUSE = 0x28,
        HB_RFTEST = 0x1c,
        HB_ERGO_RUN = 0x14,
        HB_ERGO_PAUSE = 0x10
    };
    */

    //ds->set_control_byte(_last_control_byte);
    ds->end_cal(_last_control_byte);

    //control_byte = _last_control_byte;
    //last_control_byte = _last_control_byte;
    return 0;
}

/*************************************************************************

*************************************************************************/

int EV::send_current(unsigned short _pic_current)  {

    Velotron *velotron = (Velotron *)ds;

    velotron->setPicCurrent(_pic_current);				// independent of physics

    return 0;
}

/*************************************************************************

*************************************************************************/

const char *EV::get_computrainer_cal(void)  {
    float ftemp;
    DWORD start_time;

    bp = 0;
    ds->decoder->meta.rfdrag = 0;				// unsigned short

    start_time = timeGetTime();

    while(1)  {
        if ( (timeGetTime()-start_time) >= 2000L)  {
            strcpy(calstr, "n/a");
// temporary fix:
#if 1
#ifdef _DEBUG
            strcpy(calstr, "2.50U");
#endif
#endif

            break;
        }

        if (ds->decoder->meta.rfdrag != 0)  {
            ftemp = (float) (ds->decoder->meta.rfdrag & 0x07ff) / 256.0f;
#ifdef _DEBUG
            if (ftemp >3.50f)  {
                bp = 1;
            }
#endif
            sprintf(calstr, "%.2f", ftemp);
            if ((ds->decoder->meta.rfmeas & 0x0008) == 0x0008)  {
                strcat(calstr, " C");
            }
            else  {
                strcat(calstr, " U");
            }
            break;
        }
    }

    return calstr;
}

/*************************************************************************

*************************************************************************/

void EV::set_calibrating_x(bool b)  {
    Physics *physics;

    physics = ds->get_physics_x();

    physics->calibrating = b;

    return;
}

/*************************************************************************

*************************************************************************/

void EV::start(bool _b)  {
    if (_b)  {
        ds->start();
    }
    else  {
        ds->finish();
    }

    return;
}

/*************************************************************************

*************************************************************************/

void EV::set_finished(bool _b)  {
    ds->setFinished();
    return;
}

/*************************************************************************

*************************************************************************/

void EV::set_circumference_x(float _circumference)  {
    //use_speed_cadence_sensor = _b;
    //if (!ant_stick_initialized)  {
#ifdef DO_ANT
    if (!asi)  {
        return;
    }
#endif

//	RTD *rtd = (RTD *) ds;
    //rtd->set_cadence_sensor_params(_circumference);
    //rtd->set
    //rtd->set
    //rtd = NULL;
    return;
}

#if 0
/*************************************************************************

*************************************************************************/

void EV::set_speed_cadence_sensor_params(float _circumference)  {
    //use_speed_cadence_sensor = _b;
    if (!ant_stick_initialized)  {
        return;
    }
    //RTD *rtd = (RTD *) ds;
    //rtd->set_cadence_sensor_params(_circumference);
    //rtd->set
    //rtd = NULL;
    return;
}

/*************************************************************************

*************************************************************************/

/*
void EV::set_use_heartrate_sensor_x(int _b)  {
    use_heartrate_sensor = _b;
    RTD *rtd = (RTD *) ds;
    rtd->set_use_hr_sensor((bool)use_heartrate_sensor);
    return;
}
*/

#endif


/*************************************************************************

*************************************************************************/

int EV::startCal_x(void)  {
    ds->startCal_x();
    return 0;
}					// startCal()

/*************************************************************************

*************************************************************************/

int EV::endCal(void)  {
    return 0;
}					// endCal()


/*************************************************************************

*************************************************************************/

int EV::get_calibration_x(void)  {
    int cal = ds->get_calibration_x();
    return cal;
}					// start_velotron_cal()

/*************************************************************************

*************************************************************************/

int EV::start_collection_thread(void)  {

#ifndef DO_TINYTHREAD
    /****
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    #ifdef WIN32
        stop_event = CreateEvent(
                                    NULL,						// no security attributes
                                    TRUE,						// create manual reset event
                                    FALSE,					// initial state is not signaled
                                    NULL);					// no event name

        if (stop_event == NULL)  {
            return 1;
        }

        running = false;

        hthread = (HANDLE) _beginthread(&EV::threadProc, 32768, this);
        if (hthread == NULL)  {
            return 1;
        }
    #else									// #ifdef WIN32
        // non tinythread++ linux version
        bp = 0;
        int status;

        status = pthread_create(
                    &thread,
                    NULL,
                    threadProc,
                    (void*) this
                    );
        if (status != 0)  {
            //bp  = 7;
            return 1;
        }
    #endif										// #ifdef WIN32
    **********/
#else
    //yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy
    contin = true;
    if (devices[ix].commtype==TRAINER_IS_SERVER)  {
        thrd = new tthread::thread(&clientthreadproc, this);
    }
    else if (devices[ix].commtype==TRAINER_IS_CLIENT)  {
        //thrd = new tthread::thread(&serverthreadproc, this);
    }
    else if (devices[ix].commtype==WIN_RS232)  {
        thrd = new tthread::thread(&serverthreadproc, this);
    }
    else  {
        throw(fatalError(__FILE__, __LINE__, "Bad commtype"));
    }
#endif											// #ifndef DO_TINYTHREAD

    start_time = timeGetTime();

    return 0;
}										// start_collection_thread()

/*************************************************************************

*************************************************************************/

void EV::set_logging_x(LoggingType _logging_type)  {
    ds->set_logging_x(_logging_type);
    return;
}

/*************************************************************************

*************************************************************************/

bool EV::get_ant_stick_initialized(void)  {
#ifdef DO_ANT
    return asi;
#else
    return false;
#endif

}


#if LEVEL >= DLL_TRINERD_ACCESS

/*************************************************************************

*************************************************************************/

int EV::get_computrainer_mode_x(void)  {
    int mode;
    mode = ds->get_computrainer_mode_x();
    return mode;
}
#endif


#ifdef DO_EXPORT

/*************************************************************************

*************************************************************************/

void EV::set_export(const char *_dbgfname, int _port)  {
    sprintf(gstr, "%s\\slope_%d.txt", _dbgfname, _port);
	 if (ds)  {
		ds->set_export(gstr);
	 }
    return;
}
#endif



//#ifdef EXTENDED_DLL
#if LEVEL >= DLL_FULL_ACCESS

/*************************************************************************

*************************************************************************/

int EV::load_x(const char *_fname)  {

    if (exists(_fname))  {
        strncpy(perfname, _fname, sizeof(perfname)-1);
        loaded = true;
        return 0;
    }

    memset(perfname, 0, sizeof(perfname));
    loaded = false;
    return 1;
}

#endif

#ifdef DO_TINYTHREAD

    /**********************************************************************
        I am the client the trainer is the server.
    **********************************************************************/
    //xxx

    void EV::clientthreadproc(void *tmp)  {
        //int bp = 0;
        //EnumDeviceType what;
        int status;
        DWORD now, start_time;
    #ifdef _DEBUG
        //DWORD last_display_time = 0L;
        bool reached = false;
        unsigned long cnt = 0L;
        //float prev_wind = 0.0f;
    #endif


        EV *ev = static_cast<EV *>(tmp);
        //bp = 1;
        //what = ev->what;
        ev->lock = false;

        //ev->log(true, "\nlistening for connections on port %d\n\n", server->listen_port);

#if 0
    while(1)  {
        start_time = timeGetTime();

        ev->lock = true;

        while(1)  {
            now = timeGetTime();
            if ( (now-start_time) >= 500)  {
                break;
            }
            /*
            #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                fprintf(ev->logstream, "EV:: calling gnr\n");
                fclose(ev->logstream);
            #endif
            */

            status = ev->ds->getNextRecord();			// returns 0 if there is more data

            /*
            #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                fprintf(ev->logstream, "EV:: gnr status = %d\n", status);
                fclose(ev->logstream);
            #endif
            */

            if (status!=0)  {
                ev->timeouts++;
                break;
            }
        }

        ev->lock = false;


        #ifdef EV_LOG_MPH
        //fprintf(ev->mphstream, "%10ld %.2f\n", timeGetTime(), ev->ds->decoder->meta.mph);				// doesn't jump
        #endif

        ev->ds->updateHardware();								// takes .0017 ms
        dw = WaitForMultipleObjects(NEVENTS, hArray, FALSE, 10);

        ev->thread_run_count++;

        switch(dw)  {
            case WAIT_TIMEOUT:			// The time-out interval elapsed, and the object's state is nonsignaled.
                bp = 1;
                //ev->at->update();		// 10.74 ms for 10 ms timeout, 9.7 ms for 9 ms timeout, 1.95 ms for 1 ms timeout
                break;

            case WAIT_OBJECT_0 + STOP_EVENT:  {			// stop_event
                goto finis;
            }

            case WAIT_ABANDONED:			// The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled
                bp = 3;
                break;

            default:
                bp = 4;
                break;
        }									// switch()
        //ev->at->update();					// 10.74 ms with 10 ms timeout, .0011 ms with no timeout
#ifdef _DEBUG
        cnt++;
        if (ev->ds->decoder->meta.wind > .1f)  {
            reached = true;										// cnt = 11
        }
        if (reached)  {
            if (ev->ds->decoder->meta.wind < .01f)  {
                bp = 2;											// cnt = 51
            }
        }
#endif
    }							// while(1)
#endif

        ev->running = true;

        while(ev->contin)  {
            start_time = timeGetTime();

            ev->lock = true;

            while(1)  {
                now = timeGetTime();
                if ( (now-start_time) >= 500)  {
                    break;
                }
                /*
                #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(ev->logstream, "EV:: calling gnr\n");
                    fclose(ev->logstream);
                #endif
                */


                if (ev->ds)  {
                    status = ev->ds->getNextRecord();			// returns 0 if there is more data
                }

                /*
                #ifdef LOGEV
							ev->logstream = fopen(EVLOGFILE, "a+t");
                    fprintf(ev->logstream, "EV:: gnr status = %d\n", status);
                    fclose(ev->logstream);
                #endif
                */

                if (status!=0)  {
                    ev->timeouts++;
                    break;
                }
            }

            ev->lock = false;


            #ifdef EV_LOG_MPH
            //fprintf(ev->mphstream, "%10ld %.2f\n", timeGetTime(), ev->ds->decoder->meta.mph);				// doesn't jump
            #endif

            #ifdef _DEBUG
                //ev->at->update();					// 10.00 ms
            #endif

            if (ev->ds)  {
                ev->ds->updateHardware();								// takes .0017 ms
            }
            //dw = WaitForMultipleObjects(NEVENTS, hArray, FALSE, 10);

            Sleep(10);
            ev->thread_run_count++;

            #ifdef _DEBUG
            //ev->at->update();					// 10.00 ms
            #endif

    #ifdef _DEBUG
            cnt++;
            if (ev->ds)  {
                if (ev->ds->decoder)  {
                    if (ev->ds->decoder->meta.wind > .1f)  {
                        reached = true;										// cnt = 11
                    }
                    if (reached)  {
                        if (ev->ds->decoder->meta.wind < .01f)  {
                            //bp = 2;											// cnt = 51
                        }
                    }
                }
            }
    #endif

        }

        ev->running = false;
        //printf("threadx\n");

        return;
    }										// clientthreadproc()

#endif
