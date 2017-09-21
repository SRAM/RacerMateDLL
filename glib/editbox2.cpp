
#define STRICT
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996)					// for vista strncpy_s

#include <windowsx.h>

#include <glib_defines.h>
#include <err.h>
#include <utils.h>
#include <fatalerror.h>
#include <editBox2.h>

int editBox2::instances = 0;
int editBox2::first_font_size = 0;

/**********************************************************************
	px is the left of the label
	pw is the width of the label
**********************************************************************/

editBox2::editBox2(
			HWND _phwnd,
			char *_label,
			float _px,
			float _py,
			float _pw,
			int _id,
			float _ph,
			char *_data,
			int _usermsg )  {

	HDC hdc;
	BOOL b;

	phwnd = _phwnd;
	memset(label, 0, sizeof(label));
	if (_label[0] != 0)  {
		strncpy(label, _label, sizeof(label)-1);
	}
	memset(data, 0, sizeof(data));

	if (_data)  {
		strncpy(data, _data, sizeof(data)-1);
	}

	font = NULL;
	hwndData = NULL;
	//hwndLabel = NULL;

	hInstance = GetModuleHandle(NULL);
	id = _id;
	px = _px;
	py = _py;
	pw = _pw;
	ph = _ph;

	usermsg = _usermsg;

	font_size = 0;

	GetClientRect(phwnd, &pclientrect);

	//resize(&pclientrect);

	pen = CreatePen(PS_SOLID, 1, RGB(0,0,0) );

	//---------------
	// the label:
	//---------------

#if 0
	hwndLabel = CreateWindowEx(
				WS_EX_STATICEDGE,
				"static",
				NULL,
				WS_CHILD | WS_VISIBLE | ES_LEFT,
				labelx,
				y,
				labelw,
				h,
				phwnd,
				(HMENU)id,
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
#endif

	
	//---------------
	// the data:
	//---------------

	hwndData = CreateWindowEx(
				//WS_EX_DLGMODALFRAME,
				//WS_EX_WINDOWEDGE,					// MUST use in combination with WS_THICKFRAME or WS_DLGFRAME 
				WS_EX_STATICEDGE,
				//NULL,
				"edit",
				//data,
				//"data",
				NULL,
				//WS_BORDER |
				//WS_SYSMENU |
				//DS_3DLOOK |
				//WS_DLGFRAME |
				//WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_UPPERCASE,
				WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
				datax, 
				y,
				dataw,
				h,
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
	//SetWindowText(hwndData, data);
	//SendMessage(hwndData, WM_SETTEXT, 0, (LPARAM)data);
	//setText(data);




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

	size(&pclientrect);


	if (hwndData)  {
		save = GetWindowLong(hwndData, GWL_WNDPROC);
		SetWindowLong (hwndData, GWL_USERDATA, (long)this);
		oldEditProc = (WNDPROC) SetWindowLong(hwndData, GWL_WNDPROC, (LPARAM) editproc);		// subclass to intercept messages
	}


	return;

}									// constructor

/**********************************************************************

**********************************************************************/

LRESULT CALLBACK editBox2::editproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)  {
	int bp=0;

	/*
	static editBox2 *eb = NULL;
	if (eb==NULL)  {
		CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
		if (cs==NULL)  {
			return 0;
		}
		eb = (editBox2 *)cs->lpCreateParams;
		if (eb==NULL)  {
			bp = 1;
			return 0;
		}
		else  {
			bp = 1;
		}
	}
	*/

	editBox2 *eb = (editBox2 *)GetWindowLong(hwnd, GWL_USERDATA);
	if (eb==NULL)  {
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	switch(msg)  {

		case WM_PAINT:  {
			bp = 1;
			break;
		}

	 /*
		case WM_COMMAND:  {
			bp = 2;
			break;
		}

		case WM_NOTIFY:  {
			bp = 2;
			break;
		}
	*/

		//case WM_CHAR:  {
			/*
			if(wparam==0x08)  {
				eb->dataptr--;
			}
			else  {
				//eb->data[eb->dataptr] = (char)(wparam&0x00ff);
				//eb->dataptr = (eb->dataptr + 1) % 256;
				eb->dataptr++;
			}
			*/
		//	break;
		//}

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

editBox2::~editBox2()  {
	destroy();
}


/**********************************************************************

**********************************************************************/

/*
void editBox2::move(int _x, int _y, int _w, int _h)  {

	x = _x;
	y = _y;
	w = _w;
	h = _h;

	MoveWindow(hwnd, x, y, w, h, TRUE);
	MoveWindow(hwndLabel, x, y+eh, w, h, TRUE);
	//MoveWindow(hwndLabel, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, TRUE);

	return;

}
*/

/**********************************************************************

**********************************************************************/

void editBox2::destroy(void)  {

	instances--;

	/*
	if (labelfont)  {
		DeleteObject(labelfont);
		labelfont = NULL;
	}
	*/
	//if (hdcLabel) ReleaseDC(hwndLabel, hdcLabel);

	//ReleaseDC(hwnd, hdc);

	if (font)  {
		DeleteObject(font);
		font = NULL;
	}

	if (hwndData)  {
		SetWindowLong (hwndData, GWL_WNDPROC, save);
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
	wParam = hdc
	lParam = hwnd
**********************************************************************/

LRESULT editBox2::wmctlcolorstatic(WPARAM wparam, LPARAM lparam)  {

	/*
	HWND hw = (HWND)lparam;

	if (hw==hwndLabel)  {
		//SelectObject( (HDC)wParam, labelfont);
		SetBkMode( (HDC)wparam, TRANSPARENT );
		//SetTextColor( (HDC)wParam, textcolor );
		return (BOOL)GetStockObject(NULL_BRUSH);
	}
	*/

	return FALSE;
}

/**********************************************************************

**********************************************************************/

const char *editBox2::getText(void)  {

	WORD *wp = (WORD *)data;
	*wp = sizeof(data);
	if (hwndData)  {
		SendMessage(hwndData, EM_GETLINE, 0, (long)data);
	}

	return data;
}

/**********************************************************************

**********************************************************************/

void editBox2::enable(void)  {

	if (hwndData)  {
		EnableWindow(hwndData, TRUE);
	}

	return;
}

/**********************************************************************

**********************************************************************/

void editBox2::disable(void)  {

	if (hwndData)  {
		EnableWindow(hwndData, FALSE);
	}

	return;
}

/**********************************************************************

**********************************************************************/

void editBox2::paint(HDC hdc)  {

	/*
	// centers label:
	SIZE sz;
	int ix, iy;
	if (label[0] != 0) {
		SelectFont(hdc, font);
		GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		ix = ROUND(.5*(labelrect.left + labelrect.right - sz.cx));
		iy = ROUND(.5*(labelrect.top + labelrect.bottom - sz.cy));
		TextOut(hdc, ix, iy, label, strlen(label));
	}
	*/


	//SendMessage(hwndData, WM_SETFONT, (WPARAM)font, FALSE);
	TextOut(hdc, labelx, y, label, strlen(label));

#if 0
	SelectPen(hdc, pen);
	MoveToEx(hdc, labelrect.left, labelrect.bottom, NULL);
	LineTo(hdc, labelrect.left,  labelrect.top);
	LineTo(hdc, labelrect.right,  labelrect.top);
	LineTo(hdc, labelrect.right,  labelrect.bottom);
	LineTo(hdc, labelrect.left,  labelrect.bottom);

#endif

	return;
}

/**********************************************************************
	//SetWindowText(hwndData, data);
	//SendMessage(hwndData, WM_SETTEXT, 0, (LPARAM)data);
	//setText(data);
**********************************************************************/

void editBox2::setText(char *str)  {

	//SendMessage(hwndData, EM_REPLACESEL, FALSE, (long)str);
	//SendMessage(hwndData, WM_SETTEXT, 0, (long)str);
	SetWindowText(hwndData, str);
	

	return;
}

/**********************************************************************
	entry:
		r = new parent's clientrect
**********************************************************************/

void editBox2::resize(void)  {

	RECT *r;
	//CopyRect(&pclientrect, r);
	r = &pclientrect;

/*
	x = (int) ( .5f + r->left + px * (r->left + r->right));
	y = (int) ( .5f + r->top + py * (r->top + r->bottom));
	w = (int) ( .5f + pw * (r->right - r->left) );
	h = (int) ( .5f + ph * (r->bottom - r->top) );
*/

	labelx = (int) ( .5f + r->left + px * (r->right - r->left));
	y = (int) ( .5f + r->top + py * (r->bottom - r->top));
	labelw = (int) ( .5f + pw * (r->right - r->left) );
	h = (int) ( .5f + ph * (r->bottom - r->top) );

	datax = labelx + labelw + 60;		// x is the label's x
	//datax = x + w + 18;		// x is the label's x

	memset(&labelrect, 0, sizeof(RECT));

	if (label[0] != 0)  {
		labelrect.left = labelx;
		labelrect.top = y;
		labelrect.right = labelrect.left + labelw;
		if (h!=0)  {
			labelrect.bottom = labelrect.top + h;
		}
	}

	int k;

	HDC hdc = GetDC(phwnd);

	if (font)  {
		DeleteObject(font);
		font = NULL;
	}

	int ww = labelrect.right - labelrect.left;

	if (first_font_size == 0)  {
	//if (h==0)  {					// autosizing font
		for(k=30; k>=6; k--)  {
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
			GetTextExtentPoint32(hdc, label, strlen(label), &sz);
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
		SelectFont(hdc, font);
		GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		font_size = first_font_size;

	}


	if (h==0)  {
		labelrect.bottom = labelrect.top + sz.cy;
		h = labelrect.bottom - labelrect.top;
	}

	ReleaseDC(phwnd, hdc);


	if (hwndData)  {
		SendMessage(hwndData, WM_SETFONT, (WPARAM)font, FALSE);
		MoveWindow(hwndData, datax, y, dataw, h, FALSE);
	}

	return;
}					// resize()

/**********************************************************************
	entry:
		r = new parent's clientrect
**********************************************************************/

void editBox2::size(RECT *r)  {
	int k, labelwid;
	HDC hdc;

	CopyRect(&pclientrect, r);
	memset(&labelrect, 0, sizeof(RECT));

	labelrect.left = labelx = x = ROUND(r->left + px * (r->left + r->right));
	labelrect.right = ROUND(r->left + pw * (r->left + r->right));
	labelrect.top = y = ROUND(r->top + py * (r->top + r->bottom));

	labelwid = labelrect.right - labelrect.left;
	w = ROUND(pw * (r->right - r->left) );

	hdc = GetDC(phwnd);

	if (font)  {
		DeleteObject(font);
		font = NULL;
	}


	for(k=30; k>=6; k--)  {
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
		GetTextExtentPoint32(hdc, label, strlen(label), &sz);
		//if (sz.cx >= ROUND(.9*(labelrect.right - labelrect.left)))  {
		if (sz.cx >= labelwid)  {
			DeleteObject(font);
			font = NULL;
			continue;
		}

		break;
	}								// for()

	font_size = k;
	h = sz.cy;

	labelrect.bottom = labelrect.top + sz.cy;
	labelrect.right = labelrect.left + sz.cx;
	w = ROUND(pw * (r->right - r->left) );

	SelectFont(hdc, font);

	ReleaseDC(phwnd, hdc);

	datax = labelrect.right + 20;


	if (hwndData)  {
		hdc = GetDC(hwndData);
		SelectFont(hdc, font);
		SendMessage(hwndData, WM_SETFONT, (WPARAM)font, FALSE);
		MoveWindow(hwndData, datax, y, w, h, FALSE);
		ReleaseDC(phwnd, hdc);
	}

	return;
}					// size()

