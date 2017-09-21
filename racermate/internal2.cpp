
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
#include <assert.h>
#include <shlobj.h>
#endif

#include <nvram.h>
#include <computrainer.h>

#include "dll_common.h"
#include "dll_globals.h"

#include "internal.h"

#define FLUSH_STR_LEN 256



/**********************************************************************
  this is .../_fs/dll/internal.cpp
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

#ifdef NEWPORT
const char *get_computrainer_firmware(int _ix)  {								// 0 - 255
#else
const char *get_computrainer_firmware(int _comport)  {
#endif

	dataSource *ds = NULL;
	DWORD start;
	int status;

#ifndef NEWPORT
	int _ix = _comport - 1;
#endif

	unsigned short version;
	static char verstr[32] = {0};
	bool b;
	RIDER_DATA rd;

	Course course;
	Bike bike;


#ifdef NEWPORT
	b = haveDevice(DEVICE_COMPUTRAINER, _ix);
#else
	b = haveDevice(DEVICE_COMPUTRAINER, _comport);
#endif

	if (b)  {
		return devices[_ix].nv.ver;
	}

	char str[32];
	strncpy(str, devices[_ix].portname, sizeof(str)-1);

	ds = new Computrainer(str, rd, &course, &bike, NO_LOGGING, "", -1);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return NULL;
	}

	start = timeGetTime();
	ds->decoder->set_version(0);							// unsigned short

	while(1)  {
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

	return verstr;

}			// const char *get_computrainer_firmware(int _comport)

/**********************************************************************
  comport starts at 1
  needs a 2 second delay after previouse serial port open to handlebar
  before it will respond correctly
 **********************************************************************/
// redoc
const char *get_computrainer_cal(int _ix)  {

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

	b = haveDevice(DEVICE_COMPUTRAINER, _ix);
	if (b)  {
		return devices[_ix].nv.cal;
	}


	sprintf(comstr, "COM%d", _ix+1);

	ds = new Computrainer(comstr, rd, &course, &bike, NO_LOGGING, "", -1);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return NULL;
	}

	start = timeGetTime();
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	while(1)  {
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

	DEL(ds);

	return calstr;
}								// char *get_computrainer_cal(int _comport)  {


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

	int i;
	i = haveDevice(DEVICE_COMPUTRAINER, dev->ix);
	if (i != -1)  {
		memcpy(&dev->nv, &devices[i].nv, sizeof(NV));
		return ALL_OK;
	}

	memset(&dev->nv, 0, sizeof(dev->nv));

	strncpy(comstr, dev->portname, sizeof(comstr)-1);
	ds = new Computrainer(comstr, rd, &course, &bike, NO_LOGGING, "", -1);

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
		return devices[_comport-1].nv.ver;
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

		// every 10th byte should have the high bit set

		int count = 0;

		for(i=0;i<n;i++)  {
			if ((buf[i] & 0x80)==0x80)  {
				if (count==10)  {
					//bp = 1;
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
		//bp = 1;
		return false;
	}					// if (bytes received)
#ifdef _DEBUG
	else  {
		//bp = 0;
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
	//DEVICE d = {DEVICE_NOT_SCANNED};
	DEVICE d;
	RIDER_DATA rd;
	Course course;
	Bike bike;
	DWORD dt;

#ifdef LOGDLL
	/*
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "internal read_computrainer()\n");
		fclose(logstream);
	 */
#endif

	//bike.id = id;
	ds = new Computrainer(port, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return GENERIC_ERROR;
	}

	d.what = DEVICE_COMPUTRAINER;
	//d.port = port->getPortNumber();

	const char *cptr = port->get_port_name();

	strncpy(d.portname, cptr, sizeof(d.portname)-1);

	int ix = portname_to_ix[cptr];

	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	have_ver = false;
	have_cal = false;

	start = timeGetTime();

	// read computrainer for 2 seconds to get all packets, such as version, rfdrag, hr sensor flags

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
			}

			if (have_ver && have_cal)  {
				break;
			}
		}					// if (status==0)

		ds->updateHardware();

		dt = timeGetTime() - start;
		if ( dt >= 2000)  {
			break;
		}

	}

	// dt = 749 ms

	DEL(ds);

	d.when = timeGetTime();
	//devices[port->getPortNumber()-1] = d;
	devices[ix] = d;

#ifdef LOGDLL
	/*
		logstream = fopen(DLLFILE, "a+t");
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

int read_computrainer(int _ix, Server *_server)  {
	dataSource *ds = NULL;
	DWORD start;
	int status;
	//int bp = 0;
	unsigned short version;
	float ftemp;
	bool have_ver;
	bool have_cal;
	//DEVICE d = {DEVICE_NOT_SCANNED};
	DEVICE d;
	RIDER_DATA rd;
	Course course;
	Bike bike;

#ifdef LOGDLL
	/*
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "internal read_computrainer()\n");
		fclose(logstream);
	 */
#endif

	//bike.id = id;
	ds = new Computrainer(_ix+1, _server, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return GENERIC_ERROR;
	}

	d.what = DEVICE_COMPUTRAINER;
	//d.port = port->getPortNumber();
	//d.port = SERVER_SERIAL_PORT;
	//d.port = 231;


	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	have_ver = false;
	have_cal = false;

	start = timeGetTime();

	// read computrainer for 2 seconds to get all packets, such as version, rfdrag, hr sensor flags

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
	//devices[port->getPortNumber()-1] = d;
	//devices[SERVER_SERIAL_PORT-1] = d;
	devices[230] = d;

#ifdef LOGDLL
	/*
		logstream = fopen(DLLFILE, "a+t");
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
}							// void read_computrainer(Server *_server)  {


/**********************************************************************
  ONLY called by check_for_trainers()
 **********************************************************************/

int read_velotron(Serial *port)  {
	//DEVICE d = {DEVICE_NOT_SCANNED};
	DEVICE d;
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
	//d.port = port->getPortNumber();
	d.when = timeGetTime();

	devices[port->getPortNumber()-1] = d;

	return ALL_OK;
}					// int read_velotron(Serial *port)  {


/**********************************************************************
  hd1
  hd4
  hd6, ul = 93,366

 **********************************************************************/

#ifdef NEWPORT
bool haveDevice(EnumDeviceType what, int _ix)  {
#else
bool haveDevice(EnumDeviceType what, int comport)  {
#endif

	DWORD now, dt;

	if (glog) glog->write(10,0,1,"hd1\n");
	const char *ccptr = ix_to_portname[_ix].c_str();

	if (devices[_ix].ev)  {
		//if (devices[_ix].what==what && devices[_ix].port==comport)  {					// if thread is running, we have it
		if (devices[_ix].what==what && !strcmp(devices[_ix].portname, ccptr))  {					// if thread is running, we have it
			if (glog)  {
				//glog->write(10,0,1,"hd1b: %s = %s, %d = %d, have it\n", devstrs[what], devstrs[devices[_ix].what], devices[_ix].port, comport);
			}
			return true;
		}
		else  {
			if (glog) glog->write(10,0,1,"hd2\n");
			DEL(devices[_ix].ev);
			if (glog) glog->write(10,0,1,"hd2b\n");
			return false;
		}
	}

	if (devices[_ix].what!=what)  {
		if (glog) glog->write(10,0,1,"hd3: %s   %s\n", devstrs[what], devstrs[devices[_ix].what]);
		return false;
	}

	if (glog) glog->write(10,0,1,"hd4\n");

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON || what==DEVICE_SIMULATOR || what==DEVICE_RMP)  {

		//if (devices[_ix].port==_ix+1)  {
		if (!strcmp(devices[_ix].portname, ccptr))  {

			now = timeGetTime();
			dt = now - devices[_ix].when;
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
		dt = timeGetTime() - devices[_ix].when;

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

	if (glog)  {
		glog->write(10,0,1,"cd3\n");
	}

	setup_standard_dirs(_logfilepath);

	if (glog)  {
		glog->write(10,0,1,"cdx\n");
	}


	return;
}

/*************************************************************************************************

 *************************************************************************************************/

void setup_standard_dirs(const char *_logfilepath)  {				// _logfilepath defaults to NULL
	char str[256];
	int i;
	const char *cptr;

#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	fprintf(logstream, "internal:: setup_standard_dirs(%s)\n", _logfilepath?_logfilepath:"no logfilepath");
	fprintf(logstream, "   dirs size = %ld\n", dirs.size());
	fclose(logstream);
#endif


	if (glog)  {
		glog->write(10,0,1,"ssd1\n");
	}

	if (dirs.size() != 0)  {
		if (glog)  {
			glog->write(10,0,1,"ssd2\n");
		}
		return;
	}

	if (glog)  {
		glog->write(10,0,1,"ssd3\n");
	}

	assert(dirs.size()==0);

	if (_logfilepath != NULL)  {
		if (!direxists(_logfilepath))  {
			CreateDirectory(_logfilepath, NULL);
		}


		for(i=0; i<NDIRS; i++)  {
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

	DIRTYPE dt;

	dt = (DIRTYPE)DIR_SETTINGS;							// 2
	assert(DIR_SETTINGS==2);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
	if (glog)  {
		glog->write(10,0,1,"ssd6\n");
	}

	dt = (DIRTYPE)DIR_REPORT_TEMPLATES;							// 3
	assert(DIR_REPORT_TEMPLATES==3);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());

	dt = (DIRTYPE)DIR_REPORTS;									// 4
	assert(DIR_REPORTS==4);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
	if (glog)  {
		glog->write(10,0,1,"ssd7\n");
	}

	dt = (DIRTYPE)DIR_COURSES;								// 5
	assert(DIR_COURSES==5);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());

	dt = (DIRTYPE)DIR_PERFORMANCES;							// 6
	assert(DIR_PERFORMANCES==6);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());

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
		dirs.push_back(dirs[DIR_PERSONAL].c_str());
	}

	if (glog)  {
		glog->write(10,0,1,"ssd9\n");
	}

	dt = (DIRTYPE)DIR_HELP;								// 8
	assert(DIR_HELP==8);
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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

ssdx:
#ifdef LOGDLL
	dump_dirs();
#endif

	if (glog)  {
		glog->write(10,0,1,"ssdx\n");
	}

	return;

}				// void setup_standard_dirs()




/**********************************************************************
  port starts at 1
 **********************************************************************/

#ifdef NEWPORT
bool iscal(EnumDeviceType what, int _ix)  {
#else
bool iscal(EnumDeviceType what, int _port)  {
#endif

	const char *cptr;
	int i, n;

	if (what==DEVICE_COMPUTRAINER)  {
		cptr = get_computrainer_cal(_ix);
		if (strstr(cptr, "U")==NULL)  {
			return true;
		}
	}
	else if (what==DEVICE_VELOTRON)  {
		cptr = get_velotron_cal(_ix);
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

/**********************************************************************

 **********************************************************************/

void dump_devices(void)  {

	int i;
	DEVICE d;

	printf("\n\n");

	for (i=0; i<10; i++)  {
		d = devices[i];
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

 **********************************************************************/

const char *get_computrainer_cal_from_thread(int ix, int fw)  {
	const char *cptr;


	if (!devices[ix].ev)  {
		return get_errstr2(DEVICE_NOT_RUNNING);
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
		return get_errstr2(DEVICE_NOT_RUNNING);
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

	k = devices[ix].ev->get_lower_hr_x();

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

	k = devices[ix].ev->get_upper_hr_x();
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

	k = devices[ix].ev->get_hr_beep_enabled_x();
	return k;

}

#ifdef LOGDLL

/**********************************************************************

 **********************************************************************/

void dump_bike_params(int ix)  {
	int i;
	FILE *logstream;



	logstream = fopen(DLLFILE, "a+t");
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
}											// void dump_bike_params(int ix)

/**********************************************************************

 **********************************************************************/

void dump_dirs(void)  {
	int i;

	logstream = fopen(DLLFILE, "a+t");

	for(i=0; i<(int)dirs.size(); i++)  {
		fprintf(logstream, "      dirs[%d] = %s\n", i, dirs[i].c_str());
	}

	fclose(logstream);

	return;
}

#endif				// #ifdef LOGDLL


/**********************************************************************

 **********************************************************************/

int set_calibrating(int ix, int fw)  {

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_calibrating_x(true);

	return ALL_OK;
}




/**********************************************************************

 **********************************************************************/

int get_cal_time(int ix, int fw)  {

	EnumDeviceType what;

#ifdef LOGDLL
	//logstream = fopen(DLLFILE, "a+t");
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

	int cal = devices[ix].ev->get_cal_time_x();

	return cal;
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

	status = devices[ix].ev->startCal_x();
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
	cal = devices[ix].ev->get_calibration_x();

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


/**********************************************************************
  const char *get_errstr2(int err)  {
 **********************************************************************/

const char *get_errstr2(int err)  {                                                                                                            // full

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
}								// const char *get_errstr2(int err)

/**********************************************************************

 **********************************************************************/

#ifdef DO_EXPORT

bool export_flag_file(void)  {
#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	if (dirs.size()==0)  {
		fprintf(logstream, "   eff1, dirs size is 0\n");
		fclose(logstream);
		return false;
	}
	else  {
		fprintf(logstream, "   eff1, dirs size = %d\n", dirs.size());
		fclose(logstream);
	}
#endif

	sprintf(gstring, "%s\\trigger.txt", dirs[DIR_DEBUG].c_str());

#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	fprintf(logstream, "   eff2\n");
	fclose(logstream);
#endif

	if (exists(gstring))  {
#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "   eff3\n");
		fclose(logstream);
#endif
		return true;
	}

#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	fprintf(logstream, "   eff4\n");
	fclose(logstream);
#endif

	return false;
}
#endif

/**********************************************************************
  keeps reading until the timeout occurs
 **********************************************************************/

void flush(int _ix, Logger *_logg, Server *_server, DWORD _timeout, BOOL _echo)  {
	unsigned char c[8];
	DWORD start,end;
	int i;
	int n;
	char str[FLUSH_STR_LEN];


	if (_logg) _logg->write(0,0,0,"\nflush:\n");

	start = end = timeGetTime();
	i = 0;

	while(1)  {
		n = _server->rx(_ix, (char *)c, 1);
		if (n==1)  {
			str[i++] = c[0];
			if (_echo) {
				if (_logg) _logg->write(0,0,0,"%c",c[0]);
			}
			if (i==FLUSH_STR_LEN-1)  {
				if (_logg) _logg->write(0,0,0,"i = 255 end =   %lu (%lu)\n",end, _timeout);
				if (_logg) _logg->write(0,0,0,"i = 255 start = %lu (%lu)\n",start,end-start);
				for(int j=0;j<FLUSH_STR_LEN;j++)  {
					if (_logg) _logg->write(0,0,0,"%c",str[j]);
				}
				break;
			}
		}
		end = timeGetTime();
		if ((end-start)>_timeout)  {
			if (_logg) _logg->write(0,0,0,"flush timeout end =   %lu (%lu)\n",end, _timeout);
			if (_logg) _logg->write(0,0,0,"flush timeout start = %lu (%lu)\n",start,end-start);
			break;
		}
	}

	str[i] = 0;

	return;
}							// flush()

/**********************************************************************

 **********************************************************************/

int start_server(void)  {

#ifdef WIN32
	if (server==NULL)  {
		try  {
			server = new Server(broadcast_port, listen_port, ip_discover, udp);
		}
		catch  (int i) {
			//int bp;

			switch(i)  {
				case 100:
					//bp = 0;
					DEL(server);
					return CAN_NOT_CREATE_SERVER;
					break;

				default:
					//bp = 1;
					break;
			}
		}
	}
#endif

	return ALL_OK;
}										// start_server()



/**********************************************************************
  _comport is the real comport number
 **********************************************************************/

//^ check_for_trainers
EnumDeviceType check_for_trainers(int _ix, int _dummy)  {

	int istat;
	char baudstr[16];
	int baudrate;
	Serial *port;
	DEVICE d;
	char comstr[32] = {0};
	DWORD now, diff;

#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	fprintf(logstream, "check_for_trainers(%d)\n", _ix);
	fclose(logstream);
#endif

	if (glog)  {
		glog->write(10,0,1,"check_for_trainers(%d)\n", _ix);
	}

	strncpy(comstr, ix_to_portname[_ix].c_str(), sizeof(comstr)-1);
	baudrate = 2400;
	sprintf(baudstr,"%d", baudrate);

	now = timeGetTime();

	diff = now - last_port_time;

	if ( diff < 500)  {
		//if (bluetooth_delay)  {
		Sleep(diff);
		//}
	}
	last_port_time = timeGetTime();

#ifdef WIN32
	int comport;
	comport = _ix + 1;
	// todo: handle linux version
#ifdef DO_WIFI
	if (comport >= SERVER_SERIAL_PORT_BASE && comport <= SERVER_SERIAL_PORT_BASE)  {
		if (server==NULL)  {
			int status;
			try  {
				status = start_server();
			}
			catch (const int istatus)  {

				//while(1)  {																	// infinite
				//	Sleep(100);
				//}comport

				if (glog)  {
					glog->write(10,0,1,"   server catch(%d)\n", istatus);
				}

				switch(istatus)  {
					case 100:
						//bp = 1;
						break;
					default:
						d.what = DEVICE_OTHER_ERROR;
						//d.port = 0;
						strcpy(d.nv.ver, "n/a");
						strcpy(d.nv.cal, "n/a");
						d.when = timeGetTime();
						devices[_ix] = d;
						return d.what;
				}							// switch
			}								// try
			catch(...)  {
				if (glog)  {
					glog->write(10,0,1,"   catch...\n");
				}
			}
		}								// if (server==NULL)  {

#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "server running, ix = %d\n", _ix);
		fclose(logstream);
#endif

		//---------------------------------------------------------------------------------------
		// server is running in the background, so.. see if there is a client on ix
		// ix = 200, comport = 201
		// the next client that connects will be assigned do comport = ix+1?
		// use ./client/Client.java in eclipse luna to test the server
		//---------------------------------------------------------------------------------------

		assert(server);

		// see if a client has connected to serial port for 2 seconds
		DWORD start = timeGetTime();

		while(1)  {
			if ((timeGetTime()-start) >= 3000)  {
				d.what = DEVICE_DOES_NOT_EXIST;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;
			}

			if (server->is_client_connected(_ix))  {
				break;
			}

			Sleep(50);
		}

#ifdef _DEBUG
		DWORD dt = timeGetTime() - start;
#endif
		//--------------------------------------------------
		// device exists, client has connected
		//--------------------------------------------------

		server->send(_ix, (const char unsigned *)"RacerMate", false);						// no flush
		//server->send(ix);
		istat = server->expect(_ix, "LinkUp", 1000);			// 80 ms measured, returns 0 if CT found

		if (glog)  {
			glog->write(10,0,1,"   expect istat = %d\n", istat);
			if (istat==0)  {
				glog->write(10,0,1,"   CT found\n");
			}
		}

		read_computrainer(_ix, server);							// fills in the global info for this computrainer
		flush(_ix, NULL, server, 500, FALSE);
		return DEVICE_COMPUTRAINER;

	}											// else if (_comport >= SERVER_SERIAL_PORT_BASE && _comport <= SERVER_SERIAL_PORT_BASE)  {
#endif								// #ifdef DO_WIFI

	//------------------------------------
	// check for RS232 computrainer:
	//------------------------------------

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
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;

			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
				d.what = DEVICE_ACCESS_DENIED;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
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
				devices[_ix] = d;
				return d.what;

			default:
				d.what = DEVICE_OTHER_ERROR;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;
		}							// switch
	}								// catch
	catch(...)  {
		if (glog)  {
			glog->write(10,0,1,"   catch...\n");
		}
	}
	if (glog)  {
		//glog->write(10,0,1,"   CT found\n");
	}
#else										// #ifdef WIN32
	// non windows
	if (glog)  {
		glog->write(10,0,1,"   opening %s at %s baud\n", comstr, baudstr);
	}

	port = NULL;

	if (strstr(comstr, "/dev/"))  {
		//port = new Serial((char *)"hbar", comstr, B2400, N8R);  todo

		port = new Serial(comstr, baudstr, NULL);

		if (!port->is_open())  {
			if (glog)  {
				glog->write(10,0,1,"   couldn't open serial port\n");
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
	}
	else if (!strcmp(comstr, "tcp"))  {
		int bp = 2;
	}
	istat = 0;


	if (glog)  {
		glog->write(10,0,1,"   serial port opened, checking for CT\n");
	}


	port->send("RacerMate", 0);						// no flush

//#ifndef DO_WIFI
	#if defined _DEBUG && !defined WIN32
	//sleep(5);
	#endif
	istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found
	//#endif

	port->setShutDownDelay(10);

	if (istat==0)  {
		read_computrainer(port);						// fills in the global info for this computrainer, devices[_ix]
		#ifdef WIN32
		flush(NULL, port, 500, FALSE);
		#endif
		DEL(port);										// hangs here

		devices[_ix].ix = _ix;

		#ifdef _DEBUG
		d = devices[_ix];
		#endif

		return DEVICE_COMPUTRAINER;
	}
	else  {
		DEL(port);										// <<<<<<<<<<<<<<<<<<<<< hangs here
	}

	return DEVICE_OTHER_ERROR;
#endif				// #ifdef WIN32

	//-------------------------------------------------------------
	// check for velotrons
	//-------------------------------------------------------------

	if (glog)  {
		glog->write(10,0,1,"   checking for Velotron\n");
	}

	baudrate = 38400;

	sprintf(baudstr,"%d",baudrate);


#ifdef WIN32
	now = timeGetTime();
	diff = now - last_port_time;
	if ( diff < 500)  {
		Sleep(diff);
	}
	last_port_time = timeGetTime();

	try  {
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

	if (isVelotron(port, 500))  {
		read_velotron(port);						// fills in the global info for this computrainer
		DEL(port);
		return DEVICE_VELOTRON;
	}
	DEL(port);

	// port exists, but not a computrainer or velotron

	d.what = DEVICE_EXISTS;
	strncpy(d.portname, ix_to_portname[_ix].c_str(), sizeof(d.portname)-1);
	strcpy(d.nv.ver, "n/a");
	strcpy(d.nv.cal, "n/a");
	d.when = timeGetTime();
	devices[_ix] = d;

	return d.what;
}											// check_for_trainers()
//$

