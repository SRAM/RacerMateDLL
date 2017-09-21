#ifndef _MODALDIALOG_H_
#define _MODALDIALOG_H_

#ifdef WIN32
	#include <windows.h>
#endif


#include <glib_defines.h>



/**********************************************************************************************************
	parent class for general-purpose modal dialogs
**********************************************************************************************************/

 class modalDialog  {

	public:
		modalDialog(HWND _phwnd, const char *_title, HICON _icon=0, int _x=0, int _y=0, int _w=0, int _h=0, bool _realtime=false, bool _autocenter=true);
		virtual ~modalDialog();
		bool getChanged(void)  { return changed; }
		virtual void mainloop(void);			// called by subclass or by creator!
		void repaint(void);

	protected:
		HWND phwnd;
		HWND hwnd;
		HDC hdc;
		int bp;
		RECT clientrect;
		bool keydown;
		HINSTANCE hInstance;
		COLORREF bgcolor;
		HICON icon;
		bool realtime;
		int cx, cy;

		// necessary interface for subclasses:

		/*
		virtual void wm_command(WPARAM wParam, LPARAM lParam)=0;
		virtual void wm_user(WPARAM wParam, LPARAM lParam)=0;
		virtual void wm_keydown(WPARAM wParam, LPARAM lParam)=0;
		virtual void wm_keyup(WPARAM wParam, LPARAM lParam)=0;
		*/
		virtual void wm_command(WPARAM wParam, LPARAM lParam) {
			bp = 1;
			return;
		}

		virtual int wm_close(WPARAM wp, LPARAM lp) {
			bp = 1;
			return 1;
		}

		virtual void wm_user(WPARAM wParam, LPARAM lParam) {
			bp = 1;
			return;
		}

		virtual void wm_keydown(WPARAM wParam, LPARAM lParam) {
			bp = 1;
			return;
		}

		virtual void wm_keyup(WPARAM wParam, LPARAM lParam) {
			bp = 1;
			return;
		}

		virtual void wm_char(WPARAM wparam, LPARAM lparam) {
			bp = 1;
			return;
		}

		virtual void wm_paint(WPARAM wParam, LPARAM lParam) {
			bp = 1;
			return;
		}

		bool changed;

	private:
		#define MY_DIALOG_CLASS "dlg_cls"
		//#define DIALOG_GRAY RGB(212, 208, 200)

		static LRESULT CALLBACK proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void cleanup(void);
		HBRUSH bgbrush;
		char title[32];
		int x, y, w, h;
};

#endif


