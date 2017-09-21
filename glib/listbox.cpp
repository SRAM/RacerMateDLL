
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <objbase.h>
#include <COMMCTRL.H>

#include <glib_defines.h>
#include <err.h>
#include <utils.h>
#include <fatalerror.h>

#include <listBox.h>


/**********************************************************************

**********************************************************************/

listBox::listBox(HWND _phwnd, float _px, float _py, float _pw, float _ph)  {

	phwnd = _phwnd;
	px = _px;
	py = _py;
	pw = _pw;
	ph = _ph;

	hInstance = GetModuleHandle(NULL);
	count = 0;

	if( FAILED(CoInitialize(NULL)) )  {
		MessageBox( NULL, "CoInitialize", "Error", MB_OK | MB_ICONINFORMATION );
		destroy();
		throw (fatalError(__FILE__, __LINE__));
	}


	GetClientRect(phwnd, &pclientrect);
	GetWindowRect(phwnd, &pwinrect);

	LOGFONT	lf = {
						30,								// 30, height
						20,								// 20, width
						0,								// escapement
						0,								// orientation
						5,								// 1, weight
						TRUE,							// italic flag
						0,								// underline flag
						0,								// strikeout flag
						ANSI_CHARSET,				// char set
						OUT_STRING_PRECIS,		// output precision
						0,								// clip precision
						PROOF_QUALITY,				// output quality
						FIXED_PITCH | FF_SWISS,	// pitch and family
						"Arial"
					};


	hfont = CreateFontIndirect(&lf);

	hbrush = CreateSolidBrush(LBBGCOLOR);

	x = (int) (.5 + px * pclientrect.right);
	y = (int) (.5 + py * pclientrect.bottom);
	w = (int) (.5 + pw * pclientrect.right);
	h = (int) (.5 + ph * pclientrect.bottom);

	hwnd = CreateWindowEx(
			//WS_EX_DLGMODALFRAME,
			//WS_EX_CLIENTEDGE,
			WS_EX_WINDOWEDGE,
			"listbox",
			"",					// title

			WS_VISIBLE | 
			ES_LEFT |
			WS_VSCROLL |
			WS_THICKFRAME |
			//LBS_STANDARD |
			WS_CHILD,

			x, y, w, h,
			phwnd, 
			NULL,
			hInstance, 
			NULL);

	if (hwnd==NULL)  {
		destroy();
		throw (fatalError(__FILE__, __LINE__));
	}

	return;
}


/**********************************************************************

**********************************************************************/

listBox::~listBox()  {
	destroy();
}


/**********************************************************************

**********************************************************************/

void listBox::destroy(void)  {

	if (hbrush != 0)  {
		DeleteObject(hbrush);
		hbrush = 0;
	}

	if (hfont)  {
		DeleteObject(hfont);
		hfont = NULL;
	}

	if (IsWindow(hwnd))  {
		DestroyWindow(hwnd);
		hwnd = NULL;
	}

	return;
}

/**********************************************************************

**********************************************************************/

void listBox::add(char *str)  {
	SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM) str);
	int max = SendMessage(hwnd, LB_GETCOUNT, 0, 0);
	//SendMessage(hwnd, LB_SETCURSEL, max, 0);
	count++;
	SendMessage(hwnd, LB_SETTOPINDEX, max-1, 0);

	return;
}

/**********************************************************************

**********************************************************************/

LRESULT listBox::wmCtlColorListBox(LPARAM lparam, WPARAM wparam)  {
	HDC hdc = (HDC)wparam;

	SetBkMode(hdc,TRANSPARENT );
	SetTextColor(hdc, LBTEXTCOLOR);

	return (LRESULT)hbrush;
}

