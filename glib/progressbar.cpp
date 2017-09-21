
#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
	#include <windows.h>
	#include <commctrl.h>
	#include <fatalError.h>
	#define ID_PRGB	45391
#else
	#include <utils.h>
#endif

#include <progressbar.h>



/*************************************************************************************

*************************************************************************************/

#ifdef WIN32
progressBar::progressBar(HWND _phwnd, COLORREF _bg, COLORREF _fg, int _min, int _max)  {
#else
progressBar::progressBar(COLORREF _bg, COLORREF _fg, int _min, int _max)  {
#endif

#ifdef WIN32
	phwnd = _phwnd;
#endif

	bg = _bg;
	fg = _fg;
	min = _min;
	max = _max;

	
//	if (!phwnd)  {
//		throw(fatalError(__FILE__, __LINE__));
//	}

	init();

	return;
}

/*************************************************************************************

*************************************************************************************/

#ifdef WIN32
progressBar::progressBar(const char *_classname, char *_windowname, COLORREF _bg, COLORREF _fg)  {
#else
progressBar::progressBar(const char *_windowname, COLORREF _bg, COLORREF _fg )  {
#endif


#ifdef WIN32
	strncpy(classname, _classname, sizeof(classname)-1);
#endif

	bg = _bg;
	fg = _fg;
	strncpy(windowname, _windowname, sizeof(windowname)-1);

#ifdef WIN32
	phwnd = FindWindow(classname, windowname);
	if (!phwnd)  {
		throw(fatalError(__FILE__, __LINE__));
	}
#endif

	min = 0;
	max = 100;

	init();

	return;
}

/*************************************************************************************

*************************************************************************************/
/*
progressBar::progressBar(HWND _phwnd, COLORREF _bg, COLORREF _fg)  {

	phwnd = _phwnd;
	bg = _bg;
	fg = _fg;

	if (!phwnd)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	min = 0;
	max = 100;

	init();

	return;
}
*/

/*************************************************************************************

*************************************************************************************/

progressBar::~progressBar(void)  {
#ifdef WIN32
   DestroyWindow(hwnd);
#else
	//printf("destructor\n");		// printed!
#endif
}


/*************************************************************************************

*************************************************************************************/

void progressBar::draw(int pos)  {
#ifdef WIN32
	LRESULT previousPosition;
	if (hwnd==NULL)  {
		return;
	}
	previousPosition = SendMessage(hwnd, PBM_SETPOS, pos, 0);

#else

#endif
	return;
}

/*************************************************************************************

*************************************************************************************/
#ifdef WIN32

void progressBar::init(void)  {
	long lResult;
	int x, y, w, h;
	RECT pwindow;
	RECT pclientrect;

	hInstance = GetModuleHandle(NULL);
	InitCommonControls(); 

//	if (!phwnd)  {
//		throw(fatalError(__FILE__, __LINE__));
//	}

	if (phwnd)  {
		GetWindowRect(phwnd, &pwindow);
		GetClientRect(phwnd, &pclientrect);
	}
	else  {
		pwindow.left = 100;
		pwindow.right = 350;
		pwindow.top = 100;
		pwindow.bottom = 130;
		pclientrect.top = pclientrect.bottom = 0;
	}

	if (pclientrect.bottom==pclientrect.top)  {
		x = pwindow.left;
		y = pwindow.top;
		w = pwindow.right - pwindow.left;
		h = pwindow.bottom - pwindow.top;
	}
	else  {
		x = (int) (.5 + pwindow.left + .33 * (pwindow.right - pwindow.left));
		y = (int) (.5 + pwindow.top + .485 * (pwindow.bottom - pwindow.top));
		w = (int) (.5 + .33 * (pwindow.right - pwindow.left));
		h = (int) (.5 + .03 * (pwindow.bottom - pwindow.top));
	}


   hwnd = CreateWindowEx(
					0, 
					PROGRESS_CLASS,
					(LPSTR) NULL, 
					//WS_CHILD | WS_VISIBLE | PBS_SMOOTH,			// could also set PBS_VERTICAL
					WS_POPUP | 
					WS_VISIBLE | 
					WS_DLGFRAME |
					WS_VISIBLE |
					//PBS_VERTICAL |
					PBS_SMOOTH,

					x, y, w, h,
					phwnd, 
					(HMENU) 0, 
					hInstance, 
					NULL); 

	lResult = SendMessage(hwnd, PBM_SETBARCOLOR, 0, fg );   
	lResult = SendMessage(hwnd, PBM_SETBKCOLOR,  0, bg );

	//---------------------------------------------------------------------------------
	// set the range. The default is 0 & 100, but the values can be from 0 to 65,535
	//---------------------------------------------------------------------------------

	//lResult = SendMessage(hwnd, PBM_SETRANGE,  0, (LPARAM) MAKELPARAM (0, 100) );
	lResult = SendMessage(hwnd, PBM_SETRANGE,  0,  (LPARAM) MAKELPARAM (min, max));
//	map(0.0f, (float)max, 0.0f, 100.0f, &m, &b);

	//---------------------------------------------------------------------------------
	// set the step
	// to be used with: SendMessage(hwnd, PBM_STEPIT, 0, 0);
	//---------------------------------------------------------------------------------

	int step = (int) (.5 + (max - min) / 100.0);

	lResult = SendMessage(hwnd, PBM_SETSTEP, step, 0);		// default step is 10

	return;
}

#else

void progressBar::init(void)  {
	gtk_init (NULL, NULL);
	bool titlebar = true;

	// allocate memory for the data that is passed to the callbacks

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	if (windowname[0])  {
		if (titlebar)  {
			gtk_window_set_title (GTK_WINDOW (window), windowname);
			gtk_window_set_resizable (GTK_WINDOW (window), TRUE);
			g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_progress), this );
		}
		else  {
			gtk_window_set_decorated((GtkWindow*) window, FALSE);				// turns off title bar
			gtk_progress_bar_set_text (GTK_PROGRESS_BAR (pbar), windowname);
			text = gtk_progress_bar_get_text (GTK_PROGRESS_BAR (pbar));
		}
	}
	else  {
	}


	gtk_container_set_border_width (GTK_CONTAINER (window), 0);

	vbox = gtk_vbox_new (FALSE, 5);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_widget_show (vbox);

	// Create a centering alignment object

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 5);
	gtk_widget_show(align);


	pbar = gtk_progress_bar_new();
	activity_mode = FALSE;

	gtk_container_add(GTK_CONTAINER(align), pbar);
	gtk_widget_show (pbar);

	//-----------------------------------------------------------------------
	// Add a timer callback to update the value of the progress bar
	//-----------------------------------------------------------------------

	timer = g_timeout_add(100, progress_timeout, this);

	gtk_window_set_default_size (GTK_WINDOW (window), 300, 60);

	gtk_widget_show (window);
	gtk_main ();

	return;
}

/*********************************************************************************************************
	clean up allocated memory and remove the timer
*********************************************************************************************************/

void progressBar::destroy_progress( GtkWidget *widget, gpointer data)  {
	progressBar *pb = (progressBar *) data;

	g_source_remove(pb->timer);
	pb->timer = 0;
	pb->window = NULL;
	gtk_main_quit ();

	return;
}										// destroy_progress()


/*********************************************************************************************************
	Update the value of the progress bar so that we get some movement
*********************************************************************************************************/

gboolean progressBar::progress_timeout( gpointer data )  {
	gdouble new_val;
	progressBar *pb;

	pb = (progressBar *) data;

	if (pb->activity_mode)  {
		gtk_progress_bar_pulse (GTK_PROGRESS_BAR (pb->pbar));
	}
	else  {
		new_val = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR (pb->pbar)) + 0.01;
		if (new_val > 1.0)  {
			g_source_remove(pb->timer);
			pb->timer = 0;
			pb->window = NULL;
			gtk_main_quit ();
			//printf("got to here\n");			// gets here!!!
			return FALSE;					// we'll never be called again
		}
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (pb->pbar), new_val);			// set the new value	}
	}

	return TRUE;			// as this is a timeout function, return TRUE so that it continues to get called
}

#endif

