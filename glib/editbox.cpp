
#define STRICT
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996)					// for vista strncpy_s

#include <windowsx.h>

#include <glib_defines.h>
#include <err.h>
#include <utils.h>
#include <fatalerror.h>
#include <editBox.h>

int editBox::instances = 0;
int editBox::first_font_size = 0;
float	editBox::max_px = -1.0f;

/**********************************************************************
	constructor 1
	the coords are the position if the data window, not the label window.
**********************************************************************/

editBox::editBox(
				HWND _phwnd,
				const char *_thelabel,
				float _px1,
				float _px2,
				float _py,
				float _pw,
				COLORREF _label_fg_color,
				int _id,
				const char *_data,
				int _usermsg
				 )  {

	HDC hdc;
	BOOL b;

	constructor = 1;

	label_fg_color = _label_fg_color;

	phwnd = _phwnd;
	memset(thelabel, 0, sizeof(thelabel));
	if (_thelabel[0] != 0)  {
		strncpy(thelabel, _thelabel, sizeof(thelabel)-1);
	}
	memset(data, 0, sizeof(data));

	if (_data)  {
		strncpy(data, _data, sizeof(data)-1);
	}

	font = NULL;
	hwndData = NULL;
#ifdef _DEBUG
	resizecalls = 0;
#endif
	hInstance = GetModuleHandle(NULL);
	id = _id;
	px1 = _px1;
	px2 = _px2;
	py = _py;
	pw = _pw;

	usermsg = _usermsg;

	GetClientRect(phwnd, &pclientrect);

	resize();

	pen = CreatePen(PS_SOLID, 1, RGB(0,0,0) );
	
	//---------------
	// the data:
	//---------------

	hwndData = CreateWindowEx(
				WS_EX_STATICEDGE,
				"edit",
				NULL,
				WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
				data_x, 
				y,
				//data_w,
				10,
				//h,
				10,
				phwnd,
				(HMENU)id,
				hInstance,
				NULL);

	if (hwndData==NULL)  {
		#ifdef _DEBUG
			Err *err = new Err();
			DEL(err);
		#endif
		destroy();
		throw (fatalError(__FILE__, __LINE__));
	}

	SendMessage(hwndData, WM_SETFONT, (WPARAM)font, TRUE);

#if 1
	//data_w = ROUND(pw * (pclientrect.right - pclientrect.left) );
	hdc = GetDC(phwnd);
		GetTextExtentPoint32(hdc, thelabel, strlen(thelabel), &sz);
		label_w = sz.cx;
		label_h = sz.cy;
	ReleaseDC(phwnd, hdc);
	hdc = 0;

	hdc = GetDC(hwndData);
		SelectFont(hdc, font);
		GetTextExtentPoint32(hdc, data, strlen(data), &sz);
		data_w = sz.cx;
		data_h = sz.cy;
	ReleaseDC(hwndData, hdc);
	hdc = 0;
#else
	hdc = GetDC(hwndData);
		SelectFont(hdc, font);
		char str[64];
		strcpy(str, "Van Kiddlehopperxxxxxx");
		GetTextExtentPoint32(hdc, str, strlen(str), &sz);
	ReleaseDC(hwndData, hdc);
	data_w = sz.cx;
#endif

	float f = (float) (data_x + data_w) / (float)(pclientrect.right - pclientrect.left);
	if (f > max_px)  {
		max_px = f;
	}

	MoveWindow(hwndData, data_x, y, data_w, data_h, FALSE);
	if (data[0] != 0)  {
		setText(data);
	}


	{
#if 0
		int length;             // Length of a line
		int line_count;         // Number of lines in edit box

		// Check if there are more than 128 lines
		if(SendMessage(hwndData, EM_GETLINECOUNT, 0, 0) > 128)  {
			// Yes, delete the first line before we append a new one
			length = SendMessage(hwndData, EM_LINELENGTH, 0, 0);
		   // Select the line of text + /r/n
			SendMessage(hwndData, EM_SETSEL, 0, length + 2);
			// Replace it with nothing
			SendMessage(hwndData, EM_REPLACESEL, FALSE, (long)"");
		}

		// Append the new line to the bottom of the edit control
		// Set current length to all the text
		length=SendMessage(hwndData, WM_GETTEXTLENGTH, 0, 0);

		// Select the space after all the text
		SendMessage(hwndData, EM_SETSEL, length, length);

		// Replace it with the new line
		SendMessage(hwndData, EM_REPLACESEL, FALSE, (long)data);

		// Scroll the edit box down to the bottom
		line_count = SendMessage(hwndData, EM_GETLINECOUNT, 0, 0);
		SendMessage(hwndData, EM_LINESCROLL, 0, line_count); 
#endif
	}


	keydown = false;
	instances++;

	if (instances==1)  {
		first_font_size = font_size;
	}

#ifdef INTERCEPT_LABEL
	if (hwndLabel)  {
		savelabel = GetWindowLong(hwndLabel, GWL_WNDPROC);
		SetWindowLong (hwndLabel, GWL_USERDATA, (long)this);
		oldLabelProc = (WNDPROC) SetWindowLong(hwndLabel, GWL_WNDPROC, (LPARAM) labelproc);		// subclass to intercept messages
	}
#endif

	if (hwndData)  {
		savedata = GetWindowLong(hwndData, GWL_WNDPROC);
		SetWindowLong (hwndData, GWL_USERDATA, (long)this);
		oldEditProc = (WNDPROC) SetWindowLong(hwndData, GWL_WNDPROC, (LPARAM) editproc);		// subclass to intercept messages
	}

	return;

}								// constructor 1

/**********************************************************************************
	constructor 2
	font size is a function of _p_label_start and _p_label_end ( label width )
**********************************************************************************/


editBox::editBox(
				HWND _phwnd,
				const char *_label,
				float _p_y_start,
				float _p_label_start,
				float _p_label_end,
				float _p_data_wid,
				float _dummy,
				COLORREF _label_fg_color,
				int _id,
				COLORREF _label_bg_color,
				const char *_data,
				int _usermsg
				 )  {


	BOOL b;

	constructor = 2;

	label_fg_color = _label_fg_color;
	label_bg_color = _label_bg_color;

	phwnd = _phwnd;
	memset(thelabel, 0, sizeof(thelabel));

	if (_label[0] != 0)  {
		strncpy(thelabel, _label, sizeof(thelabel)-1);
	}

	bgbrush = CreateSolidBrush(label_bg_color);
#ifdef _DEBUG
	resizecalls = 0;
#endif

	memset(data, 0, sizeof(data));

	if (_data)  {
		strncpy(data, _data, sizeof(data)-1);
	}

	//font_size = _font_size;

	p_y_start = _p_y_start;
	p_label_start = _p_label_start;
	p_label_end = _p_label_end;
	p_data_wid = _p_data_wid;

	font = NULL;
#ifdef DOLABELHWND
	hwndLabel = NULL;
#endif
	hwndData = NULL;

	hInstance = GetModuleHandle(NULL);
	id = _id;

	usermsg = _usermsg;

	GetClientRect(phwnd, &pclientrect);

	resize();

	pen = CreatePen(PS_SOLID, 1, RGB(0,0,0) );

#ifdef DOLABELHWND
	//---------------
	// the label:
	//---------------

	hwndLabel = CreateWindowEx(
				//WS_EX_STATICEDGE,
				NULL,
				"static",
				NULL,
				WS_CHILD | WS_VISIBLE | ES_LEFT,
				label_x,
				y,
				label_w,
				label_h,
				phwnd,
				NULL,								// (HMENU)id,
				hInstance,
				NULL);
	if (hwndLabel==NULL)  {
		#ifdef _DEBUG
			Err *err = new Err();
			DEL(err);
		#endif
		destroy();
		throw (fatalError(__FILE__, __LINE__));
	}
	SendMessage(hwndLabel, WM_SETFONT, (WPARAM)font, TRUE);

	HDC hdc = GetDC(hwndLabel);
	RECT r;
	GetClientRect(hwndLabel, &r);
	FillRect(hdc, &r, bgbrush);
	SetTextColor(hdc, label_fg_color);
	SetBkColor(hdc, label_bg_color);
	//SetBkMode(hdc, TRANSPARENT);
	ReleaseDC(hwndLabel,hdc);

	MoveWindow(hwndLabel, label_x, y, label_w, label_h, FALSE);
	if (thelabel[0] != 0)  {
		SetWindowText(hwndLabel, thelabel);
	}
#endif

	//---------------
	// the data:
	//---------------

	hwndData = CreateWindowEx(
				//WS_EX_STATICEDGE,
				NULL,
				"edit",
				NULL,
				WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,

				data_x, 
				y,
				data_w,
				data_h,

				phwnd,
				(HMENU)id,
				hInstance,
				NULL);

	if (hwndData==NULL)  {
		#ifdef _DEBUG
			Err *err = new Err();
			DEL(err);
		#endif
		destroy();
		throw (fatalError(__FILE__, __LINE__));
	}

	SendMessage(hwndData, WM_SETFONT, (WPARAM)font, TRUE);

	float f = (float) (data_x + data_w) / (float)(pclientrect.right - pclientrect.left);
	if (f > max_px)  {
		max_px = f;
	}

	MoveWindow(hwndData, data_x, y, data_w, data_h, FALSE);

	if (data[0] != 0)  {
		setText(data);
	}

	keydown = false;
	instances++;

	if (instances==1)  {
		first_font_size = font_size;
	}

#ifdef INTERCEPT_LABEL
	if (hwndLabel)  {
		savelabel = GetWindowLong(hwndLabel, GWL_WNDPROC);
		SetWindowLong (hwndLabel, GWL_USERDATA, (long)this);
		oldLabelProc = (WNDPROC) SetWindowLong(hwndLabel, GWL_WNDPROC, (LPARAM) labelproc);		// subclass to intercept messages
	}
#endif

	if (hwndData)  {
		savedata = GetWindowLong(hwndData, GWL_WNDPROC);
		SetWindowLong (hwndData, GWL_USERDATA, (long)this);
		oldEditProc = (WNDPROC) SetWindowLong(hwndData, GWL_WNDPROC, (LPARAM) editproc);		// subclass to intercept messages
	}

	return;

}												// constructor 2

#ifdef INTERCEPT_LABEL

/**********************************************************************

**********************************************************************/

LRESULT CALLBACK editBox::labelproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)  {
	int bp=0;

	editBox *eb = (editBox *)GetWindowLong(hwnd, GWL_USERDATA);
	if (eb==NULL)  {
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

#ifdef _DEBUG
	const char *cptr = MessageName(msg);
	if (msg = 0x138)  {
		bp = 1;
	}
#endif


	switch(msg)  {

		case WM_CTLCOLORSTATIC:  {
			//HDC hdc = GetDC(hwnd);
			//SetBkMode(hdc, TRANSPARENT);
			//ReleaseDC(hwnd, hdc);
			//SetBkMode((HDC)wparam, TRANSPARENT);
			//return (INT_PTR)(HBRUSH)GetStockObject(NULL_BRUSH);
			//return 
			//return TRUE;
			break;
		}

		default:
			break;
	}
          
	return CallWindowProc(eb->oldLabelProc, hwnd, msg, wparam, lparam) ;

}									// labelproc
#endif




/**********************************************************************

**********************************************************************/

LRESULT CALLBACK editBox::editproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)  {
	int bp=0;
	editBox *eb = (editBox *)GetWindowLong(hwnd, GWL_USERDATA);
	if (eb==NULL)  {
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	switch(msg)  {
		case WM_PAINT:  {
			bp = 1;
			break;
		}

		case WM_SETFOCUS:
			bp = 1;
			break;

		case WM_KEYDOWN:  {
			if (eb->keydown)  {
				break;
			}
			eb->keydown = true;
			switch(wparam)  {
				case VK_TAB:  {
					SendMessage(eb->phwnd, eb->usermsg, eb->id, 0);
					break;
				}
				case VK_ESCAPE:
					SendMessage(eb->phwnd, eb->usermsg, eb->id, 1);
					break;


				default:
					break;
			}
			break;
		}
		case WM_KEYUP:  {
			eb->keydown = false;
			break;
		}

		default:
			break;
	}
          
	return CallWindowProc(eb->oldEditProc, hwnd, msg, wparam, lparam) ;

}

/**********************************************************************

**********************************************************************/

editBox::~editBox()  {
	destroy();
}

/**********************************************************************

**********************************************************************/

void editBox::destroy(void)  {

	instances--;
	if (instances==0)  {
		first_font_size = 0;
	}

	if (bgbrush)  {
		DeleteObject(bgbrush);
		bgbrush = NULL;
	}

	if (font)  {
		DeleteObject(font);
		font = NULL;
	}

#ifdef DOLABELHWND
	if (hwndLabel)  {
		if (IsWindow(hwndLabel))  {
			DestroyWindow(hwndLabel);		// without this logger.exe blew up
		}
	}

	#ifdef INTERCEPT_LABEL
		if (hwndLabel)  {
			SetWindowLong (hwndLabel, GWL_WNDPROC, savelabel);
			if (IsWindow(hwndLabel))  {
				DestroyWindow(hwndLabel);		// without this logger.exe blew up
			}
		}
	#endif
#endif

	if (hwndData)  {
		SetWindowLong (hwndData, GWL_WNDPROC, savedata);
		if (IsWindow(hwndData))  {
			DestroyWindow(hwndData);		// without this logger.exe blew up
		}
	}

	if (pen)  {
		DeleteObject(pen);
		pen = NULL;
	}

	return;
}



/**********************************************************************

**********************************************************************/

const char *editBox::getText(void)  {

	WORD *wp = (WORD *)data;
	*wp = sizeof(data);
	if (hwndData)  {
		SendMessage(hwndData, EM_GETLINE, 0, (long)data);
	}

	return data;
}

/**********************************************************************

**********************************************************************/

void editBox::enable(void)  {

	if (hwndData)  {
		EnableWindow(hwndData, TRUE);
	}

	return;
}

/**********************************************************************

**********************************************************************/

void editBox::disable(void)  {

	if (hwndData)  {
		EnableWindow(hwndData, FALSE);
	}

	return;
}

/**********************************************************************

**********************************************************************/

void editBox::paint(HDC phdc)  {

#ifndef DOLABELHWND
	TextOut(phdc, label_x, y, label, strlen(label));
#endif

	return;
}

/**********************************************************************
	//SetWindowText(hwndData, data);
	//SendMessage(hwndData, WM_SETTEXT, 0, (LPARAM)data);
	//setText(data);
**********************************************************************/

void editBox::setText(char *str)  {
	SetWindowText(hwndData, str);
	return;
}

/**********************************************************************
	entry:
		r = new parent's clientrect
**********************************************************************/

void editBox::resize(void)  {
	if (constructor==2)  {
		resize2();
		return;
	}

	RECT *r;
	r = &pclientrect;

	label_x = (int) ( .5f + r->left + px1 * (r->right - r->left));
	data_x = (int) ( .5f + r->left + px2 * (r->right - r->left));
	y = (int) ( .5f + r->top + py * (r->bottom - r->top));

	label_w = (int) ( .5f + pw * (r->right - r->left) );
	//h = ROUND(ph * (r->bottom - r->top) );

	memset(&labelrect, 0, sizeof(RECT));

	//if (label[0] != 0)  {
	labelrect.left = label_x;
	labelrect.top = y;
	labelrect.right = labelrect.left + label_w;
	//if (h!=0)  {
	//	labelrect.bottom = labelrect.top + h;
	//}
	//}

	int k;

	HDC hdc = GetDC(phwnd);

	if (font)  {
		DeleteObject(font);
		font = NULL;
	}

	int ww = labelrect.right - labelrect.left;

	if (first_font_size == 0)  {
	//if (h==0)  {					// autosizing font
		for(k=160; k>=6; k--)  {
			font = CreateFont(
									-k,								// height of font
									0,									// average character width
									0,									// angle of escapement
									0,									// base-line orientation angle
									800,								// font weight
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
			GetTextExtentPoint32(hdc, thelabel, strlen(thelabel), &sz);
			//if (sz.cx >= ROUND(.9*(labelrect.right - labelrect.left)))  {
			if (sz.cx >= ww)  {
				DeleteObject(font);
				font = NULL;
				continue;
			}

			break;
		}

		font_size = k;
	}
	else  {
		font = CreateFont(
								-first_font_size,								// height of font
								0,									// average character width
								0,									// angle of escapement
								0,									// base-line orientation angle
								800,								// font weight
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
		//SelectFont(hdc, font);
		//GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		font_size = first_font_size;

	}

	// increase the height in case we need room for descenders and ascenders:
	SelectFont(hdc, font);
	GetTextExtentPoint32(hdc, "Wp", strlen("Wp"), &sz);
	label_h = sz.cy;
	labelrect.bottom = labelrect.top + label_h;

	/*
	if (h==0)  {
		labelrect.bottom = labelrect.top + sz.cy;
		h = labelrect.bottom - labelrect.top;
	}
	*/

	//SelectFont(hdc, font);
	GetTextExtentPoint32(hdc, "W", 1, &sz);
	charwid = sz.cx;

	//data_x = label_x + label_w + charwid;

	ReleaseDC(phwnd, hdc);
	hdc = 0;
	SendMessage(hwndLabel, WM_SETFONT, (WPARAM)font, FALSE);

	// pw = .25, ph = .10, font_size =  70
	// pw = .50, ph = .10, font_size = 151

	if (hwndData)  {
		hdc = GetDC(hwndData);
			SelectFont(hdc, font);
			GetTextExtentPoint32(hdc, data, strlen(data), &sz);
			data_w = sz.cx;
			data_h = sz.cy;
		ReleaseDC(hwndData, hdc);
		hdc = 0;

		SendMessage(hwndData, WM_SETFONT, (WPARAM)font, FALSE);
		MoveWindow(hwndData, data_x, y, data_w, data_h, FALSE);
	}
	return;
}					// resize()

/**********************************************************************

**********************************************************************/

void editBox::resize2(void)  {
	RECT *r;
	int k;
	HDC hdc;

#ifdef _DEBUG
	resizecalls++;
#endif

	r = &pclientrect;

	label_x = ROUND(r->left + p_label_start * (r->right - r->left));
	y = ROUND(r->top + p_y_start * (r->bottom - r->top));
	float pdx = p_label_end - p_label_start;
	int dx = r->right - r->left;
	label_w = ROUND(pdx * dx );

	data_x = label_x + label_w + 10;
	data_w = ROUND(p_data_wid * (r->right - r->left) );

	memset(&labelrect, 0, sizeof(RECT));

	labelrect.left = label_x;
	labelrect.top = y;
	labelrect.right = labelrect.left + label_w;

	datarect.left = data_x;
	datarect.top = y;
	datarect.right = datarect.left + data_w;

	hdc = GetDC(phwnd);

	if (font)  {
		DeleteObject(font);
		font = NULL;
	}


//if (first_font_size == 0)  {

	for(k=50; k>=6; k--)  {
		font = CreateFont(
									-k,								// height of font
									0,									// average character width
									0,									// angle of escapement
									0,									// base-line orientation angle
									800,								// font weight
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
		GetTextExtentPoint32(hdc, thelabel, strlen(thelabel), &sz);
		//if (sz.cx >= ROUND(.9*(labelrect.right - labelrect.left)))  {
		if (sz.cx >= label_w)  {
			DeleteObject(font);
			font = NULL;
			continue;
		}
		break;
	}										// for()
	font_size = k;

//#ifdef _DEBUG
#if 0
	if (id==4)  {
		if (resizecalls==1)  {
			bp = 2;
		}
	}
#endif

	/*
}
else  {
		font = CreateFont(
								-first_font_size,								// height of font
								0,									// average character width
								0,									// angle of escapement
								0,									// base-line orientation angle
								800,								// font weight
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
		//SelectFont(hdc, font);
		//GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		font_size = first_font_size;
}
	*/

	// increase the height in case we need room for descenders and ascenders:

	// hdc = label HDC

	SelectFont(hdc, font);						// parent's hdc!

#ifdef _DEBUG
	GetTextExtentPoint32(hdc, thelabel, strlen(thelabel), &sz);
#endif

	GetTextExtentPoint32(hdc, "Wp", strlen("Wp"), &sz);
	label_h = sz.cy;
	data_h = label_h;
	labelrect.bottom = labelrect.top + label_h;
	datarect.bottom = datarect.top + data_h;

	SelectFont(hdc, font);
	GetTextExtentPoint32(hdc, "W", 1, &sz);
	charwid = sz.cx;



	ReleaseDC(phwnd, hdc);
	hdc = 0;


#ifdef DOLABELHWND
	if (hwndLabel)  {
		if (thelabel[0] != 0)  {
			hdc = GetDC(hwndLabel);
			SelectFont(hdc, font);
			GetTextExtentPoint32(hdc, thelabel, strlen(thelabel), &sz);
			label_w = sz.cx;
			label_h = sz.cy;
			data_h = sz.cy;

			#ifdef _DEBUG
				if (id==4)  {
					GetTextExtentPoint32(hdc, thelabel, strlen(thelabel), &sz);
					if (resizecalls==1)  {
						// hwndLable is null the first time
					}
					else if (resizecalls==2)  {
						// label_w = 264
						// k = 33
						// sz1 = 264, 38
						// sz2 = 264, 38
						bp = 3;
					}
					else if (resizecalls==3)  {
						// label_w = 264   <<<<<<<<<<<<<<<<<<<<<<<
						// k = 33
						// sz1 = 264, 38
						// sz2 = 264, 38
						bp = 3;
					}
				}
			#endif

			data_x = label_x + label_w + 10;
		}					// if (text)

		//SendMessage(hwndLabel, WM_SETFONT, (WPARAM)font, FALSE);
		MoveWindow(hwndLabel, label_x, y, label_w, label_h, FALSE);
		ReleaseDC(hwndLabel, hdc);
		hdc = 0;
	}								// if (hwndLabel)  {
#endif

	/*
	if (hwndData)  {
		if (data != NULL && data[0] != 0)  {
			hdc = GetDC(hwndData);
				SelectFont(hdc, font);
				GetTextExtentPoint32(hdc, data, strlen(data), &sz);
				data_w = sz.cx;
				//data_h = sz.cy;
			ReleaseDC(hwndData, hdc);
			hdc = 0;
		}
		SendMessage(hwndData, WM_SETFONT, (WPARAM)font, FALSE);
		MoveWindow(hwndData, data_x, y, data_w, data_h, FALSE);
	}
	*/

	return;
}					// resize2()

/**********************************************************************
	wParam = hdc
	lParam = hwnd
**********************************************************************/

LRESULT editBox::wmctlcolorstatic(WPARAM wparam, LPARAM lparam)  {

	HWND hw = (HWND)lparam;
	HDC hdc = (HDC) wparam;

#ifdef _DEBUG
	//if (!strcmp(label, "Serial Number"))  {
	if (!strcmp(thelabel, "Interface Box Part Number (model_type):"))  {
		bp = 1;
	}
#endif

	if (hw==hwndLabel)  {
		//SelectObject( (HDC)wParam, labelfont);
		#if 0
			SetBkMode( (HDC)wparam, TRANSPARENT );
			return (BOOL)GetStockObject(NULL_BRUSH);
		#else
			SetBkMode( hdc, OPAQUE );
			SetTextColor( hdc, label_fg_color );
			SetBkColor( hdc, label_bg_color);
			return (INT_PTR)bgbrush;
			//return (LRESULT)bgbrush;
			//return (BOOL)bgbrush;
		#endif
	}

	return FALSE;
}														// wmctlcolorstatic
