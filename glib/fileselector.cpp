
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#pragma warning(disable:4996)					// for vista strncpy_s

	#include <fileselector.h>

	#include <assert.h>
	#include <stdio.h>
	#include <utils.h>


	/**********************************************************************

	**********************************************************************/

	fileSelector::fileSelector(
			HWND _hwnd,
			char *_title,
			char *_filter,
			const char *_datapath,
			char *_deftype,
			int _idx,						// 1-based filter index
			BOOL _openflag,
			const char *_default_file)
				: hwnd(NULL)
			{


		int i, len;

		hwnd = _hwnd;

		strcpy(title, _title);
		strcpy(filter, _filter);
		strcpy(datapath, _datapath);
		idx = _idx;
		cancelled = false;


		if (_deftype)  {
			strcpy(deftype, _deftype);
		}
		else  {
			deftype[0] = 0;
		}


		openflag = _openflag;

		if (_default_file)  {
			strcpy(default_file, _default_file);
		}
		else  {
			default_file[0] = 0;
		}

		GetCurrentDirectory(255,curdir);

		if (!SetCurrentDirectory(datapath))  {
			//DWORD err = GetLastError();
			//int bp = 1;
		}

		assert(filter[strlen(filter)-1] == '^');		// need trailing '^'

		len = strlen(filter);

		for(i=0;i<len;i++)  {				// change '^' to 0
			if (filter[i]=='^')  {
				filter[i] = 0;
			}
		}

		if (default_file[0])  {
			strcpy(fname, default_file);
		}
		else  {
			sprintf(fname, "");				// default file name
		}

		strcpy(szFileTitle,"");

		memset(&ofn, 0, sizeof(ofn));


		/**********************************

		DWORD         lStructSize;
		HWND          hwndOwner;
		HINSTANCE     hInstance;
		LPCTSTR       lpstrFilter;
		LPTSTR        lpstrCustomFilter;
		DWORD         nMaxCustFilter;
		DWORD         nFilterIndex;
		LPTSTR        lpstrFile;
		DWORD         nMaxFile;
		LPTSTR        lpstrFileTitle;
		DWORD         nMaxFileTitle;
		LPCTSTR       lpstrInitialDir;
		LPCTSTR       lpstrTitle;
		DWORD         Flags;
		WORD          nFileOffset;
		WORD          nFileExtension;
		LPCTSTR       lpstrDefExt;
		DWORD         lCustData;
		LPOFNHOOKPROC lpfnHook;
		LPCTSTR       lpTemplateName;

		*********************************/





		ofn.lStructSize			= sizeof(ofn);
		ofn.hwndOwner				= hwnd;
		ofn.hInstance				= NULL;							// ignored
		ofn.lpstrFilter			= filter;
		ofn.lpstrCustomFilter	= NULL;
		ofn.nMaxCustFilter		= 0;								// ignored
		ofn.nFilterIndex			= idx;

		ofn.lpstrFile				= fname;							// entry: default file name
																			// exit: full path & file name
		ofn.nMaxFile				= sizeof(fname);

		ofn.lpstrFileTitle		= NULL;							// receives filename & extension without path
		ofn.nMaxFileTitle			= 0;								//_MAX_FNAME + _MAX_EXT;
	//	ofn.lpstrFileTitle		= szFileTitle;					// receives filename & extension without path
	//	ofn.nMaxFileTitle			= sizeof(szFileTitle);		//_MAX_FNAME + _MAX_EXT;

	//	ofn.lpstrInitialDir		= NULL;							// null => use curdir
		ofn.lpstrInitialDir		= datapath;

		ofn.lpstrTitle				= title;							// title in title bar
	//	ofn.Flags					= NULL;							// OFN_NOCHANGEDIR;
		ofn.Flags = OFN_OVERWRITEPROMPT |
						OFN_PATHMUSTEXIST |
						OFN_HIDEREADONLY |
						OFN_SHOWHELP;

		ofn.nFileOffset			= 0;
		ofn.nFileExtension		= 0;
		ofn.lpstrDefExt			= deftype;		// default extension appended to filename you enter

		ofn.lCustData				= 0;
		ofn.lpfnHook				= NULL;
		ofn.lpTemplateName		= NULL;

		//if (logg.loglevel==0)  {
		//	log_ofn(ofn);
		//}

		//SetCurrentDirectory(curdir);
		//logg.write(10,0,1,"set current directory to %s\n", curdir);


		if (openflag)  {
			/*
			 If lpstrInitialDir has the same value as was passed the first time the application used an Open or Save As dialog box, the path most recently selected by the user is used as the initial directory.
			 Otherwise, if lpstrFile contains a path, that path is the initial directory.
			 Otherwise, if lpstrInitialDir is not NULL, it specifies the initial directory.
			 If lpstrInitialDir is NULL and the current directory contains any files of the specified filter types, the initial directory is the current directory.
			 Otherwise, the initial directory is the personal files directory of the current user.
			 Otherwise, the initial directory is the Desktop folder.
			*/

			if(!GetOpenFileName(&ofn))  {
				// normally, just cancel button
				//logg.write(0,0,1,"GetOpenFileName error %ld\n", CommDlgExtendedError());
				SetCurrentDirectory(curdir);			// "cancel" button hit
				//return NULL;
				cancelled = true;
			}
			else  {
				int bp = 1;								// "open" button hit
			}
		}
		else  {
			//strcpy(title, "Save As");
			if (!GetSaveFileName(&ofn))  {
				// normally, just cancel button
				//logg.write(0,0,1,"GetSaveFileName error %ld\n", CommDlgExtendedError());
				SetCurrentDirectory(curdir);
				cancelled = true;
				//return NULL;
			}
		}


	#ifdef _DEBUG
		//int bp = 1;
	#endif
		//logg.write(0,0,0,"GetOpenFileName successful\n");

		//if (!exists(ofn.lpstrFile))  {
			//sprintf(gstring,"can't find %s",ofn.lpstrFile);
			//logg.write(0,0,1,"i%s\n", gstring);
			//MessageBox(ghwnd, gstring, "Info", MB_OK);
			//SetCurrentDirectory(curdir);
			//return NULL;
		//}
		//else  {
			//logg.write(0,0,1,"found input file\n");

			/*
			strcpy(fname, ofn.lpstrFile);
			if (stripflag)  {
				strip_path(fname);
			}
			*/

		//}

		BOOL bstat = SetCurrentDirectory(curdir);
		if (bstat==FALSE)  {
			//logg.write(10,0,1,"select_file error %ld\n", GetLastError());
		}

		//return fname;

		strcpy(path, fname);										// fname is the full path
		strcpy(fullpath, fname);
		strip_filename(path);									// fname (full path) without the filename, no trailing slash
		strcpy(filename, fname);
		strip_path(filename);

	}


	/**********************************************************************
		destructor
	**********************************************************************/


	fileSelector::~fileSelector()  {
		hwnd = NULL;
	}

	/**********************************************************************

	**********************************************************************/

	//char * fileSelector::getSelectedFileName(void)  {
	//	return	ofn.lpstrFile;
	//}

	/**********************************************************************

	**********************************************************************/

	const char * fileSelector::getfname(void)  {
		return fname;
	}

	/**********************************************************************

	**********************************************************************/

	const char * fileSelector::getfilename(void)  {
		return filename;
	}

	/**********************************************************************

	**********************************************************************/

	const char *fileSelector::getpath(void)  {
		return path;
	}

	/**********************************************************************

	**********************************************************************/

	const char * fileSelector::getfullpath(void)  {
		return fullpath;
	}

#else											// linux version:

	/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#ifndef STDIN_FILENO
	#define STDIN_FILENO 0
#endif
#include <stdarg.h>
	*/
	
	#include <fileselector.h>
	#include <string.h>
	#include <utils.h>


	/**********************************************************************

	**********************************************************************/

	fileSelector::fileSelector(const char *_title, int _mode, GtkWindow *_parent)  {
		//int i, len;
		GtkWidget *dialog;
		const char *cptr=0;

		strcpy(title, _title);
		mode = _mode;
		cancelled = false;

		GetCurrentDirectory(255, curdir);

		gtk_init (NULL, NULL);
		// http://library.gnome.org/devel/gtk/stable/GtkFileChooserDialog.html

		switch (mode)  {
			default:
			case OPEN:
			case SELECT:
				dialog = gtk_file_chooser_dialog_new (
								//"Select File",
								title,
								parent,
								GTK_FILE_CHOOSER_ACTION_OPEN,
								GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								GTK_STOCK_OPEN,  GTK_RESPONSE_ACCEPT,
								(char *)NULL
					);
				// GTK_WINDOW_TOPLEVEL
				//dialog.window = GTK_WINDOW_TOPLEVEL;
				if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)  {
					cptr = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
					strncpy(fullpath, cptr, sizeof(fname)-1);
					strcpy(fname, fullpath);
					strcpy(path, fullpath);
					strip_path(fname);
					strip_filename(path);

					if (mode==OPEN)  {
						//open_file (fullpath);				// what mode????
						//g_free (fullpath);
					}
				}
				else  {
					//strcpy(fullpath, "cancel hit");
					cancelled = true;
				}
				//printf("fullpath = %s\n", fullpath);
				break;

			case SAVE:  {
				bool user_edited_a_new_document = true;
				const char *default_folder_for_saving = ".";
				const char *filename_for_existing_document = "x.x";

				dialog = gtk_file_chooser_dialog_new (
								"Save File",
								parent,
								GTK_FILE_CHOOSER_ACTION_SAVE,
								GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
								(char *)NULL);

				gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

				if (user_edited_a_new_document)   {
					gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
					gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled document");
				}
				else  {
					gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), filename_for_existing_document);
				}

				if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)  {
					cptr = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
					strncpy(fname, cptr, sizeof(fname)-1);
					strcpy(fname, fullpath);
					strcpy(path, fullpath);
					strip_path(fname);
					strip_filename(path);
					//save_to_file (fname);
					//g_free (fname);
				}
				else  {
					//strcpy(fullpath, "cancel hit");
					cancelled = true;
				}
				break;
			}
		}

		gtk_widget_destroy (dialog);

		/*
		if (!SetCurrentDirectory(original_path))  {
			//DWORD err = GetLastError();
			//int bp = 1;
		}
		*/

	}


	/**********************************************************************
		destructor
	**********************************************************************/


	fileSelector::~fileSelector()  {
	}

	/**********************************************************************

	**********************************************************************/

	const char * fileSelector::getfname(void)  {
		return fname;
	}

	/**********************************************************************

	**********************************************************************/

	const char * fileSelector::getpath(void)  {
		return path;
	}

	/**********************************************************************

	**********************************************************************/

	const char * fileSelector::getfullpath(void)  {
		return fullpath;
	}

#endif				// #ifdef WIN32

