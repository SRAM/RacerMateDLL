
#include <windows.h>
#include <glib_defines.h>

#include <fatalerror.h>

//#ifdef _DEBUG
	#include <err.h>
//#endif

#include <utils.h>

#include <statusdlg.h>

/********************************************************************
	statusDlg *st = new statusDlg(
		"Status",				// title
		.2f,						// x width
		.15f						// y width
		);
	char str[32];
	for(int k=0;k<4;k++)  {
		Sleep(1000);
		sprintf(str, "%d", k);
		st->setText(str);
	}
	DEL(st);

********************************************************************/

statusDlg::statusDlg(char *_title, 
							float _xp,			// width as percentage
							float _yp			// heigth as percentage
							)  {

	title = _title;
	xp = _xp;
	yp = _yp;

#ifdef _DEBUG
	dbgstream = fopen("sdlg.txt", "wt");
#endif


	hInstance = GetModuleHandle(NULL);
	//bgbrush = CreateSolidBrush(DIALOG_GRAY);
	bgbrush = CreateSolidBrush(RGB(80, 0, 0));

	WNDCLASS wc = { 
					CS_HREDRAW | CS_VREDRAW,
					proc,
					0,
					0,
					hInstance,
					LoadIcon(NULL, IDI_APPLICATION),
					LoadCursor(NULL, IDC_ARROW),
					bgbrush,
					NULL,
					MY_STATUS_DIALOG_CLASS
	};

	if (!RegisterClass(&wc))  {
		if (!UnregisterClass (MY_STATUS_DIALOG_CLASS, hInstance))  {
			#ifdef _DEBUG
				Err *err = new Err();
				char *str = err->getString();
				delete err;
			#endif
			throw(fatalError(__FILE__, __LINE__));
		}
		else  {
			if (!RegisterClass(&wc))  {
				throw(fatalError(__FILE__, __LINE__));
			}
		}
	}

	int w, h;
	w = GetSystemMetrics (SM_CXFULLSCREEN);
	h = GetSystemMetrics (SM_CYFULLSCREEN);
	w = (int) (.5 + xp*w);
	h = (int) (.5 + yp*h);

	hwnd = CreateWindowEx(	
					0,
					MY_STATUS_DIALOG_CLASS,
					title,
					WS_OVERLAPPED | WS_CAPTION,
					0,
					0,
					w,
					h,
					NULL,	//phwnd,
					NULL,								// No Menu
					hInstance,							// Instance
					this);

	if (hwnd==NULL)  {
		Err *err = new Err();
		MessageBox( NULL, err->getString(), "Error", MB_OK | MB_ICONINFORMATION );
		DEL(err);
		cleanup();
		return;
	}

	SetWindowLong (hwnd, GWL_USERDATA, (long)this);

	center_window(hwnd);
	ShowWindow(hwnd, SW_SHOWNORMAL);
	getClientRect(hwnd, &clientrect);

//	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
//	ShowWindow(phwnd, SW_HIDE);
//	EnableWindow(phwnd, FALSE);

HDC hdc = GetDC(hwnd);
SIZE sz;
	GetTextExtentPoint32(hdc, "X", 1, &sz);
	float py = 1;
	py = (float) ((float)sz.cy / clientrect.bottom);

	text = new staticText(
					hwnd, 
					"xxx", 
					//-1.0f, 
					//-1.0f, 
					//-1.0f, 
					//-1.0f,
					//.1f,.2f, .8f, .8f,			// tlm20050307  doesn't work for comtest
					0.0f, 0.0f, 1.0f, 1.0f,
					//.1f,.02f, .8f, py,
					0, 
					0, 
					RGB(0,0,0), 
					RGB(0,255,0));

}

/********************************************************************

********************************************************************/

statusDlg::~statusDlg()  {
	cleanup();
}

/**********************************************************************

**********************************************************************/

void statusDlg::cleanup(void)  {

	if (bgbrush)  {
		DeleteObject(bgbrush);
		bgbrush = 0;
	}

	DEL(text);
	DestroyWindow(hwnd);

#ifdef _DEBUG
	fclose(dbgstream);
	dbgstream = NULL;
#endif
	return;
}

/**********************************************************************

**********************************************************************/

LRESULT CALLBACK statusDlg::proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)  {

	statusDlg *st = (statusDlg *)GetWindowLong(hwnd, GWL_USERDATA);
	if (st==NULL)  {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

#ifdef _DEBUG
	//checkMsg(msg, st->dbgstream);
	/*
	WM_WINDOWPOSCHANGING
	WM_GETMINMAXINFO
	WM_WINDOWPOSCHANGED
	WM_MOVE
	WM_SHOWWINDOW
	WM_WINDOWPOSCHANGING
	WM_WINDOWPOSCHANGING
	WM_ACTIVATEAPP
	WM_NCACTIVATE
	WM_GETTEXT
	WM_ACTIVATE
	WM_SETFOCUS
	WM_NCPAINT
	WM_GETTEXT
	WM_ERASEBKGND
	WM_WINDOWPOSCHANGED
	WM_SIZE
	WM_MOVE
	WM_PARENTNOTIFY
	WM_ERASEBKGND
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_CTLCOLORSTATIC
	WM_PARENTNOTIFY
	WM_WINDOWPOSCHANGING
	WM_WINDOWPOSCHANGED
	WM_DESTROY
	WM_NCDESTROY
	*/
#endif

	switch(msg) {

		case WM_CREATE:  {
			//CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
			//dialog = (Dialog *)cs->lpCreateParams;
			st->bp = 1;
			break;
		}

		case WM_USER:
			//st->wm_user(wParam, lParam);					// editboxes send this message when tab or escape keys are hit
			//return 0;
			break;

		case WM_SIZE:  {
			GetClientRect(hwnd, &st->clientrect);
			//MoveWindow(hwnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			break;
		}

		case WM_CLOSE:				// close is called before wm_destroy!!
			st->bp = 1;
			break;

		case WM_DESTROY:  {
			//PostQuitMessage(0);
			//return 0L;
			st->bp = 1;
			break;
		}

		case WM_CHAR:  {
			switch (wParam)  {
				case VK_ESCAPE:
					//SendMessage(hwnd, WM_CLOSE, 0, 0);
					//return 0;
					st->bp = 1;
					break;
				default:
					break;
			}
			break;
		}

		/*
		case WM_KEYDOWN:  {
			switch(wParam)  {
				case VK_ESCAPE:
					//st->bp = 1;
					break;						// so wm_char will get the key
				default:
					//st->wm_keydown(wParam, lParam);					// subclass does it
					return 0;
			}
		}
		case WM_KEYUP:  {
			//st->wm_keyup(wParam, lParam);					// subclass does it
			return 0;
		}
		*/

		case WM_PAINT:  {
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);

			EndPaint (hwnd, &ps);

			break;
		}

		case WM_COMMAND:
			//st->wm_command(wParam, lParam);					// subclass does it
			st->bp = 1;
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);

}

/**********************************************************************

**********************************************************************/

void statusDlg::setText(char * _txt)  {
	text->setText(_txt);
	return;
}
