
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
	#include <windows.h>
#else
	#include "glib_defines.h"
#endif


#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
#endif


#ifdef WIN32
	//class __declspec(dllexport) Logger  {
	class Logger  {
#else
	class Logger  {
#endif


		private:
			char gstring[MAX_PATH];
			char currentDirectory[_MAX_PATH];
			char file[_MAX_PATH];
			char stub[_MAX_PATH];
			BOOL closed;
			void cleanup_logfiles(void);
			Logger(const Logger&);						// unimplemented
			Logger &operator = (const Logger&);		// unimplemented
			char folder[256];
			
		public:

			Logger(const char *stub, const char *_folder=".");
			virtual ~Logger(void);
			void write(int level, int printdepth, int reset, const char *format, ...);
			void write(const char *format, ...);
			void close(void);
			void dump(unsigned char *mb, int cnt);
			void flush(void);
			int loglevel;
			FILE *stream;
			int import_file(const char *_fname);
			
	};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _LOGGER_H_

