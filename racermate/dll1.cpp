#define WIN32_LEAN_AND_MEAN

#include "levels.h"

/**********************************************************************************************************

in tlib/course.h:

#ifdef WIN32
	//#define DO_COURSE_GL								// define for TTS
																// undef for dll
	#include <glib_types.h>
#endif

**********************************************************************************************************/

#include <aunt.h>

#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
	#include <assert.h>
	#include <shlobj.h>
	#include <assert.h>
	#include <shlobj.h>
	#include <conio.h>
	#include <time.h>
#else
	#include <termios.h>
#endif

#include <glib_defines.h>
#include <globals.h>
#include <utils.h>
#include <fatalerror.h>

#include <computrainer.h>
#include <nvram.h>

#include <errors.h>
#include <ev.h>					// includes <map.h>
#include <dll_common.h>
#include <dll_globals.h>
#include <internal.h>			// includes <serial.h>
#include <dll.h>


#ifdef WIN32
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
#else
#endif

#ifdef DO_OBFUSCATE
	#ifdef WIN32
		#pragma message ("***********************  OBFUSCATED  ***********************")
	#else
	#endif

#else
	#ifdef WIN32
		#pragma message ("***********************  NOT OBFUSCATED  ***********************")
	#else
	#endif
#endif


#ifdef LOGDLL
	#ifdef WIN32
		#pragma message("LOGDLL is defined !!!!!!!!!!!!!!!!!!!!!")
	#endif
#endif


#if LEVEL >= DLL_CANNONDALE_ACCESS

	#ifdef NEWPORT

		/**********************************************************************
			set_port_info
		**********************************************************************/

		//^ set_port_info
		int set_port_info(int _ix, PORT_INFO _pi)  {
		//int set_portname(int _ix, const char *_portname)  {

			if (_ix<0 || _ix>255)  {
				return BAD_RIDER_INDEX;
			}

			portname_to_ix[_pi.name.c_str()] = _ix;
			ix_to_port_info[_ix] = _pi;

			//portnames[_ix] = _portname;

			return ALL_OK;
		}							// int
		//$

	#endif


/**********************************************************************

**********************************************************************/

//^ racermate_init
int racermate_init(void)  {

#ifdef _DEBUG
	//glog = new gloger(str);
#endif

	#ifdef LOGDLL
		unlink(DLLFILE);
		unlink("ev.log");
	#endif

//to do
		// make sure that only one instance of the dll is running

	return 0;
}



//$ racermate_init

/**********************************************************************

**********************************************************************/

//^ racermate_close

int racermate_close(void)  {
	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "racermate_close\n");
		FCLOSE(logstream);
	#endif

#ifdef WIN32
		DEL(server);
#endif

		ResetAlltoIdle();

	return 0;
}

//$ racermate_close


/**********************************************************************

**********************************************************************/

//^ get_errstr
const char *get_errstr(int err)  {
	const char *cptr;
	int i;

	#ifdef LOGDLL
		//fprintf(logstream, "EV:: threadproc start\n");
		//fclose(logstream);
		//logstream = fopen("ev.log", "a+t");
	#endif


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

	EnumDeviceType GetRacerMateDeviceID(int _ix)  {
		EnumDeviceType what;

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "GetRacerMateDeviceID(), ix = %d\n", ix);
			fclose(logstream);
		#endif

		setup_standard_dirs();

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "grmdid1\n");
		FCLOSE(logstream);
	#endif

		//int comport = ix + 1;
		//const char *cptr = ix_to_portname[ix].c_str();

		if (glog)  {
			glog->write(10,0,1,"GetRacerMateDeviceID(%d)\n", _ix);
		}

		if (gfile_mode)  {
			if (glog) glog->write(10,0,1,"file_mode\n");
			if (devices[_ix].file_mode)  {
				what = DEVICE_RMP;
				devices[_ix].what = what;
				const char *cptr = ix_to_port_info[_ix].name.c_str();
				strncpy(devices[_ix].portname, cptr, sizeof(devices[_ix].portname)-1);
				//devices[ix].port = ix + 1;
				devices[_ix].when = timeGetTime();
				return what;
			}
		}

		if (_ix==SIMULATOR_DEVICE)  {					// 255?
			what = DEVICE_SIMULATOR;
			devices[_ix].what = what;
			//devices[ix].port = ix + 1;
			devices[_ix].when = timeGetTime();
			return what;
		}
		else if (_ix==PERF_DEVICE)  {					// 254?
			what = DEVICE_RMP;
			devices[_ix].what = what;
			//devices[ix].port = ix + 1;
			devices[_ix].when = timeGetTime();
			return what;
		}
		/*
		else if (ix==SERVER_PORT-1)  {
			what = DEVICE_WIFI;
			devices[ix].what = what;
			devices[ix].port = ix + 1;
			devices[ix].when = timeGetTime();
			return what;
		}
		*/

		what = devices[_ix].what;
		if (glog) glog->write(10,0,1,"grmid1\n");

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "grmdid2\n");
			FCLOSE(logstream);
		#endif

#ifdef NEWPORT
		if ( haveDevice(what, _ix) )  {
#else
		if ( haveDevice(what, comport) )  {
#endif
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "grmdid3\n");
				FCLOSE(logstream);
			#endif

			if (glog) glog->write(10,0,1,"grmid2\n");
			return what;
		}

		if (glog) glog->write(10,0,1,"grmid3\n");

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "grmdid2\n");
		FCLOSE(logstream);
	#endif
		what = check_for_trainers(_ix, 0);
	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "grmdix\n");
		FCLOSE(logstream);
	#endif

		if (glog) glog->write(10,0,1,"grmidx, %s\n", devstrs[what]);
		return what;

	}					// EnumDeviceType GetRacerMateDeviceID(int ix)  {

	//$

	/**********************************************************************
		NOTE!!! RM1 does not call this!!!!
	**********************************************************************/

	//^ Setlogfilepath

	int Setlogfilepath(const char *dir)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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

		/*
		dirs[DIR_DEBUG] = dir;
		if (glog)  {
			glog->write(10,0,1,"slfp3\n");
		}
		*/

		/*
		if (!direxists(dirs[DIR_DEBUG]).c_str())  {
			if (glog)  {
				glog->write(10,0,1,"slfp4\n");
			}
			return DIRECTORY_DOES_NOT_EXIST;
		}
		*/

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
			logstream = fopen(DLLFILE, "a+t");
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

		//if (exists(gstring))  {
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

	int GetFirmWareVersion(int _ix)  {
		const char *cptr;
		EnumDeviceType what=DEVICE_NOT_SCANNED;
		int n;
		float f;
		int version;

#ifdef _DEBUG
		//int bp = 1;
#endif

	#ifdef FAKE_HB
		return 240;
	#endif

	setup_standard_dirs();

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "GetFirmWareVersion(%d)\n", _ix);
		
		#ifdef WIN32
			/*
			SHGetFolderPath(
				NULL,							// HWND hwndOwner,
				CSIDL_PROGRAM_FILES,			// int nFolder,
				NULL,							// HANDLE hToken,
				0,								// DWORD dwFlags,
				gstring
			);																				// "C:\Program Files"

			fprintf(logstream, "CSIDL_PROGRAM_FILES = %s\n", gstring);
			if (dirs.size()==0)  {
				dirs.push_back(gstring);
				cptr = dirs[DIR_PROGRAM].c_str();
			}

			SHGetFolderPath(
				NULL,							// HWND hwndOwner,
				CSIDL_PERSONAL,					// int nFolder,
				NULL,							// HANDLE hToken,
				0,								// DWORD dwFlags,
				gstring
			);																				// "C:\Users\larry\Documents"

			fprintf(logstream, "CSIDL_PERSONAL = %s\n", gstring);
			if (dirs.size()==0)  {
				dirs.push_back(gstring);
				cptr = dirs[DIR_PERSONAL].c_str();
			}
			*/
		#endif

		fclose(logstream);
	#endif

	//int comport = _ix + 1;

	what = devices[_ix].what;

#ifdef NEWPORT
	if ( !haveDevice(what, _ix) )  {
#else
	if ( !haveDevice(what, comport) )  {
#endif

		#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "gfwv 1\n");
		fclose(logstream);
		#endif

		what = GetRacerMateDeviceID(_ix);

		#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "gfwv 2\n");
		fclose(logstream);
		#endif
		if ( !haveDevice(what, _ix) )  {
			return GENERIC_ERROR;
		}
	}

		if (what==DEVICE_COMPUTRAINER)  {
#ifdef NEWPORT
			cptr = get_computrainer_firmware(_ix);
#else
			cptr = get_computrainer_firmware(comport);
#endif

			n = sscanf(cptr, "%f", &f);
			if (n != 1)  {
				return GENERIC_ERROR;
			}
			version = (int) f;
			return version;
		}
		else if (what==DEVICE_VELOTRON)  {
			cptr = get_velotron_firmware(_ix);
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
			logstream = fopen(DLLFILE, "a+t");
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
			logstream = fopen(DLLFILE, "a+t");
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
		/*
		struct tm *stm;
		DWORD linktime;
		IMAGE_NT_HEADERS *NTHeader;
		IMAGE_DOS_HEADER *DosHeader;
		HMODULE ModuleHandle;
		*/

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "get_build_date()\n");
			fclose(logstream);
		#endif

		//ModuleHandle = GetModuleHandle(NULL);
		//DosHeader = (IMAGE_DOS_HEADER*)ModuleHandle;

		//NTHeader = (IMAGE_NT_HEADERS*)((char *)DosHeader + DosHeader->e_lfanew);

		/*
		if (IMAGE_NT_SIGNATURE != NTHeader->Signature)  {
			throw(fatalError(__FILE__, __LINE__));
		}
		*/


		//linktime = NTHeader->FileHeader.TimeDateStamp;
		//stm = _localtime32((__time32_t*)&linktime);
		sprintf(buildDate, "%s %s", __DATE__, __TIME__);									// "Apr 14 2015 10:05:23"
		//asctime_s( buildDate, sizeof(buildDate), stm );										// "Tue Apr 14 10:06:17 2015"

		//return BUILD_DATE;
		return buildDate;

	}				// int get_dll_version(int _portnum)
	//$

	/**********************************************************************

	**********************************************************************/

	//^ GetIsCalibrated

	bool GetIsCalibrated(int ix, int FirmwareVersion)  {
		//int i;
		EnumDeviceType what;
#ifdef WIN32
		int portnum;
		portnum = ix + 1;
#endif
		//const char *cptr;

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "GetIsCalibrated(%d)\n", ix);
		fclose(logstream);
	#endif
		//i = ix;

		what = devices[ix].what;
#ifdef NEWPORT
		if ( haveDevice(what, ix) )  {
#else
		if ( haveDevice(what, portnum) )  {
#endif


			if (what!=DEVICE_COMPUTRAINER && what!=DEVICE_VELOTRON)  {
				return false;
			}
#ifdef NEWPORT
			return iscal(what, ix);
#else
			return iscal(what, portnum);
#endif

		}

		what = check_for_trainers(ix, 0);
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

	int GetCalibration(int ix)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "GetCalibration(%d)\n", ix);
			fclose(logstream);
		#endif

		int n, cal;
#ifdef WIN32
		int comport;
#endif
		char str[32];
		float f;
		const char *cptr;
		EnumDeviceType what=DEVICE_NOT_SCANNED;

		//comport = ix + 1;
		//i = ix;
		what = devices[ix].what;


#ifdef NEWPORT
		if ( !haveDevice(what, ix) )  {
#else
		comport = ix + 1;
		if ( !haveDevice(what, comport) )  {
#endif
			what = GetRacerMateDeviceID(ix);
			if ( !haveDevice(what, ix) )  {
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
			return (int)vtcal;
		}


		what = devices[ix].what;
		if ( haveDevice(what, ix) )  {
			return GENERIC_ERROR;
		}

		what = check_for_trainers(ix, 0);
		if (what!=DEVICE_COMPUTRAINER)  {
			return GENERIC_ERROR;
		}

		cptr = get_computrainer_cal(ix);
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

	int resetTrainer(int ix, int _fw, int _cal)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
	//$


/**********************************************************************

**********************************************************************/

//^ startTrainer

int startTrainer(int ix)  {

	#ifdef WIN32
		int portnum;
		portnum = ix + 1;
	#endif

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "startTrainer(%d)\n", ix);
		fclose(logstream);
	#endif

	devices[ix].logging_type = NO_LOGGING;

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


	if (devices[ix].ev)  {				// this trainer is already started
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   returning trainer already started\n");
			fclose(logstream);
		#endif
		return DEVICE_ALREADY_RUNNING;
	}

	devices[ix].course = NULL;

	EnumDeviceType what;

	if (devices[ix].what == DEVICE_NOT_SCANNED)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   device not scanned\n");
			fclose(logstream);
		#endif
		what = GetRacerMateDeviceID(ix);

		if (what==DEVICE_DOES_NOT_EXIST)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   returning device does not exist: %d\n", DEVICE_DOES_NOT_EXIST);
				fclose(logstream);
			#endif
			return DEVICE_DOES_NOT_EXIST;
		}
		else if (what==DEVICE_OPEN_ERROR)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   returning port open error: %d\n", PORT_OPEN_ERROR);
				fclose(logstream);
			#endif
			return PORT_OPEN_ERROR;
		}
	}
	else if (devices[ix].what == DEVICE_DOES_NOT_EXIST)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   device was already scanned\n");
			fclose(logstream);
		#endif

		what = GetRacerMateDeviceID(ix);
		if ( haveDevice(what, ix) )  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   returning device does not exist 2: rc = %d\n", DEVICE_DOES_NOT_EXIST);		// 1 = serial port does not exist
				#ifdef NEWPORT
					fprintf(logstream, "   ix = %d\n", ix);
				#else
					fprintf(logstream, "   port = %d\n", portnum);
				#endif

				if (what==DEVICE_COMPUTRAINER)  {
					fprintf(logstream, "   what = COMPUTRAINER\n");
				}
				else if (what==DEVICE_VELOTRON)  {
					fprintf(logstream, "   what = VELOTRON\n");
				}
				else  {
					fprintf(logstream, "   what = ???\n");
				}

				#ifdef NEWPORT
					fprintf(logstream, "   ix = %d\n", ix);
				#else
					fprintf(logstream, "   realport = %d\n", portnum);
				#endif
				fclose(logstream);
			#endif

			return DEVICE_DOES_NOT_EXIST;
		}

		// device does not exist has timed out. see if it still doesn't exist and restart the timer:

		what = GetRacerMateDeviceID(ix);
		if (what == DEVICE_DOES_NOT_EXIST)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   returning device does not exist 3: %d\n", DEVICE_DOES_NOT_EXIST);
				fclose(logstream);
			#endif
			return DEVICE_DOES_NOT_EXIST;
		}
	}

	what = devices[ix].what;

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "   ok1\n");
		fclose(logstream);
	#endif

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		instances++;
	}

	if (what==DEVICE_COMPUTRAINER)  {
		//EV(int _realportnum, EnumDeviceType _what, Bike::PARAMS *_params=NULL, Course *_course=NULL);
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   ok2a: device is computrainer\n");
			fclose(logstream);
		#endif

		// devices[ix].rd is the rider data
		#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, NULL, devices[ix].logging_type);
		#else
		#endif

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   ev created\n");
			fclose(logstream);
		#endif

		#ifdef DO_ANT
			if (asi)  {
				float diameter = 700.0f;									// mm
				float circumference = (float) (PI * diameter);		// mm
				devices[ix].ev->set_circumference(circumference);
			}
		#endif

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   st1\n");
			fclose(logstream);
		#endif

		#ifdef DO_EXPORT
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   calling export_flag_file\n");
				fclose(logstream);
			#endif

			if (export_flag_file())  {								// in startTrainer()
				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "   back from export_flag_file\n");
					fclose(logstream);
				#endif

				//devices[ix].ev->set_export();
				#ifdef NEWPORT
					devices[ix].ev->set_export(dirs[DIR_DEBUG].c_str(), portnum);
				#else
					#ifdef LOGDLL
						logstream = fopen(DLLFILE, "a+t");
						fprintf(logstream, "   set_export %d\n", portnum);
						fclose(logstream);
					#endif

					devices[ix].ev->set_export(dirs[DIR_DEBUG].c_str(), portnum);
				#endif
			}
		#endif					// #ifdef DO_EXPORT

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   ok2a2\n");
			fclose(logstream);
		#endif

		//assert(devices[ix].ev->ds != NULL);

		devices[ix].ev->set_file_mode_x(gfile_mode);											// datasource

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   ok2b\n");
			fclose(logstream);
		#endif
	}
	else if (what==DEVICE_VELOTRON)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   ok2\n");
			fclose(logstream);
		#endif

		if (devices[ix].bike_params.real_front_teeth==0)  {					// ActualChainRing?
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   startTrainer error:\n");
				fclose(logstream);

				dump_bike_params(ix);
			#endif
			return VELOTRON_PARAMETERS_NOT_SET;
		}

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			#ifdef NEWPORT
				fprintf(logstream, "   ok6, ix = %d\n", ix);
			#else
				fprintf(logstream, "   ok6, realport = %d\n", portnum);
			#endif
			fclose(logstream);
		#endif

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   starting thread\n");
			fclose(logstream);
			dump_bike_params(ix);
		#endif

		#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_VELOTRON, devices[ix].rd, &devices[ix].bike_params, NULL, devices[ix].logging_type);
		#else
			//devices[ix].ev = new EV(portnum, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
			//devices[ix].ev = new EV(portnum, DEVICE_COMPUTRAINER, devices[ix].rd, NULL);
			devices[ix].ev = new EV(
												portnum,
												DEVICE_VELOTRON,
												devices[ix].rd,
												//NULL,														// bike::params *
												&devices[ix].bike_params,
												NULL,														// course *
												NO_LOGGING,
												devices[ix].url,
												devices[ix].tcp_port
											);
		#endif

		devices[ix].ev->set_logging_x(devices[ix].logging_type);

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "   back from thread start\n");
			fclose(logstream);
			dump_bike_params(ix);
		#endif
	}
	else if (what==DEVICE_SIMULATOR)  {
		#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_SIMULATOR, devices[ix].rd, NULL, NULL);
		#else
			//devices[ix].ev = new EV(portnum, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
			//devices[ix].ev = new EV(portnum, DEVICE_COMPUTRAINER, devices[ix].rd, NULL);
			devices[ix].ev = new EV(
													portnum,
													DEVICE_COMPUTRAINER,
													devices[ix].rd,
													NULL,														// bike::params *
													NULL,														// course *
													NO_LOGGING,
													devices[ix].url,
													devices[ix].tcp_port
												);
		#endif
	}
	else if (what==DEVICE_RMP)  {
		#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_RMP, devices[ix].rd, NULL, NULL);
		#else
			//devices[ix].ev = new EV(portnum, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
			//devices[ix].ev = new EV(portnum, DEVICE_COMPUTRAINER, devices[ix].rd, NULL);
			devices[ix].ev = new EV(
													portnum,
													DEVICE_COMPUTRAINER,
													devices[ix].rd,
													NULL,														// bike::params *
													NULL,														// course *
													NO_LOGGING,
													devices[ix].url,
													devices[ix].tcp_port
												);
		#endif
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
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "   startTrainerx\n");
		fclose(logstream);
		dump_bike_params(ix);
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


#ifdef _DEBUG
	#ifdef DO_ANT
		bool get_ant_stick_initialized(int _ix)  {
			if (devices[_ix].ev)  {
				return devices[_ix].ev->get_ant_stick_initialized();
			}
			else  {
				return false;
			}
		}
	#endif
#endif


	/**********************************************************************

	**********************************************************************/

	//^ ResetAverages

	int ResetAverages(int ix, int fw)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "ResetAverages()\n");
			fclose(logstream);
		#endif

		if (!devices[ix].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		devices[ix].ev->reset_averages();

		return ALL_OK;
	}								// ResetAverages()

	//$
	/**********************************************************************
	typedef struct TrainerData	{
		float speed;	//ALWAYS in MPH, application will metric convert. <0 on error
		float cadence; //in RPM, any number <0 if sensor not connected or errored.
		float HR;		//in BPM, any number <0 if sensor not connected or errored.
		float Power;	//in Watts <0 on error
	};

	**********************************************************************/


	//^ GetTrainerData

	struct TrainerData GetTrainerData(int ix, int fw)  {
		TrainerData td, *ptd;

		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "GetTrainerData()\n");
			fclose(logstream);
			*/
			/*
			if (dirs.size() != 0)  {
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "dirs size = %d\n", dirs.size());
				fclose(logstream);
			}
			*/

		#endif

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
	//$
	/**********************************************************************

	**********************************************************************/

	//^ get_average_bars

	float * get_average_bars(int ix, int FWVersion)  {
		float *fptr;

		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "get_average_bars()\n");
			fclose(logstream);
			*/
		#endif

		if (!devices[ix].ev)  {
			return NULL;
		}

		fptr = devices[ix].ev->get_average_bars_x();

		return fptr;
	}									// get_average_bars()
	//$
	/**********************************************************************

	**********************************************************************/

	//^ get_ss_data

#ifdef WIN32
	struct SSDATA get_ss_data(int ix, int fw) throw(...)  {
#else
	struct SSDATA get_ss_data(int ix, int fw)  {                                                                                                     // full
#endif
		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "get_ss_data(%d)\n", ix);
			fclose(logstream);
			*/
		#endif

	#ifdef _DEBUG
		EnumDeviceType what;
		what = devices[ix].ev->getwhat();

		if (what==DEVICE_COMPUTRAINER)  {
			/*
			if (fw != 4095)  {
				throw(fatalError(__FILE__, __LINE__));
			}
			*/
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
	//$
	/**********************************************************************

	**********************************************************************/

	//^ SetSlope

	int SetSlope( int ix, int fw, int cal, float bike_kgs, float person_kgs, int _drag_factor, float grade)   {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
	//$

	/**********************************************************************

		returns:
			0 = OK

		errors:
			1 = thread not running
			2 = not a valid device

	**********************************************************************/

	//^ setPause

	int setPause(int ix, bool _paused)  {
		EnumDeviceType what;


		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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

		bool p = ev->is_paused_x();
		if (p==_paused)  {
			return ALL_OK;
		}

		devices[ix].ev->set_paused(_paused);


		return ALL_OK;
	}										// int setPause()
	//$
	/**********************************************************************
		portnum starts at 0
		stop : Bar controller exits Pro-E mode.
		Application hangs waiting for return from stop, 1 core pinned at 100% ultilization as if in an infinite wait loop.

	**********************************************************************/

	//^ stopTrainer

	int stopTrainer(int ix)  {

#ifdef WIN32
	DEL(server);
#endif

#ifdef DO_ANT
	//if (ant_stick_initialized && !ant_closed)  {
	if (asi && !ant_closed)  {
		ant_close();
	}
#endif

		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "stopTrainer(%d)\n", ix);
			fclose(logstream);
			*/
		#endif

		if (!devices[ix].ev)  {
			return DEVICE_NOT_RUNNING;
		}

		EnumDeviceType what;
		what = GetRacerMateDeviceID(ix);

		devices[ix].ev->stop();
		DEL(devices[ix].ev);

		if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
			if (instances > 0)  {
				instances--;
			}
		}

		return ALL_OK;
	}								// int stopTrainer(int ix)
	//$

	/**********************************************************************

	**********************************************************************/

	//^ ResettoIdle

	int ResettoIdle(int ix)  {

		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "ResettoIdle(%d)\n", ix);
			fclose(logstream);
			*/
		#endif

		stopTrainer(ix);
		devices[ix].what = DEVICE_NOT_SCANNED;
		//devices[ix].port = 0;
		memset(devices[ix].portname, 0, sizeof(devices[ix].portname));
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
	//$

#endif						// 	#if LEVEL >= DLL_CANNONDALE_ACCESS


#if LEVEL >= DLL_TRINERD_ACCESS
	/**********************************************************************

	**********************************************************************/

	//^ GetHandleBarButtons

	int GetHandleBarButtons(int ix, int fw)  {
		int keys = 0;
		int i, newkeys = 0;
		int mask;

		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "GetHandleBarButtons(%d)\n", ix);
			fclose(logstream);
			*/
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

			if (!devices[ix].ev->is_connected_x())  {
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
	//$

	/**********************************************************************

	**********************************************************************/

	//^ SetErgModeLoad

	int SetErgModeLoad(int ix, int FirmwareVersion, int RRC, float _watts)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
	}							// int
	//$
	/**********************************************************************
		SetHRBeepBounds
	**********************************************************************/

	//^ SetHRBeepBounds

	int SetHRBeepBounds(int ix, int FirmwareVersion, int LowBound, int HighBound, bool BeepEnabled)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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

		devices[ix].ev->set_hr_bounds_x(LowBound, HighBound, BeepEnabled);			// always returns 0

		return ALL_OK;
	}						// int SetHRBeepBounds(int ix, int FirmwareVersion, int LowBound, int HighBound, bool BeepEnabled)  {
	//$

	/**********************************************************************

	**********************************************************************/

	//^ SetRecalibrationMode

	int SetRecalibrationMode(int ix, int fw)  {


		int comport;
		EnumDeviceType what;

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
			what = check_for_trainers(ix, 0);
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
	//$

	/**********************************************************************

	**********************************************************************/

	//^ EndRecalibrationMode

	int EndRecalibrationMode(int ix, int fw)  {


		int comport;
		EnumDeviceType what;

		calibrating = false;

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
			what = check_for_trainers(ix, 0);
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

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_computrainer_mode

	int get_computrainer_mode(int ix)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
	//$

#endif								// #if LEVEL >= DLL_TRINERD_ACCESS 

#if LEVEL >= DLL_ERGVIDEO_ACCESS

/**********************************************************************

**********************************************************************/

//^ get_status_bits

int get_status_bits(int ix, int fw)  {
	if (!devices[ix].ev)  {
		return -1;
	}
	return devices[ix].ev->get_status_bits_x();
	//return 0;
}						// int get_status_bits(int ix, int fw)  {

//$

	/**********************************************************************

	**********************************************************************/

	//^ get_np

	float get_np(int ix, int fw)  {

		if (!devices[ix].ev)  {
			return -1.0f;
		}
		return devices[ix].ev->get_x_np();
	}										// float get_np(int ix, int fw)  {

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_if

	float get_if(int ix, int fw)  {

		if (!devices[ix].ev)  {
			return -1.0f;
		}
		return devices[ix].ev->get_if_x();
	}							// float get_if(int ix, int fw)  {

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_tss

	float get_tss(int ix, int fw)  {

		if (!devices[ix].ev)  {
			return -1.0f;
		}
		return devices[ix].ev->get_tss_x();
	}					// float get_tss(int ix, int fw)  {

	//$

	/**********************************************************************

	**********************************************************************/

	//^ get_calories

	float get_calories(int ix, int fw)  {

		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "get_calories(%d, %d)\n", ix, fw);
			fclose(logstream);
			*/
		#endif

		if (!devices[ix].ev)  {
			return -1.0f;
		}

		return devices[ix].ev->get_calories_x();

	}				// float get_calories()

	//$

/**********************************************************************

**********************************************************************/

//^ get_pp

float get_pp(int ix, int fw)  {

	if (!devices[ix].ev)  {
		return -1.0f;
	}
	return devices[ix].ev->get_pp_x();
}					// float get_pp(int ix, int fw)  {

//$


#ifdef _DEBUG
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
#endif

	/**********************************************************************

	**********************************************************************/

	//^ set_server_network

	int set_server_network(int _broadcast_port, int _listen_port, bool _ip_discover, bool _udp)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "set_network()\n");
			fclose(logstream);
		#endif

		broadcast_port = _broadcast_port;
		listen_port = _listen_port;
		ip_discover = _ip_discover;
		udp = _udp;

#ifdef _DEBUG
#if 1
		// used for short_circuit test
		int status = start_server();
		if (status!=ALL_OK)  {
			return status;
		}
#endif
#endif

/*
while(1)  {
	Sleep(100);
}
*/

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			#ifdef NEWPORT
				fprintf(logstream, "url = xxx\n");
			#else
				fprintf(logstream, "url = %s\n", devices[_ix].url);
				fprintf(logstream, "tcp port = %d\n", devices[_ix].tcp_port);
			#endif
			fclose(logstream);
		#endif

		return ALL_OK;
	}													// set_server_network()
	//$


	/**********************************************************************

	**********************************************************************/

	//^ SetVelotronParameters

	int SetVelotronParameters( int ix, int FWVersion, int _nfront, int _nrear, int* Chainrings, int* cogset, float tire_diameter_mm, int ActualChainring, int Actualcog, float bike_kgs, int front_index, int rear_index)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "SetVelotronParameters()\n");
			fclose(logstream);
		#endif

		int i;
		//int portnum;

		if (_nfront>5 || _nrear>20)  {
			return BAD_GEAR_COUNT;
		}

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		#ifdef NEWPORT
			fprintf(logstream, "SetVelotronParameters(ix)\n");
		#else
			fprintf(logstream, "SetVelotronParameters(ix), _nfront = %d, _nrear = %d\n", ix, _nfront, _nrear);
		#endif
		fclose(logstream);
	#endif

		//portnum = ix + 1;
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
		logstream = fopen(DLLFILE, "a+t");
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
		logstream = fopen(DLLFILE, "a+t");
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
		dump_bike_params(ix);
	#endif

		return ALL_OK;
	}								// SetVelotronParameters()

	//$

	/**********************************************************************

	**********************************************************************/

	//^ GetCurrentVTGear

	struct Bike::GEARPAIR GetCurrentVTGear(int ix, int fw)  {
		//int portnum;

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "GetCurrentVTGear(%d)\n", ix);
			fclose(logstream);
		#endif

		//portnum = ix + 1;

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

	int setGear(int ix, int FWVersion, int front_index, int rear_index)  {

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "setGear(%d)\n", ix);
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

	float * get_bars(int ix, int FWVersion)  {
		#ifdef LOGDLL
			/*
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "get_bars()\n");
			fclose(logstream);
			*/
		#endif

		float *fptr;

		if (!devices[ix].ev)  {
			return NULL;
		}

		fptr = devices[ix].ev->get_bars_x();

		return fptr;
	}								// get_bars()
	//$

	/**********************************************************************
		int ResetAlltoIdle(void)  {
	**********************************************************************/

	//^ ResetAlltoIdle

	int ResetAlltoIdle(void)  {
		int i;
		//int  status;

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "\nResetAlltoIdle()\n");
			fclose(logstream);
		#endif

		for(i=0; i<256; i++)  {
			ResettoIdle(i);
		}

		return ALL_OK;
	}							// int ResetAlltoIdle(void)  {
	//$

	/**********************************************************************
		sets the 'started' flag in decoder so that distance, speed
		and things that depend on them are calculated.
	**********************************************************************/

	//^ start_trainer

	int start_trainer(int ix, bool _b)  {
//#ifndef NEWPORT
		int realport;
//#endif

		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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
	//$

/**********************************************************************

**********************************************************************/

//^ set_ftp

int set_ftp(int ix, int fw, float _ftp)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "set_ftp()\n");
			fclose(logstream);
		#endif

	EnumDeviceType what;



	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "set_ftp(%d)\n", ix);
		fclose(logstream);
	#endif


	what = devices[ix].what;
	if ( (what != DEVICE_COMPUTRAINER) && (what != DEVICE_VELOTRON) )  {
		return WRONG_DEVICE;
	}

	devices[ix].rd.ftp = _ftp;

	if (devices[ix].ev)  {
		devices[ix].ev->set_ftp_x(_ftp);
		return ALL_OK;
	}

	return DEVICE_NOT_RUNNING;
}									// int set_ftp(int ix, int fw, float _ftp)  {

//$

/**********************************************************************

**********************************************************************/

//^ set_wind

int set_wind(int ix, int fw, float _wind_kph)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "set_wind()\n");
			fclose(logstream);
		#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_wind_x(_wind_kph);
	return ALL_OK;
}									// int set_wind

//$

/**********************************************************************

**********************************************************************/

//^ set_draftwind

int set_draftwind(int ix, int fw, float _draft_wind_kph)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "set_draftwind()\n");
			fclose(logstream);
		#endif

	if (!devices[ix].ev)  {
		return DEVICE_NOT_RUNNING;
	}
	devices[ix].ev->set_draft_wind_x(_draft_wind_kph);
	return ALL_OK;
}							// int set_draftwind(int ix, int fw, float _draft_wind_kph)  {

//$

/**********************************************************************

**********************************************************************/

//^ update_velotron_current

int update_velotron_current(int ix, unsigned short pic_current)  {
		#ifdef LOGDLL
			logstream = fopen(DLLFILE, "a+t");
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

	#if DO_WIFI
		//strcat(portnames, SERVER_PORT_STR);												// server 'serial' porg
		int i;
	
		for(i=0; i<15; i++)  {
			sprintf(str, "COM%d ", SERVER_SERIAL_PORT_BASE+i);
			strcat(portnames, str);												// client 'serial' porg
		}
		sprintf(str, "COM%d ", SERVER_SERIAL_PORT_BASE+i);
		strcat(portnames, str);												// client 'serial' porg
	
		/*
		for(i=0; i<15; i++)  {
			sprintf(str, "COM%d ", CLIENT_SERIAL_PORT_BASE+i);
			strcat(portnames, str);												// client 'serial' porg
		}
		sprintf(str, "COM%d", CLIENT_SERIAL_PORT_BASE+i);
		strcat(portnames, str);												// client 'serial' porg
		*/
	#endif

#else
	//#ifdef DO_WIFI
	strcpy(str, "/dev/ttyUSB0");
	strcat(portnames, str);												// client 'serial' porg
	//#endif
#endif									// WIN32

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
			logstream = fopen(DLLFILE, "a+t");
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
			logstream = fopen(DLLFILE, "a+t");
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
			logstream = fopen(DLLFILE, "a+t");
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
			logstream = fopen(DLLFILE, "a+t");
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
			logstream = fopen(DLLFILE, "a+t");
			fprintf(logstream, "startTrainer2(%d)\n", ix);
			fclose(logstream);
		#endif

		devices[ix].logging_type = _logging_type;

		if (devices[ix].ev)  {				// this trainer is already started
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
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
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   device not scanned\n");
				fclose(logstream);
			#endif
			what = GetRacerMateDeviceID(ix);

			if (what==DEVICE_DOES_NOT_EXIST)  {
				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "   returning device does not exist: %d\n", DEVICE_DOES_NOT_EXIST);
					fclose(logstream);
				#endif
				return DEVICE_DOES_NOT_EXIST;
			}
			else if (what==DEVICE_OPEN_ERROR)  {
				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "   returning port open error: %d\n", PORT_OPEN_ERROR);
					fclose(logstream);
				#endif
				return PORT_OPEN_ERROR;
			}
		}
		else if (devices[ix].what == DEVICE_DOES_NOT_EXIST)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   device was already scanned\n");
				fclose(logstream);
			#endif

			what = GetRacerMateDeviceID(ix);
			if ( haveDevice(what, port) )  {

				#ifdef LOGDLL
					logstream = fopen(DLLFILE, "a+t");
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
					logstream = fopen(DLLFILE, "a+t");
					fprintf(logstream, "   returning device does not exist 3: %d\n", DEVICE_DOES_NOT_EXIST);
					fclose(logstream);
				#endif
				return DEVICE_DOES_NOT_EXIST;
			}

		}

		what = devices[ix].what;

	#ifdef LOGDLL
		logstream = fopen(DLLFILE, "a+t");
		fprintf(logstream, "   ok1\n");
		fclose(logstream);
	#endif

		if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
			instances++;
		}

		if (what==DEVICE_COMPUTRAINER)  {
			//EV(int _realportnum, EnumDeviceType _what, Bike::PARAMS *_params=NULL, Course *_course=NULL);
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   ok2a: device is computrainer\n");
				fclose(logstream);
			#endif

			// devices[ix].rd is the rider data
#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
#else
			devices[ix].ev = new EV(port, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
#endif
			//ds = getds();

			devices[ix].ev->set_file_mode_x(gfile_mode);


			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   ok2b\n");
				fclose(logstream);
			#endif
		}
		else if (what==DEVICE_VELOTRON)  {
			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   ok2\n");
				fclose(logstream);
			#endif

			if (devices[ix].bike_params.real_front_teeth==0)  {					// ActualChainRing?
				#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   startTrainer2 error:\n");
				fclose(logstream);

				dump_bike_params(ix);
				#endif
				return VELOTRON_PARAMETERS_NOT_SET;
			}

			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   ok6, realport = %d\n", realport);
				fclose(logstream);
			#endif

			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   starting thread\n");
				fclose(logstream);
				dump_bike_params(ix);
			#endif

#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_VELOTRON, devices[ix].rd, &devices[ix].bike_params, _course, devices[ix].logging_type);
#else
			devices[ix].ev = new EV(port, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
#endif

			devices[ix].ev->set_logging_x(devices[ix].logging_type);

			#ifdef LOGDLL
				logstream = fopen(DLLFILE, "a+t");
				fprintf(logstream, "   back from thread start\n");
				fclose(logstream);
				dump_bike_params(ix);
			#endif
		}
		else if (what==DEVICE_SIMULATOR)  {
#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_SIMULATOR, devices[ix].rd, NULL, _course);
#else
			devices[ix].ev = new EV(port, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
#endif
		}
		else if (what==DEVICE_RMP)  {
#ifdef NEWPORT
			devices[ix].ev = new EV(ix, devices[ix].portname, DEVICE_RMP, devices[ix].rd, NULL, NULL);
#else
			devices[ix].ev = new EV(port, DEVICE_COMPUTRAINER, devices[ix].rd, NULL, _course, devices[ix].logging_type);
#endif
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
		logstream = fopen(DLLFILE, "a+t");
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


