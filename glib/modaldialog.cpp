
#pragma warning(disable:4996)					// for vista strncpy_s

#include <windows.h>

#include <glib_defines.h>
#include <err.h>
#include <fatalerror.h>
#include <modalDialog.h>

#include <utils.h>



/*****************************************************************************

*****************************************************************************/

modalDialog::modalDialog(HWND _phwnd, const char *_title, HICON _icon, int _x, int _y, int _w, int _h, bool _realtime, bool _autocenter)  {

	phwnd = _phwnd;
	x = _x;
	y = _y;
	w = _w;
	h = _h;

	realtime = _realtime;

	keydown = false;
	changed = false;
	bgcolor = RGB(236, 233, 216);
	//bgcolor = RGB(128, 128, 128);

	icon = _icon;

	hInstance = GetModuleHandle(NULL);
	//bgbrush = CreateSolidBrush(DIALOG_GRAY);
	bgbrush = CreateSolidBrush(bgcolor);
	strncpy(title, _title, sizeof(title)-1);

	WNDCLASS wc = { 
						CS_HREDRAW | CS_VREDRAW,
						proc,
						0,
						0,
						hInstance,
						//LoadIcon(NULL, IDI_APPLICATION),
						//LoadIcon(NULL, icon);
						icon,
						LoadCursor(NULL, IDC_ARROW),
						bgbrush,
						NULL,
						MY_DIALOG_CLASS
	};

	if (!RegisterClass(&wc))  {
		if (!UnregisterClass (MY_DIALOG_CLASS, hInstance))  {
			Err *err = new Err();
			char *str = err->getString();
			delete err;
			throw(fatalError(__FILE__, __LINE__, "Can't unregister MY_DIALOG_CLASS"));
		}
		else  {
			if (!RegisterClass(&wc))  {
				throw(fatalError(__FILE__, __LINE__));
			}
		}
	}

	if (w==0 || h==0)  {
		int w2, h2;
		w2 = GetSystemMetrics (SM_CXFULLSCREEN);
		h2 = GetSystemMetrics (SM_CYFULLSCREEN);
		w2 = (int) (.5 + .5*w2);
		h2 = (int) (.5 + .5*h2);

		hwnd = CreateWindowEx(	
					0,
					MY_DIALOG_CLASS,
					title,
					WS_OVERLAPPED | 
					WS_SYSMENU | 
					//WS_THICKFRAME | 
					//WS_MINIMIZEBOX | 
					//WS_MAXIMIZEBOX
					WS_CAPTION
					,
					0,
					0,
					w2,
					h2,
					phwnd,
					NULL,								// No Menu
					hInstance,							// Instance
					this);
	}
	else  {
		hwnd = CreateWindowEx(	
					0,
					MY_DIALOG_CLASS,
					title,
					WS_OVERLAPPED | 
					WS_SYSMENU | 
					//WS_THICKFRAME | 
					//WS_MINIMIZEBOX | 
					//WS_MAXIMIZEBOX
					WS_CAPTION
					,
					x,
					y,
					w,
					h,
					phwnd,
					NULL,								// No Menu
					hInstance,							// Instance
					this);
	}


	if (hwnd==NULL)  {
		Err *err = new Err();
		MessageBox( NULL, err->getString(), "Error", MB_OK | MB_ICONINFORMATION );
		DEL(err);
		cleanup();
		return;
	}

	SetWindowLong (hwnd, GWL_USERDATA, (long)this);

	getClientRect(hwnd, &clientrect);
	cx = ROUND(clientrect.right / 2.0f);
	cy = ROUND(clientrect.bottom / 2.0f);

	hdc = GetDC(hwnd);

	if (_autocenter)  {
		center_window(hwnd);
	}

	ShowWindow(hwnd, SW_SHOWNORMAL);
//	ShowWindow(hwnd, SW_SHOWMAXIMIZED);

	if (phwnd != NULL)  {
		//ShowWindow(phwnd, SW_HIDE);
		EnableWindow(phwnd, FALSE);
	}

	return;			// subclass or creator will do further initialization and then call mainloop()

}

/*****************************************************************************

*****************************************************************************/

modalDialog::~modalDialog()  {
	cleanup();
}

/*****************************************************************************

*****************************************************************************/

void modalDialog::cleanup(void)  {

//	ReleaseDC(hwnd, hdc);

	if (bgbrush)  {
		DeleteObject(bgbrush);
		bgbrush = NULL;
	}

	if (phwnd != NULL)  {
		EnableWindow(phwnd, TRUE);
		//ShowWindow(phwnd, SW_SHOWNORMAL);
	}

	if (phwnd != NULL)  {
		SetFocus(phwnd);
	}
	return;
}

/**********************************************************************

**********************************************************************/

LRESULT CALLBACK modalDialog::proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)  {

	/*
#ifdef _DEBUG
	int bp;

	switch(msg)  {
		case WM_CREATE:								// 0x0001
		case WM_DESTROY:								// 0x0002
		case WM_MOVE:									// 0x0003
		case WM_SIZE:									// 0x0005
		case WM_ACTIVATE:								// 0x0006
		case WM_SETFOCUS:								// 0x0007
		case WM_KILLFOCUS:							// 0x0008
		case WM_GETTEXT:								// 0x000d
		case WM_PAINT:									// 0x000f
		case WM_CLOSE:									// 0x0010
		case WM_QUERYOPEN:							// 0x0013
		case WM_ERASEBKGND:							// 0x0014
		case WM_SHOWWINDOW:							// 0x0018
		case WM_ACTIVATEAPP:							// 0x001c
		case WM_SETCURSOR:							// 0x0020
		case WM_MOUSEACTIVATE:						// 0x0021
		case WM_GETMINMAXINFO:						// 0x0024
		case WM_DRAWITEM:								// 0x002b
		case WM_WINDOWPOSCHANGING:					// 0x0046
		case WM_WINDOWPOSCHANGED:					// 0x0047
		case WM_NOTIFY:								// 0x004e
		case WM_NOTIFYFORMAT:						// 0x0055
		case WM_CONTEXTMENU:							// 0x007b
		case WM_NCCREATE:								// 0x0081
		case WM_NCDESTROY:							// 0x0082
		case WM_NCCALCSIZE:							// 0x0083
		case WM_NCHITTEST:							// 0x0084
		case WM_SYNCPAINT:							// 0x0088
		case WM_NCPAINT:								// 0x0085
		case WM_NCACTIVATE:							// 0x0086
		case WM_NCMOUSEMOVE:							// 0x00a0
		case WM_NCLBUTTONDOWN:						// 0x00a1
		case WM_KEYFIRST:								// 0x0100
		case WM_KEYUP:									// 0x0101
		case WM_COMMAND:								// 0x0111
		case WM_SYSCOMMAND:							// 0x0112
		case WM_HSCROLL:								// 0x0114
		case WM_INITMENU:                      // 0x0116
		case WM_INITMENUPOPUP:						// 0x0117
		case WM_MENUSELECT:                    // 0x011f
		case WM_ENTERIDLE:							// 0x0121
		case WM_UNINITMENUPOPUP:					// 0x0125
		case 0x127:										// ???
		case 0x129:										// ???
		case WM_CTLCOLOREDIT:						// 0x0133
		case WM_CTLCOLORLISTBOX:					// 0x0134
		case WM_CTLCOLORBTN:							// 0x0135
		case WM_CTLCOLORSTATIC:						// 0x0138
		case WM_MOUSEFIRST:							// 0x0200
		case WM_LBUTTONDOWN:							// 0x0201
		case WM_LBUTTONUP:							// 0x0202
		case WM_RBUTTONDOWN:							// 0x0204
		case WM_RBUTTONUP:							// 0x0205
		case WM_RBUTTONDBLCLK:						// 0x0206
		case WM_PARENTNOTIFY:						// 0x0210
		case WM_ENTERMENULOOP:	               // 0x0211
		case WM_EXITMENULOOP:						// 0x0212
		case WM_CAPTURECHANGED:						// 0x0215
		case WM_MOVING:								// 0x0216
		case WM_ENTERSIZEMOVE:						// 0x0231
		case WM_EXITSIZEMOVE:						// 0x0232
			break;
		default:
			bp = 1;
			break;
	}
#endif
	*/

	modalDialog *dialog = (modalDialog *)GetWindowLong(hwnd, GWL_USERDATA);
	if (dialog==NULL)  {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

#ifdef _DEBUG
	const char *cptr = MessageName(msg);

	if ((unsigned int)dialog->phwnd==0xfeeefeee)  {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
#endif



	switch(msg) {

		case WM_CREATE:  {
			//CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
			//dialog = (Dialog *)cs->lpCreateParams;
			break;
		}

		case WM_USER:
			dialog->wm_user(wParam, lParam);					// editboxes send this message when tab or escape keys are hit
			return 0;

		case WM_SIZE:  {
			GetClientRect(hwnd, &dialog->clientrect);
			dialog->cx = ROUND(dialog->clientrect.right / 2.0f);
			dialog->cy = ROUND(dialog->clientrect.bottom / 2.0f);
			//MoveWindow(hwnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			break;
		}

		case WM_CLOSE:  {				// close is called before wm_destroy!!

			int status;
			status = dialog->wm_close(wParam, lParam);
			if (status==1)  {
				ReleaseDC(hwnd, dialog->hdc);
				dialog->hdc = 0;
			}
			else  {
				return 0L;
			}
			break;
		}

		case WM_DESTROY:  {
			PostQuitMessage(0);
			return 0L;
		}

		case WM_CHAR:  {
			switch (wParam)  {
				case VK_ESCAPE:
					dialog->wm_char(wParam, lParam);
					return 0;
				default:
					dialog->wm_char(wParam, lParam);
					break;
			}
			break;
		}

		case WM_KEYDOWN:  {
			switch(wParam)  {
				case VK_ESCAPE:
					dialog->bp = 1;
					break;						// so wm_char will get the key
				default:
					dialog->wm_keydown(wParam, lParam);					// subclass does it
					return 0;
			}
		}

		case WM_KEYUP:  {
			dialog->wm_keyup(wParam, lParam);					// subclass does it
			return 0;
		}

		case WM_PAINT:  {
			dialog->wm_paint(wParam, lParam);

			break;
		}

		case WM_COMMAND:
			dialog->wm_command(wParam, lParam);					// subclass does it
			return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);

}

/*****************************************************************************

*****************************************************************************/

void modalDialog::mainloop(void)  {
	BOOL bGotMsg;
	MSG  msg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while(WM_QUIT != msg.message)  {
		bGotMsg = GetMessage(&msg, NULL, 0U, 0U);
		if(bGotMsg)   {
			//if(0 == TranslateAccelerator(hwnd, hAccel, &msg))  {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//}
		}
	}						// mainloop

	return;
}

/*****************************************************************************

*****************************************************************************/

void modalDialog::repaint(void)  {
	repaintNow(hwnd);
	return;
}


