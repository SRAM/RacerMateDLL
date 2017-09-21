
#ifdef WIN32
#define STRICT
#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#include <stdio.h>

#include <assert.h>

#include <glib_defines.h>
#include <utils.h>
#include <nvram.h>

/***************************************************************************

***************************************************************************/

NVRAM::NVRAM(char *_portstr)  {

	strncpy(portstr, _portstr, sizeof(portstr)-1);
	memset(u.buf, 0, sizeof(u.buf));
	port = NULL;
	gotver = false;

	//int size = sizeof(DATA);

	assert(sizeof(DATA)==256);

	connect();
}

/***************************************************************************

***************************************************************************/

NVRAM::NVRAM(Serial *_port)  {

	assert(_port);
	port = _port;

	portstr[0] = 0;
	memset(u.buf, 0, sizeof(u.buf));
	gotver = false;

	//int size = sizeof(DATA);

	assert(sizeof(DATA)==256);

}

/***************************************************************************

***************************************************************************/

NVRAM::~NVRAM()  {
	disconnect();
}

/**********************************************************************

**********************************************************************/

int NVRAM::read(void)  {
	int i;
	unsigned char c;
	DWORD then;
	//unsigned char buf2[256];
	bool done;


	if (!port)  {
		return 1;
	}


	c = 0x82;
	port->txx(&c, 1);				// command to dump nvram
	port->flushrx(100);			// flush the next 100 ms of data

	// pic sleeps for 500 ms, so the next data should be nvram


	//------------------------------------------------
	// get 256 bytes
	//------------------------------------------------

	i = 0;
	done = false;

	then = timeGetTime();

#ifndef __MACH__
	int n, j;
	char buf2[256];
	DWORD now;

	while(!done)  {
		n = port->rx((char *)buf2, sizeof(buf2)-1);
		if (n==0)  {
			now = timeGetTime();
			if ((now - then) > 1000) {
				DEL(port);
				return 1;								// no comms for 1 sec
			}
			continue;
		}
		else  {
			then = timeGetTime();
		}

		for(j=0; j<n; j++)  {
			if (i<256)  {
				u.buf[i] = buf2[j];
				i++;
				if (i==256)  {
					done = true;
				}
			}
		}
	}

#endif



#ifdef _DEBUG
	/*
	FILE *stream = fopen("in.x", "wt");

	for(i=0;i<16;i++)  {
		for(int j=0; j<16; j++)  {
			fprintf(stream, "%02x ", (u.buf[i*16+j] & 0x00ff) );
		}
		fprintf(stream, "\n");
	}
	fclose(stream);
	*/
#endif


	return 0;
}

/**********************************************************************
	entry:
		the fields of u.d are set up by the caller, except for the 
		following, which are automatically computed:

		1. checksum
		2. date
		3. time

**********************************************************************/

int NVRAM::write(void)  {
	int i;
	unsigned char c;


	if (port==NULL)  {
		return 1;
	}

	/*
	SYSTEMTIME x;
	GetLocalTime(&x);
	sprintf(u.d.date, "%02d-%02d-%04d", x.wMonth, x.wDay, x.wYear);
	sprintf(u.d.time, "%02d:%02d", x.wHour, x.wMinute);
	*/

	u.d.nv_checksum = 0;
	for(i=0;i<254;i++)  {
		u.d.nv_checksum += u.buf[i];
	}


	c = 0x85;
	port->txx(&c, 1);				// command to pic to accept nvram


	for(i=0; i<256; i++)  {
		c = u.buf[i];
		port->txx(&c, 1);
		Sleep(5);
	}

#ifdef _DEBUG
	FILE *stream = fopen("out.x", "wt");

	for(i=0;i<16;i++)  {
		for(int j=0; j<16; j++)  {
			fprintf(stream, "%02x ", (u.buf[i*16+j] & 0x00ff) );
		}
		fprintf(stream, "\n");
	}

	fclose(stream);
#endif

	Sleep(500);

	return 0;
}

/**********************************************************************

**********************************************************************/

void NVRAM::connect(void)  {

#ifdef WIN32
	try  {
		port = new Serial(portstr, "38400", "N", 8, 1);
	}
	catch (const int istatus) {
		switch(istatus)  {
			case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
			case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
			case SERIAL_ERROR_BUILDCOMMDCB:
			case SERIAL_ERROR_SETCOMMSTATE:
			case SERIAL_ERROR_CREATEEVENT1:
			case SERIAL_ERROR_CREATEEVENT2:
			case SERIAL_ERROR_OTHER:
			default:
				break;
		}
	}
#else

#ifdef WIN32
	SerialPort_DCS dcs;
	//const char *devname = wxCOM1;
	port = new Serial();
	dcs.baud = wxBAUD_38400;
	dcs.parity = wxPARITY_NONE;
	dcs.wordlen = 8;
	dcs.stopbits = 1;
	dcs.rtscts = false;
	dcs.xonxoff = false;

	//printf("rtd trying to open serial port at 2400 baud\n");

   //char str[32];
   //sprintf(str, "/dev/ttyS%d", comportnum-1);


	if(port->Open(portstr, &dcs) < 0) {
		DEL(port);
      return;
	}
   else  {
		//printf("opened %s\n", portstr);
	}
#endif

#endif

	return;
}

/**********************************************************************

**********************************************************************/

void NVRAM::disconnect(void)  {
	if (portstr[0]==0)  {
		// don't delete the port if it was passed in. Only delete it if
		// we created it!!!
		return;
	}
	DEL(port);
	return;
}

/**********************************************************************

**********************************************************************/

//void NVRAM::setSerialNumber(unsigned long _sn)  {
//	u.d.sernum = _sn;
//	return;
//}

/**********************************************************************

**********************************************************************/

void NVRAM::setFactoryCalibration(unsigned long _cal)  {
	u.d.factoryCalibration = _cal;
	return;
}

/**********************************************************************

**********************************************************************/

//void NVRAM::setCalibration(unsigned long _cal)  {
//	u.d.calibration = _cal;
//	return;
//}

/**********************************************************************

**********************************************************************/

//void NVRAM::setVersion(unsigned long _ver)  {
//	u.d.version = _ver;
//	return;
//}

/*************************************************************************

*************************************************************************/
/*
int NVRAM::getversion(void)  { 
	int i, j, n;
	unsigned char c;
	DWORD then, now;
	unsigned char buf[2];
	unsigned char buf2[16];
	bool done;


	if (!port)  {
		return 1;
	}


	c = 0x86;
	port->txx(&c, 1);				// command to dump nvram
	port->flushrx(100);			// flush the next 100 ms of data

	// pic sleeps for 500 ms, so the next data should be nvram


	//------------------------------------------------
	// get 2 bytes
	//------------------------------------------------

	i = 0;
	done = false;

	then = timeGetTime();

	while(!done)  {
		n = port->rx((char *)buf2, sizeof(buf2));
		if (n==0)  {
			now = timeGetTime();
			if ((now - then) > 1000) {
				DEL(port);
				return 1;								// no comms for 1 sec
			}
			continue;
		}
		else  {
			then = timeGetTime();
		}

		for(j=0; j<n; j++)  {
			if (i<2)  {
				buf[i] = buf2[j];
				i++;
				if (i==2)  {
					done = true;
					gotver = true;
					u.d.version = buf[0];
					u.d.build = buf[1];
				}
			}
		}
	}

	return 0; 
}
*/
