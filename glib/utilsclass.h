#ifndef _UTILSCLASS_H_
#define _UTILSCLASS_H_
#ifdef WIN32
	#include <windows.h>
#endif

/////////#include <config.h>


	class Utils  {

	public:
		static void mav(float *x,int n,int m,int flag);
		static short map(float x1,float x2,float y1,float y2,float &m,float &b);

#ifdef WIN32
		static void line(HDC hdc, int x1, int y1, int x2, int y2);
		static void GradientFill(
							HDC hdc,
							RECT *rect,
							COLORREF from,
							COLORREF to,
							bool leftright);
#endif
		

};


#endif		// #ifndef _UTILSCLASS_H_

