#ifndef _FILESELECTOR_H_
#define _FILESELECTOR_H_

#ifdef WIN32
	#include <windows.h>
	#include <stdlib.h>
	#include <commdlg.h>
	#include <glib_defines.h>

//	#include <config.h>

	#ifdef DOC
		extern "C" {
	#endif

	class __declspec(dllexport) fileSelector  {
		public:
			enum  {
				FS_SELECT,
				FS_OPEN,
				FS_SAVE
			};

		private:
			HWND hwnd;
			char title[256];
			char deftype[256];
			char filter[256];
			char curdir[256];
			char default_file[256];
			char fname[MAX_PATH];
			char filename[MAX_PATH];								// the individual file name without a path
			char path[MAX_PATH];										// fullpath stripped of file name, without trailing separator
			char fullpath[MAX_PATH];								// path + separator + file name
			char szFileTitle[MAX_FNAME+MAX_EXT];
			char datapath[_MAX_PATH];
			int idx;

			BOOL openflag;
			OPENFILENAME ofn;
			fileSelector(const fileSelector&);
			fileSelector &operator = (const fileSelector&);		// unimplemented

		public:

			fileSelector(
				HWND _hwnd, 
				char *_title, 
				char *_f, 
				const char *_path,			// default path
				char *_deftype,
				int _idx,				// 1-based filter index
				BOOL _openflag, 
				const char *_default_file);

			virtual ~fileSelector();
			const char * getfname();
			const char * getfilename();
			const char * getpath();
			const char * getfullpath();
			bool cancelled;

	};
//	}			// extern 'c'

#else
	//#include <limits.h>
	#include <gtk/gtk.h>
	#include <glib_defines.h>

class fileSelector  {
	public:
		enum  {
			SELECT,
			OPEN,
			SAVE
		};

	private:
		char title[256];
		int mode;
		char original_path[_MAX_PATH];
		char fname[MAX_PATH];
		char path[MAX_PATH];
		char fullpath[MAX_PATH];

		char curdir[256];

		char deftype[256];
		char filter[256];
		char default_file[256];
		//char szFileTitle[MAX_FNAME+MAX_EXT];
		//int idx;

		bool openflag;
		fileSelector(const fileSelector&);
		fileSelector &operator = (const fileSelector&);		// unimplemented
		GtkWindow *parent;

	public:

		fileSelector(const char *_title, int _mode, GtkWindow *_parent = NULL );
		virtual ~fileSelector();
		const char * getfname(void);
		const char * getpath(void);
		const char * getfullpath(void);
		bool cancelled;

};
#endif		// #ifdef WIN32
#endif		// #ifndef _FILESELECTOR_H_

