
#ifndef _LEVELS_H_
#define _LEVELS_H_

#include <glib_defines.h>
#ifdef DLLSPEC
	#error "DLLSPEC ALREADY DEFINED"
#endif


#ifdef _DEBUG
	//#define DO_OBFUSCATE
#else
	//#define DO_OBFUSCATE
#endif


#ifdef WIN32
	#define DLLSPEC __declspec(dllexport)
#else
	#ifdef __MACH__
		#define DLLSPEC
	#else
		// linux
		#define DLLSPEC 
	#endif
#endif

#define DLL_FULL_ACCESS				65536
#define DLL_ERGVIDEO_ACCESS		32768
#define DLL_TRINERD_ACCESS			24000
#define DLL_CANNONDALE_ACCESS		16384
#define DLL_MINIMUM_ACCESS				1

//#define LEVEL	DLL_FULL_ACCESS
#define LEVEL	DLL_ERGVIDEO_ACCESS
//#define LEVEL	DLL_TRINERD_ACCESS
//#define LEVEL	DLL_CANNONDALE_ACCESS
//#define LEVEL DLL_MINIMUM_ACCESS

#endif				// #ifndef _LEVELS_H_


