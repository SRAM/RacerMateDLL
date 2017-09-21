#define WIN32_LEAN_AND_MEAN

#include <unordered_map>

#include <logger.h>
#include <crf.h>
#include <device.h>
#include <ctsrv.h>
#ifdef NEWANT
#include <ant.h>
#endif


//#include <handler.h>

#include "dlldefs.h"

//#include "server.h"

#ifdef DO_EXPORT
	//bool xprt1 = false;
#endif
int dll_ant_timer_event = 0;
int dll_tick = 0;
unsigned long timer_calls = 0L;

char buildDate[32] = {0};
char udp_client_app_key_list[256] = { 0 };
DWORD lastsynctime = 0;
DWORD dllstarttime = timeGetTime();

bool ip_discover = false;
bool udp = false;

//Server *server = NULL;
RACERMATE::Ctsrv *server = NULL;

#ifdef _DEBUG
DWORD gtid = 0;
#endif

#ifdef NEWANT
RACERMATE::ANT *ant = NULL;
#endif


//RACERMATE::Handler handler;

char gstring[2048];
//char portnames[256][16];

Logger *glog = NULL;
int instances = 0;
bool gfile_mode = false;
bool calibrating = false;
bool connected = false;
float nobars[24] = {0};


#ifdef DO_PI
	std::map<std::string, int> portname_to_ix;
	std::map<int, PORT_INFO> ix_to_port_info;
#endif

#ifdef _DEBUG
	unsigned long DEVICE_TIMEOUT_MS = 10*60*1000;						// 10 minute timeout
#else
	unsigned long DEVICE_TIMEOUT_MS = 10000;							// milliseconds
#endif

#ifdef LOGDLL
//#ifdef ERGVIDLOG
	FILE *logstream = NULL;
#endif

#ifdef MAP
//std::unordered_map<const char *, DEVICE> devices;
std::unordered_map<std::string, DEVICE> devices;
#else
DEVICE devices[NPORTS];
#endif


/*
= {
	{ DEVICE_NOT_SCANNED }
};
*/

const char *devstrs[DEVICE_OTHER_ERROR+1] = {
		"DEVICE_NOT_SCANNED",				// unknown, not scanned
		"DEVICE_DOES_NOT_EXIST",			// serial port does not exist
		"DEVICE_EXISTS",						// exists, openable, but no RM device on it
		"DEVICE_COMPUTRAINER",
		"DEVICE_VELOTRON",
		"DEVICE_SIMULATOR",
		"DEVICE_RMP",
		//"DEVICE_WIFI",
		"DEVICE_ACCESS_DENIED",				// port present but can't open it because something else has it open
		"DEVICE_OPEN_ERROR",					// port present, unable to open port
		"DEVICE_NO_URL_AND_OR_TCP_PORT",
		"DEVICE_SERVER_NOT_RUNNING",
		"DEVICE_OTHER_ERROR"					// prt present, error, none of the above
};

bool bluetooth_delay = true;
CRF cryptor;
DWORD last_port_time = 0L;
int glisten_port = 0;										// for server to listen for client connections
int gbroadcast_port = 0;

#ifdef DO_OBFUSCATE
	#if LEVEL >= DLL_TRINERD_ACCESS
		int ctmask[6] = {	0x01, 0x02, 0x08, 0x10, 0x04, 0x20 };
		int vtmask[6] = {	0x02, 0x04, 0x10, 0x01, 0x08, 0x20 };
	#else
	#endif				// #if LEVEL >= DLL_ERGVIDEO_ACCESS
#else
	//const int ctmask[6] = {	0x01, 0x02, 0x08, 0x10, 0x04, 0x20 };
	//const int vtmask[6] = {	0x02, 0x04, 0x10, 0x01, 0x08, 0x20 };
	int ctmask[6] = {	0x01, 0x02, 0x08, 0x10, 0x04, 0x20 };
	int vtmask[6] = {	0x02, 0x04, 0x10, 0x01, 0x08, 0x20 };
#endif					// #ifdef DO_OBFUSCATE

#ifdef LOGDLL
	#ifdef WIN32
		_CrtMemState memstart, memstop, memdiff;
		int crtflags =0;
	#endif
#endif

char personal_path[260];
char DLL_LOGFILE[260];

//std::unordered_map<unsigned short, std::string> associations;
std::unordered_map<std::string, unsigned short> associations;
std::unordered_map<std::string, unsigned short>::const_iterator assgot;
