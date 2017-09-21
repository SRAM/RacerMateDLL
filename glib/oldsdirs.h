
#ifndef _OLDSDIRS_H_
#define _OLDSDIRS_H_


#ifdef WIN32
	#include <windows.h>
#endif

#include <vector>
#include <string>

#include <glib_defines.h>
#include <logger.h>

#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

/**************************************************************************

**************************************************************************/

#ifdef WIN32
class __declspec(dllexport) OLDSDIRS {
#else
class Sdirs  {
#endif


	public:
		//SDIRS(void);
		OLDSDIRS(const char *_appname, Logger *logg=NULL);
		~OLDSDIRS();
		static char gstr[512];
#ifndef DOGLOBS
		static std::vector<std::string > dirs;
#endif

	private:
		char appname[256];
		Logger *logg;
		//void setup_standard_dirs(const char *_appname, Logger *logg=NULL);
};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif


#endif


