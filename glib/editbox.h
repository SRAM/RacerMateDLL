
#pragma once

#include <windows.h>
#include <glib_types.h>

#define DOLABELHWND
//#define INTERCEPT_LABEL
//#define DOFRECT


	class editBox  {

		private:
#ifdef DOFRECT
			FRECT flabelrect;
			FRECT fdatarect;
#else
			float p_y_start;
			float p_label_start;
			float p_label_end;
			float p_data_wid;

#endif
			HBRUSH bgbrush;
#ifdef _DEBUG
			int resizecalls;
#endif
			/*
			float plabelx;
			float plabely;
			float plabelw;
			float plabelh;

			float pdatax;
			float pdatay;
			float pdataw;
			float pdatah;
			*/

			HFONT font;
			RECT labelrect;
			RECT datarect;
			HINSTANCE hInstance;
			int bp;
			HWND phwnd;
			HPEN pen;
			char thelabel[64];
			char data[256];
			int id;

			int label_x, y, label_w;
			int data_h, label_h;
			int data_x, data_w;

			void destroy(void);
			SIZE sz;
			float px1, px2, py, pw;
			RECT pclientrect;						// parent client rect
			editBox(const editBox&);
			WNDPROC oldEditProc;
			static LRESULT CALLBACK editproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			static int instances;		// count of instances
			static int first_font_size;
			static float max_px;

			bool keydown;
			int usermsg;

#ifdef INTERCEPT_LABEL
			LONG savelabel;
			WNDPROC oldLabelProc;
			static LRESULT CALLBACK labelproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
			LONG savedata;
			int font_size;
			COLORREF label_fg_color;
			COLORREF label_bg_color;
			int charwid;
			int constructor;

		public:
			// constructor 1:
			editBox(
				HWND _phwnd, 
				const char *_label, 
				float _px1,
				float _px2,
				float _py, 
				float _pw,
				//float _ph,
				//int _font_size,
				COLORREF _label_text_fg_color,
				int _id, 
				const char *_data=NULL,										// default data, can also be used just for sizing edit box if you erase it before painting
				int _usermsg=0
				//COLORREF _label_text_bg_color = RGB(0xff, 0x40, 0x40)
				);

				// constructor 2:
				editBox(
					HWND _phwnd,
					const char *_thelabel,
					float _p_y_start,
					float _p_label_start,
					float _p_label_end,
					float _p_data_wid,
					float _dummy,											// to get a unique constructor
					COLORREF _label_fg_color,
					int _id, 
					COLORREF _label_bg_color = 0,
					const char *_data=NULL,									// default data, can also be used just for sizing edit box if you erase it before painting
					int _usermsg=0
				);									// EB17

			virtual ~editBox();
			static float get_max_px(void) { return max_px; }


			LRESULT wmctlcolorstatic(WPARAM wparam, LPARAM lparam);
			const char *getText(void);
			HWND hwndData;
#ifdef DOLABELHWND
			HWND hwndLabel;
#endif
			void enable(void);
			void disable(void);
			void setText(char *str);
			void paint(HDC hdc);
			void resize(void);
			void resize2(void);
			int getid(void)  { return id; }

	};


