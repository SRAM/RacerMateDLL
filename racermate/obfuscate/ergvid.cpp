
#include <ergvid.h>

#ifdef WIN32
	//#define STRICT
	#define WIN32_LEAN_AND_MEAN
	#pragma warning(disable:4297 4996)
#endif



//#ifdef OBFUSCATE								// #ifdef OBFUSCATE
#if 0
	#include "ergvid_obfuscated.cpp"				// #ifdef OBFUSCATE
#else											// #ifdef OBFUSCATE

/*********************************************************************
	x:\_fs\dll
*********************************************************************/


#ifdef WIN32
	#include <windows.h>
	#include <conio.h>
	#include <mmsystem.h>
	#include <assert.h>
	#include <shlobj.h>
#endif

#include <glib_defines.h>


#include <utils.h>
#include <vutils.h>

#include <serial.h>
#include <computrainer.h>
#include <velotron.h>
#include <nvram.h>
#include <logger.h>
#include <errors.h>

#ifdef _DEBUG
	#include <averagetimer.h>
#endif

#ifdef WIN32
#include <calibrator.h>
#endif

#include <globals.h>

char gstring[2048];

//#define FAKE_HB 1

#ifndef RD_DLL
	#error "no RD_DLL"							// just to make sure I'm using the right defines.h
#endif

#ifdef ERGVIDLOG
	//#error "ERGVIDLOG is defined !!!!!!!!!!!!!!!!!!!!!"
	#pragma message("ERGVIDLOG is defined !!!!!!!!!!!!!!!!!!!!!")
#endif



DEVICE devices[NPORTS] = {
            { DEVICE_NOT_SCANNED }
        };

DWORD last_port_time = 0L;

const int ctmask[6] = {
	0x01,
	0x02,
	0x08,
	0x10,
	0x04,
	0x20
};

const int vtmask[6] = {
		0x02,
		0x04,
		0x10,
		0x01,
		0x08,
		0x20
};

#ifdef _DEBUG
	//#define DEVICE_TIMEOUT_MS (10*60*1000)							// 10 minute timeout
	unsigned long DEVICE_TIMEOUT_MS = 10*60*1000;						// 10 minute timeout
#else
	//#define DEVICE_TIMEOUT_MS 10000
	unsigned long DEVICE_TIMEOUT_MS = 10000;							// milliseconds
#endif

const char *devstrs[DEVICE_OTHER_ERROR+1] = {
	"DEVICE_NOT_SCANNED",				// unknown, not scanned
	"DEVICE_DOES_NOT_EXIST",			// serial port does not exist
	"DEVICE_EXISTS",						// exists, openable, but no RM device on it
	"DEVICE_COMPUTRAINER",
	"DEVICE_VELOTRON",
	"DEVICE_SIMULATOR",
	"DEVICE_RMP",
	"DEVICE_ACCESS_DENIED",				// port present but can't open it because something else has it open
	"DEVICE_OPEN_ERROR",					// port present, unable to open port
	"DEVICE_OTHER_ERROR"					// prt present, error, none of the above
};

#ifdef FLASHLIGHT
	#pragma message (".")
	#pragma message (".")
	#pragma message (".")
	#pragma message ("UNDEF FLASHLIGHT!!!!!!!!")
	#pragma message ("UNDEF FLASHLIGHT!!!!!!!!")
	#pragma message ("UNDEF FLASHLIGHT!!!!!!!!")
	#pragma message ("UNDEF FLASHLIGHT!!!!!!!!")
	#pragma message ("UNDEF FLASHLIGHT!!!!!!!!")
	#pragma message (".")
	#pragma message (".")
	#pragma message (".")
#endif


bool gfile_mode = false;
bool calibrating = false;
bool bluetooth_delay = true;
bool connected = false;


Logger *glog = NULL;


#if LEVEL >= DLL_FULL_ACCESS
	#pragma message ("***********************  DLL_FULL_ACCESS  ***********************")
#elif LEVEL >= DLL_ERGVIDEO_ACCESS
	#pragma message ("***********************  DLL_ERGVIDEO_ACCESS  ***********************")
#elif LEVEL >= DLL_CANNONDALE_ACCESS
	#pragma message ("***********************  DLL_CANNONDALE_ACCESS  ***********************")
#else
	#pragma message ("***********************  UNKNOWN_ACCESS  ***********************")
	#pragma message ("***********************  UNKNOWN_ACCESS  ***********************")
	#pragma message ("***********************  UNKNOWN_ACCESS  ***********************")
	#pragma message ("***********************  UNKNOWN_ACCESS  ***********************")
	#pragma message ("***********************  UNKNOWN_ACCESS  ***********************")
	#pragma message ("***********************  UNKNOWN_ACCESS  ***********************")
#endif


CRF cryptor;
int instances = 0;
//#include <rmp.h>
//RMP *rpm;

/**********************************************************************

**********************************************************************/

bool is_running(int ix)  {

	if (devices[ix].ev)  {
		return true;
	}
	return false;
}


/**********************************************************************

**********************************************************************/

float get_slip(int ix, int fw)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->get_slip();
	}

	return 0.0f;
}

/**********************************************************************

**********************************************************************/

bool get_slipflag(int ix, int fw)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->get_slipflag();
	}

	return false;
}

/**********************************************************************

**********************************************************************/

unsigned long get_finish_ms(int ix, int fw)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->get_finish_ms();
	}

	return 0L;
}

/**********************************************************************
METADATA *get_meta(int ix)  {
	METADATA *meta=NULL;


	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_meta(%d)\n", ix);
		fclose(logstream);
	#endif

	if (devices[ix].ev)  {
		meta = devices[ix].ev->get_meta();
	}

	return meta;
}							// get_meta()

**********************************************************************/

float get_raw_rpm(int ix, int fw)  {

	//if (devices[ix].ds)  {
	if (devices[ix].ev)  {
		//return devices[ix].ev->get_raw_rpm();
		float f = devices[ix].ev->get_decoder()->raw_rpm;
		return (f + 7.0f);
	}

	return 0.0f;
}


/**********************************************************************

**********************************************************************/

void clear_slipflag(int ix, int fw)  {

	if (devices[ix].ev)  {
		devices[ix].ev->clear_slipflag();
	}

	return;
}

/**********************************************************************

**********************************************************************/

METADATA *get_meta(int ix)  {

	METADATA *meta=NULL;


	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_meta(%d)\n", ix);
		fclose(logstream);
	#endif

	if (devices[ix].ev)  {
		meta = devices[ix].ev->get_meta_x();
	}

	return meta;
}							// get_meta()

/**********************************************************************

**********************************************************************/

Decoder *get_decoder(int ix)  {

	Decoder *d=NULL;


	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_decoder(%d)\n", ix);
		fclose(logstream);
	#endif

	if (devices[ix].ev)  {
		d = devices[ix].ev->get_decoder();
	}

	return d;
}							// get_decoder()


/**********************************************************************

**********************************************************************/

unsigned char get_control_byte(int _ix)  {

	EnumDeviceType what;
	unsigned char cb = 0xff;

	if (!devices[_ix].ev)  {
		return cb;
	}

	what = devices[_ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		return cb;
	}

	cb = devices[_ix].ev->get_control_byte_x();

	return cb;
}


/**********************************************************************

**********************************************************************/

Physics::MODE get_physics_mode(int ix)  {

	EnumDeviceType what;

	Physics::MODE mode = Physics::NOMODE;

	if (ix<0 || ix>255)  {
		return mode;
	}



	if (!devices[ix].ev)  {
		return mode;
	}

	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		return mode;
	}

	mode = devices[ix].ev->get_physics_mode_x();

	return mode;
}

/**********************************************************************

**********************************************************************/

int get_computrainer_mode(int ix)  {

	EnumDeviceType what;

	/*
	enum COMPUTRAINER_MODE  {
		HB_WIND_RUN = 0x2c,
		HB_WIND_PAUSE = 0x28,
		HB_RFTEST = 0x1c,
		HB_ERGO_RUN = 0x14,
		HB_ERGO_PAUSE = 0x10
	};
	*/

	int mode = -1;

	if (ix<0 || ix>255)  {
		return mode;
	}

	if (!devices[ix].ev)  {
		return mode;
	}

	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER)  {
		return mode;
	}

	mode = devices[ix].ev->get_computrainer_mode_x();				// returns the control byte

	return mode;
}

/**********************************************************************

**********************************************************************/

float get_manual_watts(int ix)  {

	EnumDeviceType what;
	float watts = 0.0f;

	if (!devices[ix].ev)  {
		return watts;
	}

	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		return watts;
	}

	watts = devices[ix].ev->get_manual_watts();

	return watts;
}

/**********************************************************************

**********************************************************************/

float get_slope(int ix)  {

	EnumDeviceType what;
	float slope = 0.0f;

	if (!devices[ix].ev)  {
		return slope;
	}

	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		return slope;
	}

	slope = devices[ix].ev->get_grade();

	return slope;
}

/**********************************************************************

**********************************************************************/

struct Bike::GEARPAIR get_gear_indices(int ix)  {

	int portnum;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_gear_indices(%d)\n", ix);
		fclose(logstream);
	#endif

	//ix = portnum - 1;
	portnum = ix + 1;

	if (!devices[ix].ev)  {
		Bike::GEARPAIR gp;
		gp.front = 0;
		gp.rear = 0;
		return gp;
	}

	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what==DEVICE_COMPUTRAINER)  {
		Bike::GEARPAIR gp;
		gp.front = 0;
		gp.rear = 0;
		return gp;
	}

	return devices[ix].ev->get_gear_indices();

}				// struct Bike::GEARPAIR get_gear_indices(int ix)

/**********************************************************************

**********************************************************************/

int set_constant_force(int ix, float _force)  {

	DEVICE *d;

	if (devices[ix].what != DEVICE_VELOTRON)  {
		return WRONG_DEVICE;
	}

	d = &devices[ix];

	if (!d->ev)  {
		return DEVICE_NOT_RUNNING;
	}

	d->ev->set_constant_force(_force);

	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

Physics *get_physics(int ix)  {

	DEVICE *d;

	if (devices[ix].what != DEVICE_VELOTRON)  {
		return NULL;
	}

	d = &devices[ix];

	if (!d->ev)  {
		return NULL;
	}

	return d->ev->get_physics();

}
/**********************************************************************

**********************************************************************/

unsigned long get_packet_error1(int ix)  {

	if (devices[ix].what != DEVICE_COMPUTRAINER)  {
		return 0;
	}

	DEVICE *d;

	d = &devices[ix];

	if (!d->ev)  {
		return 0;
	}

	return d->ev->get_packet_error1(ix);

	return 0;
}

/**********************************************************************

**********************************************************************/

unsigned long get_packet_error2(int ix)  {

	if (devices[ix].what != DEVICE_COMPUTRAINER)  {
		return 0;
	}

	DEVICE *d;

	d = &devices[ix];

	if (!d->ev)  {
		return 0;
	}

	return d->ev->get_packet_error2(ix);

	return 0;
}

/**********************************************************************

**********************************************************************/

unsigned long get_packet_error3(int ix)  {

	if (devices[ix].what != DEVICE_COMPUTRAINER)  {
		return 0;
	}

	DEVICE *d;

	d = &devices[ix];

	if (!d->ev)  {
		return 0;
	}

	return d->ev->get_packet_error3(ix);

	return 0;
}

/**********************************************************************

**********************************************************************/

unsigned long get_packet_error4(int ix)  {

	if (devices[ix].what != DEVICE_COMPUTRAINER)  {
		return 0;
	}

	DEVICE *d;

	d = &devices[ix];

	if (!d->ev)  {
		return 0;
	}

	return d->ev->get_packet_error4(ix);

	return 0;
}

/**********************************************************************

**********************************************************************/

bool is_started(int ix)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->is_started();
	}

	return false;
}

/**********************************************************************

**********************************************************************/

bool is_paused(int ix)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->is_paused();
	}

	return false;
}

/**********************************************************************

**********************************************************************/

void set_gloger(Logger *_glog)  {
	if (glog==NULL)  {
		glog = _glog;
	}

	return;
}


/**********************************************************************

**********************************************************************/

bool is_finished(int ix)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->is_finished();
	}

	return false;
}

/**********************************************************************

**********************************************************************/

int set_logging(int ix, int fw, LoggingType _logging_type)  {

	EnumDeviceType what=DEVICE_NOT_SCANNED;
	int comport = ix + 1;

	what = devices[ix].what;

	if (what==DEVICE_SIMULATOR)  {
		return ALL_OK;
	}

	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		what = check_for_trainers(comport);
		if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
			return WRONG_DEVICE;
		}
	}

	devices[ix].logging_type = _logging_type;

	if (devices[ix].ev)  {
		devices[ix].ev->set_logging(_logging_type);
	}

	return ALL_OK;
}

/**********************************************************************
	portnum starts at 0
	stop : Bar controller exits Pro-E mode.
	Application hangs waiting for return from stop, 1 core pinned at 100% ultilization as if in an infinite wait loop.

**********************************************************************/

const char *get_perf_file_name(int ix)  {

	const char *cptr;

	if (!devices[ix].ev)  {
		return NULL;
	}

	cptr = devices[ix].ev->get_perf_file_name();

	return cptr;
}


/**********************************************************************

**********************************************************************/

void set_file_mode(int ix, bool _mode)  {

	devices[ix].file_mode = _mode;
	gfile_mode = _mode;
	if (devices[ix].ev)  {
		devices[ix].ev->set_file_mode(_mode);
	}

	return;
}

/**********************************************************************

**********************************************************************/

void set_performance_file_name(int ix, const char *_perfname)  {

	if (!devices[ix].ev)  {
		return;
	}

	devices[ix].ev->set_performance_file_name(_perfname);

	return;
}

/**********************************************************************

**********************************************************************/

int set_rider_data(int ix, RIDER_DATA _rd)  {

	if (ix<0 || ix>255)  {
		return BAD_RIDER_INDEX;
	}

	devices[ix].rd = _rd;

	return ALL_OK;
}




/**********************************************************************

**********************************************************************/

int get_instances(void)  {

	return instances;
}

/**********************************************************************

**********************************************************************/

void set_bluetooth_delay(bool _b)  {

	bluetooth_delay = _b;
	return;
}

/**********************************************************************

**********************************************************************/

void set_logger(Logger *_logg)  {

	glog = _logg;
	return;
}

/**********************************************************************

**********************************************************************/

int set_manual_speed(int ix, int fw, float _manual_mph)  {

	DEVICE *d;

	d = &devices[ix];

	if (!d->ev)  {
		return DEVICE_NOT_RUNNING;
	}

	d->ev->set_manual_speed(_manual_mph);

	return ALL_OK;

}

/**********************************************************************

**********************************************************************/

dataSource *getds(int ix, int fw)  {

	if (devices[ix].ev)  {
		return devices[ix].ev->getds();
	}

	return NULL;
}

/**********************************************************************

**********************************************************************/

int set_dirs(std::vector<std::string> &_dirs)  {

	int i, n;

	static int cnt = 0;
	FILE *stream = NULL;

	cnt++;

#if 0
	stream = fopen("sd.txt", "wt");
	if (stream==NULL)  {
		return CAN_NOT_OPEN_FILE;
	}
#endif

if (stream) fprintf(stream, "sd  %d\n", cnt);
	n = _dirs.size();
if (stream) fprintf(stream, "n = %d\n", n);

	for(i=0; i<n; i++)  {
		dirs.push_back(_dirs[i].c_str());
		if (stream) fprintf(stream, "%s\n", _dirs[i].c_str());
	}

	n = dirs.size();
	if (stream)  {
		fprintf(stream, "n = %d\n", n);
		FCLOSE(stream);
	}

	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

int load_perf(int ix, int fw, const char *_fname)  {

	int status;

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	EnumDeviceType what;
	what = GetRacerMateDeviceID(ix);

	status = devices[ix].ev->load(_fname);
	if (status != 0)  {
		return GENERIC_ERROR;
	}

	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

void reset_encryptor(void)  {

	cryptor.init();
	return;
}

/**********************************************************************

**********************************************************************/

void encrypt(unsigned char *buf, int len)  {
	cryptor.doo(buf, len);
	return;
}

/**********************************************************************

**********************************************************************/

void decrypt(unsigned char *buf, int len)  {
	cryptor.doo(buf, len);
	return;
}


///#endif									// #if LEVEL >= DLL_FULL_ACCESS

// xxx

//#if LEVEL >= DLL_ERGVIDEO_ACCESS

/**********************************************************************

**********************************************************************/

int SetErgModeLoad(int ix, int FirmwareVersion, int RRC, float _watts)  {
	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "SetErgModeLoad(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		//return GENERIC_ERROR;
		return WRONG_DEVICE;
	}

	devices[ix].ev->set_watts(_watts);

	return ALL_OK;
}							// int SetErgModeLoad(int ix, int FirmwareVersion, int RRC, float _watts)  {

/**********************************************************************

**********************************************************************/

bool is_connected(void)  {

	return connected;
}							// bool is_connected(void)  {

/**********************************************************************
	sets the 'started' flag in decoder so that distance, speed
	and things that depend on them are calculated.
**********************************************************************/

	int start_trainer(int ix, bool _b)  {

	int realport;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "start_trainer(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	realport = ix + 1;				// 1 = exists, 3 = velotron, 4 = computrainer

	EnumDeviceType what;

	if (devices[ix].what == DEVICE_NOT_SCANNED)  {
		what = GetRacerMateDeviceID(ix);
		if (what==DEVICE_DOES_NOT_EXIST)  {
			return DEVICE_DOES_NOT_EXIST;
		}
		else if (what==DEVICE_OPEN_ERROR)  {
			return PORT_OPEN_ERROR;
		}
	}
	else if (devices[ix].what == DEVICE_DOES_NOT_EXIST)  {
		what = GetRacerMateDeviceID(ix);
		if ( haveDevice(what, realport) )  {
			return DEVICE_DOES_NOT_EXIST;
		}
		
		// device does not exist has timed out. see if it still doesn't exist and restart the timer:

		what = GetRacerMateDeviceID(ix);
		if (what == DEVICE_DOES_NOT_EXIST)  {
			return DEVICE_DOES_NOT_EXIST;
		}

	}

	what = devices[ix].what;

	if (what!=DEVICE_COMPUTRAINER && what!=DEVICE_VELOTRON && what!=DEVICE_SIMULATOR && what!=DEVICE_RMP)  {
		return WRONG_DEVICE;
	}

	//if (_b)  {
	devices[ix].ev->start(_b);

	return ALL_OK;
}							// start_trainer

/**********************************************************************

**********************************************************************/

int SetHRBeepBounds(int ix, int FirmwareVersion, int LowBound, int HighBound, bool BeepEnabled)  {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "SetHRBeepBounds(%d)\n", ix);
		fclose(logstream);
	#endif

	EnumDeviceType what, what2;
	what = devices[ix].what;

	if (what == DEVICE_COMPUTRAINER)  {
	}
	else if (what == DEVICE_VELOTRON )  {
	}
	else  {
		return WRONG_DEVICE;
	}

	devices[ix].rd.lower_hr = (float)LowBound;
	devices[ix].rd.upper_hr = (float)HighBound;
	devices[ix].beep_enabled = BeepEnabled;


	if (!devices[ix].ev)  {
		return ALL_OK;
	}

	what2 = devices[ix].ev->getwhat();
	if (what != what2)  {
		return GENERIC_ERROR;
	}

	devices[ix].ev->set_hr_bounds(LowBound, HighBound, BeepEnabled);			// always returns 0

	return ALL_OK;
}						// int SetHRBeepBounds(int ix, int FirmwareVersion, int LowBound, int HighBound, bool BeepEnabled)  {

/**********************************************************************

**********************************************************************/

int GetHandleBarButtons(int ix, int fw)  {

	int keys = 0;
	int i, newkeys = 0;
	int mask;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "GetHandleBarButtons(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return 0;
	}

	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		return 0;
	}

#ifdef _DEBUG
	if (what==DEVICE_COMPUTRAINER)  {
		if (fw != 4095 && fw != 144)  {						// 144 is norm's test rom
			return BAD_FIRMWARE_VERSION;
		}
	}
	else if (what==DEVICE_VELOTRON)  {
		if (fw != 190)  {
			return BAD_FIRMWARE_VERSION;
		}
	}
#endif


	keys = devices[ix].ev->get_keys();							// get the ACCUMULATED keys since last called

	if (what==DEVICE_COMPUTRAINER)  {
		mask = 0x01;

		connected = true;
		if (keys==0x40)  {												// connection lost, set bit 6
			connected = false;
			return 0x40;
		}
		// map computrainer keys to ergvid keys:

		/*
		Reset	0x01	->	0x01
		F2		0x10	->	0x04
		+		0x08	->	0x10
		F1		0x02	->	0x02
		F3		0x04	->	0x08
		-		0x20	->	0x20

		0x40		not connected

		int ctmask[6] = {
			0x01,
			0x02,
			0x08,
			0x10,
			0x04,
			0x20
		};
		*/


		for(i=0; i<6; i++)  {
			if (mask & keys)  {
				newkeys |= ctmask[i];
			}
			mask <<= 1;
		}
	}
	else  {
		// map velotron keys to ergvid keys:

		/*
B0: Reset/Stop key
B1: F1/Start key
B2: F2/Display key
B3: F3/Set key
B4: + key
B5: - key

		VT		VT		VT			EV			EV
		KEY		NAME	code		NAME		code

		1		F1		0x02		F1			0x02
		2		F2		0x04		F2			0x04
		3		up		0x08		+			0x10
		4		Fn		0x10		Reset		0x01
		5		F3		0x20		F3			0x08
		6		down	0x40		-			0x20


		int vtmask[6] = {
			0x02,
			0x04,
			0x10,
			0x01,
			0x08,
			0x20
		};
		*/

		if (!devices[ix].ev->is_connected())  {
			return 0x40;
		}

#ifdef _DEBUG
		if (keys!=0x00)  {
			//bp = 3;
		}
#endif

		mask = 0x02;

		for(i=0; i<6; i++)  {
			if (mask & keys)  {
				newkeys |= vtmask[i];
			}
			mask <<= 1;
		}
	}


	/*
	0x01	B0: Reset/Stop key					ok
	0x02	B1: F1/Start key					ok
	0x04	B2: F2/Display key					ok
	0x08	B3: F3/Set key						ok
	0x10	B4: + key							ok
	0x20	B5: - key							ok
	0x40	B6: Alarm! Connection lost.

	0x01	B0: Fn								ok
	0x02	B1: F1								ok
	0x04	B2: F2								ok
	0x08	B3: F3								ok
	0x10	B4: + key							ok
	0x20	B5: - key							ok
	*/

#ifdef _DEBUG
		if (keys!=0x00)  {
			//bp = 3;
		}
#endif

	return newkeys;
}							// int GetHandleBarButtons(int ix, int fw)

/**********************************************************************

**********************************************************************/

int SetRecalibrationMode(int ix, int fw)  {

	int comport;
	EnumDeviceType what;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "SetRecalibrationMode(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	what = devices[ix].what;
	comport = ix + 1;

	if ( !haveDevice(what, comport) )  {
		what = GetRacerMateDeviceID(comport-1);
		if ( !haveDevice(what, comport) )  {
			return GENERIC_ERROR;
		}
	}

	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		what = check_for_trainers(comport);
		if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
			return GENERIC_ERROR;
		}
	}

	if (what==DEVICE_COMPUTRAINER)  {
		//throw(fatalError(__FILE__, __LINE__));
		devices[ix].last_control_byte = devices[ix].ev->start_cal();
		calibrating = true;
		return ALL_OK;
	}
	else if (what==DEVICE_VELOTRON)  {
		//throw(fatalError(__FILE__, __LINE__));
 		calibrating = true;
		int status = devices[ix].last_control_byte = devices[ix].ev->start_cal();
		return status;
	}

	return GENERIC_ERROR;
}					// int SetRecalibrationMode(int ix, int fw)


/**********************************************************************

**********************************************************************/

int EndRecalibrationMode(int ix, int fw)  {

	int comport;
	EnumDeviceType what;

	calibrating = false;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "EndRecalibrationMode(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	what = devices[ix].what;
	comport = ix + 1;

	if ( !haveDevice(what, comport) )  {
		what = GetRacerMateDeviceID(ix);
		if ( !haveDevice(what, comport) )  {
			return GENERIC_ERROR;
		}
	}

	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		what = check_for_trainers(comport);
		if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
			return WRONG_DEVICE;
		}
	}

	if (what==DEVICE_COMPUTRAINER)  {
		devices[ix].ev->end_cal(devices[ix].last_control_byte);

		const char *cptr = get_computrainer_cal_from_thread(ix, fw);				// "2.00 U"
		strncpy(devices[ix].nv.cal, cptr, sizeof(devices[ix].nv.cal) );
	}
	else if (what==DEVICE_VELOTRON)  {
		devices[ix].ev->end_cal(devices[ix].last_control_byte);
		const char *cptr = get_velotron_cal_from_thread(ix, fw);					// "2.00 U"
		strncpy(devices[ix].nv.cal, cptr, sizeof(devices[ix].nv.cal) );

	}

	int cal = GetCalibration(ix);

	return cal;
}						// int EndRecalibrationMode(int ix, int fw)

/**********************************************************************

**********************************************************************/

int ResetALLtoIdle(void)  {

	int i, status;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "ResetAlltoIdle()\n");
		fclose(logstream);
	#endif

	for(i=0; i<256; i++)  {
		status = ResettoIdle(i);
	}

	return ALL_OK;
}							// int ResetALLtoIdle(void)  {

/**********************************************************************

**********************************************************************/

int get_accum_tdc(int ix, int fw)  {

	if (ix<0 || ix>255)  {
		return 0;
	}

	if (!devices[ix].ev)  {
		return 0;
	}

	int tdc = devices[ix].ev->get_accum_tdc();							// get the ACCUMULATED pulses since last specifically cleared by RENDERER!
	devices[ix].ev->clear_accum_tdc();

	return tdc;
}									// int get_accum_tdc(int ix, int fw)  {

/**********************************************************************
	this is the processed accum_tdc
**********************************************************************/

int get_tdc(int ix, int fw)  {

	if (ix<0 || ix>255)  {
		return 0;
	}

	if (!devices[ix].ev)  {
		return 0;
	}

	int tdc = devices[ix].ev->get_accum_tdc();							// get the ACCUMULATED pulses since last specifically cleared by RENDERER!
	devices[ix].ev->clear_accum_tdc();

	return tdc;
}								// int get_tdc(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

int SetVelotronParameters( int ix, int FWVersion, int _nfront, int _nrear, int* Chainrings, int* cogset, float tire_diameter_mm, int ActualChainring, int Actualcog, float bike_kgs, int front_index, int rear_index)  {

	int i;
	int portnum;

	if (_nfront>5 || _nrear>20)  {
		return BAD_GEAR_COUNT;
	}

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "SetVelotronParameters(ix), _nfront = %d, _nrear = %d\n", ix, _nfront, _nrear);
	fclose(logstream);
#endif

	portnum = ix + 1;
	memset(&devices[ix].bike_params, 0, sizeof(Bike::PARAMS));

	devices[ix].bike_params.real_front_teeth = ActualChainring;		// ActualChainring;
	devices[ix].bike_params.real_rear_teeth = Actualcog;			// Actualcog = Actualcog;
	devices[ix].bike_params.nfront = _nfront;						// Actualcog = Actualcog;
	devices[ix].bike_params.nrear = _nrear;						// Actualcog = Actualcog;
	devices[ix].bike_params.kgs = bike_kgs;
	devices[ix].bike_params.tire_diameter_mm = tire_diameter_mm;

	if (front_index>=0 && front_index<=2)  {
		devices[ix].bike_params.front_ix = front_index;
	}
	else  {
		devices[ix].bike_params.front_ix = 0;
	}

	if (rear_index>=0 && rear_index<=9)  {
		devices[ix].bike_params.rear_ix = rear_index;
	}
	else  {
		devices[ix].bike_params.rear_ix = 0;
	}

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "   OK1, _nfront = %d\n", _nfront);
	fclose(logstream);
#endif


	for(i=0; i<_nfront; i++)  {
		if (Chainrings[i]<=0)  {
			return BAD_TEETH_COUNT;
		}
		devices[ix].bike_params.front_gears[i] = Chainrings[i];
	}

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "   OK2\n");
	fclose(logstream);
#endif

	for(i=0; i<_nrear; i++)  {
		if (cogset[i]<=0)  {
			return BAD_TEETH_COUNT;
		}
		devices[ix].bike_params.rear_gears[i] = cogset[i];
	}


#ifdef ERGVIDLOG
	dump_bike_params(ix);
#endif

	return ALL_OK;
}								// SetVelotronParameters()

/**********************************************************************

**********************************************************************/

int setGear(int ix, int FWVersion, int front_index, int rear_index)  {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "setGear(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	devices[ix].ev->set_gear(front_index, rear_index);

	return ALL_OK;
}						// setGear()

/**********************************************************************

**********************************************************************/

struct Bike::GEARPAIR GetCurrentVTGear(int ix, int fw)  {

	int portnum;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "GetCurrentVTGear(%d)\n", ix);
		fclose(logstream);
	#endif

	portnum = ix + 1;

	if (!devices[ix].ev)  {
		Bike::GEARPAIR gp;
		gp.front = 0;
		gp.rear = 0;
		return gp;
	}



#ifdef _DEBUG
	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what==DEVICE_COMPUTRAINER)  {
		if (fw != 4095)  {
			throw(fatalError(__FILE__, __LINE__));
		}
	}
	else if (what==DEVICE_VELOTRON)  {
		if (fw != 190)  {
			throw(fatalError(__FILE__, __LINE__));
		}
	}
#endif

	return devices[ix].ev->get_gear_pair();

}				// struct Bike::GEARPAIR GetCurrentVTGear(int ix, int fw)

/**********************************************************************

**********************************************************************/

float *get_bars(int ix, int FWVersion)  {

	float *fptr;

	if (!devices[ix].ev)  {
		return NULL;
	}

	fptr = devices[ix].ev->get_bars();

	return fptr;
}								// get_bars()



/**********************************************************************

**********************************************************************/

float get_calories(int ix, int fw)  {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_calories(%d, %d)\n", ix, fw);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return -1.0f;
	}

	return devices[ix].ev->get_calories();

}				// float get_calories()

/**********************************************************************

**********************************************************************/

float get_np(int ix, int fw)  {

	if (!devices[ix].ev)  {
		return -1.0f;
	}
	return devices[ix].ev->get_x_np();
}										// float get_np(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

float get_if(int ix, int fw)  {
	if (!devices[ix].ev)  {
		return -1.0f;
	}
	return devices[ix].ev->get_if();
}							// float get_if(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

float get_tss(int ix, int fw)  {

	if (!devices[ix].ev)  {
		return -1.0f;
	}
	return devices[ix].ev->get_tss();
}					// float get_tss(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

float get_pp(int ix, int fw)  {
	if (!devices[ix].ev)  {
		return -1.0f;
	}
	return devices[ix].ev->get_pp();
}					// float get_pp(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

int set_ftp(int ix, int fw, float _ftp)  {

	EnumDeviceType what;



	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "set_ftp(%d)\n", ix);
		fclose(logstream);
	#endif


	what = devices[ix].what;
	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		return WRONG_DEVICE;
	}

	devices[ix].rd.ftp = _ftp;

	if (devices[ix].ev)  {
		devices[ix].ev->set_ftp(_ftp);
		return ALL_OK;
	}

	return DEVICE_NOT_RUNNING;
}									// int set_ftp(int ix, int fw, float _ftp)  {

/**********************************************************************

**********************************************************************/

int set_wind(int ix, int fw, float _wind_kph)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_wind(_wind_kph);
	return ALL_OK;
}									// int set_wind(int ix, int fw, float _wind_kph)  {

/**********************************************************************

**********************************************************************/

int set_draftwind(int ix, int fw, float _draft_wind_kph)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_draft_wind(_draft_wind_kph);
	return ALL_OK;
}							// int set_draftwind(int ix, int fw, float _draft_wind_kph)  {

/**********************************************************************

**********************************************************************/

int update_velotron_current(int ix, unsigned short pic_current)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	if (devices[ix].what != DEVICE_VELOTRON)  {
		return WRONG_DEVICE;
	}

	devices[ix].ev->send_current(pic_current);


	/*
	if (flag)  {
		devices[ix].ev->send_current(667);
	}
	else  {
		devices[ix].ev->send_current(0);
	}
	*/

	return ALL_OK;
}							// int update_velotron_current(int ix, unsigned short pic_current)  {

/**********************************************************************
	enters here when mph falls from 23.0 mph to 22.0 mph
**********************************************************************/

int velotron_calibration_spindown(int ix, int fw)  {

	float mph;
	int keys = 0x40;
#ifdef WIN32
	int c;
#endif

	int bp = 0;
	bool sentcal = false;
	int thiscal;
	unsigned long start_time, now;
	int status;
	TrainerData td;
	unsigned long dt;
#ifdef _DEBUG
	int cal;
	unsigned long last_disp_time = 0L;
	//AverageTimer at("spindown");
#endif
	unsigned short pic_current = 0;
	int rc = ALL_OK;


	if (devices[ix].ev==NULL)  {
		rc =  DEVICE_NOT_RUNNING;
		goto done;
	}
	if (devices[ix].what != DEVICE_VELOTRON)  {
		rc = WRONG_DEVICE;
		goto done;
	}

	// assuming that the mph is close to 22.0f mph

	td = GetTrainerData(ix, fw);
	mph = (float)(TOMPH*td.kph);

	printf("\n");
	printf("velotron_calibration_spindown, mph = %.2f\n", mph);


	if (mph < UPPER_SPEED-.02f)  {									// 22.0f
		rc = BELOW_UPPER_SPEED;
		goto done;
	}


#ifdef _DEBUG
	cal = GetCalibration(ix);								// factory cal
#endif

	status = startCal(ix, fw);						// sets 2.0 amps: calls ds->startCal(667);
	if (status!=ALL_OK)  {
		rc = status;
		goto done;
	}
	sentcal = true;

	start_time = timeGetTime();

	keys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call


	while(1)  {
#ifdef WIN32
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
				continue;
			}
			else if (c=='r')  {
				ResetAverages(ix, fw);
			}
			else if (c==VK_ESCAPE)  {
				rc = ABORTED;
				goto done;
			}
			else if (c==VK_SPACE)  {
				pic_current = 667;
			}
		}
#endif

		now = timeGetTime();
		dt = now - start_time;

		if ( dt > 130000)  {										// 100 seconds?
#ifdef _DEBUG
			printf("************** TIMEOUT ******************\n");
#endif
			rc = TIMEOUT;
			goto done;
		}

#ifdef _DEBUG
		//at.update();												// .12 ms
#endif

		td = GetTrainerData(ix, fw);
		mph = (float)(TOMPH*td.kph);
		keys = GetHandleBarButtons(ix, fw);					// keys should be accumulated keys pressed since last call

		switch(keys)  {
			case 0x00:
				bp =  3;
				break;
			case 0x40:						// not connected
				bp =  1;
				break;
			case 0x01:						// reset
				bp =  0;
				break;
			case 0x04:						// f2
				bp =  1;
				break;
			case 0x10:						// +
				bp = 77;
				//flag = false;
				break;
			case 0x02:						// f1
				bp =  1;
				break;
			case 0x08:						// f3
				bp =  1;
				break;
			case 0x20:						// -
				bp =  1;
				break;
			default:
				bp =  99;
				break;
		}
		thiscal = get_calibration(ix);										// this is the current test, not the factory cal
 
		if (thiscal != 0)  {
			rc = thiscal;
			goto done;
		}

#ifdef _DEBUG
		if ( (now - last_disp_time) >= 500)  {							// sample data and keyboard once per seocond
			if (!sentcal)  {
				last_disp_time = now;
				if (mph>=UPPER_SPEED + .5f)  {							// 22.5f
					printf("mph = %.2f / %hd ()()()()()()\n", mph, pic_current);
				}
				else if (mph>=SPEEDUP_SPEED)  {								// 23.0f
					printf("mph = %.2f / %hd ()()()()()()()()()()()()()(\n", mph, pic_current);
				}
				else  {
					printf("mph = %.2f / %hd ()()()()()()()()()()()()()()()()()()()()()()\n", mph, pic_current);
				}
			}
		}
#endif

	}										// while(1)

done:
	if (sentcal)  {
		endCal(ix, fw);
	}

	return rc;

}										// int velotron_calibration_spindown(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

int get_status_bits(int ix, int fw)  {
	if (!devices[ix].ev)  {
		return -1;
	}
	return devices[ix].ev->get_status_bits();
	//return 0;
}						// int get_status_bits(int ix, int fw)  {

/**********************************************************************

**********************************************************************/

void set_use_speed_cadence_sensor(int ix, int _b)  {
	if (ix<0 || ix>255)  {
		return;
	}

	devices[ix].use_speed_cadence_sensor = _b;


	/*
	if (!devices[ix].ev)  {
		return;
	}
	devices[ix].ev->set_use_speed_cadence_sensor(_b);
	*/

	return;
}							// void set_use_speed_cadence_sensor(int ix, int _b)  {

//#endif						// #if LEVEL >= DLL_ERGVIDEO_ACCESS


//#if LEVEL >= DLL_CANNONDALE_ACCESS

/**********************************************************************

**********************************************************************/

int Setlogfilepath(const char *dir)  {
	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "Setlogfilepath(%s)\n", dir);
		fclose(logstream);
	#endif

	if (glog)  {
		glog->write(10,0,1,"slfp1\n");
	}

	check_dirs(dir);

	if (glog)  {
		glog->write(10,0,1,"slfp2\n");
	}

	dirs[DIR_DEBUG] = dir;
	if (glog)  {
		glog->write(10,0,1,"slfp3\n");
	}

	if (!direxists(dir))  {
		if (glog)  {
			glog->write(10,0,1,"slfp4\n");
		}
		return DIRECTORY_DOES_NOT_EXIST;
	}

	if (glog)  {
		glog->write(10,0,1,"slfp ok\n");
	}

	return ALL_OK;
}									// Setlogfilepath()
	
/**********************************************************************

**********************************************************************/

int Enablelogs(bool _bikelog, bool _courselog, bool _decoderlog, bool _dslog, bool _gearslog, bool _physicslog)  {

#ifdef _DEBUG
	//throw(fatalError(__FILE__, __LINE__));
	//throw(1);					// works
	//throw("set_dirs");		// works
#endif

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "Enablelogs():\n");
		fprintf(logstream, "   bikelog = %s\n", _bikelog?"true":"false");
		fprintf(logstream, "   courselog = %s\n", _courselog?"true":"false");
		fprintf(logstream, "   decoderlog = %s\n", _decoderlog?"true":"false");
		fprintf(logstream, "   dslog = %s\n", _dslog?"true":"false");
		fprintf(logstream, "   gearslog = %s\n", _gearslog?"true":"false");
		fprintf(logstream, "   physicslog = %s\n", _physicslog?"true":"false");
		fclose(logstream);
	#endif

	check_dirs(NULL);


	if ( !direxists(dirs[DIR_DEBUG].c_str()) )  {
		// 	need to call Setlogfilepath("x:\\_fS\\ev_dll\\cpp_tester\\logs") first!!!
		Bike::gblog = false;
		//courselog = false;
		Decoder::decoderlog = false;
		dataSource::gblog = false;
		Bike::gbgearslog = false;
		Physics::gblog = false;
		return DIRECTORY_DOES_NOT_EXIST;
	}

	Bike::gblog = _bikelog;
	//courselog = _courselog;
	Decoder::decoderlog = _decoderlog;
	dataSource::gblog = _dslog;
	Bike::gbgearslog = _gearslog;
	Physics::gblog = _physicslog;


	return ALL_OK;
}							// Enablelogs()

/**********************************************************************

**********************************************************************/

int GetFirmWareVersion(int ix)  {
	const char *cptr;
	EnumDeviceType what=DEVICE_NOT_SCANNED;
	int comport;
	int n;
	float f;
	int version;

#ifdef FAKE_HB
	return 240;
#endif

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "GetFirmWareVersion(%d)\n", ix);
	fclose(logstream);
#endif

	comport = ix + 1;

	what = devices[ix].what;

	if ( !haveDevice(what, comport) )  {

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "gfwv 1\n");
	fclose(logstream);
#endif
		what = GetRacerMateDeviceID(ix);
		
#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "gfwv 2\n");
	fclose(logstream);
#endif
		if ( !haveDevice(what, comport) )  {
			return GENERIC_ERROR;
		}
	}

	if (what==DEVICE_COMPUTRAINER)  {
		cptr = get_computrainer_firmware(comport);
		n = sscanf(cptr, "%f", &f);
		if (n != 1)  {
			return GENERIC_ERROR;
		}
		version = (int) f;
		return version;
	}
	else if (what==DEVICE_VELOTRON)  {
		cptr = get_velotron_firmware(comport);
		n = sscanf(cptr, "%f", &f);
		if (n != 1)  {
			return GENERIC_ERROR;
		}
		version = ROUND(f*100);
		return version;
	}
	else if (what==DEVICE_SIMULATOR)  {
		version = 1;
		return version;
	}
	else if (what==DEVICE_RMP)  {
		version = 1;
		return version;
	}
	else if (what==DEVICE_DOES_NOT_EXIST)  {
		return PORT_DOES_NOT_EXIST;
	}
	else if (what==DEVICE_EXISTS)  {
		return PORT_EXISTS_BUT_IS_NOT_A_TRAINER;
	}
	else if (what==DEVICE_OPEN_ERROR)  {
		return PORT_OPEN_ERROR;
	}
	else  {
		return GENERIC_ERROR;
	}

}				// int GetFirmWareVersion(int _portnum)

/**********************************************************************

**********************************************************************/

const char *get_dll_version(void)  {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_dll_version()\n");
		fclose(logstream);
	#endif

	return DLL_VERSION;

}				// int get_dll_version(int _portnum)

/**********************************************************************

**********************************************************************/

const char *GetAPIVersion(void)  {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "GetAPIVersion()\n");
		fclose(logstream);
	#endif
	return API_VERSION;
}					// GetAPIVersion()

/**********************************************************************
	_comport starts at 1
**********************************************************************/

EnumDeviceType check_for_trainers(int _comport)  {
	int istat;
	char baudstr[16];
	int baudrate;
	Serial *port;
	//int i, bp = 0;
	DEVICE d = {DEVICE_NOT_SCANNED};
	char comstr[32] = {0};

	if (glog)  {
		glog->write(10,0,1,"check_for_trainers(%d)\n", _comport);
	}

	//-----------------------------
	// check for computrainer:
	//-----------------------------

  	baudrate = 2400;

	sprintf(baudstr,"%d", baudrate);
	sprintf(comstr,"COM%d", _comport);


	DWORD now, diff;
	now = timeGetTime();

	diff = now - last_port_time;

	if ( diff < 500)  {
		//if (bluetooth_delay)  {
			Sleep(diff);
		//}
	}
	last_port_time = timeGetTime();

        
#ifdef WIN32
	/*
	ANT *ant = new ANT(0, 1, NULL);

#ifdef _DEBUG
	unsigned long tid = GetCurrentThreadId();
	tid = ant->get_tid();									// same thread
#endif

	UCHAR ucDeviceNumber;
	UCHAR ucChannelType;

	ucDeviceNumber = 0;
	ucChannelType = 1;

#ifdef _DEBUG
	HANDLE procHeap;
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif
	ant->run();
#ifdef _DEBUG
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif
	DEL(ant);
	*/

    try  {
        port = new Serial(bluetooth_delay, comstr, baudstr, FALSE, NULL);
    }
    catch (const int istatus) {
		if (glog)  {
			glog->write(10,0,1,"   catch(%d)\n", istatus);
		}

		switch(istatus)  {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
				d.what = DEVICE_DOES_NOT_EXIST;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;

			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				d.what = DEVICE_ACCESS_DENIED;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;

			case SERIAL_ERROR_BUILDCOMMDCB:
				//bp = 1;
			case SERIAL_ERROR_SETCOMMSTATE:
				//bp = 1;
			case SERIAL_ERROR_CREATEEVENT1:
				//bp = 1;
			case SERIAL_ERROR_CREATEEVENT2:
				//bp = 1;
			case SERIAL_ERROR_OTHER:
				d.what = DEVICE_OPEN_ERROR;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;

			default:
				d.what = DEVICE_OTHER_ERROR;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;
		}							// switch
	}								// catch
	catch(...)  {
		if (glog)  {
			glog->write(10,0,1,"   catch...\n");
		}
	}
#else
	port = new Serial(comstr, baudstr, NULL);

        
        if (!port->is_open())  {
            DEL(port);
            d.what = DEVICE_DOES_NOT_EXIST;
            d.port = 0;
            strcpy(d.nv.ver, "n/a");
            strcpy(d.nv.cal, "n/a");
            d.when = timeGetTime();
            devices[_comport-1] = d;
            return d.what;
        }

#endif				// #ifdef WIN32


	if (glog)  {
		glog->write(10,0,1,"   serial port opened, checking for CT\n");
	}

	port->send("RacerMate", 0);						// no flush

	istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found

	if (glog)  {
		glog->write(10,0,1,"   expect istat = %d\n", istat);
		if (istat==0)  {
			glog->write(10,0,1,"   CT found\n");
		}
	}

	port->setShutDownDelay(10);

	if (istat==0)  {
		read_computrainer(port);						// fills in the global info for this computrainer
		flush(NULL, port, 500, FALSE);
		DEL(port);										// hangs here
		return DEVICE_COMPUTRAINER;
	}
	else  {
		DEL(port);										// <<<<<<<<<<<<<<<<<<<<< hangs here
	}


	//-------------------------------------------------------------
	// check for velotrons
	//-------------------------------------------------------------

	if (glog)  {
		glog->write(10,0,1,"   checking for Velotron\n");
	}

  	baudrate = 38400;

	sprintf(baudstr,"%d",baudrate);
	sprintf(comstr,"COM%d",_comport);


#ifdef WIN32
	now = timeGetTime();
	diff = now - last_port_time;
	if ( diff < 500)  {
		Sleep(diff);
	}
	last_port_time = timeGetTime();

	try  {
		//port = new Serial(comstr, baudstr, FALSE, NULL);

		#ifdef _DEBUG
			port = new Serial(true, comstr, baudstr, FALSE, NULL);							// showing errors
		#else
			port = new Serial(true, comstr, baudstr, FALSE, NULL);
		#endif
	}
	catch (const int istatus) {
		switch(istatus)  {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
				d.what = DEVICE_DOES_NOT_EXIST;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;

			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				//bp = 1;
				d.what = DEVICE_ACCESS_DENIED;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;

			case SERIAL_ERROR_BUILDCOMMDCB:
			case SERIAL_ERROR_SETCOMMSTATE:
			case SERIAL_ERROR_CREATEEVENT1:
			case SERIAL_ERROR_CREATEEVENT2:
			case SERIAL_ERROR_OTHER:
				d.what = DEVICE_OPEN_ERROR;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;

			default:
				d.what = DEVICE_OTHER_ERROR;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;
		}
	}

#else
	port = new Serial(comstr, baudstr, NULL);
#endif

	// not catching here because I already know that the port exists and is not a computrainer

	port->setShutDownDelay(10);

	if (isVelotron(port, 500))  {
		read_velotron(port);						// fills in the global info for this computrainer
		DEL(port);
		return DEVICE_VELOTRON;
	}
	DEL(port);

	// port exists, but not a computrainer or velotron

	d.what = DEVICE_EXISTS;
	d.port = _comport;
	strcpy(d.nv.ver, "n/a");
	strcpy(d.nv.cal, "n/a");
	d.when = timeGetTime();
	devices[_comport-1] = d;

	return d.what;
}											// check_for_trainers()

/**********************************************************************

**********************************************************************/

EnumDeviceType GetRacerMateDeviceID(int ix)  {
	EnumDeviceType what;
	int comport;

	if (glog) glog->write(10,0,1,"GetRacerMateDeviceID(%d)\n", ix);

	if (gfile_mode)  {
		if (glog) glog->write(10,0,1,"file_mode\n");
		if (devices[ix].file_mode)  {
			what = DEVICE_RMP;
			devices[ix].what = what;
			devices[ix].port = ix + 1;
			devices[ix].when = timeGetTime();
			return what;
		}
	}

	if (ix==SIMULATOR_DEVICE)  {					// 255?
		what = DEVICE_SIMULATOR;
		devices[ix].what = what;
		devices[ix].port = ix + 1;
		devices[ix].when = timeGetTime();
		return what;
	}
	else if (ix==PERF_DEVICE)  {					// 254?
		what = DEVICE_RMP;
		devices[ix].what = what;
		devices[ix].port = ix + 1;
		devices[ix].when = timeGetTime();
		return what;
	}

	comport = ix + 1;

	what = devices[ix].what;
	if (glog) glog->write(10,0,1,"grmid1\n");

	if ( haveDevice(what, comport) )  {
		if (glog) glog->write(10,0,1,"grmid2\n");
		return what;
	}

	if (glog) glog->write(10,0,1,"grmid3\n");
	what = check_for_trainers(comport);

	if (glog) glog->write(10,0,1,"grmidx\n");
	return what;

}					// EnumDeviceType GetRacerMateDeviceID(int ix)  {

/**********************************************************************

**********************************************************************/
bool GetIsCalibrated(int ix, int FirmwareVersion)  {
	int i;
	EnumDeviceType what;
	int portnum;
	//const char *cptr;

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "GetIsCalibrated(%d)\n", ix);
	fclose(logstream);
#endif

	i = ix;
	portnum = ix + 1;

	what = devices[i].what;
	if ( haveDevice(what, portnum) )  {

		if (what!=DEVICE_COMPUTRAINER && what!=DEVICE_VELOTRON)  {
			return false;
		}
		return iscal(what, portnum);
	}

	what = check_for_trainers(portnum);
	#ifdef WIN32
	assert(what>=0 && what<=DEVICE_OTHER_ERROR);
	#endif

	if (what!=DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON)  {
		return false;
	}

	return iscal(what, portnum);

}					// bool GetIsCalibrated(int _portnum)  {

/**********************************************************************
	comport starts at 0
**********************************************************************/

int GetCalibration(int ix)  {
	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "GetCalibration(%d)\n", ix);
		fclose(logstream);
	#endif

	int n, cal, comport;
	char str[32];
	float f;
	const char *cptr;
	EnumDeviceType what=DEVICE_NOT_SCANNED;
		
	comport = ix + 1;
	//i = ix;
	what = devices[ix].what;
	


	if ( !haveDevice(what, comport) )  {
		what = GetRacerMateDeviceID(ix);
		if ( !haveDevice(what, comport) )  {
			return GENERIC_ERROR;
		}

	}

	if (what==DEVICE_DOES_NOT_EXIST)  {
		return DEVICE_DOES_NOT_EXIST;
	}
	else if (what==DEVICE_EXISTS)  {
		return DEVICE_EXISTS;
	}
	else if (what==DEVICE_COMPUTRAINER)  {
		cptr = devices[ix].nv.cal;
		n = sscanf(cptr, "%f %s", &f, str);
		if (n!=2)  {
			return GENERIC_ERROR;
		}
		cal = (int) (100.0f*f);
		return cal;
	}
	else if (what==DEVICE_VELOTRON)  {
		cptr = devices[ix].nv.cal;
		long vtcal;
		n = sscanf(cptr, "%ld", &vtcal);
		if (n!=1)  {
			return GENERIC_ERROR;
		}
		return vtcal;
	}


	what = devices[ix].what;
	if ( haveDevice(what, comport) )  {
		return GENERIC_ERROR;
	}

	what = check_for_trainers(comport);
	if (what!=DEVICE_COMPUTRAINER)  {
		return GENERIC_ERROR;
	}

	cptr = get_computrainer_cal(comport);
	n = sscanf(cptr, "%f %s", &f, str);
	if (n!=2)  {
		return GENERIC_ERROR;
	}
	cal = ROUND(100.0f*f);
	return cal;
}											// GetCalibration()

/**********************************************************************

**********************************************************************/

const char *get_errstr(int err)  {

	const char *cptr;
	int i;



	if (err==0)  {
		return err_strs[0];
	}


	if (err>GENERIC_ERROR || err>0)  {
		return err_strs[NERRORS-1];
	}

	i = err - INT_MIN + 1;

#ifdef _DEBUG
	if (i>NERRORS-1)  {
		throw(fatalError(__FILE__, __LINE__));
	}
#else
	if (i>NERRORS-1)  {
		i = NERRORS-1;
	}
#endif


	cptr = err_strs[i];

	return cptr;
}								// const char *get_errstr(int err)

/**********************************************************************

**********************************************************************/

int resetTrainer(int ix, int _fw, int _cal)  {
	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "resetTrainer(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	EnumDeviceType what;

	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON)  {
		return GENERIC_ERROR;
	}

	devices[ix].ev->reset();

	return ALL_OK;
}							// int resetTrainer(int ix, int _fw, int _cal)

/**********************************************************************

**********************************************************************/

int startTrainer(int ix, Course *_course, LoggingType _logging_type)  {
	int port;
	//int i;

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "startTrainer(%d)\n", ix);
		fclose(logstream);
	#endif

	devices[ix].logging_type = _logging_type;

	if (devices[ix].ev)  {				// this trainer is already started
		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   returning trainer already started\n");
			fclose(logstream);
		#endif
		return DEVICE_ALREADY_RUNNING;
	}

	devices[ix].course = _course;

	port = ix + 1;				// 1 = exists, 3 = velotron, 4 = computrainer

	EnumDeviceType what;

	if (devices[ix].what == DEVICE_NOT_SCANNED)  {
		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   device not scanned\n");
			fclose(logstream);
		#endif
		what = GetRacerMateDeviceID(ix);

		if (what==DEVICE_DOES_NOT_EXIST)  {
			#ifdef ERGVIDLOG
				logstream = fopen(ERGVIDFILE, "a+t");
				fprintf(logstream, "   returning device does not exist: %d\n", DEVICE_DOES_NOT_EXIST);
				fclose(logstream);
			#endif
			return DEVICE_DOES_NOT_EXIST;
		}
		else if (what==DEVICE_OPEN_ERROR)  {
			#ifdef ERGVIDLOG
				logstream = fopen(ERGVIDFILE, "a+t");
				fprintf(logstream, "   returning port open error: %d\n", PORT_OPEN_ERROR);
				fclose(logstream);
			#endif
			return PORT_OPEN_ERROR;
		}
	}
	else if (devices[ix].what == DEVICE_DOES_NOT_EXIST)  {
		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   device was already scanned\n");
			fclose(logstream);
		#endif

		what = GetRacerMateDeviceID(ix);
		if ( haveDevice(what, port) )  {

			#ifdef ERGVIDLOG
				logstream = fopen(ERGVIDFILE, "a+t");
				fprintf(logstream, "   returning device does not exist 2: rc = %d\n", DEVICE_DOES_NOT_EXIST);		// 1 = serial port does not exist
				fprintf(logstream, "   realport = %d\n", realport);
				if (what==DEVICE_COMPUTRAINER)  {
					fprintf(logstream, "   what = COMPUTRAINER\n");
				}
				else if (what==DEVICE_VELOTRON)  {
					fprintf(logstream, "   what = VELOTRON\n");
				}
				else  {
					fprintf(logstream, "   what = ???\n");
				}

				fprintf(logstream, "   realport = %d\n", realport);
				fclose(logstream);
			#endif

			return DEVICE_DOES_NOT_EXIST;
		}
		
		// device does not exist has timed out. see if it still doesn't exist and restart the timer:

		what = GetRacerMateDeviceID(ix);
		if (what == DEVICE_DOES_NOT_EXIST)  {
			#ifdef ERGVIDLOG
				logstream = fopen(ERGVIDFILE, "a+t");
				fprintf(logstream, "   returning device does not exist 3: %d\n", DEVICE_DOES_NOT_EXIST);
				fclose(logstream);
			#endif
			return DEVICE_DOES_NOT_EXIST;
		}

	}

	what = devices[ix].what;

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "   ok1\n");
	fclose(logstream);
#endif

//#ifdef EXTENDED_DLL
//#if LEVEL >= DLL_FULL_ACCESS
	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		instances++;
	}
//#endif

	if (what==DEVICE_COMPUTRAINER)  {
		//EV(int _realportnum, EnumDeviceType _what, Bike::PARAMS *_params=NULL, Course *_course=NULL);
		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   ok2a: device is computrainer\n");
			fclose(logstream);
		#endif

		// devices[ix].rd is the rider data
		devices[ix].ev = new EV(port, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
		//ds = getds();

		devices[ix].ev->set_use_speed_cadence_sensor(devices[ix].use_speed_cadence_sensor);

		devices[ix].ev->set_file_mode(gfile_mode);


		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   ok2b\n");
			fclose(logstream);
		#endif
	}
	else if (what==DEVICE_VELOTRON)  {
		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   ok2\n");
			fclose(logstream);
		#endif

		if (devices[ix].bike_params.real_front_teeth==0)  {					// ActualChainRing?
			#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   startTrainer error:\n");
			fclose(logstream);

			dump_bike_params(ix);
			#endif
			return VELOTRON_PARAMETERS_NOT_SET;
		}

		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   ok6, realport = %d\n", realport);
			fclose(logstream);
		#endif

		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   starting thread\n");
			fclose(logstream);
			dump_bike_params(ix);
		#endif

		devices[ix].ev = new EV(port, DEVICE_VELOTRON, devices[ix].rd, &devices[ix].bike_params, _course, devices[ix].logging_type);

//#ifdef EXTENDED_DLL
//#if LEVEL >= DLL_FULL_ACCESS
		devices[ix].ev->set_logging(devices[ix].logging_type);
//#endif

		#ifdef ERGVIDLOG
			logstream = fopen(ERGVIDFILE, "a+t");
			fprintf(logstream, "   back from thread start\n");
			fclose(logstream);
			dump_bike_params(ix);
		#endif
	}
	else if (what==DEVICE_SIMULATOR)  {
		devices[ix].ev = new EV(port, DEVICE_SIMULATOR, devices[ix].rd, NULL, _course);
	}
	else if (what==DEVICE_RMP)  {
		devices[ix].ev = new EV(port, DEVICE_RMP, devices[ix].rd, NULL, NULL);
		// need to get the perf file name into EV::ds
	}
	else  {
		return WRONG_DEVICE;
	}



	if (!devices[ix].ev->initialized)  {
		DEL(devices[ix].ev);
		//return GENERIC_ERROR;								// <<<<<<<<<<<<<<<<< blew up here
		return DEVICE_NOT_INITIALIZED;
	}

	Sleep(100);


	devices[ix].ev->set_hr_bounds( ROUND(devices[ix].rd.lower_hr), ROUND(devices[ix].rd.upper_hr), devices[ix].beep_enabled);
	devices[ix].ev->set_ftp(devices[ix].rd.ftp);


#ifndef DO_EV_EVENT
	// wait  to make sure that the thread is running:	while (!devices[ix].ev->get_running())  {
	}
#endif
#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "   startTrainerx\n");
	fclose(logstream);
	dump_bike_params(ix);
#endif

	// the trainer is running, but decoder::started is false... you need to 'start' it if you want the decoder to accumulate time and distance.

	return ALL_OK;
}							// startTrainer

/**********************************************************************

**********************************************************************/

int ResetAverages(int ix, int fw)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	devices[ix].ev->reset_averages();

	return ALL_OK;
}								// ResetAverages()

/**********************************************************************
typedef struct TrainerData	{
	float speed;	//ALWAYS in MPH, application will metric convert. <0 on error
	float cadence; //in RPM, any number <0 if sensor not connected or errored.
	float HR;		//in BPM, any number <0 if sensor not connected or errored.
	float Power;	//in Watts <0 on error
};

**********************************************************************/

struct TrainerData GetTrainerData(int ix, int fw)  {
	TrainerData td, *ptd;

#ifdef _DEBUG
	if (devices[ix].ev==NULL)  {
		memset(&td, 0, sizeof(td));
		return td;
	}

	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what==DEVICE_COMPUTRAINER)  {
		//if (fw != 4095)  {
		//	throw(fatalError(__FILE__, __LINE__));
		//}
	}
	else if (what==DEVICE_VELOTRON)  {
		if (fw != 190)  {
			throw(fatalError(__FILE__, __LINE__));
		}
	}
#endif

	if (devices[ix].ev)  {
		if (calibrating)  {
			//throw(fatalError(__FILE__, __LINE__));
			ptd = devices[ix].ev->fastread();
			//ptd = devices[ix].ev->read();
		}
		else  {
			//throw(fatalError(__FILE__, __LINE__));
			ptd = devices[ix].ev->myread();
		}
		memcpy(&td, ptd, sizeof(TrainerData));
	}
	else  {
		//throw(fatalError(__FILE__, __LINE__));
		memset(&td, 0, sizeof(td));
	}

	return td;
}											// GetTrainerData()

/**********************************************************************

**********************************************************************/

float *get_average_bars(int ix, int FWVersion)  {

	float *fptr;

	if (!devices[ix].ev)  {
		return NULL;
	}

	fptr = devices[ix].ev->get_average_bars();

	return fptr;
}									// get_average_bars()


/**********************************************************************

**********************************************************************/

struct SSDATA get_ss_data(int ix, int fw)  {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "get_ss_data(%d)\n", ix);
		fclose(logstream);
	#endif

#ifdef _DEBUG
	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what==DEVICE_COMPUTRAINER)  {
		if (fw != 4095)  {
			throw(fatalError(__FILE__, __LINE__));
		}
	}
	else if (what==DEVICE_VELOTRON)  {
		if (fw != 190)  {
			throw(fatalError(__FILE__, __LINE__));
		}
	}
#endif
	SSDATA ssd, *pssd;

	if (devices[ix].ev)  {
		pssd = devices[ix].ev->read_ss();
		memcpy(&ssd, pssd, sizeof(SSDATA));
	}
	else  {
		memset(&ssd, 0, sizeof(ssd));
	}

	return ssd;
}							// get_ss_data()

/**********************************************************************

**********************************************************************/

int SetSlope( int ix, int fw, int cal, float bike_kgs, float person_kgs, int _drag_factor, float grade)   {

	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "SetSlope(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	EnumDeviceType what;
	what = devices[ix].ev->getwhat();

	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		return GENERIC_ERROR;
	}

	//float kgs;
	//kgs = bike_kgs + person_kgs;

	devices[ix].ev->set_slope(bike_kgs, person_kgs, _drag_factor, grade);			// sets windload mode
	devices[ix].rd.watts_factor = devices[ix].ev->get_watts_factor();
	return ALL_OK;
}							// SetSlope()

/**********************************************************************

	returns:
		0 = OK

	errors:
		1 = thread not running
		2 = not a valid device

**********************************************************************/

int setPause(int ix, bool _paused)  {

	EnumDeviceType what;


	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "setPause(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return GENERIC_ERROR;
	}

	EV *ev = devices[ix].ev;
	//Decoder *decoder = ev->get_decoder();

	what = devices[ix].what;
	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		return GENERIC_ERROR;
	}

	//comport = _ix + 1;

	bool p = ev->is_paused();
	if (p==_paused)  {
		return ALL_OK;
	}

	devices[ix].ev->set_paused(_paused);


	return ALL_OK;
}										// int setPause(int ix, bool _paused)  {


/**********************************************************************
	portnum starts at 0
	stop : Bar controller exits Pro-E mode.
	Application hangs waiting for return from stop, 1 core pinned at 100% ultilization as if in an infinite wait loop.

**********************************************************************/

int stopTrainer(int ix)  {
	#ifdef ERGVIDLOG
		logstream = fopen(ERGVIDFILE, "a+t");
		fprintf(logstream, "stopTrainer(%d)\n", ix);
		fclose(logstream);
	#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	EnumDeviceType what;
	what = GetRacerMateDeviceID(ix);

	devices[ix].ev->stop();
	DEL(devices[ix].ev);

//#ifdef EXTENDED_DLL
//#if LEVEL >= DLL_FULL_ACCESS
	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		if (instances > 0)  {
			instances--;
		}
	}
//#endif

	return ALL_OK;
}								// int stopTrainer(int ix)

/**********************************************************************

**********************************************************************/

int ResettoIdle(int ix)  {

	stopTrainer(ix);
	devices[ix].what = DEVICE_NOT_SCANNED;
	devices[ix].port = 0;
	devices[ix].when = 0L;
	memset(&devices[ix].nv, 0, sizeof(NV));
	devices[ix].last_control_byte = 0;
#ifdef WIN32
	devices[ix].hthread = 0;
#endif

	if (devices[ix].ds)  {
		DEL(devices[ix].ds);
	}

	return ALL_OK;
}						// int ResettoIdle(int ix)


//#endif					// #if LEVEL >= DLL_CANNONDALE_ACCESS


//#if LEVEL >= DLL_MINIMUM_ACCESS

/**********************************************************************

**********************************************************************/

int init_dll(void)  {
	FILE *stream = fopen("rm1.lck", "w");
	FCLOSE(stream);
	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

int stop_dll(void)  {
	//dirs.clear();
	while(dirs.size())  {
		dirs.pop_back();
	}
	rm("rm1.lck");
	return ALL_OK;
}



/**********************************************************************
	comport starts at 1
**********************************************************************/

const char *get_computrainer_firmware(int _comport)  {

	dataSource *ds = NULL;
	DWORD start;
	int status;
//	char str[32];
	int bp = 0;
	unsigned short version;
	static char verstr[32] = {0};
	//char verstr[32] = {0};
	//std::string s;
	bool b;
	RIDER_DATA rd;

	Course course;
	Bike bike;

	b = haveDevice(DEVICE_COMPUTRAINER, _comport);
	if (b)  {
#ifdef _DEBUG
		if (devices[_comport-1].nv.ver[0]==0)  {
			bp = 2;
		}
#endif
		return devices[_comport-1].nv.ver;
	}

	//sprintf(str, "com%d", _comport);


	char str[32];
	sprintf(str, "COM%d", _comport);
	ds = new Computrainer(str, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		//return s;
		return NULL;
	}

	start = timeGetTime();
	ds->decoder->set_version(0);							// unsigned short

	while(1)  {
      //status = ds->getNextRecord(0);
      status = ds->getNextRecord();
		if (status==0)  {
			version = ds->decoder->get_version();
			if (version != 0)  {
				break;
			}
		}
		
	   ds->updateHardware();

		if ( (timeGetTime() - start) >= 2000)  {
			break;
		}

   }

	sprintf(verstr, "%hd", version);
	DEL(ds);

#ifdef _DEBUG
	if (verstr[0]==0)  {
		bp = 1;
	}
#endif

	return verstr;

}			// const char *get_computrainer_firmware(int _comport)

/**********************************************************************
	comport starts at 1
	needs a 2 second delay after previouse serial port open to handlebar
	before it will respond correctly
**********************************************************************/

const char *get_computrainer_cal(int _comport)  {
	char comstr[32] = {0};
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//char str[32];
	static char calstr[32] = {0};
	bool b;
	RIDER_DATA rd;
	Course course;
	Bike bike;

	b = haveDevice(DEVICE_COMPUTRAINER, _comport);
	if (b)  {
		return devices[_comport-1].nv.cal;
	}

	//sprintf(str, "com%d", _comport);


	sprintf(comstr, "COM%d", _comport);
	ds = new Computrainer(comstr, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return NULL;
	}

	start = timeGetTime();
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	while(1)  {
      //status = ds->getNextRecord(0);
      status = ds->getNextRecord();
		if (status==0)  {
			if (ds->decoder->meta.rfdrag != 0)  {
				float ftemp;
				ftemp = (float) (ds->decoder->meta.rfdrag & 0x07ff);
				sprintf(calstr, "%.6f", ftemp / 256.0f);
				if ((ds->decoder->meta.rfmeas & 0x0008) == 0x0008)  {
					strcat(calstr, " C");
				}
				else  {
					strcat(calstr, " U");
				}
				break;
			};
		}

	   ds->updateHardware();

		if ( (timeGetTime() - start) >= 15000)  {
			break;
		}

   }

	//DWORD dms = timeGetTime() - start;				// 899, 936, 732, 1035, 940, 692, 836, 1037

	DEL(ds);

	return calstr;
}								// char *get_computrainer_cal(int _comport)  {


/**********************************************************************
	comport starts at 1
**********************************************************************/

bool computrainer_exists(int _comport)  {

	int istat;
	char comstr[16];
	Serial *port;
	//int bp = 0;

	int i = haveDevice(DEVICE_COMPUTRAINER, _comport);
	if (i != -1)  {
		return true;
	}

	sprintf(comstr, "COM%d", _comport);

	//-----------------------------------------------------------------------
	// 
	//-----------------------------------------------------------------------

#ifdef WIN32
	try  {
		port = new Serial(true, comstr, "2400", FALSE, NULL);
	}
	catch (const int istatus) {
		switch(istatus)  {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
				return false;
			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				return false;
			case SERIAL_ERROR_BUILDCOMMDCB:
			case SERIAL_ERROR_SETCOMMSTATE:
			case SERIAL_ERROR_CREATEEVENT1:
			case SERIAL_ERROR_CREATEEVENT2:
			case SERIAL_ERROR_OTHER:
				return false;
			default:
				return false;
		}
	}
#else
	port = new Serial(comstr, "2400", NULL);
#endif


	port->send("RacerMate", 0);

	istat = port->expect("LinkUp", 200);			// 80 ms measured
	port->setShutDownDelay(10);

	if (istat!=0)  {
		DEL(port);
		return false;
	}

	//myflush(port, 500, 1);
	DEL(port);
	return true;
}							// bool computrainer_exists(int _comport)  {

/**********************************************************************
	comport starts at 1
	gets both the cal and version in one function
**********************************************************************/

int get_computrainer_nv(DEVICE *dev)  {
	char comstr[32] = {0};
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//char str[32];
	//int bp = 0;
	unsigned short version;
	RIDER_DATA rd;

	Course course;
	Bike bike;

	int i = haveDevice(DEVICE_COMPUTRAINER, dev->port);
	if (i != -1)  {
		memcpy(&dev->nv, &devices[i].nv, sizeof(NV));
		return ALL_OK;
	}

	memset(&dev->nv, 0, sizeof(dev->nv));

	//sprintf(str, "com%d", dev->port);

	sprintf(comstr, "COM%d", dev->port);
	ds = new Computrainer(comstr, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return GENERIC_ERROR;
	}

	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	bool have_ver = false;
	bool have_cal = false;

	start = timeGetTime();

	while(1)  {
      //status = ds->getNextRecord(0);
      status = ds->getNextRecord();
		if (status==0)  {
			version = ds->decoder->get_version();
			if (version != 0)  {
				sprintf(dev->nv.ver, "%hd", version);
				have_ver = true;
			}

			if (ds->decoder->meta.rfdrag != 0)  {
				float ftemp;
				ftemp = (float) (ds->decoder->meta.rfdrag & 0x07ff);
				sprintf(dev->nv.cal, "%.2f", ftemp / 256.0f);
				if ((ds->decoder->meta.rfmeas & 0x0008) == 0x0008)  {
					strcat(dev->nv.cal, " C");
				}
				else  {
					strcat(dev->nv.cal, " U");
				}
				have_cal = true;
			};

			if (have_ver && have_cal)  {
				break;
			}
		}					// if (status==0)

	   ds->updateHardware();

		if ( (timeGetTime() - start) >= 2000)  {
			break;
		}

   }

	//DWORD dms = timeGetTime() - start;				// 1035, 1003, 902, 974, 1051

	DEL(ds);

	if (have_ver && have_cal)  {
		return ALL_OK;
	}

	return GENERIC_ERROR;
}							// int get_computrainer_nv(int _comport, NV *nv)  {

// -------------------------------------------  V E L O T R O N  ------------------------------------------------------

/**********************************************************************
	comport starts at 1
**********************************************************************/

const char *get_velotron_firmware(int _comport)  {
	static char verstr[32] = {0};
	unsigned char version;
	unsigned char build;
	bool b;
	char comstr[32] = {0};

	b = haveDevice(DEVICE_VELOTRON, _comport);
	if (b)  {
		return devices[_comport - 1].nv.ver;
	}


	sprintf(comstr, "com%d", _comport);

	NVRAM::U *u;
	NVRAM *nvram = new NVRAM(comstr);

	nvram->read();
	u = nvram->getBuf();
	version = u->d.version;
	build = u->d.build;

	DEL(nvram);

	sprintf(verstr, "%d.%d", version, build);
	return verstr;
}						// const char *get_velotron_firmware(int _comport)  {

/**********************************************************************
	comport starts at 1
**********************************************************************/

const char *get_velotron_cal(int _comport)  {

	DWORD factoryTime;
	static char calstr[32] = {0};
	bool b;

	b = haveDevice(DEVICE_VELOTRON, _comport);
	if (b)  {
		return devices[_comport-1].nv.cal;
	}

	char str[32];

	sprintf(str, "com%d", _comport);

	NVRAM::U *u;
	NVRAM *nvram = new NVRAM(str);

	nvram->read();
	u = nvram->getBuf();
	factoryTime = u->d.factoryCalibration;
	DEL(nvram);

	sprintf(calstr, "%ld", factoryTime);
	return calstr;
}								// char *get_velotron_cal(int _comport)  {

/**********************************************************************
	check for a velotron
	'port' is open at 38400 baud
**********************************************************************/

bool isVelotron(Serial *port, unsigned long _delay)  {

	char buf[64] = {0};
	int n;
	unsigned char b[3];
	unsigned short cctp = 0;
	int bp;
	int i;
	bool bb;

	bb = haveDevice(DEVICE_VELOTRON, port->getPortNumber());
	if (bb)  {
		return true;
	}

	b[0] = 0x80;
	b[1] = (cctp & 0x03ff)>>4;		// bits 4-9  (0000 10xx zzzz 0000)
	b[2] = cctp & 0x000f;			// bits 0-3  (0000 0000 0000 1111)

	port->txx(b, 3);

	Sleep(_delay);

	n = port->rx(buf, 63);		// returns the number of bytes received

	if (n != 0)  {
		// we got data, now check to see if it looks like velotron data

		#ifdef _DEBUG
			/*
			FILE *stream = fopen("commtest.txt", "wt");
			for(i=0; i<n; i++)  {
				if (buf[i] & 0x80)  {
					fprintf(stream, "\n");
				}
				fprintf(stream, "%02x ", (buf[i] & 0x0ff));
			}
			fclose(stream);
			*/
			bp = 1;
		#endif		

		/*
			73 
			cb 7f 1d 21 3c 1b 3a 4d 5a 73 
			cb 7f 1d 21 3c 1b 3a 4d 5a 73 
			cb 7f 1d 21 3c 1b 3a 4d 5a 73 
			cb 7f 1d 21 3c 1b 3a 4d 5a 73 
			cb 7f 1d 21 3c 1b 3a 4d 5a 73 
			cb 7f 1d 21 3d 1b 3a 4d 5a 70 
			cb 7f 
		*/

		// every 10th byte should have the high bit set

		int count = 0;

		for(i=0;i<n;i++)  {
			if ((buf[i] & 0x80)==0x80)  {
				if (count==10)  {
					bp = 1;
					return true;
				}
				else  {
					count = 0;
				}
				count++;
			}
			else  {
				count++;
			}
		}
		bp = 1;
		return false;
	}					// if (bytes received)
#ifdef _DEBUG
	else  {
		bp = 0;
	}
#endif


	return false;
}									// isVelotron()

/**********************************************************************
	comport starts at 1
**********************************************************************/

bool velotron_exists(int _comport)  {
	//int istat;
	char comstr[16];
	Serial *port;
	//int bp = 0;

	int i = haveDevice(DEVICE_VELOTRON, _comport);
	if (i != -1)  {
		return true;
	}

	sprintf(comstr, "COM%d", _comport);

	//-----------------------------------------------------------------------
	// 
	//-----------------------------------------------------------------------

#ifdef WIN32
	try  {
		port = new Serial(true, comstr, "38400", FALSE, NULL);
	}
	catch (const int istatus) {
		switch(istatus)  {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
				return false;
			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				return false;
			case SERIAL_ERROR_BUILDCOMMDCB:
			case SERIAL_ERROR_SETCOMMSTATE:
			case SERIAL_ERROR_CREATEEVENT1:
			case SERIAL_ERROR_CREATEEVENT2:
			case SERIAL_ERROR_OTHER:
				return false;
			default:
				return false;
		}
	}

#else
	port = new Serial(comstr, "38400", NULL);
#endif

	port->setShutDownDelay(10);

	if (isVelotron(port, 500))  {
		DEL(port);
		return true;
	}

	DEL(port);
	return false;
}							// bool velotron_exists(int _comport)  {

/**********************************************************************
	ONLY called by check_for_trainers()
**********************************************************************/

int read_computrainer(Serial *port)  {
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//int bp = 0;
	unsigned short version;
	float ftemp;
	bool have_ver;
	bool have_cal;
	DEVICE d = {DEVICE_NOT_SCANNED};
	RIDER_DATA rd;

	Course course;
	Bike bike;


	//bike.id = id;
	ds = new Computrainer(port, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return GENERIC_ERROR;
	}

	d.what = DEVICE_COMPUTRAINER;
	d.port = port->getPortNumber();

	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	have_ver = false;
	have_cal = false;

	start = timeGetTime();

	while(1)  {
      //status = ds->getNextRecord(0);
      status = ds->getNextRecord();
		if (status==0)  {
			version = ds->decoder->get_version();
			if (version != 0)  {
				sprintf(d.nv.ver, "%hd", version);
				have_ver = true;
			}

			if (ds->decoder->meta.rfdrag != 0)  {
				ftemp = (float) (ds->decoder->meta.rfdrag & 0x07ff);
				sprintf(d.nv.cal, "%.2f", ftemp / 256.0f);
				if ((ds->decoder->meta.rfmeas & 0x0008) == 0x0008)  {
					strcat(d.nv.cal, " C");
				}
				else  {
					strcat(d.nv.cal, " U");
				}
				have_cal = true;
			};

			if (have_ver && have_cal)  {
				break;
			}
		}					// if (status==0)

	   ds->updateHardware();

		if ( (timeGetTime() - start) >= 2000)  {
			break;
		}

   }

	DEL(ds);

	d.when = timeGetTime();
	devices[port->getPortNumber()-1] = d;

#ifdef ERGVIDLOG
	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "read_computrainer x\n");
	fclose(logstream);
#endif

	return ALL_OK;
}							// void read_computrainer(Serial *port)  {

/**********************************************************************
	ONLY called by check_for_trainers()
**********************************************************************/

int read_velotron(Serial *port)  {
	DEVICE d = {DEVICE_NOT_SCANNED};
	NVRAM::U *u;
	DWORD factoryTime;
	unsigned char version;
	unsigned char build;
	//char str[32];

	NVRAM *nvram = new NVRAM(port);
	nvram->read();
	u = nvram->getBuf();
	factoryTime = u->d.factoryCalibration;
	version = u->d.version;
	build = u->d.build;
	DEL(nvram);

	sprintf(d.nv.ver, "%d.%d", version, build);
	sprintf(d.nv.cal, "%ld", factoryTime);
	d.what = DEVICE_VELOTRON;
	d.port = port->getPortNumber();
	d.when = timeGetTime();

	devices[port->getPortNumber()-1] = d;

	return ALL_OK;
}					// int read_velotron(Serial *port)  {


/**********************************************************************
hd1
hd4
hd6, ul = 93,366

**********************************************************************/

bool haveDevice(EnumDeviceType what, int comport)  {
	int ix;
	DWORD now, dt;

	ix = comport - 1;

	if (glog) glog->write(10,0,1,"hd1\n");

	if (devices[ix].ev)  {
		if (devices[ix].what==what && devices[ix].port==comport)  {					// if thread is running, we have it
			if (glog) glog->write(10,0,1,"hd1b: %s = %s, %d = %d, have it\n", devstrs[what], devstrs[devices[ix].what], devices[ix].port, comport);
			return true;
		}
		else  {
			if (glog) glog->write(10,0,1,"hd2\n");
			DEL(devices[ix].ev);
			if (glog) glog->write(10,0,1,"hd2b\n");
			return false;
		}
	}



	if (devices[ix].what!=what)  {
		if (glog) glog->write(10,0,1,"hd3: %s   %s\n", devstrs[what], devstrs[devices[ix].what]);
		return false;
	}

	if (glog) glog->write(10,0,1,"hd4\n");

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON || what==DEVICE_SIMULATOR || what==DEVICE_RMP)  {

		if (devices[ix].port==comport)  {

			now = timeGetTime();
			dt = now - devices[ix].when;
			if (glog) glog->write(10,0,1,"hd4b\n");

			if ( dt < DEVICE_TIMEOUT_MS )   {							// 10,000 ms
				if (glog) glog->write(10,0,1,"hd5, dt = %ld\n", dt);
				return true;
			}
		}
	}
	else if (what==DEVICE_NOT_SCANNED)  {
		if (glog) glog->write(10,0,1,"hd5B\n");
		return false;
	}
	else  {
		unsigned long ul;
		ul = DEVICE_TIMEOUT_MS;
		ul = timeGetTime();
		dt = timeGetTime() - devices[ix].when;

		if ( dt < DEVICE_TIMEOUT_MS)  {
			if (glog) glog->write(10,0,1,"hd6, ul = %ld\n", ul);
			return true;
		}
	}

	if (glog) glog->write(10,0,1,"hdx\n");
	return false;

}				// bool haveDevice(EnumDeviceType what, int comport)

/**********************************************************************

**********************************************************************/

void check_dirs(const char *_logfilepath)  {

	if (glog)  {
		glog->write(10,0,1,"cd1\n");
	}

	if (dirs.size()!=0)  {
		if (glog)  {
			glog->write(10,0,1,"cd2\n");
		}
		return;
	}

	//int status;

	if (glog)  {
		glog->write(10,0,1,"cd3\n");
	}

	setup_standard_dirs(_logfilepath);

//	status = set_dirs(dirs);
//	assert(status==ALL_OK);

	if (glog)  {
		glog->write(10,0,1,"cdx\n");
	}


	return;
}

/*************************************************************************************************

*************************************************************************************************/

void setup_standard_dirs(const char *_logfilepath)  {
	char str[256];
	int i;
	const char *cptr;

	if (glog)  {
		glog->write(10,0,1,"ssd1\n");
	}

	if (dirs.size() != 0)  {
		//if (glog) glog->write(10,0,1,"program directory already set up: %s\n", dirs[DIR_PROGRAM].c_str());
		if (glog)  {
			glog->write(10,0,1,"ssd2\n");
		}
		return;
	}

	/*
	enum DIRTYPE {
		DIR_PROGRAM,
		DIR_PERSONAL,
		DIR_SETTINGS,
		DIR_REPORT_TEMPLATES,
		DIR_REPORTS,
		DIR_COURSES,
		DIR_PERFORMANCES,
		DIR_DEBUG
	};
	*/

	if (glog)  {
		glog->write(10,0,1,"ssd3\n");
	}

#ifdef WIN32
	SHGetFolderPath(
			NULL,							// HWND hwndOwner,
			CSIDL_PROGRAM_FILES,			// int nFolder,
			NULL,							// HANDLE hToken,
			0,								// DWORD dwFlags,
			gstring
			);																				// "C:\Program Files"
	dirs.push_back(gstring);
	cptr = dirs[DIR_PROGRAM].c_str();

	if (glog)  {
		glog->write(10,0,1,"ssd4\n");
	}

	SHGetFolderPath(
			NULL,							// HWND hwndOwner,
			CSIDL_PERSONAL,					// int nFolder,
			NULL,							// HANDLE hToken,
			0,								// DWORD dwFlags,
			gstring
			);																				// "C:\Users\larry\Documents"
	dirs.push_back(gstring);
	cptr = dirs[DIR_PERSONAL].c_str();
	if (glog)  {
		glog->write(10,0,1,"ssd5\n");
	}

	/*
	if (glog)  {
		glog->write(10, 0, 1, "\n");
		glog->write("program directory = %s\n", dirs[DIR_PROGRAM].c_str());
		glog->write("personal directory = %s\n", dirs[DIR_PERSONAL].c_str());
		glog->write(10, 0, 1, "\n");
	}
	*/

	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	*/

	DIRTYPE dt;

	dt = (DIRTYPE)DIR_SETTINGS;							// 2
	assert(DIR_SETTINGS==2);
	dirs.push_back(".");
	if (glog)  {
		glog->write(10,0,1,"ssd6\n");
	}

	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Settings");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_SETTINGS].c_str();
	*/

	dt = (DIRTYPE)DIR_REPORT_TEMPLATES;							// 3
	assert(DIR_REPORT_TEMPLATES==3);
	dirs.push_back(".");
	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Report Templates");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_REPORT_TEMPLATES].c_str();
	*/

	dt = (DIRTYPE)DIR_REPORTS;									// 4
	assert(DIR_REPORTS==4);
	dirs.push_back(".");
	if (glog)  {
		glog->write(10,0,1,"ssd7\n");
	}
	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Reports");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_REPORTS].c_str();
	*/


	dt = (DIRTYPE)DIR_COURSES;								// 5
	assert(DIR_COURSES==5);
	dirs.push_back(".");
	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Courses");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_COURSES].c_str();
	*/


	dt = (DIRTYPE)DIR_PERFORMANCES;							// 6
	assert(DIR_PERFORMANCES==6);
	dirs.push_back(".");
	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Performances");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_PERFORMANCES].c_str();
	*/

	if (glog)  {
		glog->write(10,0,1,"ssd8\n");
	}

	dt = (DIRTYPE)DIR_DEBUG;								// 7
	assert(DIR_DEBUG==7);
	if (_logfilepath)  {
		dirs.push_back(_logfilepath);
		if (!direxists(gstring))  {
			CreateDirectory(_logfilepath, NULL);
		}
		cptr = dirs[DIR_DEBUG].c_str();
	}
	else  {
		dirs.push_back(".");
	}

	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Debug");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_DEBUG].c_str();
	*/

	if (glog)  {
		glog->write(10,0,1,"ssd9\n");
	}

	dt = (DIRTYPE)DIR_HELP;								// 8
	assert(DIR_HELP==8);
	dirs.push_back(".");
	/*
	sprintf(gstring, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstring, "\\Help");
	dirs.push_back(gstring);
	if (!direxists(gstring))  {
		CreateDirectory(gstring, NULL);
	}
	cptr = dirs[DIR_HELP].c_str();
	*/

#else
	//strcpy(personal_folder, "zzz");
	//strcpy(program_files_folder, "zzz");

	//bp = -1;

	for(i=0; i<8; i++)  {
		cptr = dirs[i].c_str();
		if (!direxists(cptr))  {
			CreateDirectory(cptr, NULL);
		}
	}
	//strcpy(logpath, dirs[DIR_DEBUG]);
	sprintf(str, "%s%smain.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
//	glog = new gloger(str);
#endif

	/*
	if (glog)  {
		for(i=0; i<9; i++)  {
			glog->write("%d   %s\n", i, dirs[i].c_str());
		}
		glog->write(10,0,1,"\n");
	}
	*/

	if (glog)  {
		glog->write(10,0,1,"ssdx\n");
	}

	return;
}				// void setup_standard_dirs()




/**********************************************************************
	port starts at 1
**********************************************************************/

bool iscal(EnumDeviceType what, int port)  {
	const char *cptr;
	int i, n;

	if (what==DEVICE_COMPUTRAINER)  {
		cptr = get_computrainer_cal(port);
		if (strstr(cptr, "U")==NULL)  {
			return true;
		}
	}
	else if (what==DEVICE_VELOTRON)  {
		cptr = get_velotron_cal(port);
		n = sscanf(cptr, "%d", &i);
		if (n!=1)  {
			return false;
		}
		if (i<100000)  {
			return false;
		}
		if (i>400000)  {
			return false;
		}
		return true;
	}

	return false;

}					// bool iscal(EnumDeviceType what, int port)  {











//////////////////////////////////////////////////////////////////////////////////////////









/**********************************************************************

**********************************************************************/

#ifdef WIN32
__declspec(dllexport) void dump_devices(void)  {
#else
void dump_devices(void)  {
#endif

	int i;
	DEVICE d;
	int bp = -1;

	printf("\n\n");

	//for (i=0; i<NPORTS; i++)  {
	for (i=0; i<10; i++)  {
		d = devices[i];
		bp = i;
		printf("%3d %-24s %ld %-20s %s\n",
			//d.port,
			i+1,
			devstrs[d.what],
			d.when,
			d.nv.cal,
			d.nv.ver
		);
	}
	return;
}








/**********************************************************************

**********************************************************************/

EnumDeviceType get_what(int _ix)  {

	return devices[_ix].what;
}


/**********************************************************************
	timeout is in milliseconds
**********************************************************************/

void set_timeout(unsigned long _ms)  {

	DEVICE_TIMEOUT_MS = _ms;
	return;
}

/**********************************************************************

**********************************************************************/

const char *get_computrainer_cal_from_thread(int ix, int fw)  {
	const char *cptr;


	if (!devices[ix].ev)  {
		return get_errstr(DEVICE_NOT_RUNNING);
	}

	cptr = devices[ix].ev->get_computrainer_cal();
	strcpy(devices[ix].nv.cal, cptr);

	return cptr;
}

/**********************************************************************

**********************************************************************/

const char *get_velotron_cal_from_thread(int ix, int fw)  {
	const char *cptr;


	if (!devices[ix].ev)  {
		return get_errstr(DEVICE_NOT_RUNNING);
	}

	cptr = devices[ix].nv.cal;

	return cptr;
}




/**********************************************************************

**********************************************************************/

int get_lower_hr(int ix)  {

	int k;
	EnumDeviceType what, what2;

	what = devices[ix].what;

	if (what == DEVICE_COMPUTRAINER)  {
	}
	else if (what == DEVICE_VELOTRON )  {
	}
	else  {
		return WRONG_DEVICE;
	}


	if (!devices[ix].ev)  {
		return ROUND(devices[ix].rd.lower_hr);
	}

	what2 = devices[ix].ev->getwhat();
	if (what != what2)  {
		return GENERIC_ERROR;
	}

	k = devices[ix].ev->get_lower_hr();

	return k;
}							// int get_lower_hr(int ix)

/**********************************************************************

**********************************************************************/

int get_upper_hr(int ix)  {

	int k;
	EnumDeviceType what, what2;

	what = devices[ix].what;

	if (what == DEVICE_COMPUTRAINER)  {
	}
	else if (what == DEVICE_VELOTRON )  {
	}
	else  {
		return WRONG_DEVICE;
	}


	if (!devices[ix].ev)  {
		return ROUND(devices[ix].rd.upper_hr);
	}

	what2 = devices[ix].ev->getwhat();
	if (what != what2)  {
		return GENERIC_ERROR;
	}

	k = devices[ix].ev->get_upper_hr();
	return k;

}

/**********************************************************************

**********************************************************************/

int get_hr_beep_enabled(int ix)  {

	int k;
	EnumDeviceType what, what2;

	what = devices[ix].what;

	if (what == DEVICE_COMPUTRAINER)  {
	}
	else if (what == DEVICE_VELOTRON )  {
	}
	else  {
		return WRONG_DEVICE;
	}


	if (!devices[ix].ev)  {
		if (devices[ix].beep_enabled)  {
			return 1;
		}
		else  {
			return 0;
		}
	}

	what2 = devices[ix].ev->getwhat();
	if (what != what2)  {
		return GENERIC_ERROR;
	}

	k = devices[ix].ev->get_hr_beep_enabled();
	return k;

}



#ifdef ERGVIDLOG

/**********************************************************************

**********************************************************************/

void dump_bike_params(int ix)  {
	int i;
	FILE *logstream;



	logstream = fopen(ERGVIDFILE, "a+t");
	fprintf(logstream, "   dump_bike_params():\n");

	fprintf(logstream, "      real_front_teeth = %d\n", devices[ix].bike_params.real_front_teeth);
	fprintf(logstream, "      real_rear_teeth = %d\n", devices[ix].bike_params.real_rear_teeth);
	fprintf(logstream, "      front_ix = %d\n", devices[ix].bike_params.front_ix);
	fprintf(logstream, "      rear_ix = %d\n", devices[ix].bike_params.rear_ix);
	fprintf(logstream, "      kgs = %f\n", devices[ix].bike_params.kgs);
	fprintf(logstream, "      nfront = %d\n", devices[ix].bike_params.nfront);
	fprintf(logstream, "      nrear = %d\n", devices[ix].bike_params.nrear);
	fprintf(logstream, "      tire_diameter_mm = %f\n", devices[ix].bike_params.tire_diameter_mm);

	fprintf(logstream, "      front gears:\n");
	for(i=0; i<devices[ix].bike_params.nfront; i++)  {
		fprintf(logstream, "         %d   %d\n", i, devices[ix].bike_params.front_gears[i]);
	}
	fprintf(logstream, "      rear gears:\n");
	for(i=0; i<devices[ix].bike_params.nrear; i++)  {
		fprintf(logstream, "         %d   %d\n", i, devices[ix].bike_params.rear_gears[i]);
	}

	fclose(logstream);
	return;
}


#endif				// #ifdef ERGVIDLOG


/**********************************************************************

**********************************************************************/

bool get_shiftflag(int ix)  {

	if (!devices[ix].ev)  {
		return false;
	}

	return devices[ix].ev->get_shiftflag();

}				// bool get_shiftflag()

/**********************************************************************

**********************************************************************/

int set_calibrating(int ix, int fw)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_calibrating(true);

	return ALL_OK;
}




/**********************************************************************

**********************************************************************/

int get_cal_time(int ix, int fw)  {

	EnumDeviceType what;

	#ifdef ERGVIDLOG
		//logstream = fopen(ERGVIDFILE, "a+t");
		//fprintf(logstream, "set_get_cal_time(%d)\n", ix);
		//fclose(logstream);
	#endif


	what = devices[ix].what;
	if ( what != DEVICE_VELOTRON )  {
		return WRONG_DEVICE;
	}

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	int cal = devices[ix].ev->get_cal_time();

	return cal;
}










/**********************************************************************

**********************************************************************/

int set_velotron_calibration(int ix, int fw, int _cal)  {

	DEVICE *d;
	int n, size;
	unsigned char version;
	unsigned char build;
	NVRAM::U u = {
                        { 0 }
                    };
        
	char comstr[32];
	NVRAM *nvram = NULL;
	NV nv = {{0},{0}};
	int port;
	EnumDeviceType what=DEVICE_NOT_SCANNED;



	d = &devices[ix];
	if (d->ev)  {
		return DEVICE_RUNNING;
		return DEVICE_NOT_RUNNING;
	}

	port = ix + 1;
	what = devices[ix].what;

	if ( !haveDevice(what, port) )  {
		what = GetRacerMateDeviceID(ix);
		if ( !haveDevice(what, port) )  {
			return GENERIC_ERROR;
		}
	}

	if (what!=DEVICE_VELOTRON)  {
		return WRONG_DEVICE;
	}

	nv = devices[19].nv;
	nv = devices[ix].nv;


	n = sscanf(nv.ver, "%c.%c", &version, &build);
	if (n != 2)  {
		return GENERIC_ERROR;
	}

	sprintf(comstr, "com%d", port);
	
	nvram = new NVRAM(comstr);
		size = sizeof(NVRAM::DATA);
		memset(&u, 0, sizeof(NVRAM::DATA));
		nvram->setFactoryCalibration(_cal);
		nvram->setVersion(version);
		nvram->setBuild(build);
		nvram->write();
	DEL(nvram);


	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

int startCal(int ix, int fw)  {

	int status;

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	if (devices[ix].what != DEVICE_VELOTRON)  {
		return WRONG_DEVICE;
	}

	status = devices[ix].ev->startCal();
	if (FAILED(status))  {
		return GENERIC_ERROR;
	}

	return ALL_OK;
}



/**********************************************************************

**********************************************************************/

int get_calibration(int ix)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	if (devices[ix].what != DEVICE_VELOTRON)  {
		return WRONG_DEVICE;
	}
	int cal;
	cal = devices[ix].ev->get_calibration();

	return cal;

}								// int get_calibration()


/**********************************************************************

**********************************************************************/

int endCal(int ix, int fw)  {
	int status;

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}

	if (devices[ix].what != DEVICE_VELOTRON)  {
		return WRONG_DEVICE;
	}

	status = devices[ix].ev->endCal();
	if (FAILED(status))  {
		return GENERIC_ERROR;
	}

	return ALL_OK;
}

//#endif								// #if LEVEL >= DLL_MINIMUM_ACCESS


#endif								// #ifdef OBFUSCATE


