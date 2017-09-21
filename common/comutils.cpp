
#include <string.h>

#ifdef QT_CORE_LIB
	#include <QDateTime>
#endif

#include "comutils.h"


#ifndef WIN32

/**********************************************************************
  strupr is a microsoft invention
 **********************************************************************/

char *strupr(char *str)  {
	 for(int i=0; i<(int)strlen(str); i++)  {
		  if (str[i]>='a' && str[i]<='z')  {
				str[i] = str[i] - 0x20;
		  }
	 }

	 return str;
}

/**********************************************************************
  strlwr is a microsoft invention
**********************************************************************/

char *strlwr(char *str)  {
	 for(int i=0; i<(int)strlen(str); i++)  {
		  if (str[i]>='A' && str[i]<='Z')  {
				str[i] = str[i] + 0x20;
		  }
	 }

	 return str;
}
#endif


/********************************************************************
  m a p
  returns m,b that maps x1,x2 to y1,y2
  general mapping:
  m = (y2 - y1) / (x2-x1)
  y = mx + (y1 - m*x1)
exit:
returns -1 when mapping is impossible (divide by 0)
********************************************************************/

short map(float x1,float x2,float y1,float y2,float *m,float *b)  {
	float d;

	d = x2 - x1;

	if (d == 0.0f) {		// prevent divide by 0
		*m = 0.0f;
		*b = y1;
		return -1;
	}
	*m = (y2-y1) / (x2-x1);
	*b = (y1 - *m*x1);

	return 0;
}
/********************************************************************
  m a p
  returns m,b that maps x1,x2 to y1,y2
  general mapping:
  m = (y2 - y1) / (x2-x1)
  y = mx + (y1 - m*x1)
exit:
returns -1 when mapping is impossible (divide by 0)
 ********************************************************************/

short dmap(double x1,double x2,double y1,double y2,double *m,double *b)  {
	double d;

	d = x2 - x1;

	if (d == 0.0f) {		// prevent divide by 0
		*m = 0.0f;
		*b = y1;
		return -1;
	}
	*m = (y2-y1) / (x2-x1);
	*b = (y1 - *m*x1);

	return 0;
}


#ifdef QT_CORE_LIB

/********************************************************************
	wrapper for timeGetTime()
********************************************************************/

//unsigned long timeGetTime(void)  {
unsigned long getms(void)  {
	return (unsigned long) QDateTime::currentMSecsSinceEpoch();
}								// getms()

#endif

