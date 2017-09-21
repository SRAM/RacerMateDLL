
#ifndef _AXIS_H_
#define _AXIS_H_

#ifdef WIN32
	#include <windows.h>
#else
	#include <defines.h>
#endif


class Axis  {

	public:
#ifdef WIN32
		Axis(
			HDC _hdc, 
			RECT _waverect,								// the frame that the waveform is drawn in
			RECT _fullrect,
			//float _fmin, 
			//float _fmax,
			double _dmin, 
			double _dmax,
			int _maxticks,
			COLORREF _labelColor,
			const char *_label,
			int _orientation,
			COLORREF _scaleColor
			);

		Axis(
			HDC _hdc, 
			RECT _waverect, 
			double _dmin, 
			double _dmax,
			int _maxticks,
			COLORREF _labelColor,
			const char *_label,
			int _orientation,
			COLORREF _scaleColor
			);
#else
		Axis(
			RECT _waverect,								// the frame that the waveform is drawn in
			RECT _fullrect,
			float _fmin,
			float _fmax,
			int _maxticks,
			COLORREF _labelColor,
			const char *_label,
			int _orientation,
			COLORREF _scaleColor
			);

		Axis(
			RECT _waverect,
			float _fmin,
			float _fmax,
			int _maxticks,
			COLORREF _labelColor,
			const char *_label,
			int _orientation,
			COLORREF _scaleColor
			);
#endif

		virtual ~Axis();
		void draw(bool _printer, bool _grid, bool _integer_only, bool _leftyaxis, bool _force=false, int _precision=2);
		void draw(bool _printer);

		//void setscalemin(float _min);
		//void setscalemax(float _max);
		//double getMinScale(void)  {	return scalemin; }
		//double getMaxScale(void)  { return scalemax;}
		double getscalemin(void)  { return scalemin; }
		double getscalemax(void)  { return scalemax; }

		void setGridColor(COLORREF _gridcolor)  {
			gridcolor = _gridcolor;
#ifdef WIN32
			DeleteObject(gridpen);
			gridpen = CreatePen(PS_SOLID, 0, gridcolor );
#endif
			return;
		}

	private:
#ifdef WIN32
		HDC hdc;
		HPEN pen;
		HPEN gridpen;
#endif
		//bool manual_scale;
		COLORREF gridcolor;
		RECT waverect;							// waveform rectangle
		//float fmin, fmax;
		double dmin, dmax;
		//int imin, imax;
		float mx, bx;
		COLORREF scaleColor;
		COLORREF labelColor;
		int orientation;				// 0 = xaxis, 1 = yaxis
		void destroy(void);
		double scalemin;
		double scalemax;
		int actual;
		char gstr[256];
		const char *label;
		bool grid;
		int maxticks;
		int tickw, tickh;
		int screenw, screenh;
		RECT fullrect;							// same as clientrect
};


#endif		// #ifndef _AXIS_H_

