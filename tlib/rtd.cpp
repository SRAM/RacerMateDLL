
//lint -save -e18 -e40 -e32 -e30 -e36 -e145 -e49 -e39 -e119 -e736
#ifdef WIN32
	//#define STRICT
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	//#include <winsock2.h>
#endif


#include <stdlib.h>
#include <assert.h>

#include <comglobs.h>
#include <globals.h>
#include <oldsdirs.h>
#include <tglobs.h>
#include <utils.h>
#include <rtd.h>
#include <fatalerror.h>
#include <crf.h>
#ifdef DO_ANT
	#include <aunt.h>
#endif

//#include <ant.h>

#ifdef DO_ANT
	extern SC sc;
	extern HR hr;
	extern CAD cad;
#endif

/**********************************************************************
	constructor #1
	called by Computrainer() constructor #1
	EV() in ev.cpp
	startTrainer() in dll.cpp
**********************************************************************/

RTD::RTD(
				const char *_port,
				int _baud,
				RIDER_DATA &_rd, 
				Course *_course, 
				Bike *_bike,
				//const char *_dataFileName,
				bool _show_errors,
				LoggingType _logging_type,
				const char *_url,
				int _tcp_port
	  		) : dataSource(_rd, _course, _bike, _logging_type)  {

	int prt, n;
	char _comportstr[32];
	char _baudstr[32];
	char str[256];
	char str2[256];

#ifdef HELO_WRLD
	server = nullptr;
#endif


#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 3)  {
		bp = 0;
	}
#endif

	ix = -1;										// constructor 1

	memset(url, 0, sizeof(url));
	strncpy(url, _url, sizeof(url)-1);

	tcp_port = _tcp_port;

#ifdef DO_RMP
	rmp = NULL;
	memset(&rmpdata, 0, sizeof(rmpdata));
#endif

	initialized = false;
	caller_created_port = false;
	sout = NULL;
	dsmetric = false;

	n = sscanf(_port, "COM%d", &prt);
	if (n==1)  {
		comportnum = prt;
		sprintf(_comportstr, "COM%d", prt);
		//id = prt-1;
		id = prt;
	}
	else  {
		comportnum = -1;
		id = -1;
		strncpy(_comportstr, _port, sizeof(_comportstr)-1);
	}


	sprintf(_baudstr, "%d", _baud);

	assert(log==NULL);

	if (gblog)  {
		char str[256];
#ifdef DOGLOBS
		sprintf(str, "%s%sds%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
		sprintf(str, "%s%sds%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
		log = new Logger(str);													// subclasses may close this, delete it, and change the id number
		if (log) log->write(10,0,1,"rtd1: logging started\n");
	}

	memset(&pinf, 0, sizeof(PerformanceInfo));

	memset(outfilename, 0, sizeof(outfilename));

	show_errors = _show_errors;


	port = NULL;
	workbuf = NULL;
	outstream = NULL;

	unsigned long offs = 0L;

	memset(&perf_file_header, 0, sizeof(perf_file_header));
	perf_file_header.sec.type = SEC_PERF_FILE_HEADER;
	perf_file_header.sec.struct_start_offset = offs;
	offs += sizeof(perf_file_header);
	perf_file_header.sec.structsize = sizeof(perf_file_header);
	perf_file_header.sec.data_start_offset = sizeof(perf_file_header);
	perf_file_header.sec.version = 0;

	memset(&rinf, 0, sizeof(rinf));
	rinf.sec.type = SEC_RIDER_INFO;
	rinf.sec.structsize = sizeof(rinf);
	rinf.sec.struct_start_offset = offs;
	offs +=	sizeof(SEC);
	rinf.sec.data_start_offset = rinf.sec.struct_start_offset + sizeof(rinf);

	rinf.pinf.sec.type = SEC_PERSON_INFO;
	rinf.pinf.sec.structsize = sizeof(rinf.pinf);
	rinf.pinf.sec.struct_start_offset = offs;	//rinf.sec.struct_start_offset + sizeof(SEC);
	offs += sizeof(pinf);
	rinf.pinf.sec.data_start_offset = rinf.pinf.sec.struct_start_offset + sizeof(pinf);

	rinf.pinf.age = rd.pd.age;							// double age;
	rinf.pinf.kgs = rd.pd.kgs;							// double kgs;
	rinf.pinf.sex = rd.pd.sex;							// char sex;
	strncpy(rinf.pinf.lastname, rd.pd.lastname.c_str(), sizeof(rinf.pinf.lastname)-1);			// char lastname[40];
	strncpy(rinf.pinf.firstname, rd.pd.firstname.c_str(), sizeof(rinf.pinf.firstname)-1);		// char firstname[40];
	strncpy(rinf.pinf.username, rd.pd.username.c_str(), sizeof(rinf.pinf.username)-1);			// char username[40];
	strncpy(rinf.pinf.email, rd.pd.email.c_str(), sizeof(rinf.pinf.email)-1);					// char email[64];
	strncpy(rinf.pinf.city, rd.pd.city.c_str(), sizeof(rinf.pinf.city)-1);						// char city[24];
	strncpy(rinf.pinf.country, rd.pd.country.c_str(), sizeof(rinf.pinf.country)-1);				// char country[24];
	strncpy(rinf.pinf.state, rd.pd.state.c_str(), sizeof(rinf.pinf.state)-1);					// char state[16];
	strncpy(rinf.pinf.zip, rd.pd.zip.c_str(), sizeof(rinf.pinf.zip)-1);							// char zip[16];

	rinf.lower_hr = rd.lower_hr;								// float
	rinf.upper_hr = rd.upper_hr;								// float

	if (rd.hr_beep_enabled)  {
		rinf.hr_beep_enabled = 1;
	}
	else  {
		rinf.hr_beep_enabled = 0;
	}

	if (rd.draft_enable)  {
		rinf.draft_enable = 1;
	}
	else  {
		rinf.draft_enable = 0;
	}

	rinf.drag_aerodynamic = rd.drag_aerodynamic;				// float
	rinf.drag_tire = rd.drag_tire;								// float
	rinf.rfdrag = rd.cal;											// unsigned short for computrainer
	rinf.watts_factor = rd.watts_factor;						// float
	rinf.ftp = rd.ftp;											// float

	memset(&result, 0, sizeof(result));
	result.sec.type = SEC_RESULT;
	result.sec.structsize = sizeof(result);
	result.sec.data_start_offset = offs;
	offs += sizeof(result);
	result.sec.data_size = 0L;

	if (log) log->write(10,0,1,"rtd: 1\n");

	RTD::reset();

	if (log) log->write(10,0,1,"rtd: 2\n");

	if (logging_type != NO_LOGGING)  {
#ifdef DOGLOBS
		if (!direxists(dirs[DIR_PERFORMANCES].c_str()))  {
			if (CreateDirectory(dirs[DIR_PERFORMANCES].c_str(), NULL))  {
#else
		if (!direxists(SDIRS::dirs[DIR_PERFORMANCES].c_str()))  {
			if (CreateDirectory(SDIRS::dirs[DIR_PERFORMANCES].c_str(), NULL))  {
#endif
				throw(fatalError(__FILE__, __LINE__));
			}
		}
	}


	switch(logging_type)  {
		case RAW_PERF_LOGGING:
#ifdef DOGLOBS
			sprintf(outfilename, "%s%s%d.dat", dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, comportnum);
#else
			sprintf(outfilename, "%s%s%d.dat", SDIRS::dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, comportnum);
#endif
			break;
		case THREE_D_PERF_LOGGING:
#ifdef DOGLOBS
			sprintf(outfilename, "%s%s%d.3dp", dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, comportnum);
#else
			sprintf(outfilename, "%s%s%d.3dp", SDIRS::dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, comportnum);
#endif
			break;

		case RM1_PERF_LOGGING:  {
			/*
			Naming is:
			2012-06-15@14-39-25_3-EPaul5 Smeulders_8_17miler_3dcPartOBex_0h00m10.0s.rmp

			Year-month-day@HH-mm-ss_rideposition-FirstName 
			Lastname_Coursefilename_0h00m00.0s ??

			Where HH-mm-ss is 24hr time at start of ride, 0h00m00.0s is completions time 
			for ride.

			You can generate as many as you like with the "save performances" after a 
			ride, and it even works if you do it in demo mode. Hit "G" then "d" and 
			after some riding, hit "r" and save performance.

			EPaul5 Smeulders in the above is a ridername, I was using several 
			identities.

			Go ahead and try to generate some a few to see if my interpretation is 
			right. Try two riders and use demo mode, you will see the rider positions 
			added in.
			*/

			#ifdef WIN32
				SYSTEMTIME x;
#ifdef _DEBUG
				_CrtMemState memstart, memstop, memdiff;
				_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
				_CrtMemCheckpoint(&memstart);
#endif
			#else
				//struct tm *x;
				time_t aclock;
			#endif

			memset(str2, 0, sizeof(str2));									// ok
			strncpy(str2, course->get_name(), sizeof(str2)-1);
			strip_extension(str2);
			//int position;
			//position = 1;		// rider position
			//CourseType ct = Video;
			//int ct = 0x05;			// see CourseType in tdefs.h: Video | Distance

			#ifdef WIN32
				GetLocalTime(&x);
				// 2012-06-15@14-39-25_3-EPaul5 Smeulders_8_17miler_3dcPartOBex_0h00m10.0s.rmp
				// 2012-06-23@09-23-54_3-John Manley_8_NYCtri_0h00m10.0s.rmp


				sprintf(str,"%4d-%02d-%02d@%02d-%02d-%02d_%d-%s %s_%d_%s_0h00m00.0s",
					x.wYear,
					x.wMonth,
					x.wDay,
					x.wHour,
					x.wMinute,
					x.wSecond,
					//position,
					1,
					rinf.pinf.firstname,
					rinf.pinf.lastname,
					0x05,							// see CourseType in tdefs.h: Video | Distance
					str2
					);
#ifdef _DEBUG
				bp = strlen(str);
				bp = strlen(str2);
				bp = 1;
				strcpy(str, "x");						// this renames it to x.rmp
#endif

			#else
				time(&aclock);						// Get time in seconds
				//x = localtime(&aclock);			// Convert time to struct tm form
				//sprintf(file,"%02d%02d%02d%02d.log",x->tm_mon+1, x->tm_mday, x->tm_hour, x->tm_min);
			#endif

#ifdef DOGLOBS
			sprintf(outfilename, "%s%s%s.rmp", dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, str);
#else
			sprintf(outfilename, "%s%s%s.rmp", SDIRS::dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, str);
#endif

#ifdef _DEBUG
				bp = (int)strlen(outfilename);
				bp = 1;
#endif
			break;
		}

		default:
			break;
	}


	dataStart = 0;
	CLEN = 1024;
	lastCommTime = 0;
	lastHWTime = 0;
	packetIndex = 0;
	recordsOut = 0;
	saved = false;
	dsmetric = false;

	workbuf = new unsigned char[(unsigned int)CLEN];
	for(int i=0;i<CLEN;i++)  workbuf[i] = 0;

	port = NULL;

#ifdef HELO_WRLD
	server = NULL;
#endif

	client = NULL;

	#ifndef WIN32
		port = new Serial(_port, _baudstr, NULL);
	#else
		if (log) log->write(10,0,1,"rtd: 3, comportnum = %d\n", comportnum);
		try  {
			if (comportnum >= CLIENT_SERIAL_PORT_BASE && comportnum <= CLIENT_SERIAL_PORT_BASE + 16)  {
				if (log) log->write(10,0,1,"rtd: calling Client(\"%s\", %d)\n", url, tcp_port);
				client = new Client(url, tcp_port, 2);											// the url and port still need to be set up for this instantiation
			}
			else  {
				port = new Serial(_comportstr, _baudstr, NULL);				// constructor 4
			}

			//port = new Serial(_comportstr, _baudstr, NULL);				// constructor 1

		}
		catch (const int istatus) {
			if (log) log->write(10,0,1,"rtd: error, status = %d\n", istatus);
			// return with initialized = false;
			switch(istatus)  {
				case 100:											// bind error for server socket
					if (log) log->write(10,0,1,"Server socket TIME_WAIT error\n");
					destruct();
					if (show_errors)  {
						//MessageBox(NULL, "Serial Port Error\nThe selected serial port does not exist\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						//Serial Port Error
						//	A selected serial port does not exist
						//	Please check your settings again to correct the problem.
						MessageBox(NULL, "Server Socket Error\n", "Error", MB_OK);
						//throw(fatalError(__FILE__, __LINE__, "Serial Port Does Not Exist"));
						return;
					}
					else  {
						//throw istatus;
						return;
					}
					break;
				case 101:											// url/port not yet set up in client
					bp = 1;
					break;
	
				case SERIAL_ERROR_DOES_NOT_EXIST:			// doesn't exist
					if (log) log->write(10,0,1,"Serial Port Does Not Exist\n");
					destruct();
					if (show_errors)  {
						//MessageBox(NULL, "Serial Port Error\nThe selected serial port does not exist\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						//Serial Port Error
						//	A selected serial port does not exist
						//	Please check your settings again to correct the problem.
						MessageBox(NULL, "Serial Port Error\nA selected serial port does not exist\nPlease check your settings again to correct the problem.", "Error", MB_OK);
						//throw(fatalError(__FILE__, __LINE__, "Serial Port Does Not Exist"));
						return;
					}
					else  {
						//throw istatus;
						return;
					}
					//return;
					break;
	
				case SERIAL_ERROR_ACCESS_DENIED:				// exists, but can't open
					if (log) log->write(10,0,1,"Serial Port Access Denied\n");
					destruct();
					if (show_errors)  {
						MessageBox(NULL, "Serial Port Error\nSerial Port Access Denied\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						throw(fatalError(__FILE__, __LINE__, "Serial Port Access Denied"));
					}
					else  {
						//throw istatus;
					}
					return;
					//break;
	
				case SERIAL_ERROR_BUILDCOMMDCB:
					if (log) log->write(10,0,1,"Serial Port DCB Error\n");
					destruct();
					if (show_errors)  {
						MessageBox(NULL, "Serial Port Error\nSerial Port DCB Error\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						throw(fatalError(__FILE__, __LINE__, "Serial Port DCB Error"));
					}
					else  {
						//throw istatus;
					}
					return;
					//break;
	
				case SERIAL_ERROR_SETCOMMSTATE:
					if (log) log->write(10,0,1,"Serial Port Commstate Error\n");
					destruct();
					if (show_errors)  {
						MessageBox(NULL, "Serial Port Error\nSerial Port Commstate Error\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						throw(fatalError(__FILE__, __LINE__, "Serial Port Commstate Error"));
					}
					else  {
						//throw istatus;
					}
					return;
					//break;
	
				case SERIAL_ERROR_CREATEEVENT1:
					if (log) log->write(10,0,1,"Serial Port Event Error 1\n");
					destruct();
					if (show_errors)  {
						MessageBox(NULL, "Serial Port Error\nSerial Port Event Error 1\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						throw(fatalError(__FILE__, __LINE__, "Serial Port Event Error 1"));
					}
					else  {
						//throw istatus;
					}
					return;
					//break;
	
				case SERIAL_ERROR_CREATEEVENT2:
					if (log) log->write(10,0,1,"Serial Port Event Error 2\n");
					destruct();
					if (show_errors)  {
						MessageBox(NULL, "Serial Port Error\nSerial Port Event Error 2\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						throw(fatalError(__FILE__, __LINE__, "Serial Port Event Error 2"));
					}
					else  {
						//throw istatus;
					}
					return;
					//break;
	
				case SERIAL_ERROR_OTHER:
				default:
					if (log) log->write(10,0,1,"Serial Port Unknown Error\n");
					destruct();
					if (show_errors)  {
						MessageBox(NULL, "Serial Port Error\nSerial Port Unknown Error 1\nPlease run Test Com Ports in the Options Screen to correct the problem.", "Error", MB_OK);
						throw(fatalError(__FILE__, __LINE__, "Serial Port Unknown Error"));
					}
					else  {
						//throw istatus;
					}
					return;
					//break;
				}					// switch()
			}						// catch

			if (log) log->write(10,0,1,"rtd: created client or port\n");
		#endif

#ifdef _DEBUG
	//printf("rtd4\n");
#endif

	if (log) log->write(10,0,1,"rtd: 5\n");


	if (logging_type==RAW_PERF_LOGGING)  {
		if (log) log->write(10,0,1,"raw\n");
#ifdef DOGLOBS
		sprintf(str, "%s%s%d.dat", dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, comportnum);
#else
		sprintf(str, "%s%s%d.dat", SDIRS::dirs[DIR_PERFORMANCES].c_str(), FILESEPSTR, comportnum);
#endif
		outstream = fopen(str, "wb");

		fwrite(&perf_file_header, sizeof(perf_file_header), 1, outstream);
		fwrite(&rinf, sizeof(rinf), 1, outstream);
		fwrite(&result, sizeof(result), 1, outstream);

		bp = 1;
	}
	else if (logging_type==THREE_D_PERF_LOGGING)  {
		if (log) log->write(10,0,1,"3d 1\n");
		create_perf_file();
		if (log) log->write(10,0,1,"3d 2\n");
	}
	else if (logging_type==RM1_PERF_LOGGING)  {
		if (log) log->write(10,0,1,"rm1\n");
		create_perf_file();
		if (log) log->write(10,0,1,"rm2\n");
		bp = 1;
	}
	else  {
		if (log) log->write(10,0,1,"rtd: 6\n");
	}

#ifdef DO_ANT
#ifdef WIN32

	//-------------------------------
	// ant heartrate sensor:
	//-------------------------------

	strcpy(hr.name, "Heartrate");
	hr.dev_type = HR_DEV_TYPE;
	hr.freq = 57;
	hr.period = 8070;
	hr.chan_num = ANT_HR;
	hr.trans_type = 0;
	hr.chan_type = 0;

	//-------------------------------
	// ant speed/cadence sensor:
	//-------------------------------

	strcpy(sc.name, "Speed Cadence");
	sc.dev_type = SC_DEV_TYPE;									// 121
	sc.freq = 57;
	sc.period = 8086;
	sc.chan_num = ANT_SC;							// 0
	//scant.trans_type = ANT_SC;							// 0
	sc.trans_type = 0;									// 0
	sc.chan_type = 0;										// 0
	//scant.type = 0;

	//-------------------------------
	// ant cadence sensor:
	//-------------------------------

	strcpy(cad.name, "Cadence");
	cad.dev_type = C_DEV_TYPE;							// 122
	cad.freq = 57;
	cad.period = 8102;
	cad.chan_num = ANT_C;								// channel 0
	cad.trans_type = 0;
	cad.chan_type = 0;

	
	#ifdef _DEBUG
		unsigned long tid = GetCurrentThreadId();
	#endif

	//if (ant_stick_initialized)  {
	if (asi)  {
		float diameter = 700.0f;									// mm
		float circumference = (float) (PI * diameter);		// mm
		ant_set_circumference(circumference);

		int status;

#ifdef _DEBUG
		ant_start_heartrate();
		ant_start_cad();
		ant_start_sc();
#else
		ant_start_heartrate();
		ant_start_cad();
		ant_start_sc();
#endif

#ifdef _DEBUG
		log_raw = true;
#endif

		sensors_initialized = true;
	}

#endif					// #ifdef WIN32
#endif					// #ifdef DO_ANT

	initialized = true;
	if (log) log->write(10,0,1,"rtd: x\n");
	return;
}																	// constructor 1

/**********************************************************************
	constructor #2
**********************************************************************/

RTD::RTD(
		 Serial *_port,
		 RIDER_DATA &_rd,
		 Course *_course,
		 bool _show_errors,
		 Bike *_bike,
		 LoggingType _logging_type
		 ) : dataSource(_rd, _course, _bike, _logging_type)  {

	char str[256];

#ifdef HELO_WRLD
	server = nullptr;
#endif

	//server = NULL;
	port = _port;
	caller_created_port = true;
#ifdef DO_RMP
	rmp = NULL;
	memset(&rmpdata, 0, sizeof(rmpdata));
#endif

	memset(url, 0, sizeof(url));									// constructor 2
	tcp_port = -1;
	ix = -1;

	sout = NULL;
	dsmetric = false;

	assert(log==NULL);
	id = port->getPortNumber();

	if (gblog)  {
		//strcpy(logpath, dirs[DIR_DEBUG]);
#ifdef DOGLOBS
		sprintf(str, "%s%sds%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
		sprintf(str, "%s%sds%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
		log = new Logger(str);													// subclasses may close this, delete it, and change the id number
		//strncpy(logfname, "ds.log", sizeof(logfname)-1);
		//log = new Logger(logfname, logpath);
		if (log) log->write(10,0,1,"rtd: logging started\n");
	}


	show_errors = _show_errors;
	//memset(dataFileName, 0, sizeof(dataFileName));

	/*
	if (user)  {
		lbs = user->data.lbs;
	}
	*/

	workbuf = NULL;
	outstream = NULL;
	memset(&rinf, 0, sizeof(rinf));
	memset(&perf_file_header, 0, sizeof(perf_file_header));

	perf_file_header.sec.type = SEC_PERF_FILE_HEADER;
	perf_file_header.sec.structsize = sizeof(perf_file_header);
	perf_file_header.sec.version = 0;

	memset(&result, 0, sizeof(result));
	result.sec.type = SEC_RESULT;
	result.sec.structsize = sizeof(result);

	rinf.sec.type = SEC_RIDER_INFO;
	rinf.sec.structsize = sizeof(rinf);

	rinf.pinf.sec.type = SEC_PERSON_INFO;
	rinf.pinf.sec.structsize = sizeof(rinf.pinf);

	rinf.pinf.age = rd.pd.age;							// double age;
	rinf.pinf.kgs = rd.pd.kgs;							// double kgs;
	rinf.pinf.sex = rd.pd.sex;							// char sex;
	strncpy(rinf.pinf.lastname, rd.pd.lastname.c_str(), sizeof(rinf.pinf.lastname)-1);			// char lastname[40];
	strncpy(rinf.pinf.firstname, rd.pd.firstname.c_str(), sizeof(rinf.pinf.firstname)-1);		// char firstname[40];
	strncpy(rinf.pinf.username, rd.pd.username.c_str(), sizeof(rinf.pinf.username)-1);			// char username[40];
	strncpy(rinf.pinf.email, rd.pd.email.c_str(), sizeof(rinf.pinf.email)-1);					// char email[64];
	strncpy(rinf.pinf.city, rd.pd.city.c_str(), sizeof(rinf.pinf.city)-1);						// char city[24];
	strncpy(rinf.pinf.country, rd.pd.country.c_str(), sizeof(rinf.pinf.country)-1);				// char country[24];
	strncpy(rinf.pinf.state, rd.pd.state.c_str(), sizeof(rinf.pinf.state)-1);					// char state[16];
	strncpy(rinf.pinf.zip, rd.pd.zip.c_str(), sizeof(rinf.pinf.zip)-1);							// char zip[16];

	rinf.lower_hr = rd.lower_hr;								// float
	rinf.upper_hr = rd.upper_hr;								// float

	//rinf.hr_beep_enabled = (bool)rd.hr_beep_enabled;					// bool
	//rinf.draft_enable = (bool)rd.draft_enable;						// bool

	if (rd.hr_beep_enabled)  {
		rinf.hr_beep_enabled = true;
	}
	else  {
		rinf.hr_beep_enabled = false;
	}

	if (rd.draft_enable)  {
		rinf.draft_enable = true;
	}
	else  {
		rinf.draft_enable = false;
	}
	rinf.drag_aerodynamic = rd.drag_aerodynamic;				// float
	rinf.drag_tire = rd.drag_tire;								// float
	rinf.rfdrag = rd.cal;										// unsigned short for computrainer
	rinf.watts_factor = rd.watts_factor;						// float
	rinf.ftp = rd.ftp;											// float

	RTD::reset();

	comportnum = port->getPortNumber();


	//sprintf(outfilename, ".%sperfs%s%d.3dp", FILESEPSTR, FILESEPSTR, comportnum);
	//sprintf(outfilename, "%s%s%d.3dp", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);
	//sprintf(outfilename, "%s%s%d.dat", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);
#ifdef _DEBUG
	//printf("dirs[DIR_PERFORMANCES] = %s\n", dirs[DIR_PERFORMANCES]);
#endif

	dataStart = 0;
	CLEN = 1024;
	lastCommTime = 0;
	lastHWTime = 0;
	packetIndex = 0;
	recordsOut = 0;
	saved = false;
	dsmetric = false;

	workbuf = new unsigned char[(unsigned int)CLEN];
	for(int i=0;i<CLEN;i++)  workbuf[i] = 0;

#ifndef WIN32
	/*
	SerialPort_DCS dcs;
	//const char *devname = wxCOM1;
	port = new Serial();
	if (!strcmp(_baudstr, "2400"))  {
		dcs.baud = wxBAUD_2400;
	}
	else  if (!strcmp(_baudstr, "38400"))  {
		dcs.baud = wxBAUD_38400;
	}
	else  {
		dcs.baud = wxBAUD_2400;
	}
	
	dcs.parity = wxPARITY_NONE;
	dcs.wordlen = 8;
	dcs.stopbits = 1;
	dcs.rtscts = false;
	dcs.xonxoff = false;

	//printf("rtd trying to open serial port at 2400 baud\n");

        //char str[32];
        //sprintf(str, "/dev/ttyS%d", comportnum-1);


      if(port->Open(_comportstr, &dcs) < 0) {
         DEL(port);
         return;
      }
      else  {
         printf("opened %s\n", _comportstr);
      }
	*/
	
#endif
	if (log) log->write(10,0,1,"rtd x\n");
	initialized = true;
}								// constructor 2


/**********************************************************************
	constructor #3
**********************************************************************/

#if 1
RTD::RTD(
		int _ix,
		 //Server *_server,
#ifdef HELO_WRLD
	RACERMATE::Ctsrv *_server,
#endif
		 RIDER_DATA &_rd,
		 Course *_course,
		 bool _show_errors,
		 Bike *_bike,
		 LoggingType _logging_type
		 ) : dataSource(_rd, _course, _bike, _logging_type)  {

	char str[256];

	ix = _ix;
#ifdef HELO_WRLD
	server = _server;
#endif
	port = NULL;											// constructor 3
	caller_created_port = true;
#ifdef DO_RMP
	rmp = NULL;
	memset(&rmpdata, 0, sizeof(rmpdata));
#endif

	memset(url, 0, sizeof(url));
	tcp_port = -1;

	sout = NULL;
	dsmetric = false;

	assert(log==NULL);

#ifdef HELO_WRLD
	id = server->getPortNumber();							// 221 hardcoded for now
#endif

	if (gblog)  {
		//strcpy(logpath, dirs[DIR_DEBUG]);
#ifdef DOGLOBS
		sprintf(str, "%s%sds%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
		sprintf(str, "%s%sds%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
		log = new Logger(str);													// subclasses may close this, delete it, and change the id number
		//strncpy(logfname, "ds.log", sizeof(logfname)-1);
		//log = new Logger(logfname, logpath);
		if (log) log->write(10,0,1,"rtd: logging started\n");
	}


	show_errors = _show_errors;
	//memset(dataFileName, 0, sizeof(dataFileName));

	/*
	if (user)  {
		lbs = user->data.lbs;
	}
	*/

	workbuf = NULL;
	outstream = NULL;
	memset(&rinf, 0, sizeof(rinf));
	memset(&perf_file_header, 0, sizeof(perf_file_header));

	perf_file_header.sec.type = SEC_PERF_FILE_HEADER;
	perf_file_header.sec.structsize = sizeof(perf_file_header);
	perf_file_header.sec.version = 0;

	memset(&result, 0, sizeof(result));
	result.sec.type = SEC_RESULT;
	result.sec.structsize = sizeof(result);

	rinf.sec.type = SEC_RIDER_INFO;
	rinf.sec.structsize = sizeof(rinf);

	rinf.pinf.sec.type = SEC_PERSON_INFO;
	rinf.pinf.sec.structsize = sizeof(rinf.pinf);

	rinf.pinf.age = rd.pd.age;							// double age;
	rinf.pinf.kgs = rd.pd.kgs;							// double kgs;
	rinf.pinf.sex = rd.pd.sex;							// char sex;
	strncpy(rinf.pinf.lastname, rd.pd.lastname.c_str(), sizeof(rinf.pinf.lastname)-1);			// char lastname[40];
	strncpy(rinf.pinf.firstname, rd.pd.firstname.c_str(), sizeof(rinf.pinf.firstname)-1);		// char firstname[40];
	strncpy(rinf.pinf.username, rd.pd.username.c_str(), sizeof(rinf.pinf.username)-1);			// char username[40];
	strncpy(rinf.pinf.email, rd.pd.email.c_str(), sizeof(rinf.pinf.email)-1);					// char email[64];
	strncpy(rinf.pinf.city, rd.pd.city.c_str(), sizeof(rinf.pinf.city)-1);						// char city[24];
	strncpy(rinf.pinf.country, rd.pd.country.c_str(), sizeof(rinf.pinf.country)-1);				// char country[24];
	strncpy(rinf.pinf.state, rd.pd.state.c_str(), sizeof(rinf.pinf.state)-1);					// char state[16];
	strncpy(rinf.pinf.zip, rd.pd.zip.c_str(), sizeof(rinf.pinf.zip)-1);							// char zip[16];

	rinf.lower_hr = rd.lower_hr;								// float
	rinf.upper_hr = rd.upper_hr;								// float

	//rinf.hr_beep_enabled = (bool)rd.hr_beep_enabled;					// bool
	//rinf.draft_enable = (bool)rd.draft_enable;						// bool

	if (rd.hr_beep_enabled)  {
		rinf.hr_beep_enabled = true;
	}
	else  {
		rinf.hr_beep_enabled = false;
	}

	if (rd.draft_enable)  {
		rinf.draft_enable = true;
	}
	else  {
		rinf.draft_enable = false;
	}
	rinf.drag_aerodynamic = rd.drag_aerodynamic;				// float
	rinf.drag_tire = rd.drag_tire;								// float
	rinf.rfdrag = rd.cal;										// unsigned short for computrainer
	rinf.watts_factor = rd.watts_factor;						// float
	rinf.ftp = rd.ftp;											// float

	RTD::reset();

	//comportnum = server->getPortNumber();


	//sprintf(outfilename, ".%sperfs%s%d.3dp", FILESEPSTR, FILESEPSTR, comportnum);
	//sprintf(outfilename, "%s%s%d.3dp", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);
	//sprintf(outfilename, "%s%s%d.dat", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);
#ifdef _DEBUG
	//printf("dirs[DIR_PERFORMANCES] = %s\n", dirs[DIR_PERFORMANCES]);
#endif

	dataStart = 0;
	CLEN = 1024;
	lastCommTime = 0;
	lastHWTime = 0;
	packetIndex = 0;
	recordsOut = 0;
	saved = false;
	dsmetric = false;

	workbuf = new unsigned char[(unsigned int)CLEN];
	for(int i=0;i<CLEN;i++)  workbuf[i] = 0;

#ifndef WIN32
	/*
	SerialPort_DCS dcs;
	//const char *devname = wxCOM1;
	port = new Serial();
	if (!strcmp(_baudstr, "2400"))  {
		dcs.baud = wxBAUD_2400;
	}
	else  if (!strcmp(_baudstr, "38400"))  {
		dcs.baud = wxBAUD_38400;
	}
	else  {
		dcs.baud = wxBAUD_2400;
	}
	
	dcs.parity = wxPARITY_NONE;
	dcs.wordlen = 8;
	dcs.stopbits = 1;
	dcs.rtscts = false;
	dcs.xonxoff = false;

	//printf("rtd trying to open serial port at 2400 baud\n");

        //char str[32];
        //sprintf(str, "/dev/ttyS%d", comportnum-1);


      if(port->Open(_comportstr, &dcs) < 0) {
         DEL(port);
         return;
      }
      else  {
         printf("opened %s\n", _comportstr);
      }
	*/
	
#endif
	if (log) log->write(10,0,1,"rtd x\n");
	initialized = true;
}								// constructor 3
#endif				// #if 1



/**********************************************************************
	constructor #4
	called by read_computrainer() in internal.cpp
	computrainer constructor #4 in computrainer.cpp
**********************************************************************/

RTD::RTD(
		 Client *_client,
		 RIDER_DATA &_rd,
		 Course *_course,
		 bool _show_errors,
		 Bike *_bike,
		 LoggingType _logging_type
		 ) : dataSource(_rd, _course, _bike, _logging_type)  {

	char str[256];

	client = _client;
#ifdef HELO_WRLD
	server = NULL;
#endif
	port = NULL;
	caller_created_port = true;
	//rmp = NULL;
	memset(url, 0, sizeof(url));
	tcp_port = -1;
	ix = -1;

	sout = NULL;
	dsmetric = false;

	assert(log==NULL);
	id = client->getPortNumber();

	if (gblog)  {
		//strcpy(logpath, dirs[DIR_DEBUG]);
#ifdef DOGLOBS
		sprintf(str, "%s%sds%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
		sprintf(str, "%s%sds%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
		log = new Logger(str);													// subclasses may close this, delete it, and change the id number
		//strncpy(logfname, "ds.log", sizeof(logfname)-1);
		//log = new Logger(logfname, logpath);
		if (log) log->write(10,0,1,"rtd: logging started\n");
	}


	show_errors = _show_errors;
	//memset(dataFileName, 0, sizeof(dataFileName));

	/*
	if (user)  {
		lbs = user->data.lbs;
	}
	*/

	workbuf = NULL;
	outstream = NULL;
	memset(&rinf, 0, sizeof(rinf));
	memset(&perf_file_header, 0, sizeof(perf_file_header));

#ifdef DO_RMP
	memset(&rmpdata, 0, sizeof(rmpdata));
#endif

	perf_file_header.sec.type = SEC_PERF_FILE_HEADER;
	perf_file_header.sec.structsize = sizeof(perf_file_header);
	perf_file_header.sec.version = 0;

	memset(&result, 0, sizeof(result));
	result.sec.type = SEC_RESULT;
	result.sec.structsize = sizeof(result);

	rinf.sec.type = SEC_RIDER_INFO;
	rinf.sec.structsize = sizeof(rinf);

	rinf.pinf.sec.type = SEC_PERSON_INFO;
	rinf.pinf.sec.structsize = sizeof(rinf.pinf);

	rinf.pinf.age = rd.pd.age;							// double age;
	rinf.pinf.kgs = rd.pd.kgs;							// double kgs;
	rinf.pinf.sex = rd.pd.sex;							// char sex;
	strncpy(rinf.pinf.lastname, rd.pd.lastname.c_str(), sizeof(rinf.pinf.lastname)-1);			// char lastname[40];
	strncpy(rinf.pinf.firstname, rd.pd.firstname.c_str(), sizeof(rinf.pinf.firstname)-1);		// char firstname[40];
	strncpy(rinf.pinf.username, rd.pd.username.c_str(), sizeof(rinf.pinf.username)-1);			// char username[40];
	strncpy(rinf.pinf.email, rd.pd.email.c_str(), sizeof(rinf.pinf.email)-1);					// char email[64];
	strncpy(rinf.pinf.city, rd.pd.city.c_str(), sizeof(rinf.pinf.city)-1);						// char city[24];
	strncpy(rinf.pinf.country, rd.pd.country.c_str(), sizeof(rinf.pinf.country)-1);				// char country[24];
	strncpy(rinf.pinf.state, rd.pd.state.c_str(), sizeof(rinf.pinf.state)-1);					// char state[16];
	strncpy(rinf.pinf.zip, rd.pd.zip.c_str(), sizeof(rinf.pinf.zip)-1);							// char zip[16];

	rinf.lower_hr = rd.lower_hr;								// float
	rinf.upper_hr = rd.upper_hr;								// float

	//rinf.hr_beep_enabled = (bool)rd.hr_beep_enabled;					// bool
	//rinf.draft_enable = (bool)rd.draft_enable;						// bool

	if (rd.hr_beep_enabled)  {
		rinf.hr_beep_enabled = true;
	}
	else  {
		rinf.hr_beep_enabled = false;
	}

	if (rd.draft_enable)  {
		rinf.draft_enable = true;
	}
	else  {
		rinf.draft_enable = false;
	}
	rinf.drag_aerodynamic = rd.drag_aerodynamic;				// float
	rinf.drag_tire = rd.drag_tire;								// float
	rinf.rfdrag = rd.cal;										// unsigned short for computrainer
	rinf.watts_factor = rd.watts_factor;						// float
	rinf.ftp = rd.ftp;											// float

	RTD::reset();

	comportnum = client->getPortNumber();


	//sprintf(outfilename, ".%sperfs%s%d.3dp", FILESEPSTR, FILESEPSTR, comportnum);
	//sprintf(outfilename, "%s%s%d.3dp", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);
	//sprintf(outfilename, "%s%s%d.dat", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);
#ifdef _DEBUG
	//printf("dirs[DIR_PERFORMANCES] = %s\n", dirs[DIR_PERFORMANCES]);
#endif

	dataStart = 0;
	CLEN = 1024;
	lastCommTime = 0;
	lastHWTime = 0;
	packetIndex = 0;
	recordsOut = 0;
	saved = false;
	dsmetric = false;

	workbuf = new unsigned char[(unsigned int)CLEN];
	for(int i=0;i<CLEN;i++)  workbuf[i] = 0;

#ifndef WIN32
	/*
	SerialPort_DCS dcs;
	//const char *devname = wxCOM1;
	port = new Serial();
	if (!strcmp(_baudstr, "2400"))  {
		dcs.baud = wxBAUD_2400;
	}
	else  if (!strcmp(_baudstr, "38400"))  {
		dcs.baud = wxBAUD_38400;
	}
	else  {
		dcs.baud = wxBAUD_2400;
	}
	
	dcs.parity = wxPARITY_NONE;
	dcs.wordlen = 8;
	dcs.stopbits = 1;
	dcs.rtscts = false;
	dcs.xonxoff = false;

	//printf("rtd trying to open serial port at 2400 baud\n");

        //char str[32];
        //sprintf(str, "/dev/ttyS%d", comportnum-1);


      if(port->Open(_comportstr, &dcs) < 0) {
         DEL(port);
         return;
      }
      else  {
         printf("opened %s\n", _comportstr);
      }
	*/
	
#endif
	if (log) log->write(10,0,1,"rtd x\n");
	initialized = true;
}								// constructor 4


/**********************************************************************
	destructor
**********************************************************************/

RTD::~RTD()  {
	//lint -save -e1740
	destruct();
	//lint -restore
}

/**********************************************************************

**********************************************************************/

void RTD::destruct(void)  {

#ifdef DO_RMP

	if (rmp)  {
		char str[MAX_PATH];
		rmp->finalize();						// renames rmp->outfilename
		strncpy(str, rmp->outfilename, sizeof(str)-1);

		#ifdef _DEBUG
			/*
			//rmp->dump("final.txt");			// done in finalize()
			if (rmp->fname)  {
				const char *cptr = rmp->fname;
				strncpy(outfilename, rmp->fname, sizeof(outfilename)-1);
			}
			*/
		#endif

		DEL(rmp);

		#ifdef _DEBUG
			if (exists(str))  {
				//rmp = new RMP("rmp_load.log");
				rmp = new RMP();
				//strncpy(rmp->outfilename, str, sizeof(rmp->outfilename)-1);
				rmp->load(str);
				DEL(rmp);
				CopyFile(str, "C:\\Users\\larry\\Documents\\RacerMate\\Performances\\x.rmp", FALSE);
			}
		#endif
	}
#endif

	DEL(sout);

	if (workbuf)  {
		delete[] workbuf;
		workbuf = NULL;
	}

	if (!caller_created_port)  {
		if (port)  {
			delete port;
			port = NULL;
		}
		DEL(client);
	#ifdef HELO_WRLD
		DEL(server);
	#endif

	}

	// outfilename, dataFileName

	if (outstream)  {
		FCLOSE(outstream);
	}

	//close_outstream();

#ifdef _DEBUG
	//unlink(outfilename);
#endif


	bp = 0;

	return;
}										// destruct()

/*************************************************************************

*************************************************************************/

void RTD::set_circumference(float _circumference)  {
#ifdef DO_ANT
	ant_set_circumference(_circumference);
#endif
	return;
}

#if 0
/*************************************************************************

*************************************************************************/

void RTD::set_cadence_sensor_params(float _circumference)  {

	if (ant_stick_initialized)  {
		int status = ant_add_sc(scant);
	}
}

/*************************************************************************

*************************************************************************/

void RTD::set_use_hr_sensor(bool _b)  {

//	FILE *logstream = fopen("foo.txt", "a+t");
//	fprintf(logstream, "suhrs\n");
//	fclose(logstream);

	if (ant_stick_initialized)  {
//fprintf(logstream, "ant_initialized\n");
		int status = ant_add_heartrate(hrant);
	}

//fclose(logstream);
	return;
}

#endif





/*************************************************************************

*************************************************************************/

void RTD::reset(void)  {

	dataSource::reset();

	if (decoder)  {
		decoder->set_started(false);
		decoder->set_finished(false);
		decoder->set_paused(true);
	}

	saved = false;
	finalized = false;

#ifdef WIN32
	if (port)  {
		port->flush();
	}
	//if (server)  {
	//	server->flush();
	//}
#endif

	//int n1, n2, n3;
	//n1 = winds.size();
	//n2 = calories.size();
	#ifdef VELOTRON
		//n3 = gi.size();
		while(gi.size())  {
			gi.pop_back();
		}
	#endif

#ifdef _DEBUG
		//if (n1>0)  {
		//	bp = 1;
		//}
#endif

	winds.clear();
	calories.clear();
	/*
	while(winds.size())  {
		winds.pop_back();
	}
	while(calories.size())  {
		calories.pop_back();
	}
	*/
	pinf.perfcount = 0;

	recordsOut = 0;

	return;
}					// reset()


/*************************************************************************
	called from velotronRTD::save()
*************************************************************************/

void RTD::resetOutputFile(void)  {
	int i, status;
	FILE *is, *os;
	char tmpfname[256];

	strncpy(tmpfname, outfilename, sizeof(tmpfname)-1);
	strip_extension(tmpfname);
	strcat(tmpfname, ".cpy");

	if (outstream != NULL)  {
		fclose(outstream);													// close .rtd
		outstream = NULL;
	}


	//int sz = filesize_from_name(outfilename);
	is = fopen(outfilename, "rb");										// ".rtd"
	if (is==NULL)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	os = fopen(tmpfname, "wb");

	for(i=0;i<dataStart;i++)  {
		status = fgetc(is);							// read from ".rtd"
		if (status==EOF)  {
			fclose(os);
			fclose(is);
			throw(fatalError(__FILE__, __LINE__));
		}
		fputc(status, os);							// write to tmpfname
	}

	fclose(is);
	fclose(os);

	//int sz1, sz2;

//	sz1 = filesize_from_name(tmpfname);
//	sz2 = filesize_from_name(outfilename);

	CopyFile(tmpfname, outfilename, FALSE);					// copy "1.cpy" to "1.tmp"
	rm(tmpfname);												// get rid of "1.cpy"

//	sz = filesize_from_name(outfilename);

	outstream = fopen(outfilename, "r+b");						// open "1.tmp"
	long offs;

	offs = ftell(outstream);									// 0
	file_size = dataStart;

	fseek(outstream, 0L, SEEK_END);

	offs = ftell(outstream);
	assert(offs == dataStart);

	return;
}

/*************************************************************************

*************************************************************************/

void RTD::setShutDownDelay(unsigned long _ms)  {
#ifdef WIN32
	if (port)  {
		port->setShutDownDelay(_ms);
	}
#endif
	return;
}


/*************************************************************************

*************************************************************************/

/*
void RTD::close_outstream(void)  {
	if (outstream)  {
		fflush(outstream);
	}
	FCLOSE(outstream);
	return;
}
*/



/*************************************************************************

*************************************************************************/

void RTD::write_string_section(const char *secstr, const char *str)  {
	int type, seclen, slen;

	type = (int)make_section_code(secstr);
	fwrite(&type, 4, 1, outstream);

	slen = (int)strlen(str);

	seclen = 4 + slen;
	fwrite(&seclen, 4, 1, outstream);

	fwrite(&slen, 4, 1, outstream);
	fwrite(str, 1, strlen(str), outstream);

	return;
}

/*************************************************************************

*************************************************************************/

void RTD::write_long_section(const char *str, long i)  {
	int type, seclen;
	type = (int)make_section_code(str);
	fwrite(&type, 4, 1, outstream);
	seclen = 4;
	fwrite(&seclen, 4, 1, outstream);
	fwrite(&i, 4, 1, outstream);
	return;
}

/*************************************************************************

*************************************************************************/

void RTD::write_float_section(const char *str, float f)  {
	int type, seclen;

	type = (int)make_section_code(str);
	fwrite(&type, 4, 1, outstream);
	seclen = 4;
	fwrite(&seclen, 4, 1, outstream);
	fwrite(&f, 4, 1, outstream);
	return;
}

/*************************************************************************

*************************************************************************/

void RTD::write_floats_section(const char *str, std::vector <float> &vec)  {
	int i, type;
	int vsize;
	int seclen;

	type = (int)make_section_code(str);
	fwrite(&type, 4, 1, outstream);

	vsize = (int)vec.size();
	seclen = (int) (
							(int)sizeof(int) + 
							((int)sizeof(float)*vsize)
						);			// number of elements in vector + the vector itself


	fwrite(&seclen, 4, 1, outstream);
	fwrite(&vsize, 4, 1, outstream);

	for(i=0; i<vsize; i++)  {
		fwrite(&vec[i], sizeof(float), 1, outstream);
	}

	return;
}


/*************************************************************************

*************************************************************************/

void RTD::write_gear_section(void)  {
	int i, type;
	int vsize;
	int seclen;

	if (gi.size()==0)  {
		return;
	}


	type = (int)make_section_code("GEAR");
	fwrite(&type, 4, 1, outstream);

#ifdef _DEBUG
	vsize = (int)winds.size();
	vsize = (int)calories.size();
	vsize = sizeof(GEARINFO);
#endif

	vsize = (int)gi.size();
	seclen = (int) (
							(int)sizeof(int) + 
							((int)sizeof(GEARINFO)*vsize)
						);			// number of elements in vector + the vector itself


	fwrite(&seclen, 4, 1, outstream);
	fwrite(&vsize, 4, 1, outstream);

	for(i=0; i<vsize; i++)  {
		fwrite(&gi[i], sizeof(GEARINFO), 1, outstream);
	}

	return;
}


/********************************************************************************

********************************************************************************/

unsigned long RTD::make_section_code(const char *str)  {
	unsigned long ul;
	ul = ((unsigned long)str[3]) << 24;
	ul |= str[2] << 16;
	ul |= str[1] << 8;
	ul |= str[0];
	return ul;
}

/*************************************************************************
	rewrite pinf because some things have changed in it
*************************************************************************/

#if 0
void RTD::finalize_3dp(void)  {
	int seclen;
	//int type;
	int pilen, pplen;
	//int i;
	long offs;
	char str[256];

	if (!outstream)  {
		return;
	}


	if (finalized)  {
		return;
	}
	finalized = true;

	if (outstream==NULL)  {
		return;
	}

	if (recordsOut==0L)  {
		FCLOSE(outstream);
		unlink(outfilename);
		return;
	}

#ifdef _DEBUG
	FILE *fp=NULL;
	sprintf(str, "%s%sfinalize.txt", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
	fp = fopen(str, "wt");
#endif

	pilen = sizeof(PerformanceInfo);
	pplen = sizeof(PerfPoint);

	pinf.perfcount = recordsOut;
	pinf.length = decoder->meta.miles - decoder->get_startMiles();
	pinf.length = (float)(MILESTOMETERS * pinf.length);					// stored as meters
	pinf.racetime = decoder->meta.time;		// timeGetTime() - decoder->startTime;
#ifndef NEWUSER
	pinf.ftp = user->ftp;
	strncpy(pinf.name, user->data.name, sizeof(pinf.name));
	pinf.gender = user->data.sex[0];
	pinf.age = user->data.age;
	pinf.weight = user->data.kgs;
	pinf.upper_hr = (long) ROUND(user->data.upper_hr);
	pinf.lower_hr = (long) ROUND(user->data.lower_hr);
	pinf.watts_factor = user->get_watts_factor();
#else
	pinf.ftp = rd.ftp;
	sprintf(str, "%s %s",rd.pd.firstname.c_str(), rd.pd.lastname.c_str());
	strncpy(pinf.name, str, sizeof(pinf.name));
	pinf.gender = rd.pd.sex;
	pinf.age = (unsigned char)ROUND(rd.pd.age);
	pinf.weight = (float)rd.pd.kgs;
	pinf.upper_hr = (long) ROUND(rd.upper_hr);
	pinf.lower_hr = (long) ROUND(rd.lower_hr);
	pinf.watts_factor = rd.watts_factor;
#endif

	pinf.version = 2;
	pinf.height = 0.0f;
	pinf.rfDrag = decoder->meta.rfdrag & 0x07ff;
	pinf.rfMeas = decoder->meta.rfmeas;

#ifdef _DEBUG
	fprintf(fp, "perfinfo length = %d\n", pilen);
	fprintf(fp, "pp length = %d\n", pplen);
	fprintf(fp, "perflenoffset = %ld\n", perflenoffset);
	fprintf(fp, "pinfoffset = %ld\n", pinfoffset);
	fprintf(fp, "perfoffset = %ld\n", perfoffset);
	fprintf(fp, "perfpoints = %ld\n", pinf.perfcount);
#endif

	fseek(outstream, perflenoffset, SEEK_SET);												// 8
	seclen = pilen + pinf.perfcount*pplen;
	fwrite(&seclen, 4, 1, outstream);

	offs = ftell(outstream);
	assert(offs == pinfoffset);

	fwrite(&pinf, sizeof(PerformanceInfo), 1, outstream);

	offs = ftell(outstream);
	assert(offs == perfoffset);

	// go to the end of the file and write any other sections
	// course, ssrt

	offs = ftell(outstream);
	fseek(outstream, 0L, SEEK_END);				// now seeking past the perf section
	offs = ftell(outstream);

#ifdef _DEBUG
	fprintf(fp, "cals starts at %ld\n", offs);
#endif

	write_floats_section("CALS", calories);

	write_long_section("LAPS", 1);
	write_floats_section("Wind", winds);

#ifdef _DEBUG
	int size;
	size = ftell(outstream) - offs;
	fprintf(fp, "cals size = %d bytes\n", size );
	fprintf(fp, "cals ends at %ld\n", ftell(outstream) );
#endif

	write_gear_section();

	if (decoder->get_startMiles() > 0.0f)  {
		write_float_section("SSTR", (float) (MILESTOMETERS*decoder->get_startMiles()) );
	}

#ifdef _DEBUG
	FCLOSE(fp);
#endif

	add_course();

	FCLOSE(outstream);


	//close_outstream();			// unless the parent does this vista was giving an error!!!

	return;
}						// finalize_3dp()

#endif




/*************************************************************************
	adds a .crs file
*************************************************************************/

int RTD::add_course(void)  {
	unsigned long type;
//	unsigned long seclen;

	assert(outstream);
	if (!course)  {
		return 0;
	}


	fseek(outstream, 0L, SEEK_END);

	type = make_section_code("_CRS");
	fwrite(&type, 4, 1, outstream);

	course->write("", outstream);

	//seclen = 0L;


	/*
	vsize = vec.size();
	seclen = (int) (
							(int)sizeof(int) + 
							((int)sizeof(float)*vsize)
						);			// number of elements in vector + the vector itself


	fwrite(&seclen, 4, 1, outstream);
	fwrite(&vsize, 4, 1, outstream);

	for(i=0; i<vsize; i++)  {
		fwrite(&vec[i], sizeof(float), 1, outstream);
	}

	return;
	*/


	return 0;
}						// int RTD::addcourse(void)  {


/*************************************************************************
	creates a 3dp or rmp compatible file
*************************************************************************/

void RTD::create_perf_file(void)  {
	unsigned long version, type, seclen=0L;

	FCLOSE(outstream);
	if (outfilename[0] == 0)  {
		return;
	}

	//char str[256];
	//sprintf(str, "%s%s%d.3dp", dirs[DIR_PERFORMANCES], FILESEPSTR, comportnum);


	if (logging_type==THREE_D_PERF_LOGGING)  {
		outstream = fopen(outfilename, "wb");
		if (outstream==NULL)  {
			return;
		}

		winds.clear();
		calories.clear();

		memset(&pinf, 0, sizeof(pinf));

		version = 6;
		type = make_section_code("perf");

		fwrite(&version, sizeof(unsigned long), 1, outstream);
		fwrite(&type, sizeof(type), 1, outstream);
		perflenoffset = ftell(outstream);
		fwrite(&seclen, sizeof(seclen), 1, outstream);
		pinfoffset = ftell(outstream);						// 12

		fwrite(&pinf, sizeof(PerformanceInfo), 1, outstream);
		perfoffset = ftell(outstream);						// 248
	}
#ifdef DO_RMP
	else if (logging_type==RM1_PERF_LOGGING)  {
		//int status;
		//rmp = new RMP("rmp_write.log");
		rmp = new RMP();

	    if (rmp->outstream)  {
			throw(fatalError(__FILE__, __LINE__, "rmp error 1"));
	    }

	    rmp->outstream = fopen(outfilename, "w+b");
		strncpy(rmp->outfilename, outfilename, sizeof(rmp->outfilename)-1);

	    if (rmp->outstream==NULL)  {
			throw(fatalError(__FILE__, __LINE__, "rmp error 2"));
	    }

	    //-----------------------------------------------------
		// RMHEADER
	    //-----------------------------------------------------

		memset(&rmp->rmheader, 0, sizeof(rmp->rmheader));

		rmp->rmheader.tag = fourcc("RM1X");
		rmp->rmheader.version = rmp->VERSION;
		rmp->compute_date(rmp->rmheader.date);
		strncpy(rmp->rmheader.creator_exe, "TTS", sizeof(rmp->rmheader.creator_exe)-1);
		strncpy(rmp->rmheader.copyright, rmp->COPYRIGHT, sizeof(rmp->rmheader.copyright)-1);
		strncpy(rmp->rmheader.comment, "Performance File", sizeof(rmp->rmheader.comment)-1);
		rmp->rmheader.compress_type = 0;

	    int n = (int)fwrite(&rmp->rmheader, sizeof(rmp->rmheader), 1, rmp->outstream);
		if (n != 1)  {
			throw(fatalError(__FILE__, __LINE__, "rmp write error"));
		}

		rmp->version = rmp->rmheader.version / 1000.0f;

		//long offs;

		//offs = ftell(rmp->outstream);

		/*
		RIDER_DATA &_rd, 
		Course *_course, 
		Bike *_bike,
		*/

		//-----------------------------------------------------
		// PERFINFO
		//-----------------------------------------------------

		memset(&rmp->info, 0, sizeof(rmp->info));

		rmp->info.tag = fourcc("RMPI");							// INT32
		rmp->info.Age = (INT16)ROUND(rd.pd.age);				// INT16
		rmp->info.Height = (INT16) rd.pd.cm;					// INT16
		rmp->info.Weight = (INT16)rd.pd.kgs;					// INT16
		rmp->info.Gender = rd.pd.sex;							// BYTE
		rmp->info.HeartRate = 0;								// BYTE
		rmp->info.Upper_HeartRate = (BYTE)ROUND(rd.upper_hr);	// BYTE
		rmp->info.Lower_HeartRate = (BYTE)ROUND(rd.lower_hr);	// BYTE
		rmp->info.CourseType = 5;								// INT16
		rmp->info.Distance = 0.0;								// double
		rmp->info.RFDrag = rd.drag_tire;						// float
		rmp->info.RFMeas = 0.0f;								// float
		rmp->info.Watts_Factor = rd.watts_factor;				// float
		rmp->info.FTP = rd.ftp;									// float
		rmp->info.PerfCount = 0;								// INT32
		rmp->info.TimeMS = 0LL;									// UINT64
		rmp->info.CourseOffset = 0LL;							// INT64

		strncpy(rmp->info.CourseName, "coursename", sizeof(rmp->info.CourseName));

		//const char *fname, *lname;
		//fname = rd.pd.firstname.c_str();
		//lname = rd.pd.lastname.c_str();

		sprintf(rmp->info.RiderName, "%s %s", rd.pd.firstname.c_str(), rd.pd.lastname.c_str());

		n = (int)fwrite(&rmp->info, sizeof(rmp->info), 1, rmp->outstream);
		if (n != 1)  {
			throw(fatalError(__FILE__, __LINE__, "rmp write error"));
		}

		//offs = ftell(rmp->outstream);

		//-----------------------------------------------------
		// PERFINFOEXT1
		//-----------------------------------------------------

		if (rmp->rmheader.version > 1010)  {
			rmp->infoext.Tag = fourcc("RMPX");					// INT32
			rmp->infoext.Mode = 0;								// UINT16
			rmp->infoext.RawCalibrationValue = (INT16)rd.cal;	// INT16
			rmp->infoext.DragFactor = 0;						// INT16
			rmp->infoext.DeviceType = 0;						// UINT16
			rmp->infoext.DeviceVersion = 0;						// UINT16
			rmp->infoext.PowerAeT = 0;							// UINT16
			rmp->infoext.PowerFTP = 0;							// UINT16
			rmp->infoext.HrAeT = 0;								// BYTE
			rmp->infoext.HrMin = 0;								// BYTE
			rmp->infoext.HrMax = 0;								// BYTE
			rmp->infoext.HrZone1 = 0;							// BYTE
			rmp->infoext.HrZone2 = 0;							// BYTE
			rmp->infoext.HrZone3 = 0;							// BYTE
			rmp->infoext.HrZone4 = 0;							// BYTE
			rmp->infoext.HrZone5 = 0;							// BYTE

			n = (int)fwrite(&rmp->infoext, sizeof(rmp->infoext), 1, rmp->outstream);
			if (n != 1)  {
				throw(fatalError(__FILE__, __LINE__, "rmp write error"));
			}

			//offs = ftell(rmp->outstream);
		}                                  // if (rmheader.version > 1.01f)  {

		rmp->data_start = ftell(rmp->outstream);

		//rmp->set_drag_factor((BYTE)ROUND(100.0f*rd.watts_factor));
//#ifdef _DEBUG
#if 0
		FCLOSE(rmp->outstream);
		//rmp->dump(outfilename);
		status = rmp->load(outfilename, NULL);
		assert(status==RMP::WRONG_TAG);
		rmp->outstream = fopen(outfilename, "a+b");
		fseek(rmp->outstream, 0L, SEEK_END);
		offs = ftell(rmp->outstream);
		bp = 1;
#endif
	}						// else if (logging_type==RM1_PERF_LOGGING)
#endif				// #ifdef DO_RMP


	return;
}							// void RTD::create_perf_file(void)  {



/*************************************************************************

*************************************************************************/

void RTD::write_encrypted_section(unsigned char *buf)  {
	CRF *s = NULL;
	//unsigned long size;
	//unsigned long type;



	if (!outstream)  {
		return;
	}


	s = new CRF();

	s->init();


	s->doo((unsigned char *) &buf, sizeof(RIDER_INFO));

	fwrite(&rinf, sizeof(rinf), 1, outstream);

	DEL(s);

	return;
}

/*************************************************************************

*************************************************************************/

const char *RTD::get_perf_file_name(void) {
	return outfilename;
}
