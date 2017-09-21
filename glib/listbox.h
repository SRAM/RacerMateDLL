
#pragma once

#include <windows.h>

/***
		U N F I N I S H E D ! ! !!!!!
***/


	class listBox  {

		private:
			#define LBBGCOLOR RGB(100, 100, 100)
			#define LBTEXTCOLOR RGB(255, 255, 0)
			HINSTANCE hInstance;
			int bp;
			HWND phwnd;
			HWND hwnd;
			void destroy(void);
			RECT pclientrect;
			RECT pwinrect;
			HBRUSH hbrush;
			HDC hdc;
			HFONT hfont;
			int x, y, w, h;
			float px, py, pw, ph;			// percentages in parent clientrect
			int count;
			listBox(const listBox&);

		public:
			class listBox(HWND _phwnd, float _px, float _py, float _pw, float _ph);
			virtual ~listBox();
			LRESULT wmCtlColorListBox(LPARAM lParam, WPARAM wParam);
			void add(char *str);
			inline HWND gethwnd(void)  { return hwnd; }

	};


