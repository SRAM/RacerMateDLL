
#define WIN32_LEAN_AND_MEAN


#include "levels.h"



/**********************************************************************************************************
racermate_stop()			racermate_close()

racermate_init()
start_server(..)

**********************************************************************************************************/

#include <aunt.h>

#pragma warning(disable:4996)					// for vista strncpy_s
#include <assert.h>
#include <shlobj.h>
#include <assert.h>
#include <shlobj.h>
#include <conio.h>
#include <time.h>

#include <glib_defines.h>
#include <globals.h>
#include <comglobs.h>
#include <utils.h>
#include <fatalerror.h>
#include <err.h>

#include <computrainer.h>
#include <nvram.h>

#include <errors.h>
#include <ev.h>					// includes <map.h>
#include <dll_globals.h>
#include <internal.h>			// includes <serial.h>
#include <dll.h>


#ifdef LOGDLL
	#define _CRTDBG_MAP_ALLOC
#endif


#if LEVEL >= DLL_FULL_ACCESS
	#pragma message ("***********************  FULL_ACCESS  ***********************")
#elif LEVEL >= DLL_ERGVIDEO_ACCESS
	#pragma message ("***********************  DLL_ERGVIDEO_ACCESS  ***********************")
#elif LEVEL >= DLL_TRINERD_ACCESS
	#pragma message ("***********************  DLL_TRINERD_ACCESS  ***********************")
#elif LEVEL >= DLL_CANNONDALE_ACCESS
	#pragma message ("***********************  DLL_CANNONDALE_ACCESS  ***********************")
#else
	//#pragma message ("***********************  DLL_UNKNOWN_ACCESS  ***********************")
	#error ***********************  NOT OBFUSCATED UNKNOWN_ACCESS  ***********************
#endif

#ifdef LOGDLL
	#pragma message("LOGDLL is defined !!!!!!!!!!!!!!!!!!!!!")
#endif


#if LEVEL >= DLL_CANNONDALE_ACCESS

/**********************************************************************

**********************************************************************/

const char *get_udpkey(const char * _id) {
	std::unordered_map<std::string, DEVICE>::const_iterator got;

	got = devices.find(_id);
	if(got == devices.end()) {
		return "";
	}

	return devices[_id].udpkey;

	/*
	status = sync(_id, devices[_id].udpkey);

	switch(status) {
		case ADJ_OK:
			break;
		case ADJ_NOSERVER:												// no server
			break;
		case ADJ_UDPKEY_RESET:
			break;
		case 	ADJ_NO_DEVICES:
			break;
		case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
			break;
		case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
			break;
		default:
			break;
	}
	*/

	//return "";
}

#ifdef DO_PI
/**********************************************************************
	set_port_info
**********************************************************************/

//^ set_port_info
int set_port_info(int _ix, const char *_name, int _type, int _portnum)  {
		
			
#ifdef LOGDLL
   logstream = fopen(DLL_LOGFILE, "a+t");
   fprintf(logstream, "set_port_info(%d, %s, %d, %d)\n", _ix, _name, _type, _portnum);
   fclose(logstream);
#endif

	if (_ix<0 || _ix>255)  {
		return BAD_RIDER_INDEX;
	}

	PORT_INFO pi;
	pi.name = _name;
	pi.portnum = _portnum;
	pi.type = (TRAINER_COMMUNICATION_TYPE) _type;

#ifdef LOGDLL
    logstream = fopen(DLL_LOGFILE, "a+t");
	 fprintf(logstream, "   pi.name = %s\n", pi.name.c_str());
	 fprintf(logstream, "   pi.type = %d\n", pi.type);
	 fprintf(logstream, "   pi.portnum = %d\n", pi.portnum);
    fclose(logstream);
#endif

	portname_to_ix[_name] = _ix;
	ix_to_port_info[_ix] = pi;

#ifdef LOGDLL
	dump_maps();
#endif

	return ALL_OK;
}							// int

//$
#endif

/**********************************************************************

**********************************************************************/


//^ racermate_init
int racermate_init(void)  {

	SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, personal_path);					// "C:\Users\larry\Documents", in dll_globals.h
	strcat(personal_path, "\\dll");
	if (!direxists(personal_path))  {
		CreateDirectory(personal_path, NULL);
	}


	#ifdef LOGDLL
		sprintf(DLL_LOGFILE, "%s\\dll.log", personal_path);
		unlink(DLL_LOGFILE);
		#ifdef WIN32
			crtflags = _CRTDBG_REPORT_FLAG;
			crtflags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
			crtflags |= _CRTDBG_CHECK_ALWAYS_DF;
			crtflags |= _CRTDBG_ALLOC_MEM_DF;
			crtflags |= _CRTDBG_LEAK_CHECK_DF;
			_CrtSetDbgFlag(crtflags);
			_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
			_CrtSetReportMode(_CRT_ASSERT, 0);				// Disable the message box for assertions.
			_CrtMemCheckpoint(&memstart);
			//_crtBreakAlloc = 150;
		#endif
	    logstream = fopen(DLL_LOGFILE, "a+t");
		 fprintf(logstream, "racermate_init()\n");
		fclose(logstream);
	#endif

//to do
	// make sure that only one instance of the dll is running

	return 0;
}								// racermate_init()


/**********************************************************************
	returns 1 if there is no network available
**********************************************************************/

int start_server(int _listen_port,  int _broadcast_port, const char *_myip, int _debug_level)  {
	int bp = 0;

	try {
		server = new RACERMATE::Ctsrv(_listen_port, _broadcast_port, _myip, _debug_level);             // ip_discovery = false, udp = true
	}
	catch (int i) {
		//bp = 2;
		return i;
	}

	if(!server) {
		return -3;
	}


	int nclients = server->get_nclients();

	//int n = check_map_keys();
	int n = server->check_udp_keys();

	return 0;
}






/**********************************************************************

**********************************************************************/

int stop_server(void)   {
	DEL(server);
	return ALL_OK;
}

/**********************************************************************

**********************************************************************/

EnumDeviceType get_trainer_type(int _ix)  {

	return DEVICE_DOES_NOT_EXIST;
}
//$ racermate_init

/**********************************************************************

**********************************************************************/

//^ racermate_close

int racermate_close(void)  {
	static bool called = false;
	if (called)  {
		return 0;
	}

	called = true;

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "racermate_close\n");
		FCLOSE(logstream);
	#endif

	if (server) {
		DEL(server);
	}

#ifdef DO_PI
	portname_to_ix.clear();
	ix_to_port_info.clear();
#endif

	ResetAlltoIdle();


	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");

		_CrtCheckMemory( );

		//fprintf(logstream, "\ncalling _CrtDumpMemoryLeaks:\n");
		//_CrtDumpMemoryLeaks();

		_CrtMemCheckpoint(&memstop);


		fprintf(logstream, "\ncalling _CrtMemDifference:\n");
		int kk = _CrtMemDifference(&memdiff, &memstart, &memstop);

		if(kk)  {
			OutputDebugString("\n_CrtMemDumpStatistics");
			_CrtMemDumpStatistics(&memdiff);
		}
		fprintf(logstream, "\ndone checking memory corruption\n\n");

		fprintf(logstream, "dllx\n");
		FCLOSE(logstream);

		// make a copy of LOGFILE to c:\Users\larry\documents\dll

		SYSTEMTIME x;
		GetLocalTime(&x);
		char fname[64];
		char dest[260];
		strcpy(dest, personal_path);
		sprintf(fname,"\\dll_%04d-%02d-%02d_%02d-%02d-%02d.log",x.wYear, x.wMonth, x.wDay, x.wHour, x.wMinute, x.wSecond);
		strcat(dest, fname);

		if (exists(DLL_LOGFILE))  {
			BOOL b = CopyFile(DLL_LOGFILE, dest, FALSE);					// existing, new
			if (!b)  {
				Err *err = new Err();							// The filename, directory name, or volume label syntax is incorrect
				OutputDebugString(err->getString());
				OutputDebugString("\n");
				DEL(err);
			}
		}

	#endif									// logdll
	return 0;
}
//$ racermate_close


/**********************************************************************

**********************************************************************/

//^ get_errstr
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
//$ get_errstr

/**********************************************************************

**********************************************************************/

//^ GetRacerMateDeviceID

EnumDeviceType GetRacerMateDeviceID(const char *_id)  {

	EnumDeviceType what;

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fclose(logstream);
	#endif


	setup_standard_dirs();

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   back from setup_standard_dirs()\n");
		FCLOSE(logstream);
	#endif



	// xxx
	strncpy(devices[_id].id, _id, sizeof(devices[_id].id) - 1);



	//DEVICE *d;
	//d = &devices[_id];
	//devices.erase(_id);





	if (strstr(devices[_id].id, "UDP-") != NULL) {
		update_udp_device(_id);

		#if 0
			//devices[_id].udpkey = server->get_udpkey_from_devnum(&_id[4]);						// eg, "192.168.1.40 44085"
			strncpy(devices[_id].udpkey, server->get_udpkey_from_devnum(&_id[4]), sizeof(devices[_id].udpkey));

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
				int bp = 0;
				if(n > 1) {
					bp = 2;
				}
			#endif

			unsigned short fw = server->get_fw(devices[_id].udpkey);
			int cal = server->get_cal(devices[_id].udpkey);
			sprintf(devices[_id].nv.ver, "%d", fw);
			sprintf(devices[_id].nv.cal, "%d", cal);
			devices[_id].when = timeGetTime();
		#endif
	}					// 	if (strstr(devices[_id].id, "UDP-") != NULL)
	else if (strstr(devices[_id].id, "COM") != NULL)  {
		devices[_id].commtype = WIN_RS232;
		devices[_id].comport_number = atoi(&devices[_id].id[3]);
		if(devices[_id].comport_number<1 || devices[_id].comport_number>256)  {
			throw(fatalError(__FILE__, __LINE__));
		}
	}



	if (glog)  {
		glog->write(10,0,1,"GetRacerMateDeviceID(%s)\n", _id);
	}

	if (gfile_mode)  {
		if (glog) glog->write(10,0,1,"file_mode\n");
		if (devices[_id].file_mode)  {
			what = DEVICE_RMP;
			devices[_id].what = what;
			//const char *cptr = ix_to_port_info[_ix].name.c_str();
#ifdef DO_PI
			const char *cptr = ix_to_port_info[_ix].name.c_str();
			strncpy(devices[_ix].portname, cptr, sizeof(devices[_ix].portname)-1);
#endif
			//devices[ix].port = ix + 1;
			devices[_id].when = timeGetTime();
			return what;
		}
	}

	what = devices[_id].what;
	if (glog) glog->write(10,0,1,"grmid1\n");

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   devices[_ix].what = %s\n", devstrs[what]);
		FCLOSE(logstream);
	#endif

	if ( haveDevice(what, _id) )  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   already have device\n");
			FCLOSE(logstream);
		#endif

		if (glog) glog->write(10,0,1,"   already have device\n");
		return what;
	}


	if (glog) glog->write(10,0,1,"   grmid3b\n");

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   did not haveDevice(%s), calling check_for_trainers(%s, 0)\n", _id, _id);
		FCLOSE(logstream);
	#endif

	what = check_for_trainers(_id);

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   back from check_for_trainers, what = %s\n", devstrs[what]);
		FCLOSE(logstream);
	#endif

	if (glog) glog->write(10,0,1,"   grmidx2, %s\n", devstrs[what]);
	return what;
}					// EnumDeviceType GetRacerMateDeviceID(int ix)  {

//$

	/**********************************************************************
		NOTE!!! RM1 does not call this!!!!
	**********************************************************************/

	//^ Setlogfilepath

	int Setlogfilepath(const char *dir)  {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
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

		if (glog)  {
			glog->write(10,0,1,"slfp ok\n");
		}

		return ALL_OK;
	}									// Setlogfilepath()
	//$

	/**********************************************************************
		Enablelogs
	**********************************************************************/

	//^ Enablelogs

	int Enablelogs(bool _bikelog, bool _courselog, bool _decoderlog, bool _dslog, bool _gearslog, bool _physicslog)  {                    // full

	#ifdef _DEBUG
		//throw(fatalError(__FILE__, __LINE__));
		//throw(1);					// works
		//throw("set_dirs");		// works
	#endif


		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
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


		sprintf(gstring, "%s\\log.txt", dirs[DIR_DEBUG].c_str());

		if (false)  {
			Bike::gblog = true;
			//courselog = true;
			Decoder::decoderlog = true;
			dataSource::gblog = true;
			Bike::gbgearslog = true;
			Physics::gblog = true;
		}
		else  {
			Bike::gblog = _bikelog;
			//courselog = _courselog;
			Decoder::decoderlog = _decoderlog;
			dataSource::gblog = _dslog;
			Bike::gbgearslog = _gearslog;
			Physics::gblog = _physicslog;
		}

		return ALL_OK;
	}							// Enablelogs()

	//$

	/**********************************************************************

	**********************************************************************/

	//^ GetFirmWareVersion

	int GetFirmWareVersion(const char *_id) {

	const char *cptr;
	EnumDeviceType what=DEVICE_NOT_SCANNED;
	int n;
	float f;
	int version;

	setup_standard_dirs();

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "GetFirmWareVersion(%s)\n", _id);
		fclose(logstream);
	#endif

	what = devices[_id].what;

	if ( !haveDevice(what, _id) )  {

		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "gfwv 1\n");
		fclose(logstream);
		#endif

		what = GetRacerMateDeviceID(_id);

		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "gfwv 2\n");
		fclose(logstream);
		#endif
		if ( !haveDevice(what, _id) )  {
			return GENERIC_ERROR;
		}
	}

		if (what==DEVICE_COMPUTRAINER)  {
			cptr = get_computrainer_firmware(_id);

			n = sscanf(cptr, "%f", &f);
			if (n != 1)  {
				return GENERIC_ERROR;
			}
			version = (int) f;
			return version;
		}
		else if (what==DEVICE_VELOTRON)  {

			int i = atoi(_id);
			cptr = get_velotron_firmware(_id);
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
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_dll_version

	const char *get_dll_version(void)  {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_dll_version()\n");
			fclose(logstream);
		#endif

		return DLL_VERSION;

	}				// int get_dll_version(int _portnum)
	//$

	/**********************************************************************

	**********************************************************************/

	//^ GetAPIVersion

	const char *GetAPIVersion(void)  {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "GetAPIVersion()\n");
			fclose(logstream);
		#endif
		return API_VERSION;
	}					// GetAPIVersion()
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_dll_version

	const char *get_build_date(void)  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_build_date()\n");
			fclose(logstream);
		#endif

		sprintf(buildDate, "%s %s", __DATE__, __TIME__);									// "Apr 14 2015 10:05:23"
		return buildDate;

	}				// int get_dll_version(int _portnum)
	//$

	/**********************************************************************

	**********************************************************************/

	//^ GetIsCalibrated

	bool GetIsCalibrated(const char *ix, int FirmwareVersion) {
		EnumDeviceType what;

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "GetIsCalibrated(%s)\n", ix);
		fclose(logstream);
	#endif

		what = devices[ix].what;
		if ( haveDevice(what, ix) )  {

			if (what!=DEVICE_COMPUTRAINER && what!=DEVICE_VELOTRON)  {
				return false;
			}
			return iscal(what, ix);
		}

		what = check_for_trainers(ix);
		#ifdef WIN32
		assert(what>=0 && what<=DEVICE_OTHER_ERROR);
		#endif

		if (what!=DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON)  {
			return false;
		}

		return iscal(what, ix);

	}					// bool GetIsCalibrated(int _portnum)  {
	//$

	/**********************************************************************
		comport starts at 0
	**********************************************************************/

	//^ GetCalibration

	int GetCalibration(const char *_id)  {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "GetCalibration(%s)\n", _id);
			fclose(logstream);
		#endif

	int cal;
	DEVICE *d = &devices[_id];

	if(d->commtype == TRAINER_IS_CLIENT) {
		const char *cptr = d->udpkey;
		if(server) {
			cal = server->get_cal(d->udpkey);
		}
	}

		int n;
		int comport;
		char str[32];
		float f;
		const char *cptr;
		EnumDeviceType what=DEVICE_NOT_SCANNED;

		what = devices[_id].what;

		if ( !haveDevice(what, _id) )  {
			what = GetRacerMateDeviceID(_id);
			if ( !haveDevice(what, _id) )  {
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
			cptr = devices[_id].nv.cal;

			n = sscanf(cptr, "%f", &f);
			if (n != 1) {
				return GENERIC_ERROR;
			}

			cal = (int)(.5 + 100.0f*f);

#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   GetCalibration: returning %d\n", cal);
			fclose(logstream);
#endif
			return cal;
		}
		else if (what==DEVICE_VELOTRON)  {
			cptr = devices[_id].nv.cal;
			long vtcal;
			n = sscanf(cptr, "%ld", &vtcal);
			if (n!=1)  {
				return GENERIC_ERROR;
			}
			return (int)vtcal;
		}


		what = devices[_id].what;
		if ( haveDevice(what, _id) )  {
			return GENERIC_ERROR;
		}

		what = check_for_trainers(_id);
		if (what!=DEVICE_COMPUTRAINER)  {
			return GENERIC_ERROR;
		}

		cptr = get_computrainer_cal(_id);
		n = sscanf(cptr, "%f %s", &f, str);
		if (n!=2)  {
			return GENERIC_ERROR;
		}
		cal = ROUND(100.0f*f);
		return cal;
	}											// GetCalibration()
	//$
	/**********************************************************************

	**********************************************************************/

	//^ resetTrainer
	
	int resetTrainer(const char *_id, int _fw, int _cal) {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "resetTrainer(%s)\n", _id);
			fclose(logstream);
		#endif


		DEVICE *d = &devices[_id];

		if(d->commtype == TRAINER_IS_CLIENT) {
			const char *cptr = d->udpkey;
			if(server) {
				//server->???(d->udpkey);
			}
		}
		else {
			devices[_id].ev->reset();
		}

		return ALL_OK;
	}							// int resetTrainer(int ix, int _fw, int _cal)
	//$


/**********************************************************************

**********************************************************************/

//^ startTrainer
int startTrainer(const char * _id) {

	int bp = 0;
#ifdef _DEBUG
	int n;
	n = check_maps("dll.cpp, startTrainer()");

	//assert(n > 0);
	if(n > 1) {
		bp = 2;
	}
#endif



	DEVICE *d = &devices[_id];

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "startTrainer(%s)\n", _id);
		fclose(logstream);
	#endif

	devices[_id].logging_type = NO_LOGGING;

	#ifdef DO_ANT
		//if (!ant_stick_initialized)  {					// global from ant library
		if (!asi)  {					// global from ant library
			int bp;
			int status = ant_init();
			switch(status)  {
				case 0:								// ant stick plugged in and initialized
					bp = 1;
					break;
				case 1:								// no ant stick
					bp = 2;
					break;
				case 2:								// reset error
					MessageBox(NULL, "ResetSystem failed", "Error", MB_OK);
					break;
				case 3:								// no ant stick
					MessageBox(NULL, "SetNetworkKey failed 1", "Error", MB_OK);
					break;
				case 4:								// no ant stick
					MessageBox(NULL, "SetNetworkKey failed 2", "Error", MB_OK);
					break;
				default:
					bp = 0;
					break;
			}

			if (status != 0)  {
				int bp = 1;
				//MessageBox
				//return GENERIC_ERROR;
			}
		}
	#endif

	/*
	if (server) {
		if (server->client_is_running(ix)) {
			//return DEVICE_ALREADY_RUNNING;
			return ALL_OK;
		}
	}
	if (devices[ix].ev) {				// this trainer is already started
		return DEVICE_ALREADY_RUNNING;
	}
	*/


	devices[_id].course = NULL;

	EnumDeviceType what;

	if (devices[_id].what == DEVICE_NOT_SCANNED)  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   device not scanned\n");
			fclose(logstream);
		#endif
		what = GetRacerMateDeviceID(_id);

		if (what==DEVICE_DOES_NOT_EXIST)  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   returning device does not exist: %d\n", DEVICE_DOES_NOT_EXIST);
				fclose(logstream);
			#endif
			return DEVICE_DOES_NOT_EXIST;
		}
		else if (what==DEVICE_OPEN_ERROR)  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   returning port open error: %d\n", PORT_OPEN_ERROR);
				fclose(logstream);
			#endif
			return PORT_OPEN_ERROR;
		}
	}
	else if (devices[_id].what == DEVICE_DOES_NOT_EXIST)  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   device was already scanned\n");
			fclose(logstream);
		#endif

		what = GetRacerMateDeviceID(_id);
		if ( haveDevice(what, _id) )  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   returning device does not exist 2: rc = %d\n", DEVICE_DOES_NOT_EXIST);		// 1 = serial port does not exist
				fprintf(logstream, "   ix = %s\n", _id);

				if (what==DEVICE_COMPUTRAINER)  {
					fprintf(logstream, "   what = COMPUTRAINER\n");
				}
				else if (what==DEVICE_VELOTRON)  {
					fprintf(logstream, "   what = VELOTRON\n");
				}
				else  {
					fprintf(logstream, "   what = ???\n");
				}

				fprintf(logstream, "   ix = %s\n", _id);
				fclose(logstream);
			#endif

			return DEVICE_DOES_NOT_EXIST;
		}

		// device does not exist has timed out. see if it still doesn't exist and restart the timer:

		what = GetRacerMateDeviceID(_id);
		if (what == DEVICE_DOES_NOT_EXIST)  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   returning device does not exist 3: %d\n", DEVICE_DOES_NOT_EXIST);
				fclose(logstream);
			#endif
			return DEVICE_DOES_NOT_EXIST;
		}
	}

	what = devices[_id].what;

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   ok1\n");
		fclose(logstream);
	#endif


	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		instances++;
	}

	if (what == DEVICE_COMPUTRAINER)  {
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   ok2a: device is computrainer\n");
		fclose(logstream);
	#endif

		if(devices[_id].commtype == WIN_RS232) {
			devices[_id].ev = new EV(
				d->id,									// "COM4"
				DEVICE_COMPUTRAINER,					// EnumDeviceType _what,
				devices[_id].rd,						// RIDER_DATA _rd,
				&devices[_id].bike_params,			// Bike::PARAMS *_params,
				NULL,										// Course *_course,
				devices[_id].logging_type			// LoggingType,
			);

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   ev created\n");
			fclose(logstream);
		#endif
		}


		#ifdef DO_ANT
			if (asi)  {
				float diameter = 700.0f;									// mm
				float circumference = (float) (PI * diameter);		// mm
				if(devices[_id].commtype == WIN_RS232) {
					devices[_id].ev->set_circumference_x(circumference);
				}
			}
		#endif

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   st1\n");
			fclose(logstream);
		#endif

		#ifdef DO_EXPORT
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   calling export_flag_file\n");
				fclose(logstream);
			#endif

				if (export_flag_file()) {								// in startTrainer()
#ifdef LOGDLL
					logstream = fopen(DLL_LOGFILE, "a+t");
					fprintf(logstream, "   back from export_flag_file\n");
					fclose(logstream);
#endif

					//devices[ix].ev->set_export();
					if (d->commtype == TRAINER_IS_CLIENT) {
						if (server) {
							server->set_export(_id, dirs[DIR_DEBUG].c_str());
						}
					}
					else  {
						devices[_id].ev->set_export(dirs[DIR_DEBUG].c_str(), d->comport_number);
					}
			}
		#endif					// #ifdef DO_EXPORT

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   ok2a2\n");
			fclose(logstream);
		#endif

		if (d->commtype == TRAINER_IS_CLIENT) {
			if (server) {
				//const char *key = server->get_udpkey_from_devnum(&_id[4]);
				//const char *cptr = d->udpkey;
				server->set_file_mode(d->udpkey, gfile_mode);
			}
		}
		else {
			devices[_id].ev->set_file_mode_x(gfile_mode);											// datasource
		}

		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   ok2b\n");
		fclose(logstream);
		#endif
	}														// if (what == DEVICE_COMPUTRAINER) {
	else if (what==DEVICE_VELOTRON)  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   ok2\n");
			fclose(logstream);
		#endif

		if (devices[_id].bike_params.real_front_teeth==0)  {					// ActualChainRing?
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   startTrainer error:\n");
				fclose(logstream);

				//dump_bike_params(ix);
			#endif
			return VELOTRON_PARAMETERS_NOT_SET;
		}


		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "   ok6, ix = %s\n", _id);
			fclose(logstream);
		#endif

		devices[_id].ev = new EV(
			_id + 1,										// int _realportnum,
			DEVICE_VELOTRON,						// EnumDeviceType _what,
			devices[_id].rd,						// RIDER_DATA _rd,
			&devices[_id].bike_params,			// Bike::PARAMS *_params,
			NULL,										// Course *_course,
			devices[_id].logging_type			// LoggingType,
			//devices[_id].url,						// const char *_url,
			//devices[_id].tcp_port					// int _tcp_port
		);

		devices[_id].ev->set_logging_x(devices[_id].logging_type);
	}


	if (d->commtype == TRAINER_IS_CLIENT) {
		//if (portnum >= UDP_SERVER_SERIAL_PORT_BASE && portnum <= UDP_SERVER_SERIAL_PORT_BASE + 16) {								// trainer is UDP client
		if (server) {
		}
	}
	else {
		//if (devices[_id].ev)  {
		if(what == DEVICE_COMPUTRAINER || what == DEVICE_VELOTRON) {
			if(!devices[_id].ev->initialized) {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   error: ev not initialized\n");
				fclose(logstream);
			#endif
				DEL(devices[_id].ev);
				return DEVICE_NOT_INITIALIZED;
			}
		}
	}


	Sleep(100);

	#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "   ok6b\n");
	fclose(logstream);
	#endif

	if (d->commtype == TRAINER_IS_CLIENT) {
		//if (portnum >= UDP_SERVER_SERIAL_PORT_BASE && portnum <= UDP_SERVER_SERIAL_PORT_BASE + 16) {								// trainer is UDP client
		if (server) {
			server->set_hr_bounds(d->udpkey, ROUND(devices[_id].rd.lower_hr), ROUND(devices[_id].rd.upper_hr), devices[_id].beep_enabled);
			server->set_ftp(d->udpkey, server->get_ftp(_id));
		}
	}
	else if (d->commtype == WIN_RS232)  {
		devices[_id].ev->set_hr_bounds_x(ROUND(devices[_id].rd.lower_hr), ROUND(devices[_id].rd.upper_hr), devices[_id].beep_enabled);
		devices[_id].ev->set_ftp_x(devices[_id].rd.ftp);
	}

	#ifndef DO_EV_EVENT
		// wait  to make sure that the thread is running:	while (!devices[ix].ev->get_running())  {
		}
	#endif

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   startTrainerx\n");
		fclose(logstream);
		//dump_bike_params(ix);
	#endif

	// the trainer is running, but decoder::started is false... you need to 'start' it if you want the decoder to accumulate time and distance.
	#ifdef _DEBUG
		#if defined (WIN32)
			DWORD tid = GetCurrentThreadId();										// 0X00002530
		#endif
	#endif

	return ALL_OK;
}			// startTrainer(int ix)

//$

	/**********************************************************************

	**********************************************************************/

	//^ ResetAverages


int ResetAverages(const char *_id, int _fw) {

	#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "ResetAverages()\n");
	fclose(logstream);
	#endif

	DEVICE *d = &devices[_id];
	const char *cptr = d->udpkey;

	//xxx
	if (d->commtype == TRAINER_IS_CLIENT) {
		if (server) {
			server->reset_averages(d->udpkey);
		}
	}
	else {
		devices[_id].ev->reset_averages();
	}

	return ALL_OK;
}								// ResetAverages()

	//$
	/**********************************************************************
		example _id = "UDP-5678"

	**********************************************************************/

	//^ GetTrainerData


	struct RACERMATE::TrainerData GetTrainerData(const char *_id, int fw) {
		RACERMATE::TrainerData td;
		RACERMATE::TrainerData *ptd;

		if(devices[_id].commtype == TRAINER_IS_CLIENT) {
			DEVICE *d;
			ADJCODE status;

			status = sync(_id, devices[_id].udpkey);

			switch(status) {
				case ADJ_OK:
					d = &devices[_id];			// this will create devices[_id] if it doesn't exist
					td = server->GetTrainerData(d->udpkey);
					break;
				case ADJ_NOSERVER:												// no server
					memset(&td, 0, sizeof(td));
					break;
				case ADJ_UDPKEY_RESET:
					d = &devices[_id];			// this will create devices[_id] if it doesn't exist
					td = server->GetTrainerData(d->udpkey);
					break;
				case 	ADJ_NO_DEVICES:
					memset(&td, 0, sizeof(td));
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					memset(&td, 0, sizeof(td));
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					memset(&td, 0, sizeof(td));
					break;
				default:
					memset(&td, 0, sizeof(td));
					break;
			}
			return td;
		}									// if(devices[_id].commtype == TRAINER_IS_CLIENT)


	#ifdef _DEBUG
		if (devices[_id].ev==NULL)  {
			memset(&td, 0, sizeof(td));
			return td;
		}

		EnumDeviceType what;
		what = devices[_id].ev->getwhat();

		if (what==DEVICE_VELOTRON)  {
			if (fw != 190)  {
				throw(fatalError(__FILE__, __LINE__));
			}
		}
	#endif

		if (devices[_id].ev)  {
			if (calibrating)  {
				ptd = devices[_id].ev->fastread();
			}
			else  {
				ptd = devices[_id].ev->myread();
			}
			memcpy(&td, ptd, sizeof(RACERMATE::TrainerData));
		}
		else  {
			memset(&td, 0, sizeof(td));
		}

		return td;
	}											// GetTrainerData()
	//$

	/**********************************************************************

	**********************************************************************/
	
	//^ get_average_bars

	float * get_average_bars(const char *_id, int FWVersion) {

		float *fptr;

		if(devices[_id].commtype == TRAINER_IS_CLIENT) {
			DEVICE *d;
			ADJCODE status;
			status = sync(_id, devices[_id].udpkey);
			switch(status) {
				case ADJ_OK:
					d = &devices[_id];			// this will create devices[_id] if it doesn't exist
					fptr = server->get_average_bars(d->udpkey);
					return fptr;
				case ADJ_UDPKEY_RESET:
					break;
				case ADJ_NOSERVER:												// no server
					break;
				case 	ADJ_NO_DEVICES:
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					return nobars;
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					break;
				default:
					break;
			}
			return nobars;
		}

		if (!devices[_id].ev)  {
			return nobars;
		}

		fptr = devices[_id].ev->get_average_bars_x();

		return fptr;
	}									// get_average_bars()
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_ss_data

	struct SSDATA get_ss_data(const char *_id, int fw) throw(...) {

		#ifdef LOGDLL
			/*
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_ss_data(%d)\n", ix);
			fclose(logstream);
			*/
		#endif

		SSDATA ssd;

		if(devices[_id].commtype == TRAINER_IS_CLIENT) {
			/*
			if(sync(_id, devices[_id].udpkey) == 1) {
				memset(&ssd, 0, sizeof(ssd));
				return ssd;;
			}
			DEVICE *d = &devices[_id];			// this will create devices[_id] if it doesn't exist
			if(server) {
				ssd = server->get_ss_data(d->udpkey, fw);
				return ssd;
			}
			memset(&ssd, 0, sizeof(ssd));
			return ssd;;
			*/

			DEVICE *d;
			ADJCODE status;
			status = sync(_id, devices[_id].udpkey);
			switch(status) {
				case ADJ_OK:
					d = &devices[_id];			// this will create devices[_id] if it doesn't exist
					ssd = server->get_ss_data(d->udpkey, fw);
					return ssd;
				case ADJ_UDPKEY_RESET:
					break;
				case ADJ_NOSERVER:												// no server
					memset(&ssd, 0, sizeof(ssd));
					return ssd;

				case 	ADJ_NO_DEVICES:
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					break;
				default:
					break;
			}
			memset(&ssd, 0, sizeof(ssd));
			return ssd;;
		}


	#ifdef _DEBUG
		if (devices[_id].ev)  {
			EnumDeviceType what;
			what = devices[_id].ev->getwhat();
			if (what==DEVICE_VELOTRON)  {
				if (fw != 190)  {
					throw(fatalError(__FILE__, __LINE__));
				}
			}
		}
	#endif
		SSDATA *pssd;

		if (devices[_id].ev)  {
			pssd = devices[_id].ev->read_ss();
			memcpy(&ssd, pssd, sizeof(SSDATA));
		}
		else  {
			memset(&ssd, 0, sizeof(ssd));
		}
		return ssd;
	}							// get_ss_data()
	//$

	/**********************************************************************

	**********************************************************************/


	//^ SetSlope

int SetSlope(const char *_id, int fw, int cal, float bike_kgs, float person_kgs, int _drag_factor, float grade) {
	
	DEVICE *d;
	d = &devices[_id];

	if (d->commtype == TRAINER_IS_CLIENT) {
		if (server) {
			server->set_slope(d->udpkey, bike_kgs, person_kgs, _drag_factor, grade);
			devices[_id].rd.watts_factor = server->get_watts_factor(d->udpkey);
		}
	}
	else {
		devices[_id].ev->set_slope(bike_kgs, person_kgs, _drag_factor, grade);
		devices[_id].rd.watts_factor = 100.0f;
	}
	return ALL_OK;
}							// SetSlope()
	//$

	/**********************************************************************

		returns:
			0 = OK

		errors:
			1 = thread not running
			2 = not a valid device

	**********************************************************************/

	//^ setPause

int setPause(const char *_id, bool _paused) {
	EnumDeviceType what;

	#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "setPause(%s, %s)\n", _id, _paused?"true":"false");
	fclose(logstream);
	#endif

	DEVICE *d;
	d = &devices[_id];

	if (d->commtype == TRAINER_IS_CLIENT) {
		if (server) {
			server->setPaused(d->udpkey, _paused);
		}
	}
	else {
		devices[_id].ev->set_paused(_paused);
	}

	/*
	what = devices[ix].what;
	if ((what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON)) {
		return GENERIC_ERROR;
	}
	bool p;
	*/

	return ALL_OK;
}										// int setPause()
//$

	/**********************************************************************
		portnum starts at 0
		stop : Bar controller exits Pro-E mode.
		Application hangs waiting for return from stop, 1 core pinned at 100% ultilization as if in an infinite wait loop.

	**********************************************************************/

	//^ stopTrainer

	int stopTrainer(const char *_id) {


#ifdef DO_ANT
		if (asi && !ant_closed) {
			ant_close();
		}
#endif


		if(devices[_id].commtype == TRAINER_IS_CLIENT) {

			/*
			if(sync(_id, devices[_id].udpkey) == 1) {
				return DEVICE_NOT_RUNNING;
			}
			//DEVICE *d = &devices[_id];			// this will create devices[_id] if it doesn't exist
			if(server) {
				//server->stop(d->udpkey);
				return ALL_OK;
			}
			*/

			DEVICE *d;
			ADJCODE status;
			status = sync(_id, devices[_id].udpkey);

			switch(status) {
				case ADJ_OK:
					return ALL_OK;

				case ADJ_UDPKEY_RESET:
					break;
				case ADJ_NOSERVER:												// no server
					return DEVICE_NOT_RUNNING;

				case 	ADJ_NO_DEVICES:
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					break;
				default:
					break;
			}								// switch
			return GENERIC_ERROR;
		}

		if(!devices[_id].ev) {
			return DEVICE_NOT_RUNNING;
		}

		EnumDeviceType what;
		what = GetRacerMateDeviceID(_id);

		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   %10ld: back from GetRacerMateDeviceID, calling ev->stop() and deleting ev\n", timeGetTime());
		FCLOSE(logstream);
		#endif

		devices[_id].ev->stop();
		DEL(devices[_id].ev);

		if(what == DEVICE_COMPUTRAINER || what == DEVICE_VELOTRON) {
			if(instances > 0) {
				instances--;
			}
		}
		return ALL_OK;
	}								// int stopTrainer(int ix)

	//$

	/**********************************************************************

	**********************************************************************/

	//^ ResettoIdle

	int ResettoIdle(const char *_id) {
		stopTrainer(_id);
		devices[_id].what = DEVICE_NOT_SCANNED;
		//devices[_id].port = 0;
		memset(devices[_id].id, 0, sizeof(devices[_id].id));
		devices[_id].when = 0L;
		memset(&devices[_id].nv, 0, sizeof(NV));
		devices[_id].last_control_byte = 0;
		devices[_id].hthread = 0;

		if (devices[_id].ds) {
			DEL(devices[_id].ds);
		}

		return ALL_OK;
	}						// int ResettoIdle(int ix)
	//$

#endif						// 	#if LEVEL >= DLL_CANNONDALE_ACCESS

#if LEVEL >= DLL_TRINERD_ACCESS

	/**********************************************************************

	**********************************************************************/

	//^ GetHandleBarButtons



	int GetHandleBarButtons(const char *_id, int fw)  {
		int i = 0;
	#ifdef _DEBUG
		int bp = 0;
	#endif

		if(devices[_id].commtype == TRAINER_IS_CLIENT) {

			/*
			if(sync(_id, devices[_id].udpkey) == 1) {
				return 0x40;
			}
			DEVICE *d = &devices[_id];			// this will create devices[_id] if it doesn't exist
			if(server) {
				i = server->get_handlebar_buttons(d->udpkey);
				//server->setPaused(d->udpkey, _paused);
			#ifdef _DEBUG
				if(i == 0x40) {
					bp = 2;
				}
			#endif
				return i;
			}
			return i;
			*/
			DEVICE *d;
			ADJCODE status;
			status = sync(_id, devices[_id].udpkey);

			switch(status) {
				case ADJ_OK:
					d = &devices[_id];			// this will create devices[_id] if it doesn't exist
					#ifdef _DEBUG
						if(memcmp(_id, "UDP-6666", 8)==0) {
							bp = 7;
						}
					#endif
					i = server->get_handlebar_buttons(d->udpkey);
					#ifdef _DEBUG
					if(i == 0x40) {
						if(memcmp(_id, "UDP-6666", 8) == 0) {
							bp = 7;
						}
					}
					#endif
					return i;

				case ADJ_UDPKEY_RESET:
					bp = 1;
					break;
				case ADJ_NOSERVER:												// no server
					bp = 1;
					break;
				case 	ADJ_NO_DEVICES:
					bp = 1;
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					bp = 1;
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					bp = 1;
					break;
				default:
					bp = 1;
					break;
			}								// switch

		#ifdef _DEBUG
			if(memcmp(_id, "UDP-6666", 8) == 0) {
				bp = 7;
			}
		#endif

			return 0x40;

		}

		int keys = 0;
		int newkeys = 0;
		int mask;
		EnumDeviceType what;

		//devices[_id].ev->set_paused(_paused);
		if(!devices[_id].ev) {
			return 0;
		}
		what = devices[_id].ev->getwhat();

		if(what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON) {
			return 0;
		}




	#ifdef _DEBUG
		if (what==DEVICE_COMPUTRAINER)  {
			switch(fw)  {
				case 4095:
				case 144:
				case 3611:
					break;
				default:
					return BAD_FIRMWARE_VERSION;
			}
		}
		else if (what==DEVICE_VELOTRON)  {
			if (fw != 190)  {
				return BAD_FIRMWARE_VERSION;
			}
		}
	#endif

		keys = devices[_id].ev->get_keys();							// get the ACCUMULATED keys since last called

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

			if (!devices[_id].ev->is_connected_x())  {
				return 0x40;
			}

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
	//$

	/**********************************************************************

	**********************************************************************/

	//^ SetErgModeLoad

	int SetErgModeLoad(const char *_ix, int _fw, int _rrc, float _manwatts) {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "SetErgModeLoad(%s)\n", _ix);
			fclose(logstream);
		#endif
			int status;
		return ALL_OK;
	}							// int

	//$
	/**********************************************************************
		SetHRBeepBounds
	**********************************************************************/

	//^ SetHRBeepBounds


	int SetHRBeepBounds(const char *_id, int FirmwareVersion, int LowBound, int HighBound, bool BeepEnabled) {
		#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "SetHRBeepBounds(%s)\n", _id);
		fclose(logstream);
		#endif

	
		DEVICE *d;
		d = &devices[_id];

		EnumDeviceType what, what2;
		what = devices[_id].what;

		if (what == DEVICE_COMPUTRAINER)  {
		}
		else if (what == DEVICE_VELOTRON )  {
		}
		else  {
			return WRONG_DEVICE;
		}

		devices[_id].rd.lower_hr = (float)LowBound;
		devices[_id].rd.upper_hr = (float)HighBound;
		devices[_id].beep_enabled = BeepEnabled;

		if(d->commtype == TRAINER_IS_CLIENT) {
			if(server) {
				server->set_hr_bounds(d->udpkey, LowBound, HighBound, BeepEnabled);
			}
			return ALL_OK;
		}


		if (!devices[_id].ev)  {
			return ALL_OK;
		}

		what2 = devices[_id].ev->getwhat();
		if (what != what2)  {
			return GENERIC_ERROR;
		}

		devices[_id].ev->set_hr_bounds_x(LowBound, HighBound, BeepEnabled);			// always returns 0

		return ALL_OK;
	}						// int SetHRBeepBounds(int ix, int FirmwareVersion, int LowBound, int HighBound, bool BeepEnabled)  {
	//$

	/**********************************************************************

	**********************************************************************/

	//^ SetRecalibrationMode


	int SetRecalibrationMode(const char *_id, int fw) {

#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "SetRecalibrationMode(%s)\n", ix);
		fclose(logstream);
#endif

		if(devices[_id].commtype == TRAINER_IS_CLIENT) {

			/*
			if(sync(_id, devices[_id].udpkey) == 1) {
				return DEVICE_NOT_RUNNING;
			}
			if(server) {
				int status = server->start_cal(devices[_id].udpkey);
				if(status != -1) {
					return ALL_OK;
				}
			}
			else {
				return DEVICE_NOT_RUNNING;
			}
			return DEVICE_NOT_RUNNING;
			*/
			DEVICE *d;
			ADJCODE status;
			status = sync(_id, devices[_id].udpkey);

			switch(status) {
				case ADJ_OK:
				{
					int i = server->start_cal(devices[_id].udpkey);
					if(i != -1) {
						return ALL_OK;
					}
					break;
				}
				case ADJ_UDPKEY_RESET:
					break;
				case ADJ_NOSERVER:												// no server
					break;
				case 	ADJ_NO_DEVICES:
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					break;
				default:
					break;
			}								// switch
			return DEVICE_NOT_RUNNING;
		}



		EnumDeviceType what;
		const char *comport = _id;


		if (!devices[_id].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		what = devices[_id].what;

		if ( !haveDevice(what, comport) )  {
			what = GetRacerMateDeviceID(comport-1);
			if ( !haveDevice(what, comport) )  {
				return GENERIC_ERROR;
			}
		}

		if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
			what = check_for_trainers(_id);
			if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
				return GENERIC_ERROR;
			}
		}

		if (what==DEVICE_COMPUTRAINER)  {
			//throw(fatalError(__FILE__, __LINE__));
			devices[_id].last_control_byte = devices[_id].ev->start_cal();
			calibrating = true;
			return ALL_OK;
		}
		else if (what==DEVICE_VELOTRON)  {
			//throw(fatalError(__FILE__, __LINE__));
 			calibrating = true;
			int status = devices[_id].last_control_byte = devices[_id].ev->start_cal();
			if(status != -1) {
				return ALL_OK;
			}
		}

		return GENERIC_ERROR;
	}					// int SetRecalibrationMode(int ix, int fw)
	//$

	/**********************************************************************

	**********************************************************************/

	//^ EndRecalibrationMode

	int EndRecalibrationMode(const char *_id, int fw) {
		EnumDeviceType what;
		calibrating = false;

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "EndRecalibrationMode(%s)\n", ix);
			fclose(logstream);
		#endif

			if(devices[_id].commtype == TRAINER_IS_CLIENT) {
				/*
				if(sync(_id, devices[_id].udpkey) == 1) {
					return DEVICE_NOT_RUNNING;
				}
				if(server) {
					int status = server->end_cal(devices[_id].udpkey);
					if(status != -1) {
						return ALL_OK;
					}
				}
				else {
					return DEVICE_NOT_RUNNING;
				}
				return DEVICE_NOT_RUNNING;
				*/
				DEVICE *d;
				ADJCODE status;
				status = sync(_id, devices[_id].udpkey);

				switch(status) {
					case ADJ_OK:
					{
						int i = server->end_cal(devices[_id].udpkey);
						if(i != -1) {
							return ALL_OK;
						}
						break;
					}
					case ADJ_UDPKEY_RESET:
						break;
					case ADJ_NOSERVER:												// no server
						break;
					case 	ADJ_NO_DEVICES:
						break;
					case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
						break;
					case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
						break;
					default:
						break;
				}								// switch
				return DEVICE_NOT_RUNNING;

			}


		const char *comport = _id;
		if (!devices[_id].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		what = devices[_id].what;


		if ( !haveDevice(what, comport) )  {
			what = GetRacerMateDeviceID(_id);
			if ( !haveDevice(what, comport) )  {
				return GENERIC_ERROR;
			}
		}

		if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
			what = check_for_trainers(_id);
			if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
				return WRONG_DEVICE;
			}
		}

		if (what==DEVICE_COMPUTRAINER)  {
			devices[_id].ev->end_cal(devices[_id].last_control_byte);

			const char *cptr = get_computrainer_cal_from_thread(_id, fw);				// "2.00 U"
			strncpy(devices[_id].nv.cal, cptr, sizeof(devices[_id].nv.cal) );
		}
		else if (what==DEVICE_VELOTRON)  {
			devices[_id].ev->end_cal(devices[_id].last_control_byte);
			const char *cptr = get_velotron_cal_from_thread(_id, fw);					// "2.00 U"
			strncpy(devices[_id].nv.cal, cptr, sizeof(devices[_id].nv.cal) );

		}

		int cal = GetCalibration(_id);

		return cal;
	}						// int EndRecalibrationMode(int ix, int fw)

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_computrainer_mode

	int get_computrainer_mode(const char *ix) {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_computrainer_mode()\n");
			fclose(logstream);
		#endif

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
	//$

#endif								// #if LEVEL >= DLL_TRINERD_ACCESS 

#if LEVEL >= DLL_ERGVIDEO_ACCESS

/**********************************************************************

**********************************************************************/

//^ get_status_bits

int get_status_bits(const char *_id, int fw) {

	if(devices[_id].commtype == TRAINER_IS_CLIENT) {
		/*
		if(sync(_id, devices[_id].udpkey) == 1) {
			return -1;
		}
		DEVICE *d = &devices[_id];			// this will create devices[_id] if it doesn't exist
		if(server) {
			return server->get_status_bits(d->udpkey);
		}
		return -1;
		*/

		DEVICE *d;
		ADJCODE status;
		status = sync(_id, devices[_id].udpkey);

		switch(status) {
			case ADJ_OK:
				d = &devices[_id];			// this will create devices[_id] if it doesn't exist
				return server->get_status_bits(d->udpkey);
			case ADJ_UDPKEY_RESET:
				break;
			case ADJ_NOSERVER:												// no server
				break;
			case 	ADJ_NO_DEVICES:
				break;
			case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
				break;
			case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
				break;
			default:
				break;
		}								// switch
		return -1;

	}


	if (!devices[_id].ev)  {
		return -1;
	}
	return devices[_id].ev->get_status_bits_x();
	//return 0;
}						// int get_status_bits(int ix, int fw)  {

//$

	/**********************************************************************

	**********************************************************************/

	//^ get_np


float get_np(const char *ix, int fw)  {
		if (!devices[ix].ev)  {
			return -1.0f;
		}
		return devices[ix].ev->get_x_np();
	}										// float get_np(int ix, int fw)  {

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_if


float get_if(const char *ix, int fw) {
		if (!devices[ix].ev)  {
			return -1.0f;
		}
		return devices[ix].ev->get_if_x();
	}							// float get_if(int ix, int fw)  {

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_tss

	float get_tss(const char * ix, int fw)  {

		if (!devices[ix].ev)  {
			return -1.0f;
		}
		return devices[ix].ev->get_tss_x();
	}					// float get_tss(int ix, int fw)  {

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_calories

	float get_calories(const char * ix, int fw)  {

		if (!devices[ix].ev)  {
			return -1.0f;
		}

		return devices[ix].ev->get_calories_x();

	}				// float get_calories()

	//$

/**********************************************************************

**********************************************************************/

//^ get_pp

float get_pp(const char * ix, int fw)  {

	if (!devices[ix].ev)  {
		return -1.0f;
	}
	return devices[ix].ev->get_pp_x();
}					// float get_pp(int ix, int fw)  {

//$


#ifdef _DEBUG
#ifdef DO_ANT
#if defined(WIN32)
	extern char channel_status_string[4][32];

	const char *get_state(int _chan)  {
		if (_chan < 0 || _chan > ANT_SC)  {
			return "nil";
		}

		int status;

		switch(_chan)  {
			case ANT_HR:
				status = hr.channel_status;
				break;
			case ANT_C:
				status = cad.channel_status;
				break;
			case ANT_SC:
				status = sc.channel_status;
				break;
		}

		return channel_status_string[status];
	}
#endif
#endif							// #ifdef DO_ANT
#endif


/**********************************************************************

**********************************************************************/

	//^ SetVelotronParameters

	int SetVelotronParameters(const char * ix, int FWVersion, int _nfront, int _nrear, int* Chainrings, int* cogset, float tire_diameter_mm, int ActualChainring, int Actualcog, float bike_kgs, int front_index, int rear_index)  {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "SetVelotronParameters()\n");
			fclose(logstream);
		#endif

		int i;

		if (_nfront>5 || _nrear>20)  {
			return BAD_GEAR_COUNT;
		}

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "SetVelotronParameters(ix)\n");
		fclose(logstream);
	#endif

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

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   OK1, _nfront = %d\n", _nfront);
		fclose(logstream);
	#endif


		for(i=0; i<_nfront; i++)  {
			if (Chainrings[i]<=0)  {
				return BAD_TEETH_COUNT;
			}
			devices[ix].bike_params.front_gears[i] = Chainrings[i];
		}

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   OK2\n");
		fclose(logstream);
	#endif

		for(i=0; i<_nrear; i++)  {
			if (cogset[i]<=0)  {
				return BAD_TEETH_COUNT;
			}
			devices[ix].bike_params.rear_gears[i] = cogset[i];
		}


	#ifdef LOGDLL
		//dump_bike_params(ix);
	#endif

		return ALL_OK;
	}								// SetVelotronParameters()

	//$

	/**********************************************************************

	**********************************************************************/

	//^ GetCurrentVTGear

	struct Bike::GEARPAIR GetCurrentVTGear(const char * ix, int fw)  {

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

	//$
	/**********************************************************************

	**********************************************************************/

	//^ setGear

	int setGear(const char * ix, int FWVersion, int front_index, int rear_index)  {

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "setGear(%s)\n", ix);
			fclose(logstream);
		#endif

		if (!devices[ix].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		devices[ix].ev->set_gear(front_index, rear_index);

		return ALL_OK;
	}						// setGear()

	//$
	/**********************************************************************

	**********************************************************************/
	
	//^ get_bars
	
	float * get_bars(const char * _id, int FWVersion)  {
		float *fptr;

		if(devices[_id].commtype == TRAINER_IS_CLIENT) {
			/*
			if(sync(_id, devices[_id].udpkey) == 1) {
				return nobars;
			}
			DEVICE *d = &devices[_id];			// this will create devices[_id] if it doesn't exist
			if(server) {
				fptr = server->get_bars(d->udpkey);
				return fptr;
			}
			return nobars;
			*/
			DEVICE *d;
			ADJCODE status;
			status = sync(_id, devices[_id].udpkey);

			switch(status) {
				case ADJ_OK:
					d = &devices[_id];			// this will create devices[_id] if it doesn't exist
					fptr = server->get_bars(d->udpkey);
					return fptr;
				case ADJ_UDPKEY_RESET:
					break;
				case ADJ_NOSERVER:												// no server
					break;
				case 	ADJ_NO_DEVICES:
					break;
				case ADJ_HAVE_UDPKEY_BUT_NQTCLIENTS_NOT_EQUAL_TO_NDEVICES:
					break;
				case ADJ_GET_UDPKEY_FROM_DEVNUM_RETURNED_NULL:
					break;
				default:
					break;
			}								// switch
			return nobars;

		}

		if (!devices[_id].ev)  {
			return nobars;
		}

		fptr = devices[_id].ev->get_bars_x();
		return fptr;
	}								// get_bars()
	//$

	/**********************************************************************
		int ResetAlltoIdle(void)  {
	**********************************************************************/

	//^ ResetAlltoIdle

	int ResetAlltoIdle(void)  {
		int i;

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "\nResetAlltoIdle()\n");
			fclose(logstream);
		#endif

		return ALL_OK;
	}							// int ResetAlltoIdle(void)  {
	//$

	/**********************************************************************
		sets the 'started' flag in decoder so that distance, speed
		and things that depend on them are calculated.
	**********************************************************************/

	//^ start_trainer

	int start_trainer(const char * ix, bool _b)  {
		int realport;

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "start_trainer(%s)\n", ix);
			fclose(logstream);
		#endif

		if (!devices[ix].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		realport = 1;

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
			if ( haveDevice(what, ix) )  {
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

		devices[ix].ev->start(_b);

		return ALL_OK;
	}							// start_trainer
	//$

/**********************************************************************

**********************************************************************/

//^ set_ftp

int set_ftp(const char * ix, int fw, float _ftp)  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "set_ftp()\n");
			fclose(logstream);
		#endif

	EnumDeviceType what;



	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "set_ftp(%s)\n", ix);
		fclose(logstream);
	#endif


	what = devices[ix].what;
	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		return WRONG_DEVICE;
	}

	devices[ix].rd.ftp = _ftp;

	//return ALL_OK;
	return DEVICE_NOT_RUNNING;
}									// int set_ftp(int ix, int fw, float _ftp)  {

//$

/**********************************************************************

**********************************************************************/

//^ set_wind

int set_wind(const char * _id, int fw, float _wind_kph)  {
	#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "set_wind()\n");
	fclose(logstream);
	#endif

	DEVICE *d;
	d = &devices[_id];

	if (d->commtype == TRAINER_IS_CLIENT) {
		if (server) {
			server->setwind(d->udpkey, _wind_kph);
		}
		else {
			return DEVICE_NOT_RUNNING;
		}
	}
	else {
		if (d->ev) {
			d->ev->set_wind_x(_wind_kph);
		}
		else {
			return DEVICE_NOT_RUNNING;
		}
	}

	return ALL_OK;
}									// int set_wind

//$

/**********************************************************************

**********************************************************************/

//^ set_draftwind

int set_draftwind(const char * _id, int fw, float _draft_wind_kph)  {
#ifdef LOGDLL
	logstream = fopen(DLL_LOGFILE, "a+t");
	fprintf(logstream, "set_draft_wind()\n");
	fclose(logstream);
#endif

	DEVICE *d;
	d = &devices[_id];

	if (d->commtype == TRAINER_IS_CLIENT) {
		if (server) {
			server->set_draftwind(d->udpkey, _draft_wind_kph);
		}
		else {
			return DEVICE_NOT_RUNNING;
		}
	}
	else {
		if (d->ev) {
			d->ev->set_draft_wind_x(_draft_wind_kph);
		}
		else {
			return DEVICE_NOT_RUNNING;
		}
	}

	return ALL_OK;
}							// int set_draftwind(int ix, int fw, float _draft_wind_kph)  {

//$

/**********************************************************************

**********************************************************************/

//^ update_velotron_current

int update_velotron_current(const char * ix, unsigned short pic_current)  {
		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "update_velotron_current()\n");
			fclose(logstream);
		#endif

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


//$

/**********************************************************************

**********************************************************************/

//^ get_udp_clients

const char *get_udp_trainers(void)  {

	if(server) {
		return server->get_udpclient_names();
	}

	return "";
}								// get_udp_clients()

/**********************************************************************

**********************************************************************/

int get_n_udp_clients(void) {

	if(server) {
		return server->get_nclients();
	}

	return 0;
}

/**********************************************************************

**********************************************************************/

int get_n_devices(void) {
	return devices.size();
}								// get_udp_clients()

/**********************************************************************

**********************************************************************/

//^ GetPortNames

const char * GetPortNames(void)  {
	char str[32];
	static char portnames[48*8];										// allocate memory for up to 48 com port names
	memset(portnames, 0, sizeof(portnames));

#ifdef WIN32
	//-------------------------------------------------
	// enumerate the serial ports from the registry:
	//-------------------------------------------------

	char sKeyName[MAX_PATH];
	char sKeyValue[MAX_PATH];
	HKEY key;
	int bp = 0;

	if (RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			"Hardware\\DeviceMap\\SerialComm",
			0,
			KEY_QUERY_VALUE,
			&key) == ERROR_SUCCESS) {

		LONG retval = ERROR_SUCCESS;
		DWORD index = MAXDWORD;
		DWORD nameSize;
		DWORD valSize;
		DWORD type;

		int ncomports = 0;

		//logg->write("\ncomm ports listed in registry:\n");

		while (retval == ERROR_SUCCESS)  {
			++index;
			nameSize = MAX_PATH;
			valSize = MAX_PATH;
			retval = RegEnumValue(
							key,
							index,
							sKeyName,
							&nameSize,
							NULL,
							&type,
							(unsigned char *)sKeyValue,
							&valSize);

			if (retval == ERROR_SUCCESS)  {
				bp = 1;
				ncomports++;
				if (ncomports==15)  {
					throw(fatalError(__FILE__, __LINE__, "ncomports = 15"));
				}

				// sKeyValue now contains one of the COM ports available  on the machine
				strcat(portnames, sKeyValue);
				strcat(portnames, " ");
				//logg->write("%s\n", sKeyValue);
			}
			else  {
				bp = 2;
			}
		}
	}
	else  {
		//logg->write(10,0,1,"error opening registry\n");
		bp = 3;
	}

	if (RegCloseKey(key) != ERROR_SUCCESS)  {
		//logg->write(10,0,1,"error closing registry\n");
	}

	//strcat(portnames, SERVER_PORT_STR);												// server 'serial' porg
	
#if 1
	int i;
	/*
	for(i=0; i<15; i++)  {
		sprintf(str, "COM%d ", CLIENT_SERIAL_PORT_BASE+i);			// trainer is a server
		strcat(portnames, str);												// client 'serial' porg
	}
	sprintf(str, "COM%d", CLIENT_SERIAL_PORT_BASE+i);				// trainer is a server
	strcat(portnames, str);													// client 'serial' porg
	*/
	//strcat(portnames, " COM221");
	//strcat(portnames, " COM222");
#else
		for(i=0; i<15; i++)  {
			sprintf(str, "COM%d ", SERVER_SERIAL_PORT_BASE+i);			// trainer is a client
			strcat(portnames, str);												// client 'serial' porg
		}
		sprintf(str, "COM%d ", SERVER_SERIAL_PORT_BASE+i);				// trainer is a client
		strcat(portnames, str);													// client 'serial' porg
#endif



#else
	strcpy(str, "/dev/ttyUSB0");
	strcat(portnames, str);												// client 'serial' porg
#endif									// WIN32

	trim(portnames);


	if (server) {
		const char *ccptr = server->get_devnums();
		strcat(portnames, " ");
		strcat(portnames, ccptr);
	}

	trim(portnames);
	ltrim(portnames);

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "getportnames: \"%s\"\n", portnames);
		fclose(logstream);
	#endif

	return portnames;
}									// const char * GetPortNames(void)

//$


#endif									// #if LEVEL >= DLL_ERGVIDEO_ACCESS

#if LEVEL >= DLL_FULL_ACCESS

	/**********************************************************************

	**********************************************************************/

	//^ is_running
	bool is_running(int ix)  {

		if (devices[ix].ev)  {
			return true;
		}
		return false;
	}
	//$


	/**********************************************************************

	**********************************************************************/

	//^ get_slip

	float get_slip(int ix, int fw)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->get_slip_x();
		}

		return 0.0f;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_slipflag

	bool get_slipflag(int ix, int fw)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->get_slipflag_x();
		}

		return false;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_finish_ms

	unsigned long get_finish_ms(int ix, int fw)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->get_finish_ms_x();
		}

		return 0L;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_shiftflag

	bool get_shiftflag(int ix)  {

		if (!devices[ix].ev)  {
			return false;
		}

		return devices[ix].ev->get_shiftflag_x();

	}				// bool get_shiftflag()
	//$

	/**********************************************************************
	METADATA *get_meta(int ix)  {
		METADATA *meta=NULL;


		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_meta(%d)\n", ix);
			fclose(logstream);
		#endif

		if (devices[ix].ev)  {
			meta = devices[ix].ev->get_meta();
		}

		return meta;
	}							// get_meta()

	**********************************************************************/

	//^ get_raw_rpm

	float get_raw_rpm(int ix, int fw)  {

		//if (devices[ix].ds)  {
		if (devices[ix].ev)  {
			//return devices[ix].ev->get_raw_rpm();
			float f = devices[ix].ev->get_decoder_x()->raw_rpm;
			return (f + 7.0f);
		}

		return 0.0f;
	}
	//$


	/**********************************************************************

	**********************************************************************/

	//^ clear_slipflag

	void clear_slipflag(int ix, int fw)  {

		if (devices[ix].ev)  {
			devices[ix].ev->clear_slipflag_x();
		}

		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_meta

	METADATA *get_meta(int ix)  {

		METADATA *meta=NULL;


		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_meta(%d)\n", ix);
			fclose(logstream);
		#endif

		if (devices[ix].ev)  {
			meta = devices[ix].ev->get_meta_x();
		}

		return meta;
	}							// get_meta()
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_decoder

	Decoder *get_decoder(int ix)  {

		Decoder *d=NULL;


		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "get_decoder(%d)\n", ix);
			fclose(logstream);
		#endif

		if (devices[ix].ev)  {
			d = devices[ix].ev->get_decoder_x();
		}

		return d;
	}							// get_decoder()
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_control_byte

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
	//$


	/**********************************************************************

	**********************************************************************/

	//^ get_physics_mode

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
	//$


	/**********************************************************************

	**********************************************************************/

	//^ get_manual_watts

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

		watts = devices[ix].ev->get_manual_watts_x();

		return watts;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_slope

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

		slope = devices[ix].ev->get_grade_x();

		return slope;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_gear_indices

	struct Bike::GEARPAIR get_gear_indices(int ix)  {

		int portnum;

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
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

		return devices[ix].ev->get_gear_indices_x();

	}				// struct Bike::GEARPAIR get_gear_indices(int ix)
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_constant_force

	int set_constant_force(int ix, float _force)  {

		DEVICE *d;

		if (devices[ix].what != DEVICE_VELOTRON)  {
			return WRONG_DEVICE;
		}

		d = &devices[ix];

		if (!d->ev)  {
			return DEVICE_NOT_RUNNING;
		}

		d->ev->set_constant_force_x(_force);

		return ALL_OK;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_physics

	Physics *get_physics(int ix)  {

		DEVICE *d;

		if (devices[ix].what != DEVICE_VELOTRON)  {
			return NULL;
		}

		d = &devices[ix];

		if (!d->ev)  {
			return NULL;
		}

		return d->ev->get_physics_x();

	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_packet_error1

	unsigned long get_packet_error1(int ix)  {

		if (devices[ix].what != DEVICE_COMPUTRAINER)  {
			return 0;
		}

		DEVICE *d;

		d = &devices[ix];

		if (!d->ev)  {
			return 0;
		}

		return d->ev->get_packet_error1_x(ix);

		return 0;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_packet_error2

	unsigned long get_packet_error2(int ix)  {

		if (devices[ix].what != DEVICE_COMPUTRAINER)  {
			return 0;
		}

		DEVICE *d;

		d = &devices[ix];

		if (!d->ev)  {
			return 0;
		}

		return d->ev->get_packet_error2_x(ix);

		return 0;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_packet_error3

	unsigned long get_packet_error3(int ix)  {

		if (devices[ix].what != DEVICE_COMPUTRAINER)  {
			return 0;
		}

		DEVICE *d;

		d = &devices[ix];

		if (!d->ev)  {
			return 0;
		}

		return d->ev->get_packet_error3_x(ix);

		return 0;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_packet_error4

	unsigned long get_packet_error4(int ix)  {

		if (devices[ix].what != DEVICE_COMPUTRAINER)  {
			return 0;
		}

		DEVICE *d;

		d = &devices[ix];

		if (!d->ev)  {
			return 0;
		}

		return d->ev->get_packet_error4_x(ix);

		return 0;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ is_started

	bool is_started(int ix)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->is_started_x();
		}

		return false;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ is_paused

	bool is_paused(int ix)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->is_paused_x();
		}

		return false;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_gloger

	void set_gloger(Logger *_glog)  {
		if (glog==NULL)  {
			glog = _glog;
		}

		return;
	}
	//$


	/**********************************************************************

	**********************************************************************/

	//^ is_finished

	bool is_finished(int ix)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->is_finished_x();
		}

		return false;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_logging

	int set_logging(int ix, int fw, LoggingType _logging_type)  {

		EnumDeviceType what=DEVICE_NOT_SCANNED;
		int comport = ix + 1;

		what = devices[ix].what;

		if (what==DEVICE_SIMULATOR)  {
			return ALL_OK;
		}

		if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
			what = check_for_trainers(ix, 0);
			if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
				return WRONG_DEVICE;
			}
		}

		devices[ix].logging_type = _logging_type;

		if (devices[ix].ev)  {
			devices[ix].ev->set_logging_x(_logging_type);
		}

		return ALL_OK;
	}
	//$

	/**********************************************************************
		portnum starts at 0
		stop : Bar controller exits Pro-E mode.
		Application hangs waiting for return from stop, 1 core pinned at 100% ultilization as if in an infinite wait loop.

	**********************************************************************/

	//^ get_perf_file_name

	const char *get_perf_file_name(int ix)  {

		const char *cptr;

		if (!devices[ix].ev)  {
			return NULL;
		}

		cptr = devices[ix].ev->get_perf_file_name_x();

		return cptr;
	}
	//$


	/**********************************************************************

	**********************************************************************/

	//^ set_file_mode

	void set_file_mode(int ix, bool _mode)  {

		devices[ix].file_mode = _mode;
		gfile_mode = _mode;
		if (devices[ix].ev)  {
			devices[ix].ev->set_file_mode_x(_mode);
		}

		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_performance_file_name

	void set_performance_file_name(int ix, const char *_perfname)  {

		if (!devices[ix].ev)  {
			return;
		}

		devices[ix].ev->set_performance_file_name_x(_perfname);

		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_rider_data

	int set_rider_data(int ix, RIDER_DATA _rd)  {

		if (ix<0 || ix>255)  {
			return BAD_RIDER_INDEX;
		}

		devices[ix].rd = _rd;

		return ALL_OK;
	}
	//$




	/**********************************************************************

	**********************************************************************/

	//^ get_instances

	int get_instances(void)  {

		return instances;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_bluetooth_delay

	void set_bluetooth_delay(bool _b)  {

		bluetooth_delay = _b;
		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_logger

	void set_logger(Logger *_logg)  {

		glog = _logg;
		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_manual_speed

	int set_manual_speed(int ix, int fw, float _manual_mph)  {

		DEVICE *d;

		d = &devices[ix];

		if (!d->ev)  {
			return DEVICE_NOT_RUNNING;
		}

		d->ev->set_manual_speed_x(_manual_mph);

		return ALL_OK;

	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ getds

	dataSource *getds(int ix, int fw)  {

		if (devices[ix].ev)  {
			return devices[ix].ev->getds_x();
		}

		return NULL;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ set_dirs

	int set_dirs(std::vector<std::string> &_dirs)  {

		int i, n;

		static int cnt = 0;
		FILE *stream = NULL;

		cnt++;

	//#if 0
	#ifdef _DEBUG
		stream = fopen("sd.txt", "wt");
		if (stream==NULL)  {
			return CAN_NOT_OPEN_FILE;
		}
	#endif

	if (stream) fprintf(stream, "sd  %d\n", cnt);
		n = (int)dirs.size();
	if (stream) fprintf(stream, "n = %d\n", n);

		for(i=0; i<n; i++)  {
			dirs.push_back(_dirs[i].c_str());
			if (stream) fprintf(stream, "%s\n", _dirs[i].c_str());
		}

		n = (int)dirs.size();
		if (stream)  {
			fprintf(stream, "n = %d\n", n);
			FCLOSE(stream);
		}

		return ALL_OK;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ load_perf

	int load_perf(int ix, int fw, const char *_fname)  {

		int status;

		if (!devices[ix].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		EnumDeviceType what;
		what = GetRacerMateDeviceID(ix);

		status = devices[ix].ev->load_x(_fname);
		if (status != 0)  {
			return GENERIC_ERROR;
		}

		return ALL_OK;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ reset_encryptor

	void reset_encryptor(void)  {

		cryptor.init();
		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ encrypt

	void encrypt(unsigned char *buf, int len)  {
		cryptor.doo(buf, len);
		return;
	}
	//$

	/**********************************************************************

	**********************************************************************/

	//^ decrypt

	void decrypt(unsigned char *buf, int len)  {
		cryptor.doo(buf, len);
		return;
	}
	//$



	/**********************************************************************

	**********************************************************************/

	//^ startTrainer2

	int startTrainer2(int ix, Course *_course, LoggingType _logging_type)  {

		int port;
		//int i;

		#ifdef LOGDLL
			logstream = fopen(DLL_LOGFILE, "a+t");
			fprintf(logstream, "startTrainer2(%d)\n", ix);
			fclose(logstream);
		#endif

		devices[ix].logging_type = _logging_type;

		if (devices[ix].ev)  {				// this trainer is already started
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   returning trainer already started\n");
				fclose(logstream);
			#endif
			return DEVICE_ALREADY_RUNNING;
		}

		devices[ix].course = _course;

		port = ix + 1;				// 1 = exists, 3 = velotron, 4 = computrainer

		EnumDeviceType what;

		if (devices[ix].what == DEVICE_NOT_SCANNED)  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   device not scanned\n");
				fclose(logstream);
			#endif
			what = GetRacerMateDeviceID(ix);

			if (what==DEVICE_DOES_NOT_EXIST)  {
				#ifdef LOGDLL
					logstream = fopen(DLL_LOGFILE, "a+t");
					fprintf(logstream, "   returning device does not exist: %d\n", DEVICE_DOES_NOT_EXIST);
					fclose(logstream);
				#endif
				return DEVICE_DOES_NOT_EXIST;
			}
			else if (what==DEVICE_OPEN_ERROR)  {
				#ifdef LOGDLL
					logstream = fopen(DLL_LOGFILE, "a+t");
					fprintf(logstream, "   returning port open error: %d\n", PORT_OPEN_ERROR);
					fclose(logstream);
				#endif
				return PORT_OPEN_ERROR;
			}
		}
		else if (devices[ix].what == DEVICE_DOES_NOT_EXIST)  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   device was already scanned\n");
				fclose(logstream);
			#endif

			what = GetRacerMateDeviceID(ix);
			if ( haveDevice(what, port) )  {

				#ifdef LOGDLL
					logstream = fopen(DLL_LOGFILE, "a+t");
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
				#ifdef LOGDLL
					logstream = fopen(DLL_LOGFILE, "a+t");
					fprintf(logstream, "   returning device does not exist 3: %d\n", DEVICE_DOES_NOT_EXIST);
					fclose(logstream);
				#endif
				return DEVICE_DOES_NOT_EXIST;
			}

		}

		what = devices[ix].what;

	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   ok1\n");
		fclose(logstream);
	#endif

		if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
			instances++;
		}

		if (what==DEVICE_COMPUTRAINER)  {
			//EV(int _realportnum, EnumDeviceType _what, Bike::PARAMS *_params=NULL, Course *_course=NULL);
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   ok2a: device is computrainer\n");
				fclose(logstream);
			#endif

			// devices[ix].rd is the rider data
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
			//ds = getds();

			devices[ix].ev->set_file_mode_x(gfile_mode);


			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   ok2b\n");
				fclose(logstream);
			#endif
		}
		else if (what==DEVICE_VELOTRON)  {
			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   ok2\n");
				fclose(logstream);
			#endif

			if (devices[ix].bike_params.real_front_teeth==0)  {					// ActualChainRing?
				#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   startTrainer2 error:\n");
				fclose(logstream);

				dump_bike_params(ix);
				#endif
				return VELOTRON_PARAMETERS_NOT_SET;
			}

			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   ok6, realport = %d\n", realport);
				fclose(logstream);
			#endif

			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   starting thread\n");
				fclose(logstream);
				dump_bike_params(ix);
			#endif

			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_VELOTRON, devices[ix].rd, &devices[ix].bike_params, _course, devices[ix].logging_type);

			devices[ix].ev->set_logging_x(devices[ix].logging_type);

			#ifdef LOGDLL
				logstream = fopen(DLL_LOGFILE, "a+t");
				fprintf(logstream, "   back from thread start\n");
				fclose(logstream);
				dump_bike_params(ix);
			#endif
		}
		else if (what==DEVICE_SIMULATOR)  {
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_SIMULATOR, devices[ix].rd, NULL, _course);
		}
		else if (what==DEVICE_RMP)  {
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_RMP, devices[ix].rd, NULL, NULL);
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


		devices[ix].ev->set_hr_bounds_x( ROUND(devices[ix].rd.lower_hr), ROUND(devices[ix].rd.upper_hr), devices[ix].beep_enabled);
		devices[ix].ev->set_ftp_x(devices[ix].rd.ftp);


	#ifndef DO_EV_EVENT
		// wait  to make sure that the thread is running:	while (!devices[ix].ev->get_running())  {
		}
	#endif
	#ifdef LOGDLL
		logstream = fopen(DLL_LOGFILE, "a+t");
		fprintf(logstream, "   startTrainer2x\n");
		fclose(logstream);
		dump_bike_params(ix);
	#endif

		// the trainer is running, but decoder::started is false... you need to 'start' it if you want the decoder to accumulate time and distance.

		return ALL_OK;
	}							// startTrainer2(int ix, Course *_course, LoggingType _logging_type)  {
	//$







	/**********************************************************************
		racermate_init
	**********************************************************************/
	/*
	//^ racermate_init

	int racermate_init(void)  {
		memset(gstring, 0, sizeof(gstring));
		portname_to_ix.clear();
		ix_to_portname.clear();
		return ALL_OK;
	}							// racermate_init()
	//$
	*/



	/**********************************************************************

	**********************************************************************/

	//^ is_connected
	bool is_connected(void)  {
		return connected;
	}							// bool is_connected()
	//$



	/**********************************************************************

	**********************************************************************/

	//^ get_accum_tdc

	int	get_accum_tdc(int ix, int fw)  {

		if (ix<0 || ix>255)  {
			return 0;
		}

		if (!devices[ix].ev)  {
			return 0;
		}

		int tdc = devices[ix].ev->get_accum_tdc_x();							// get the ACCUMULATED pulses since last specifically cleared by RENDERER!
		devices[ix].ev->clear_accum_tdc_x();

		return tdc;
	}									// int	get_accum_tdc(int ix, int fw)  {

	//$

	/**********************************************************************
		this is the processed accum_tdc
	**********************************************************************/

	//^ get_tdc

	int get_tdc(int ix, int fw)  {

		if (ix<0 || ix>255)  {
			return 0;
		}

		if (!devices[ix].ev)  {
			return 0;
		}

		int tdc = devices[ix].ev->get_accum_tdc_x();							// get the ACCUMULATED pulses since last specifically cleared by RENDERER!
		devices[ix].ev->clear_accum_tdc_x();

		return tdc;
	}								// int get_tdc(int ix, int fw)  {

	//$











/**********************************************************************

**********************************************************************/

//^ set_velotron_calibration

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
}								// int set_velotron_calibration(int ix, int fw, int _cal)  {

//$

/**********************************************************************
	enters here when mph falls from 23.0 mph to 22.0 mph
**********************************************************************/

//^ velotron_calibration_spindown

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

#ifdef WIN32
	unsigned short pic_current = 0;
#endif

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

//$

/**********************************************************************
	timeout is in milliseconds
**********************************************************************/

//^ set_timeout

void set_timeout(unsigned long _ms)  {

	DEVICE_TIMEOUT_MS = _ms;
	return;
}

//$

#endif		//	#if LEVEL >= DLL_FULL_ACCESS


