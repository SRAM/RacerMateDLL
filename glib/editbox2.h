
#pragma once

#include <windows.h>

/******************************************************************************************
	old editbox from _f/lib/gen
	added to new glib for the fuelflow programmer update
******************************************************************************************/

	class editBox2  {

		private:
			HFONT font;
			RECT labelrect;
			RECT editrect;
			HINSTANCE hInstance;
			int bp;
			HWND phwnd;
			HPEN pen;
			char label[64];
			char data[256];
			int id;
			int labelx, y, labelw, h;
			int datax, dataw;
			void destroy(void);
			SIZE sz;
			float px, py, pw, ph;
			RECT pclientrect;						// parent client rect
			editBox2(const editBox2&);
			WNDPROC oldEditProc;
			static LRESULT CALLBACK editproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			static int instances;		// count of instances
			static int first_font_size;
			bool keydown;
			int usermsg;
			LONG save;
			int font_size;
			int x, w;

		public:
			class editBox2(HWND _phwnd, char *_label, float _px, float _py, float _pw, int _id, float _ph=0.0f, char *_data=NULL, int _usermsg=0);
			virtual ~editBox2();
			LRESULT wmctlcolorstatic(WPARAM wparam, LPARAM lparam);
			const char *getText(void);
			HWND hwndData;
			//HWND hwndLabel;
			void enable(void);
			void disable(void);
			void setText(char *str);
			void paint(HDC hdc);
			void resize(void);
			void size(RECT *);
	};


