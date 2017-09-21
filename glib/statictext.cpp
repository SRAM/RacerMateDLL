#pragma warning(disable:4996)					// for vista strncpy_s

#define STRICT
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
	#include <err.h>
#endif

#include <staticText.h>



/****************************************************************************
	to do: get autoscaling to work!!!
****************************************************************************/

staticText::staticText(
							  HWND _phwnd, 
							  char *_text, 
							  float _px, 
							  float _py, 
							  float _pw, 
							  float _ph, 
							  int _id, 
							  HFONT _hfont, 
							  COLORREF _bgcolor, 
							  COLORREF _textcolor)  {

	strncpy(text, _text, sizeof(text)-1);
	phwnd = _phwnd;
	hinstance = GetModuleHandle(NULL);
	hfont = _hfont;
	bgcolor = _bgcolor;
	textcolor = _textcolor;
	id = _id;
	hbrush = 0;

	px = _px;
	py = _py;
	pw = _pw;
	ph = _ph;

	GetClientRect(phwnd, &pclientRect);

	if (pw < 0.0f || ph < 0.0f || px < 0.0f || py < 0.0f )  {
		autoscale = true;
	}
	else  {
		autoscale = false;
	}

	if (autoscale)  {
		x = 0;
		y = 0;
		w = 10;
		h = 10;
	}
	else  {
		x = (int) ( .5f + (pclientRect.left + pclientRect.right) * px);
		y = (int) ( .5f + (pclientRect.top + pclientRect.bottom) * py);

		w = (int) ( .5f + (pclientRect.right - pclientRect.left) * pw);
		h = (int) ( .5f + (pclientRect.bottom - pclientRect.top) * ph);
	}

	hwnd = CreateWindowEx(
				0,
				//WS_EX_CLIENTEDGE, 
				//WS_EX_OVERLAPPEDWINDOW,
				//WS_EX_STATICEDGE,
				//WS_EX_TRANSPARENT,
				"static",
				text,
            WS_CHILD | WS_VISIBLE,
            x, 
				y, 
				w, 
				h,
            phwnd, 
				(HMENU)id,
				hinstance, 
				NULL) ;


	if (hwnd==NULL)  {
		#ifdef _DEBUG
			Err *err = new Err();
			char *str = err->getString();
			delete err;
			cleanup();
			return;
		#endif
	}

	if (hfont)  {
		SendMessage(hwnd, WM_SETFONT, (WPARAM)hfont, 0);
	}



	if (autoscale)  {
		SIZE sz;
		HDC hdc = GetDC(hwnd);
		GetTextExtentPoint32(hdc, text, strlen(text), &sz);
		ReleaseDC(hwnd, hdc);

		if (px < 0.0f)  {
			x = (int) (.5 + (pclientRect.right / 2) - (sz.cx/2));
		}

		if (py < 0.0f)  {
			y = (int) (.5 + (pclientRect.bottom / 2) - (sz.cy/2));
		}

		if (pw < 0.0f)  {
			w = sz.cx;
		}
		if (ph < 0.0f)  {
			h = sz.cy;
		}

		MoveWindow(hwnd, x, y, w, h, TRUE);
	}


//	SetWindowLong (hwnd, GWL_USERDATA, (long)this);
//	ShowWindow(hwnd, SW_SHOWNORMAL);
	//b = 
	SetWindowText(hwnd, text);

	//bp = 1;
}

/****************************************************************************

****************************************************************************/

staticText::~staticText()  {
	cleanup();
}


/****************************************************************************

****************************************************************************/

void staticText::cleanup(void)  {

	if (hbrush)  {
		DeleteObject(hbrush);
		hbrush = NULL;
	}

	DestroyWindow(hwnd);
	return;

}

/**********************************************************************
	wParam = hdc
	lParam = hwnd
**********************************************************************/

LRESULT staticText::wmctlcolorstatic(LPARAM lParam, WPARAM wParam)  {

	HWND hw = (HWND)lParam;

	if (hw==hwnd)  {
		if (hfont)  {
			SelectObject( (HDC)wParam, hfont);
		}
		SetBkMode((HDC)wParam, OPAQUE);
		SetBkColor((HDC)wParam, bgcolor);
		SetTextColor( (HDC)wParam, textcolor );

		hbrush = CreateSolidBrush(bgcolor);
		return (long)hbrush;
	}

	return FALSE;
}

/****************************************************************************

****************************************************************************/

void staticText::setText(char * _txt)  {

	if (autoscale)  {
		SIZE sz;
		HDC hdc = GetDC(hwnd);
		GetTextExtentPoint32(hdc, text, strlen(text), &sz);
		ReleaseDC(hwnd, hdc);

		if (px < 0.0f)  {
			x = (int) (.5 + (pclientRect.right / 2) - (sz.cx/2));
		}

		if (py < 0.0f)  {
			y = (int) (.5 + (pclientRect.bottom / 2) - (sz.cy/2));
		}

		if (pw < 0.0f)  {
			w = sz.cx;
		}
		if (ph < 0.0f)  {
			h = sz.cy;
		}

		MoveWindow(hwnd, x, y, w, h, TRUE);
	}


	SetWindowText(hwnd, _txt);
//	InvalidateRect(hwnd, NULL, TRUE);
	return;
}

