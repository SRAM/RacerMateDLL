
#ifdef WIN32
	#pragma warning(disable:4996)			// strncpy_s
#endif


#ifndef WIN32
//	#include <string.h>
//	#include <utils.h>
#else
	#include <process.h>
#endif

#include <fatalerror.h>
#include <err.h>
#include <timeout.h>


/****************************************************************************

****************************************************************************/

Timeout::Timeout(void)  {

	#ifdef WIN32
	hthread = NULL;
	stop_event = 0;
	#endif

	timeout = 0L;
	start_time = 0L;
	running = false;
	kill_timeouts = 0L;
}

/******************************************************************************

******************************************************************************/

Timeout::~Timeout(void)  {
	stop();
}


/******************************************************************************

******************************************************************************/

int Timeout::start(unsigned long _timeout)  {
	timeout = _timeout;

	if (stop_event != 0)  {
		return 1;
	}
	if (hthread==NULL)  {
		return 2;
	}
	if (running)  {
		return 3;
	}

	stop_event = CreateEvent(
								NULL,						// no security attributes
								TRUE,						// create manual reset event
								FALSE,					// initial state is not signaled
								NULL);					// no event name

	if (stop_event == NULL)  {
		return 4;
	}

	running = false;

	//hthread = (HANDLE) _beginthread(&Timeout::thread, 32768, this);
	hthread = (HANDLE) _beginthread(&Timeout::thread, 8*1024, this);
	if (hthread == NULL)  {
		return 5;
	}

	return 0;
}


/******************************************************************************

******************************************************************************/

void Timeout::stop(void)  {
	unsigned long start_time;

	if (stop_event == 0)  {
		return;
	}
	if (hthread == NULL)  {
		return;
	}

	SetEvent(stop_event);

	start_time = timeGetTime();

	while(running)  {
		if ( (timeGetTime()-start_time)>= 1000)  {
			kill_timeouts++;
			break;
		}
		Sleep(20);
	}

	running = false;
	stop_event = 0;
	hthread = NULL;

	return;
}

#define TIMEOUT_STOP_EVENT 0
#define N_TIMEOUT_EVENTS 1

/*************************************************************************

*************************************************************************/

void Timeout::thread(void *tmp)  {
	DWORD dw;
	int bp = 0;
	HANDLE hArray[N_TIMEOUT_EVENTS];
	bool timedout = false;


	Timeout *t = static_cast<Timeout *>(tmp);

	hArray[TIMEOUT_STOP_EVENT] = t->stop_event;

	t->running = true;

	while(1)  {
		dw = WaitForMultipleObjects(N_TIMEOUT_EVENTS, hArray, FALSE, t->timeout);

		switch(dw)  {
			case WAIT_TIMEOUT:  {			// The time-out interval elapsed, and the object's state is nonsignaled. 
				//bp = 1;
				//t->at->update();		// 10.74 ms for 10 ms timeout, 9.7 ms for 9 ms timeout, 1.95 ms for 1 ms timeout
				//break;
				timedout = true;
				goto finis;
			}

			case WAIT_OBJECT_0 + TIMEOUT_STOP_EVENT:  {			// stop_event
				goto finis;
			}

			case WAIT_ABANDONED:			// The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread, and the mutex is set to nonsignaled
				bp = 3;
				throw(fatalError(__FILE__, __LINE__));
				break;

			case WAIT_FAILED: {
#ifdef _DEBUG
				Err *err = new Err();		// the handle is invalid
				DEL(err);
#endif
				throw(fatalError(__FILE__, __LINE__));
				break;
			}

			default:
				bp = 4;
				throw(fatalError(__FILE__, __LINE__));
				break;
		}									// switch()
		//t->at->update();					// 10.74 ms with 10 ms timeout, .0011 ms with no timeout
	}							// while(1)


finis:
	//CloseHandle(ev->stop_event);
	t->stop_event = 0;
	t->hthread = NULL;
	//_endthread();						// return invokes _endthread() anyway
	if (timedout)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	return;
}						// void Timeout::thread(void *tmp)  {
