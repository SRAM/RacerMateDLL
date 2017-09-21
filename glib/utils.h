
#ifndef _UTILS_H_
#define _UTILS_H_

	#ifdef WIN32
		#include <win_utils.h>
	#else
		#include <linux_utils.h>
	#endif
#endif		// #ifndef _UTILS_H_

