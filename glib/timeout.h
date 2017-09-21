#ifndef _TIMEOUT_H_
#define _TIMEOUT_H_

#include <stdio.h>

#ifdef WIN32
	#include <windows.h>
	#include <mmsystem.h>
#endif


/****************************************************************************

****************************************************************************/

#ifdef WIN32
	//class __declspec(dllexport) Timeout  {
	class Timeout  {
#else
	class Timeout  {
#endif
		private:
			unsigned long start_time;					// in ms
			unsigned long timeout;						// in ms
			Timeout(const Timeout&);
			Timeout &operator = (const Timeout&);		// unimplemented
			HANDLE hthread;
			HANDLE stop_event;
			static void thread(void *);
			bool running;
			unsigned long kill_timeouts;

		public:
			Timeout(void);
			virtual ~Timeout(void);
			int start(unsigned long _ms);
			void stop(void);
	};

#endif			// #ifndef _AVERAGETIMER_H_
