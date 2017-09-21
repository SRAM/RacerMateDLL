#define STRICT
#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4996)					// for vista strncpy_s

#include <windows.h>
#include <windowsx.h>

#include <comutils.h>
#include <utils.h>
#include <scaler.h>
#include <math.h>

#include "axis.h"

/**********************************************************************
	to do:
		add default parameter to allow manual overried of axis scale.
**********************************************************************/

Axis::Axis(HDC _hdc, RECT _waverect, RECT _fullrect, double _dmin, double _dmax, int _maxticks, COLORREF _labelColor, const char *_label, int _orientation, COLORREF _scaleColor)  {
	hdc = _hdc;
	CopyRect(&waverect, &_waverect);
	CopyRect(&fullrect, &_fullrect);
	dmin = _dmin;
	dmax = _dmax;
	maxticks = _maxticks;
	scaleColor = _scaleColor;
	labelColor = _labelColor;
	orientation = _orientation;
	label = _label;
	//manual_scale = false;

/*
	imax = (int)_fmax;
	if (_fmax==(float)imax)  {
	}
	else  {
		imax += 1;
	}

	imin = (int)_fmin;
	if (_fmin==(float)imin)  {
	}
	else  {
		imin -= 1;
	}
*/
	/*
	if (_printer)  {
		rpmpen = CreatePen(PS_SOLID, _lw, RGB(0,0,0));
	}
	else  {
		rpmpen = CreatePen(PS_SOLID, _lw, RPMCOLOR);
	}
	*/

	pen = CreatePen(PS_SOLID, 0, scaleColor);
	SelectPen(hdc, pen);

	gridcolor = RGB(60, 60, 60);
	gridpen = CreatePen(PS_SOLID, 0, gridcolor );

	MaxIntervalScale(
		dmin,					// data minimum, input
		dmax,					// data maximum, input (suggestion)
		//20,					// 20 max number of possible intervals (major ticks), input
		maxticks,
		&scalemin,			// scale minimum, output
		&scalemax,			// scale maximum, output
		&actual);			// actual number of intervals used, output

	//grid = true;

	HDC hdc2 = GetWindowDC(NULL);
	screenw = GetDeviceCaps(hdc2, HORZRES);
	screenh = GetDeviceCaps(hdc2, VERTRES);
	ReleaseDC(NULL, hdc2);

	tickw = (int) (.5 + .010f * screenh);
	tickh = (int) (.5 + .013f * screenh);


	return;
}

/**********************************************************************

**********************************************************************/

Axis::Axis(HDC _hdc, RECT _waverect, double _dmin, double _dmax, int _maxticks, COLORREF _labelColor, const char *_label, int _orientation, COLORREF _scaleColor)  {
	hdc = _hdc;
	CopyRect(&waverect, &_waverect);
	dmin = _dmin;
	dmax = _dmax;
	maxticks = _maxticks;
	scaleColor = _scaleColor;
	labelColor = _labelColor;
	orientation = _orientation;
	label = _label;

/*
	imax = (int)_fmax;
	if (_fmax==(float)imax)  {
	}
	else  {
		imax += 1;
	}

	imin = (int)_fmin;
	if (_fmin==(float)imin)  {
	}
	else  {
		imin -= 1;
	}
*/
	/*
	if (_printer)  {
		rpmpen = CreatePen(PS_SOLID, _lw, RGB(0,0,0));
	}
	else  {
		rpmpen = CreatePen(PS_SOLID, _lw, RPMCOLOR);
	}
	*/

	//manual_scale = false;
	pen = CreatePen(PS_SOLID, 0, scaleColor);
	SelectPen(hdc, pen);

	gridpen = CreatePen(PS_SOLID, 0, RGB(60, 60, 60) );

	//tlm20051112+++
	MaxIntervalScale(
		dmin,					// data minimum, input
		dmax,					// data maximum, input (suggestion)
		//20,					// 20 max number of possible intervals (major ticks), input
		maxticks,
		&scalemin,			// scale minimum, output
		&scalemax,			// scale maximum, output
		&actual);			// actual number of intervals used, output

	if (fabs(scalemax-dmax) > .001)  {
		scalemin = dmin;
		scalemax = dmax;
		actual = maxticks;
	}

	//tlm20051112---

	grid = true;

	int screenw, screenh;
	HDC hdc = GetWindowDC(NULL);
	screenw = GetDeviceCaps(hdc, HORZRES);
	screenh = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(NULL, hdc);

	tickw = (int) (.5 + .010f * screenh);
	tickh = (int) (.5 + .013f * screenh);

	return;
}


/**********************************************************************

**********************************************************************/

Axis::~Axis()  {
	destroy();
}

/**********************************************************************

**********************************************************************/

/*
void Axis::setscalemin(float _min)  {
	manual_scale = true;
	scalemin = _min;
	return;
}
*/

/**********************************************************************

**********************************************************************/

/*
void Axis::setscalemax(float _max)  {
	manual_scale = true;
	scalemax = _max;
	return;
}
*/

/**********************************************************************

**********************************************************************/

void Axis::destroy(void)  {
	DeleteObject(pen);
	DeleteObject(gridpen);
	return;
}

/**********************************************************************

**********************************************************************/

void Axis::draw(bool _printer, bool _grid, bool _integer_only, bool _normalAxis, bool _force, int _precision)  {
	int i, x, y;
	#define WRL	.075f
	#define WRR	.900f
	#define WRT	.050f
	#define WRB	.800f
	SIZE sz;
	float m, b;
	//double d, xd;
	float d, xd;
	int bp;
	char format[16];

	bp = 0;

	SetBkMode(hdc, TRANSPARENT);

	if (!_force)  {
		MaxIntervalScale(
				dmin,					// data minimum, input
				dmax,					// data maximum, input (suggestion)
				maxticks,			// 20 max number of possibel intervals (major ticks), input
				&scalemin,			// scale minimum, output
				&scalemax,			// scale maximum, output
				&actual);			// actual number of intervals used, output
	}
	else  {
		scalemin = dmin;
		scalemax = dmax;
		actual = maxticks;
	}

	if (orientation==0)  {				// x axis
		map((float)scalemin, (float)scalemax, (float)waverect.left, (float)waverect.right, &m, &b);
	}
	else  {								// y axis
		map((float)scalemin, (float)scalemax, (float)waverect.bottom, (float)waverect.top, &m, &b);
	}

	d = (float) ((scalemax-scalemin) / actual);


	SelectPen(hdc, pen);

	if (_integer_only)  {
		strcpy(format, "%.0lf");
	}
	else  {
		if (scalemax < 1.0)  {
			strcpy(format, "%.2lf");
		}
		else if (scalemax <= 10.0)  {
			//strcpy(format, "%.2lf");
			sprintf(format, "%%.%dlf", _precision);
		}
		else if (scalemax <= 20.0)  {
			strcpy(format, "%.2lf");
		}
		else  {
			strcpy(format, "%.0lf");
		}
	}



	if (orientation==0)  {				// x axis
		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, scaleColor );
		}

		for (xd=(float)scalemin; xd<=(scalemax+.0001f); xd+=d)  {

			if (_integer_only)  {
				if (fmod((double)xd, 1.0)>.000001)  {
					continue;
				}
			}

			x = (int) (.5 + (m * xd + b) );
			y = waverect.bottom + tickh;
			MoveToEx(hdc, x, y, NULL);
			y = waverect.bottom;
			SelectPen(hdc, pen);
			LineTo(hdc, x, y);

			if (_grid)  {
				y = waverect.top;
				SelectPen(hdc, gridpen);
				LineTo(hdc, x, y);

			}

			sprintf(gstr, format, xd);
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			y = waverect.bottom+tickh;
			y -= sz.cy/2;
			x -= (int) (.5 + 1.2*sz.cx);
			TextOut(hdc, x, y, gstr, strlen(gstr));

		}

		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, labelColor );
		}

		int xc = (int) (.5 + waverect.left + (waverect.right - waverect.left)/2);
		y = waverect.bottom + 30;
		GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		x = xc - sz.cx/2;
		TextOut(hdc, x, y, label, strlen(label));

	}
	else  {									// y axis
		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, scaleColor );
		}
		for (xd=(float)scalemin; xd<=((float)scalemax+.00001f); xd+=d)  {
			y = (int) (.5 + (m * xd + b) );
			if (_normalAxis)  {
				x = waverect.left-tickw;
				MoveToEx(hdc, x, y, NULL);
				x = waverect.left;
			}
			else  {
				x = waverect.right;
				MoveToEx(hdc, x, y, NULL);
				x = waverect.right + tickw;
			}
			SelectPen(hdc, pen);
			LineTo(hdc, x, y);

			if (_grid)  {
				x = waverect.right;
				SelectPen(hdc, gridpen);
				LineTo(hdc, x, y);
			}

			sprintf(gstr, format, xd);
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			y -= sz.cy/2;
			if (_normalAxis)  {
				x = (int) (.5 + (waverect.left-tickw) -  1.2*sz.cx);
			}
			else  {
				x = (int) (.5 + (waverect.right+tickw) + 10);
			}
			TextOut(hdc, x, y, gstr, strlen(gstr));
		}

		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, labelColor );
		}

		gstr[1] = 0;
		int yc = (int) (.5 + waverect.top + .5 * (waverect.bottom-waverect.top));					// center point of wave rect

		if (_normalAxis)  {
			x = 8;
		}
		else  {
			x = fullrect.right - 20;
		}


		// add up how tall the y axis label is:

		int ylen = 0;
		for(i=0; i<(int)strlen(label); i++)  {
			gstr[0] = label[i];
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			ylen += sz.cy;
		}

		y = yc - ylen/2;

		for(i=0; i<(int)strlen(label); i++)  {
			gstr[0] = label[i];
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			TextOut(hdc, x, y, gstr, strlen(gstr));
			y += sz.cy;
		}
	}

	return;
}

/**********************************************************************
	used by multirider
**********************************************************************/

void Axis::draw(bool _printer)  {
	int i, x, y;
	#define WRL	.075f
	#define WRR	.900f
	#define WRT	.050f
	#define WRB	.800f
	SIZE sz;
	float m, b;
	//double d, xd;
	float d, xd;
	int bp;
	char format[16];

	bp = 0;

	//tlm20051112+++
	/*
	MaxIntervalScale(
			fmin,					// info->minrpm data minimum, input
			fmax,					// data maximum, input (suggestion)
			maxticks,			// 20 max number of possibel intervals (major ticks), input
			&scalemin,			// scale minimum, output
			&scalemax,			// scale maximum, output
			&actual);			// actual number of intervals used, output
	*/
	//tlm20051112---

	if (orientation==0)  {
		map((float)scalemin, (float)scalemax, (float)waverect.left, (float)waverect.right, &m, &b);
	}
	else  {
		map((float)scalemin, (float)scalemax, (float)waverect.bottom, (float)waverect.top, &m, &b);
	}

	d = (float) ((scalemax-scalemin) / actual);


	SelectPen(hdc, pen);

	if (scalemax < 1.0)  {
		strcpy(format, "%.2lf");
	}
	else if (scalemax < 10.0)  {
		strcpy(format, "%.2lf");
	}
	else  {
		strcpy(format, "%.0lf");
	}


	if (orientation==0)  {				// x axis
		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, scaleColor );
		}
		for (xd=(float)scalemin; xd<=((float)scalemax+.000001f); xd+=d)  {
			x = (int) (.5 + (m * xd + b) );
			y = waverect.bottom + tickh;
			MoveToEx(hdc, x, y, NULL);
			y = waverect.bottom;
			SelectPen(hdc, pen);
			LineTo(hdc, x, y);

			if (grid)  {
				y = waverect.top;
				SelectPen(hdc, gridpen);
				LineTo(hdc, x, y);

			}

			sprintf(gstr, format, xd);
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			y = waverect.bottom+tickh;
			y -= sz.cy/2;
			x -= (int) (.5 + 1.2*sz.cx);
			TextOut(hdc, x, y, gstr, strlen(gstr));

		}

		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, labelColor );
		}

		int xc = (int) (.5 + waverect.left + (waverect.right - waverect.left)/2);
		y = waverect.bottom + 30;
		GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		x = xc - sz.cx/2;
		TextOut(hdc, x, y, label, strlen(label));

	}
	else  {									// y axis
		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, scaleColor );
		}
		for (xd=(float)scalemin; xd<=scalemax; xd+=d)  {
			y = (int) (.5 + (m * xd + b) );
			x = waverect.left-tickw;
			MoveToEx(hdc, x, y, NULL);
			x = waverect.left;
			SelectPen(hdc, pen);
			LineTo(hdc, x, y);

			if (grid)  {
				x = waverect.right;
				SelectPen(hdc, gridpen);
				LineTo(hdc, x, y);
			}

			sprintf(gstr, format, xd);
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			y -= sz.cy/2;
			x = (int) (.5 + (waverect.left-tickw) -  1.2*sz.cx);
			TextOut(hdc, x, y, gstr, strlen(gstr));
		}

		if (_printer)  {
			SetTextColor(hdc, RGB(0,0,0));
		}
		else  {
			SetTextColor(hdc, labelColor );
		}

		gstr[1] = 0;
		int yc = (int) (.5 + waverect.top + .5 * (waverect.bottom-waverect.top));					// center point of wave rect
		x = 8;


		// add up how tall the y axis label is:

		int ylen = 0;
		for(i=0; i<(int)strlen(label); i++)  {
			gstr[0] = label[i];
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			ylen += sz.cy;
		}

		y = yc - ylen/2;

		for(i=0; i<(int)strlen(label); i++)  {
			gstr[0] = label[i];
			GetTextExtentPoint32(hdc, gstr, strlen(gstr), &sz);
			TextOut(hdc, x, y, gstr, strlen(gstr));
			y += sz.cy;
		}
	}

	return;
}
