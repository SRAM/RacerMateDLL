
#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
	#pragma once

	#ifndef WEBAPP
		#include <windows.h>
	#endif
#endif


#include <stdio.h>
#include <comglobs.h>
#include <globals.h>
#include <oldsdirs.h>
#include <fatalerror.h>

//extern "C" {

//extern char gstring[1024];

/*****************************************************************************
An error has occured and the program will be shut down. 
Details on this error are stored in path/error.log and should be sent to 
service@racermate.com to determine cause.
*****************************************************************************/

fatalError::fatalError(const char *_filename, int _line, const char *msg)  {
	char path[256];

	filename = _filename;
	line = _line;

	if (msg)  {
		sprintf(str, "\r\nError in %s at line %d:\r\n%s\r\n\r\n", filename, line, msg);
	}
	else  {
		sprintf(str, "\r\nError in %s at line %d\r\n\r\n", filename, line);
	}

	#ifdef _DEBUG
	//OutputDebugString(str);
	#endif

#ifdef DOGLOBS
	if (dirs.size()>0)  {
		sprintf(path, "%s%sfatal.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
		FILE *stream = fopen(path, "wt");
		fprintf(stream, "%s\r\n", str);
		fclose(stream);
	}
#else
	if (SDIRS::dirs.size()>0)  {
		sprintf(path, "%s%sfatal.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR);
		FILE *stream = fopen(path, "wt");
		fprintf(stream, "%s\n", str);
		fclose(stream);
	}
#endif

	return;
}

/*****************************************************************************

*****************************************************************************/

fatalError::~fatalError()  {

}

//}				// extern "C" {


