
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
#include <assert.h>
#include <shlobj.h>
#endif

#ifdef _DEBUG
	#include <assert.h>
#endif


#include <comglobs.h>
#include <globals.h>
#include <utils.h>
#include <fatalerror.h>
#include <nvram.h>
#include <computrainer.h>
#include <device.h>

#include <dlldefs.h>
//#include "dll_common.h"
#include "dll_globals.h"
#include "errors.h"

#include "internal.h"

#define FLUSH_STR_LEN 256



/**********************************************************************
  this is .../_fs/dll/internal.cpp
 **********************************************************************/

int init_dll(void) {
	FILE *stream = fopen("rm1.lck", "w");
	FCLOSE(stream);
	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

int stop_dll(void) {
	//dirs.clear();
	while(dirs.size()) {
		dirs.pop_back();
	}
	rm("rm1.lck");
	return ALL_OK;
}



/**********************************************************************
  comport starts at 1
 **********************************************************************/

const char *get_computrainer_firmware(const char *_ix) {								// 0 - 255
	dataSource *ds = NULL;
	DWORD start;
	int status;
	unsigned short version;
	static char verstr[32] = { 0 };
	bool b;
	RIDER_DATA rd;

	Course course;
	Bike bike;


	b = haveDevice(DEVICE_COMPUTRAINER, _ix);
	if(b) {
		/*
		if (devices[_ix].commtype==TRAINER_IS_CLIENT)  {
			if (!strcmp(devices[_ix].nv.ver, "0"))  {
				// wait for decode_slow to execute
				unsigned short fw;
				fw = server->get_fw(devices[_ix].udpkey);
				if (fw != 0)  {
					int cal = server->get_cal(devices[_ix].udpkey);
					sprintf(devices[_ix].nv.ver, "%d", fw);
					sprintf(devices[_ix].nv.cal, "%d", cal);
				}
			}
		}
		*/

		return devices[_ix].nv.ver;
	}

	//char str[32];
	//strncpy(str, devices[_ix].id, sizeof(str)-1);

	ds = new Computrainer(devices[_ix].id, rd, &course, &bike, NO_LOGGING, "", -1);

	if(!ds->is_initialized()) {
		DEL(ds);
		return NULL;
	}

	start = timeGetTime();
	ds->decoder->set_version(0);							// unsigned short

	while(1) {
		status = ds->getNextRecord();
		if(status == 0) {
			version = ds->decoder->get_version();
			if(version != 0) {
				break;
			}
		}

		ds->updateHardware();

		if((timeGetTime() - start) >= 2000) {
			break;
		}

	}

	sprintf(verstr, "%hd", version);
	DEL(ds);

	return verstr;

}			// const char *get_computrainer_firmware(const char *_comport)

/**********************************************************************
  comport starts at 1
  needs a 2 second delay after previouse serial port open to handlebar
  before it will respond correctly
 **********************************************************************/
 // redoc
const char *get_computrainer_cal(const char *_ix) {

	char comstr[32] = { 0 };
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//char str[32];
	static char calstr[32] = { 0 };
	bool b;
	RIDER_DATA rd;
	Course course;
	Bike bike;

	b = haveDevice(DEVICE_COMPUTRAINER, _ix);
	if(b) {
		return devices[_ix].nv.cal;
	}

	int ix = atoi(_ix) + 1;
	sprintf(comstr, "COM%d", ix);

	ds = new Computrainer(comstr, rd, &course, &bike, NO_LOGGING, "", -1);

	if(!ds->is_initialized()) {
		DEL(ds);
		return NULL;
	}

	start = timeGetTime();
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	while(1) {
		status = ds->getNextRecord();
		if(status == 0) {
			if(ds->decoder->meta.rfdrag != 0) {
				float ftemp;
				ftemp = (float)(ds->decoder->meta.rfdrag & 0x07ff);
				sprintf(calstr, "%.6f", ftemp / 256.0f);
				if((ds->decoder->meta.rfmeas & 0x0008) == 0x0008) {
					strcat(calstr, " C");
				}
				else {
					strcat(calstr, " U");
				}
				break;
			};
		}

		ds->updateHardware();

		if((timeGetTime() - start) >= 15000) {
			break;
		}

	}

	DEL(ds);

	return calstr;
}								// char *get_computrainer_cal(const char *_comport)  {


/**********************************************************************
  comport starts at 1
  gets both the cal and version in one function
 **********************************************************************/

int get_computrainer_nv(DEVICE *dev) {
	char comstr[32] = { 0 };
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//char str[32];
	//int bp = 0;
	unsigned short version;
	RIDER_DATA rd;

	Course course;
	Bike bike;

	/*
	int i;
	i = haveDevice(DEVICE_COMPUTRAINER, dev->ix);
	if (i != -1)  {
		memcpy(&dev->nv, &devices[ix].nv, sizeof(NV));
		return ALL_OK;
	}
	*/
	if(haveDevice(DEVICE_COMPUTRAINER, dev->id)) {
		memcpy(&dev->nv, &devices[dev->id].nv, sizeof(NV));
		return ALL_OK;
	}

	memset(&dev->nv, 0, sizeof(dev->nv));

	strncpy(comstr, dev->id, sizeof(comstr) - 1);
	ds = new Computrainer(comstr, rd, &course, &bike, NO_LOGGING, "", -1);

	if(!ds->is_initialized()) {
		DEL(ds);
		return GENERIC_ERROR;
	}

	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	bool have_ver = false;
	bool have_cal = false;

	start = timeGetTime();

	while(1) {
		status = ds->getNextRecord();
		if(status == 0) {
			version = ds->decoder->get_version();
			if(version != 0) {
				sprintf(dev->nv.ver, "%hd", version);
				have_ver = true;
			}

			if(ds->decoder->meta.rfdrag != 0) {
				float ftemp;
				ftemp = (float)(ds->decoder->meta.rfdrag & 0x07ff);
				sprintf(dev->nv.cal, "%.2f", ftemp / 256.0f);
				if((ds->decoder->meta.rfmeas & 0x0008) == 0x0008) {
					strcat(dev->nv.cal, " C");
				}
				else {
					strcat(dev->nv.cal, " U");
				}
				have_cal = true;
			};

			if(have_ver && have_cal) {
				break;
			}
		}					// if (status==0)

		ds->updateHardware();

		if((timeGetTime() - start) >= 2000) {
			break;
		}

	}

	DEL(ds);

	if(have_ver && have_cal) {
		return ALL_OK;
	}

	return GENERIC_ERROR;
}							// int get_computrainer_nv(const char *_comport, NV *nv)  {


// -------------------------------------------  V E L O T R O N  ------------------------------------------------------

/**********************************************************************
  comport starts at 1
 **********************************************************************/

const char *get_velotron_firmware(const char *_ix) {
	static char verstr[32] = { 0 };
	unsigned char version;
	unsigned char build;
	bool b;
	char comstr[32] = { 0 };

	b = haveDevice(DEVICE_VELOTRON, _ix);
	if(b) {
		return devices[_ix].nv.ver;
	}


	int ix = atoi(_ix) + 1;
	sprintf(comstr, "com%d", ix);

	NVRAM::U *u;
	NVRAM *nvram = new NVRAM(comstr);

	nvram->read();
	u = nvram->getBuf();
	version = u->d.version;
	build = u->d.build;

	DEL(nvram);

	sprintf(verstr, "%d.%d", version, build);
	return verstr;
}						// const char *get_velotron_firmware(const char *_comport)  {

/**********************************************************************
  comport starts at 1
 **********************************************************************/

const char *get_velotron_cal(const char *_comport) {

	DWORD factoryTime;
	static char calstr[32] = { 0 };
	bool b;

	b = haveDevice(DEVICE_VELOTRON, _comport);
	if(b) {
		return devices[_comport].nv.cal;
	}

	char str[32];

	int ix = atoi(_comport) + 1;
	sprintf(str, "com%d", ix);

	NVRAM::U *u;
	NVRAM *nvram = new NVRAM(str);

	nvram->read();
	u = nvram->getBuf();
	factoryTime = u->d.factoryCalibration;
	DEL(nvram);

	sprintf(calstr, "%ld", factoryTime);
	return calstr;
}								// char *get_velotron_cal(const char *_comport)  {

/**********************************************************************
  check for a velotron
  'port' is open at 38400 baud
 **********************************************************************/

bool isVelotron(Serial *port, unsigned long _delay) {

	char buf[64] = { 0 };
	int n;
	unsigned char b[3];
	unsigned short cctp = 0;
	int i;
	bool bb;

	/*
		bb = haveDevice(DEVICE_VELOTRON, port->getPortNumber()-1);
		if (bb)  {
			return true;
		}
	*/

	b[0] = 0x80;
	b[1] = (cctp & 0x03ff) >> 4;		// bits 4-9  (0000 10xx zzzz 0000)
	b[2] = cctp & 0x000f;			// bits 0-3  (0000 0000 0000 1111)

	port->txx(b, 3);

	Sleep(_delay);

	n = port->rx(buf, 63);		// returns the number of bytes received

	if(n != 0) {
		// we got data, now check to see if it looks like velotron data

		// every 10th byte should have the high bit set

		int count = 0;

		for(i = 0; i < n; i++) {
			if((buf[i] & 0x80) == 0x80) {
				if(count == 10) {
					//bp = 1;
					return true;
				}
				else {
					count = 0;
				}
				count++;
			}
			else {
				count++;
			}
		}
		//bp = 1;
		return false;
	}					// if (bytes received)
#ifdef _DEBUG
	else {
		//bp = 0;
	}
#endif

	return false;
}									// isVelotron()

/**********************************************************************
  comport starts at 1
 **********************************************************************/

bool velotron_exists(const char *_comport) {
	//int istat;
	char comstr[16];
	Serial *port;
	//int bp = 0;

	int i = haveDevice(DEVICE_VELOTRON, _comport);
	if(i != -1) {
		return true;
	}

	int ix = atoi(_comport) + 1;
	sprintf(comstr, "COM%d", ix);

	//-----------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------

#ifdef WIN32
	try {
		port = new Serial(true, comstr, "38400", FALSE, NULL);
	}
	catch(const int istatus) {
		switch(istatus) {
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

	if(isVelotron(port, 500)) {
		DEL(port);
		return true;
	}

	DEL(port);
	return false;
}							// bool velotron_exists(const char *_comport)  {

/**********************************************************************
  ONLY called by check_for_trainers()
 **********************************************************************/

int read_computrainer(const char *_ix, Serial *port) {
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//int bp = 0;
	unsigned short version;
	float ftemp;
	bool have_ver;
	bool have_cal;
	//DEVICE d = {DEVICE_NOT_SCANNED};
	//DEVICE d;
	RIDER_DATA rd;
	Course course;
	Bike bike;
	DWORD dt;

#ifdef LOGDLL
	/*
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "internal read_computrainer()\n");
		fclose(logstream);
	 */
#endif

	 //bike.id = id;
	ds = new Computrainer(port, rd, &course, &bike);

	if(!ds->is_initialized()) {
		DEL(ds);
		return GENERIC_ERROR;
	}

	//d.what = DEVICE_COMPUTRAINER;
	//d.port = port->getPortNumber();
	//d.port = CLIENT_SERIAL_PORT_BASE;


	const char *cptr = port->get_port_name();

	//strncpy(d.id, cptr, sizeof(d.id)-1);

#ifndef WIN32
	int ix = portname_to_ix[cptr];
#endif

	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	have_ver = false;
	have_cal = false;

	start = timeGetTime();

	// read computrainer for 2 seconds to get all packets, such as version, rfdrag, hr sensor flags
	// known here
	//		rs232, windows
	//			devices[_ix].ix
	//			devices[_ix].id
	//			devices[_ix].commtype
	//			devices[_ix].what

	while(1) {
		//status = ds->getNextRecord(0);
		status = ds->getNextRecord();
		if(status == 0) {
			version = ds->decoder->get_version();
			if(version != 0) {
				sprintf(devices[_ix].nv.ver, "%hd", version);
				have_ver = true;
			}

			if(ds->decoder->meta.rfdrag != 0) {
				ftemp = (float)(ds->decoder->meta.rfdrag & 0x07ff);					// rfdrag = 512
				sprintf(devices[_ix].nv.cal, "%.2f", ftemp / 256.0f);				// "2.00"
				if((ds->decoder->meta.rfmeas & 0x0008) == 0x0008) {
					strcat(devices[_ix].nv.cal, " C");
				}
				else {
					strcat(devices[_ix].nv.cal, " U");
				}
				have_cal = true;
			}

			if(have_ver && have_cal) {
				break;
			}
		}					// if (status==0)
	#ifdef _DEBUG
		else {
			int bp = 3;
		}
	#endif
		ds->updateHardware();

		dt = timeGetTime() - start;
		if(dt >= 2000) {
			break;
		}

	}

	// dt = 749 ms

	DEL(ds);

	devices[_ix].when = timeGetTime();
	//devices[port->getPortNumber()-1] = d;

#ifndef WIN32
	devices[ix] = d;
#endif

#ifdef LOGDLL
	/*
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "read_computrainer x\n");
		fclose(logstream);
	 */
#endif

#ifdef _DEBUG
#ifdef WIN32
	HANDLE procHeap;
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif
#endif


	return ALL_OK;
}							// void read_computrainer(Serial *port)  {


/**********************************************************************
  ONLY called by check_for_trainers()
 **********************************************************************/

int read_server_computrainer(const char *_ix) {
	unsigned long start;
	const char *ix;

	if(server == NULL) {
		return GENERIC_ERROR;
	}


	//	ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	ix = _ix;

	Sleep(2000);


	//	sprintf(devices[_ix].nv.ver, "%d", server->get_fw(ix));
	//#ifdef _DEBUG
	//	int i = server->get_cal(ix);						// 266
	//#endif

	sprintf(devices[_ix].nv.cal, "%.2f", server->get_cal(ix) / 100.0f);				// "2.66"

	devices[_ix].when = timeGetTime();

	return ALL_OK;
}							// void read_computrainer(Server *_server)  {


/**********************************************************************
  ONLY called by check_for_trainers()
 **********************************************************************/

int read_velotron(Serial *port) {
	//DEVICE d = {DEVICE_NOT_SCANNED};
	DEVICE d;
	NVRAM::U *u;
	DWORD factoryTime;
	unsigned char version;
	unsigned char build;
	const char *ix = NULL;
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
	d.commtype = WIN_RS232;
	//d.port = port->getPortNumber();
	d.when = timeGetTime();

	//ix = port->getPortNumber()-1;
	//strncpy(d.id, ix, sizeof(d.id) - 1);
	ix = port->get_port_name();
	strncpy(d.id, ix, sizeof(d.id) - 1);


	devices[ix] = d;

	return ALL_OK;
}					// int read_velotron(Serial *port)  {


/**********************************************************************
  hd1
  hd4
  hd6, ul = 93,366

 **********************************************************************/

bool haveDevice(EnumDeviceType what, const char * _id) {
	DWORD now, dt;

	if(glog) glog->write(10, 0, 1, "hd1\n");
	const char *ccptr = NULL;
	ccptr = devices[_id].id;

	if(devices[_id].ev) {
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "      hd: ev is running, must have it?\n");
		FCLOSE(logstream);
	#endif

		if(devices[_id].what == what) {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "      hd: devices[_ix].what = %d, what = %d\n", devices[_id].what, what);
			fprintf(logstream, "      hd: devices[_ix].id = %s\n", devices[_id].id);
			fprintf(logstream, "      hd: returning true\n");
			FCLOSE(logstream);
		#endif
			if(!strcmp(devices[_id].id, ccptr)) {					// if thread is running, we have it
			}
			return true;
		}
		else {
			if(glog) glog->write(10, 0, 1, "hd2\n");
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "      hd: devices[_ix].what = %d, what = %d\n", devices[_id].what, what);
			fprintf(logstream, "      hd: ev running, but whats are different. Deleting ev and returning false\n");
			FCLOSE(logstream);
		#endif
			DEL(devices[_id].ev);
			if(glog) glog->write(10, 0, 1, "hd2b\n");
			return false;
		}

	#ifndef WIN32
		if(devices[_ix].what == what && !strcmp(devices[_ix].id, ccptr)) {					// if thread is running, we have it
			if(glog) {
				//glog->write(10,0,1,"hd1b: %s = %s, %d = %d, have it\n", devstrs[what], devstrs[devices[_ix].what], devices[_ix].port, comport);
			}
			return true;
		}
		else {
			if(glog) glog->write(10, 0, 1, "hd2\n");
			DEL(devices[_ix].ev);
			if(glog) glog->write(10, 0, 1, "hd2b\n");
			return false;
		}
	#endif

	}

	if(devices[_id].what != what) {
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "      hd: devices[_ix].what = %d, what = %d\n", devices[_id].what, what);
		fprintf(logstream, "      hd: ev not running, whats are different, returning false\n");
		FCLOSE(logstream);
	#endif
		if(glog) glog->write(10, 0, 1, "hd3: %s   %s\n", devstrs[what], devstrs[devices[_id].what]);
		return false;
	}

	if(glog) glog->write(10, 0, 1, "hd4\n");

	if(what == DEVICE_COMPUTRAINER || what == DEVICE_VELOTRON || what == DEVICE_SIMULATOR || what == DEVICE_RMP) {

		//if (devices[_ix].port==_ix+1)  {
		if(!strcmp(devices[_id].id, ccptr)) {
			now = timeGetTime();
			dt = now - devices[_id].when;
			if(glog) glog->write(10, 0, 1, "hd4b\n");

			if(dt < DEVICE_TIMEOUT_MS) {							// 10,000 ms
				if(glog) glog->write(10, 0, 1, "hd5, dt = %ld\n", dt);
				return true;
			}
		}
	}
	else if(what == DEVICE_NOT_SCANNED) {
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "      hd: device not scanned, returning false\n");
		FCLOSE(logstream);
	#endif
		if(glog) glog->write(10, 0, 1, "hd5B\n");
		return false;
	}
	else {
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "      hd: ERROR? device = %s, returning false\n", devstrs[what]);
		FCLOSE(logstream);
	#endif

		unsigned long ul;
		ul = DEVICE_TIMEOUT_MS;
		ul = timeGetTime();
		dt = timeGetTime() - devices[_id].when;

		if(dt < DEVICE_TIMEOUT_MS) {
			if(glog) glog->write(10, 0, 1, "hd6, ul = %ld\n", ul);
			return true;
		}
	}

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "      hdx: ERROR? device = %s, returning false\n", devstrs[what]);
	FCLOSE(logstream);
#endif

	if(glog) glog->write(10, 0, 1, "hdx\n");
	return false;

}				// bool haveDevice(EnumDeviceType what, int comport)

/**********************************************************************

**********************************************************************/

void check_dirs(const char *_logfilepath) {

	if(glog) {
		glog->write(10, 0, 1, "cd1\n");
	}

	if(dirs.size() != 0) {
		if(glog) {
			glog->write(10, 0, 1, "cd2\n");
		}
		return;
	}

	if(glog) {
		glog->write(10, 0, 1, "cd3\n");
	}

	setup_standard_dirs(_logfilepath);

	if(glog) {
		glog->write(10, 0, 1, "cdx\n");
	}


	return;
}

/*************************************************************************************************

 *************************************************************************************************/

void setup_standard_dirs(const char *_logfilepath) {				// _logfilepath defaults to NULL
	char str[256];
	int i;
	const char *cptr;

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "   internal:: setup_standard_dirs(%s)\n", _logfilepath ? _logfilepath : "no logfilepath");
	fprintf(logstream, "      dirs size = %ld\n", dirs.size());
	fclose(logstream);
#endif


	if(glog) {
		glog->write(10, 0, 1, "ssd1\n");
	}

	if(dirs.size() != 0) {
		if(glog) {
			glog->write(10, 0, 1, "ssd2\n");
		}
		return;
	}

	if(glog) {
		glog->write(10, 0, 1, "ssd3\n");
	}

	assert(dirs.size() == 0);

	if(_logfilepath != NULL) {
		if(!direxists(_logfilepath)) {
			CreateDirectory(_logfilepath, NULL);
		}


		for(i = 0; i < NDIRS; i++) {
			dirs.push_back(_logfilepath);
		}
		goto ssdx;
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

	if(glog) {
		glog->write(10, 0, 1, "ssd4\n");
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
	if(glog) {
		glog->write(10, 0, 1, "ssd5\n");
	}

	DIRTYPE dt;

	dt = (DIRTYPE)DIR_SETTINGS;							// 2
	assert(DIR_SETTINGS == 2);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
	if(glog) {
		glog->write(10, 0, 1, "ssd6\n");
	}

	dt = (DIRTYPE)DIR_REPORT_TEMPLATES;							// 3
	assert(DIR_REPORT_TEMPLATES == 3);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());

	dt = (DIRTYPE)DIR_REPORTS;									// 4
	assert(DIR_REPORTS == 4);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
	if(glog) {
		glog->write(10, 0, 1, "ssd7\n");
	}

	dt = (DIRTYPE)DIR_COURSES;								// 5
	assert(DIR_COURSES == 5);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());

	dt = (DIRTYPE)DIR_PERFORMANCES;							// 6
	assert(DIR_PERFORMANCES == 6);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());

	if(glog) {
		glog->write(10, 0, 1, "ssd8\n");
	}

	dt = (DIRTYPE)DIR_DEBUG;								// 7
	assert(DIR_DEBUG == 7);
	if(_logfilepath) {
		dirs.push_back(_logfilepath);
		if(!direxists(gstring)) {
			CreateDirectory(_logfilepath, NULL);
		}
		cptr = dirs[DIR_DEBUG].c_str();
	}
	else {
		dirs.push_back(dirs[DIR_PERSONAL].c_str());
	}

	if(glog) {
		glog->write(10, 0, 1, "ssd9\n");
	}

	dt = (DIRTYPE)DIR_HELP;								// 8
	assert(DIR_HELP == 8);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
#else
	//strcpy(personal_folder, "zzz");
	//strcpy(program_files_folder, "zzz");

	//bp = -1;

	for(i = 0; i < 8; i++) {
		cptr = dirs[i].c_str();
		if(!direxists(cptr)) {
			CreateDirectory(cptr, NULL);
		}
	}
	//strcpy(logpath, dirs[DIR_DEBUG]);
	sprintf(str, "%s%smain.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
	//	glog = new gloger(str);
#endif

ssdx:
#ifdef LOGDLL
	dump_dirs();
#endif

	if(glog) {
		glog->write(10, 0, 1, "ssdx\n");
	}

	return;

}				// void setup_standard_dirs()





bool iscal(EnumDeviceType what, const char *_ix) {
	const char *cptr;
	int i, n;

	if(what == DEVICE_COMPUTRAINER) {
		cptr = get_computrainer_cal(_ix);
		if(strstr(cptr, "U") == NULL) {
			return true;
		}
	}
	else if(what == DEVICE_VELOTRON) {
		cptr = get_velotron_cal(_ix);
		n = sscanf(cptr, "%d", &i);
		if(n != 1) {
			return false;
		}
		if(i < 100000) {
			return false;
		}
		if(i > 400000) {
			return false;
		}
		return true;
	}

	return false;

}					// bool iscal(EnumDeviceType what, int port)  {

/**********************************************************************

 **********************************************************************/

void dump_devices(void) {

	//int i;
	//DEVICE d;

	//printf("\n\n");

	//for (i=0; i<10; i++)  {
	//    d = devices[i];
	//    printf("%3d %-24s %ld %-20s %s\n",
	//            //d.port,
	//            i+1,
	//            devstrs[d.what],
	//            d.when,
	//            d.nv.cal,
	//            d.nv.ver
	//            );
	//}
	return;
}

/**********************************************************************

 **********************************************************************/

EnumDeviceType get_what(const char *_ix) {

	return devices[_ix].what;
}

/**********************************************************************

 **********************************************************************/

const char *get_computrainer_cal_from_thread(const char * ix, int fw) {
	const char *cptr;


	if(!devices[ix].ev) {
		return get_errstr2(DEVICE_NOT_RUNNING);
	}

	cptr = devices[ix].ev->get_computrainer_cal();
	strcpy(devices[ix].nv.cal, cptr);

	return cptr;
}

/**********************************************************************

 **********************************************************************/

const char *get_velotron_cal_from_thread(const char * ix, int fw) {
	const char *cptr;


	if(!devices[ix].ev) {
		return get_errstr2(DEVICE_NOT_RUNNING);
	}

	cptr = devices[ix].nv.cal;

	return cptr;
}

/**********************************************************************

 **********************************************************************/

int get_lower_hr(const char * ix) {

	int k;
	EnumDeviceType what, what2;

	what = devices[ix].what;

	if(what == DEVICE_COMPUTRAINER) {
	}
	else if(what == DEVICE_VELOTRON) {
	}
	else {
		return WRONG_DEVICE;
	}


	if(!devices[ix].ev) {
		return ROUND(devices[ix].rd.lower_hr);
	}

	what2 = devices[ix].ev->getwhat();
	if(what != what2) {
		return GENERIC_ERROR;
	}

	k = devices[ix].ev->get_lower_hr_x();

	return k;
}							// int get_lower_hr(int ix)

/**********************************************************************

 **********************************************************************/

int get_upper_hr(const char * ix) {

	int k;
	EnumDeviceType what, what2;

	what = devices[ix].what;

	if(what == DEVICE_COMPUTRAINER) {
	}
	else if(what == DEVICE_VELOTRON) {
	}
	else {
		return WRONG_DEVICE;
	}


	if(!devices[ix].ev) {
		return ROUND(devices[ix].rd.upper_hr);
	}

	what2 = devices[ix].ev->getwhat();
	if(what != what2) {
		return GENERIC_ERROR;
	}

	k = devices[ix].ev->get_upper_hr_x();
	return k;

}


/**********************************************************************

 **********************************************************************/

int get_hr_beep_enabled(const char * ix) {

	int k;
	EnumDeviceType what, what2;

	what = devices[ix].what;

	if(what == DEVICE_COMPUTRAINER) {
	}
	else if(what == DEVICE_VELOTRON) {
	}
	else {
		return WRONG_DEVICE;
	}


	if(!devices[ix].ev) {
		if(devices[ix].beep_enabled) {
			return 1;
		}
		else {
			return 0;
		}
	}

	what2 = devices[ix].ev->getwhat();
	if(what != what2) {
		return GENERIC_ERROR;
	}

	k = devices[ix].ev->get_hr_beep_enabled_x();
	return k;

}

#ifdef LOGDLL

/**********************************************************************

 **********************************************************************/

void dump_bike_params(const char * ix) {
	int i;
	FILE *logstream;



	logstream = fopen(DLL_LOGFILE, "a+t");
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
	for(i = 0; i < devices[ix].bike_params.nfront; i++) {
		fprintf(logstream, "         %d   %d\n", i, devices[ix].bike_params.front_gears[i]);
	}
	fprintf(logstream, "      rear gears:\n");
	for(i = 0; i < devices[ix].bike_params.nrear; i++) {
		fprintf(logstream, "         %d   %d\n", i, devices[ix].bike_params.rear_gears[i]);
	}

	fclose(logstream);
	return;
}											// void dump_bike_params(int ix)

/**********************************************************************

 **********************************************************************/

void dump_dirs(void) {
	int i;

	logstream = fopen(DLL_LOGFILE, "a+t");

	for(i = 0; i < (int)dirs.size(); i++) {
		fprintf(logstream, "      dirs[%d] = %s\n", i, dirs[i].c_str());
	}

	fclose(logstream);

	return;
}

#endif				// #ifdef LOGDLL


/**********************************************************************

 **********************************************************************/

int set_calibrating(const char * ix, int fw) {

	if(!devices[ix].ev) {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_calibrating_x(true);

	return ALL_OK;
}




/**********************************************************************

 **********************************************************************/

int get_cal_time(const char * ix, int fw) {

	EnumDeviceType what;

#ifdef LOGDLL
	//logstream = fopen(DLL_LOGFILE, "a+t");
	//fprintf(logstream, "set_get_cal_time(%d)\n", ix);
	//fclose(logstream);
#endif


	what = devices[ix].what;
	if(what != DEVICE_VELOTRON) {
		return WRONG_DEVICE;
	}

	if(!devices[ix].ev) {
		return DEVICE_NOT_RUNNING;
	}

	int cal = devices[ix].ev->get_cal_time_x();

	return cal;
}


/**********************************************************************

 **********************************************************************/

int startCal(const char * ix, int fw) {

	int status;

	if(!devices[ix].ev) {
		return DEVICE_NOT_RUNNING;
	}

	if(devices[ix].what != DEVICE_VELOTRON) {
		return WRONG_DEVICE;
	}

	status = devices[ix].ev->startCal_x();
	if(FAILED(status)) {
		return GENERIC_ERROR;
	}

	return ALL_OK;
}

/**********************************************************************

 **********************************************************************/

int get_calibration(const char * ix) {

	if(!devices[ix].ev) {
		return DEVICE_NOT_RUNNING;
	}
	if(devices[ix].what != DEVICE_VELOTRON) {
		return WRONG_DEVICE;
	}
	int cal;
	cal = devices[ix].ev->get_calibration_x();

	return cal;

}								// int get_calibration()


/**********************************************************************

 **********************************************************************/

int endCal(const char * ix, int fw) {
	int status;

	if(!devices[ix].ev) {
		return DEVICE_NOT_RUNNING;
	}

	if(devices[ix].what != DEVICE_VELOTRON) {
		return WRONG_DEVICE;
	}

	status = devices[ix].ev->endCal();
	if(FAILED(status)) {
		return GENERIC_ERROR;
	}

	return ALL_OK;
}


/**********************************************************************
  const char *get_errstr2(int err)  {
 **********************************************************************/

const char *get_errstr2(int err) {                                                                                                            // full

	const char *cptr;
	int i;

	if(err == 0) {
		return err_strs[0];
	}

	if(err > GENERIC_ERROR || err > 0) {
		return err_strs[NERRORS - 1];
	}

	i = err - INT_MIN + 1;

#ifdef _DEBUG
	if(i > NERRORS - 1) {
		throw(fatalError(__FILE__, __LINE__));
	}
#else
	if(i > NERRORS - 1) {
		i = NERRORS - 1;
	}
#endif

	cptr = err_strs[i];

	return cptr;
}								// const char *get_errstr2(int err)

/**********************************************************************

 **********************************************************************/

#ifdef DO_EXPORT

bool export_flag_file(void) {
#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	if(dirs.size() == 0) {
		fprintf(logstream, "   eff1, dirs size is 0\n");
		fclose(logstream);
		return false;
	}
	else {
		fprintf(logstream, "   eff1, dirs size = %d\n", dirs.size());
		fclose(logstream);
	}
#endif

	sprintf(gstring, "%s\\trigger.txt", dirs[DIR_DEBUG].c_str());

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "   eff2\n");
	fclose(logstream);
#endif

	if(exists(gstring)) {
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   %s exists, returning true\n", gstring);
		fclose(logstream);
	#endif
		return true;
	}

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "   %s does not exist, returning false\n", gstring);
	fclose(logstream);
#endif

	return false;
}
#endif

/**********************************************************************
  keeps reading until the timeout occurs
 **********************************************************************/

void flush(const char *_ix, Logger *_logg, RACERMATE::Ctsrv *_server, DWORD _timeout, BOOL _echo) {
	//unsigned char c[8];
	//DWORD start,end;
	//int i;
	//int n;
	//char str[FLUSH_STR_LEN];


	//if (_logg) _logg->write(0,0,0,"\nflush:\n");

	//start = end = timeGetTime();
	//i = 0;

	//while(1)  {
	//    n = _server->rx(_ix, c, 1);
	//    if (n==1)  {
	//        str[i++] = c[0];
	//        if (_echo) {
	//            if (_logg) _logg->write(0,0,0,"%c",c[0]);
	//        }
	//        if (i==FLUSH_STR_LEN-1)  {
	//            if (_logg) _logg->write(0,0,0,"i = 255 end =   %lu (%lu)\n",end, _timeout);
	//            if (_logg) _logg->write(0,0,0,"i = 255 start = %lu (%lu)\n",start,end-start);
	//            for(int j=0;j<FLUSH_STR_LEN;j++)  {
	//                if (_logg) _logg->write(0,0,0,"%c",str[j]);
	//            }
	//            break;
	//        }
	//    }
	//    end = timeGetTime();
	//    if ((end-start)>_timeout)  {
	//        if (_logg) _logg->write(0,0,0,"flush timeout end =   %lu (%lu)\n",end, _timeout);
	//        if (_logg) _logg->write(0,0,0,"flush timeout start = %lu (%lu)\n",start,end-start);
	//        break;
	//    }
	//}

	//str[i] = 0;

	return;
}							// flush()

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_trainers(const char *_id) {
	EnumDeviceType what = DEVICE_OTHER_ERROR;

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "      check_for_trainers(%s)\n", _id);
	fclose(logstream);
#endif

	if(glog) {
		glog->write(10, 0, 1, "   check_for_trainers(%s)\n", _id);
	}

	if(devices[_id].commtype == WIN_RS232) {
		what = check_for_windows_rs232_computrainer(_id);
		if(what != DEVICE_COMPUTRAINER) {
			what = check_for_windows_rs232_velotron(_id);
		}
	}
	else if(devices[_id].commtype == TRAINER_IS_CLIENT) {
		what = check_for_windows_server_computrainer(_id);						// try to connect to the computrainer handlebar client
	}
	else {
		sprintf(gstring, "commtype[%s] = %d", _id, devices[_id].commtype);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}

	return what;

}											// check_for_trainers()

/**********************************************************************

**********************************************************************/

#ifdef WIN32

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_windows_rs232_computrainer(const char *_id) {
	int baudrate;
	DWORD now, diff;
	char baudstr[16];
	Serial *port;
	DEVICE d;
	DEVICE *dp;
	int istat;


	dp = &devices[_id];
	baudrate = 2400;
	sprintf(baudstr, "%d", baudrate);

	now = timeGetTime();

	diff = now - last_port_time;

	if(diff < 500) {
		Sleep(diff);
	}
	last_port_time = timeGetTime();

	//--------------------------------------------
	// check for RS232 computrainer in windows
	//--------------------------------------------

	try {
		port = new Serial(bluetooth_delay, devices[_id].id, baudstr, FALSE, NULL);
	}
	catch(const int istatus) {
		if(glog) {
			glog->write(10, 0, 1, "   catch(%d)\n", istatus);
		}

		switch(istatus) {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
				d.what = DEVICE_DOES_NOT_EXIST;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_id] = d;
				return d.what;

			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				d.what = DEVICE_ACCESS_DENIED;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_id] = d;
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
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_id] = d;
				return d.what;

			default:
				d.what = DEVICE_OTHER_ERROR;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_id] = d;
				return d.what;
		}							// switch
	}								// catch
	catch(...) {
		if(glog) {
			glog->write(10, 0, 1, "   catch...\n");
		}
	}

	// opened serial port

	if(glog) {
		glog->write(10, 0, 1, "   serial port opened, checking for CT\n");
	}

	port->send("RacerMate", 0);						// no flush

	istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found

	 /*
while(1)  {
	Sleep(10);
}
	*/


	port->setShutDownDelay(10);

	if(istat == 0) {
		devices[_id].what = DEVICE_COMPUTRAINER;

		// known here
		//		rs232, windows
		//			devices[_ix].ix
		//			devices[_ix].id
		//			devices[_ix].commtype
		//			devices[_ix].what

		read_computrainer(_id, port);						// fills in the global info for this computrainer, devices[_ix]
		DEL(port);										// hangs here
		//devices[_ix].ix = _ix;

		// known here
		//		rs232, windows
		//			devices[_ix].ix
		//			devices[_ix].id
		//			devices[_ix].commtype
		//			devices[_ix].what
		//			devices[_ix].nv.ver
		//			devices[_ix].nv.cal

		return DEVICE_COMPUTRAINER;
	}
	else {
		DEL(port);										// <<<<<<<<<<<<<<<<<<<<< hangs here
	}

	return DEVICE_OTHER_ERROR;


	DEL(port);
	return DEVICE_OTHER_ERROR;
}								// check_for_windows_rs232_computrainer(const char *_ix)
#endif				// #ifdef WIN32


#ifndef WIN32

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_unix_rs232_computrainer(const char *_ix) {
	//int baudrate;
	//DWORD now, diff;
	char baudstr[16] = { "2400" };
	Serial *port;
	char comstr[32] = { 0 };
	DEVICE d;
	int istat;

	PORT_INFO pi = ix_to_port_info[_ix];
	assert(pi.type == UNIX_RS232);

	strncpy(comstr, pi.name.c_str(), sizeof(comstr) - 1);

	if(glog) {
		glog->write(10, 0, 1, "   opening %s at %s baud\n", comstr, baudstr);
	}

	port = new Serial(comstr, baudstr, NULL);				// calls constructor 1

	if(!port->is_open()) {
		if(glog) {
			glog->write(10, 0, 1, "   couldn't open serial port\n");
		}
		DEL(port);
		d.what = DEVICE_DOES_NOT_EXIST;
		//d.port = 0;
		strcpy(d.nv.ver, "n/a");
		strcpy(d.nv.cal, "n/a");
		d.when = timeGetTime();
		devices[_ix] = d;
		return d.what;
	}


	if(glog) {
		glog->write(10, 0, 1, "   serial port opened, checking for CT\n");
	}

	port->send("RacerMate", 0);						// no flush

	istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found

	port->setShutDownDelay(10);

	if(istat == 0) {
		read_computrainer(port);						// fills in the global info for this computrainer, devices[_ix]
		DEL(port);										// hangs here
		devices[_ix].ix = _ix;
	#ifdef _DEBUG
		d = devices[_ix];
	#endif
		return DEVICE_COMPUTRAINER;
	}
	else {
		DEL(port);										// <<<<<<<<<<<<<<<<<<<<< hangs here
	}

	return DEVICE_OTHER_ERROR;
}								// check_for_unix_rs232_computrainer()

#endif								// #ifndef WIN32

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_windows_rs232_velotron(const char *_ix) {
	int baudrate;
	char baudstr[16];
	Serial *port;
	//char comstr[32] = {0};
	DEVICE d;

#ifdef DO_PI
	PORT_INFO pi = ix_to_port_info[_ix];
	assert(pi.type == WIN_RS232);
	assert(pi.portnum == _ix + 1);

	strncpy(comstr, pi.name.c_str(), sizeof(comstr) - 1);
#endif

	//-------------------------------------------------------------
	// check for velotrons
	//-------------------------------------------------------------

	if(glog) {
		glog->write(10, 0, 1, "   checking for Velotron\n");
	}

	baudrate = 38400;

	sprintf(baudstr, "%d", baudrate);


#ifdef WIN32
	DWORD now, diff;
	now = timeGetTime();
	diff = now - last_port_time;
	if(diff < 500) {
		Sleep(diff);
	}
	last_port_time = timeGetTime();

	try {
	#ifdef _DEBUG
		port = new Serial(true, devices[_ix].id, baudstr, FALSE, NULL);							// showing errors
	#else
		port = new Serial(true, devices[_ix].id, baudstr, FALSE, NULL);
	#endif
	}
	catch(const int istatus) {
		switch(istatus) {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
				d.what = DEVICE_DOES_NOT_EXIST;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;

			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				//bp = 1;
				d.what = DEVICE_ACCESS_DENIED;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;

			case SERIAL_ERROR_BUILDCOMMDCB:
			case SERIAL_ERROR_SETCOMMSTATE:
			case SERIAL_ERROR_CREATEEVENT1:
			case SERIAL_ERROR_CREATEEVENT2:
			case SERIAL_ERROR_OTHER:
				d.what = DEVICE_OPEN_ERROR;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;

			default:
				d.what = DEVICE_OTHER_ERROR;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;
		}
	}

#else
	port = new Serial(comstr, baudstr, NULL);
#endif										// #ifdef WIN32

	// not catching here because I already know that the port exists and is not a computrainer

	port->setShutDownDelay(10);

	if(isVelotron(port, 500)) {
		read_velotron(port);						// fills in the global info for this computrainer
		DEL(port);
		return DEVICE_VELOTRON;
	}
	DEL(port);

	// port exists, but not a computrainer or velotron

	d.what = DEVICE_EXISTS;

#ifdef DO_PI
	strncpy(d.id, ix_to_port_info[_ix].name.c_str(), sizeof(d.id) - 1);
#endif

	strcpy(d.nv.ver, "n/a");
	strcpy(d.nv.cal, "n/a");
	d.when = timeGetTime();
	devices[_ix] = d;

	return d.what;
}											// check_for_windows_rs232_velotron()




/**********************************************************************
	keeps reading until the timeout occurs
**********************************************************************/

void flush(Logger *logg, Client *_client, DWORD timeout, BOOL echo) {
	_client->flush(timeout);
	return;
}							// flush()


/**********************************************************************
	ONLY called by check_for_trainers()
**********************************************************************/

int read_computrainer(const char *_ix, Client *_client) {
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//int bp = 0;
	unsigned short version;
	float ftemp;
	bool have_ver;
	bool have_cal;
	RIDER_DATA rd;
	Course course;
	Bike bike;

#ifdef LOGDLL
	/*
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "internal read_computrainer()\n");
	fclose(logstream);
	*/
#endif

	ds = new Computrainer(_client, rd, &course, &bike);

	if(!ds->is_initialized()) {
		DEL(ds);
		return GENERIC_ERROR;
	}

	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	have_ver = false;
	have_cal = false;

	start = timeGetTime();

	// read computrainer for 2 seconds to get all packets, such as version, rfdrag, hr sensor flags

	 // known here
	 //		win32 trainer is server
	 //			devices[_ix].ix
	 //			devices[_ix].commtype
	 //			devices[_ix].tcp_port
	 //			devices[_ix].url
	 //			devices[_ix].id
	 //			devices[_ix].what

	while(1) {
		//status = ds->getNextRecord(0);
		status = ds->getNextRecord();
		if(status == 0) {
			version = ds->decoder->get_version();
			if(version != 0) {
				sprintf(devices[_ix].nv.ver, "%hd", version);
				have_ver = true;
			}

			if(ds->decoder->meta.rfdrag != 0) {
				ftemp = (float)(ds->decoder->meta.rfdrag & 0x07ff);
				sprintf(devices[_ix].nv.cal, "%.2f", ftemp / 256.0f);
				if((ds->decoder->meta.rfmeas & 0x0008) == 0x0008) {
					strcat(devices[_ix].nv.cal, " C");
				}
				else {
					strcat(devices[_ix].nv.cal, " U");
				}
				have_cal = true;
			}

			if(have_ver && have_cal) {
				break;
			}
		}					// if (status==0)
	#ifdef _DEBUG
		else {
			int bp = 1;
		}
	#endif

		ds->updateHardware();
	#ifdef _DEBUG
		//cnt++;
	#endif

		if((timeGetTime() - start) >= 2000) {
			break;
		}

	}

	DEL(ds);

	devices[_ix].when = timeGetTime();
	//const char *_ix = _client->getPortNumber() -1;
	//d.tcp_port = devices[_ix].tcp_port;
	//strncpy(d.url, devices[_ix].url, sizeof(d.url)-1);
	 //strncpy(d.id, devices[_ix].id, sizeof(d.id)-1);

	//strncpy(d.id, d.url, sizeof(d.id)-1);
#ifdef DO_PI
	const char *ccptr = ix_to_port_info[_ix].name.c_str();
	strncpy(d.id, ccptr, sizeof(d.id) - 1);
#endif

	//d.ix = _ix;

 /*
 enum TRAINER_COMMUNICATION_TYPE {
	 BAD_INPUT_TYPE = -1,
	 WIN_RS232,
	 UNIX_RS232,
	 TRAINER_IS_SERVER,									// trainer is a server
	 TRAINER_IS_CLIENT									// trainer is a client
 };
 */

 //d.commtype = TRAINER_IS_SERVER;

//devices[_ix] = d;

#ifdef LOGDLL
	/*
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "read_computrainer x\n");
	fclose(logstream);
	*/
#endif

#ifdef _DEBUG
#ifdef WIN32
	HANDLE procHeap;
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif
#endif

	// known here
	//		win32 trainer is server
	//			devices[_ix].ix
	//			devices[_ix].commtype
	//			devices[_ix].tcp_port
	//			devices[_ix].url
	//			devices[_ix].id
	//			devices[_ix].what
	//			devices[_ix].nv.ver
	//			devices[_ix].nv.cal
	//			devices[_ix].when

	return ALL_OK;
}							// void read_computrainer(Client *)  {

#ifdef DO_PI
/**********************************************************************

**********************************************************************/
#ifdef LOGDLL
void dump_maps(void) {
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "dump_maps\n");

	// std::map<std::string, int> portname_to_ix;
	// std::map<int, PORT_INFO> ix_to_port_info;

	for(std::map<std::string, int>::iterator it = portname_to_ix.begin(); it != portname_to_ix.end(); ++it) {
		fprintf(logstream, "   %s    =>    %d\n", it->first.c_str(), it->second);
	}

	fprintf(logstream, "\n");

	for(std::map<int, PORT_INFO>::iterator it = ix_to_port_info.begin(); it != ix_to_port_info.end(); ++it) {
		fprintf(logstream, "%d    =>    portname = %s, portnum = %d, type = %d\n", it->first, it->second.name.c_str(), it->second.portnum, it->second.type);
	}

	FCLOSE(logstream);

	return;
}
#endif
#endif


/**********************************************************************
check for WIFI computrainer
the trainer is the client
**********************************************************************/

EnumDeviceType check_for_windows_server_computrainer(const char *_id) {
	//Client *client = NULL;

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "            check_for_windows_client_computrainer(%s)\n", _id);
	fclose(logstream);
#endif

	if(!strstr(_id, "UDP-")) {
		return DEVICE_OTHER_ERROR;
	}

	if(server == NULL) {
		return DEVICE_OTHER_ERROR;
	}

#ifdef _DEBUG
	DEVICE *d;
	d = &devices[_id];
#endif

	/*
	unsigned long start = timeGetTime();

	while (1) {
		if (server->is_client_connected(_id)) {
			read_server_computrainer(_id);							// reads computrainer for 2 seconds, fills in stuff
			devices[_id].what = DEVICE_COMPUTRAINER;
			break;
		}

		if ((timeGetTime() - start) >= 10000) {
			devices[_id].what = DEVICE_DOES_NOT_EXIST;
			break;
		}

	}
	*/

	devices[_id].when = timeGetTime();
	return devices[_id].what;

}						// check_for_windows_server_computrainer()


#ifdef _DEBUG

/**********************************************************************

**********************************************************************/

int check_maps(const char *_caller)  {

	if(!server) {
		return 0;
	}

	DEVICE *d;
	//const char *const* appkey;				// eg, "UDP-5678"
	//const char *ccptr;
	int n = 0;

	for(auto it = devices.begin(); it != devices.end(); ++it) {
		/*
		appkey = &it->first;
		ccptr = *appkey;
		if(!strstr(ccptr, "UDP-")) {
			throw(fatalError(__FILE__, __LINE__));							// never gets here
		}
		*/

#ifdef _DEBUG
		d = &it->second;
		char str[256];
		n = devices.size();
		sprintf(str, "caller = %s, n = %d, udpkey = %s\n", _caller, n, d->udpkey);
		OutputDebugString(str);
		if(strstr(d->udpkey, "192.168.1.40 ")==NULL) {
			/*
			for(auto it = devices.begin(); it != devices.end(); ++it) {
				appkey = &it->first;							// "UDP-6666"
				ccptr = d->udpkey;							// ""
			}
			*/

			// after GetIsCalibrated									1			492.
			// after startTrainer()										5			"4", y
			// trainer.cpp, init() after setPaused 1				7			1, 1,1

			//throw(fatalError(__FILE__, __LINE__));
			return -1;
		}
#endif

		//n++;
	}

	//if (!strstr(d->udpkey, "192.168.1.40 ")) {
	//	throw(fatalError(__FILE__, __LINE__));
	//}
	return n;
}
#endif


/**********************************************************************

**********************************************************************/

//DLLSPEC int check_maps(void) {
//	return DLLSPEC int();
//}


/**********************************************************************

	syncs
	worker->clients			QHash<QString, UDPClient *> clients;
		with
	devices						std::unordered_map<std::string, DEVICE> devices


	example input:
		_appkey = "UDP-5678"
		_udpkey = "192.168.1.40 37842"

	this is where devices can grow?

**********************************************************************/

ADJCODE sync(const char *_appkey, const char *_udpkey)  {
	ADJCODE rc = ADJ_OK;
	DWORD now;

	now = timeGetTime();

	if((now - dllstarttime) < 15000) {				// wait until the dll has been loaded for at least 10 seconds
		return rc;
	}

	if((now - lastsynctime) < 2333) {
		return rc;
	}


	// only sync/scan every 2.333 seconds

	lastsynctime = now;

#ifdef _DEBUG
	int bp = 0;
#endif

	int ndevices, nqtclients;
	ndevices = devices.size();
	nqtclients = server->get_nclients();

	if(nqtclients == ndevices) {
		// number matches, but should check the ids to make sure they're in sync

		std::unordered_map<std::string, DEVICE>::const_iterator it;
		//std::string appkey;
		const char *appkey;
		const char *udpkey;
		const char *devnum;
		const char *x;

		for(auto device : devices) {							// for each device
			appkey = device.first.c_str();					// "UDP-5678"
			devnum = &appkey[4];									// "5678"
			udpkey = device.second.udpkey;					// "192.168.1.40 59192"

			if(!server->have(udpkey))  {
				// no matching key, do something

				#ifdef _DEBUG
				printf("don't have passed in udpkey = %s\n", _udpkey);
				static int cnt = 0;
				cnt++;
				if(cnt == 2) {
					bp = 3;
				}
				#endif

				x = server->get_udpkey_from_devnum(devnum);

				if(x != NULL) {
					// it reconnected on a different client port, so the udpkey has changed to 'x'. Update devices list
					#ifdef _DEBUG
					printf("   switching %s to %s\n", appkey, x);
					#endif

					//strncpy(device.second.udpkey, x, sizeof(device.second.udpkey) - 1);
					strncpy(devices[_appkey].udpkey, x, sizeof(devices[_appkey].udpkey) - 1);

					//bp = 2;
				}
				// otherwise do nothing for now. it's up to the high level app to acknowledge the new client
			}
		}

		//bp = 1;
	}
	else if(ndevices > nqtclients) {
		// a qt client went away, but it might come back later, so do nothing?
		//bp = 2;
	}
	else {
		// nqtclients > ndevices, a new udp client has connected (after 10 seconds since the dll was loaded)
		//bp = 3;
	}



#if 0
	std::unordered_map<std::string, DEVICE>::const_iterator it;

	static int lastdiff = -99;

#ifdef _DEBUG
	int bp = 0;
	const char *ccptr;
#endif

	if(!server) {
		return ADJ_NOSERVER;
	}

	int ndevices, nqtclients;
	ndevices = devices.size();								// 1

	if(ndevices != 2) {
		bp = 2;
	}
	nqtclients = server->get_nclients();				// 0

	//it = devices.find(_appkey);
	//if(it != devices.end()) {

	std::string appkey;
	std::string udpkey;
	/*

	bp = 0;

	for(auto device : devices) {
		appkey = device.first;
		udpkey = device.second.udpkey;

		if(!server->have(_udpkey)) {
		}
		bp = 2;
	}
	*/

	//std::string s;
	//s = get_udp_trainers();
	//std::vector<std::string> clients;

	//char str[256];
	ccptr = get_udp_trainers();

	strncpy(udp_client_app_key_list, get_udp_trainers(), sizeof(udp_client_app_key_list) - 1);

	char * pch;

	pch = strtok(udp_client_app_key_list, " ");

	bp = 0;
	bool haveit;

	while(pch != NULL) {
		//printf("%s\n", pch);
		bp++;
		haveit = false;
		for(auto device : devices) {
			std::string s2 = pch;
			appkey = device.first;

			if(appkey == s2) {
				haveit = true;
				break;
			}
			//udpkey = device.second.udpkey;
		}

		if(!haveit) {
			// don't have UDP-6666, 
			bp = 9;
		}
		pch = strtok(NULL, " ");
	}

	bp = 3;
#endif


#if 0
	const char *ccptr = get_udp_trainers();


	if(nqtclients != ndevices) {
		if(ndevices > nqtclients) {
			// one of the clients disconnected, but may come back
			#ifdef _DEBUG
			bp = 0;
			#endif
		}
		else {
			// ndevices < nqtclients
			if(!server->have(_udpkey)) {
				if(_udpkey[0] != 0) {
					std::unordered_map<std::string, DEVICE>::const_iterator got;
					got = devices.find(_appkey);
					if(got != devices.end()) {
						DEVICE *d;
						d = &devices[_appkey];
						//d->commtype = ;
						memset(d->udpkey, 0, sizeof(d->udpkey));
						//devices.erase(_appkey);
						rc = ADJ_UDPKEY_RESET;
						goto finis;
					}
					else {
						rc = ADJ_NO_DEVICES;
						goto finis;
					}
				}
			}
			else {
				//rc = ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES;
				goto finis;
			}
		}						// 		if(ndevices < nqtclients) {
	}
	else {
		if(lastdiff > 0) {
			bp = 2;
		}

		if(_udpkey[0] == 0) {
			const char *cptr = server->get_udpkey_from_devnum(&_appkey[4]);
			if(cptr == NULL) {
				rc = ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL;
				goto finis;
			}
			strncpy(
						devices[_appkey].udpkey,
						cptr,
						sizeof(devices[_appkey].udpkey)
			);

			//server->set
			#ifdef _DEBUG
			bp = 1;
			#endif
		}
		else {
		}
	}

finis:
	lastdiff = ndevices - nqtclients;


#endif

	return rc;

}									// sync()

/**********************************************************************

**********************************************************************/

void init_udp_device(const char *_id)  {
	//devices[_id].udpkey = server->get_udpkey_from_devnum(&_id[4]);						// eg, "192.168.1.40 44085"

#ifdef _DEBUG
	const char *udpkey;
	const char *ccptr;
	ccptr = &_id[4];
	udpkey = server->get_udpkey_from_devnum(&_id[4]);
	int bp = 1;
#endif

	strncpy(
		devices[_id].udpkey,
		server->get_udpkey_from_devnum(&_id[4]),
		sizeof(devices[_id].udpkey)
	);

	//#ifdef _DEBUG
	#if 0
		DEVICE *d;
		DWORD start, dt;
		int status;

		d = &devices[_id];

		start = timeGetTime();
		while((timeGetTime() - start) < 5000) {
			status = check_maps("dll.cpp, GetRacerMateDeviceID");
			if(status == -1) {
				dt = timeGetTime() - start;
				status = 1;
			}
			Sleep(50);
		}
	#endif

	devices[_id].commtype = TRAINER_IS_CLIENT;
	devices[_id].what = DEVICE_COMPUTRAINER;							// not doing wifi velotrons at the moment

	#ifdef _DEBUG
		int n = check_maps("dll.cpp, GetRacerMateDeviceID");
		if(n > 1) {
			bp = 2;
		}
	#endif

	//Sleep(400);					// sleep a bit to allow decode_slow() to execute

	// wait up to 750 ms for fw to come in (decode_slow())
	unsigned long start = timeGetTime();
	unsigned short fw;
	int cal;

	while(1)  {
		fw = server->get_fw(devices[_id].udpkey);
		if (fw != 0)  {
			cal = server->get_cal(devices[_id].udpkey);
			sprintf(devices[_id].nv.ver, "%d", fw);
			sprintf(devices[_id].nv.cal, "%d", cal);
			devices[_id].when = timeGetTime();
			break;
		}
		if ((timeGetTime() - start) >= 750)  {
			break;
		}
	}
	return;
}						// void init_udp_device(const char *_id)

/**********************************************************************
	interrupts at 1000 ms
	timer that controls scanning for ant devices and sensors
**********************************************************************/

void CALLBACK ant_timer_proc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {

	timer_calls++;

#ifdef _DEBUG
	//at->update();						// 1000.00 ms
	static fastTimer ft("dll_timer");
	static fastTimer ft2("dll_timer2");
	static double tot1 = 0.0;
	static int scannum1 = 0;
	static double tot2 = 0.0;
	static int scannum2 = 0;
	DWORD tid = GetCurrentThreadId();
#endif

#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "tick\n");
	fclose(logstream);
#endif


	dll_tick++;

	if(timer_calls < 2) {
		if(dll_tick < 8) {
			return;
		}
	}
	else {
		if(dll_tick < 8) {
			return;
		}
	}

	//------------------------------------
	// gets here every 8 seconds
	//------------------------------------


	dll_tick = 0;

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "tock\n");
		fclose(logstream);
	#endif



	int status = -1;
	//Q_UNUSED(status);


	if(ant) {
		if(!ant->get_scanning_for_devices()) {										// checcks bool flag
			#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "calling scan_for_devices\n");
			fclose(logstream);
			#endif

		#ifdef _DEBUG
			ft.start();
		#endif

			/*
				with ant stick		first call is 1.6 seconds, after that average is about 150 ms
				without ant stick	first call is  .142 seconds, after that average is about 150 ms
			*/
			status = ant->scan_for_devices();										// can take > 3 seconds the first time <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		#ifdef _DEBUG
			double secs = ft.stop();
			char str[128];
			scannum1++;
			tot1 += secs;
			if(status) {
				sprintf(str, "scan_for_devices took %.3lf seconds, status = %d\n", secs, status);
			}
			else {
				sprintf(str, "scan_for_devices took %.3lf seconds, avg = %.0lf ms, timerproc tid = %08lx, gtid = %08lx\n", secs, (tot1 / scannum1)*1000, tid, gtid);
			}
			OutputDebugString(str);
		#endif

			#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "scan_for_devices returned %d\n", status);
			fclose(logstream);
			#endif

			if(status) {
				return;
			}
			else  {
				#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "internal timer, calling get_n_devices()\n");
				fclose(logstream);
				#endif
			}
		}					// if(!ant->get_scanning_for_devices()) {



		if(ant->get_n_devices() == 0) {
			#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "ok2\n");
			fclose(logstream);
			#endif
			return;
		}

		// if no sticks, this is not reached

		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "ok3\n");
		fclose(logstream);
		#endif

		if(!ant->is_listening()) {
			#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "ok4\n");
			fclose(logstream);
			#endif

		#ifdef _DEBUG
			ft2.start();
		#endif

			/*
			with ant stick		only called once, took 3.2 seconds
			without ant stick	not called
			*/

			status = ant->start_listening();			// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		#ifdef _DEBUG
			double secs = ft2.stop();
			char str[128];
			scannum2++;
			tot2 += secs;
			if(status) {
				sprintf(str, "start_listening took %.3lf seconds, status = %d\n", secs, status);
			}
			else {
				sprintf(str, "start_listening took %.3lf seconds, avg = %.0lf ms\n", secs, (tot2 / scannum2) * 1000);
			}
			OutputDebugString(str);
		#endif

		}

		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "ok5 = %d\n", status);
		fclose(logstream);
		#endif

		if(status == 0) {
			#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "ok6\n");
			fclose(logstream);
			#endif

			//const char *ccptr = ant->get_sensors_string();

			#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "ok7b\n");
			fclose(logstream);
			#endif
#ifdef _DEBUG
			//if(ccptr) {
				//qDebug("SENSORS = %s\n", ccptr);
			//}
			//else {
				//qDebug("no sensors\n");
			//}
#endif

		}
	}								// TimerProc()



//	if(ant) {
//		int status;
//		status = ant->scan_for_devices();
//		status = ant->start_listening();
//	}

	return;
}


/*********************************************************

*********************************************************/

int scan_for_ant_devices(void) {
	if(ant) {
		return ant->scan_for_devices();
	}
	return -1;
}


