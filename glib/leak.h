
#ifndef _LEAK_H_
#define _LEAK_H_

	#ifdef WIN32
		#include <windows.h>
		#include <crtdbg.h>
		#include <glib_defines.h>
		#include <logger.h>
	#ifdef DOC
		#include <config.h>
		extern "C" {
	#endif
			/*************************************************************

			*************************************************************/

			class __declspec(dllexport) Leak  {

				private:
				#ifdef _DEBUG
					#define _CRTDBG_MAP_ALLOC
				#endif
					Leak(const Leak&);
					Leak &operator = (const Leak&);		// unimplemented
					_CrtMemState start, stop, diff;
					unsigned long mem;
					Logger *logg;

				public:
					Leak(Logger *_logg=NULL);
					virtual ~Leak();
					void reset(void);
					unsigned long getmem(void);
			};

	#ifdef DOC
		}							// extern "C" {
	#endif

	#else
		#include <glib_defines.h>
		#include <logger.h>
		// linux version
		/*************************************************************

 		*************************************************************/

		class Leak  {

			private:
				Leak(const Leak&);
				Leak &operator = (const Leak&);		// unimplemented
				unsigned long mem;
				Logger *logg;

			public:
				Leak(Logger *_logg=NULL);
				virtual ~Leak();
				void reset(void);
				unsigned long getmem(void);
		};


	#endif		// #ifdef WIN32
#endif

