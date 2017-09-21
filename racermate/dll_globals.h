#ifndef _DLL_GLOBALS_H_
#define _DLL_GLOBALS_H_

#define WIN32_LEAN_AND_MEAN

#include <unordered_map>

#include <glib_defines.h>

#include <logger.h>

#include <crf.h>
//#include "dll_common.h"
#include <device.h>
#include <ctsrv.h>

//#include <handler.h>


#include "dll.h"
//#include "server.h"
#ifdef NEWANT
#include <ant.h>
#endif

#ifdef _DEBUG
extern DWORD gtid;
#endif


//extern RACERMATE::Handler handler;
extern int dll_ant_timer_event;
extern int dll_tick;
extern unsigned long timer_calls;

extern char DLL_LOGFILE[260];
extern char personal_path[260];
extern float nobars[24];
extern char gstring[2048];
extern Logger *glog;
extern int instances;
extern bool gfile_mode;
extern bool calibrating;
extern bool connected;
extern unsigned long DEVICE_TIMEOUT_MS;

extern std::unordered_map<std::string, DEVICE> devices;
//extern std::unordered_map<const char *, DEVICE> devices;
//extern std::vector<const char *, DEVICE> devices;
//extern std::unordered_map<int, const char *> associations;
//extern std::unordered_map<unsigned short, std::string> associations;
extern std::unordered_map<std::string, unsigned short> associations;
extern std::unordered_map<std::string, unsigned short>::const_iterator assgot;


extern const char *devstrs[DEVICE_OTHER_ERROR+1];
extern bool bluetooth_delay;
extern CRF cryptor;
extern DWORD last_port_time;
extern int glisten_port;
extern int gbroadcast_port;
extern bool ip_discover;
extern bool udp;
extern char buildDate[32];
extern char udp_client_app_key_list[256];
extern DWORD lastsynctime;
extern DWORD dllstarttime;

//extern Server *server;
extern RACERMATE::Ctsrv *server;

#ifdef NEWANT
extern RACERMATE::ANT *ant;
#endif

//extern char portnames[256][16];
//extern char url[256];
//extern int tcp_port;

#ifdef DO_PI
extern std::map<std::string, int> portname_to_ix;
extern std::map<int, PORT_INFO> ix_to_port_info;
#endif

#ifdef DO_EXPORT
	//extern bool xprt1;
#endif

#ifdef LOGDLL
	extern FILE *logstream;
#endif

#ifdef DO_OBFUSCATE
	#if LEVEL >= DLL_TRINERD_ACCESS
		extern int ctmask[6];
		extern int vtmask[6];
	#endif
#else
	//extern const int ctmask[6];
	//extern const int vtmask[6];
	extern int ctmask[6];
	extern int vtmask[6];
#endif

#ifdef LOGDLL
	#ifdef WIN32
		extern _CrtMemState memstart, memstop, memdiff;
		extern int crtflags;
	#endif
#endif

#endif											// #ifndef _DLL_GLOBALS_H_

