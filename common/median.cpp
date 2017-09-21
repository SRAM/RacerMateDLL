#include "median.h"

/**************************************************************************

**************************************************************************/

MEDIAN::MEDIAN(int _n)  {
	n = _n;
	data = new double[n];
}

/**************************************************************************

**************************************************************************/

MEDIAN::~MEDIAN(void)  {
	DELARR(data);
}

/**************************************************************************

**************************************************************************/

double MEDIAN::calc(void)  {
	short itemp;
	itemp = (short)n;
	qsort(data,itemp,sizeof(float),(int (*)(const void *,const void *))comp);

	if ((itemp%2)!=0)  {												// if odd
		return (double)data[itemp/2];
	}
	else  {																// even
		return (data[itemp/2] + data[(itemp/2)-1])/2.0;
	}
}

/**********************************************************************

  qsort double comparison

Return Value Description
< 0 elem1 less than elem2
0 elem1 equivalent to elem2
> 0 elem1 greater than elem2

**********************************************************************/

int MEDIAN::comp( const void *p1, const void *p2 ) {
	double f1, f2;

	f1 = *(double *)p1;
	f2 = *(double *)p2;

	if (f1 < f2)  {
		return -1;
	}
	else if(f1 > f2)  {
		return 1;
	}
	else  {
		return 0;
	}

}

