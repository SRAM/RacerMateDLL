
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
    #pragma warning(disable:4996 4006)
#endif

/*************************************************************************************

1) chan 1 and chan 2 seem to resume searching after the signal
is lost but chan 0 does not.


2) very odd hr reading, lots of zeroes between an occasional 73?
Maybe have to watch the "message toggle bit"?

3) many "collisions", with two sensors running - how does the radio
return the various channels? Do you have to poll them?

I guess I have to look at the code and the ant specs.



pclSerialObject = new DSISerialGeneric();
	pclSerialObject->Init(ant_baud, devnum)

framer = new DSIFramerANT(pclSerialObject)

pclSerialObject->SetCallback(framer);


DSIFramerANT
	has-a DSISerialGeneric

	methods:
		resetSystem()
		setNetworkKey()

DSISerialGeneric
	variables:
		DSISerialCallback* pclCallback
		const USBDevice* pclDevice;

	methods:
		Open() must have a pclCallback, opens a pclDevice


*****************************************************************************************/

#include <assert.h>
#include <time.h>

	#include <map>

#ifdef WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <shlobj.h>
	#include <conio.h>
	#include <psapi.h>
	#include <tlhelp32.h>
	#include <crtdbg.h>
	#include <new.h>
	#include <seh.h>
#else
    #include <setjmp.h>
    #include <signal.h>
#endif

#include <vector>
#include <string>

#include <stdlib.h>


#include <utils.h>
#include <logger.h>

#include <dll.h>
#include <ev.h>
#include <computrainer.h>

//#include <vutils.h>
#include <errors.h>
#include <fatalerror.h>
#include <err.h>
#include <globals.h>
//#include <ini.h>
#include <dll_globals.h>

void cleanup(void);

#ifdef WIN32
	void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp);
	int handle_program_memory_depletion(size_t);
	void iph(const wchar_t * expression, const wchar_t * function, const wchar_t * file, unsigned int line, uintptr_t pReserved );
	void SignalHandler(int signal);
#else
    jmp_buf env;
    static void signal_handler(int sig);
    int kbhit(void);
    unsigned char getch(void);
    int set_tty_raw(void);
    int set_tty_cooked(void);
#endif



void test_drag_factor(void);

//void rmp_test1(void);

#if LEVEL == DLL_FULL_ACCESS
	void just_run_one_computrainer_full_access(void);
#elif LEVEL == DLL_ERGVIDEO_ACCESS
	void just_run_one_trainer_ergvideo_access(void);
#elif LEVEL == DLL_TRINERD_ACCESS
	void just_run_one_computrainer_trinerd_access(void);
#elif LEVEL == DLL_CANNONDALE_ACCESS
	void just_run_one_computrainer_cannondale_access(void);
#endif

void get_devices(void);

std::vector<std::string> strs;
const char *strings[] = {
	"DEVICE_NOT_SCANNED",				// unknown, not scanned
	"DEVICE_DOES_NOT_EXIST",			// serial port does not exist
	"DEVICE_EXISTS",					// exists, openable, but no RM device on it
	"DEVICE_COMPUTRAINER",
	"DEVICE_VELOTRON",
	"DEVICE_SIMULATOR",
	"DEVICE_PERF_FILE",
	"DEVICE_ACCESS_DENIED",				// port present but can't open it because something else has it open
	"DEVICE_OPEN_ERROR",				// port present, unable to open port
	"DEVICE_OTHER_ERROR"				// prt present, error, none of the above
};
FILE *glogstream=NULL;

char gstring[2048] = {0};
int bp = 0;

/*********************************************************************************************************

*********************************************************************************************************/


int main(int argc, char *argv[])  {
	int rc = 0;
	DWORD start_time;
//#include <stdio.h>

	/*
	char c;
	while(1)  {
		if (kbhit())  {
			c = getch();
			printf("%c", c);
			break;
		}
		Sleep(10);
	}
	return 0;
	*/
	

	start_time = timeGetTime();

#ifdef WIN32
	_CrtMemState memstart, memstop, memdiff;
	int flags =0;
	flags = _CRTDBG_REPORT_FLAG;
	flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	flags |= _CRTDBG_CHECK_ALWAYS_DF;
	flags |= _CRTDBG_ALLOC_MEM_DF;
	flags |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flags);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtMemCheckpoint(&memstart);
	//_crtBreakAlloc = 149;
#endif
	
	//OutputDebugString("\r\nBEGINNING PROGRAM...\r\n\r\n");					// causes a memory leak????
	printf("\r\nBEGINNING PROGRAM...\r\n\r\n");
	

//goto xit;

	try  {
#ifdef WIN32
		_set_se_translator(trans_func);
		_set_new_handler(handle_program_memory_depletion);
		_set_new_mode(1);

		_invalid_parameter_handler oldHandler, newHandler;
		newHandler = iph;
		oldHandler = _set_invalid_parameter_handler(newHandler);
		_CrtSetReportMode(_CRT_ASSERT, 0);				// Disable the message box for assertions.

#else
                int status;
                
        	status = setjmp(env);		// status = 0 when setjmp() returns

                if (status != 0)  {
                        switch (status)  {
                                case SIGINT:
                                    printf("\r\nlongjumped from control-c, status = %d\n", status);
                                    //runflag = false;
                                    break;
                                default:
                                    printf("???\n");
                                    cleanup();
                                    return 0;
                      }
                }
                else  {
                    printf("setting SIGINT\n");
                    signal(SIGINT, signal_handler);
                }

		set_tty_raw();         // set up character-at-a-time


#endif

	// make the console wide enough:
	HWND console = GetConsoleWindow();
   RECT r;
   GetWindowRect(console, &r);
   MoveWindow(console, r.left, r.top, 1600, 800, TRUE);

		rc = 0;
		start_time = timeGetTime();

		//strs.push_back("x");			// causes memory leak
		//strs.clear();

	//get_devices();
	//test_drag_factor();

	#if LEVEL == DLL_FULL_ACCESS
		//rmp_test1();
		just_run_one_computrainer_full_access();
	#elif LEVEL == DLL_ERGVIDEO_ACCESS
		//rmp_test1();
		just_run_one_trainer_ergvideo_access();
		//just_run_one_computrainer_trinerd_access();
		bp = 1;
	#elif LEVEL == DLL_TRINERD_ACCESS
		just_run_one_computrainer_trinerd_access();
	#endif
		exit(0);
		//goto finis;
	}						// try
	catch (const char *str) {
		rc = 1;
		printf("\r\nconst char *exception: %s\r\n\r\n", str);
	}
	catch (fatalError & e)  {
		rc = 1;
		printf("\r\nfatal error: %s\r\n\r\n", e.get_str() );
	}
	catch (int &i)  {
		rc = 1;
		printf("\r\nInteger Exception (%d)\r\n\r\n", i);
	}
	catch (...)  {
		rc = 1;
		printf("\r\nUnhandled Exception\r\n\r\n");
	}

//finis:
	printf("\r\n\r\ncalling cleanup():\r\n\r\n");
	cleanup();

	sprintf(gstring, "total time = %.2f seconds", (timeGetTime()-start_time)/1000.0f);

//xit:
#ifdef WIN32
	printf("\r\ncalling _CrtCheckMemory:\r\n");
	_CrtCheckMemory( );

	printf("\r\ncalling _CrtDumpMemoryLeaks:\r\n");
	_CrtDumpMemoryLeaks();
	_CrtMemCheckpoint(&memstop);

	printf("\r\ncalling _CrtMemDifference:\n");
	int kk = _CrtMemDifference(&memdiff, &memstart, &memstop);
	if(kk)  {
		printf("kk = %d\n", kk);
		_CrtMemDumpStatistics(&memdiff);
	}
	printf("\r\ndone checking memory corruption\r\n\r\n");
#endif



#ifdef WIN32
	printf("\r\nhit a key...");
	getch();
#else
	getchar();
#endif

	return rc;

}									// main()



/**************************************************************************

**************************************************************************/


void cleanup(void)  {
	int status;


#if LEVEL >= DLL_ERGVIDEO_ACCESS
	status = ResetAlltoIdle();
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}
#endif

#ifndef WIN32
	set_tty_cooked();
#endif

        FCLOSE(glogstream);
        
	return;
}

#ifdef WIN32

/**********************************************************************

**********************************************************************/

void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp)  {
	throw SE_Exception(u, (DWORD)pExp->ExceptionRecord->ExceptionAddress);
}

/**********************************************************************

**********************************************************************/

int handle_program_memory_depletion(size_t)  {
	throw (fatalError(__FILE__, __LINE__, "Out Of Memory"));
	return 0;
}
#endif

/**********************************************************************

**********************************************************************/

#ifdef WIN32
void iph(
   const wchar_t * expression,
   const wchar_t * function, 
   const wchar_t * file, 
   unsigned int line,
   uintptr_t pReserved
	)  {
	wchar_t wstr[512];

	swprintf(wstr, L"IP: func = %s, file = %s, line = %d\nexp = %s", function, file, line, expression);
	wcstombs(gstring, wstr, sizeof(gstring));
	throw (fatalError(__FILE__, __LINE__, gstring));
}
#endif



#if 0

/**************************************************************************

I wanted to make sure you got this message.  I didn't see any reply to it.

Also they seem to be having issues with the TSS score:   Here is what Roger
wrote to me:

TSS value never accumulates.  IF and NP seem correct.  When checked against
old software, the TSS value at a winpy output on a flat course after the 30
second build-up began at about .5 and continued to climb the longer I rode.
RM1 I never saw it change.


Also they noticed the TSS scores reset after a pause.   I'm not sure yet if
that I'm doing a reset or if the exit from pause is resetting the values.
I'll check my stuff and make sure I'm not calling reset, but I don't believe
I am.

Let me know if you have any Ideas.

Will

**************************************************************************/


void test_drag_factor(void)  {
	char c;
	int drag_factor = 100;
	int port, ix, fw, cal;
	EnumDeviceType what;
	const char *cptr;
	int status;
	int keys = 0x40;
	int lastkeys=0;
	float watts, mwatts;
	float rpm, hr;
	float mph;
	unsigned long display_count = 0L;
	float *bars;
	float *average_bars;
	float seconds;
	unsigned long start_time;
	unsigned long sleep_ms = 5L;
	float slope;
	float np, iff, tss;
	bool rising_edge = false;
	DWORD now, last_display_time = 0;
	int i;
	float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	TrainerData td;
	Ini ini("test.ini");


	status = Setlogfilepath(".");
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	bool _bikelog = true;
	bool _courselog = true;
	bool _decoderlog = true;
	bool _dslog = true;
	bool _gearslog = true;
	bool _physicslog = true;
	METADATA *meta = NULL;
	Decoder *decoder = NULL;


	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	port = ini.getInt("options", "port", 6, true);
	ix = port - 1;

	printf("getting device id\n");

	what = GetRacerMateDeviceID(ix);
	if (what != DEVICE_COMPUTRAINER && what != DEVICE_VELOTRON )  {
		throw(fatalError(__FILE__, __LINE__));
	}

	if (what==DEVICE_COMPUTRAINER)  {
		printf("found computrainer\n");
	}
	else  {
		printf("found velotron\n");
	}

	fw = GetFirmWareVersion(ix);
	if (FAILED(fw))  {
		cptr = get_errstr(fw);
		sprintf(gstring, "couldn't get firmware: %s", cptr);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}
	printf("firmware = %d\n", fw);

	cal = GetCalibration(port-1);

	status = set_ftp(ix, fw, 1.0f);
	if (status!=DEVICE_NOT_RUNNING)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	if (what==DEVICE_VELOTRON)  {
		int Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };
		int cogset[MAX_REAR_GEARS] = { 23, 21, 19, 17, 16, 15, 14, 13, 12, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float wheeldiameter = 700.0f;				// (float) (INCHES_TO_MM*27.0);
		int ActualChainring = 62;
		int Actualcog = 14;
		float bike_kgs = (float)(TOKGS*20.0f);
		SetVelotronParameters(
							ix,
							fw,
							3,
							10,
							Chainrings, 
							cogset, 
							wheeldiameter,			// mm
							ActualChainring, 
							Actualcog,
							bike_kgs,
							//person_kgs,
							2,						// 2, start off in gear 56/11
							9						// 9, start off in gear 56/11
							);
	}


	status = startTrainer(ix);										// start computrainer
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	status = set_ftp(ix, fw, 1.0f);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("computrainer started\n");

	slope = 0.0f;

	status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}


	start_trainer(ix, true);										// set started

	float kph = (float) (MPHTOKPH * 10.0f);
	set_wind(ix, fw, kph);
	decoder = get_decoder(ix);

	kph = (float) (MPHTOKPH * 6.0f);
	set_draftwind(ix, fw, kph);

	meta = get_meta(ix);
	decoder = get_decoder(ix);


	Sleep(1000);											// wait a little for threads to prime the data
	decoder = get_decoder(ix);

	keys = lastkeys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call

	ResetAverages(ix, fw);
	setPause(ix, false);

	printf("Seconds   Bits     DF         TSS    IFF    NP          TIME\n");

	start_time = timeGetTime();


	while(1)  {
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
				continue;
			}
			if (c==VK_ESCAPE)  {
				break;
			}
			else if (c==VK_UP)  {
			}
			else if (c==VK_RIGHT)  {
			}
			else if (c=='p')  {
				bool p = is_paused(ix);
				p = !p;
				setPause(ix, p);
			}
			else if (c=='x')  {
				bool p = is_paused(ix);
				//p = !p;
				start_trainer(ix, false);
			}
			else  {
			}
		}

		now = timeGetTime();

		td = GetTrainerData(ix, fw);
		watts = td.Power;
		mph = (float)(TOMPH*td.kph);
		rpm = td.cadence;
		hr = td.HR;
		keys = GetHandleBarButtons(ix, fw);					// keys should be accumulated keys pressed since last call
		bars = get_bars(ix, fw);
		average_bars = get_average_bars(ix, fw);
		meta = get_meta(ix);
		decoder = get_decoder(ix);

#ifdef _DEBUG
		if (meta->wind < .001f)  {
			bp = 2;
		}
		//if (meta-> < .001f)  {
		//	bp = 2;
		//}
#endif

		// it takes 30 seconds for non-zero values to show up!!!!!!!!!!!!!!!!

		np = get_np(ix, fw);
		iff = get_if(ix, fw);
		tss = get_tss(ix, fw);
#if 0
		if (np > 0.0f)  {
			bp = 2;
		}
#endif

		int bits = get_status_bits(ix, fw);

		if (keys && (keys ^ lastkeys))  {
			rising_edge = true;
		}

		int ddrag = 5;

		if (rising_edge)  {
			rising_edge = false;
			switch(keys)  {
				case 0x00:
					break;
				case 0x40:						// not connected
					break;

				case 0x01:						// reset
					break;
				case 0x04:						// f2
					break;
				case 0x10:						// +
					if (what==DEVICE_COMPUTRAINER)  {
						drag_factor += ddrag;
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;
				case 0x02:						// f1
					if (what==DEVICE_VELOTRON)  {
						drag_factor += ddrag;
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;
				case 0x08:						// f3
					break;
				case 0x20:						// -
					if (what==DEVICE_COMPUTRAINER)  {
						drag_factor -= ddrag;
						if (drag_factor<0)  {
							drag_factor = 0;
						}
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;
				default:
					break;
			}
		}

		lastkeys = keys;


		if ( (now - last_display_time) >= 500)  {
			last_display_time = now;
			seconds = (float) ((now - start_time)/1000.0f) ;
			//                                 tss    iff    np
			printf("%6.1f    %04X  %6d        %.2f   %.2f   %.2f        %.3f %s\n",
				seconds,
				bits & 0x0000ffff,
				drag_factor,
				tss,
				iff,
				np,
				(float)meta->time/1000.0f,
				is_paused(ix)?"P":" "
				);
		}


		Sleep(sleep_ms);
	}


	status = stopTrainer(ix);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("computrainer stopped\n");


	return;
}							// test_drag_factor()

#endif

#if LEVEL == DLL_CANNONDALE_ACCESS
/**********************************************************************

**********************************************************************/

void just_run_one_computrainer_cannondale_access(void)  {
	int port, ix, fw, cal;
	bool b;
	EnumDeviceType what;
	const char *cptr;
	int status;
	int keys = 0x40;
	unsigned char cb = 0;
	//MODE mode;
	Physics::MODE mode;
	float watts, mwatts;
	float rpm, hr;
	//bool flag = true;
	float mph;
	char keystr[128];
	unsigned long display_count = 0L;
	float *bars;
	float *average_bars;
	float seconds;
	unsigned long start_time;
	unsigned long ms = 50L;
	unsigned long sleep_ms = 5L;
	unsigned long packet_error1=0L;
	unsigned long packet_error2=0L;
	unsigned long packet_error3=0L;
	unsigned long packet_error4=0L;
	char cbstr[32];
	char modestr[32];
	int cnt;
	char cwd[256];
	int i;
	char c;
	SSDATA ssdata;
	DWORD now, last_display_time = 0;
	TrainerData td;
	int mask;
        bool flag = true;
const char *ctkeyname[6] = {
	"Reset",
	"F1",
	"F2",
	"F3",
	"+",
	"-"
};

const char *vtkeyname[6] = {
	"Fn",
	"F1",
	"F2",
	"F3",
	"Up",
	"Down"
};



	GetCurrentDirectory(strlen(cwd), cwd);					// x:/_fs/ev_dll/cpp_tester
	for(i=0; i<NDIRS; i++)  {
		dirs.push_back(".");
	}

	//status = set_dirs(dirs);
	//assert(status==ALL_OK);

#ifdef WIN32
	//status = Setlogfilepath("x:\\_fs\\dll\\cpp_tester\\logs");				// sets dirs[DIR_DEBUG]
	status = Setlogfilepath(".");												// sets dirs[DIR_DEBUG]
#else
	status = Setlogfilepath("./logs");				// sets dirs[DIR_DEBUG]
#endif

	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	bool _bikelog = false;
	bool _courselog = false;
	bool _decoderlog = false;
	bool _dslog = false;
	bool _gearslog = false;
	bool _physicslog = false;


	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	//set_timeout(1000*60*10);						// set timeout to 10 minutes

	port = COMPUTRAINER_PORT3;
	ix = port - 1;


	printf("getting device id\r\n");

	what = GetRacerMateDeviceID(ix);
	if (what != DEVICE_COMPUTRAINER)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	printf("found computrainer\r\n");

	fw = GetFirmWareVersion(ix);
	if (FAILED(fw))  {
		cptr = get_errstr(fw);
		sprintf(gstring, "%s", cptr);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}
	//if (fw != 4095)  {
	//	sprintf(gstring, "%d / %x", fw, fw);
	//	throw(fatalError(__FILE__, __LINE__, gstring));
	//}
	printf("firmware = %d\r\n", fw);

	cptr = GetAPIVersion();

	cptr = get_dll_version();

	b = GetIsCalibrated(ix, fw);									// false

	cal = GetCalibration(ix);										// 200

	status = startTrainer(ix);										// start computrainer
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("computrainer started\r\n");

	//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((


	Sleep(1000);											// wait a little for threads to prime the data

	//keys = 0x40;

	/*
	what = get_what(ix);
	if (what != DEVICE_COMPUTRAINER)  {
		throw(fatalError(__FILE__, __LINE__));
	}
	fw = GetFirmWareVersion(ix);
	*/

	status = resetTrainer(ix, fw, cal);									// resets ds, decoder, etc
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	status = ResetAverages(ix, fw);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	int drag_factor = 100;

	status = setPause(ix, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	Sleep(500);

	status = setPause(ix, false);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	/*
	status = SetErgModeLoad(ix, fw, cal, 300.0f);			// sets ergo mode, doesn't take effect until you reach 12 mph, and then after a 2 second ramp up
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	*/

	status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, 1.0f);	// sets windload mode
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

#if LEVEL >= DLL_ERGVIDEO_ACCESS
	keys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call
#endif

	cnt = 0;

	glogstream = fopen("run.log", "wt");

	start_time = timeGetTime();

	while(flag)  {
#ifdef WIN32
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
				continue;
			}
			if (c==VK_ESCAPE)  {
               flag = false;
				break;
			}
		}
#else
                /*
                // run for 5 seconds:
		now = timeGetTime();
                if ((now-start_time) >= 15000)  {
                    break;
                }
                */

                if (kbhit())  {
                    printf("keyboard hit\r\n");
                    c = getch();
                    switch(c)  {
                        case 0:
                            c = getch();
                            break;
			case 0x1b:
                            printf("escape\r\n");
                            flag = false;
                            continue;
                            //break;

			case 0x03:				// might be control-C
                            printf("control-c\r\n");
                            flag = false;
                            continue;
                            //break;

			default:
                            bp = 1;
                            break;
                    }
		}

#endif


		now = timeGetTime();

		td = GetTrainerData(ix, fw);
		//METADATA *meta = get_meta(ix);
		watts = td.Power;
		mph = (float)(TOMPH*td.kph);
		rpm = td.cadence;
		hr = td.HR;

#if LEVEL >= DLL_ERGVIDEO_ACCESS
		keys = GetHandleBarButtons(ix, fw);					// keys should be accumulated keys pressed since last call
		bars = get_bars(ix, fw);
#endif
		average_bars = get_average_bars(ix, fw);
		ssdata = get_ss_data(ix, fw);

#if LEVEL >= DLL_ERGVIDEO_ACCESS
		if (keys==0x40)  {
			strcpy(keystr, "NO COMMUNICATION");
		}
		else  {
			mask = 0x01;
			strcpy(keystr, "keys =");
			for(i=0; i<6; i++)  {
				if (mask & keys)  {
					strcat(keystr, "   ");
					if (what == DEVICE_COMPUTRAINER)  {
						strcat(keystr, ctkeyname[i]);
					}
					else  {
						strcat(keystr, vtkeyname[i]);
					}
				}
				mask <<= 1;
			}
		}
#elif LEVEL >= DLL_CANNONDALE_ACCESS
//		if (is
//		bp = 1;
#endif


#if 0
		cb = get_control_byte(ix);
		switch(cb)  {
			case HB_ERGO_RUN:
				strcpy(cbstr, "HB_ERGO_RUN");
				break;
			case HB_WIND_RUN:
				strcpy(cbstr, "HB_WIND_RUN");
				break;
			case HB_RFTEST:
				strcpy(cbstr, "HB_RFTEST");
				break;
			case HB_WIND_PAUSE:
				strcpy(cbstr, "HB_WIND_PAUSE");
				break;
			case HB_ERGO_PAUSE:
				strcpy(cbstr, "HB_ERGO_PAUSE");
				break;
			default:
				bp2 =  1;
				break;
		}

		mode = get_physics_mode(ix);
		if (mode != Physics::CONSTANT_WATTS)  {
			bp2 =  2;
		}

/*
Physics::MODE
Physics::WINDLOAD,				// old  Physics::WINDLOAD mode
Physics::CONSTANT_TORQUE,
Physics::CONSTANT_WATTS,			// old Physics::CONSTANT_WATTS mode
Physics::CONSTANT_CURRENT,
Physics:: Physics::NOMODE
*/

		switch(mode)  {
			//case Physics::CONSTANT_WATTS:
			case Physics::CONSTANT_WATTS:
				strcpy(modestr, "ERGO");
				break;
			//case  Physics::WINDLOAD:
			case Physics::WINDLOAD:
				strcpy(modestr, " Physics::WINDLOAD");
				break;
			//case  Physics::NOMODE:
			case Physics::NOMODE:
				strcpy(modestr, " Physics::NOMODE");
				break;
			default:
				strcpy(modestr, "??????");
				break;
		}

		mwatts = get_manual_watts(ix);
#endif


		if ( (now - last_display_time) >= 500)  {
			last_display_time = now;
			seconds = (float) ((now - start_time)/1000.0f) ;
			//printf("%6.1f  %02x  %s\r\n",seconds, keys, keystr);
#if LEVEL >= DLL_ERGVIDEO_ACCESS
			printf("%6.1f  %6.1f  %6.1f  %6.1f  %6.1f  %02x  %s\r\n",seconds, rpm, mph, watts, hr, keys, keystr);
#elif LEVEL >= DLL_CANNONDALE_ACCESS
			printf("%6.1f  %6.1f  %6.1f  %6.1f  %6.1f\r\n",seconds, rpm, mph, watts, hr);
#endif
		}


		Sleep(sleep_ms);
	}


	FCLOSE(glogstream);

	//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((

	status = stopTrainer(ix);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("computrainer stopped\n");

	status = ResettoIdle(ix);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	return;
}							// just_run_one_computrainer_cannondale_access()()

#endif					//#if LEVEL == DLL_CANNONDALE_ACCESS

/**************************************************************************

**************************************************************************/

void get_devices(void)  {
	int i, port, k;
	EnumDeviceType what;

	//for(i=0; i<256; i++)  {
	for(i=0; i<32; i++)  {
		port = i+1;
		what = check_for_trainers(port);

		/*
enum EnumDeviceType {
	DEVICE_NOT_SCANNED,					// unknown, not scanned
	DEVICE_DOES_NOT_EXIST,				// serial port does not exist
	DEVICE_EXISTS,							// exists, openable, but no RM device on it
	DEVICE_COMPUTRAINER,
	DEVICE_VELOTRON,
	DEVICE_SIMULATOR,
	DEVICE_PERF_FILE,
	DEVICE_ACCESS_DENIED,				// port present but can't open it because something else has it open
	DEVICE_OPEN_ERROR,					// port present, unable to open port
	DEVICE_OTHER_ERROR					// prt present, error, none of the above
};
		*/

		switch(what)  {
			case DEVICE_NOT_SCANNED:					// unknown, not scanned
				bp = 1;
				break;
			case DEVICE_DOES_NOT_EXIST:				// serial port does not exist
				bp = 1;
				break;
			case DEVICE_EXISTS:							// exists, openable, but no RM device on it
				bp = 1;
				break;
			case DEVICE_COMPUTRAINER:
				bp = 1;
				break;
			case DEVICE_VELOTRON:
				bp = 1;
				break;
			case DEVICE_SIMULATOR:
				bp = 1;
				break;
			//case DEVICE_PERF_FILE:
			//	bp = 1;
			//	break;
			case DEVICE_ACCESS_DENIED:				// port present but can't open it because something else has it open
				bp = 1;
				break;
			case DEVICE_OPEN_ERROR:					// port present, unable to open port
				bp = 1;
				break;
			case DEVICE_OTHER_ERROR:					// prt present, error, none of the above
				bp = 1;
				break;
			default:
				bp = 0;
				break;

		}

		if(what>=DEVICE_NOT_SCANNED && what<=DEVICE_VELOTRON)  {
			bp = 1;
		}
		else if(what==DEVICE_COMPUTRAINER)  {
			bp = 3;
		}

		k = (int)what;
		printf("%3d   %s\r\n", port, strings[k] );
	}
	return;
}


#ifndef WIN32

/**************************************************************************

**************************************************************************/

static void signal_handler(int sig)  {

	switch (sig)  {
		case SIGINT:   // process for interrupt
			longjmp(env, sig);
		case SIGALRM:  // process for alarm
			printf("SIGALRM\n");
			longjmp(env, sig);
		default:
			printf("sig DEFAULT\n");
			exit(sig);
	}

	return;
}

/* ***************************************************************************

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

extern int errno;

static struct termios termattr, save_termattr;
static int ttysavefd = -1;
static enum {
	RESET,
	RAW2,
	CBREAK
} ttystate = RESET;

/* ***************************************************************************
 *
 * set_tty_raw(), put the user's TTY in one-character-at-a-time mode.
 * returns 0 on success, -1 on failure.
 *
 *************************************************************************** */

int set_tty_raw(void) {
	int i;

	i = tcgetattr (STDIN_FILENO, &termattr);
	if (i < 0)
	{
		printf("tcgetattr() returned %d for fildes=%d\n",i,STDIN_FILENO);
		perror ("");
		return -1;
	}
	save_termattr = termattr;

	termattr.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	termattr.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	termattr.c_cflag &= ~(CSIZE | PARENB);
	termattr.c_cflag |= CS8;
	termattr.c_oflag &= ~(OPOST);

	termattr.c_cc[VMIN] = 1;  /* or 0 for some Unices;  see note 1 */
	termattr.c_cc[VTIME] = 0;

	i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
	if (i < 0)
	{
		printf("tcsetattr() returned %d for fildes=%d\n",i,STDIN_FILENO);
		perror("");
		return -1;
	}

	ttystate = RAW2;
	ttysavefd = STDIN_FILENO;

	return 0;
}

/* ***************************************************************************
 *
 * set_tty_cbreak(), put the user's TTY in cbreak mode.
 * returns 0 on success, -1 on failure.
 *
 *************************************************************************** */
int set_tty_cbreak() {
	int i;

	i = tcgetattr (STDIN_FILENO, &termattr);
	if (i < 0)
	{
		printf("tcgetattr() returned %d for fildes=%d\n",i,STDIN_FILENO);
		perror ("");
		return -1;
	}

	save_termattr = termattr;

	termattr.c_lflag &= ~(ECHO | ICANON);
	termattr.c_cc[VMIN] = 1;
	termattr.c_cc[VTIME] = 0;

	i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
	if (i < 0)
	{
		printf("tcsetattr() returned %d for fildes=%d\n",i,STDIN_FILENO);
		perror ("");
		return -1;
	}
	ttystate = CBREAK;
	ttysavefd = STDIN_FILENO;

	return 0;
}

/* ***************************************************************************
 *
 * set_tty_cooked(), restore normal TTY mode. Very important to call
 *   the function before exiting else the TTY won't be too usable.
 * returns 0 on success, -1 on failure.
 *
 *************************************************************************** */

int set_tty_cooked() {
	int i;
	if (ttystate != CBREAK && ttystate != RAW2) {
		return 0;
	}
	i = tcsetattr (STDIN_FILENO, TCSAFLUSH, &save_termattr);
	if (i < 0) {
		return -1;
	}
	ttystate = RESET;
	return 0;
}

/* ***************************************************************************
 *
 * kb_getc(), if there's a typed character waiting to be read,
 *   return it; else return 0.
 *
 *************************************************************************** */
unsigned char kb_getc(void) {
	unsigned char ch;
	ssize_t size;

	size = read (STDIN_FILENO, &ch, 1);
	if (size == 0)
	{
		return 0;
	}
	else
	{
		return ch;
	}
}

/* ***************************************************************************
 *
 * kb_getc_w(), wait for a character to be typed and return it.
 *
 *************************************************************************** */
unsigned char kb_getc_w(void) {
	unsigned char ch;
	size_t size;

	while (1)
	{

		usleep(20000);        /* 1/50th second: thanks, Floyd! */

		size = read (STDIN_FILENO, &ch, 1);
		if (size > 0)
		{
			break;
		}
	}
	return ch;
}

/**************************************************************************

 **************************************************************************/

int kbhit(void)  {
	fd_set set;
	FD_ZERO(&set);
	FD_SET(0, &set);
	struct timeval timeout = {0, 0};
	select(1, &set, NULL, NULL, &timeout);
	return FD_ISSET(0, &set);
}

/* ***************************************************************************

 *****************************************************************************/

unsigned char getch(void) {
	unsigned char ch;
	ssize_t size;

	size = read (STDIN_FILENO, &ch, 1);
	if (size == 0) {
		return 0;
	}
	else {
		return ch;
	}
}



#endif              // #ifndef WIN32



#if LEVEL == DLL_FULL_ACCESS

/**********************************************************************

**********************************************************************/

void just_run_one_computrainer_full_access(void)  {
	int ix, fw=0, cal;
	bool b;
	EnumDeviceType what;
	const char *cptr;
	int status;
	int keys = 0x40;
	//unsigned char cb = 0;
	//MODE mode;
	//Physics::MODE mode;
	float watts;
	//float mwatts;
	float rpm, hr;
	//bool flag = true;
	float mph;
	char keystr[128];
	//unsigned long display_count = 0L;
	float *bars;
	float *average_bars;
	float seconds;
	unsigned long start_time;
	//unsigned long ms = 50L;
	unsigned long sleep_ms = 5L;
	//unsigned long packet_error1=0L;
	//unsigned long packet_error2=0L;
	//unsigned long packet_error3=0L;
	//unsigned long packet_error4=0L;
	//char cbstr[32];
	//char modestr[32];
	int cnt;
	//char cwd[256];
	int i;
	char c;
	SSDATA ssdata;
	DWORD now, last_display_time = 0;
	TrainerData td;
	int mask;
        bool flag = true;
const char *ctkeyname[6] = {
	"Reset",
	"F1",
	"F2",
	"F3",
	"+",
	"-"
};

const char *vtkeyname[6] = {
	"Fn",
	"F1",
	"F2",
	"F3",
	"Up",
	"Down"
};
	int slope = 0;				// %grade * 100

#ifdef NEWPORT
	char port[32];
#else
	int port;
#endif

	Ini ini("test.ini");

	//GetCurrentDirectory(strlen(cwd), cwd);					// x:/_fs/ev_dll/cpp_tester
	for(i=0; i<(int)NDIRS; i++)  {
		dirs.push_back(".");
	}

	Logger logg("x.x");

	set_logger(&logg);

	//status = set_dirs(dirs);
	//assert(status==ALL_OK);

	//racermate_init();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef WIN32
	//status = Setlogfilepath("x:\\_fs\\dll\\cpp_tester\\logs");				// sets dirs[DIR_DEBUG]
	status = Setlogfilepath(".");				// sets dirs[DIR_DEBUG]		<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#else
	status = Setlogfilepath(".");				// sets dirs[DIR_DEBUG]
#endif


	if (status != ALL_OK)  {
		sprintf(gstring, "status = %d", status);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}

	bool _bikelog = false;
	bool _courselog = false;
	bool _decoderlog = false;
	bool _dslog = false;
	bool _gearslog = false;
	bool _physicslog = false;


	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);		// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	//set_timeout(1000*60*10);						// set timeout to 10 minutes

	int iport = ini.getInt("options", "port", 6, true);

	//port = COMPUTRAINER_PORT3;
	ix = iport - 1;



	//bool connected = false;

#ifdef NEWPORT
	std::map<std::string, int> portname_to_ix;
	std::map<int, std::string> ix_to_portname;

	#ifdef WIN32
		for(i=0; i<256; i++)  {
			sprintf(port, "%d", i+1);
			portname_to_ix[port] = i;
			ix_to_portname[i] = port;
		}

		#ifdef _DEBUG
		i = portname_to_ix.size();
		#endif

		sprintf(port, "%d", COMPUTRAINER_PORT3);
		//strcpy(port, "1");										// my serial port
		portname_to_ix[port] = COMPUTRAINER_PORT3 - 1;
		ix_to_portname[ix] = port;
	#else
		strcpy(port, "/dev/tty.usbserial-FTE06HEY");				// usb-serial adapter
		portname_to_ix[port] = 0;					// device 0 = my serial port
	#endif

	ix = portname_to_ix[port];
	//ix = portmap[port];
	set_portname(ix, port);									// only sets the maps, not the device[].portname

	printf("ix = %d, port = %s\r\n", ix, port);

#else
	port = COMPUTRAINER_PORT3;
	ix = port - 1;
#endif

	printf("getting device id\r\n");

	what = GetRacerMateDeviceID(ix);						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (what == DEVICE_COMPUTRAINER)  {
		printf("found computrainer on port %d\r\n", ix+1);
	}
	else if (what == DEVICE_EXISTS)  {
		sprintf(gstring, "don't see computrainer on port %d\r\n", ix+1);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}
	else  {
		sprintf(gstring, "No Computrainer on port %s", port);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}

	/*
		main
			getfirmwareversion()		dll.cpp
	*/

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		fw = GetFirmWareVersion(ix);
		if (FAILED(fw))  {
			cptr = get_errstr(fw);
			sprintf(gstring, "%s", cptr);
			throw(fatalError(__FILE__, __LINE__, gstring));
		}

		//if (fw != 4095)  {
		//	sprintf(gstring, "%d / %x", fw, fw);
		//	throw(fatalError(__FILE__, __LINE__, gstring));
		//}
		printf("firmware = %d\r\n", fw);
	}

	cptr = GetAPIVersion();

	cptr = get_dll_version();

	b = GetIsCalibrated(ix, fw);									// false

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		cal = GetCalibration(ix);										// 200
	}

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		status = startTrainer(ix);										// start computrainer
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("computrainer started\r\n");
	}


	//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((


	Sleep(1000);											// wait a little for threads to prime the data

	//keys = 0x40;

	/*
	what = get_what(ix);
	if (what != DEVICE_COMPUTRAINER)  {
		throw(fatalError(__FILE__, __LINE__));
	}
	fw = GetFirmWareVersion(ix);
	*/

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		status = resetTrainer(ix, fw, cal);									// resets ds, decoder, etc
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
	}

	status = ResetAverages(ix, fw);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	int drag_factor = 100;

	status = setPause(ix, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	Sleep(500);

	status = setPause(ix, false);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	status = SetErgModeLoad(ix, fw, cal, 300.0f);			// sets ergo mode, doesn't take effect until you reach 12 mph, and then after a 2 second ramp up
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	slope = 100;			// set slope to 1%

	status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);	// sets windload mode
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

#if LEVEL >= DLL_ERGVIDEO_ACCESS
	keys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call
#endif

	cnt = 0;

	glogstream = fopen("run.log", "wt");

	start_time = timeGetTime();


	while(flag)  {
#ifdef WIN32
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
				switch(c)  {
					case 72:							// up arrow
						slope += 10;						// go up by .1%
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						bp = 3;
						break;
					case 80:							// down arrow
						slope -= 10;						// go down by .1%
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						bp = 3;
						break;
					default:
						bp = 8;
						break;
				}
				continue;
			}
			if (c==VK_ESCAPE)  {
				flag = false;
                break;
			}
			else  {
				bp = 7;
			}
		}
#else
                /*
                // run for 5 seconds:
		now = timeGetTime();
                if ((now-start_time) >= 15000)  {
                    break;
                }
                */

                if (kbhit())  {
                    printf("keyboard hit\r\n");
                    c = getch();
                    switch(c)  {
                        case 0:
                            c = getch();
                            break;
			case 0x1b:
                            printf("escape\r\n");
                            flag = false;
                            continue;
                            //break;

			case 0x03:				// might be control-C
                            printf("control-c\r\n");
                            flag = false;
                            continue;
                            //break;

			default:
                            bp = 1;
                            break;
                    }
		}

#endif


		now = timeGetTime();

		td = GetTrainerData(ix, fw);

#if 0
		// ANT logic (to be moved into internal dll:
		METADATA *meta = get_meta(ix);
		if (keys!=0x40)  {						// connected
			/*
				BIT 0
					1 = blink heart
					0 = solid heart, display 0

				BIT 1
					1 = show "E" on display
					0 = don't

				BIT 2
					1 = below limit, show "LO" and beep slow
					0 = don't

				BIT 3
					1 = above "HI" limit, show "HI" and beep fast
					0 = don't

				if bits 2&3 set, no sensor is installed

			*/

			if ( (meta->hrflags&0x0c)==0x0c)  {
				// no hr sensor installed, so see if there is an ANT heartrate sensor

				bp = 2;
			}
			else  {
				bp = 3;
			}

			if (meta->rpmflags==0x00)  {
				bp = 4;
			}
			else  {
				bp = 5;
			}
		}
		else  {
			bp = 1;
		}
#endif


		watts = td.Power;
		mph = (float)(TOMPH*td.kph);
		rpm = td.cadence;
#ifdef _DEBUG
		unsigned long ul = (unsigned long)rpm;
#endif
		hr = td.HR;

#if LEVEL >= DLL_ERGVIDEO_ACCESS
		keys = GetHandleBarButtons(ix, fw);					// keys should be accumulated keys pressed since last call
		bars = get_bars(ix, fw);
#endif
		average_bars = get_average_bars(ix, fw);
		ssdata = get_ss_data(ix, fw);

#if LEVEL >= DLL_ERGVIDEO_ACCESS
		if (keys==0x40)  {
			strcpy(keystr, "NO COMMUNICATION");
		}
		else  {
			mask = 0x01;
			strcpy(keystr, "keys =");
			for(i=0; i<6; i++)  {
				if (mask & keys)  {
					strcat(keystr, "   ");
					if (what == DEVICE_COMPUTRAINER)  {
						strcat(keystr, ctkeyname[i]);
					}
					else  {
						strcat(keystr, vtkeyname[i]);
					}
				}
				mask <<= 1;
			}
		}
#elif LEVEL >= DLL_CANNONDALE_ACCESS
//		if (is
//		bp = 1;
#endif


#if 0
		cb = get_control_byte(ix);
		switch(cb)  {
			case HB_ERGO_RUN:
				strcpy(cbstr, "HB_ERGO_RUN");
				break;
			case HB_WIND_RUN:
				strcpy(cbstr, "HB_WIND_RUN");
				break;
			case HB_RFTEST:
				strcpy(cbstr, "HB_RFTEST");
				break;
			case HB_WIND_PAUSE:
				strcpy(cbstr, "HB_WIND_PAUSE");
				break;
			case HB_ERGO_PAUSE:
				strcpy(cbstr, "HB_ERGO_PAUSE");
				break;
			default:
				bp2 =  1;
				break;
		}

		mode = get_physics_mode(ix);
		if (mode != Physics::CONSTANT_WATTS)  {
			bp2 =  2;
		}

/*
Physics::MODE
Physics::WINDLOAD,				// old  Physics::WINDLOAD mode
Physics::CONSTANT_TORQUE,
Physics::CONSTANT_WATTS,			// old Physics::CONSTANT_WATTS mode
Physics::CONSTANT_CURRENT,
Physics:: Physics::NOMODE
*/

		switch(mode)  {
			//case Physics::CONSTANT_WATTS:
			case Physics::CONSTANT_WATTS:
				strcpy(modestr, "ERGO");
				break;
			//case  Physics::WINDLOAD:
			case Physics::WINDLOAD:
				strcpy(modestr, " Physics::WINDLOAD");
				break;
			//case  Physics::NOMODE:
			case Physics::NOMODE:
				strcpy(modestr, " Physics::NOMODE");
				break;
			default:
				strcpy(modestr, "??????");
				break;
		}

		mwatts = get_manual_watts(ix);
#endif

		if ( (now - last_display_time) >= 500)  {
			last_display_time = now;
			seconds = (float) ((now - start_time)/1000.0f) ;
			//printf("%6.1f  %02x  %s\r\n",seconds, keys, keystr);
			printf("%6.1f  %6.1f  %6.1f  %6.1f  %6.1f  %02x  %s\r\n",seconds, mph, rpm, hr, watts, keys, keystr);
		}


		Sleep(sleep_ms);
	}


	FCLOSE(glogstream);

	//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((

	status = stopTrainer(ix);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("computrainer stopped\n");

	status = ResettoIdle(ix);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	return;
}							// just_run_one_computrainer_full_access()()

#endif						// #if LEVEL = DLL_FULL_ACCESS


//#ifdef _DEBUG
#if 0

/***********************************************************************************************************

***********************************************************************************************************/

#include <rmp.h>

void rmp_test1(void)  {
    int status;
    const char *cptr;
	FILE *dbgstream;

    RMP *rmp = NULL;

#ifdef _DEBUG
    rmp = new RMP("x.log");
#else
    rmp = new RMP();
#endif

#if 1
    int test = 1;

    if (test==1)  {
		dbgstream = fopen("dbg.log", "wt");
        //status = rmp->load("./rmp/1.rmp", dbgstream);
        //status = rmp->load("./rmp/2.rmp", dbgstream);
        status = rmp->load("../../apps/test_programs/main_utils/rmp/2.rmp", dbgstream);

        if (status != 0)  {
            cptr = rmp->geterrstr();
            bp = 2;
        }
    }
    else if (test==2)  {
    }

#else
    rm("./rmp/out.rmp");
    rmp->set_exe("TTS");

    status = rmp->save("./rmp/out.rmp");
    if (status != 0)  {
        bp = 2;
    }
#endif

    DEL(rmp);
	FCLOSE(dbgstream);

    return;
}

#endif					// _DEBUG





#if LEVEL == DLL_ERGVIDEO_ACCESS

/**********************************************************************

**********************************************************************/

void just_run_one_trainer_ergvideo_access(void)  {
	char url[256] = {0};
	int client_port = 0;
	int broadcast_port = 0;
	int listen_port = 0;
	int ix, fw=0, cal;
	bool b;
	EnumDeviceType what;
	const char *cptr;
	int status;
	int keys = 0x40;
	//unsigned char cb = 0;
	//MODE mode;
	//Physics::MODE mode;
	float watts;
	//float mwatts;
	float rpm, hr;
	//bool flag = true;
	float mph;
	char keystr[128];
	//unsigned long display_count = 0L;
	float *bars;
	float *average_bars;
	float seconds;
	unsigned long start_time;
	//unsigned long ms = 50L;
	unsigned long sleep_ms = 5L;
	//unsigned long packet_error1=0L;
	//unsigned long packet_error2=0L;
	//unsigned long packet_error3=0L;
	//unsigned long packet_error4=0L;
	//char cbstr[32];
	//char modestr[32];
	int cnt;
	//char cwd[256];
	int i;
	char c;
	SSDATA ssdata;
	DWORD now, last_display_time = 0;
	TrainerData td;
	int mask;
        bool flag = true;
const char *ctkeyname[6] = {
	"Reset",
	"F1",
	"F2",
	"F3",
	"+",
	"-"
};

const char *vtkeyname[6] = {
	"Fn",
	"F1",
	"F2",
	"F3",
	"Up",
	"Down"
};
	int slope = 0;				// %grade * 100

#ifdef NEWPORT
	char port[32];
#else
	int port;
#endif
	//xxx
	Bike::GEARPAIR gp = {0};
	int fix = 0;										// index of velotron front gear (0-2)
	int rix = 0;										// index of velotron rear gear (0-9)
	int maxfix = -1;									// highest non-zero index for front gear
	int maxrix = -1;									// highest non-zero index for rear gear
	int Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };																			// up to 5 front gears
	int cogset[MAX_REAR_GEARS] = { 23, 21, 19, 17, 16, 15, 14, 13, 12, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };			// up to 20 rear gears
	float wheeldiameter = 700.0f;					// (float) (INCHES_TO_MM*27.0);
	int ActualChainring = 62;						// physical number of teeth on velotron front gear
	int Actualcog = 14;								// physical number of teeth on velotron rear gear
	float bike_kgs = (float)(TOKGS*20.0f);		// weight of bike in kgs


	Ini ini("test.ini");
	racermate_init();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	status = Setlogfilepath(".");				// sets dirs[DIR_DEBUG]
	if (status != ALL_OK)  {
		sprintf(gstring, "status = %d", status);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}

	// RM1 DOES NOT CALL EnableLogs!!!!
	bool _dslog = true;
	bool _decoderlog = true;
	bool _courselog = true;

	bool _bikelog = false;
	bool _gearslog = false;
	bool _physicslog = false;


	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);		// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	//set_timeout(1000*60*10);						// set timeout to 10 minutes

	int iport = ini.getInt("options", "serial port", 6, true);
	cptr = ini.getString("options", "ipaddr", "10.10.100.254", true);
//iport = 35;				// for ant testing

	strncpy(url, cptr, sizeof(url)-1);
	client_port = ini.getInt("options", "client port", 8899, true);

	broadcast_port = ini.getInt("options", "broadcast port", 9071, true);
	listen_port = ini.getInt("options", "listen port", 9072, true);
	bp = 2;


#ifdef NEWPORT
	std::map<std::string, int> portname_to_ix;
	std::map<int, std::string> ix_to_portname;

	#ifdef WIN32
		for(i=0; i<256; i++)  {
			sprintf(port, "%d", i+1);
			portname_to_ix[port] = i;
			ix_to_portname[i] = port;
		}

		#ifdef _DEBUG
		i = portname_to_ix.size();
		#endif

		sprintf(port, "%d", COMPUTRAINER_PORT3);
		//strcpy(port, "1");										// my serial port
		portname_to_ix[port] = COMPUTRAINER_PORT3 - 1;
		ix_to_portname[ix] = port;
	#else
		strcpy(port, "/dev/tty.usbserial-FTE06HEY");				// usb-serial adapter
		portname_to_ix[port] = 0;					// device 0 = my serial port
	#endif

	ix = portname_to_ix[port];
	//ix = portmap[port];
	set_portname(ix, port);									// only sets the maps, not the device[].portname

	printf("ix = %d, port = %s\r\n", ix, port);


#else
	//port = COMPUTRAINER_PORT3;
	port = iport;
	ix = port - 1;
#endif

	const char *ccptr = GetPortNames();						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	char seps[]   = " ";
	char *token;
	int count = 0;
	std::vector<std::string> portnames;

   token = strtok((char *)ccptr, seps);

   while( token != NULL )  {
		portnames.push_back(token);
		count++;
      token = strtok( NULL, seps );
   }


	for(i=0; i<(int)portnames.size(); i++)  {
		printf("%s\n", portnames[i].c_str());
	}

	printf("\ngetting device id, port = %d, ix = %d\r\n", port, ix);
	char curdir[256];
	GetCurrentDirectory(sizeof(curdir), curdir);
	printf("curdir = %s\n", curdir);

	if (port>=CLIENT_SERIAL_PORT_BASE && port <= CLIENT_SERIAL_PORT_BASE+16)  {
		status = set_client_network(ix, url, client_port);				// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		assert(status==ALL_OK);
	}
	else if (port>=SERVER_SERIAL_PORT_BASE && port <= SERVER_SERIAL_PORT_BASE+16)  {
		status = set_server_network(
						broadcast_port,				// 9071
						listen_port,					// 9072
						true,								// ip_discover
						false								// udp flag
						);				// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		assert(status==ALL_OK);
	}

	bool short_circuit = false;

	if (port > 201)  {
		/*
		while(1)  {
			Sleep(100);
		}
		*/
		short_circuit = true;
		if (short_circuit)  {
			goto loop;
		}
	}

	what = GetRacerMateDeviceID(ix);						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, /dev/ttyUSB2 (.../hbsim/3)

	printf("back from GetRacerMateDeviceID\n");


	if (what == DEVICE_COMPUTRAINER)  {
		printf("found computrainer on port %d\r\n", ix+1);
	}
	else if (what == DEVICE_VELOTRON)  {
		printf("found velotron on port %d\r\n", ix+1);
	}
	/*
	else if (what == DEVICE_WIFI)  {
		printf("found WIFI computrainer on port %d\r\n", ix+1);
	}
	*/
	else if (what == DEVICE_EXISTS)  {						// comment
		sprintf(gstring, "don't see computrainer on port %d\r\n", ix+1);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}
	else  {
		sprintf(gstring, "No Computrainer on port %d", port);
		throw(fatalError(__FILE__, __LINE__, gstring));
	}
//printf("ok2\n");

	/*
		main
			getfirmwareversion()		dll.cpp
	*/

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		fw = GetFirmWareVersion(ix);
		if (FAILED(fw))  {
			cptr = get_errstr(fw);
			sprintf(gstring, "%s", cptr);
			throw(fatalError(__FILE__, __LINE__, gstring));
		}

		//if (fw != 4095)  {
		//	sprintf(gstring, "%d / %x", fw, fw);
		//	throw(fatalError(__FILE__, __LINE__, gstring));
		//}
		printf("firmware = %d\r\n", fw);
	}

	cptr = GetAPIVersion();

	printf("api version = %s\r\n", cptr);

	cptr = get_dll_version();
	printf("dll version = %s\r\n", cptr);

	b = GetIsCalibrated(ix, fw);									// false
	printf("calibrated = %s\r\n", b?"true":"false");

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		cal = GetCalibration(ix);										// 200
	}

	printf("cal = %d\r\n", cal);

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		/*
			#2
			dataSource()											// constructor 1
			RTD::RTD()												// constructor 1
			Computrainer::Computrainer()						// constructor 1
			Computrainer::init()
			EV::EV()
		*/

		status = set_ftp(ix, fw, 1.0f);
		if (status!=DEVICE_NOT_RUNNING)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("ftp set\r\n");


		if (what==DEVICE_VELOTRON)  {

			/*
			SetVelotronParameters()
			Bike::GEARPAIR GetCurrentVTGear(int, int)
			xxx
			int setGear(int ix, int FWVersion, int front_index, int rear_index)  {
			*/

	//int Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };																			// up to 5 front gears
	//int cogset[MAX_REAR_GEARS] = { 23, 21, 19, 17, 16, 15, 14, 13, 12, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };			// up to 20 rear gears

			maxfix = -1;
			for(i=0; i<MAX_FRONT_GEARS; i++)  {
				if (Chainrings[i]==0)  {
					break;
				}
				maxfix++;
			}

			maxrix = -1;
			for(i=0; i<MAX_REAR_GEARS; i++)  {
				if (cogset[i]==0)  {
					break;
				}
				maxrix++;
			}

			bp = 1;

			status = SetVelotronParameters(
								ix,
								fw,
								3,
								10,
								Chainrings, 
								cogset, 
								wheeldiameter,			// mm
								ActualChainring, 
								Actualcog,
								bike_kgs,
								//person_kgs,
								2,						// 2, start off in gear 56/11
								9						// 9, start off in gear 56/11
								);

			if (status!=ALL_OK)  {
				cptr = get_errstr(status);
				throw(fatalError(__FILE__, __LINE__, cptr));
			}

			printf("velotron parameters set\r\n");

		}


		status = startTrainer(ix);										// start trainer

		//if (!ant_initialized)  {
#ifdef _DEBUG
		if (!get_ant_stick_initialized(ix))  {
			bp = 4;
		}
#endif

		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			racermate_close();
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		if (what==DEVICE_COMPUTRAINER)  {
			printf("computrainer started\r\n");
		}
		else  {
			printf("velotron started\r\n");
		}

	}										// if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {

	status = setGear(ix, fw, fix, rix);													// go in to starting gear
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((


	Sleep(1000);											// wait a little for threads to prime the data

	//keys = 0x40;

	/*
	what = get_what(ix);
	if (what != DEVICE_COMPUTRAINER)  {
		throw(fatalError(__FILE__, __LINE__));
	}
	fw = GetFirmWareVersion(ix);
	*/

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		status = resetTrainer(ix, fw, cal);									// resets ds, decoder, etc
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
		printf("trainer reset\r\n");
	}

	status = ResetAverages(ix, fw);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("averages reset\r\n");

	//float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	int drag_factor = 100;

	status = setPause(ix, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("paused\r\n");

	Sleep(500);

	status = setPause(ix, false);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("unpaused\r\n");

	status = SetErgModeLoad(ix, fw, cal, 300.0f);			// sets ergo mode, doesn't take effect until you reach 12 mph, and then after a 2 second ramp up
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("erg mode set\r\n");

	slope = 100;			// set slope to 1%

	status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);	// sets windload mode
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("wind load mode set\r\n");

	keys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call
	if (keys==BAD_FIRMWARE_VERSION)  {
		throw(fatalError(__FILE__, __LINE__, cptr));
	}



loop:
	cnt = 0;

	glogstream = fopen("run.log", "wt");

	start_time = timeGetTime();

	DWORD tick = 0L;


	while(flag)  {
#ifdef WIN32
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
				switch(c)  {
					case 72:							// up arrow
						slope += 10;						// go up by .1%
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						bp = 3;
						break;
					case 80:							// down arrow
						slope -= 10;						// go down by .1%
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						bp = 3;
						break;

					//xxx

					case 'u':																// rear gear on velotron up
						if (what==DEVICE_VELOTRON)  {
							if (rix < maxrix)  {
								rix++;
							}
							status = setGear(ix, fw, fix, rix);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
						}
						break;
					case 'U':																	// front gear on velotron up
						if (what==DEVICE_VELOTRON)  {
							if (fix < maxfix)  {
								fix++;
							}
							status = setGear(ix, fw, fix, rix);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
						}
						break;

					default:
						bp = 8;
						break;
				}
				continue;
			}
			if (c==VK_ESCAPE)  {
				flag = false;
				break;
			}
			else  {
				bp = 7;
			}
		}
#else
                /*
                // run for 5 seconds:
		now = timeGetTime();
                if ((now-start_time) >= 15000)  {
                    break;
                }
                */

                if (kbhit())  {
                    printf("keyboard hit\r\n");
                    c = getch();
                    switch(c)  {
                        case 0:
                            c = getch();
                            break;
			case 0x1b:
                            printf("escape\r\n");
                            flag = false;
                            continue;
                            //break;

			case 0x03:				// might be control-C
                            printf("control-c\r\n");
                            flag = false;
                            continue;
                            //break;

			default:
                            bp = 1;
                            break;
                    }
		}

#endif


		if (short_circuit)  {
			Sleep(100);
			continue;
		}

		now = timeGetTime();
		td = GetTrainerData(ix, fw);

#if 0
		// ANT logic (to be moved into internal dll:
		METADATA *meta = get_meta(ix);
		if (keys!=0x40)  {						// connected
			/*
				BIT 0
					1 = blink heart
					0 = solid heart, display 0

				BIT 1
					1 = show "E" on display
					0 = don't

				BIT 2
					1 = below limit, show "LO" and beep slow
					0 = don't

				BIT 3
					1 = above "HI" limit, show "HI" and beep fast
					0 = don't

				if bits 2&3 set, no sensor is installed

			*/

			if ( (meta->hrflags&0x0c)==0x0c)  {
				// no hr sensor installed, so see if there is an ANT heartrate sensor

				bp = 2;
			}
			else  {
				bp = 3;
			}

			if (meta->rpmflags==0x00)  {
				bp = 4;
			}
			else  {
				bp = 5;
			}
		}
		else  {
			bp = 1;
		}
#endif


		watts = td.Power;
		mph = (float)(TOMPH*td.kph);
		rpm = td.cadence;
#ifdef _DEBUG
		unsigned long ul = (unsigned long)rpm;
#endif
		hr = td.HR;

#ifdef _DEBUG
		if (hr > 10.0f)  {
			bp = 1;
		}
#endif

		keys = GetHandleBarButtons(ix, fw);					// keys should be accumulated keys pressed since last call

		bars = get_bars(ix, fw);
		average_bars = get_average_bars(ix, fw);
		ssdata = get_ss_data(ix, fw);

		if (keys==0x40)  {
			strcpy(keystr, "NO COMMUNICATION");
		}
		else  {
			mask = 0x01;
			//strcpy(keystr, "(");
			strcpy(keystr, "");
			for(i=0; i<6; i++)  {
				if (mask & keys)  {
					strcat(keystr, "   ");
					if (what == DEVICE_COMPUTRAINER)  {
						strcat(keystr, ctkeyname[i]);
					}
					else  {
						strcat(keystr, vtkeyname[i]);
					}
				}
				mask <<= 1;
			}
		}


#if 0
		cb = get_control_byte(ix);
		switch(cb)  {
			case HB_ERGO_RUN:
				strcpy(cbstr, "HB_ERGO_RUN");
				break;
			case HB_WIND_RUN:
				strcpy(cbstr, "HB_WIND_RUN");
				break;
			case HB_RFTEST:
				strcpy(cbstr, "HB_RFTEST");
				break;
			case HB_WIND_PAUSE:
				strcpy(cbstr, "HB_WIND_PAUSE");
				break;
			case HB_ERGO_PAUSE:
				strcpy(cbstr, "HB_ERGO_PAUSE");
				break;
			default:
				bp2 =  1;
				break;
		}

		mode = get_physics_mode(ix);
		if (mode != Physics::CONSTANT_WATTS)  {
			bp2 =  2;
		}

/*
Physics::MODE
Physics::WINDLOAD,				// old  Physics::WINDLOAD mode
Physics::CONSTANT_TORQUE,
Physics::CONSTANT_WATTS,			// old Physics::CONSTANT_WATTS mode
Physics::CONSTANT_CURRENT,
Physics:: Physics::NOMODE
*/

		switch(mode)  {
			//case Physics::CONSTANT_WATTS:
			case Physics::CONSTANT_WATTS:
				strcpy(modestr, "ERGO");
				break;
			//case  Physics::WINDLOAD:
			case Physics::WINDLOAD:
				strcpy(modestr, " Physics::WINDLOAD");
				break;
			//case  Physics::NOMODE:
			case Physics::NOMODE:
				strcpy(modestr, " Physics::NOMODE");
				break;
			default:
				strcpy(modestr, "??????");
				break;
		}

		mwatts = get_manual_watts(ix);
#endif

#ifdef _DEBUG
		if (hr > 10.0f)  {
			bp = 1;
		}
#endif

		if ( (now - last_display_time) >= 1000)  {
			last_display_time = now;
			seconds = (float) ((now - start_time)/1000.0f) ;
			//printf("%6.1f  %02x  %s\r\n",seconds, keys, keystr);
#ifdef _DEBUG
			if (mph > .1f)  {
				bp = 1;
			}
#endif
			tick++;

#ifdef _DEBUG
			if (what==DEVICE_VELOTRON)  {
				gp = GetCurrentVTGear(ix, fw);

				sprintf(gstring, "%5ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s\r\n",
					tick,
					mph,
					rpm,
					hr,
					watts,
					keys&0x000000ff,
					keystr
					);
			}
			else if (what==DEVICE_COMPUTRAINER)  {
				sprintf(gstring, "%5ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s  %s    HR_%s    CAD_%s    SC_%s\r\n",
					tick,
					mph,
					rpm,
					hr,
					watts,
					keys&0x000000ff,
					keystr,
					get_ant_stick_initialized(ix)?"IN":"OUT",
					get_state(ANT_HR),
					get_state(ANT_C),
					get_state(ANT_SC)
					);
			}
#else
			sprintf(gstring, "%5ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s\r\n",
				tick,
				mph,
				rpm,
				hr,
				watts,
				keys&0x000000ff,
				keystr
				);
#endif

			printf(gstring);
			fprintf(glogstream, "%s", gstring);
		}


		Sleep(sleep_ms);
	}


	FCLOSE(glogstream);

	//((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((

	if (!short_circuit)  {
		status = stopTrainer(ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}


		if (what==DEVICE_COMPUTRAINER)  {
			printf("computrainer stopped\n");
		}
		else  {
			printf("velotron stopped\n");
		}

		status = ResettoIdle(ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
	}									// if (!short_circuit)

	status = racermate_close();
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	return;
}							// just_run_one_trainer_ergvideo_access()()

#endif						// #if LEVEL = DLL_ERGVIDEO_ACCESS



#if LEVEL==DLL_TRINERD_ACCESS
//#if 1

/*************************************************************************************************************************
 just_run_one_trainer_ergvideo_access
*************************************************************************************************************************/

void just_run_one_computrainer_trinerd_access(void)  {

int rc = 0;
int ix, fw, cal;
int i, status, cnt;
bool b;
bool flag = true;
float watts;
float rpm, hr, mph;
float *average_bars;
float seconds;
const char *cptr;
	//unsigned char cb = 0;
	//unsigned long display_count = 0L;
unsigned long start_time;
	//unsigned long ms = 50L;
unsigned long sleep_ms = 5L;
unsigned long now, last_display_time = 0;
char cwd[256];
char c;
EnumDeviceType what;
SSDATA ssdata;
TrainerData td;
int keys = 0x40;
int lastkeys=0;
int mask;
bool rising_edge = false;
char keystr[128] = {0};
const char *ctkeyname[6] = {
	"Reset",
	"F1",
	"F2",
	"F3",
	"+",
	"-"
};
enum COMPUTRAINER_MODE  {							// make sure this is in sync with 'computrainer_modes'
	HB_WIND_RUN = 0x2c,
	HB_WIND_PAUSE = 0x28,
	HB_RFTEST = 0x1c,
	HB_ERGO_RUN = 0x14,
	HB_ERGO_PAUSE = 0x10
};

int mode = -1;

#ifdef NEWPORT
	char port[32];
#else
	int port;
#endif

/*
#ifdef __MACH__
	char port[32];
#else
	int port;
#endif
*/

try  {
	
	GetCurrentDirectory((int)strlen(cwd), cwd);
	for(i=0; i<NDIRS; i++)  {
		dirs.push_back(".");
	}
	
	dirs[DIR_PROGRAM] = ".";
	dirs[DIR_PERSONAL] = ".";
	dirs[DIR_SETTINGS] = ".";
	dirs[DIR_REPORT_TEMPLATES] = ".";
	dirs[DIR_REPORTS] = ".";
	dirs[DIR_COURSES] = ".";
	dirs[DIR_PERFORMANCES] = ".";
	dirs[DIR_DEBUG] = ".";
	dirs[DIR_HELP] = ".";
	
	
	status = Setlogfilepath(".");
	if (status != ALL_OK)  {
		printf("error in %s at %d\n", __FILE__, __LINE__);
		exit(1);
	}
	
	bool _bikelog = false;
	bool _courselog = false;
	bool _decoderlog = false;
	bool _dslog = false;
	bool _gearslog = false;
	bool _physicslog = false;
	
	
	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);
	if (status != ALL_OK)  {
		printf("error in %s at %d\n", __FILE__, __LINE__);
		exit(1);
	}

#ifdef NEWPORT
	std::map<std::string, int> portname_to_ix;
	std::map<int, std::string> ix_to_portname;

	#ifdef WIN32
		for(i=0; i<256; i++)  {
			sprintf(port, "%d", i+1);
			portname_to_ix[port] = i;
			ix_to_portname[i] = port;
		}

#ifdef _DEBUG
		i = portname_to_ix.size();
#endif

		sprintf(port, "%d", COMPUTRAINER_PORT3);
		//strcpy(port, "1");										// my serial port
		portname_to_ix[port] = COMPUTRAINER_PORT3 - 1;
		ix_to_portname[ix] = port;
	#else
		strcpy(port, "/dev/tty.usbserial-FTE06HEY");				// usb-serial adapter
		portmap[port] = 0;					// device 0 = my serial port
	#endif

	ix = portname_to_ix[port];
	//ix = portmap[port];
	set_portname(ix, port);
#else
	port = COMPUTRAINER_PORT3;
	ix = port - 1;
#endif
	
	printf("getting device id\r\n");
	
	what = GetRacerMateDeviceID(ix);
	if (what != DEVICE_COMPUTRAINER)  {
#ifndef __MACH__
		printf("error in %s at %d, cant' find computrainer on port %d\npress enter... ", __FILE__, __LINE__, port);
#else
#endif
		getchar();
		exit(1);
	}
	
	
	printf("found computrainer\r\n");
	
	fw = GetFirmWareVersion(ix);
	if (FAILED(fw))  {
		cptr = get_errstr(fw);
		sprintf(gstring, "%s", cptr);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	printf("firmware = %d\r\n", fw);
	
	cptr = GetAPIVersion();
	
	cptr = get_dll_version();
	
	b = GetIsCalibrated(ix, fw);									// false
	
	cal = GetCalibration(ix);										// 200
	
	status = startTrainer(ix);										// start computrainer
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	printf("computrainer started\r\n");
	
	Sleep(1000);											// wait a little for threads to prime the data
	
	status = resetTrainer(ix, fw, cal);									// resets ds, decoder, etc
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	status = ResetAverages(ix, fw);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	int drag_factor = 100;
	
	status = setPause(ix, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	Sleep(500);
	
	status = setPause(ix, false);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	status = SetErgModeLoad(ix, fw, cal, 100.0f);			// go to ergo mode at 100 watts
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	
	status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, 1.0f);	// sets windload mode
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	cnt = 0;
	
	
	status = SetHRBeepBounds(ix, fw, 40, 140, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	
	keys = lastkeys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call
	strcpy(keystr, "keys =");
	
	// 2013-06-24 +++
	
	status = SetRecalibrationMode(ix, fw);											// enter calibration mode
	if (status != ALL_OK)  {
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
	mode = get_computrainer_mode(ix);
	assert(mode==HB_RFTEST);
	
	printf("in calibration mode\n");
	// 2013-06-24 ---
	
	status = EndRecalibrationMode(ix, fw);
	if (FAILED(status))  {
		printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
		exit(1);
	}

	mode = get_computrainer_mode(ix);
	assert(mode==HB_WIND_RUN);
	
	printf("exited calibration mode\n");


	start_time = timeGetTime();
	
	while(flag)  {
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
				continue;
			}
			if (c==0x1b)  {
				flag = false;
				break;
			}
		}
		
		now = timeGetTime();
		
		td = GetTrainerData(ix, fw);
		watts = td.Power;
		mph = (float)(TOMPH*td.kph);
		rpm = td.cadence;
		hr = td.HR;
		keys = GetHandleBarButtons(ix, fw);					// accumulated keys pressed since last call
		
		average_bars = get_average_bars(ix, fw);
		ssdata = get_ss_data(ix, fw);
		
		// just an example:
		
		switch(keys)  {
			case 0x00:
				break;
			case 0x40:						// not connected
				break;
			case 0x01:						// reset
				break;
			case 0x04:						// f2
				break;
			case 0x10:						// +
				break;
			case 0x02:						// f1
				break;
			case 0x08:						// f3
				break;
			case 0x20:						// -
				break;
			default:
				break;
		}
		
		// another way to handle key presses:
		
		if (keys && (keys ^ lastkeys))  {
			rising_edge = true;
		}
		
		if (rising_edge)  {
			rising_edge = false;
			if (keys==0x40)  {
				strcpy(keystr, "NO COMMUNICATION\n");
			}
			else  {
				if (keys==0x08)  {							// F3 key on handlebar
					// 2013-06-24 +++
					status = EndRecalibrationMode(ix, fw);
					if (FAILED(status))  {
						printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
						exit(1);
					}
					mode = get_computrainer_mode(ix);
					assert(mode==HB_WIND_RUN);
					printf("exited calibration mode\n");
					// 2013-06-24 ---
				}
				
				mask = 0x01;
				strcpy(keystr, "keys =");
				
				for(i=0; i<6; i++)  {
					if (mask & keys)  {
						strcat(keystr, "   ");
						if (what == DEVICE_COMPUTRAINER)  {
							strcat(keystr, ctkeyname[i]);
						}
					}
					mask <<= 1;
				}
			}
		}
					
		lastkeys = keys;
					
					
		if ( (now - last_display_time) >= 500)  {
			last_display_time = now;
			seconds = (float) ((now - start_time)/1000.0f) ;
			printf("%6.1f  %6.1f  %6.1f  %6.1f  %6.1f   %s\n",seconds, rpm, mph, watts, hr, keystr);
			strcpy(keystr, "keys =");
		}
					
		Sleep(sleep_ms);
	}								// while(flag)
					
					
	status = stopTrainer(ix);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
		getchar();
		exit(1);
	}
		printf("computrainer stopped\n");
					
		status = ResettoIdle(ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\npress enter... ", __FILE__, __LINE__, cptr);
			getchar();
			exit(1);
		}
					
}						// try
catch (const char *str) {
	rc = 1;
	printf("\r\nconst char *exception: %s\r\n\r\n", str);
}
catch (int &i)  {
	rc = 1;
	printf("\r\nInteger Exception (%d)\r\n\r\n", i);
}
catch (...)  {
	rc = 1;
	printf("\r\nUnhandled Exception\r\n\r\n");
}
					
	printf("\r\npress enter... ");
					
	getchar();
					
	//return rc;
	
	return;
}						// void just_run_one_computrainer_trinerd_access(void)

#endif					// #if LEVEL==DLL_TRINERD_ACCESS

