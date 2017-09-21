#ifndef _AVERAGETIMER_H_
#define _AVERAGETIMER_H_

//http://doc.qt.io/qt-5.5/qtglobal.html#Q_OS_WIN32
//#ifdef Q_OS_WIN32
//Q_WS_X11
//See also Q_WS_MAC, Q_WS_WIN, Q_WS_QWS, Q_WS_QPA, and Q_WS_S60
//Q_WS_MAC
//Q_OS_LINUX
//Q_CC_MSVC

#ifdef Q_CREATOR_RUN
//xxx
	int bp = "xxx";
#endif

#ifdef WIN32
	#pragma once
	#include <windows.h>
	#include <mmsystem.h>
#else
	#include <glib_defines.h>
#endif

#include <stdio.h>


/****************************************************************************

****************************************************************************/

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
	extern "C" {
	#endif
	class __declspec(dllexport) AverageTimer  {
	//class DLLEXPORT AverageTimer  {
#else
		class AverageTimer  {
#endif

			private:
				DWORD now;
				DWORD dt;
				DWORD maxdt;
				DWORD mindt;
				DWORD lastTime;
				DWORD totaldt;
				DWORD count;
				char name[32];
				AverageTimer(const AverageTimer&);
				AverageTimer &operator = (const AverageTimer&);		// unimplemented

			public:
				AverageTimer(void);
				AverageTimer(const char *_name);
				virtual ~AverageTimer(void);
				void update(void);
				double getAverage(void);
				double getms(void);
				void reset(void);
		};

#ifdef WIN32
	#ifdef DOC
	}			// extern "C" {
	#endif
#endif

#endif		// #ifndef _AVERAGETIMER_H_
