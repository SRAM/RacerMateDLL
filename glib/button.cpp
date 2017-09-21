
#pragma warning(disable:4996)					// for vista strncpy_s

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windowsx.h>

#include <glib_defines.h>
#include <err.h>
#include <fatalerror.h>
#include <button.h>


/****************************************************************************
	button1 = new Button(hwnd,  "Retrieve Data",	.32f, .38f, .13f, .06f, BUTTON1, Button::BUTTON_REGULAR, true );

****************************************************************************/


Button::Button(
			   HWND _phwnd, 
				char *_title,
				float _xOffsetPercentage,
				float _yOffsetPercentage,
				float _xPercentage,			// width in percentage of client area
				float _yPercentage,			// heigth in percentage of client area
				int _id,
				int _type,
				bool _ownerdraw
				)
		{

	strncpy(title, _title, sizeof(title)-1);
	ownerdraw = _ownerdraw;

#ifdef _DEBUG
	if (ownerdraw)  {
		bp = 1;
	}
#endif

	hinstance = GetModuleHandle(NULL);

	init(_phwnd, _xOffsetPercentage, _yOffsetPercentage, _xPercentage,	_yPercentage, _id, _type);

}


/****************************************************************************

****************************************************************************/

Button::~Button()  {

	DeleteObject(font);
	font = NULL;

	ReleaseDC(hwnd, hdc);

	if (bgbrush)  {
		DeleteObject(bgbrush);
		bgbrush = NULL;
	}

	if (framebrush)  {
		DeleteObject(framebrush);
		framebrush = NULL;
	}

	if (pen1)  {
		DeleteObject(pen1);
		pen1 = NULL;
	}
	if (pen2)  {
		DeleteObject(pen2);
		pen2 = NULL;
	}
	if (pen3)  {
		DeleteObject(pen3);
		pen3 = NULL;
	}
	if (pen4)  {
		DeleteObject(pen4);
		pen4 = NULL;
	}

	if (IsWindow(hwnd))  {
		DestroyWindow(hwnd);		// without this logger.exe blew up
	}
}

/****************************************************************************

****************************************************************************/

void Button::resize(void)  {
	computeSize();
	//MoveWindow(phwnd, x, y, w, h, TRUE);
	MoveWindow(hwnd, x, y, w, h, TRUE);
	return;
}

/****************************************************************************

****************************************************************************/

void Button::onMove(void)  {
	return;
}

/****************************************************************************

****************************************************************************/

void Button::computeSize(void)  {

	GetClientRect(phwnd, &pclientRect);

	x = (int) ( .5f + (pclientRect.left + pclientRect.right) * xOffsetPercentage);
	y = (int) ( .5f + (pclientRect.top + pclientRect.bottom) * yOffsetPercentage);

	if (xPercentage>0.001f)  {
		w = (int) ( .5f + (pclientRect.right - pclientRect.left) * xPercentage);
	}

	if (yPercentage>.001f)  {
		h = (int) ( .5f + (pclientRect.bottom - pclientRect.top) * yPercentage);
	}

	//x = (int) ( .5f + (pclientRect.left + pclientRect.right) * xOffsetPercentage - w/2);
	//y = (int) ( .5f + (pclientRect.top + pclientRect.bottom) * yOffsetPercentage - h/2);
}


/****************************************************************************

****************************************************************************/

void Button::init(
			   HWND _phwnd, 
				float _xOffsetPercentage,
				float _yOffsetPercentage,
				float _xPercentage,			// width in percentage of client area
				float _yPercentage,			// heigth in percentage of client area
				int _id,
				int _type)  {

	phwnd = _phwnd;
	xOffsetPercentage = _xOffsetPercentage;
	yOffsetPercentage = _yOffsetPercentage;
	xPercentage = _xPercentage;
	yPercentage = _yPercentage;
	id = _id;
	w = 0;
	h = 0;
	hwnd = 0;
	type = _type;
	bp = 0;
	oldproc = 0;
	pw = 1;
	font = NULL;

#ifdef _DEBUG
	if (!strcmp(title, "Set Fueltron Contrast"))  {
		bp = 8;
	}
#endif

	if (ownerdraw)  {
		bgcolor = RGB(51,51,51);
		//framecolor = RGB(199, 199, 199);
		framecolor = RGB(255, 0, 0);
		bgbrush = CreateSolidBrush(bgcolor);
		textcolor = RGB(199, 199, 199);
		framebrush = CreateSolidBrush(framecolor);

		compute_colors();
	}
	else  {
		bgcolor = RGB(0,0,0);
		framecolor = RGB(255, 255, 255);
		bgbrush = NULL;
		textcolor = RGB(128, 128, 128);
		framebrush = NULL;
		color1 = RGB(0,0,0);
		color2 = RGB(0,0,0);
		color3 = RGB(0,0,0);
		color4 = RGB(0,0,0);
		pen1 = NULL;
		pen2 = NULL;
		pen3 = NULL;
		pen4 = NULL;
		bgbrush = CreateSolidBrush(bgcolor);
	}

	computeSize();			// computes x, y, w, h


	if (type==BUTTON_REGULAR)  {

		if (ownerdraw)  {
			hwnd = CreateWindow(
						"button",
						title,
						WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
						x, 
						y, 
						w, 
						h,
						phwnd, 
						(HMENU) id, 
						hinstance, 
						NULL) ;
		}
		else  {
			hwnd = CreateWindow(
						"button",
						title,
						//WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
						WS_CHILD | WS_VISIBLE | BS_MULTILINE,
						x, 
						y, 
						w, 
						h,
						phwnd, 
						(HMENU) id, 
						hinstance, 
						NULL) ;
		}

	}
	else if (type==BUTTON_RADIO)  {
		ownerdraw = false;
		/*
BS_AUTORADIOBUTTON 	Creates a radio button that, when selected by a user, clears all other buttons in the same group.
BS_LEFT 	Left-aligns the text in the button rectangle on the right side of the check box.
BS_RADIOBUTTON 	Creates a small circle with a label displayed to the right of it. To display the text to the left of the circle, combine this flag with the BS_RIGHTBUTTON style.
BS_RIGHT 	Right-aligns the text in the button rectangle on the right side of the check box.
BS_RIGHTBUTTON 	Positions a check box square on the right side of the button rectangle.
WS_TABSTOP
		*/
		hwnd = CreateWindow(
					"button",
					title,
					//WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
					WS_CHILD | 
					WS_VISIBLE | 
					//BS_RADIOBUTTON
					BS_AUTORADIOBUTTON |
					BS_LEFT
					//BS_AUTOCHECKBOX
					,
					x, 
					y, 
					w, 
					h,
					phwnd, 
					(HMENU) id, 
					hinstance, 
					NULL) ;
	}
	else if (type==BUTTON_CHECKBOX)  {
		ownerdraw = false;
		hwnd = CreateWindow(
					"button",
					title,
					WS_CHILD | 
					WS_VISIBLE | 
					BS_CHECKBOX | BS_LEFTTEXT
					,
					x, 
					y, 
					w, 
					h,
					phwnd, 
					(HMENU) id, 
					hinstance, 
					NULL) ;
	}


	hdc = GetDC(hwnd);

	bool flag = false;
	SIZE sz;
	BOOL b;

#ifdef _DEBUG
	if (!strcmp(title, "Set Fueltron Contrast"))  {
		bp = 8;
	}
#endif

	if (w==0)  {
		GetTextExtentPoint32(hdc, title, strlen(title), &sz);
		w = sz.cx;
		flag = true;
	}
	if (h==0)  {
		GetTextExtentPoint32(hdc, title, strlen(title), &sz);
		h = sz.cy;
		flag = true;
	}

	if (flag)  {
		b = MoveWindow(hwnd, x, y, w, h, TRUE);
		if (!b)  {
			#ifdef _DEBUG
				Err *err = new Err();
				DEL(err);
			#endif
		}
	}

#ifdef _DEBUG
	if (!strcmp(title, "Set Fueltron Contrast"))  {
		bp = 8;
	}
#endif

	//GetTextExtentPoint32(hdc, title, strlen(title), &sz);

	//if (ownerdraw)  {
	if (1)  {
		int k;

		for(k=36; k>4; k--)  {
			font = CreateFont(
							-k,								// height of font
							0,									// average character width
							0,									// angle of escapement
							0,									// base-line orientation angle
							100,								// font weight
							FALSE,								// italic attribute option
							0,									// underline attribute option
							0,									// strikeout attribute option
							ANSI_CHARSET,					// character set identifier
							OUT_STRING_PRECIS,			// output precision
							CLIP_TT_ALWAYS,				// clipping precision
							PROOF_QUALITY,				// output quality
							//ANTIALIASED_QUALITY,
							//NONANTIALIASED_QUALITY,
							FIXED_PITCH | FF_SWISS,		// pitch and family
							"Arial"							// typeface name
			);
			SelectFont(hdc, font);
			GetTextExtentPoint32(hdc, title, strlen(title), &sz);

			if (sz.cx >= ROUND(.9*w))  {
				DeleteObject(font);
				font = NULL;
				continue;
			}

			if (sz.cy >= ROUND(.9*h))  {
				DeleteObject(font);
				font = NULL;
				continue;
			}

			break;

		}					// for(k=36; k>7; k--)  {
		bp = 7;
	}

	if (font)  {
		SendMessage(hwnd, WM_SETFONT, (WPARAM)font, FALSE);
	}

	SetWindowLong (hwnd, GWL_USERDATA, (long)this);
	oldproc = (WNDPROC) SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM) proc);		// subclass to intercept messages
	return;

}

/**********************************************************************

**********************************************************************/

LRESULT CALLBACK Button::proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)  {
	int bp=0;

	Button *button = (Button *)GetWindowLong(hwnd, GWL_USERDATA);
	if (button==NULL && msg!=WM_CREATE)  {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	switch(msg)  {
		case WM_KEYDOWN:  {
			if (button->keydown)  {
				break;
			}
			button->keydown = true;
			switch(wParam)  {
				case VK_TAB:  {
					bp = 1;
					break;
				}
				default:
					break;
			}
			break;
		}
		case WM_KEYUP:  {
			button->keydown = false;
			break;
		}

		/****************************************************************
		
			handle ownerdraw buttons here:

			Normally, you can have an "ownerdraw" button with
			BS_OWNERDRAW style, drawing whatever you want
			at WM_DRAWITEM. But that only works for normal
			buttons. There are no ownerdraw radio or check buttons.
			You'll need a custom control for this.

		****************************************************************/

#if 0
		case WM_DRAWITEM:  {
			DRAWITEMSTRUCT* di;
			di = (DRAWITEMSTRUCT*)lParam;
			bp = 1;

			/*
typedef struct tagDRAWITEMSTRUCT {
  UINT      CtlType; 
  UINT      CtlID; 
  UINT      itemID; 
  UINT      itemAction; 
  UINT      itemState; 
  HWND      hwndItem; 
  HDC       hDC; 
  RECT      rcItem; 
  ULONG_PTR itemData; 
} DRAWITEMSTRUCT;
			*/	

			switch(di->CtlType)  {
				case ODT_BUTTON:						// Owner-drawn button 
					bp = 2;
					break;

				/*
				case ODT_COMBOBOX:					// Owner-drawn combo box 
					bp = 2;
					break;
				case ODT_LISTBOX:						// Owner-drawn list box 
					bp = 2;
					break;
//				case ODT_LISTVIEW:					// List view control 
//					bp = 2;
//					break;
				case ODT_MENU:							// Owner-drawn menu item 
					bp = 2;
					break;
				case ODT_STATIC:						// Owner-drawn static control 
					bp = 2;
					break;
//				case ODT_TAB:
//					bp = 2;
//					break;
				*/

				default:
					throw (fatalError(__FILE__, __LINE__));
			}

			/*
			switch(di->CtlID)  {
				default:
					throw (fatalError(__FILE__, __LINE__));
			}
			*/
			int cx, cy;

			FillRect(di->hDC, &di->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH)); FrameRect(di->hDC, &di->rcItem, (HBRUSH) GetStockObject (BLACK_BRUSH));
			cx = di->rcItem.right - di->rcItem.left;
			cy = di->rcItem.bottom - di->rcItem.top;
			POINT pt[3];
			pt[0].x = 3 * cx / 8; pt[0].y = 1 * cy / 8;
			pt[1].x = 5 * cx / 8; pt[1].y = 1 * cy / 8;
			pt[2].x = 4 * cx / 8; pt[2].y = 3 * cy / 8;
			SelectObject(di->hDC, GetStockObject(BLACK_BRUSH));
			Polygon(di->hDC, pt, 3);
			if (di->itemState & ODS_SELECTED)  {
				InvertRect(di->hDC, &di->rcItem);
			}

			break;
		}
#endif



		case WM_CTLCOLORSTATIC:  {
			bp = 3;
			break;
		}

		case WM_CTLCOLORLISTBOX:
			bp = 3;
			break;

		case WM_CTLCOLOREDIT:
			bp = 3;
			break;

		case WM_CTLCOLORMSGBOX:
			bp = 3;
			break;

		case WM_CTLCOLORBTN:
			bp = 3;
			break;

		case WM_CTLCOLORDLG:
			bp = 3;
			break;

		case WM_CTLCOLORSCROLLBAR:
			bp = 3;
			break;

		case WM_NOTIFY:
			bp = 3;
			break;

		default:
			break;
	}
          
	return CallWindowProc(button->oldproc, hwnd, msg, wParam, lParam) ;

}


/**********************************************************************
	future non-rect constructor
**********************************************************************/

void Button::OnCreate(void)  {		// temporary

	//-------------------------------------------
	// load the bitmaps from a resource or file
	//-------------------------------------------

//	hGlobalBit = (HBITMAP)LoadImage(GetModuleHandle(NULL), "test.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
//	if (hGlobalBit == NULL)  {
//		hGlobalBit = (HBITMAP)LoadImage(NULL, "test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
//	}
//	if (hGlobalBit == NULL)  {
//		throw("button couldn't load up bitmap");
//	}

	
	//width = GetSystemMetrics(SM_CXSCREEN);

	SetWindowPos(
		hwnd,
		HWND_TOPMOST,
		0,0, 170, 226, 0);
	
	RECT rectDlg;
	GetWindowRect(hwnd, &rectDlg);

	/*
	hRgn = CreateEllipticRgn(
		rectDlg.left + 36,
		rectDlg.top + 13,
		rectDlg.right - 43,
		rectDlg.bottom - 109);

	if (!SetWindowRgn(hwnd, hRgn, TRUE))  {
		throw("SetWindowRgn Failed");
	}
	*/

}

/**********************************************************************
	future non-rect draw():
**********************************************************************/

/*
void Button::OnPaint(void)  {
	PAINTSTRUCT paintstruct;

	HDC hdc = BeginPaint(hwnd, &paintstruct);
	HDC memDC = CreateCompatibleDC(hdc);

	SelectObject(memDC, hGlobalBit);
	BitBlt(hdc, 0, 0, 170, 226, memDC, 0, 0, SRCCOPY);
	EndPaint(hwnd, &paintstruct);
	DeleteDC(memDC);

	return;
}
*/

/**********************************************************************

**********************************************************************/

bool Button::getchecked(void)  {
	LRESULT lr;

	lr = SendMessage(hwnd, BM_GETCHECK, 0, 0);

	switch(lr)  {
		case BST_CHECKED:					// Button is checked. 
			return true;
		case BST_INDETERMINATE:			//  Button is grayed, indicating an indeterminate state (applies only if the button has the BS_3STATE or BS_AUTO3STATE style). 
			return false;
		case BST_UNCHECKED:				// Button is cleared 
			return false;
		default:
			return false;
	}

	return false;
}


/**********************************************************************

**********************************************************************/

void Button::setchecked(bool _b)  {
	WPARAM wparam;

	if (_b)  {
		wparam = BST_CHECKED;
	}
	else  {
		//BST_INDETERMINATE Sets the button state to grayed, indicating an indeterminate state. Use this value only if the button has the BS_3STATE or BS_AUTO3STATE style. 
		wparam = BST_UNCHECKED;
	}

	SendMessage(hwnd, BM_SETCHECK, wparam, 0);

	return;
}

/****************************************************************************

****************************************************************************/

void Button::setLabel(const char *_label)  {
	SetWindowText(hwnd, _label);
	return;
}

/****************************************************************************

****************************************************************************/

void Button::set_bg_color(COLORREF color)  {
	bgcolor = color;
	if (bgbrush)  {
		DeleteObject(bgbrush);
	}
	bgbrush = CreateSolidBrush(bgcolor);

	compute_colors();

	return;
}

/****************************************************************************

****************************************************************************/

void Button::set_text_color(COLORREF color)  {
	textcolor = color;
	return;
}

/****************************************************************************

****************************************************************************/

void Button::set_fg_color(COLORREF color)  {
	framecolor = color;
	if (framebrush)  {
		DeleteObject(framebrush);
	}
	framebrush = CreateSolidBrush(framecolor);

	//compute_colors();

	return;
}

/****************************************************************************

****************************************************************************/

void Button::compute_colors(void)  {
	int r,g,b;
	int r1, r2, g1, g2, b1, b2;

#if 1
	color1 = RGB(255,255,255);				// the light color
	color2 = RGB(0,0,0);						// the dark color
#else
	color1 = RGB(255,0,0);
	color2 = RGB(0,255,0);
#endif

	//------------------------------------------------
	// color3 goes 1/2 way toward the light color
	//------------------------------------------------

	r1 = GetRValue(bgcolor);
	r2 = GetRValue(color1);
	r = ROUND( (r1 + r2) / 2.0);

	g1 = GetGValue(bgcolor);
	g2 = GetGValue(color1);
	g = ROUND( (g1 + g2) / 2.0);

	b1 = GetBValue(bgcolor);
	b2 = GetBValue(color1);
	b = ROUND( (b1 + b2) / 2.0);

	color3 = RGB(r, g, b);

	//------------------------------------------------
	// color4 goes 1/2 way toward the dark color
	//------------------------------------------------

	r1 = GetRValue(bgcolor);
	r2 = GetRValue(color2);
	r = ROUND( (r1 + r2) / 2.0);

	g1 = GetGValue(bgcolor);
	g2 = GetGValue(color2);
	g = ROUND( (g1 + g2) / 2.0);

	b1 = GetBValue(bgcolor);
	b2 = GetBValue(color2);
	b = ROUND( (b1 + b2) / 2.0);

	color4 = RGB(r,g,b);



	if (pen1)  {
		DeleteObject(pen1);
		pen1 = NULL;
	}
	if (pen2)  {
		DeleteObject(pen2);
		pen2 = NULL;
	}
	if (pen3)  {
		DeleteObject(pen3);
		pen3 = NULL;
	}
	if (pen4)  {
		DeleteObject(pen4);
		pen4 = NULL;
	}

	pen1 = CreatePen(PS_SOLID, pw, color1 );
	pen2 = CreatePen(PS_SOLID, pw, color2 );
	pen3 = CreatePen(PS_SOLID, pw, color3 );
	pen4 = CreatePen(PS_SOLID, pw, color4 );

	return;
}


/**********************************************************************
	wParam		Handle to the device context for the static control window. 
	lParam		Handle to the button
**********************************************************************/

LRESULT Button::wmctlcolorbtn(WPARAM wparam, LPARAM lparam)  {
//	LRESULT lr = 0;

	SetTextColor((HDC)wparam, textcolor);
	SetBkColor((HDC)wparam, bgcolor);

	return((LRESULT)bgbrush);

}

/****************************************************************************

****************************************************************************/

LRESULT Button::drawitem(WPARAM wparam, LPARAM lparam)  {
	DRAWITEMSTRUCT* dis;
	dis = (DRAWITEMSTRUCT*)lparam;

#if 1
	SIZE sz;
	int ix, iy;
	HDC hdc;
	RECT r;

	hdc = dis->hDC;
	CopyRect(&r, &dis->rcItem);

	FillRect(hdc, &r, (HBRUSH) bgbrush);
	//FrameRect(hdc, &r, (HBRUSH) framebrush);

	SelectPen(hdc, pen1);
	MoveToEx(hdc, r.left, r.bottom, NULL);
	LineTo(hdc, r.left,  r.top);
	LineTo(hdc, r.right, r.top);

	SelectPen(hdc, pen2);
	MoveToEx(hdc, r.right, r.top, NULL);
	LineTo(hdc, r.right, r.bottom);
	LineTo(hdc, r.left, r.bottom);

	SelectPen(hdc, pen3);
	MoveToEx(hdc, r.left+pw, r.bottom-pw, NULL);
	LineTo(hdc, r.left+pw, r.top+pw);
	LineTo(hdc, r.right-pw, r.top+pw);

	SelectPen(hdc, pen4);
	MoveToEx(hdc, r.right-pw, r.top+pw, NULL);
	LineTo(hdc, r.right-pw, r.bottom-pw);
	LineTo(hdc, r.left+pw, r.bottom-pw);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textcolor);
	SelectFont(hdc, font);

	GetTextExtentPoint32(hdc, title, strlen(title), &sz);

	ix = ROUND( .5*(r.right - sz.cx) );
	iy = ROUND( .5*(r.bottom - sz.cy) );

	TextOut(hdc, ix, iy, title, strlen(title));
#else
	int cx, cy;
	POINT pt[3];

	FillRect(dis->hDC, &dis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH)); FrameRect(dis->hDC, &dis->rcItem, (HBRUSH) GetStockObject (BLACK_BRUSH));
	cx = dis->rcItem.right - dis->rcItem.left;
	cy = dis->rcItem.bottom - dis->rcItem.top;
	pt[0].x = 3 * cx / 8; pt[0].y = 1 * cy / 8;
	pt[1].x = 5 * cx / 8; pt[1].y = 1 * cy / 8;
	pt[2].x = 4 * cx / 8; pt[2].y = 3 * cy / 8;
	SelectObject(dis->hDC, GetStockObject(BLACK_BRUSH));
	Polygon(dis->hDC, pt, 3);
	if (dis->itemState & ODS_SELECTED)  {
		InvertRect(dis->hDC, &dis->rcItem);
	}
#endif

	return 0L;
}

