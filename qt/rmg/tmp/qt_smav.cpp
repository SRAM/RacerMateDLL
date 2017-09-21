
#include "commdefs.h"
#include "qt_smav.h"

#include <math.h>
namespace MYQT  {

/**************************************************************************
	simple moving average class
**************************************************************************/

qt_sMav::qt_sMav(int n, bool _dosigma)  {

#ifdef _DEBUG
	bp = 0;
	count = 0L;
#endif

	len = 0;
	raw = NULL;
	fval = NULL;

	dosigma = _dosigma;
	sigma = 0.0;

	if (n==0)  {
		return;
	}

	curval = 0.0f;
	len = n;
	raw = new float[(unsigned int)n];
	fval = new float[(unsigned int)n];

	if (dosigma)  {
		squares = new double[(unsigned int)n];
	}
	else  {
		squares = NULL;
	}
	shiftlen = 0;
	reset();

}

/**************************************************************************

**************************************************************************/

qt_sMav::~qt_sMav(void)  {
	delete [] raw;
	raw = nullptr;
	delete [] fval;
	fval = nullptr;
	delete [] squares;
	squares = nullptr;

}

/**************************************************************************

**************************************************************************/

float qt_sMav::compute(float input)  {
	float ftemp;
	double d;

	sum -= raw[k];			// take out the oldest
	raw[k] = input;			// replace oldest with newest;
	sum += input;
	ftemp = sum / (float)len;
	fval[k] = ftemp;
	curval = ftemp;

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

	int i = k;
	k = (k+1) % len;
	return fval[i];
}

/*****************************************************************
	returns the oldest value in the array
*****************************************************************/

float qt_sMav::first(void)  {
	return raw[k];
}

/*****************************************************************
	returns the newest value in the array
*****************************************************************/

float qt_sMav::last(void)  {
	int i;
	i = k;
	i--;
	if (i<0) i = len-1;
	return raw[i];
}

/*****************************************************************

*****************************************************************/

void qt_sMav::reset(float val)  {
	int i;

	for(i=0; i<len; i++)  {
		raw[i] = 0.0f;
		fval[i] = 0.0f;
			if (squares)  {
				squares[i] = 0.0;
			}
	}

	k = 0;
	//sum = 0.0f;
	sum = len*val;

	squares_sum = 0.0;
	//squares_sum = len * val;

	if (raw)  {
		for(int i=0;i<len;i++)  {
			raw[i] = val;
		}
	}

	return;
}

/*****************************************************************

*****************************************************************/

void qt_sMav::setShift(int n)  {

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
}

