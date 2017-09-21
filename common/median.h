
#ifndef _MEDIAN_H_
#define _MEDIAN_H_


#include "comdefs.h"


#if 0
	/**********************************************************************
	  M E D I A N
	 **********************************************************************/

	/*
		double median(float *data,short n)  {
		short itemp;

		itemp = (short)n;

		qsort(data,itemp,sizeof(float),(int (*)(const void *,const void *))fcomp);
		if ((itemp%2)!=0)  {												// if odd
		return (double)data[itemp/2];
		}
		else  {																// even
		return (data[itemp/2] + data[(itemp/2)-1])/2.0;
		}

		}
		*/
#endif


#ifdef WIN32
//	#include <windows.h>
#endif
//#include "comdefs.h"

/**************************************************************************

**************************************************************************/

class X_EXPORT MEDIAN  {

	private:
		int n=0;
		double *data=NULL;
		static int comp( const void *p1, const void *p2 );

	public:
		MEDIAN(int _n);
		~MEDIAN();
		double calc();

};

#endif		// #ifndef _X_H_

