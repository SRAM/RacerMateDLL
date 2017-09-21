
#if 0
#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4996 4006)

#define DISP  0
#define VELDISP  0
//#define MEMLEAKTEST


#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <assert.h>
#include <time.h>

#include <shlobj.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <new.h>
#include <seh.h>

#include <vector>
#include <string>
//#include <map>



#ifndef MEMLEAKTEST

#include <glib_defines.h>

#include <utils.h>
#include <logger.h>

#include <computrainer.h>
#include <errors.h>

#include <fatalerror.h>
#include <err.h>

#include <globals.h>
#include <dll_globals.h>

#include <ev.h>
#include <dll.h>

void cleanup(void);


void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp);
int handle_program_memory_depletion(size_t);
void iph(const wchar_t * expression, const wchar_t * function, const wchar_t * file, unsigned int line, uintptr_t pReserved );
void SignalHandler(int signal);

void test(void);

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

#endif		//#ifndef MEMLEAKTEST

#endif		// #if 0


#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
    #pragma warning(disable:4996 4006)
#endif


#define COMPDISP  0

/*****************************************************************************************
./tlib/client.h:		int getPortNumber(void)  { return CLIENT_SERIAL_PORT_BASE; }
./tlib/tdefs.h:#define CLIENT_SERIAL_PORT_BASE 231							// 231 - 246
./tlib/rtd.cpp:		if (comportnum >= CLIENT_SERIAL_PORT_BASE && comportnum <= CLIENT_SERIAL_PORT_BASE + 16)  {



./racermate/ev.cpp:		if (realportnum >= CLIENT_SERIAL_PORT_BASE && realportnum <= CLIENT_SERIAL_PORT_BASE+16)  {

./racermate/internal.cpp:	d.port = CLIENT_SERIAL_PORT_BASE;

./racermate/dll.cpp:		if (_comport >= CLIENT_SERIAL_PORT_BASE && _comport <= CLIENT_SERIAL_PORT_BASE)  {
./racermate/dll.cpp:		}													// if (_comport >= CLIENT_SERIAL_PORT_BASE && _comport <= CLIENT_SERIAL_PORT_BASE)  {
./racermate/dll.cpp:		if (_comport == CLIENT_SERIAL_PORT_BASE)  {
./racermate/dll.cpp:				if (portnum == CLIENT_SERIAL_PORT_BASE)  {
./racermate/dll.cpp:		sprintf(str, "COM%d ", CLIENT_SERIAL_PORT_BASE+i);
./racermate/dll.cpp:	sprintf(str, "COM%d", CLIENT_SERIAL_PORT_BASE+i);




./racermate/cpp_tester/test.cpp:	if (port>=CLIENT_SERIAL_PORT_BASE && port <= CLIENT_SERIAL_PORT_BASE+16)  {

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


void test(void);
void test_n_trainers(void);

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


/*********************************************************************************************************

*********************************************************************************************************/


int main(int argc, char *argv[])  {

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
	_crtBreakAlloc = 150;										// 150 is caused by "#include <string>" AND any printf()!!!!

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
		//test();
		test_n_trainers();
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


/**********************************************************************

**********************************************************************/

void test(void)  {
	char url[256] = {0};
	int client_port = 0;
	char str[256];

	char portname[64] = {0};
	TRAINER_COMMUNICATION_TYPE trainer_communication_type = BAD_INPUT_TYPE;

	int broadcast_port = 0;
	int listen_port = 0;
	int ix, fw=0, cal;
	bool b;
	EnumDeviceType what;
	const char *cptr;
	int status;
	int keys = 0x40;
	float watts;
	float rpm, hr;
	float mph;
	char keystr[128];
	float *bars;
	float *average_bars;
	float seconds;
	unsigned long start_time;
	unsigned long sleep_ms = 5L;
	int cnt;
	int i;
	char c;
	SSDATA ssdata;
	DWORD now, last_display_time = 0;
	RACERMATE::TrainerData td;
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

	Bike::GEARPAIR gp = {0};
	int fix = 0;										// index of velotron front gear (0-2)
	int rix = 0;										// index of velotron rear gear (0-9)
	int maxfix = -1;									// highest non-zero index for front gear
	int maxrix = -1;									// highest non-zero index for rear gear
	int Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };																			// up to 5 front gears
	int cogset[MAX_REAR_GEARS] = { 11, 12, 13, 14, 15, 16, 17, 19, 21, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };			// up to 20 rear gears
	float wheeldiameter = 700.0f;					// in mm, 27.0 inches
	int ActualChainring = 62;						// physical number of teeth on velotron front gear
	int Actualcog = 14;								// physical number of teeth on velotron rear gear
	float bike_kgs = (float)(TOKGS*20.0f);		// weight of bike in kgs


	printf("\r\ntest\r\n");

	rm("client.log");

	Ini ini("test.ini");
	racermate_init();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

	status = Setlogfilepath(".");				// sets dirs[DIR_DEBUG] dll call
	if (status != ALL_OK)  {
		sprintf(gteststring, "status = %d", status);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}

	const char *dll_build_date = get_build_date();								// dll call
	printf("\r\ndll build date = %s\r\n\r\n", dll_build_date);

	// RM1 DOES NOT CALL EnableLogs!!!!
	bool _dslog = true;
	bool _decoderlog = true;
	bool _courselog = true;

	bool _bikelog = false;
	bool _gearslog = false;
	bool _physicslog = false;

	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);		// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	//set_timeout(1000*60*10);						// set timeout to 10 minutes

	#ifdef WIN32
		cptr = ini.getString("options", "input", "COM1", true);						// "com20"
	#elif defined __MACH__
		cptr = ini.getString("options", "input", "/dev/ttyUSB0", true);
	#else
		cptr = ini.getString("options", "input", "/dev/tty/USB0", true);
	#endif

	strncpy(portname, cptr, sizeof(portname)-1);
	//strcpy(port, "/dev/tty.usbserial-FTE06HEY");				// usb-serial adapter

	cptr = ini.getString("options", "ipaddr", "10.10.100.254", true);
	strncpy(url, cptr, sizeof(url)-1);
	client_port = ini.getInt("options", "client port", 8899, true);


	#ifdef DO_PI
		PORT_INFO pi;
	#endif

	int iport = -1;

	if ( strstr(portname, "COM"))  {
		int n;
		n = sscanf(portname, "COM%d", &iport);
		assert(n==1);
		assert(iport>=1 && iport < 257);
		#ifdef DO_PI
			pi.portnum = iport;
		#endif

		ix = iport - 1;

		if (iport>=CLIENT_SERIAL_PORT_BASE && iport <= CLIENT_SERIAL_PORT_BASE+16)  {			// trainer is server
			trainer_communication_type = TRAINER_IS_SERVER;																			// trainer is a server
		}
		else if (iport>=SERVER_SERIAL_PORT_BASE && iport <= SERVER_SERIAL_PORT_BASE+16)  {	// trainer is client
			trainer_communication_type = TRAINER_IS_CLIENT;																			// trainer is client
		}
		else if (iport >= UDP_SERVER_SERIAL_PORT_BASE && iport <= UDP_SERVER_SERIAL_PORT_BASE + 16) {								// trainer is UDP client
			trainer_communication_type = TRAINER_IS_CLIENT;																			// trainer is client
		}
		else  {
			trainer_communication_type = WIN_RS232;
		}
	}
	else if ( strstr(portname, "/dev/"))  {
		trainer_communication_type = UNIX_RS232;
		ix = 0;
	}
	else if ( !strcmp(portname, "tcp_server"))  {								// trainer is a server
		trainer_communication_type = TRAINER_IS_SERVER;
		ix = 0;
	}
	else if ( !strcmp(portname, "tcp_client"))  {								// trainer is a client
		trainer_communication_type = TRAINER_IS_CLIENT;
		ix = 0;
	}
	else  {
		throw(fatalError(__FILE__, __LINE__));
	}

	broadcast_port = ini.getInt("options", "broadcast port", 9071, true);
	listen_port = ini.getInt("options", "listen port", 9072, true);
	bp = 2;


	#ifdef WIN32
		assert(trainer_communication_type==WIN_RS232 || trainer_communication_type==TRAINER_IS_SERVER || trainer_communication_type==TRAINER_IS_CLIENT);
	#else
		assert(trainer_communication_type!=WIN_RS232);
        //portname_to_ix[portname] = ix;					// device 0 = my serial port
        //ix_to_portname[ix] = portname;
	#endif

	#ifndef WIN32
		pi.type = trainer_communication_type;
		//strncpy(pi.name, portname, sizeof(pi.name)-1);
		pi.name = portname;
		pi.portnum = iport;

		set_port_info(ix, portname, pi.type, pi.portnum);									// only sets the maps, not the device[].portname
	#endif

	printf("ix = %d, port = %s\r\n", ix, portname);

	if (trainer_communication_type==TRAINER_IS_CLIENT)  {						// trainer is a client
		/*
		status = set_server_network(
						broadcast_port,				// 9071
						listen_port,					// 9099
						false,								// ip_discover
						//true,								// ip_discover
						true								// udp flag
						);				// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   creates server AND STARTS it
		*/

		int debug_level = 2;

		status = set_network_parameters(
			broadcast_port,				// 9071
			listen_port,					// 9099
			false,								// ip_discover
			true,								// udp flag
			ix,
			debug_level

			);				// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   creates server AND STARTS it dll call

		assert(status==ALL_OK);

		//status = start_network_server();											// does nothing
		//assert(status == ALL_OK);
	}
	else if (trainer_communication_type==TRAINER_IS_SERVER)  {						// trainer is a server

		// sets device ix, url, tcp_port, commtype, portname
		//status = set_client_network(ix, url, client_port);				// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		assert(status==ALL_OK);
	}



	const char *ccptr = GetPortNames();						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

	 if (strlen(ccptr) > 0)  {
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
			printf("%s\r\n", portnames[i].c_str());
		}
	 }

	char curdir[256];
	GetCurrentDirectory(sizeof(curdir), curdir);
	printf("curdir = %s\r\n", curdir);

	bool short_circuit = false;						// to just run the server thread, skips to keyboard loop

	if (trainer_communication_type==TRAINER_IS_SERVER) {
		//short_circuit = true;
	}

	//float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	int drag_factor = 100;


if (!short_circuit)  {
	//what = check_for_trainers(port);						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	printf("calling GetRacerMateDeviceID(%d)\r\n", ix);
	what = GetRacerMateDeviceID(ix);						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<, /dev/ttyUSB2 (.../hbsim/3) dll call

	printf("back from GetRacerMateDeviceID\r\n");


	if (what == DEVICE_COMPUTRAINER)  {
		printf("found computrainer on port %s\r\n", portname);
	}
	else if (what == DEVICE_VELOTRON)  {
		printf("found velotron on port %d\r\n", ix+1);
	}
	else if (what == DEVICE_EXISTS)  {						// comment
		sprintf(gteststring, "don't see trainer on port %s\r\n", portname);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}
	else  {
		sprintf(gteststring, "no trainer on port %s", portname);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}


	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		fw = GetFirmWareVersion(ix);

		if (trainer_communication_type==TRAINER_IS_CLIENT)  {						// trainer is a client
			assert(fw==4543);
		}
		else  {
			assert(fw==4095);
		}

		if (FAILED(fw))  {
			cptr = get_errstr(fw);
			sprintf(gteststring, "%s", cptr);
			throw(fatalError(__FILE__, __LINE__, gteststring));
		}
		if (what==DEVICE_VELOTRON)  {
			if (fw != 190)  {
				sprintf(gteststring, "unknown firmware version ( %d )", fw);
				throw(fatalError(__FILE__, __LINE__, gteststring));
			}
		}
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

		status = set_ftp(ix, fw, 1.0f);
		if (status!=DEVICE_NOT_RUNNING)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("ftp set\r\n");


		if (what==DEVICE_VELOTRON)  {
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

			fix = maxfix;									// whatever you want the default gears to be
			rix = 0;											// whatever you want the default gears to be

			status = SetVelotronParameters(
								ix,
								fw,
								maxfix+1,
								maxrix+1,
								Chainrings, 
								cogset, 
								wheeldiameter,				// mm
								ActualChainring, 
								Actualcog,
								bike_kgs,
								//person_kgs,
								fix,						// 2, start off in gear 56/11
								rix						// 9, start off in gear 56/11
								);												// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			if (status!=ALL_OK)  {
				cptr = get_errstr(status);
				throw(fatalError(__FILE__, __LINE__, cptr));
			}

			printf("velotron parameters set\r\n");

		}


		status = startTrainer(ix);										// start trainer <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		//if (!ant_initialized)  {
		#ifdef _DEBUG
			#ifdef DO_ANT
				if (!get_ant_stick_initialized(ix))  {
					bp = 4;
				}
			#endif
		#endif

		if (status!=ALL_OK && status != DEVICE_ALREADY_RUNNING)  {
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

	if (what==DEVICE_VELOTRON)  {
		status = setGear(ix, fw, fix, rix);				// go in to starting gear

		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
	}

	Sleep(1000);											// wait a little for threads to prime the data


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


	if (true)  {
		slope = 100;			// set slope to 1%

		status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);	// sets windload mode
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("wind load mode set\r\n");
	}

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

	float wind_mph = 10.0f;		// in mph

	status = set_wind(ix, fw, (float)(TOKPH*wind_mph));
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	/*
	float draft_wind_mph = 6.66f;			// in mph

	status = set_draftwind(ix, fw, (float)(TOKPH*draft_wind_mph));
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	*/

	status =  SetHRBeepBounds(ix, fw, 39, 139, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}


	keys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call
	if (keys==BAD_FIRMWARE_VERSION)  {
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

}			// if (!short_circuit)  {

//loop:

	cnt = 0;

	glogstream = fopen("test.log", "wt");

	start_time = timeGetTime();

	DWORD tick = 0L;
	bool paused = false;
	//int mode = 0;
	//bool windmode = true;
	int mode = 0;
	float manwatts = 100.0f;
	//int rrc = 200;
	bool calmode = false;

	while(flag)  {
		if (kbhit())  {
			// what can be sent:
			// slope, up/down arrows
			// rider weight 'u'/'d'

			// modes: 'w', 'e', 'c'
			// pause: 'p' toggles

			// manwatts			345
			//	tire drag		1.078 = .006 * rider weight
			//	aero drag		8.0			(drag factor)
			//	minhr
			//	maxhr

			c = getch();
			switch(c)  {
				case 0:   {
					c = getch();
					switch(c)  {
						case 72:							// up arrow
							slope += 10;						// go up by .1%
							status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", slope / 100.0f);
							bp = 3;
							break;
						case 80:							// down arrow
							slope -= 10;						// go down by .1%
							status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", slope / 100.0f);
							bp = 3;
							break;

						default:
							bp = 8;
							break;
					}
					break;
				}												// case 0

				case 0x1b:
					flag = false;
					break;

				case 'w': {									// wind run mode
					status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope / 100.0f);	// sets windload mode
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      wind load mode\n");
					break;
				}
				case 'e': {									// erg run mode
					status = SetErgModeLoad(ix, fw, cal, manwatts);					// bug
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      ergo mode\n");
					break;
				}
				case 'c': {									// cal mode
					calmode = !calmode;

					if (calmode) {
						status = SetRecalibrationMode(ix, fw);
						printf("      cal mode entered\n");
					}
					else {
						status = EndRecalibrationMode(ix, fw);
						printf("      cal mode exit\n");
					}
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					break;
				}

				case 'p':
					paused = !paused;
					status = setPause(ix, paused);								// bug
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      paused = %s\n", paused ? "true" : "false");
					break;

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
					else if (what == DEVICE_COMPUTRAINER) {
						person_kgs += .5f;
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope / 100.0f);
						if (status != ALL_OK) {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						printf("      weight = %.1lf\n", KGSTOLBS*person_kgs);
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

				case 'd':																// rear gear on velotron down
					if (what==DEVICE_VELOTRON)  {
						if (rix > 0)  {
							rix--;
						}
						status = setGear(ix, fw, fix, rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					else if (what == DEVICE_COMPUTRAINER) {
						person_kgs -= .5f;
						status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope / 100.0f);
						if (status != ALL_OK) {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						printf("      weight = %.1lf\n", KGSTOLBS*person_kgs);
					}
					break;
				case 'D':																	// front gear on velotron down
					if (what==DEVICE_VELOTRON)  {
						if (fix > 0)  {
							fix--;
						}
						status = setGear(ix, fw, fix, rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;

				default:
					bp = 7;
					break;
			}													// switch (c)
		}														// if (kbhit())  {

		if (short_circuit)  {
			Sleep(100);
			continue;
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

		if ((now - last_display_time) >= 1000) {
			last_display_time = now;
			seconds = (float)((now - start_time) / 1000.0f);
			tick++;

#ifdef _DEBUG
			if (what == DEVICE_VELOTRON) {
				gp = GetCurrentVTGear(ix, fw);

				#if VELDISP==0
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
				if (glogstream) {
					fprintf(glogstream, "%s", gteststring);
				}
			}
			else if (what == DEVICE_COMPUTRAINER) {

	#if COMPDISP==0
				sprintf(gteststring, "%6ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s",
					tick,
					mph,
					rpm,
					hr,
					watts,
					keys & 0x000000ff,
					keystr
					);
				strcat(gteststring, "\n");
	#elif COMPDISP==1
				gteststring[0] = 0;
				for (i = 0; i < 23; i++) {
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
	#endif					// #if COMPDISP==0
#else							// #ifdef _DEBUG
			sprintf(gteststring, "%5ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s\r\n",
				tick,
				mph,
				rpm,
				hr,
				watts,
				keys & 0x000000ff,
				keystr
				);
#endif									// #ifdef _DEBUG

				printf("%s", gteststring);
				if (glogstream) {
					fprintf(glogstream, "%s", gteststring);
					fflush(glogstream);
					//fclose(glogstream);
					//glogstream = fopen("test.log", "a+t");
				}
			}
		}

		Sleep(sleep_ms);
	}


	FCLOSE(glogstream);


	if (!short_circuit)  {
		status = stopTrainer(ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}


		if (what==DEVICE_COMPUTRAINER)  {
			printf("computrainer stopped\r\n");
		}
		else  {
			printf("velotron stopped\r\n");
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
}							// test()

class Trainer  {
	public:
		EnumDeviceType what;
		int ix;
		int iport;
		char portname[64];
		TRAINER_COMMUNICATION_TYPE comtype;
		int fw;
		int cal;

		Trainer(void)  {
			what = DEVICE_NOT_SCANNED;
			ix = -1;
			iport = -1;
			memset(portname, 0, sizeof(portname));
			comtype = BAD_INPUT_TYPE;
			fw = 0;
			cal = 0;

			return;
		}
};

/**********************************************************************

**********************************************************************/

void test_n_trainers(void)  {
	char url[256] = {0};
	int client_port = 0;
	char str[256];


	int broadcast_port = 0;
	int listen_port = 0;
	bool b;
	const char *cptr;
	int status;
	int keys = 0x40;
	float watts;
	float rpm, hr;
	float mph;
	char keystr[128];
	float *bars;
	float *average_bars;
	float seconds;
	unsigned long start_time;
	unsigned long sleep_ms = 5L;
	int cnt;
	char c;
	SSDATA ssdata;
	DWORD now, last_display_time = 0;
	RACERMATE::TrainerData td;
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

	Bike::GEARPAIR gp = {0};
	int fix = 0;										// index of velotron front gear (0-2)
	int rix = 0;										// index of velotron rear gear (0-9)
	int maxfix = -1;									// highest non-zero index for front gear
	int maxrix = -1;									// highest non-zero index for rear gear
	int Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };																			// up to 5 front gears
	int cogset[MAX_REAR_GEARS] = { 11, 12, 13, 14, 15, 16, 17, 19, 21, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };			// up to 20 rear gears
	float wheeldiameter = 700.0f;					// in mm, 27.0 inches
	int ActualChainring = 62;						// physical number of teeth on velotron front gear
	int Actualcog = 14;								// physical number of teeth on velotron rear gear
	float bike_kgs = (float)(TOKGS*20.0f);		// weight of bike in kgs
	int k;

	printf("\r\ntest\r\n");

	rm("client.log");

	Ini ini("test.ini");
	racermate_init();														//<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

	status = Setlogfilepath(".");				// sets dirs[DIR_DEBUG] dll call
	if (status != ALL_OK)  {
		sprintf(gteststring, "status = %d", status);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}

	const char *dll_build_date = get_build_date();								// dll call
	printf("\r\ndll build date = %s\r\n\r\n", dll_build_date);

	// RM1 DOES NOT CALL EnableLogs!!!!
	bool _dslog = true;
	bool _decoderlog = true;
	bool _courselog = true;

	bool _bikelog = false;
	bool _gearslog = false;
	bool _physicslog = false;

	status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);		// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call
	if (status != ALL_OK)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	//set_timeout(1000*60*10);						// set timeout to 10 minutes

	broadcast_port = ini.getInt("options", "broadcast port", 9071, true);
	listen_port = ini.getInt("options", "listen port", 9072, true);


	const char *ccptr = GetPortNames();						// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< dll call

	 if (strlen(ccptr) > 0)  {
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
		 //int i;
		for(int i=0; i<(int)portnames.size(); i++)  {
			printf("%s\r\n", portnames[i].c_str());
		}
	 }

	char curdir[256];
	GetCurrentDirectory(sizeof(curdir), curdir);
	printf("curdir = %s\r\n", curdir);

	//float bike_kgs = (float)(TOKGS*20.0f);
	float person_kgs = (float)(TOKGS*150.0f);
	int drag_factor = 100;

	cptr = GetAPIVersion();
	printf("api version = %s\r\n", cptr);

	cptr = get_dll_version();
	printf("dll version = %s\r\n", cptr);



#define N 2

	Trainer trainers[N];

	int ixs[N] = {19, 220};

	for(k=0; k<N; k++)  {
		trainers[k].ix = ixs[k];
		trainers[k].iport = trainers[k].ix + 1;
		#ifdef WIN32
			sprintf(trainers[k].portname, "COM%d", trainers[k].iport);
		#elif defined __MACH__
		#else
		#endif

		if (k==1)  {
			bp = 7;
		}

		if (trainers[k].iport>=CLIENT_SERIAL_PORT_BASE && trainers[k].iport <= CLIENT_SERIAL_PORT_BASE+16)  {			// trainer is server
			trainers[k].comtype = TRAINER_IS_SERVER;																			// trainer is a server
		}
		else if (trainers[k].iport>=SERVER_SERIAL_PORT_BASE && trainers[k].iport <= SERVER_SERIAL_PORT_BASE+16)  {	// trainer is client
			trainers[k].comtype = TRAINER_IS_CLIENT;																			// trainer is client
		}
		else if (trainers[k].iport >= UDP_SERVER_SERIAL_PORT_BASE && trainers[k].iport <= UDP_SERVER_SERIAL_PORT_BASE + 16) {								// trainer is UDP client
			trainers[k].comtype = TRAINER_IS_CLIENT;																			// trainer is client
		}
		else  {
			trainers[k].comtype = WIN_RS232;
		}


		if (trainers[k].comtype==TRAINER_IS_CLIENT)  {						// trainer is a client
			int debug_level = 2;

			status = set_network_parameters(
				broadcast_port,				// 9071
				listen_port,					// 9099
				false,								// ip_discover
				true,								// udp flag
				trainers[k].ix,
				debug_level
				);
				assert(status==ALL_OK);
		}

		bp = 17;

		trainers[k].what = GetRacerMateDeviceID(trainers[k].ix);

		if (trainers[k].what == DEVICE_COMPUTRAINER)  {
			printf("found computrainer on port %s\r\n", trainers[k].portname);
		}
		else if (trainers[k].what == DEVICE_VELOTRON)  {
			printf("found velotron on port %s\n", trainers[k].portname);
		}
		else if (trainers[k].what == DEVICE_EXISTS)  {
			sprintf(gteststring, "don't see trainer on port %s\r\n", trainers[k].portname);
			throw(fatalError(__FILE__, __LINE__, gteststring));
		}
		else  {
			sprintf(gteststring, "no trainer on port %s", trainers[k].portname);
			throw(fatalError(__FILE__, __LINE__, gteststring));
		}

		if (trainers[k].what==DEVICE_COMPUTRAINER || trainers[k].what==DEVICE_VELOTRON)  {
			trainers[k].fw = GetFirmWareVersion(trainers[k].ix);
			if (trainers[k].comtype==TRAINER_IS_CLIENT)  {						// trainer is a client
				assert(trainers[k].fw==4543);
			}
			else  {
				assert(trainers[k].fw==4095);
			}

			if (FAILED(trainers[k].fw))  {
				cptr = get_errstr(trainers[k].fw);
				sprintf(gteststring, "%s", cptr);
				throw(fatalError(__FILE__, __LINE__, gteststring));
			}
			if (trainers[k].what==DEVICE_VELOTRON)  {
				if (trainers[k].fw != 190)  {
					sprintf(gteststring, "unknown firmware version ( %d )", trainers[k].fw);
					throw(fatalError(__FILE__, __LINE__, gteststring));
				}
			}
			printf("firmware = %d\r\n", trainers[k].fw);
		}

		b = GetIsCalibrated(trainers[k].ix, trainers[k].fw);									// false
		printf("calibrated = %s\r\n", b?"true":"false");

		if (trainers[k].what==DEVICE_COMPUTRAINER || trainers[k].what==DEVICE_VELOTRON)  {
			trainers[k].cal = GetCalibration(trainers[k].ix);										// 200
			printf("cal = %d\r\n", trainers[k].cal);

			status = set_ftp(trainers[k].ix, trainers[k].fw, 1.0f);
			if (status!=DEVICE_NOT_RUNNING)  {
				cptr = get_errstr(status);
				throw(fatalError(__FILE__, __LINE__, cptr));
			}

			printf("ftp set\r\n");

			if (trainers[k].what==DEVICE_VELOTRON)  {
				maxfix = -1;
				for(int i=0; i<MAX_FRONT_GEARS; i++)  {
					if (Chainrings[i]==0)  {
						break;
					}
					maxfix++;
				}

				maxrix = -1;
				for(int i=0; i<MAX_REAR_GEARS; i++)  {
					if (cogset[i]==0)  {
						break;
					}
					maxrix++;
				}

				bp = 1;

				fix = maxfix;									// whatever you want the default gears to be
				rix = 0;											// whatever you want the default gears to be

				status = SetVelotronParameters(
									trainers[k].ix,
									trainers[k].fw,
									maxfix+1,
									maxrix+1,
									Chainrings, 
									cogset, 
									wheeldiameter,				// mm
									ActualChainring, 
									Actualcog,
									bike_kgs,
									//person_kgs,
									fix,						// 2, start off in gear 56/11
									rix						// 9, start off in gear 56/11
									);												// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

				if (status!=ALL_OK)  {
					cptr = get_errstr(status);
					throw(fatalError(__FILE__, __LINE__, cptr));
				}

				printf("velotron parameters set\r\n");

				status = setGear(trainers[k].ix, trainers[k].fw, fix, rix);				// go in to starting gear

				if (status!=ALL_OK)  {
					cptr = get_errstr(status);
					throw(fatalError(__FILE__, __LINE__, cptr));
				}
			}					// if (what==DEVICE_VELOTRON)  {
		}						// if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {

		status = startTrainer(trainers[k].ix);										// start trainer <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		if (status!=ALL_OK && status != DEVICE_ALREADY_RUNNING)  {
			cptr = get_errstr(status);
			racermate_close();
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		if (trainers[k].what==DEVICE_COMPUTRAINER)  {
			printf("computrainer started\r\n");
		}
		else  {
			printf("velotron started\r\n");
		}

		if (trainers[k].what==DEVICE_COMPUTRAINER || trainers[k].what==DEVICE_VELOTRON)  {
			status = resetTrainer(trainers[k].ix, trainers[k].fw, trainers[k].cal);									// resets ds, decoder, etc
			if (status!=ALL_OK)  {
				cptr = get_errstr(status);
				throw(fatalError(__FILE__, __LINE__, cptr));
			}
			printf("trainer reset\r\n");
		}

		status = ResetAverages(trainers[k].ix, trainers[k].fw);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
		printf("averages reset\r\n");

		slope = 100;			// set slope to 1%

		status = SetSlope(trainers[k].ix, trainers[k].fw, trainers[k].cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);	// sets windload mode
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("wind load mode set\r\n");

		status = setPause(trainers[k].ix, true);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("paused\r\n");

		Sleep(500);

		status = setPause(trainers[k].ix, false);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("unpaused\r\n");

		float wind_mph = 10.0f;		// in mph

		status = set_wind(trainers[k].ix, trainers[k].fw, (float)(TOKPH*wind_mph));
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		status =  SetHRBeepBounds(trainers[k].ix, trainers[k].fw, 39, 139, true);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}


		keys = GetHandleBarButtons(trainers[k].ix, trainers[k].fw);			// keys should be accumulated keys pressed since last call
		if (keys==BAD_FIRMWARE_VERSION)  {
			throw(fatalError(__FILE__, __LINE__, cptr));
		}



	}							// for(i=0; i<N; i++)  {

	bp = 1;

	cnt = 0;

	glogstream = fopen("test.log", "wt");

	start_time = timeGetTime();

	DWORD tick = 0L;
	bool paused = false;
	int mode = 0;
	float manwatts = 100.0f;
	bool calmode = false;
	int iix = 0;									// trainer 0

	while(flag)  {
		if (kbhit())  {
			c = getch();
			switch(c)  {
				case 0:   {
					c = getch();
					switch(c)  {
						case 72:							// up arrow
							slope += 10;						// go up by .1%
							status = SetSlope(trainers[iix].ix, trainers[iix].fw, trainers[iix].cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", slope / 100.0f);
							bp = 3;
							break;
						case 80:							// down arrow
							slope -= 10;						// go down by .1%
							status = SetSlope(trainers[iix].ix, trainers[iix].fw, trainers[iix].cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);
							if (status!=ALL_OK)  {
								cptr = get_errstr(status);
								throw(fatalError(__FILE__, __LINE__, cptr));
							}
							printf("      slope = %.1f\n", slope / 100.0f);
							bp = 3;
							break;

						default:
							bp = 8;
							break;
					}
					break;
				}												// case 0

				case 0x1b:
					flag = false;
					break;

				case 'w': {									// wind run mode
					status = SetSlope(trainers[iix].ix, trainers[iix].fw, trainers[iix].cal, bike_kgs, person_kgs, drag_factor, slope / 100.0f);	// sets windload mode
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      wind load mode\n");
					break;
				}
				case 'e': {									// erg run mode
					status = SetErgModeLoad(trainers[iix].ix, trainers[iix].fw, trainers[iix].cal, manwatts);					// bug
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      ergo mode\n");
					break;
				}
				case 'c': {									// cal mode
					calmode = !calmode;

					if (calmode) {
						status = SetRecalibrationMode(trainers[iix].ix, trainers[iix].fw);
						printf("      cal mode entered\n");
					}
					else {
						status = EndRecalibrationMode(trainers[iix].ix, trainers[iix].fw);
						printf("      cal mode exit\n");
					}
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					break;
				}

				case 'p':
					paused = !paused;
					status = setPause(trainers[iix].ix, paused);								// bug
					if (status != ALL_OK) {
						cptr = get_errstr(status);
						throw(fatalError(__FILE__, __LINE__, cptr));
					}
					printf("      paused = %s\n", paused ? "true" : "false");
					break;

				case 'u':																// rear gear on velotron up
					if (trainers[iix].what==DEVICE_VELOTRON)  {
						if (rix < maxrix)  {
							rix++;
						}
						status = setGear(trainers[iix].ix, trainers[iix].fw, fix, rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					else if (trainers[iix].what == DEVICE_COMPUTRAINER) {
						person_kgs += .5f;
						status = SetSlope(trainers[iix].ix, trainers[iix].fw, trainers[iix].cal, bike_kgs, person_kgs, drag_factor, slope / 100.0f);
						if (status != ALL_OK) {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						printf("      weight = %.1lf\n", KGSTOLBS*person_kgs);
					}
					break;
				case 'U':																	// front gear on velotron up
					if (trainers[iix].what==DEVICE_VELOTRON)  {
						if (fix < maxfix)  {
							fix++;
						}
						status = setGear(trainers[iix].ix, trainers[iix].fw, fix, rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;

				case 'd':																// rear gear on velotron down
					if (trainers[iix].what==DEVICE_VELOTRON)  {
						if (rix > 0)  {
							rix--;
						}
						status = setGear(trainers[iix].ix, trainers[iix].fw, fix, rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					else if (trainers[iix].what == DEVICE_COMPUTRAINER) {
						person_kgs -= .5f;
						status = SetSlope(trainers[iix].ix, trainers[iix].fw, trainers[iix].cal, bike_kgs, person_kgs, drag_factor, slope / 100.0f);
						if (status != ALL_OK) {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
						printf("      weight = %.1lf\n", KGSTOLBS*person_kgs);
					}
					break;
				case 'D':																	// front gear on velotron down
					if (trainers[iix].what==DEVICE_VELOTRON)  {
						if (fix > 0)  {
							fix--;
						}
						status = setGear(trainers[iix].ix, trainers[iix].fw, fix, rix);
						if (status!=ALL_OK)  {
							cptr = get_errstr(status);
							throw(fatalError(__FILE__, __LINE__, cptr));
						}
					}
					break;

				default:
					bp = 7;
					break;
			}													// switch (c)
		}														// if (kbhit())  {

		now = timeGetTime();
		td = GetTrainerData(trainers[iix].ix, trainers[iix].fw);
		
		watts = td.Power;
		mph = (float)(TOMPH*td.kph);
		rpm = td.cadence;
		hr = td.HR;


		keys = GetHandleBarButtons(trainers[iix].ix, trainers[iix].fw);					// keys should be accumulated keys pressed since last call
		bars = get_bars(trainers[iix].ix, trainers[iix].fw);
		average_bars = get_average_bars(trainers[iix].ix, trainers[iix].fw);
		ssdata = get_ss_data(trainers[iix].ix, trainers[iix].fw);


		if (keys==0x40)  {
			strcpy(keystr, "NO COMMUNICATION");
		}
		else  {
			mask = 0x01;
			//strcpy(keystr, "(");
			strcpy(keystr, "");
			for(int i=0; i<6; i++)  {
				if (mask & keys)  {
					strcat(keystr, "   ");
					if (trainers[iix].what == DEVICE_COMPUTRAINER)  {
						strcat(keystr, ctkeyname[i]);
					}
					else  {
						strcat(keystr, vtkeyname[i]);
					}
				}
				mask <<= 1;
			}
		}

		if ((now - last_display_time) >= 1000) {
			last_display_time = now;
			seconds = (float)((now - start_time) / 1000.0f);
			tick++;

			if (trainers[iix].what == DEVICE_VELOTRON) {
				gp = GetCurrentVTGear(trainers[iix].ix, trainers[iix].fw);

				#if VELDISP==0
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
				if (glogstream) {
					fprintf(glogstream, "%s", gteststring);
				}
			}
			else if (trainers[iix].what == DEVICE_COMPUTRAINER) {
				#if COMPDISP==0
					sprintf(gteststring, "%6ld  MPH = %6.1f  CAD = %6.1f  HR = %6.1f  w=%6.1f  k=%02x  %s",
						tick,
						mph,
						rpm,
						hr,
						watts,
						keys & 0x000000ff,
						keystr
						);
					strcat(gteststring, "\n");
				#elif COMPDISP==1
					gteststring[0] = 0;
					for (i = 0; i < 23; i++) {
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
				#endif					// #if COMPDISP==0

				printf("%s", gteststring);
				if (glogstream) {
					fprintf(glogstream, "%s", gteststring);
					fflush(glogstream);
					//fclose(glogstream);
					//glogstream = fopen("test.log", "a+t");
				}
			}
		}

		Sleep(sleep_ms);
	}


	FCLOSE(glogstream);

	for(k=0; k<N; k++)  {
		status = stopTrainer(trainers[k].ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
		if (trainers[k].what==DEVICE_COMPUTRAINER)  {
			printf("computrainer stopped\r\n");
		}
		else  {
			printf("velotron stopped\r\n");
		}

		status = ResettoIdle(trainers[k].ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
	}

	status = racermate_close();
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	return;
}							// test_n_trainers()

#endif				// #ifndef MEMLEAKTEST
