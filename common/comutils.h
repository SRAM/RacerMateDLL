
#ifndef _COM_UTILS_H_
#define _COM_UTILS_H_

#ifdef QT_CORE_LIB
	#include <QtCore>
#endif

#include "comdefs.h"

#ifndef WIN32
	 char * strupr(char *str);
	 char * strlwr(char *str);
#endif

#ifdef QT_CORE_LIB
	short X_EXPORT map(float x1,float x2,float y1,float y2,float *m,float *b);
	short X_EXPORT dmap(double x1,double x2,double y1,double y2,double *m,double *b);
#else
	 short map(float x1,float x2,float y1,float y2,float *m,float *b);
	 short dmap(double x1,double x2,double y1,double y2,double *m,double *b);
#endif

#ifdef QT_CORE_LIB
	unsigned long X_EXPORT getms(void);								// wrapper for timeGetTime()
	unsigned long getms(void);								// wrapper for timeGetTime()
#endif

#ifndef WIN32
	char * strupr(char *str);
	char * strlwr(char *str);
#endif

#endif

