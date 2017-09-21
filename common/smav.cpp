
//#include <glib_defines.h>

#include "smav.h"

#ifdef DOSIGMA
	#include <math.h>
#endif

//extern "C" {

/**************************************************************************
	simple moving average class
**************************************************************************/

#ifdef DOSIGMA
sMav::sMav(int n, bool _dosigma)  {
#else
sMav::sMav(int n)  {
#endif

#ifdef _DEBUG
	bp = 0;
	count = 0L;
#endif

	len = 0;
	raw = NULL;
	fval = NULL;

	#ifdef DOSIGMA
	dosigma = _dosigma;
	sigma = 0.0;
	#endif

	if (n==0)  {
		return;
	}

	curval = 0.0f;
	len = n;
	raw = new float[(unsigned int)n];
	fval = new float[(unsigned int)n];

	#ifdef DOSIGMA
	if (dosigma)  {
		squares = new double[(unsigned int)n];
	}
	else  {
		squares = NULL;
	}
	#endif

	shiftlen = 0;
	reset();

}

/**************************************************************************

**************************************************************************/

sMav::~sMav(void)  {
	//DELARR(raw);
	//DELARR(fval);
	delete[] raw;
	raw = 0;

	delete[] fval;
	fval = 0;

	#ifdef DOSIGMA
		//DELARR(squares);
		delete[] squares;
		squares = 0;
	#endif

}

/**************************************************************************

**************************************************************************/

float sMav::compute(float input)  {
	float ftemp;
	double d;

	sum -= raw[k];			// take out the oldest
	raw[k] = input;			// replace oldest with newest;
	sum += input;
	ftemp = sum / (float)len;
	fval[k] = ftemp;
	curval = ftemp;

	#ifdef DOSIGMA
	if (dosigma)  {		// compute the standard deviation for bollinger bands
		d = (curval - input) * (curval - input);
		squares_sum -= squares[k];
		squares[k] = d;
		squares_sum += d;
		sigma = sqrt(squares_sum / (double)len);
		#ifdef _DEBUG
		bp = 2;
		#endif
	}
	#endif

	int i = k;
	k = (k+1) % len;
	return fval[i];
}

/*****************************************************************
	returns the oldest value in the array
*****************************************************************/

float sMav::first(void)  {
	return raw[k];
}

/*****************************************************************
	returns the newest value in the array
*****************************************************************/

float sMav::last(void)  {
	int i;
	i = k;
	i--;
	if (i<0) i = len-1;
	return raw[i];
}

/*****************************************************************

*****************************************************************/

void sMav::reset(float val)  {
	int i;

	for(i=0; i<len; i++)  {
		raw[i] = 0.0f;
		fval[i] = 0.0f;
		#ifdef DOSIGMA
			if (squares)  {
				squares[i] = 0.0;
			}
		#endif
	}

	k = 0;
	//sum = 0.0f;
	sum = len*val;

	#ifdef DOSIGMA
	squares_sum = 0.0;
	//squares_sum = len * val;
	#endif

	if (raw)  {
		for(int i=0;i<len;i++)  {
			raw[i] = val;
		}
	}

	return;
}

/*****************************************************************

*****************************************************************/

void sMav::setShift(int n)  {

	shiftlen = n;
	if (shiftlen<0)  {
		shiftlen = 0;
	}
	else if (shiftlen>len-1)  {
		shiftlen = len-1;
	}

	return;
}

//}		// extern "C" {
