
#ifndef _DLLDEFS_H_
#define _DLLDEFS_H_

#define MAP


#ifdef LOGDLL
#error "LOGDLL ALREADY DEFINED"
#endif

#ifdef LOGEV
#error "LOGEV ALREADY DEFINED"
#endif


#ifdef _DEBUG
	//#define LOGDLL
	//#define LOGEV
#else
	//#define LOGDLL
	//#define LOGEV
#endif

#endif


