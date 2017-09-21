
#ifndef _SCALER_H_
#define _SCALER_H_

#ifdef WIN32
	extern "C" {
		__declspec(dllexport) void MaxIntervalScale(
				double DataMin, 
				double DataMax, 
				int MaxIntervals, 
				double *ScaleMin, 
				double *ScaleMax, 
				int *Actual
			);
	}
#else
	void MaxIntervalScale(double DataMin, double DataMax, int MaxIntervals, double *ScaleMin, double *ScaleMax, int *Actual);
#endif


#endif		// #ifndef _SCALER_H_
