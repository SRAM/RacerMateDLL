#pragma once

#ifdef WIN32
	#ifndef STRICT
		#define STRICT
	#endif
	#include <windows.h>
#endif

/************************************************************************

#include <float2d.h>

	float2d *data;
	data = new float2d(200, 24);

	for(i=0; i<200; i++)  {
		for(j=0; j<24; j++)  {
			f = data->v[i][j];
		}
	}

	DEL(data);

************************************************************************/


#ifdef WIN32
class __declspec(dllexport) float2d  {
#else
class float2d  {
#endif


	protected:

	private:
		int rows;
		int cols;
		float2d(const float2d&);
		float2d &operator = (const float2d&);		// unimplemented

	public:
		float2d(void);
		float2d(int _rows, int _cols);
		virtual ~float2d(void);
		float **v;

};



