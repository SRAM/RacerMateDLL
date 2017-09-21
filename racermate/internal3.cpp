
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
#include <assert.h>
#include <shlobj.h>
#endif

#include <comglobs.h>
#include <globals.h>
#include <utils.h>
#include <fatalerror.h>
#include <nvram.h>
#include <computrainer.h>
#include <device.h>

//#include "dll_common.h"
#include "dll_globals.h"
#include "errors.h"

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

const char *get_computrainer_firmware(int _ix)  {								// 0 - 255
    dataSource *ds = NULL;
    DWORD start;
    int status;
    unsigned short version;
    static char verstr[32] = {0};
    bool b;
    RIDER_DATA rd;

    Course course;
    Bike bike;


    b = haveDevice(DEVICE_COMPUTRAINER, _ix);
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

const char *get_velotron_firmware(int _ix)  {
    static char verstr[32] = {0};
    unsigned char version;
    unsigned char build;
    bool b;
    char comstr[32] = {0};

    b = haveDevice(DEVICE_VELOTRON, _ix);
    if (b)  {
        return devices[_ix].nv.ver;
    }


    sprintf(comstr, "com%d", _ix+1);

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


    bb = haveDevice(DEVICE_VELOTRON, port->getPortNumber()-1);
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

int read_computrainer(int _ix, Serial *port)  {
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

    //d.what = DEVICE_COMPUTRAINER;
    //d.port = port->getPortNumber();
    //d.port = CLIENT_SERIAL_PORT_BASE;


    const char *cptr = port->get_port_name();

    //strncpy(d.portname, cptr, sizeof(d.portname)-1);

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
	//			devices[_ix].portname
	//			devices[_ix].commtype
	//			devices[_ix].what

    while(1)  {
        //status = ds->getNextRecord(0);
        status = ds->getNextRecord();
        if (status==0)  {
            version = ds->decoder->get_version();
            if (version != 0)  {
                sprintf(devices[_ix].nv.ver, "%hd", version);
                have_ver = true;
            }

            if (ds->decoder->meta.rfdrag != 0)  {
                ftemp = (float) (ds->decoder->meta.rfdrag & 0x07ff);
                sprintf(devices[_ix].nv.cal, "%.2f", ftemp / 256.0f);
                if ((ds->decoder->meta.rfmeas & 0x0008) == 0x0008)  {
                    strcat(devices[_ix].nv.cal, " C");
                }
                else  {
                    strcat(devices[_ix].nv.cal, " U");
                }
                have_cal = true;
            }

            if (have_ver && have_cal)  {
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
        if ( dt >= 2000)  {
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

int read_server_computrainer(int _ix)   {
	unsigned long start;
	int ix;


	if (server == NULL) {
		return GENERIC_ERROR;
	}

	ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	Sleep(2000);

	DEVICE d;

	sprintf(d.nv.ver, "%d", server->get_fw(ix));
	d.when = timeGetTime();

	devices[_ix] = d;								// UDP_SERVER_SERIAL_PORT_BASE

#if 0
    dataSource *ds = NULL;
    DWORD start;
    int status;
    //int bp = 0;
    unsigned short version;
    float ftemp;
    bool have_ver;
    bool have_cal;
    //DEVICE d = {DEVICE_NOT_SCANNED};
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

#endif			//#if 0

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
    int ix;
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

    ix = port->getPortNumber()-1;
    d.ix = ix;
    strncpy(d.portname, port->get_port_name(), sizeof(d.portname)-1);


    devices[ix] = d;

    return ALL_OK;
}					// int read_velotron(Serial *port)  {


/**********************************************************************
  hd1
  hd4
  hd6, ul = 93,366

 **********************************************************************/

bool haveDevice(EnumDeviceType what, int _ix)  {
    DWORD now, dt;

    if (glog) glog->write(10,0,1,"hd1\n");
    //const char *ccptr = ix_to_port_info[_ix].name.c_str();
//#ifndef WIN32
	 const char *ccptr = NULL;
#ifdef DO_PI
	 ccptr = ix_to_port_info[_ix].name.c_str();
#else
	 ccptr = devices[_ix].portname;
#endif

    if (devices[_ix].ev)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "      hd: ev is running, must have it?\n");
			FCLOSE(logstream);
		#endif

        if (devices[_ix].what==what)  {
				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "      hd: devices[_ix].what = %d, what = %d\n", devices[_ix].what, what);
					fprintf(logstream, "      hd: devices[_ix].portname = %s\n", devices[_ix].portname);
					fprintf(logstream, "      hd: returning true\n");
					FCLOSE(logstream);
				#endif
				if (!strcmp(devices[_ix].portname, ccptr))  {					// if thread is running, we have it
				}
            return true;
		  }
        else  {
            if (glog) glog->write(10,0,1,"hd2\n");
  				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "      hd: devices[_ix].what = %d, what = %d\n", devices[_ix].what, what);
					fprintf(logstream, "      hd: ev running, but whats are different. Deleting ev and returning false\n");
					FCLOSE(logstream);
				#endif
				DEL(devices[_ix].ev);
            if (glog) glog->write(10,0,1,"hd2b\n");
            return false;
        }

        //if (devices[_ix].what==what && devices[_ix].port==comport)  {					// if thread is running, we have it
#ifndef WIN32
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
#endif

    }

    if (devices[_ix].what!=what)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "      hd: devices[_ix].what = %d, what = %d\n", devices[_ix].what, what);
				fprintf(logstream, "      hd: ev not running, whats are different, returning false\n");
				FCLOSE(logstream);
			#endif
        if (glog) glog->write(10,0,1,"hd3: %s   %s\n", devstrs[what], devstrs[devices[_ix].what]);
        return false;
    }

    if (glog) glog->write(10,0,1,"hd4\n");

    if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON || what==DEVICE_SIMULATOR || what==DEVICE_RMP)  {

        //if (devices[_ix].port==_ix+1)  {
#ifdef DO_PI
	    if (!strcmp(devices[_ix].portname, ccptr))  {

            now = timeGetTime();
            dt = now - devices[_ix].when;
            if (glog) glog->write(10,0,1,"hd4b\n");

            if ( dt < DEVICE_TIMEOUT_MS )   {							// 10,000 ms
                if (glog) glog->write(10,0,1,"hd5, dt = %ld\n", dt);
                return true;
            }
        }
#else
	    if (!strcmp(devices[_ix].portname, ccptr))  {
            now = timeGetTime();
            dt = now - devices[_ix].when;
            if (glog) glog->write(10,0,1,"hd4b\n");

            if ( dt < DEVICE_TIMEOUT_MS )   {							// 10,000 ms
                if (glog) glog->write(10,0,1,"hd5, dt = %ld\n", dt);
                return true;
            }
        }
#endif
    }
    else if (what==DEVICE_NOT_SCANNED)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "      hd: device not scanned, returning false\n");
				FCLOSE(logstream);
			#endif
        if (glog) glog->write(10,0,1,"hd5B\n");
        return false;
    }
    else  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "      hd: ERROR? device = %s, returning false\n", devstrs[what]);
				FCLOSE(logstream);
			#endif

        unsigned long ul;
        ul = DEVICE_TIMEOUT_MS;
        ul = timeGetTime();
        dt = timeGetTime() - devices[_ix].when;

        if ( dt < DEVICE_TIMEOUT_MS)  {
            if (glog) glog->write(10,0,1,"hd6, ul = %ld\n", ul);
            return true;
        }
    }

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "      hdx: ERROR? device = %s, returning false\n", devstrs[what]);
		FCLOSE(logstream);
	#endif

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
    fprintf(logstream, "   internal:: setup_standard_dirs(%s)\n", _logfilepath?_logfilepath:"no logfilepath");
    fprintf(logstream, "      dirs size = %ld\n", dirs.size());
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

bool iscal(EnumDeviceType what, int _ix)  {
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
        fprintf(logstream, "   %s exists, returning true\n", gstring);
        fclose(logstream);
#endif
        return true;
    }

#ifdef LOGDLL
    logstream = fopen(DLLFILE, "a+t");
    fprintf(logstream, "   %s does not exist, returning false\n", gstring);
    fclose(logstream);
#endif

    return false;
}
#endif

/**********************************************************************
  keeps reading until the timeout occurs
 **********************************************************************/
#if 0
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
        n = _server->rx(_ix, c, 1);
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

#else
void flush(int _ix, Logger *_logg, RACERMATE::Ctsrv *_server, DWORD _timeout, BOOL _echo)  {
    unsigned char c[8];
    DWORD start,end;
    int i;
    int n;
    char str[FLUSH_STR_LEN];


    if (_logg) _logg->write(0,0,0,"\nflush:\n");

    start = end = timeGetTime();
    i = 0;

    while(1)  {
        n = _server->rx(_ix, c, 1);
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
#endif




/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_trainers(int _ix, int _dummy)  {
   EnumDeviceType what = DEVICE_OTHER_ERROR;

		// known here
		//		rs232, windows
		//			devices[_ix].ix
		//			devices[_ix].portname
		//			devices[_ix].commtype
		//
		//		win32 trainer is server
		//			devices[_ix].ix
		//			devices[_ix].commtype
		//			devices[_ix].tcp_port
		//			devices[_ix].url
		//			devices[_ix].portname

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "      check_for_trainers(%d)\n", _ix);
		fclose(logstream);
	#endif

//#ifndef WIN32
#ifdef DO_PI
	 int n = ix_to_port_info.size();

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "         n = %d");
		if (n==0)  {
			fprintf(logstream, ", returning error\n");
		}
		else  {
			fprintf(logstream, ", name = %s\n", ix_to_port_info[_ix].name.c_str() );
			fprintf(logstream, ", portnum = %d\n", ix_to_port_info[_ix].portnum );
			fprintf(logstream, ", type = %d\n", ix_to_port_info[_ix].type );
		}

		fclose(logstream);
	#endif

	if (n==0)  {
		return what;
	}

	/*
	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		for(int i=0;i<ix_to_port_info.size(); i++)  {
			fprintf(logstream, "%s   %d   %d\n", ix_to_port_info[
		fclose(logstream);
	#endif
	*/

   PORT_INFO pi = ix_to_port_info[_ix];
/*
enum EnumDeviceType {
	DEVICE_NOT_SCANNED,					// unknown, not scanned
	DEVICE_DOES_NOT_EXIST,				// serial port does not exist
	DEVICE_EXISTS,							// exists, openable, but no RM device on it
	DEVICE_COMPUTRAINER,
	DEVICE_VELOTRON,
	DEVICE_SIMULATOR,
	DEVICE_RMP,
	//DEVICE_WIFI,
	DEVICE_ACCESS_DENIED,				// port present but can't open it because something else has it open
	DEVICE_OPEN_ERROR,					// port present, unable to open port
	DEVICE_NO_URL_AND_OR_TCP_PORT,
	DEVICE_SERVER_NOT_RUNNING,
	DEVICE_OTHER_ERROR					// prt present, error, none of the above
};
*/
	type = pi.type;
#endif							// #ifdef DO_PI

	// all we know so far is _ix. we have to discover what devices[_ix] is

#if 0
	type = devices[_ix].commtype;

	 switch(type)  {
		case WIN_RS232:
		case UNIX_RS232:
		case TRAINER_IS_SERVER:
		case TRAINER_IS_CLIENT:
			break;
		default:
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "         error, type = %d\n", type);
			fclose(logstream);
		#endif
			return what;
	 }
#endif

#ifdef WIN32
	//#ifndef DO_PI
	 //if (devices[_ix].ix >= CLIENT_SERIAL_PORT_BASE && devices[_ix].ix <= CLIENT_SERIAL_PORT_BASE+15)  {
	//	 sprintf(devices[_ix].portname, "COM%d", devices[_ix].ix+1);
	 //}
	//#endif

	// in windows, all we know now is _ix and devices[_ix].portname. we have to discover what devices[_ix] is
#endif


#if 0
#ifdef LOGDLL
    logstream = fopen(DLLFILE, "a+t");
#ifdef DO_PI
	 fprintf(logstream, "         pi.name = %s, pi.portnum = %d, ", pi.name, pi.portnum);
#else
	 fprintf(logstream, "         name = %s, ", devices[_ix].portname);
#endif

	 switch(type)  {
		case WIN_RS232:
			fprintf(logstream, "type = WIN_RS232\n");
			break;
		case UNIX_RS232:
			fprintf(logstream, "type = UNIX_RS232\n");
			break;
		case TRAINER_IS_SERVER:
			fprintf(logstream, "type = TRAINER_IS_SERVER\n");
			break;
		case TRAINER_IS_CLIENT:
			fprintf(logstream, "type = TRAINER_IS_CLIENT\n");
			break;
		default:
			fprintf(logstream, "type = ???????????\n");
			break;
	 }

    fclose(logstream);
#endif					// logdll
#endif					// #if 0

    if (glog)  {
        glog->write(10,0,1,"   check_for_trainers(%d)\n", _ix);
    }

	// known here
	//		rs232, windows
	//			devices[_ix].ix
	//			devices[_ix].portname
	//			devices[_ix].commtype
	//
	//		win32 trainer is server
	//			devices[_ix].ix
	//			devices[_ix].commtype
	//			devices[_ix].tcp_port
	//			devices[_ix].url
	//			devices[_ix].portname

#ifdef WIN32
	//int bp = 0;
	// in windows, all we know now is devices[_ix].ix, devices[_ix].portname, and devices[_ix].commtype. we have to discover if there is something there...

	if (devices[_ix].commtype == WIN_RS232)  {
		what = check_for_windows_rs232_computrainer(_ix);
		if (what != DEVICE_COMPUTRAINER)  {
			what = check_for_windows_rs232_velotron(_ix);
		}
	}
	else if(devices[_ix].commtype == TRAINER_IS_SERVER)  {
		what = check_for_windows_client_computrainer(_ix);						// try to connect to the computrainer handlebar server
		if (what != DEVICE_COMPUTRAINER)  {
		}
	}
	else if(devices[_ix].commtype == TRAINER_IS_CLIENT)  {
		what = check_for_windows_server_computrainer(_ix);						// try to connect to the computrainer handlebar client
		//throw(fatalError(__FILE__, __LINE__));
	}
	else  {
		sprintf(gstring, "commtype[%d] = %d", _ix, devices[_ix].commtype);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}
#endif


#if 0
     if (type==WIN_RS232)  {
        what = check_for_windows_rs232_computrainer(_ix);
        if (what != DEVICE_COMPUTRAINER)  {
            what = check_for_windows_rs232_velotron(_ix);
        }
     }
     else if (type==UNIX_RS232)  {
     }
     else if (type==TRAINER_IS_SERVER)  {								// trainer is a server
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "         calling check_for_windows_client_computrainer\n");
			fclose(logstream);
		#endif
        what = check_for_windows_client_computrainer(_ix);						// try to connect to the computrainer handlebar server
        if (what != DEVICE_COMPUTRAINER)  {
            //what = check_for_windows_client_velotron(_ix);						// try to connect to the computrainer handlebar server
        }
     }
     else if (type==TRAINER_IS_CLIENT)  {								// trainer is a client
     }
#else
	#ifndef  WIN32
    if (pi.type == TRAINER_IS_SERVER)  {								// trainer is a server
        what = check_for_linux_computrainer_server(_ix);						// try to connect to the computrainer handlebar server
        //what = check_for_windows_client_computrainer(_ix);			// try to connect to the computrainer handlebar server
    }
    else  {
        assert (pi.type==UNIX_RS232);
        what = check_for_unix_rs232_computrainer(_ix);
    }
	#endif
#endif

	// known here
	//		rs232, windows
	//			devices[_ix].ix
	//			devices[_ix].portname
	//			devices[_ix].commtype
	//			devices[_ix].what
	//			devices[_ix].nv.ver
	//			devices[_ix].nv.cal
	//-----------------------------------
	//		win32 trainer is server
	//			devices[_ix].ix
	//			devices[_ix].commtype
	//			devices[_ix].tcp_port
	//			devices[_ix].url
	//			devices[_ix].portname
	//			devices[_ix].what
	//			devices[_ix].nv.ver
	//			devices[_ix].nv.cal
	//			devices[_ix].when

    return what;

//    if (what==DEVICE_COMPUTRAINER)  {
//        return what;
//    }

//	what = check_for_windows_rs232_velotron(_ix);
//	return what;
//	return DEVICE_DOES_NOT_EXIST;
}											// check_for_trainers()

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_computrainer_client(int _ix, int _debug_level)  {

#if 0
    assert(server);
    /*
    int n = server->clients.size();
    assert(_ix < n);
    server->clients[_ix].x;
    */

    return DEVICE_COMPUTRAINER;
#else
    DEVICE d;
    //int istat;

    if (server==NULL)  {
		devices[_ix].ix = _ix;
		devices[_ix].commtype = TRAINER_IS_CLIENT;

        int status;
        try  {
            status = start_server(_debug_level);
            status = status;			// omit warning
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

	DWORD timeout;

    // see if a client has connected to serial port for 2 seconds
    DWORD start = timeGetTime();

	if (devices[_ix].commtype == TRAINER_IS_CLIENT)  {
		timeout = 10000;
	}
	else  {
		timeout = 3000;
	}

	while(1)  {
        if ((timeGetTime()-start) >= timeout)  {
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
        dt = dt;
    #endif

    //--------------------------------------------------
    // device exists, client has connected
    //--------------------------------------------------

    int istat;

    server->send(_ix, (const char unsigned *)"RacerMate", false);						// no flush
    //server->send(ix);
    istat = server->expect(_ix, "LinkUp", 1000);			// 80 ms measured, returns 0 if CT found

    if (glog)  {
        glog->write(10,0,1,"   expect istat = %d\n", istat);
        if (istat==0)  {
            glog->write(10,0,1,"   CT found\n");
        }
    }

    read_server_computrainer(_ix);							// fills in the global info for this computrainer
    flush(_ix, NULL, server, 500, FALSE);
    return DEVICE_COMPUTRAINER;
#endif				// #if 0

    //return DEVICE_DOES_NOT_EXIST;
}												// check_for_computrainer_client


#ifdef WIN32

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_windows_rs232_computrainer(int _ix)  {
    int baudrate;
    DWORD now, diff;
    char baudstr[16];
    Serial *port;
    //char comstr[32] = {0};
    DEVICE d;
    int comport;
    int istat;


#ifdef DO_PI
    PORT_INFO pi = ix_to_port_info[_ix];
    assert (pi.type==WIN_RS232);
    assert (pi.portnum==_ix+1);

	 strncpy(comstr, pi.name.c_str(), sizeof(comstr)-1);
#else
	 //strncpy(comstr, devices[_ix].portname, sizeof(comstr)-1);
#endif

    comport = _ix + 1;

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

    //--------------------------------------------
    // check for RS232 computrainer in windows
    //--------------------------------------------

    try  {
        port = new Serial(bluetooth_delay, devices[_ix].portname, baudstr, FALSE, NULL);
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

    // opened serial port

    if (glog)  {
        glog->write(10,0,1,"   serial port opened, checking for CT\n");
    }

    port->send("RacerMate", 0);						// no flush

    istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found

	 /*
while(1)  {
	Sleep(10);
}
	*/


    port->setShutDownDelay(10);

    if (istat==0)  {
		 devices[_ix].what = DEVICE_COMPUTRAINER;

		// known here
		//		rs232, windows
		//			devices[_ix].ix
		//			devices[_ix].portname
		//			devices[_ix].commtype
		//			devices[_ix].what

        read_computrainer(_ix, port);						// fills in the global info for this computrainer, devices[_ix]
        DEL(port);										// hangs here
        //devices[_ix].ix = _ix;

		// known here
		//		rs232, windows
		//			devices[_ix].ix
		//			devices[_ix].portname
		//			devices[_ix].commtype
		//			devices[_ix].what
		//			devices[_ix].nv.ver
		//			devices[_ix].nv.cal

        return DEVICE_COMPUTRAINER;
    }
    else  {
        DEL(port);										// <<<<<<<<<<<<<<<<<<<<< hangs here
    }

    return DEVICE_OTHER_ERROR;


    DEL(port);
    return DEVICE_OTHER_ERROR;
}								// check_for_windows_rs232_computrainer(int _ix)
#endif				// #ifdef WIN32


#ifndef WIN32

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_unix_rs232_computrainer(int _ix)  {
    //int baudrate;
    //DWORD now, diff;
    char baudstr[16] = {"2400"};
    Serial *port;
    char comstr[32] = {0};
    DEVICE d;
    int istat;

    PORT_INFO pi = ix_to_port_info[_ix];
    assert (pi.type==UNIX_RS232);

    strncpy(comstr, pi.name.c_str(), sizeof(comstr)-1);

    if (glog)  {
        glog->write(10,0,1,"   opening %s at %s baud\n", comstr, baudstr);
    }

    port = new Serial(comstr, baudstr, NULL);				// calls constructor 1

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


    if (glog)  {
        glog->write(10,0,1,"   serial port opened, checking for CT\n");
    }

    port->send("RacerMate", 0);						// no flush

    istat = port->expect("LinkUp", 500);			// 80 ms measured, returns 0 if CT found

    port->setShutDownDelay(10);

    if (istat==0)  {
        read_computrainer(port);						// fills in the global info for this computrainer, devices[_ix]
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
}								// check_for_unix_rs232_computrainer()

#endif								// #ifndef WIN32

/**********************************************************************

**********************************************************************/

EnumDeviceType check_for_windows_rs232_velotron(int _ix)  {
    int baudrate;
    char baudstr[16];
    Serial *port;
    //char comstr[32] = {0};
    DEVICE d;

#ifdef DO_PI
    PORT_INFO pi = ix_to_port_info[_ix];
    assert (pi.type==WIN_RS232);
    assert (pi.portnum==_ix+1);

    strncpy(comstr, pi.name.c_str(), sizeof(comstr)-1);
#endif

    //-------------------------------------------------------------
    // check for velotrons
    //-------------------------------------------------------------

    if (glog)  {
        glog->write(10,0,1,"   checking for Velotron\n");
    }

    baudrate = 38400;

    sprintf(baudstr,"%d",baudrate);


#ifdef WIN32
    DWORD now, diff;
    now = timeGetTime();
    diff = now - last_port_time;
    if ( diff < 500)  {
        Sleep(diff);
    }
    last_port_time = timeGetTime();

    try  {
#ifdef _DEBUG
        port = new Serial(true, devices[_ix].portname, baudstr, FALSE, NULL);							// showing errors
#else
        port = new Serial(true, devices[_ix].portname, baudstr, FALSE, NULL);
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

#ifdef DO_PI
	 strncpy(d.portname, ix_to_port_info[_ix].name.c_str(), sizeof(d.portname)-1);
#endif

    strcpy(d.nv.ver, "n/a");
    strcpy(d.nv.cal, "n/a");
    d.when = timeGetTime();
    devices[_ix] = d;

    return d.what;
}											// check_for_windows_rs232_velotron()



#ifdef WIN32
/**********************************************************************
    check for WIFI computrainer
    the trainer is the server
**********************************************************************/

EnumDeviceType check_for_windows_client_computrainer(int _ix)  {
    Client *client=NULL;
    DEVICE d;

    int comport = _ix + 1;

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "            check_for_windows_client_computrainer(%d)\n", _ix);
		fclose(logstream);
	#endif

    if (comport < CLIENT_SERIAL_PORT_BASE || comport > CLIENT_SERIAL_PORT_BASE)  {
        return DEVICE_OTHER_ERROR;
    }

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

#ifdef LOGDLL
    logstream = fopen(DLLFILE, "a+t");
    fprintf(logstream, "            cft1\n");
    fclose(logstream);
#endif

#if 1
    if (devices[_ix].url[0] == 0 || devices[_ix].tcp_port == -1)  {
#ifdef LOGDLL
        logstream = fopen(DLLFILE, "a+t");
        fprintf(logstream, "            cft2\n");
        if (devices[_ix].url[0] == 0)  {
            fprintf(logstream, "url is NULL\n");
        }
        else  {
            fprintf(logstream, "            url = %s\n", devices[_ix].url);
        }

        if (devices[_ix].tcp_port == -1)  {
            fprintf(logstream, "            tcp port = -1\n");
        }
        else  {
            fprintf(logstream, "            tcp port = %d\n", devices[_ix].tcp_port);
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
#endif


#ifdef LOGDLL
    logstream = fopen(DLLFILE, "a+t");
    fprintf(logstream, "            creating client, url = %s, port = %d\n", devices[_ix].url, devices[_ix].tcp_port);
    fclose(logstream);
#endif

    try  {
        //client = new Client("127.0.0.1", 9072);
        //client = new Client("miney2.mselectron.net", 9072);
        client = new Client(devices[_ix].url, devices[_ix].tcp_port, 1);
    }
    catch (const int istatus) {
		 DEL(client);

        //while(1)  {																	// infinite
        //	Sleep(100);
        //}

#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "            istatus = %d\n", istatus);
			fclose(logstream);
#endif

        if (glog)  {
            glog->write(10,0,1,"   catch(%d)\n", istatus);
        }

        switch(istatus)  {
            case 1:						// expect failed
                //bp = 1;
#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "%s", "            expect failed = %d\n");
					fclose(logstream);
#endif
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
#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "            created client\n");
			fclose(logstream);
#endif
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

	 devices[_ix].what = DEVICE_COMPUTRAINER;

	// known here
	//		win32 trainer is server
	//			devices[_ix].ix
	//			devices[_ix].commtype
	//			devices[_ix].tcp_port
	//			devices[_ix].url
	//			devices[_ix].portname
	//			devices[_ix].what

     // we've connected to the server and received linkup message
#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "            calling read_computrainer()\n");
			fclose(logstream);
#endif

    int status = read_computrainer(_ix, client);						// fills in the global info for this computrainer ( devices[_ix] )
	 assert(status==ALL_OK);

    flush(NULL, client, 500, FALSE);
    DEL(client);										// LOCAL client object

#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "            deleted client\n");
			fclose(logstream);
#endif
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

}						// check_for_windows_client_computrainer()



#endif			// #ifdef WIN32

/**********************************************************************
    keeps reading until the timeout occurs
**********************************************************************/

void flush(Logger *logg, Client *_client, DWORD timeout, BOOL echo)  {
    _client->flush(timeout);
    return;
}							// flush()


/**********************************************************************
    ONLY called by check_for_trainers()
**********************************************************************/

int read_computrainer(int _ix, Client *_client)  {
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
    logstream = fopen(DLLFILE, "a+t");
    fprintf(logstream, "internal read_computrainer()\n");
    fclose(logstream);
    */
#endif

    ds = new Computrainer(_client, rd, &course, &bike);

    if (!ds->is_initialized() )  {
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
	//			devices[_ix].portname
	//			devices[_ix].what

   while(1)  {
      //status = ds->getNextRecord(0);
      status = ds->getNextRecord();
      if (status==0)  {
            version = ds->decoder->get_version();
         if (version != 0)  {
             sprintf(devices[_ix].nv.ver, "%hd", version);
             have_ver = true;
         }

         if (ds->decoder->meta.rfdrag != 0)  {
             ftemp = (float) (ds->decoder->meta.rfdrag & 0x07ff);
             sprintf(devices[_ix].nv.cal, "%.2f", ftemp / 256.0f);
             if ((ds->decoder->meta.rfmeas & 0x0008) == 0x0008)  {
                 strcat(devices[_ix].nv.cal, " C");
             }
             else  {
                 strcat(devices[_ix].nv.cal, " U");
             }
             have_cal = true;
         }

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

   devices[_ix].when = timeGetTime();
   //int _ix = _client->getPortNumber() -1;
   //d.tcp_port = devices[_ix].tcp_port;
   //strncpy(d.url, devices[_ix].url, sizeof(d.url)-1);
	//strncpy(d.portname, devices[_ix].portname, sizeof(d.portname)-1);

   //strncpy(d.portname, d.url, sizeof(d.portname)-1);
#ifdef DO_PI
	const char *ccptr = ix_to_port_info[_ix].name.c_str();
   strncpy(d.portname, ccptr, sizeof(d.portname)-1);
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

	// known here
	//		win32 trainer is server
	//			devices[_ix].ix
	//			devices[_ix].commtype
	//			devices[_ix].tcp_port
	//			devices[_ix].url
	//			devices[_ix].portname
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
void dump_maps(void)  {
   logstream = fopen(DLLFILE, "a+t");
   fprintf(logstream, "dump_maps\n");

	// std::map<std::string, int> portname_to_ix;
	// std::map<int, PORT_INFO> ix_to_port_info;

	for (std::map<std::string,int>::iterator it=portname_to_ix.begin(); it!=portname_to_ix.end(); ++it)  {
		fprintf(logstream, "   %s    =>    %d\n", it->first.c_str(), it->second);
	}

	fprintf(logstream, "\n");

	for (std::map<int,PORT_INFO>::iterator it=ix_to_port_info.begin(); it!=ix_to_port_info.end(); ++it)  {
		fprintf(logstream, "%d    =>    portname = %s, portnum = %d, type = %d\n", it->first, it->second.name.c_str(), it->second.portnum, it->second.type);
	}

	FCLOSE(logstream);

	return;
}
#endif
#endif


#ifndef WIN32
/**********************************************************************
check for WIFI computrainer
the trainer is the server
**********************************************************************/

EnumDeviceType check_for_linux_computrainer_server(int _ix) {
	Client *client = NULL;
	DEVICE d;

	int comport = _ix + 1;

	if (comport < CLIENT_SERIAL_PORT_BASE || comport > CLIENT_SERIAL_PORT_BASE) {
		return DEVICE_OTHER_ERROR;
	}

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

#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	fprintf(logstream, "cft1\n", _comport);
	fclose(logstream);
#endif

#if 1
	if (devices[_ix].url[0] == 0 || devices[_ix].tcp_port == -1) {
#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "cft2\n", _comport);
		if (devices[ix].url[0] == 0) {
			fprintf(logstream, "url is NULL\n");
		}
		else {
			fprintf(logstream, "url = %s\n", devices[ix].url);
		}

		if (devices[ix].tcp_port == -1) {
			fprintf(logstream, "tcp port = -1\n");
		}
		else {
			fprintf(logstream, "tcp port = %d\n", devices[ix].tcp_port);
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
#endif


	try {
		//client = new Client("127.0.0.1", 9072);
		//client = new Client("miney2.mselectron.net", 9072);
		client = new Client(devices[_ix].url, devices[_ix].tcp_port);
	}
	catch (const int istatus) {

		//while(1)  {																	// infinite
		//	Sleep(100);
		//}

		if (glog) {
			glog->write(10, 0, 1, "   catch(%d)\n", istatus);
		}

		switch (istatus) {
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
	catch (...) {
		if (glog) {
			glog->write(10, 0, 1, "   catch...\n");
		}
	}

	if (client != NULL) {
		if (client->get_socket() == INVALID_SOCKET) {
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
	else {
		//return DEVICE_DOES_NOT_EXIST;
		d.what = DEVICE_DOES_NOT_EXIST;
		//d.port = 0;
		strcpy(d.nv.ver, "n/a");
		strcpy(d.nv.cal, "n/a");
		d.when = timeGetTime();
		devices[_ix] = d;
		return d.what;
	}

	if (glog) {
		glog->write(10, 0, 1, "   connected to CT server\n");
	}

	// we've connected to the server and received linkup message

	read_computrainer(client);						// fills in the global info for this computrainer ( devices[_ix] )

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

}						// check_for_linux_computrainer_server()


#else

/**********************************************************************
check for WIFI computrainer
the trainer is the client
**********************************************************************/

EnumDeviceType check_for_windows_server_computrainer(int _ix) {
	Client *client = NULL;
	DEVICE d;

#ifdef LOGDLL
	logstream = fopen(DLLFILE, "a+t");
	fprintf(logstream, "            check_for_windows_client_computrainer(%d)\n", _ix);
	fclose(logstream);
#endif

	int comport = _ix + 1;

	if (comport < UDP_SERVER_SERIAL_PORT_BASE || comport > UDP_SERVER_SERIAL_PORT_BASE + 15) {
		return DEVICE_OTHER_ERROR;
	}

	if (server == NULL)  {
		return DEVICE_OTHER_ERROR;
	}

	unsigned long start = timeGetTime();
	//ds->decoder->set_version(0);							// unsigned short

	while (1) {
		if (server->is_client_connected(_ix)) {
			read_server_computrainer(_ix);							// reads computrainer for 2 seconds, fills in stuff
			//flush(_ix, NULL, server, 500, FALSE);
			//return DEVICE_COMPUTRAINER;

			devices[_ix].what = DEVICE_COMPUTRAINER;
			break;
		}

		if ((timeGetTime() - start) >= 10000) {
			devices[_ix].what = DEVICE_DOES_NOT_EXIST;
			break;
		}

	}

	/*
	d.what = DEVICE_DOES_NOT_EXIST;
	//d.port = 0;
	strcpy(d.nv.ver, "n/a");
	strcpy(d.nv.cal, "n/a");
	d.when = timeGetTime();
	devices[_ix] = d;
	return d.what;
	*/

	strncpy(devices[_ix].nv.ver, "4095", sizeof(devices[_ix].nv.ver));
	strncpy(devices[_ix].nv.cal, "2.00 C", sizeof(devices[_ix].nv.cal));
	//devices[_ix].nv.cal = 200;


	devices[_ix].when = timeGetTime();

	return devices[_ix].what;

}						// check_for_windows_server_computrainer()

#endif				// #ifdef WIN32


						/**********************************************************************

						**********************************************************************/

int start_server(int _debug_level) {

	if (server == NULL) {
		try {
			//server = new Server(broadcast_port, listen_port, ip_discover, udp);
			//server = new RACERMATE::Ctsrv(broadcast_port, listen_port, ip_discover);
			server = new RACERMATE::Ctsrv(listen_port, broadcast_port, _debug_level);
		}
		catch (int i) {
			//int bp;

			switch (i) {
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

	return ALL_OK;
}										// start_server()

