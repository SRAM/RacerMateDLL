#ifndef _PROGRESSBAR_H_
#define _PROGRESSBAR_H_

#ifdef WIN32
	#include <windows.h>
#else
	#include <gtk/gtk.h>
	#include <glib_defines.h>
#endif


/***********************************************************************************************************************
	linux version from ..../c++/gtk+/main11.cpp
***********************************************************************************************************************/

class progressBar  {

	private:
		#define COLOUR_BAR    RGB(0,0,255)
		#define COLOUR_TEXT    RGB(255,255,255)

#ifdef WIN32
		HWND hwnd;
		HWND phwnd;
		HINSTANCE hInstance;
		char classname[64];
#else
		GtkWidget *window;
		GtkWidget *pbar;
		int timer;
		gboolean activity_mode;
		GtkWidget *align;
		GtkWidget *check;
		GtkWidget *vbox;
		const gchar *text;
		static void destroy_progress( GtkWidget *widget, gpointer data );
		static gboolean progress_timeout( gpointer data );
#endif

		COLORREF bg;
		COLORREF fg;
		char windowname[64];
		void init(void);
		int min;
		int max;
		progressBar(const progressBar&);

	public:
#ifdef WIN32
		progressBar(char *_classname, char *_windowname, COLORREF bg, COLORREF fg);
		progressBar(HWND _phwnd, COLORREF bg, COLORREF fg, int _min=0, int _max=100);
#else
		progressBar(const char *_windowname, COLORREF bg, COLORREF fg);
		progressBar(COLORREF bg, COLORREF fg, int _min=0, int _max=100);
#endif

		~progressBar(void);
		void draw(int);
};

#endif			// #ifndef _PROGRESSBAR_H_
