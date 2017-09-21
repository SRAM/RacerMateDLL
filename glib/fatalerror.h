#ifndef _FATAL_ERROR_H_
#define _FATAL_ERROR_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <stdio.h>

#include <glib_defines.h>

#ifdef WIN32
	#ifdef DOC
		#include <config.h>
		extern "C" {
	#endif
class __declspec(dllexport) fatalError  {
#else
class fatalError  {
#endif


	private:
		const char *filename;
		int line;
		char str[256];
		//fatalError(const fatalError&);
		fatalError &operator = (const fatalError&);		// unimplemented

	public:
		fatalError(const char *_filename, int _line, const char *msg=NULL);
		virtual ~fatalError();
		const char *get_str(void)  { return str; }
		const char *get_filename(void)  { return filename; }
		int get_line(void)  { return line; }
};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif


