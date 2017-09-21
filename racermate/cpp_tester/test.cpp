
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
	#pragma warning(disable:4996 4006)
#endif

#define COMPDISP  4
#define VELDISP  0

/*****************************************************************************************

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
//#include <string>

#include <stdlib.h>

#include "trainer.h"




#include <glib_defines.h>

#include <utils.h>
#include <logger.h>

#include <computrainer.h>
#include <errors.h>

#include <fatalerror.h>
#include <err.h>

#ifndef MEMTEST
	#include <globals.h>
	#include <dll_globals.h>
#endif

#include <ev.h>

#include <internal.h>
#include <dll.h>


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


void test_n_trainers(void);
void test_wifi(void);
void test_ant(void);
void test_wifi_and_ant(void);
void emulate_rm2(void);

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

char gteststring[2048] = {0};
int bp = 0;

enum UDMODE {													// for 'u' / 'd' keys
	NONE,
	GRADE,														// 'mg'
	WEIGHT,														// 'ml'
	MANWATTS,													// 'mm'
	WIND,
	DRAFT_WIND
};

char lastchar = 0;
UDMODE udmode = NONE;

int iwind_mph = 0;
int idraftwind_mph = 0;

/*********************************************************************************************************

*********************************************************************************************************/


int main(int argc, char *argv[])  {

#ifdef _DEBUG
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
	//_crtBreakAlloc = 217;										// 150 is caused by "#include <string>" AND any printf()!!!!
#endif

	int rc = 0;

//goto xit;

#ifndef MEMLEAKTEST

	DWORD start_time;

	start_time = timeGetTime();

	printf("\r\nBEGINNING PROGRAM...\r\n\r\n");
	

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

#ifdef WIN32
	/*
	// make the console wide enough:
	HWND console = GetConsoleWindow();
   RECT r;
   GetWindowRect(console, &r);
   MoveWindow(console, r.left, r.top, 1600, 800, TRUE);
	*/
#endif

		rc = 0;
		start_time = timeGetTime();
		//test_n_trainers();
		//test_ant();
		//test_wifi();
		//test_wifi_and_ant();
		emulate_rm2();

		cleanup();
		goto finis;
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


finis:
	printf("\r\n\r\ncalling cleanup():\r\n\r\n");
	cleanup();

	sprintf(gteststring, "total time = %.2f seconds", (timeGetTime()-start_time)/1000.0f);

#endif		// #ifndef MEMLEAKTEST

//xit:
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

#ifndef MEMLEAKTEST
	printf("\r\ndone checking memory corruption\r\n\r\n");
	printf("\r\nhit a key...");
	getch();
#endif

	return rc;

}									// main()

#ifndef MEMLEAKTEST

/**************************************************************************

**************************************************************************/

void cleanup(void)  {
	int status;
	const char *ccptr;

	status = ResetAlltoIdle();
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	status = racermate_close();
	if (status!=ALL_OK)  {
		ccptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, ccptr));
	}

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
	wcstombs(gteststring, wstr, sizeof(gteststring));
	throw (fatalError(__FILE__, __LINE__, gteststring));
}
#endif





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

int handle_pc_keyboard(Trainer *);
int handle_pc_keyboard(void);
//int broadcast_port = 0;
//int listen_port = 0;




/**********************************************************************
	returns
		0 normally
		1 if escape
		2 if pause key hit
**********************************************************************/

int handle_pc_keyboard(Trainer *t)  {
	const char *cptr;
	char c;
	int status;
	int rc = 0;

		if (kbhit())  {
			c = getch();
			switch(c)  {
				case 0:   {
					c = getch();
					switch(c)  {
						/*
						case 72:							// up arrow
							t->slope += 10;						// go up by .1%
							status = SetSlope(t->ix, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", t->slope / 100.0f);
							bp = 3;
							break;
						*/
						/*
						case 80:							// down arrow
							t->slope -= 10;						// go down by .1%
							status = SetSlope(t->ix, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", t->slope / 100.0f);
							bp = 3;
							break;
						*/
						default:
							bp = 8;
							break;
					}
					//break;
					return rc;
				}												// case 0

				case 0x1b:
					rc = 1;
					break;

				case 'w': {									// wind run mode
					if (lastchar=='m')  {
						udmode = WIND;
						printf("udmode = wind\n");
					}
					else  {
						status = SetSlope(t->appkey, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope / 100.0f);	// sets windload mode
						if (status != ALL_OK) {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						printf("      wind load mode\n");
					}
					break;
				}
				case 'e': {									// erg run mode
					status = SetErgModeLoad(t->appkey, t->fw, t->cal, (float)t->manwatts);					// bug
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      ergo mode\n");
					break;
				}

				case 'c': {									// cal mode
					t->calmode = !t->calmode;

					if (t->calmode) {
						status = SetRecalibrationMode(t->appkey, t->fw);
						printf("      cal mode entered\n");
					}
					else {
						status = EndRecalibrationMode(t->appkey, t->fw);
						printf("      cal mode exit\n");
					}
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					break;
				}

				case 'p':
					rc = 2;
					break;

				case 'u':																// rear gear on velotron up
					if (t->what==DEVICE_VELOTRON)  {
						if (t->rix < t->maxrix)  {
							t->rix++;
						}
						status = setGear(t->appkey, t->fw, t->fix, t->rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					else if (t->what == DEVICE_COMPUTRAINER) {
						if (udmode==NONE)  {
							printf("      udmode = none\n");
						}
						else if (udmode==WEIGHT)  {
							t->person_kgs += .5f;
							status = SetSlope(t->appkey, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope / 100.0f);
							if (status != ALL_OK) {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      weight = %.1lf pounds\n", KGSTOLBS*(t->person_kgs + t->bike_kgs));
						}
						else if (udmode==GRADE)  {
							t->slope += 10;						// go up by .1%
							status = SetSlope(t->appkey, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", t->slope / 100.0f);
						}
						else if (udmode==MANWATTS)  {
							t->manwatts += 50;
							if (t->manwatts > 1500)  {
								t->manwatts = 1500;
							}
							SetErgModeLoad(t->appkey, t->fw, t->cal, (float)t->manwatts);					// bug
							printf("      manwatts = %d\n", t->manwatts);
						}
						else if (udmode==WIND)  {
							iwind_mph += 1;
							t->wind = (float)(TOKPH*iwind_mph);
							set_wind(t->appkey, t->fw, t->wind);
							printf("      wind = %d mph\n", iwind_mph);
						}
						else if (udmode==DRAFT_WIND)  {
							idraftwind_mph += 1;
							t->draft_wind = (float)(TOKPH*idraftwind_mph);
							set_draftwind(t->appkey, t->fw, t->draft_wind);
							printf("      draftwind = %d mph\n", idraftwind_mph);
						}
						else  {
							throw(fatalError(__FILE__, __LINE__, "bad udpmode"));
						}
					}
					break;
				case 'U':																	// front gear on velotron up
					if (t->what==DEVICE_VELOTRON)  {
						if (t->fix < t->maxfix)  {
							t->fix++;
						}
						status = setGear(t->appkey, t->fw, t->fix, t->rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;

				case 'd':																// rear gear on velotron down
					if (t->what==DEVICE_VELOTRON)  {
						if (t->rix > 0)  {
							t->rix--;
						}
						status = setGear(t->appkey, t->fw, t->fix, t->rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					else if (t->what == DEVICE_COMPUTRAINER) {
						if (lastchar=='m')  {
							udmode = DRAFT_WIND;
							printf("udmode = draft_wind\n");
						}
						else  {
							if (udmode==NONE)  {
							}
							if (udmode==WEIGHT)  {
								t->person_kgs -= .5f;
								status = SetSlope(t->appkey, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope / 100.0f);
								if (status != ALL_OK) {
									cptr = get_errstr(status);
									throw(fatalError(__FILE__, __LINE__, cptr));
								}
								printf("      weight = %.1lf pounds\n", KGSTOLBS*(t->person_kgs + t->bike_kgs));
							}
							else if (udmode==GRADE)  {
								t->slope -= 10;						// go down by .1%
								status = SetSlope(t->appkey, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope/100.0f);
								if (status!=ALL_OK)  {
									cptr = get_errstr(status);
									throw(fatalError(__FILE__, __LINE__, cptr));
								}
								printf("      slope = %.1f\n", t->slope / 100.0f);
							}
							else if (udmode==MANWATTS)  {
								t->manwatts -= 50;
								if (t->manwatts < 0)  {
									t->manwatts = 0;
								}
								SetErgModeLoad(t->appkey, t->fw, t->cal, (float)t->manwatts);					// bug
								printf("      manwatts = %d\n", t->manwatts);
							}
							else if (udmode==WIND)  {
								iwind_mph -= 1;
								t->wind = (float)(TOKPH*iwind_mph);
								set_wind(t->appkey, t->fw, t->wind);
								printf("      wind = %d mph\n", iwind_mph);
							}
							else if (udmode==DRAFT_WIND)  {
								idraftwind_mph -= 1;
								t->draft_wind = (float)(TOKPH*idraftwind_mph);
								set_draftwind(t->appkey, t->fw, t->draft_wind);
								printf("      draftwind = %d mph\n", idraftwind_mph);
							}
							else  {
								throw(fatalError(__FILE__, __LINE__, "bad udmode"));
							}
						}
					}
					break;

				case 'D':																	// front gear on velotron down
					if (t->what==DEVICE_VELOTRON)  {
						if (t->fix > 0)  {
							t->fix--;
						}
						status = setGear(t->appkey, t->fw, t->fix, t->rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;

				case 'm':									// walk thru modes
					if (lastchar=='m')  {
						udmode = MANWATTS;
						lastchar = 0;
						printf("udmode = manwatts\n");
						return rc;
					}
					break;

				case 'g':
					if (lastchar=='m')  {
						udmode = GRADE;
						printf("udmode = grade\n");
					}
					break;
				case 'l':									// lower case L
					if (lastchar=='m')  {
						udmode = WEIGHT;
						printf("udmode = weight, %.1lf pounds\n", KGSTOLBS*(t->person_kgs + t->bike_kgs));
						t->person_kgs -= .5f;
						SetSlope(t->appkey, t->fw, t->cal, t->bike_kgs, t->person_kgs, t->drag_factor, t->slope / 100.0f);
					}
					break;

				default:
					bp = 7;
					break;
			}													// switch (c)

			lastchar = c;
		}														// if (kbhit())  {

		return rc;
}												// int handle_pc_keyboard(Trainer *t)


#endif				// #ifndef MEMLEAKTEST

/**********************************************************************
	used for ant testing only
**********************************************************************/

int handle_pc_keyboard(void) {
	const char *cptr;
	char c;
	int status;
	int rc = 0;

	if(kbhit()) {
		c = getch();
		switch(c) {
			case 0:
			{
				c = getch();
				switch(c) {
					default:
						bp = 8;
						break;
				}
				return rc;
			}												// case 0
			case 0x1b:
				rc = 1;
				break;
			default:
				bp = 7;
				break;
		}													// switch (c)

		lastchar = c;
	}														// if (kbhit())  {

	return rc;
}												// int handle_pc_keyboard()


/**********************************************************************

**********************************************************************/

void test_ant(void) {
	int status, debug_level = 2;
	unsigned long sleep_ms = 5L, now, last_display_time = 0, tick = 0L;
	const char *ccptr;
	bool flag = true;
	fastTimer ft("ft");
	double dt;
	char curdir[256];
	float seconds;
	const char * sensor_string;
	const SENSORS* sensors;


	printf("\r\ntest_ant\r\n");

	racermate_init();																//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call, just sets up paths

	//---------------------------------------------------------
	// initialize ANT
	//---------------------------------------------------------

	try {
		ft.start();
		status = start_ant(2);														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call, creates an ANT object, and start a 1000ms TimerProc in internal.cpp
		ft.stop();
		dt = ft.getSeconds();				// about 100 ms

		if(status) {
			bp = 1;
		}
		else {

		}
	}
	catch(...) {
		bp = 7;
	}

	//printf("\n\n");

	// block until there are some sensors

	printf("\nlooking for sensors\n");


	unsigned long start = timeGetTime();
	//unsigned long dms;
	//tick=4;

	while(1) {
		if((timeGetTime() - start) >= 10000)  {
			printf("\nno sensors after 10 seconds\n\n");
			break;
		}
		//sensors = get_ant_sensors_string();												//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		//if(sensors) {
		//	printf("\nfound sensors in %ld ms\n\n", timeGetTime() - start);
		//	break;
		//}
		sensors = get_ant_sensors();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		if(sensors) {
			if(sensors->n > 0) {
				break;
			}
		}
		Sleep(50);
	}

	if(sensors && sensors->n > 0) {
		associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created

		status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		status = unassociate("zzz", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
	}


//while(1) {
//	Sleep(10);
//}


	//----------------------------------------------------------------------------
	// M A I N  L O O P
	//----------------------------------------------------------------------------

//	cnt = 0;
	glogstream = fopen("test.log", "wt");
//	start_time = timeGetTime();
	tick = 0;

	start = timeGetTime();

	while(flag) {
		status = handle_pc_keyboard();
		switch(status) {
			case 1:						// escape
				flag = false;
				continue;
			default:
				break;
		}


		//---------------------
		// data polling
		//---------------------

		sensor_string = get_ant_sensors_string();											//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
		sensors = get_ant_sensors();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created




		now = timeGetTime();

		if((now - last_display_time) < 1000) {
			Sleep(sleep_ms);
			continue;
		}

		//----------------------
		// the display
		//----------------------

		last_display_time = now;
		seconds = (float)((now - start) / 1000.0f);
		tick++;

		if(sensors && sensors->n>0) {
			for(int i = 0; i < sensors->n; i++) {
				unsigned short sn;
				unsigned char type;
				unsigned char val=0xff;

				sn = sensors->sensors[i].sn;
				type = sensors->sensors[i].type;
				if(type == 120) {
					val = get_ant_hr(sn);
				}
				sprintf(gteststring, "%-6.1f %3d  %3d  %d\n", seconds, sn, type, val);
			}
		}
		else {
			sprintf(gteststring, "%-6.1f no sensors\n", seconds);
		}
		printf("%s", gteststring);
		if(glogstream) {
			fprintf(glogstream, "%s", gteststring);
			fflush(glogstream);
		}
		Sleep(sleep_ms);
	}												// while(flag)

	status = stop_ant();

	FCLOSE(glogstream);

	status = racermate_close();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
	if(status != ALL_OK) {
		ccptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, ccptr));
	}
	return;
}							// test_ant()

/**********************************************************************

**********************************************************************/

void test_n_trainers(void) {
	int status;
	int k;
	int cnt;

	unsigned long sleep_ms = 5L;
	DWORD now, last_display_time = 0;

	float seconds;

	Bike::GEARPAIR gp = { 0 };

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
	RACERMATE::TrainerData td;
	int mask;
	bool b;
	unsigned long start_time;
	const char *cptr;

	bool flag = true;
	DWORD tick = 0L;
	bool paused = false;
	const char *ccptr;
	int nclients = 0;
	int lastnclients = 0;
	AverageTimer at("at");
	fastTimer ft("ft");
	double dt;
	int ndevices = 0;

	int target_trainer = 0;									// trainer 0 is serial port, which trainer the keypresses talk to

	const char *sensor_string;
	const SENSORS *sensors;


	printf("\r\ntest_n_trainers\r\n");

	rm("client.log");

	Ini ini("test.ini");

	int broadcast_port = 0;
	int listen_port = 0;

	broadcast_port = ini.getInt("options", "broadcast port", 9071, true);
	listen_port = ini.getInt("options", "listen port", 9072, true);


	racermate_init();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

	int debug_level = 2;


	Enablelogs(true, true, true, true, true, true);

	ccptr = get_build_date();
	ccptr = GetAPIVersion();											// "1.1.0"			<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	ccptr = get_dll_version();											// "1.0.13"			<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	ccptr = get_rs232_ports();												// ""				ctsrv
	ccptr = get_udp_trainers();										// NULL				ctsrv
	_ASSERT(ccptr == NULL);

	bp = 0;
	char myip[128];
	//strcpy(myip, "192.168.1.200");
	myip[0] = 0;

	//---------------------------------------------------------
	// initialize the udp server
	//---------------------------------------------------------

	try {
		ft.start();
		status = start_server(listen_port, broadcast_port, myip, 2);				// ctsrv
		ft.stop();
		dt = ft.getSeconds();				// 94, 78, 86, 122, 156 ms
	}
	catch(...) {
		bp = 7;
	}

	switch(status) {
		case 0:
			break;
		case 1:
			throw(fatalError(__FILE__, __LINE__, "NO NETWORK"));
			break;
		case 2:
			throw(fatalError(__FILE__, __LINE__, "NO BCADDR"));
			break;
		default:
			printf("\nerror %d\n", status);
			throw(fatalError(__FILE__, __LINE__));
			break;
	}

	ccptr = get_udp_trainers();
	_ASSERT(ccptr == NULL);

	Sleep(5000);

	ccptr = get_udp_trainers();										// "UDP-5678 UDP-6666"

	ft.start();
	ccptr = get_rs232_ports();												// "COM3 UDP-5678 UDP-6666"
	ft.stop();
	dt = ft.getSeconds();												// .1 ms

	ft.start();
	ccptr = getPortNames();												// "COM3 UDP-5678 UDP-6666"
	ft.stop();
	dt = ft.getSeconds();												// .1 ms

	bp = 0;

	//---------------------------------------------------------
	// initialize ANT
	//---------------------------------------------------------


	try {
		ft.start();
		status = start_ant(2);														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
		ft.stop();
		dt = ft.getSeconds();				// 56 ms

		if(status) {
			bp = 1;
		}
		else {

		}
	}
	catch(...) {
		bp = 7;
	}

	//printf("\n\n");

	//--------------------------------------------------
	// block until there are some ANT sensors
	//--------------------------------------------------

	printf("\nlooking for sensors\n");


	unsigned long start = timeGetTime();
	//unsigned long dms;
	//tick=4;

	while(1) {
		if((timeGetTime() - start) >= 10000) {
			printf("\nno sensors after 10 seconds\n\n");
			break;
		}
		//sensors = get_ant_sensors_string();												//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		//if(sensors) {
		//	printf("\nfound sensors in %ld ms\n\n", timeGetTime() - start);
		//	break;
		//}
		sensors = get_ant_sensors();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		if(sensors) {
			if(sensors->n > 0) {
				break;
			}
		}
		Sleep(50);
	}

	if(sensors && sensors->n > 0) {
		sensor_string = get_ant_sensors_string();
		associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created

		status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		status = unassociate("zzz", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
	}




	//while(1) {
	//	Sleep(100);			// 100 ms
	//}
	//Sleep(2000);

	ft.start();

	//while(1) {
	/*
	ccptr = getPortNames();
	if(strlen(ccptr) > 0) {
	//dt = ft.stop();
	dt = ft.getSeconds();												// 1.3 seconds
	bp = 2;
	}
	*/

	/*
	ccptr = get_ant_sensors();
	if(ccptr) {
	bp = 1;
	break;
	}
	Sleep(100);
	*/
	//}



	status = Setlogfilepath(".");				// sets dirs[DIR_DEBUG] dll call
	if(status != ALL_OK) {
		sprintf(gteststring, "status = %d", status);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}

	const char *dll_build_date = get_build_date();								// dll call
	printf("\r\ndll build date = %s\r\n\r\n\n\n\n\n", dll_build_date);

	// RM1 DOES NOT CALL EnableLogs!!!!
	bool _dslog = true;
	bool _decoderlog = true;
	bool _courselog = true;

	bool _bikelog = false;
	bool _gearslog = false;
	bool _physicslog = false;

	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);		// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
	if(status != ALL_OK) {
		throw(fatalError(__FILE__, __LINE__));
	}

	//set_timeout(1000*60*10);						// set timeout to 10 minutes


	//ccptr = GetPortNames();						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

	std::vector<std::string> portnames;

	if(strlen(ccptr) > 0) {
		char seps[] = " ";
		char *token;
		int count = 0;

		token = strtok((char *)ccptr, seps);

		while(token != NULL) {
			portnames.push_back(token);
			count++;
			token = strtok(NULL, seps);
		}
		//int i;
		for(int i = 0; i < (int)portnames.size(); i++) {
			printf("%s\r\n", portnames[i].c_str());
		}
	}

	char curdir[256];
	//WSTR lcurdir[256];

	GetCurrentDirectory(sizeof(curdir), curdir);
	//	printf("curdir = %s\r\n", curdir);

	//float bike_kgs = (float)(TOKGS*20.0f);

	cptr = GetAPIVersion();
	//	printf("api version = %s\r\n", cptr);

	cptr = get_dll_version();
	//	printf("dll version = %s\r\n", cptr);

	//------------------------------------------------------
	// initialize N trainers:
	//------------------------------------------------------

	int N = portnames.size();

	bool have_udp_clients = false;

	for(k = 0; k < N; k++) {
		if(strstr(portnames[k].c_str(), "UDP-") != NULL) {
			have_udp_clients = true;
		}
	}

	if(!have_udp_clients) {
		printf("no trainers found\n");
		//stop_server();
	}


	//#define N 1
	//int ixs[N] = {19, 33, 220};					// com20, com34, com221
	//int ixs[N] = {19};
	//const char * ixs[N] = {"221"};
	//int ixs[N] = {19, 220};							// com20, com221
	//int ixs[N] = {220, 221};

	//Trainer trainers[N];
	Trainer *trainers = new Trainer[N];
	int cal;


	for(k = 0; k < N; k++) {
		trainers[k].init(portnames[k].c_str());			// sets gbroadcast_port and glisten_port in dll_globals.cpp!!!!
		trainers[k].wind = (float)(TOKPH*iwind_mph);
		trainers[k].draft_wind = (float)(TOKPH*idraftwind_mph);

		if(sensors && sensors->n > 0) {
			associate(portnames[k].c_str(), sensors->sensors[0].sn);			// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
		}


	
		status = set_wind(trainers[k].appkey, trainers[k].fw, trainers[k].wind);
		if(status != ALL_OK) {
			throw(fatalError(__FILE__, __LINE__));
		}

	#ifdef _DEBUG
		check_maps("ok1");
	#endif

		status = set_draftwind(trainers[k].appkey, trainers[k].fw, trainers[k].draft_wind);
		if(status != ALL_OK) {
			throw(fatalError(__FILE__, __LINE__));
		}

		//check_maps("ok2");
		//cal = trainers[k].cal;
		cal = GetCalibration(trainers[k].appkey);
		bp = k;
	}					// for(k)

	printf("\n\n");

	//------------------------------------------------------
	// finished initializing trainers
	// start main loop
	//------------------------------------------------------

	cnt = 0;
	glogstream = fopen("test.log", "wt");
	start_time = timeGetTime();

	int ii = MAX_FRONT_GEARS;

	char state[2][32];

	ccptr = get_udp_trainers();													// "UDP-5678 UDP-6666"
	lastnclients = nclients = get_n_udp_clients();												// 2

	status = set_ftp(trainers[0].appkey, trainers[0].fw, 1.0f);
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	DWORD ms;
	float f;

	if(N > 0) {
		f = get_if(trainers[0].appkey, trainers[0].fw);
		f = get_np(trainers[0].appkey, trainers[0].fw);
		f = get_tss(trainers[0].appkey, trainers[0].fw);
	}


	//----------------------------------------------------------------------------
	// M A I N  L O O P
	//----------------------------------------------------------------------------

	while(flag) {
		ndevices = get_n_devices();

		/*
		nclients = get_n_udp_clients();
		if(nclients != lastnclients) {
		ccptr = get_udp_trainers();
		lastnclients = nclients;
		// adjust N, portnames, trainers, and devices in dll to match worker->clients
		}
		*/

		status = handle_pc_keyboard(&trainers[target_trainer]);

		switch(status) {
			case 1:						// escape
				flag = false;
				continue;

			case 2:						// pause key hit
				paused = !paused;

				for(int i = 0; i < N; i++) {
					if(i == 0) {
						bp = 2;
					}

					status = setPause(trainers[i].appkey, paused);

					if(status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
				}
				printf("      paused = %s\n", paused ? "true" : "false");
				break;

			default:
				break;
		}


		//------------------------------------------------------
		// data polling loop
		//------------------------------------------------------

		/*

		if(devices[_id].commtype == TRAINER_IS_CLIENT)  {
		if(adjust(_id, devices[_id].udpkey) == 1)  {
		return zzz
		}
		DEVICE *d = &devices[_id];			// this will create devices[_id] if it doesn't exist
		if(server) {
		}
		}

		*/

		ccptr = get_udp_trainers();
		nclients = get_n_udp_clients();
		ndevices = get_n_devices();

		sensor_string = get_ant_sensors_string();				// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
		sensors = get_ant_sensors();							// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

		if(sensor_string) {
			bp = 1;
		}

	#ifdef _DEBUG
		for(k = 0; k < N; k++) {
			if(strstr(state[k], trainers[k].appkey) == NULL) {
				bp = 1;
			}
		}
	#endif

		for(k = 0; k<N; k++) {
			ms = timeGetTime();

			ccptr = get_udpkey(trainers[k].appkey);
			if(ccptr != NULL) {
				if(strcmp(ccptr, trainers[k].udpkey)) {
					strncpy(trainers[k].udpkey, ccptr, sizeof(trainers[k].udpkey));
					bp = 3;
				}
			}

			td = GetTrainerData(trainers[k].appkey, trainers[k].fw);
			//strncpy(trainers[k].udpkey, )
		#ifdef _DEBUG
			if(k == 0 && !strstr(trainers[k].appkey, "6666")) {
				bp = 2;
			}
		#endif

			trainers[k].watts = td.Power;
			trainers[k].mph = (float)(TOMPH*td.kph);
			trainers[k].rpm = td.cadence;
			trainers[k].hr = td.HR;

		#ifdef _DEBUG
			if(trainers[k].keys == 0x40) {
				if(memcmp(trainers[k].appkey, "UDP-6666", 8) == 0) {
					bp = 7;
				}
			}
		#endif

			trainers[k].keys = GetHandleBarButtons(trainers[k].appkey, trainers[k].fw);					// keys should be accumulated keys pressed since last call

		#ifdef _DEBUG
			if(trainers[k].keys == 0x40) {
				if(memcmp(trainers[k].appkey, "UDP-6666", 8) == 0) {
					bp = 7;
				}
			}
		#endif


			trainers[k].bars = get_bars(trainers[k].appkey, trainers[k].fw);
			trainers[k].average_bars = get_average_bars(trainers[k].appkey, trainers[k].fw);
			trainers[k].status_bits = get_status_bits(trainers[k].appkey, trainers[k].fw);
			trainers[k].ssdata = get_ss_data(trainers[k].appkey, trainers[k].fw);

			if(trainers[k].keys == 0x40) {
			#ifdef _DEBUG
				if(k == 1) {
					bp = 8;
				}
			#endif
				strcpy(trainers[k].keyboard_keys_string, "NC!");
			}
			else {
				mask = 0x01;
				//strcpy(keystr, "(");
				strcpy(trainers[k].keyboard_keys_string, "");
				for(int i = 0; i<6; i++) {
					if(mask & trainers[k].keys) {
						strcat(trainers[k].keyboard_keys_string, "   ");
						if(trainers[k].what == DEVICE_COMPUTRAINER) {
							strcat(trainers[k].keyboard_keys_string, ctkeyname[i]);
						}
						else {
							strcat(trainers[k].keyboard_keys_string, vtkeyname[i]);
						}
					}
					mask <<= 1;
				}
			}

			calc_tp(trainers[k].appkey, trainers[k].fw, ms, trainers[k].watts);

			f = get_if(trainers[k].appkey, trainers[k].fw);
			f = get_np(trainers[k].appkey, trainers[k].fw);
			f = get_tss(trainers[k].appkey, trainers[k].fw);

		}							// for each trainer



		now = timeGetTime();

		if((now - last_display_time) < 1000) {
			Sleep(sleep_ms);
			continue;
		}

		//----------------------
		// the display
		//----------------------

		last_display_time = now;
		seconds = (float)((now - start_time) / 1000.0f);
		tick++;


		if(N == 1) {
		}
		else if(N>1) {
			printf("\n");
		}


		/***************************************************************
		check:
		key latching
		pausing

		***************************************************************/

		for(k = 0; k<N; k++) {

			if(trainers[k].what == DEVICE_VELOTRON) {
				gp = GetCurrentVTGear(trainers[k].appkey, trainers[k].fw);

			#if VELDISP==0
				sprintf(gteststring, "%5ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s    %d / %d\r\n",
					tick,
					trainers[k].mph,
					trainers[k].rpm,
					trainers[k].hr,
					trainers[k].watts,
					trainers[k].keys & 0x000000ff,
					trainers[k].keyboard_keys_string,
					gp.front,
					gp.rear
				);
			#elif VELDISP==1
				sprintf(gteststring, "%5.1f %5.1f %5.1f %5.1f %5.1f ",
					ssdata.lss,
					ssdata.ss,
					ssdata.rss,
					ssdata.lsplit,
					ssdata.rsplit
				);
				strcat(gteststring, "\n");
			#else
				sprintf(gteststring, "%5ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s    %d / %d\r\n",
					tick,
					mph,
					rpm,
					hr,
					watts,
					keys & 0x000000ff,
					keystr,
					gp.front,
					gp.rear
				);
			#endif

				printf("%s", gteststring);
				if(glogstream) {
					fprintf(glogstream, "%s", gteststring);
				}
			}
			else if(trainers[k].what == DEVICE_COMPUTRAINER) {
			#if COMPDISP==0
				sprintf(gteststring, "%6ld  cal=%3d MPH=%6.1f  CAD=%6.1f  HR=%6.1f  w=%6.1f  k=%02x  %s",
					tick,
					trainers[k].cal,
					trainers[k].mph,
					trainers[k].rpm,
					trainers[k].hr,
					trainers[k].watts,
					trainers[k].keys & 0x000000ff,
					trainers[k].keyboard_keys_string
				);
				strcat(gteststring, "\n");
			#elif COMPDISP==1
				gteststring[0] = 0;
				for(i = 0; i < 23; i++) {
					sprintf(str, "%.1f ", bars[i]);
					strcat(gteststring, str);
				}
				sprintf(str, "%.1f", bars[i]);
				strcat(gteststring, str);
				strcat(gteststring, "\r\n");
			#elif COMPDISP==2
				sprintf(gteststring, "%5.1f %5.1f %5.1f %5.1f %5.1f ",
					ssdata.lss,
					ssdata.ss,
					ssdata.rss,
					ssdata.lsplit,
					ssdata.rsplit
				);
				strcat(gteststring, "\n");
			#elif COMPDISP==3
				if(trainers[k].keys == 0x40) {
					bp = 1;
				}

				if(sensors) {
					sprintf(gteststring, "%6ld  %s  %6.1f  k=%02x  %s   %s   %s",
						tick,
						sensors,
						trainers[k].watts,
						trainers[k].keys & 0x000000ff,
						trainers[k].appkey,
						trainers[k].udpkey,
						trainers[k].keyboard_keys_string
					);
				}
				else {
					sprintf(gteststring, "%6ld  %6.1f  k=%02x  %s   %s   %s",
						tick,
						trainers[k].watts,
						trainers[k].keys & 0x000000ff,
						trainers[k].appkey,
						trainers[k].udpkey,
						trainers[k].keyboard_keys_string
					);
				}
				strcat(gteststring, "\n");
			#elif COMPDISP==4
				bp = 1;
				sprintf(gteststring, "%-4.0f %s",
					trainers[k].hr,
					sensor_string ? sensor_string : "no sensors"
				);
				strcat(gteststring, "\n");
			#else
				strcpy(gteststring, "???\n");
			#endif					// #if COMPDISP==0

				printf("%s", gteststring);
				if(glogstream) {
					fprintf(glogstream, "%s", gteststring);
					fflush(glogstream);
					//fclose(glogstream);
					//glogstream = fopen("test.log", "a+t");
				}
			}						// if (trainers[k].what == DEVICE_VELOTRON) {
		}							// for(k=0; k<N; k++)  {
		Sleep(sleep_ms);
	}												// while(flag)


	FCLOSE(glogstream);

	for(k = 0; k<N; k++) {
		status = stopTrainer(trainers[k].appkey);
		if(status != ALL_OK) {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
		if(trainers[k].what == DEVICE_COMPUTRAINER) {
			printf("computrainer stopped\r\n");
		}
		else {
			printf("velotron stopped\r\n");
		}

		status = ResettoIdle(trainers[k].appkey);
		if(status != ALL_OK) {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		//DEL(trainers[k]);
	}

	DELARR(trainers);;

	status = racermate_close();
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	return;
}							// test_n_trainers()


/**********************************************************************
	minimal wifi test
**********************************************************************/

void test_wifi(void) {
	int status;
	int k;
	DWORD now, last_display_time = 0;
	float seconds;
	RACERMATE::TrainerData td;
	bool b;
	unsigned long start_time;
	const char *cptr;
	bool flag = true;
	const char *ccptr;
	int cnt;
	int nclients = 0;
	std::vector<std::string> portnames;
	Trainer *trainer = new Trainer;
	float f;
	char myip[128] = { 0 };


	printf("\r\ntest_wifi\r\n");

	racermate_init();				//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call 

	//---------------------------------------------------------
	// initialize the udp server
	//---------------------------------------------------------

	try {
		status = start_server(9072, 9071, myip, 2);	// ctsrv  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
	}
	catch(...) {
		bp = 7;
	}

	switch(status) {
		case 0:
			break;
		case 1:
			throw(fatalError(__FILE__, __LINE__, "NO NETWORK"));
			break;
		case 2:
			throw(fatalError(__FILE__, __LINE__, "NO BCADDR"));
			break;
		default:
			printf("\nerror %d\n", status);
			throw(fatalError(__FILE__, __LINE__));
			break;
	}

	Sleep(5000);

	ccptr = get_udp_trainers();	// "UDP-5678 UDP-6666"  <<<<<<<<<<<<<<<<<<<<<<<< 


	if(ccptr) {
		char seps[] = " ";
		char *token;
		int count = 0;

		token = strtok((char *)ccptr, seps);

		while(token != NULL) {
			portnames.push_back(token);
			count++;
			token = strtok(NULL, seps);
		}
		for(int i = 0; i < (int)portnames.size(); i++) {
			printf("%s\r\n", portnames[i].c_str());
		}
	}

	//------------------------------------------------------
	// initialize trainer:
	//------------------------------------------------------

	bool have_udp_clients = false;

	if(portnames.size() > 0) {
		if(strstr(portnames[0].c_str(), "UDP-") != NULL) {
			have_udp_clients = true;
		}
	}


	if(!have_udp_clients) {
		printf("no trainers found\n");
		stop_server();
	}


	if(portnames.size() > 0) {
		trainer->init(portnames[0].c_str());			// sets gbroadcast_port and glisten_port in dll_globals.cpp!!!!
		trainer->wind = (float)(TOKPH*iwind_mph);

		status = set_wind(trainer->appkey, trainer->fw, trainer->wind);
		if(status != ALL_OK) {
			throw(fatalError(__FILE__, __LINE__));
		}
	}

	printf("\n\n");

	//------------------------------------------------------
	// finished initializing trainers
	// start main loop
	//------------------------------------------------------

	cnt = 0;
	start_time = timeGetTime();

	ccptr = get_udp_trainers();													// "UDP-5678 UDP-6666" 
	nclients = get_n_udp_clients();												// 


	//----------------------------------------------------------------------------
	// M A I N  L O O P
	//----------------------------------------------------------------------------

	while(flag) {
		ccptr = get_udp_trainers();													// "UDP-5678 UDP-6666"
		nclients = get_n_udp_clients();												// 

		status = handle_pc_keyboard();

		switch(status) {
			case 1:						// escape
				flag = false;
				continue;
			default:
				break;
		}

		//------------------------------------------------------
		// data polling loop
		//------------------------------------------------------

		ccptr = get_udpkey(trainer->appkey);			// 
		if(ccptr != NULL) {
			if(strcmp(ccptr, trainer->udpkey)) {
				strncpy(trainer->udpkey, ccptr, sizeof(trainer->udpkey));
			}
		}

		td = GetTrainerData(trainer->appkey, trainer->fw);		// 

		trainer->watts = td.Power;
		trainer->mph = (float)(TOMPH*td.kph);
		trainer->rpm = td.cadence;
		trainer->hr = td.HR;
		trainer->keys = GetHandleBarButtons(trainer->appkey, trainer->fw);					// keys should be accumulated keys pressed since last call
		trainer->bars = get_bars(trainer->appkey, trainer->fw);
		trainer->average_bars = get_average_bars(trainer->appkey, trainer->fw);
		trainer->status_bits = get_status_bits(trainer->appkey, trainer->fw);
		trainer->ssdata = get_ss_data(trainer->appkey, trainer->fw);

		now = timeGetTime();

		if((now - last_display_time) < 1000) {
			Sleep(10);
			continue;
		}

		//----------------------
		// the display
		//----------------------

		last_display_time = now;
		seconds = (float)((now - start_time) / 1000.0f);

		printf("%.1f  %-4.0f\n", seconds, trainer->watts);
		Sleep(10);
	}												// while(flag)


	status = stopTrainer(trainer->appkey);			// 
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("computrainer stopped\r\n");

	status = ResettoIdle(trainer->appkey);			// 
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	DEL(trainer);

	status = racermate_close();			// 
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	return;
}							// test_wifi()


/**********************************************************************

**********************************************************************/

#define DOWIFI
#define DOANT

void test_wifi_and_ant(void) {
	int status, debug_level = 2;
	unsigned long sleep_ms = 5L, now, last_display_time = 0, tick = 0L;
	const char *ccptr=NULL;
	bool flag = true;
	fastTimer ft("ft");
	double dt;
	char curdir[256];
	float seconds;
	unsigned long start;

#ifdef DOANT
	const char *sensor_string = NULL;
	const SENSORS* sensors = NULL;
#endif				// #ifdef DOANT

#ifdef DOWIFI
	std::vector<std::string> portnames;
	char myip[128] = { 0 };
	Trainer *trainer = new Trainer;
	int cnt;
	int nclients = 0;
	RACERMATE::TrainerData td;
	const char *cptr;
	char seps[] = " ";
	char *token;
	int count = 0;
#endif

	printf("\ntest_wifi_and_ant\n");

	racermate_init();																//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call, just sets up paths

#ifdef DOANT

//---------------------------------------------------------
// initialize ANT
//---------------------------------------------------------

	try {
		ft.start();
		status = start_ant(2);				//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call, creates an ANT object, and start a 1000ms ant_timer_proc() in internal.cpp
											// ant: libusb_init(), creates workerThread,  creates antWorker, starts workerThread
		ft.stop();
		dt = ft.getSeconds();				// about 100 ms

		if(status) {
			bp = 1;
		}
		else {

		}
	}
	catch(...) {
		bp = 7;
	}

#endif						// #ifdef DOANT



#ifdef DOWIFI
	//---------------------------------------------------------
	// initialize the udp server
	//---------------------------------------------------------

	try {
		ft.start();
		status = start_server(9072, 9071, myip, 2);	// ctsrv  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 
		ft.stop();
		dt = ft.getSeconds();				// about 50 ms
	}
	catch(...) {
		bp = 7;
	}

	switch(status) {
		case 0:
			break;
		case 1:
			throw(fatalError(__FILE__, __LINE__, "NO NETWORK"));
			break;
		case 2:
			throw(fatalError(__FILE__, __LINE__, "NO BCADDR"));
			break;
		default:
			printf("\nerror %d\n", status);
			throw(fatalError(__FILE__, __LINE__));
			break;
	}

	Sleep(5000);
	ccptr = get_udp_trainers();	// "UDP-5678 UDP-6666"  <<<<<<<<<<<<<<<<<<<<<<<< 

	if(ccptr) {
		count = 0;
		token = strtok((char *)ccptr, seps);

		while(token != NULL) {
			portnames.push_back(token);
			count++;
			token = strtok(NULL, seps);
		}
		for(int i = 0; i < (int)portnames.size(); i++) {
			printf("%s\r\n", portnames[i].c_str());
		}
	}				// if (ccptr)

	//------------------------------------------------------
	// initialize trainer:
	//------------------------------------------------------

	bool have_udp_clients = false;

	if(portnames.size() > 0) {
		if(strstr(portnames[0].c_str(), "UDP-") != NULL) {
			have_udp_clients = true;
		}
	}

	if(!have_udp_clients) {
		printf("no trainers found\n");
		stop_server();
	}


	if(portnames.size() > 0) {
		trainer->init(portnames[0].c_str());			// sets gbroadcast_port and glisten_port in dll_globals.cpp!!!!
		trainer->wind = (float)(TOKPH*iwind_mph);

		status = set_wind(trainer->appkey, trainer->fw, trainer->wind);
		if(status != ALL_OK) {
			throw(fatalError(__FILE__, __LINE__));
		}
	}
#endif							// #ifdef DOWIFI


	//printf("\n\n");


	start = timeGetTime();

#ifdef DOANT
	// block until there are some sensors
	printf("\nlooking for ANT sensors\n");


	//unsigned long dms;
	//tick=4;

	while(1) {
		if((timeGetTime() - start) >= 10000) {
			printf("\nno sensors after 10 seconds\n\n");
			break;
		}
		//sensors = get_ant_sensors_string();												//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		//if(sensors) {
		//	printf("\nfound sensors in %ld ms\n\n", timeGetTime() - start);
		//	break;
		//}
		sensors = get_ant_sensors();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		if(sensors) {
			if(sensors->n > 0) {
				break;
			}
		}
		Sleep(50);
	}

	if(sensors && sensors->n > 0) {
		associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created

		status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
		status = unassociate("zzz", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
	}
#endif				// #ifdef DOANT


	//------------------------------------------------------
	// finished initializing trainers
	// start main loop
	//------------------------------------------------------


#ifdef DOWIFI
	cnt = 0;
	ccptr = get_udp_trainers();													// "UDP-5678 UDP-6666" 
	nclients = get_n_udp_clients();												// 
#endif

//	while(1) {
//		Sleep(10);
//	}


	//----------------------------------------------------------------------------
	// M A I N  L O O P
	//----------------------------------------------------------------------------

	//	cnt = 0;
	glogstream = fopen("test.log", "wt");
	//	start_time = timeGetTime();
	tick = 0;

	start = timeGetTime();

	while(flag) {
		status = handle_pc_keyboard();
		switch(status) {
			case 1:						// escape
				flag = false;
				continue;
			default:
				break;
		}

		//------------------------------------------------------
		// data polling loop
		//------------------------------------------------------

		#ifdef DOWIFI
			ccptr = get_udp_trainers();													// "UDP-5678 UDP-6666"
			nclients = get_n_udp_clients();												// 

			ccptr = get_udpkey(trainer->appkey);			// 
			if(ccptr != NULL) {
				if(strcmp(ccptr, trainer->udpkey)) {
					strncpy(trainer->udpkey, ccptr, sizeof(trainer->udpkey));
				}
			}
		#endif

	#ifdef DOANT
		sensor_string = get_ant_sensors_string();											//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
		sensors = get_ant_sensors();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
	#endif						// #ifdef DOANT


	#ifdef DOWIFI
		td = GetTrainerData(trainer->appkey, trainer->fw);		// 

		trainer->watts = td.Power;
		trainer->mph = (float)(TOMPH*td.kph);
		trainer->rpm = td.cadence;
		trainer->hr = td.HR;
		trainer->keys = GetHandleBarButtons(trainer->appkey, trainer->fw);					// keys should be accumulated keys pressed since last call
		trainer->bars = get_bars(trainer->appkey, trainer->fw);
		trainer->average_bars = get_average_bars(trainer->appkey, trainer->fw);
		trainer->status_bits = get_status_bits(trainer->appkey, trainer->fw);
		trainer->ssdata = get_ss_data(trainer->appkey, trainer->fw);
	#endif



		now = timeGetTime();

		if((now - last_display_time) < 1000) {
			Sleep(sleep_ms);
			continue;
		}

		//----------------------
		// the display
		//----------------------

		last_display_time = now;
		seconds = (float)((now - start) / 1000.0f);
		tick++;

#ifdef DOWIFI
	#ifdef DOANT
		if(sensors && sensors->n>0) {
			for(int i = 0; i < sensors->n; i++) {
				unsigned short sn;
				unsigned char type;
				unsigned char val = 0xff;

				sn = sensors->sensors[i].sn;
				type = sensors->sensors[i].type;
				if(type == 120) {
					val = get_ant_hr(sn);
				}
				if(trainer) {
					sprintf(gteststring, "%-6.1f ant sensor: %3d  %3d  %d\n", seconds, sn, type, val);
				}
				else {
					sprintf(gteststring, "%-6.1f ant sensor: %3d  %3d  %d\n", seconds, sn, type, val);
				}
			}
		}
		else {
			sprintf(gteststring, "%-6.1f ant sensor: no sensors, wifi trainer watts = %-4.0f\n", seconds, trainer->watts);
			//sprintf(gteststring, "%.1f  %-4.0f\n", seconds, trainer->watts);
		}
	#else				// #ifdef DOANT
		printf("%.1f  %-4.0f\n", seconds, trainer->watts);
	#endif				// 	#ifdef DOANT

#else					// #ifdef DOWIFI
		if(sensors && sensors->n>0) {
			for(int i = 0; i < sensors->n; i++) {
				unsigned short sn;
				unsigned char type;
				unsigned char val = 0xff;

				sn = sensors->sensors[i].sn;
				type = sensors->sensors[i].type;
				if(type == 120) {
					val = get_ant_hr(sn);
				}
				sprintf(gteststring, "%-6.1f %3d  %3d  %d\n", seconds, sn, type, val);
			}
		}
		else {
			sprintf(gteststring, "%-6.1f no sensors\n", seconds);
		}
#endif				// #ifdef DOWIFI

		printf("%s", gteststring);
		if(glogstream) {
			fprintf(glogstream, "%s", gteststring);
			fflush(glogstream);
		}
		Sleep(sleep_ms);
	}												// while(flag)

#ifdef DOANT
	status = stop_ant();
#endif								// #ifdef DOANT

#ifdef DOWIFI
	status = stopTrainer(trainer->appkey);			// 
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("computrainer stopped\r\n");

	status = ResettoIdle(trainer->appkey);			// 
	if(status != ALL_OK) {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	DEL(trainer);
	stop_server();

#endif

	FCLOSE(glogstream);

	status = racermate_close();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
	if(status != ALL_OK) {
		ccptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, ccptr));
	}
	return;
}							// test_wifi_and_ant()

/**********************************************************************

**********************************************************************/

void emulate_rm2(void)  {
	int status;
	double dt;
	char myip[128] = { 0 };
	int broadcast_port = 0;
	int listen_port = 0;
	int debug_level = 2;
	bool flag = true;
	char port[32] = { 0 };
	int fw = 0;
	const char *ccptr = NULL;
	unsigned long sleep_ms = 5L, now, last_display_time = 0, tick = 0L;
	unsigned long start = 0L;
	float seconds;
	unsigned long dms;
	EnumDeviceType what;
	const char * sensor_string = NULL;
	const SENSORS* sensors = NULL;
	float *bars = NULL;
	float *average_bars = NULL;
	int status_bits;
	char keyboard_keys_string[128] = { 0 };
	SSDATA ssdata;
	int keys=0;
	int mask;
	RACERMATE::TrainerData td = { 0 };
	const RACERMATE::UDPClient::COMSTATS *comstats=NULL;

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

	fastTimer ft("test");
	Ini ini("test.ini");


	broadcast_port = ini.getInt("options", "broadcast port", 9071, true);
	listen_port = ini.getInt("options", "listen port", 9072, true);


	try {
		ft.start();
		racermate_init();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
		ft.stop();
		dt = ft.getSeconds();				// 2 ms
		printf("racermate_init() took %.0lf ms\n", dt*1000);

		//-----------------------------------------------------------------------

		ft.start();
		status = start_server(listen_port, broadcast_port, myip, 2);				// ctsrv
		ft.stop();
		dt = ft.getSeconds();				// 123 ms
		printf("start_server() took %.0lf ms\n", dt * 1000);
		switch(status) {
			case 0:
				break;
			case 1:
				throw(fatalError(__FILE__, __LINE__, "NO NETWORK"));
				break;
			case 2:
				throw(fatalError(__FILE__, __LINE__, "NO BCADDR"));
				break;
			default:
				printf("\nerror %d\n", status);
				throw(fatalError(__FILE__, __LINE__));
				break;
		}

		//-----------------------------------------------------------------------

		ft.start();
		status = start_ant(2);														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call, creates an ANT object, and start a 1000ms TimerProc in internal.cpp
		ft.stop();
		dt = ft.getSeconds();				// 43 ms
		printf("start_ant() took %.0lf ms\n", dt * 1000);
		if(status) {
			bp = 1;
		}
		else {

		}


		ft.start();
		ccptr = getPortNames();												// "" for now, talkes about 6 before portname shows up
		ft.stop();
		dt = ft.getSeconds();												// .1 ms
		strncpy(port, ccptr, sizeof(port) - 1);

		if(port[0]) {
			ccptr = getPortNames();												// "UDP-5678"
			if(ccptr && ccptr[0]) {
				now = timeGetTime();
				dms = now - start;												// 5872 ms
				strncpy(port, ccptr, sizeof(port) - 1);
				what = GetRacerMateDeviceID(port);							// DEVICE_COMPUTRAINER
				fw = GetFirmWareVersion(port);
				bp = 1;
			}
		}

		flag = true;
		start = timeGetTime();

		//-----------------------------------------------------------------------

		while(flag) {
			status = handle_pc_keyboard();
			switch(status) {
				case 1:						// escape
					flag = false;
					continue;
				default:
					break;
			}

			// edge detector:

			if(!port[0])  {
				ccptr = getPortNames();												// "UDP-5678"
				if(ccptr && ccptr[0])  {
					now = timeGetTime();
					dms = now - start;												// 5872 ms
					strncpy(port, ccptr, sizeof(port) - 1);
					what = GetRacerMateDeviceID(port);							// DEVICE_COMPUTRAINER
					fw = GetFirmWareVersion(port);
					bp = 1;
				}
			}
			else {
				bp = 0;
			}

			//Sleep(400);

			if(port[0]) {
				td = GetTrainerData(port, fw);
				keys = GetHandleBarButtons(port, fw);					// keys should be accumulated keys pressed since last call

				bars = get_bars(port, fw);
				average_bars = get_average_bars(port, fw);
				status_bits = get_status_bits(port, fw);
				ssdata = get_ss_data(port, fw);

				if(keys == 0x40) {
					strcpy(keyboard_keys_string, "NC!");
				}
				else {
					mask = 0x01;
					//strcpy(keystr, "(");
					strcpy(keyboard_keys_string, "");
					for(int i = 0; i<6; i++) {
						if(mask & keys) {
							strcat(keyboard_keys_string, "   ");
							if(what == DEVICE_COMPUTRAINER) {
								strcat(keyboard_keys_string, ctkeyname[i]);
							}
							else {
								strcat(keyboard_keys_string, vtkeyname[i]);
							}
						}
						mask <<= 1;
					}
				}

			}			// if (port)


			now = timeGetTime();

			if((now - last_display_time) < 1000) {
				Sleep(sleep_ms);
				continue;
			}

			//----------------------
			// the display
			//----------------------

			last_display_time = now;
			seconds = (float)((now - start) / 1000.0f);
			tick++;

			comstats = get_comstats(port, fw);

			sprintf(gteststring, "%d  %4.0f   u = %ld   seq = %ld   indiff = %ld   0x%02x  %s\n",
						tick,
						td.Power,
						comstats?comstats->ubermsgs:0,
						comstats ? comstats->seqnum : 0,
						comstats ? comstats->indiff : 0,
						keys,
						keyboard_keys_string
			);

			printf("%s", gteststring);
			if(glogstream) {
				fprintf(glogstream, "%s", gteststring);
				fflush(glogstream);
			}

			Sleep(5);
		}

	}
	catch(const char *str) {
		printf("\r\nconst char *exception: %s\r\n\r\n", str);
	}
	catch(fatalError & e) {
		printf("\r\nfatal error: %s\r\n\r\n", e.get_str());
	}
	catch(int &i) {
		printf("\r\nInteger Exception (%d)\r\n\r\n", i);
	}
	catch(...) {
		printf("\r\nUnhandled Exception\r\n\r\n");
	}

	racermate_close();

	return;
}							// emulate_rm2()

							/*
							fw = GetFirmWareVersion(port);
							//assert(fw != 0);

							#if 0
							sensors = get_ant_sensors();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
							if(sensors) {
							if(sensors->n > 0) {
							break;
							}
							}

							if(sensors && sensors->n > 0) {
							associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
							//associate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created

							//status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
							//status = unassociate("UDP-5678", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
							//status = unassociate("zzz", sensors->sensors[0].sn);								//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  takes about 8 seconds for a sensor to appear after device has been created
							}
							#endif
							*/
