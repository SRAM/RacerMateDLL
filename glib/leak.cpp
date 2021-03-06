

#ifdef WIN32
	#include <windows.h>
	#include <leak.h>

	//extern "C" {

	/*************************************************************

	*************************************************************/

	Leak::Leak(Logger *_logg)  {
		logg = _logg;
		reset();
	}

	/*************************************************************

	*************************************************************/

	Leak::~Leak()  {
	}


	/*************************************************************

	*************************************************************/

	void Leak::reset(void)  {
		_CrtMemCheckpoint(&start);
		mem = 0L;
		return;
	}

	/*************************************************************

	*************************************************************/

	unsigned long Leak::getmem(void)  {
		_CrtMemCheckpoint(&stop);
		_CrtMemDifference(&diff, &start, &stop);
		mem = diff.lTotalCount;
		return mem;			
	}

//}						// extern "C" {

#else
	#include <leak.h>

	/*************************************************************
		linux version
	*************************************************************/

	Leak::Leak(Logger *_logg)  {
		logg = _logg;
		reset();
	}

	/*************************************************************

	*************************************************************/

	Leak::~Leak()  {
	}


	/*************************************************************

	*************************************************************/

	void Leak::reset(void)  {
		//_CrtMemCheckpoint(&start);
		mem = 0L;
		return;
	}

	/*************************************************************

	*************************************************************/

	unsigned long Leak::getmem(void)  {
		/*
		_CrtMemCheckpoint(&stop);
		_CrtMemDifference(&diff, &start, &stop);
		mem = diff.lTotalCount;
		return mem;
		*/
		return 0L;
	}

#endif					// #ifdef WIN32


