
#pragma warning(disable:4996)					// for vista strncpy_s
#pragma warning(disable:4786)

#include <windows.h>
#include <commctrl.h>

#include <glib_defines.h>
#include <fatalerror.h>

#include <modalListBox.h>


/********************************************************************

********************************************************************/

//modalListBox::modalListBox(HWND _phwnd, char *_title, char **items, int _x, int _y, int _w, int _h) : modalDialog(_phwnd, _title, _x, _y, _w, _h)  {
modalListBox::modalListBox(HWND _phwnd, char *_title, const std::vector<std::string> &str, int _x, int _y, int _w, int _h, bool _autocenter) : modalDialog(_phwnd, _title, 0, _x, _y, _w, _h, false, _autocenter)  {

	hlist = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				//NULL,
				"listbox",
				"",
				WS_CHILD |
				WS_VSCROLL |
				WS_HSCROLL |
				WS_VISIBLE
				,
				0, 0, clientrect.right, clientrect.bottom,			// r needs to be re-obtained after a listview window is created!!!
				hwnd,
				(HMENU)IDC_LISTBOX,
				hInstance,
				0);

	SetFocus(hlist);
	
	//GetClientRect(hwnd, &clientrect);

//	RECT r;
//	GetClientRect(hlist, &r);
//	MoveWindow(hlist, clientrect.left, clientrect.top, clientrect.right, clientrect.bottom, TRUE);

	//SendMessage(hlist, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0L) ;
	listhdc = GetDC(hlist);

	/*
	int i = 0;
	while(items[i])  {
		SendMessage(hlist, LB_ADDSTRING, NULL, (long)items[i++]);
	}
	*/

	int i = str.size();

	for(i=0; i<(int)str.size(); i++)  {
		SendMessage(hlist, LB_ADDSTRING, NULL, (long)str[i].c_str());
	}

	mainloop();							// run the parent's message loop

}

/********************************************************************

********************************************************************/

modalListBox::~modalListBox()  {

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

void modalListBox::wm_command(WPARAM wParam, LPARAM lParam)  {

	/*
	#ifdef _DEBUG
	WORD hw = HIWORD(wParam);
	#endif

	int id = LOWORD(wParam);

	switch(id)  {
		case OK_BUTTON:  {
			SetFocus(hwnd);
			break;
		}

		case CANCEL_BUTTON:
			SetFocus(hwnd);
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		default:  {
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

			break;
		}
	}
	*/



	return;
}

/********************************************************************

********************************************************************/

void modalListBox::wm_keydown(WPARAM wParam, LPARAM lParam)  {

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

void modalListBox::wm_keyup(WPARAM wParam, LPARAM lParam)  {

	keydown = false;

	return;
}

/********************************************************************
	an editBox has sent us a message if the tab key or escape key was
	hit while the focus was on that editBox.
********************************************************************/

void modalListBox::wm_user(WPARAM wParam, LPARAM lParam)  {
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

