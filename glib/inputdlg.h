
#ifndef _INPUTDLG_H_
#define _INPUTDLG_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <modaldialog.h>
#include <button.h>
//#include <editbox2.h>
//#include <editbox.h>


/***********************************************************************************

***********************************************************************************/

class inputDlg : public modalDialog  {

	public:
		inputDlg(HWND _phwnd=NULL, const char *title="title", const char *_label="Label", HICON _icon=0, int _x=100, int _y=100, int _w=400, int _h=400, const char *_default_text=NULL);
		virtual ~inputDlg();
		const char *get_text(void);
		inline bool get_cancelled(void)  { return cancelled; }
		inline void set_text(const char *_str)  { strncpy(data, _str, sizeof(data)-1); }

	private:
		virtual void wm_command(WPARAM wParam, LPARAM lParam);
		virtual void wm_keydown(WPARAM wParam, LPARAM lParam);
		virtual void wm_keyup(WPARAM wParam, LPARAM lParam);
		virtual void wm_user(WPARAM wParam, LPARAM lParam);
		virtual void wm_paint(WPARAM wParam, LPARAM lParam);
		void destroy(void);
		const char *default_text;

		enum  {
			OK_BUTTON = 1977,
			CANCEL_BUTTON,
			EB1
		};

		HWND hwndData;
		HWND hwndLabel;
		int labelx, labely, labelw, labelh;
		int datax, datay, dataw, datah;
		RECT labelrect;
		//RECT datarect;
		const char *label;
		char data[256];

		//HBRUSH brush, oldbrush;

		//editBox *eb;



		//editBox2 *eb;

//	eb[0] = new editBox(hwnd, "Fuel-Tron part Number (model_type):",	.05f, .18f, .250f, EB1, .03f, "eb1");
//	eb[1] = new editBox(hwnd, "Serial number (model_serial):",			.05f, .25f, .250f, EB2, .03f, "eb2");
//	eb[2] = new editBox(hwnd, "Model ID (model_id):",						.05f, .32f, .250f, EB3, .03f, "eb3");

		//Button **button;
		//RECT rb;							// rectangle occupied by the first radio button
		//void seticon(void);

		Button *ok_button;
		Button *cancel_button;

		bool cancelled;


};

#endif


