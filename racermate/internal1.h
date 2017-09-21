#ifndef _INTERNAL_H_	
#define _INTERNAL_H_	

#define WIN32_LEAN_AND_MEAN

#include <serial.h>

#include "levels.h"
#include "dll_common.h"

//---------------------------------------------------
// used internally, so no dll or JNI interface:
//---------------------------------------------------

EnumDeviceType check_for_trainers(int _ix, int _dummy);
int start_server(void);
const char *get_errstr2(int err);							// duplicated in the exported dll functions
EnumDeviceType GetRacerMateDeviceID2(int ix);				// duplicated in the exported dll functions
EnumDeviceType check_for_trainers2(int _ix, int _dummy);			// duplicated in the exported dll functions

int endCal(int ix, int fw);
int startCal(int ix, int fw);
int get_calibration(int ix);

void check_dirs(const char *);
void setup_standard_dirs(const char *_x=NULL);
bool iscal(EnumDeviceType, int);
bool isVelotron(Serial *, unsigned long );
const char *get_computrainer_firmware(int);					// starts at 1
const char *get_velotron_firmware(int);						// starts at 1
const char *get_computrainer_cal(int);						// starts at 1
const char *get_velotron_cal(int);								// starts at 1
//bool computrainer_exists(int);
bool velotron_exists(int);
int get_computrainer_nv(DEVICE *);
int read_computrainer(Serial *);

#ifdef DO_RTD_SERVER
	int read_computrainer(RTDServer *);								// where each CT acts as a server
	void flush(Logger *logg, RTDServer *,DWORD timeout, BOOL echo);
#else
#ifdef WIN32
	int read_computrainer(int _ix, Server *);									// single dll server for up to 16 clients
	void flush(int _ix, Logger *logg, Server *,DWORD timeout, BOOL echo);
#endif
#endif

#ifdef WIN32
int read_computrainer(Client *);
#endif

#ifdef WIN32
void flush(Logger *logg, Client *,DWORD timeout, BOOL echo);
#endif

const char *get_computrainer_cal_from_thread(int, int);
const char *get_velotron_cal_from_thread(int, int);
int read_velotron(Serial *);
bool haveDevice(EnumDeviceType, int);					// xxx

#ifdef LOGDLL
	void dump_bike_params(int);
	void dump_dirs(void);
#endif

#ifdef DO_EXPORT
	bool export_flag_file(void);
	//const char * export_flag_file(void);
#endif

#endif						// sentry


