
#ifndef _ERR_H_
#define _ERR_H_

	#ifdef WIN32
		#include <windows.h>
		#include <glib_defines.h>
	#ifdef DOC
		#include <config.h>
		extern "C" {
	#endif
			/*************************************************************

			*************************************************************/

			class __declspec(dllexport) Err  {

				private:
					DWORD code;
					char *gstring;
					Err(const Err&);
					Err &operator = (const Err&);		// unimplemented

				public:
					Err(void);
					virtual ~Err();
					char * getString(void);
			};

	#ifdef DOC
		}							// extern "C" {
	#endif

	#endif		// #ifdef WIN32
#endif

