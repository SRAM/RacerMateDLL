
#ifndef _SERIAL_H_
#define _SERIAL_H_

	#ifdef WIN32
		#include <win_serial.h>
	#else
		#include <unix_serial.h>
		/*
		#ifdef __MACH__
			#include "unix_serial.h"
		#else
			#include <linux_serial.h>
		#endif
		*/
	#endif

#endif			// #ifndef _SERIAL_H_



