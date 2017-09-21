
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
//	char str[32];
#ifdef _DEBUG
	//int bp = 0;
#endif

#ifndef NEWPORT
	int _ix = _comport - 1;
#endif

	unsigned short version;
	static char verstr[32] = {0};
	//char verstr[32] = {0};
	//std::string s;
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
		/*
		if (_comport==SERVER_SERIAL_PORT)  {
			strcpy(devices[_comport-1].nv.ver, "4095");
		}
		*/
		//if (_comport==CLIENT_SERIAL_PORT)  {
		//	strcpy(devices[_comport-1].nv.ver, "4095");
		//}
		return devices[_ix].nv.ver;
	}

	//sprintf(str, "com%d", _comport);


	char str[32];
	//sprintf(str, "COM%d", _ix+1);
	strncpy(str, devices[_ix].portname, sizeof(str)-1);

	ds = new Computrainer(str, rd, &course, &bike, NO_LOGGING, "", -1);

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
		//bp = 1;
	}
#endif

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
		/*
		if (_comport==SERVER_SERIAL_PORT)  {
			strcpy(devices[_comport-1].nv.cal, "2.00 U");
		}
		*/
		//if (_comport==CLIENT_SERIAL_PORT)  {
		//	strcpy(devices[_comport-1].nv.cal, "2.00 U");
		//}
		return devices[_ix].nv.cal;
	}
	//sprintf(str, "com%d", _comport);


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
/*
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
*/


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
	//i = haveDevice(DEVICE_COMPUTRAINER, dev->port);
	i = haveDevice(DEVICE_COMPUTRAINER, dev->ix);
	if (i != -1)  {
		memcpy(&dev->nv, &devices[i].nv, sizeof(NV));
		return ALL_OK;
	}

	memset(&dev->nv, 0, sizeof(dev->nv));

	//sprintf(str, "com%d", dev->port);

	//sprintf(comstr, "COM%d", dev->port);
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
	//int bp;
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
			//bp = 1;
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

#ifdef DO_RTD_SERVER

int read_computrainer(RTDServer *_server)  {
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
	ds = new Computrainer(_server, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return GENERIC_ERROR;
	}

	d.what = DEVICE_COMPUTRAINER;
	//d.port = port->getPortNumber();
	//d.port = SERVER_SERIAL_PORT;
	d.port = 231;


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
}							// void read_computrainer(RTDServer *_server)  {
#endif						// #ifdef DO_RTD_SERVER

#ifdef WIN32
/**********************************************************************
	ONLY called by check_for_trainers()
**********************************************************************/

int read_computrainer(Client *_client)  {
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
	ds = new Computrainer(_client, rd, &course, &bike);

	if (!ds->is_initialized() )  {
		DEL(ds);
		return GENERIC_ERROR;
	}

	d.what = DEVICE_COMPUTRAINER;
	//d.port = port->getPortNumber();
	//d.port = CLIENT_SERIAL_PORT_BASE;


	ds->decoder->set_version(0);							// unsigned short
	ds->decoder->meta.rfdrag = 0;
	ds->decoder->meta.rfmeas = 0;

	have_ver = false;
	have_cal = false;

	start = timeGetTime();

	// read computrainer for 2 seconds to get all packets, such as version, rfdrag, hr sensor flags

#ifdef _DEBUG
	//int cnt=0;
#endif

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
#ifdef _DEBUG
		else  {
			int bp = 1;
		}
#endif

	   ds->updateHardware();
#ifdef _DEBUG
		//cnt++;
#endif

		if ( (timeGetTime() - start) >= 2000)  {
			break;
		}

   }

	DEL(ds);

	d.when = timeGetTime();
	//devices[port->getPortNumber()-1] = d;
	int ix = _client->getPortNumber() -1;
	d.tcp_port = devices[ix].tcp_port;
	strncpy(d.url, devices[ix].url, sizeof(d.url)-1);

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
}							// void read_computrainer(Client *)  {


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

#endif				// #ifdef WIN32

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
	/*
	int comport;
#ifdef WIN32
	comport = _ix + 1;
#else
	comport = _ix + 1;
#endif
	*/
#else
	//xxx
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

	//unlink(_logfilepath);

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
		//if (glog) glog->write(10,0,1,"program directory already set up: %s\n", dirs[DIR_PROGRAM].c_str());
		if (glog)  {
			glog->write(10,0,1,"ssd2\n");
		}
		//goto ssdx;
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
		DIR_DEBUG,
		DIR_HELP
	};
	*/


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
	//dirs.push_back(".");
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
	//dirs.push_back(".");
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
	//dirs.push_back(".");
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
	//dirs.push_back(".");
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
	//dirs.push_back(".");
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
		//dirs.push_back(".");
		dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
	//dirs.push_back(".");
	dirs.push_back(dirs[DIR_PERSONAL].c_str());
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
	//const char *portname = devices[_ix].portname;
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











//////////////////////////////////////////////////////////////////////////////////////////









/**********************************************************************

**********************************************************************/

#ifdef WIN32
//__declspec(dllexport) 
void dump_devices(void)  {
#else
void dump_devices(void)  {
#endif

	int i;
	DEVICE d;
	//int bp = -1;

	printf("\n\n");

	//for (i=0; i<NPORTS; i++)  {
	for (i=0; i<10; i++)  {
		d = devices[i];
		//bp = i;
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
/*
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
		what = GetRacerMateDeviceID2(ix);
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
*/

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
	EnumDeviceType GetRacerMateDeviceID(int ix)  {
**********************************************************************/

EnumDeviceType GetRacerMateDeviceID2(int ix)  {
	EnumDeviceType what;
	int comport;

	if (glog) glog->write(10,0,1,"GetRacerMateDeviceID(%d)\n", ix);

	if (gfile_mode)  {
		if (glog) glog->write(10,0,1,"file_mode\n");
		if (devices[ix].file_mode)  {
			what = DEVICE_RMP;
			devices[ix].what = what;
			//devices[ix].port = ix + 1;
			devices[ix].when = timeGetTime();
			return what;
		}
	}

	if (ix==SIMULATOR_DEVICE)  {					// 255?
		what = DEVICE_SIMULATOR;
		devices[ix].what = what;
		//devices[ix].port = ix + 1;
		devices[ix].when = timeGetTime();
		return what;
	}
	else if (ix==PERF_DEVICE)  {					// 254?
		what = DEVICE_RMP;
		devices[ix].what = what;
		//devices[ix].port = ix + 1;
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
	what = check_for_trainers2(ix, 0);

	if (glog) glog->write(10,0,1,"grmidx\n");
	return what;

}					// EnumDeviceType GetRacerMateDeviceID2(int ix)  {


/**********************************************************************
	_comport starts at 1
	EnumDeviceType check_for_trainers2(int _comport)
**********************************************************************/

EnumDeviceType check_for_trainers2(int _ix, int _dummy)  {
	int istat;
	char baudstr[16];
	int baudrate;
	Serial *port;
	//int i, bp = 0;
	//DEVICE d = {DEVICE_NOT_SCANNED};
	DEVICE d;
	char comstr[32] = {0};

	if (glog)  {
		glog->write(10,0,1,"check_for_trainers2(%d)\n", _ix);
	}

	//-----------------------------
	// check for computrainer:
	//-----------------------------

  	baudrate = 2400;

	sprintf(baudstr,"%d", baudrate);

	
	sprintf(comstr,"COM%d", _ix+1);


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
#else
	port = new Serial(comstr, baudstr, NULL);

        
        if (!port->is_open())  {
            DEL(port);
            d.what = DEVICE_DOES_NOT_EXIST;
				//d.port = 0;
            strcpy(d.nv.ver, "n/a");
            strcpy(d.nv.cal, "n/a");
            d.when = timeGetTime();
            devices[_ix] = d;
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
#ifdef WIN32
		flush(NULL, port, 500, FALSE);
#endif
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
	sprintf(comstr,"COM%d",_ix+1);


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
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
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
	//d.port = _ix+1;
	strcpy(d.nv.ver, "n/a");
	strcpy(d.nv.cal, "n/a");
	d.when = timeGetTime();
	devices[_ix] = d;

	return d.what;
}											// check_for_trainers2()

/**********************************************************************

**********************************************************************/

#ifdef DO_EXPORT

bool export_flag_file(void)  {
//const char * export_flag_file(void)  {
	/*
enum DIRTYPE {
	DIR_PROGRAM,                    // 0
	DIR_PERSONAL,
	DIR_SETTINGS,
	DIR_REPORT_TEMPLATES,
	DIR_REPORTS,
	DIR_COURSES,
	DIR_PERFORMANCES,
	DIR_DEBUG,
	DIR_HELP,                       // 8
	NDIRS                           // 9
};
	*/

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
		//return gstring;
		//return dirs[DIR_DEBUG].c_str();
	}

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "   eff4\n");
		fclose(logstream);
	#endif

	return false;
	//return NULL;
}
#endif


#ifdef DO_RTD_SERVER

/**********************************************************************
	keeps reading until the timeout occurs
**********************************************************************/


void flush(Logger *logg, RTDServer *_server, DWORD timeout, BOOL echo)  {
	//int status;
	unsigned char c[8];
	DWORD start,end;
	int i;
	int n;
	char str[FLUSH_STR_LEN];


	if (logg) logg->write(0,0,0,"\nflush:\n");

//	start = end = GetTickCount();
	start = end = timeGetTime();
   i = 0;

	while(1)  {
//		status = rx(port,&c[0]);
//    if (status)  {

		//status = port->Receive(c,1,&n);

		n = _server->rx((char *)c, 1);

//		if (status==0 && n==1)  {
		if (n==1)  {
      	str[i++] = c[0];
			if (echo) {
				if (logg) logg->write(0,0,0,"%c",c[0]);
			}
         if (i==FLUSH_STR_LEN-1)  {
	      	if (logg) logg->write(0,0,0,"i = 255 end =   %lu (%lu)\n",end,timeout);
   	   	if (logg) logg->write(0,0,0,"i = 255 start = %lu (%lu)\n",start,end-start);
            for(int j=0;j<FLUSH_STR_LEN;j++)  {
            	if (logg) logg->write(0,0,0,"%c",str[j]);
            }
         	break;
         }
		}
//      end = GetTickCount();
      end = timeGetTime();
      if ((end-start)>timeout)  {
      	if (logg) logg->write(0,0,0,"flush timeout end =   %lu (%lu)\n",end,timeout);
      	if (logg) logg->write(0,0,0,"flush timeout start = %lu (%lu)\n",start,end-start);
	     	break;
		}
	}

   str[i] = 0;

	return;
}							// flush()
#endif							// #ifdef DO_RTD_SERVER


/**********************************************************************
	keeps reading until the timeout occurs
**********************************************************************/

void flush(int _ix, Logger *_logg, Server *_server, DWORD _timeout, BOOL _echo)  {
	//int status;
	unsigned char c[8];
	DWORD start,end;
	int i;
	int n;
	char str[FLUSH_STR_LEN];


	if (_logg) _logg->write(0,0,0,"\nflush:\n");

//	start = end = GetTickCount();
	start = end = timeGetTime();
   i = 0;

	while(1)  {
//		status = rx(port,&c[0]);
//    if (status)  {

		//status = port->Receive(c,1,&n);

		n = _server->rx(_ix, (char *)c, 1);

//		if (status==0 && n==1)  {
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
//      end = GetTickCount();
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
	keeps reading until the timeout occurs
**********************************************************************/

void flush(Logger *logg, Client *_client, DWORD timeout, BOOL echo)  {
	_client->flush(timeout);
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

#if 1
	/**********************************************************************
		_comport is the real comport number
	**********************************************************************/

	//^ check_for_trainers

	EnumDeviceType check_for_trainers(int _ix, int _dummy)  {
		//int ix;

#ifdef DO_WIFI
		//Client *client=NULL;
#endif
		int istat;
		char baudstr[16];
		int baudrate;
		Serial *port;
		//RTDServer *server;
		//int i, bp = 0;
		//int 
		//DEVICE d = {DEVICE_NOT_SCANNED};
		DEVICE d;
		char comstr[32] = {0};
		DWORD now, diff;
		//int bp;

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "check_for_trainers(%d)\n", _ix);
			fclose(logstream);
		#endif

		if (glog)  {
			glog->write(10,0,1,"check_for_trainers(%d)\n", _ix);
		}

		//ix = _comport -1;
#ifdef DO_WIFI
		//assert(server==NULL);				// comment out start_server() further upstream, for short_circuit
#endif

		//sprintf(comstr,"COM%d", _ix+1);
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

		//-----------------------------
		// check for WIFI computrainer:
		//-----------------------------

		/*
		else if (_comport == SERVER_SERIAL_PORT)  {
			int bp = 1;
			server = new Server(9072);

			if (server->get_client_socket() == INVALID_SOCKET)  {
				DEL(server);
				//return DEVICE_DOES_NOT_EXIST;
				d.what = DEVICE_DOES_NOT_EXIST;
				d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_comport-1] = d;
				return d.what;
			}

			server->tx("RacerMate", 0);						// no flush

			istat = server->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found
			if (glog)  {
				glog->write(10,0,1,"   expect istat = %d\n", istat);
				if (istat==0)  {
					glog->write(10,0,1,"   WIFI CT found\n");
				}
			}

			if (istat==0)  {
				read_computrainer(server);						// fills in the global info for this computrainer
				flush(NULL, server, 500, FALSE);
				DEL(server);
				return DEVICE_COMPUTRAINER;
			}
			else  {
				DEL(server);
			}
		}											// if (_comport==SERVER_PORT)
		*/

		//port->setShutDownDelay(10);

		
#ifdef WIN32
	//const char *ccptr = devices[_ix].portname;							// ""
	int comport;
	comport = _ix + 1;
	// todo: handle linux version
	#ifdef DO_WIFI

		if (comport >= CLIENT_SERIAL_PORT_BASE && comport <= CLIENT_SERIAL_PORT_BASE)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "cft1\n", comport);
				fclose(logstream);
			#endif

			if (devices[_ix].url[0] == 0 || devices[_ix].tcp_port == -1)  {
				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "cft2\n", comport);
					if (devices[_ix].url[0] == 0)  {
						fprintf(logstream, "url is NULL\n");
					}
					else  {
						fprintf(logstream, "url = %s\n", devices[_ix].url);
					}

					if (devices[_ix].tcp_port == -1)  {
						fprintf(logstream, "tcp port = -1\n");
					}
					else  {
						fprintf(logstream, "tcp port = %d\n", devices[_ix].tcp_port);
					}
					fclose(logstream);
				#endif

				d.what = DEVICE_NO_URL_AND_OR_TCP_PORT;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;
			}

#if 0
			try  {
				//client = new Client("127.0.0.1", 9072);
				//client = new Client("miney2.mselectron.net", 9072);
				client = new Client(devices[_ix].url, devices[_ix].tcp_port);
			}
			catch (const int istatus) {

//while(1)  {																	// infinite
//	Sleep(100);
//}


				if (glog)  {
					glog->write(10,0,1,"   catch(%d)\n", istatus);
				}

				switch(istatus)  {
					case 1:						// expect failed
						//bp = 1;
						break;

					case 2:
					case 3:
					case 4:			// connection refused
					case 5:			// WSAEADDRNOTAVAIL
					case 6:			// default: from connect() status
					case 7:			// connected to a server, but it did not respond with "LinkUp"
						d.what = DEVICE_DOES_NOT_EXIST;
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
						//return d.what;
						return devices[_ix].what;
				}							// switch
			}								// try
			catch(...)  {
				if (glog)  {
					glog->write(10,0,1,"   catch...\n");
				}
			}

			if (client!=NULL)  {
				if (client->get_socket() == INVALID_SOCKET)  {
					DEL(client);
					//return DEVICE_DOES_NOT_EXIST;
					d.what = DEVICE_DOES_NOT_EXIST;
					//d.port = 0;
					strcpy(d.nv.ver, "n/a");
					strcpy(d.nv.cal, "n/a");
					d.when = timeGetTime();
					devices[_ix] = d;
					return d.what;
				}
			}
			else  {
				//return DEVICE_DOES_NOT_EXIST;
				d.what = DEVICE_DOES_NOT_EXIST;
				//d.port = 0;
				strcpy(d.nv.ver, "n/a");
				strcpy(d.nv.cal, "n/a");
				d.when = timeGetTime();
				devices[_ix] = d;
				return d.what;
			}

			if (glog)  {
				glog->write(10,0,1,"   connected to CT server\n");
			}

			read_computrainer(client);						// fills in the global info for this computrainer
			flush(NULL, client, 500, FALSE);
			DEL(client);										// LOCAL client object
			return DEVICE_COMPUTRAINER;

			/*
			if (istat==0)  {
				read_computrainer(server);						// fills in the global info for this computrainer
				flush(NULL, server, 500, FALSE);
				DEL(server);
				return DEVICE_COMPUTRAINER;
			}
			else  {
				DEL(server);
			}
			*/

		}													// if (_comport >= CLIENT_SERIAL_PORT_BASE && _comport <= CLIENT_SERIAL_PORT_BASE)  {
#endif

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

			/*
			d.what = DEVICE_OPEN_ERROR;
			d.port = _comport;
			strcpy(d.nv.ver, "n/a");
			strcpy(d.nv.cal, "n/a");
			d.when = timeGetTime();
			devices[ix] = d;
			*/

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
			//assert(client==NULL);

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

			//bp = 1;
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
			glog->write(10,0,1,"   expect istat = %d\n", istat);
			if (istat==0)  {
				glog->write(10,0,1,"   CT found\n");
			}
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
	#endif				// #ifdef WIN32


		if (glog)  {
			glog->write(10,0,1,"   serial port opened, checking for CT\n");
		}


		port->send("RacerMate", 0);						// no flush

		/*
		comport = _ix + 1;
		comport = portname_to_ix[comstr];
		*/

#ifdef DO_WIFI
		/*
		if (comport == CLIENT_SERIAL_PORT_BASE)  {
			istat = port->expect("LinkUp", 10000);			// 80 ms measured, returns 0 if CT found
		}
		else  {
			istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found
		}
		*/
#else
		#if defined _DEBUG && !defined WIN32
			//sleep(5);
		#endif
		istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found
#endif



		port->setShutDownDelay(10);

		if (istat==0)  {
			read_computrainer(port);						// fills in the global info for this computrainer, devices[_ix]
#ifdef WIN32
			flush(NULL, port, 500, FALSE);
#endif
			DEL(port);										// hangs here

			/*
			//sprintf(comstr,"COM%d", _comport);
			d.what = DEVICE_COMPUTRAINER;
			//d.port = comport;
			strncpy(d.portname, comstr, sizeof(d.portname)-1);
			strcpy(d.nv.ver, "n/a");
			strcpy(d.nv.cal, "n/a");
			d.when = timeGetTime();
			d.ix = _ix;
			devices[_ix] = d;
			return d.what;
			*/
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

		//-------------------------------------------------------------
		// check for velotrons
		//-------------------------------------------------------------

		if (glog)  {
			glog->write(10,0,1,"   checking for Velotron\n");
		}

  		baudrate = 38400;

		sprintf(baudstr,"%d",baudrate);
		//sprintf(comstr,"COM%d",comport);


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
					//d.port = 0;
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
		//d.port = comport;
		strncpy(d.portname, ix_to_portname[_ix].c_str(), sizeof(d.portname)-1);
		strcpy(d.nv.ver, "n/a");
		strcpy(d.nv.cal, "n/a");
		d.when = timeGetTime();
		devices[_ix] = d;

		return d.what;
	}											// check_for_trainers()
	//$
#endif

