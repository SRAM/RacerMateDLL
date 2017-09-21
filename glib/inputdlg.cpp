
#define STRICT
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996)					// for vista strncpy_s

//#include <config.h>
#include <glib_defines.h>
#include <utils.h>
#include <err.h>
#include <fatalerror.h>

#include <inputdlg.h>

extern char gstring[2048];

/**********************************************************************
	constructor
**********************************************************************/

inputDlg::inputDlg(
						  HWND _phwnd,
						  const char *_title, 
						  const char *_label, 
						  HICON _icon, 
						  int _x, 
						  int _y, 
						  int _w, 
						  int _h,
						  const char *_default_text) : modalDialog(_phwnd, _title, _icon, _x, _y, _w, _h)  {
	char str[32];
	int i;
	float yf;

	default_text = _default_text;


	label = _label;

	cancelled = true;
	memset(data, 0, sizeof(data));

	float px, py, ph, pw;
	SIZE sz;
	int iw, ih;

	iw = clientrect.right - clientrect.left;
	ih = clientrect.bottom - clientrect.top;

	//---------------
	// the label:
	//---------------

	px = .01f;
	py = .25f;
	ph = .10f;
	pw = .98f;

	labelx = ROUND( clientrect.left + px * iw);
	labely = ROUND( clientrect.top + py * ih);
	labelw = ROUND( pw * iw);
	labelh = ROUND( ph * ih );

	labelrect.left = labelx;
	labelrect.top = labely;
	labelrect.right = labelrect.left + labelw;
	labelrect.bottom = labelrect.top + labelh;


	hwndLabel = CreateWindowEx(
				NULL,
				"static",
				label,
				WS_CHILD | WS_VISIBLE,
				labelx, 
				labely,
				labelw,
				labelh,
				hwnd,
				(HMENU)0,
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


	//brush = CreateSolidBrush(RGB(80, 0, 0));
	//oldbrush = (HBRUSH)SelectObject (hdc, brush);
	//FillRect(hdc, &labelrect, brush);


	//GetTextExtentPoint32(hdc, label, strlen(label), &sz);
	//datax = labelx + sz.cx + 15;

	px = .01f;
	datax = ROUND( clientrect.left + px * iw);

	py = .4f;
	datay = ROUND( clientrect.top + py * ih);
	//datay = labely;
	GetTextExtentPoint32(hdc, "A", 1, &sz);
	//datah = labelh;
	datah = sz.cy + 4;
	dataw = clientrect.right - datax - 15;


	//---------------
	// the data:
	//---------------

	hwndData = CreateWindowEx(
				WS_EX_STATICEDGE,
				"edit",
				NULL,
				WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
				datax, 
				datay,
				dataw,
				datah,
				hwnd,
				(HMENU)EB1,
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

	if (default_text)  {
		SetWindowText(hwndData, default_text);
	}
	SetFocus(hwndData);


	yf = .8f;
	ok_button = new Button(hwnd, "OK", .1f, yf, .25f, .1f, OK_BUTTON);
	cancel_button = new Button(hwnd, "Cancel",   .65f, yf, .25f, .1f, CANCEL_BUTTON);

	mainloop();							// run the parent's message loop

	return;
}


/**********************************************************************
	destructor
**********************************************************************/

inputDlg::~inputDlg()  {
	destroy();
	return;
}

/********************************************************************
wParam 
	The high-order word specifies the notification code if the message is from a control. 
	If the message is from an accelerator, this value is 1. 
	If the message is from a menu, this value is zero.

	The low-order word specifies the identifier of the menu item, control, or accelerator. 

lParam 
	Handle to the control sending the message if the message is from a control. 
	Otherwise, this parameter is NULL. 

********************************************************************/

void inputDlg::wm_command(WPARAM wParam, LPARAM lParam)  {

	#ifdef _DEBUG
	WORD hw = HIWORD(wParam);
	#endif


	int id = LOWORD(wParam);


	switch(id)  {
		case OK_BUTTON:
			GetWindowText(hwndData, data, sizeof(data));
			cancelled = false;
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case CANCEL_BUTTON:
			cancelled = true;
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case EB1:
			bp = 2;
			break;

		default:  {
#if 0
			// a message from one of the edit controls: 0 to cols-1
			int code = HIWORD(wParam);
			long cwh = lParam;

			switch(code)  {
				case EN_UPDATE:
					bp = 1;								// 1
					break;
				case EN_CHANGE:
					bp = 1;								// 2
					break;
				case EN_SETFOCUS:
					bp = 1;								// 3, when you click in an edit box
					break;
				case EN_KILLFOCUS:
					bp = 1;								// 4, when you click outside an edit box
					break;

				case EN_ERRSPACE:
					bp = 1;
					break;
				case EN_MAXTEXT:
					bp = 1;
					break;
				case EN_HSCROLL:
					bp = 1;
					break;
				case EN_VSCROLL:
					bp = 1;
					break;
				default:
					bp = 1;
					break;
			}
#endif



			break;
		}
	}

	return;
}

/********************************************************************

********************************************************************/

void inputDlg::wm_keydown(WPARAM wParam, LPARAM lParam)  {

	if (keydown)  {
		return;
	}
	keydown = true;

	switch(wParam)  {
		case VK_TAB:  {
			bp = 1;
			break;
		}
		case VK_ESCAPE:
			break;

		default:
			break;
	}
	return;
}


/********************************************************************

********************************************************************/

void inputDlg::wm_keyup(WPARAM wParam, LPARAM lParam)  {

	keydown = false;

	return;
}

/********************************************************************
	an editBox has sent us a message if the tab key or escape key was
	hit while the focus was on that editBox.
********************************************************************/

void inputDlg::wm_user(WPARAM wParam, LPARAM lParam)  {
	int id;
	int command;

	id = wParam;
	command = (int)lParam;

	id++;

	switch(command) {
		case 0:									// tab key
			//SetFocus(edt[id]->hwndData);
			break;
		case 1:  {									// escape key, save the data!!!
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
	}

	return;
}

/********************************************************************

********************************************************************/

void inputDlg::wm_paint(WPARAM wParam, LPARAM lParam)  {
	int i;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT r;


	hdc = BeginPaint(hwnd, &ps);


	r.left = ph(clientrect, .5f);
	r.right = ph(clientrect, .7f);


	EndPaint (hwnd, &ps);

	return;
}


#if 0
/********************************************************************

********************************************************************/

void inputDlg::seticon(void)  {

	return;
}

#endif

/********************************************************************

********************************************************************/

const char *inputDlg::get_text(void)  {
	return data;
}


/********************************************************************

********************************************************************/

void inputDlg::destroy(void)  {
	DEL(ok_button);
	DEL(cancel_button);
//	DEL(eb);

	if (icon)  {
		DestroyIcon(icon);
		icon = 0;
	}

	//SelectObject (hdc, oldbrush);
	//DeleteObject (brush);

	return;
}

