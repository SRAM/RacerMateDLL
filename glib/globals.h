
#ifndef _MYGLOBALS_H_
#define _MYGLOBALS_H_

#include <glib_defines.h>

#ifdef DOGLOBS

	#ifdef WIN32
		#include <windows.h>
	#endif

	#include <vector>
	#include <string>


	//extern char gstring[2048];

	//#define NDIRS 8
	//extern char dirs[NDIRS][MAX_PATH];
	//extern std::vector<std::string> dirs;

	#ifdef _DEBUG
		//extern bool oktobreak;
		//extern int bp;
	#endif

	//extern bool ogl_log;
	//extern bool main_course_log;

	#endif		// DOGLOBS
#endif		// #ifndef _MYGLOBALS_H_

