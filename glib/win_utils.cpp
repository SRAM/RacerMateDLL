//#define WIN32_LEAN_AND_MEAN

/*********************************************************************

*********************************************************************/

#include <glib_defines.h>
#include <globals.h>
#include <crf.h>
//#include <md5.h>

	//#pragma warning(disable:4101 4786)
	#pragma warning(disable:4996)					// for vista strncpy_s

	#include <windows.h>
	#include <windowsx.h>
	
	#include <stdio.h>
	#include <math.h>
	#include <mmsystem.h>
	#include <malloc.h>
	#include <crtdbg.h>
	#include <assert.h>
	#include <shlwapi.h>
	#include <time.h>
	#include <float.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <io.h>
	#include <stdio.h>
	//#include <WinSock2.h>
	//#include <Ws2tcpip.h>

	#include <utils.h>
	#include <glib_types.h>
	//#include <xbase64/xbase64.h>
	#include <algorithm>
	//#include <axis.h>

	#ifndef WEBAPP
		#include <logger.h>
	#endif
	
	#include <fatalerror.h>
	#ifdef _DEBUG
		#include <err.h>
	#endif

//	#include <minmax.h>

//extern int loglevel;
//extern FILE *logstream;
//#include <string>

//MACHINE_CAPS gMachineCaps = {0};
MACHINE_CAPS gMachineCaps;

void bit_reverse(unsigned char *b);

/**********************************************************************

**********************************************************************/

#if 0		// commented out on 20050422, need to move to the xbase library
void dbase_to_csv(char *dbfname)  {
	int ii, nrows, ncols;
	int row = 0;
	char buf[256];
	int count = 0;
	char str[4096];
	FILE *outstream;
	char csvname[256];


	xbShort rc;
	xbXBase *xb = NULL;
	xbDbf *dbf = NULL;
	xbNdx *idx1 = NULL;

	xb = new xbXBase();
	dbf = new xbDbf(xb);

	if(( rc = dbf->OpenDatabase(dbfname)) != XB_NO_ERROR )  {
		printf("Can't Open Database\n");
		DEL(idx1);
		DEL(dbf);
		DEL(xb);
		return;
	}

	ncols = dbf->FieldCount();
	nrows = dbf->NoOfRecords();
	ii = dbf->PhysicalNoOfRecords();

	assert(ii==nrows);

	//--------------------------------------
	// loop through the records
	//--------------------------------------

	rc = dbf->GetFirstRecord(); 
	row = dbf->GetCurRecNo();
	assert(row==1);

	strcpy(csvname, dbfname);
	replace_extension(csvname, "csv");

	outstream = fopen(csvname, "wt");

	while( rc == XB_NO_ERROR ) {
		count++;
		row = dbf->GetCurRecNo();
		
		strcpy(str, "\x22");
	/****************************************************

Structure for database: E:\DBASE\CHECKS2.DBF
Number of data records:    6210
Date of last update   : 12/17/04
Field  Field Name  Type       Width    Dec    Index
    1  EDATE       Date           8               N
    2  TDATE       Date           8               N
    3  CHECKNUM    Character      8               N
    4  AMOUNT      Numeric        9      2        N
    5  WHO         Character     40               N
    6  PURPOSE     Character     40               N
    7  SDATE       Date           8               N
    8  CHECKED     Logical        1               N
    9  CODE        Character      1               N
   10  BALANCE     Numeric        9      2        N
** Total **                     133

	****************************************************/


		for(int j=0; j<ncols; j++)  {
			dbf->GetField(j, buf);								//	gets the jth column of the current record
			trimcrlf(buf);


			if (dbf->GetFieldType(j)=='D')  {							// fix y2k problem for the date fields
				// eg, 19041225
				if (myindex(buf, "1900", 0) != -1)  {
					replace(buf, "1900","2000", false);
				}
				else if (myindex(buf, "1901", 0) != -1)  {
					replace(buf, "1901","2001", false);
				}
				else if (myindex(buf, "1902", 0) != -1)  {
					replace(buf, "1902","2002", false);
				}
				else if (myindex(buf, "1903", 0) != -1)  {
					replace(buf, "1903","2003", false);
				}
				else if (myindex(buf, "1904", 0) != -1)  {
					replace(buf, "1904","2004", false);
				}
			}
			strcat(str, buf);
			if (j==ncols-1)  {
				strcat(str, "\x22");
			}
			else  {
				strcat(str, "\x22,\x22");
			}
		}

		#ifdef XB_MEMO_FIELDS
		#endif

		// write the csv string to a file
		fprintf(outstream, "%s\n", str);
		rc = dbf->GetNextRecord();
	}				// 	while(rc == XB_NO_ERROR)

	assert(count==nrows);

	fclose(outstream);
	dbf->CloseDatabase();							// Close database and associated indexes
	DEL(idx1);
	DEL(dbf);
	DEL(xb);

	return;
}

#endif


/**************************************************************************************************
	force the clientrect of hwnd to fit the RECT r
	note: r gets changed!
**************************************************************************************************/

void SetClientRect(HWND hwnd, RECT *r)  {
	int w,h;
#ifdef _DEBUG
	RECT tr;
	CopyRect(&tr, r);
#endif

	BOOL has_menu = GetMenu(hwnd) != NULL;
	AdjustWindowRectEx(r, GetWindowLong(hwnd,GWL_STYLE), has_menu, GetWindowLong(hwnd,GWL_EXSTYLE) );

	h = r->bottom - r->top;
	w = r->right - r->left;

	r->left -= r->left;
	r->right = r->left + w;
	r->top -= r->top;
	r->bottom = r->top + h;

	MoveWindow(hwnd, r->left, r->top, r->right - r->left, r->bottom, TRUE);

#ifdef _DEBUG
	RECT cr;
	GetClientRect(hwnd, &cr);
	int bp = 1;
	assert( (tr.bottom-tr.top)==(cr.bottom-cr.top));
	assert( (tr.right-tr.left)==(cr.right-cr.left));
#endif

	return;
}

/**********************************************************************************

**********************************************************************************/

//#ifdef UNICODE
WCHAR *to_wide(const char *str)  {

	static WCHAR wstr[256] = { 0 };			// tlm20080119
	//WCHAR wstr[256] = { 0 };
	int status;
	memset(wstr, 0, sizeof(wstr));

	status = MultiByteToWideChar(
			GetACP(), 
			MB_PRECOMPOSED, 
			str, 
			strlen(str), 
			wstr, 
			strlen(str)
	);

	return wstr;

}
//#else
//char * to_wide(const char *str)  {
//
//}
//#endif

/**********************************************************************************

**********************************************************************************/

void to_narrow(WCHAR *wstr, char *str, int len)  {
	int status;
	int i;

	i = wcslen(wstr);

	if (i>len)  {
		i = len;
	}

	status = WideCharToMultiByte(
					GetACP(),				// UINT CodePage,            // code page
					0,							// DWORD dwFlags,            // performance and mapping flags
					wstr,						// LPCWSTR lpWideCharStr,    // wide-character string
					wcslen(wstr),			// int cchWideChar,          // number of chars in string.
					str,						// LPSTR lpMultiByteStr,     // buffer for new string
					len,						// int cbMultiByte,          // size of buffer
					NULL,						// LPCSTR lpDefaultChar,     // default for unmappable chars
					NULL						// LPBOOL lpUsedDefaultChar  // set when default char used
	);

	str[i] = 0;

	return;
}

/***********************************************************************************
	convert char array to wchar_t array
***********************************************************************************/

void charToW(const char* cPtr, int cSize, wchar_t*wPtr)  {

	for (int i = 0; i < cSize; i++)
		wPtr[i] = wchar_t(cPtr[i]);

	return;
} 

/**********************************************************************

    Helper routine.  Takes second parameter as Ansi string, copies
     it to first parameter as wide character (16-bits / char) string,
     and returns integer number of wide characters (words) in string
     (including the trailing wide char NULL).
**********************************************************************/

int nCopyAnsiToWideChar (LPWORD lpWCStr, LPSTR lpAnsiIn)  {
  int cchAnsi = lstrlen(lpAnsiIn);

  return MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, lpAnsiIn, cchAnsi, (LPWSTR)lpWCStr, cchAnsi) + 1;
}


/***********************************************************************************

***********************************************************************************/

void repaintNow(HWND hwnd)  {
	if (hwnd==0)  {
		return;
	}
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
	return;
}

/**********************************************************************

**********************************************************************/

void delay(DWORD ms)  {
	DWORD start;

   start = timeGetTime();
   while(1)  {
   	if ((timeGetTime()-start)>ms)  {
      	break;
   	}
   }

   return;
}

/*********************************************************************

*********************************************************************/

void select_color(HWND hwnd, COLORREF &color, Ini *ini)  {
	BOOL B;
	CHOOSECOLOR cc;
	static COLORREF acrCustClr[16]; // array of custom colors 
	int bp = 0;
	char str[256];

	/*
	typedef struct {
		 DWORD lStructSize;
		 HWND hwndOwner;
		 HWND hInstance;
		 COLORREF rgbResult;
		 COLORREF *lpCustColors;
		 DWORD Flags;
		 LPARAM lCustData;
		 LPCCHOOKPROC lpfnHook;
		 LPCTSTR lpTemplateName;
	} CHOOSECOLOR, *LPCHOOSECOLOR;
	*/

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = color;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;
	//cc.Flags = CC_RGBINIT;
	//cc.Flags = CC_ANYCOLOR;

	B = ChooseColor(&cc);
	if (B)  {
		color = cc.rgbResult;
		if (ini)  {
			sprintf(str, "%d,%d,%d", GetRValue(color), GetGValue(color), GetBValue(color));
			ini->writeString("options", "fg color", str);
		}
	}
	else  {
		DWORD dw = CommDlgExtendedError();
		switch(dw)  {
			case CDERR_DIALOGFAILURE:
				bp = 2;
				break;
			case CDERR_FINDRESFAILURE:
				bp = 2;
				break;
			case CDERR_MEMLOCKFAILURE:
				bp = 2;
				break;
			case CDERR_INITIALIZATION:
				bp = 2;
				break;
			case CDERR_NOHINSTANCE:
				bp = 2;
				break;
			case CDERR_NOHOOK:
				bp = 2;
				break;
			case CDERR_LOADRESFAILURE:
				bp = 2;
				break;
			case CDERR_NOTEMPLATE:
				bp = 2;
				break;
			case CDERR_LOADSTRFAILURE:
				bp = 2;
				break;
			case CDERR_STRUCTSIZE:
				bp = 2;
				break;
			case CDERR_MEMALLOCFAILURE:
				bp = 2;
				break;
			default:
				bp = 3;			// canceled
				break;
		};
	}

	return;
}

/**********************************************************************

**********************************************************************/

SYSTEMTIME filetime(char *fname)  {
	WIN32_FIND_DATA fd;
	SYSTEMTIME x;

	HANDLE handle = FindFirstFile(fname, &fd);
	if (handle == INVALID_HANDLE_VALUE)  {
		//throw (fatalError(__FILE__, __LINE__));
		memset(&x, 0, sizeof(x));
		return x;
	}

	FILETIME *ft;
	FILETIME localtime;

	ft = &fd.ftLastWriteTime;
	FileTimeToLocalFileTime(
		ft,									//CONST FILETIME *lpFileTime,  // UTC file time to convert
		&localtime							//LPFILETIME lpLocalFileTime   // converted file time
	);

	FileTimeToSystemTime(
		&localtime,							// CONST FILETIME *lpFileTime,  // file time to convert
		&x										// LPSYSTEMTIME lpSystemTime    // receives system time
	);
	FindClose(handle);

	return x;
}



/**********************************************************************
	generates a file name based on the date and time. Eg, 05171535.log
**********************************************************************/

void generate_log_file_name(char *logfile)  {

//	if (keep_logs)  {
		SYSTEMTIME x;
		GetLocalTime(&x);
		sprintf(logfile,"%02d%02d%02d%02d.log",x.wMonth,x.wDay,x.wHour,x.wMinute);
//	}
//	else  {
//		strcpy(logfile,"log.log");
//	}

	return;
}



/**********************************************************************

**********************************************************************/

void rotate(float &x, float &y, float theta)  {

	x = (float) (x * cos(theta) - y * sin(theta));
	y = (float) (x * sin(theta) + y * cos(theta));

	return;
}


/**********************************************************************

**********************************************************************/

void circle(HDC hdc,int xc,int yc,int ir, bool filled)  {
	int left,right,top,bottom;

	/*
	left = xc - ir;
	right = xc + ir;
	top = yc - ir;
	bottom = yc + ir;
	*/


	if (filled)  {
		left = xc - ir;
		right = xc + ir + 1;
		top = yc - ir;
		bottom = yc + ir + 1;

		Ellipse(hdc,left,top,right,bottom);
	}

	else  {
		/*
		int i, j;

		for(i=yc-ir; i<=(yc+ir); i++)  {
			for(j=xc-ir; j<=(xc+ir); j++)  {
				LineTo
			}
		}
		*/

		Arc(
			hdc,
			xc-ir,	// x-coord of rectangle's upper-left corner
			yc-ir,	// y-coord of rectangle's upper-left corner

			xc+ir,	// x-coord of rectangle's lower-right corner
			yc+ir,	// y-coord of rectangle's lower-right corner

			xc+ir,	// x-coord of first radial ending point
			yc,		// y-coord of first radial ending point

			xc+ir,	// x-coord of second radial ending point
			yc			// y-coord of second radial ending point
		);
	}

}

/**********************************************************************

**********************************************************************/

void ellipse(HDC hdc,int xc,int yc,int irx, int iry, bool filled)  {
	int left,right,top,bottom;

	/*
	left = xc - ir;
	right = xc + ir;
	top = yc - ir;
	bottom = yc + ir;
	*/


	if (filled)  {
		left = xc - irx;
		right = xc + irx + 1;
		top = yc - iry;
		bottom = yc + iry + 1;

		Ellipse(hdc,left,top,right,bottom);
	}

	else  {
		/*
		int i, j;

		for(i=yc-ir; i<=(yc+ir); i++)  {
			for(j=xc-ir; j<=(xc+ir); j++)  {
				LineTo
			}
		}
		*/

		Arc(
			hdc,
			xc-irx,	// x-coord of rectangle's upper-left corner
			yc-iry,	// y-coord of rectangle's upper-left corner

			xc+irx,	// x-coord of rectangle's lower-right corner
			yc+iry,	// y-coord of rectangle's lower-right corner

			xc+irx,	// x-coord of first radial ending point
			yc,		// y-coord of first radial ending point

			xc+irx,	// x-coord of second radial ending point
			yc			// y-coord of second radial ending point
		);
	}

}


/*************************************************************************
	3 point moving average;
*************************************************************************/

float avgfps(float f)  {
//	static float farray[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	static float farray[3] = { 0.0f, 0.0f, 0.0f };
	static int k=0;
	float sum;
	int i;


	farray[k] = f;
	k = (k+1) % 3;

	sum = 0.0f;

	for(i=0;i<3;i++)  {
		sum += farray[i];
	}

	int itemp = (int) (sum/3.0f);			// round off to nearest integer


	return ((float) itemp);

}


/**********************************************************************
	centers top level windows only
**********************************************************************/

void center_window(HWND hw)  {
	RECT r;
	int w, h;

	GetWindowRect(hw,&r);			// get current position of window
	w = r.right - r.left;				// calc width
	h = r.bottom - r.top;				// calc height

	MoveWindow(
		hw,
		(GetSystemMetrics(SM_CXSCREEN)-w)/2,
		(GetSystemMetrics(SM_CYSCREEN)-h)/2,
		w,
		h,
		TRUE
	);

   return;
}


/**********************************************************************
	centers child windows only
**********************************************************************/
/*
void center_child_window(HWND hwnd)  {
	RECT r;
	int w, h;

	GetWindowRect(hwnd,&r);			// get current position of window
	w = r.right - r.left;			// calc width
	h = r.bottom - r.top;			// calc height

	MoveWindow(
		hwnd,
		(GetSystemMetrics(SM_CXSCREEN)-w)/2,
		(GetSystemMetrics(SM_CYSCREEN)-h)/2,
		w,
		h,
		TRUE
	);

   return;
}
*/

/**********************************************************************

**********************************************************************/

void TransparentStretchBlt(
			HDC hDC,							// dest
			LONG left,						// dest
			LONG top,						// dest
         LONG width,						// dest
			LONG height,					// dest

         HBITMAP hBitmap,				// source bitmap
         LONG bmLeft,					// source
			LONG bmTop,						// source
         LONG bmWidth,					// source
			LONG bmHeight,					// scource
         COLORREF colorMask)   {		// transparent color

	HDC hMemDC, hStretchDC, hMaskDC;
	HBITMAP hStretchBm, hMaskBm,
            hOldMemBm, hOldStretchBm, hOldMaskBm;
    COLORREF oldColor;

    // Create the memory DC's.
    hMemDC = CreateCompatibleDC(hDC);
    hStretchDC = CreateCompatibleDC(hDC);
    hMaskDC = CreateCompatibleDC(hDC);

    // Create the bitmaps needed for the memory DC's.
    hStretchBm = CreateCompatibleBitmap(hDC, width, height);
    hMaskBm = CreateBitmap(width, height, 1, 1, NULL);

    // Select the bitmaps into the memory DC's.
    hOldMemBm = (HBITMAP)SelectObject(hMemDC, hBitmap);
    hOldStretchBm = (HBITMAP)SelectObject(hStretchDC, hStretchBm);
    hOldMaskBm = (HBITMAP)SelectObject(hMaskDC, hMaskBm);

    // StretchBlt to a colored DC.
    StretchBlt(hStretchDC, 0, 0, width, height,
               hMemDC, bmLeft, bmTop, bmWidth, bmHeight, SRCCOPY);

    // BitBlt to a monochrome DC.
    oldColor = SetBkColor(hStretchDC, colorMask);
    BitBlt(hMaskDC, 0, 0, width, height,
           hStretchDC, 0, 0, SRCCOPY);
    SetBkColor(hStretchDC, oldColor);

    // Transparent BitBlt technique.
    BitBlt(hDC, left, top, width, height,
           hMaskDC, 0, 0, SRCAND);
    BitBlt(hDC, left, top, width, height,
           hStretchDC, 0, 0, SRCPAINT);

    // Restore the memory DC's.
    SelectObject(hMemDC, hOldMemBm);
    SelectObject(hStretchDC, hOldStretchBm);
    SelectObject(hMaskDC, hOldMaskBm);

    // Delete the allocated bitmaps.
    DeleteObject(hStretchBm);
    DeleteObject(hMaskBm);

    // Delete the allocated memory DC's.
    DeleteDC(hMemDC);
    DeleteDC(hStretchDC);
    DeleteDC(hMaskDC);
	 return;
}

/**********************************************************************

       GetMachineCaps

 DESCRIPTION:
       determine various performance parameters for current machine

**********************************************************************/

#ifndef WEBAPP

void GetMachineCaps(Logger *log) {
	SYSTEM_INFO info = {0};
	HKEY hPCI;
	MEMORYSTATUS memStat;

	memset(&gMachineCaps, 0, sizeof(MACHINE_CAPS));

	//----------------
	// processor type
	//----------------

	GetSystemInfo(&info);

	switch (info.dwProcessorType) {
		case PROCESSOR_INTEL_PENTIUM:
			gMachineCaps.processor = MCP_PENTIUM;
			log->write(10,0,0,"processor = pentium\n");
			break;

		case PROCESSOR_INTEL_486:
			gMachineCaps.processor = MCP_486;
			log->write(10,0,0,"processor = 486\n");
			break;

		case PROCESSOR_INTEL_386:
			gMachineCaps.processor = MCP_386;
			log->write(10,0,0,"processor = 386\n");
			break;

		default:
			gMachineCaps.processor = MCP_UNKNOWN;
			log->write(10,0,0,"processor = unknown\n");
			break;
	}

	//----------------------------
	// number of processors:
	//----------------------------

	log->write(10,0,0, "NUMBER OF PROCESSORS = %ld\n", info.dwNumberOfProcessors);

	//----------
	// bus type
	//----------

	if (RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			"Enum\\PCI",
			0,    // reserved
			KEY_READ,
			&hPCI) == ERROR_SUCCESS)  {

		gMachineCaps.bus = MCB_PCI;
		RegCloseKey(hPCI);
		log->write(10,0,0,"bus = pci\n");
	}
	else  {
		gMachineCaps.bus = MCB_ISA;
		log->write(10,0,0,"bus = isa\n");
	}

	//---------------
	// system memory
	//---------------

	GlobalMemoryStatus(&memStat);
	gMachineCaps.sysMemory = memStat.dwTotalPhys;

	log->write(10,0,0,"total system memory = %lu\n\n", gMachineCaps.sysMemory);

	return;
}

//}

/**********************************************************************

**********************************************************************/

void send(Logger *logg, Serial *port, char *str, int flush_flag)  {
	int i=0;

	logg->write(0,0,1,"\n****************************\n");
	logg->write(0,0,1,"send:   %s   (len = %d)\n",str,strlen(str));

	while(str[i])  {
		port->tx(str[i++]);
	}

	if (flush_flag)  {
		port->flush();
	}

	if (port->txasleep)  {
		SetEvent(port->txControlEvent);				// wake up transmitter thread
	}

	return;
}


/**********************************************************************
	waits for a result
   returns 0 if it is found, 1 otherwise.
**********************************************************************/


int expect(Logger *logg, char *str, Serial *port, DWORD timeout)  {
	//int status;
	int len,i=0;
	unsigned char c[8];
	DWORD start,end;
	DWORD charcount=0;
	int n;
//	BOOL expecting;


//	expecting = TRUE;

	len = strlen(str);

   logg->write(0,0,0,"\n\nexpect: %s (len=%d, timeout = %lu)\n",str,len,timeout);

	start = timeGetTime();

	while(1)  {
		//status = port->Receive(c,1,&n);
		n = port->rx((char *)c, 1);

//		if (!(status==0 && n==1))  {
		if (n!=1)  {
			end = timeGetTime();
         if ((end-start)>timeout)  {
				logg->write(0,0,0,"\nexpect timeout end =   %lu (%lu)",end,timeout);
      		logg->write(0,0,0,"\nexpect timeout start = %lu (%lu)\n",start,end-start);
            logg->write(0,0,0,"at timeout i = %d\n",i);
            logg->write(0,0,0,"at timeout char count = %lu\n",charcount);
				//expecting = FALSE;
				return 1;
         }
		}
      else  {
      	charcount++;
      	logg->write(0,0,0,"%c",c[0]);

			if (c[0]==str[i])  {
         	i++;
				if (i==len)  {
   		   	logg->write(0,0,0,"\nfound it in %lu ms.",timeGetTime()-start);
					//expecting = FALSE;
					return 0;
				}
			}
			else  {
				i = 0;
			}
		}
	}
}




#endif		// #ifndef WEBAPP



void getClientRect(HWND hwnd, RECT *clientrect)  {

	GetClientRect(hwnd, clientrect);

	HWND thwnd = FindWindow("Shell_TrayWnd", NULL);
	if (thwnd)  {			// If there is a taskbar, ie WIN95 then adjust clientrect
		RECT rectTaskBar;
		GetWindowRect(thwnd, &rectTaskBar);

		if ( (rectTaskBar.right - rectTaskBar.left) > (rectTaskBar.bottom - rectTaskBar.top))  {
			if (rectTaskBar.top <= 0)  {
				// TaskBar at the Top of Screen.
				clientrect->top = clientrect->top + rectTaskBar.bottom;
			}
			else  {
				// TaskBar at the Bottom of Screen
				clientrect->bottom = clientrect->bottom - (rectTaskBar.bottom - rectTaskBar.top);
			}
		}
		else  {
			// TaskBar is on the Left side of the Screen
			if (rectTaskBar.left <= 0)  {
				clientrect->left = clientrect->left + rectTaskBar.right;
			}
         else  { 
				// TaskBar is on the Right side of the Screen
				clientrect->right = clientrect->right - (rectTaskBar.right - rectTaskBar.left);
			}
		}
	}

	return;
}


/**********************************************************************

**********************************************************************/
#if 0
unsigned short getRandomNumber(void)  {
	BOOL bstatus;
	LARGE_INTEGER performanceCount;
	unsigned short seed;

	bstatus = QueryPerformanceCounter(&performanceCount);
	if (bstatus)  {
		seed = (unsigned short)performanceCount.u.LowPart & 0x0000ffff;

		/*
		bstatus = QueryPerformanceFrequency(&performanceFreq);
		if (bstatus)  {
		}
		else  {
		}
		*/
	}
	else  {
		srand(9829);
		seed = rand();				// see better random number gens in utils
		srand(seed);
		seed = rand();
	}

	return seed;
}
#endif



/**********************************************************************

**********************************************************************/

float fround(float number, int digits)  {
	float ans;		// note: this NEEDS to be float to work. I know we're dealing with doubles.
	double n, d;

	//double dtemp = .1f;		// this DOESN'T WORK!! (there is 'garbage' at about 10 decimal places

	d = floor(pow(10.0, digits));
	n = floor(number * d + .5);
	ans = (float) (n / d);
	return ans;

}

double dround(double value, int precision)  {
    double result, power;
    long ltemp;
    power = pow(10.0, (double) precision);
    result = (value * power + .5);
    ltemp = (long) result;
    result = ((double) ltemp) / power;
 
    return result;
}



/**********************************************************************
  BitmapToRegion :	Create a region from the "non-transparent" pixels of a bitmap

  cTransparentColor :	Color base for the "transparent" pixels (default is black)

  cTolerance :		Color tolerance for the "transparent" pixels.

  A pixel is assumed to be transparent if the value of each of its 3 components 
  (blue, green and red) is greater or equal to the corresponding value in cTransparentColor 
  and is lower or equal to the corresponding value in cTransparentColor + cTolerance.

**********************************************************************/

HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)  {
	HRGN hRgn = NULL;

	if (hBmp)	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)	{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
					sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					//bm.bmBitsPixel,
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;

					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));

					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up

					GlobalUnlock(hData);
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}	
	}

	return hRgn;
}
/**********************************************************************
	Helper routine.  Take an input pointer, return closest
	pointer that is aligned on a DWORD (4 byte) boundary.
**********************************************************************/

LPWORD lpwAlign(LPWORD lpIn)   {
	ULONG ul;
	ul = (ULONG) lpIn;
	ul +=3;
	ul >>=2;
	ul <<=2;
	return (LPWORD) ul;
}



/**********************************************************************

**********************************************************************/

double dlerp(double t, double x, double y)  {
	//return ( (1.0 - t) * x + t*y);
	return (x + t*(y-x));
}

/**********************************************************************

**********************************************************************/

void heapdump(void)  {
   _HEAPINFO hinfo;
   int heapstatus;
   hinfo._pentry = NULL;
	//FILE *stream = fopen("heap.txt", "a+t");
	char str[256];

   while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )  { 
		sprintf(str, "%6s block at %p of size %4.4X\n",
        ( hinfo._useflag == _USEDENTRY ? "USED" : "FREE" ),
          hinfo._pentry, 
			 hinfo._size 
		);
		//fprintf(stream, "%s", str);
		OutputDebugString(str);
   }

   switch(heapstatus)  {
		case _HEAPEMPTY:
			sprintf(str, "OK - empty heap\n" );
			break;
		case _HEAPEND:
			sprintf(str, "OK - end of heap\n" );
			break;
		case _HEAPBADPTR:
			sprintf(str, "ERROR - bad pointer to heap\n" );
			break;
		case _HEAPBADBEGIN:
			sprintf(str, "ERROR - bad start of heap\n" );
			break;
		case _HEAPBADNODE:
			sprintf(str, "ERROR - bad node in heap\n" );
			break;
   }
	OutputDebugString(str);

	//fclose(stream);
	return;

}



/************************************************************************************

************************************************************************************/

int irand(int min, int max)  {
	float f;
	int i;
	f = (float)rand() / RAND_MAX;			// 0.0 to 1.0
	i = (int) (.5 + min + f * (max - min));
	return i;
}

/************************************************************************************
	creates a random string of length 'len'
************************************************************************************/

void crand(char *str, int len)  {
	int i, j, flag;
	char c;

	for(i=0;i<len;i++)  {
		flag = rand() % 2;
		j = rand() % 26;
		if (flag==0)  {			// upper case
			c = 'A'+ j;
		}
		else  {					// lower case
			c = 'a' + j;
		}
		str[i] = c;
	}

	str[i] = 0;

#ifdef _DEBUG
	int bp = 1;
#endif

}

/************************************************************************************
	creates a random bool
************************************************************************************/

bool brand(void)  {
	int i = rand()%2;
	if (i==0)  {
		return false;
	}
	return true;
}

/**********************************************************************
	compares two doubles
**********************************************************************/
/*
bool deq(double f1, double f2, double tolerance)  {
	if (fabs(f1-f2) <= tolerance)  {
		return true;
	}
	return false;
}
*/



/**********************************************************************

  qsort floating point comparison

Return Value Description 
< 0 elem1 less than elem2 
0 elem1 equivalent to elem2 
> 0 elem1 greater than elem2 

**********************************************************************/

int fcompare( const void *p1, const void *p2 ) {
	float f1, f2;

	f1 = *(float *)p1;
	f2 = *(float *)p2;

   if (f1 < f2)  {
		return -1;
	}
	else if(f1 > f2)  {
		return 1;
	}
	else  {
		return 0;
	}

}

/**********************************************************************
	0x00bbggrr
**********************************************************************/

void SetRValue( COLORREF* pColor, BYTE value ) {
    *pColor = ( *pColor & 0x00FFFF00 ) | (COLORREF)value;
}

/**********************************************************************

**********************************************************************/

void SetGValue( COLORREF* pColor, BYTE value )  {
    *pColor = ( *pColor & 0x00FF00FF ) | ( (COLORREF)value << 8 );
}

/**********************************************************************

**********************************************************************/

void SetBValue( COLORREF* pColor, BYTE value )  {
    *pColor = ( *pColor & 0x0000FFFF ) | ( (COLORREF)value << 16 );
}


/***************************************************************************
	This routine place comments at the head of a section of debug output
***************************************************************************/

void OutputHeading(const char * explanation)  {
   _RPT1( _CRT_WARN, "\n\n%s:\n**************************************\
************************************\n", explanation );
}


/*************************************************************************
	prints _filename on the default printer
*************************************************************************/

int printfile(char *_filename, int _copies, bool _dialog, bool _cursorhidden, bool landscape, int _linesPerPage)  {
	char buf[256];
	char *cptr;
	int y = 0;
	int x = 100;		// left margin
	SIZE sz;
	HDC hdc;
	int xres, yres, status;
	int linesPerPage;
	int line;
	int h;
	int logpixelsy;

	//bool landscape = false;

	/*
	CURSORINFO pci;  // cursor information
	bool hidden = false;

	memset(&pci, 0, sizeof(pci));
	pci.cbSize = sizeof(pci);

	if (!GetCursorInfo(&pci))  {
		int bp = 1;
	}
	else  {
		if (pci.flags!=CURSOR_SHOWING)  {
			ShowCursor(TRUE);
			hidden = true;
		}
	}
	*/

	if (_cursorhidden)  {
		ShowCursor(TRUE);
	}


	FILE *stream = fopen(_filename, "rt");
	if (stream==NULL)  {
		if (_cursorhidden)  {
			ShowCursor(TRUE);
		}
		return 1;
	}

	//(((((((((((((((((((((((((((((((
#if 0
	bool printerSetup = true;
	if (printerSetup)  {
		BOOL b;
		int printer_xres;
		int printer_yres;
		PRINTDLG printdlg;
		HFONT printerfont;
		int printer_font_h;
		int printer_line_w;
		int printer_boldness;
		memset(&printdlg, 0, sizeof(PRINTDLG));
		printdlg.lStructSize = sizeof(PRINTDLG);
		printdlg.Flags = PD_RETURNDC;
		printdlg.Flags |= PD_PRINTSETUP;
		printdlg.Flags |= PD_ALLPAGES;
		printdlg.nFromPage = 1;
		printdlg.nToPage = 0xffff;
		printdlg.nMinPage = 1;
		printdlg.nMaxPage = 0xffff;
		printdlg.nCopies = 1;
		b = PrintDlg(&printdlg);
		if (b==FALSE)  {
			return 1;
		}
		printer_xres = GetDeviceCaps(printdlg.hDC, HORZRES);
		printer_yres = GetDeviceCaps(printdlg.hDC, VERTRES);
		printer_font_h = printer_yres / 80;
		printer_line_w = (int) (.5 + (printer_yres / 2400) );
		printer_font_h = (int) (.5 + printer_yres / 2400.0f) * 30;
		if (printer_yres >= 9800)  {
			printer_line_w = 3;
		}
		else if (printer_yres >= 4800)  {
			printer_line_w = 1;
		}
		else  {
			printer_line_w = 0;
		}
		printer_boldness = 140;
		printerfont = CreateFont(
								-printer_font_h,							// height of font
								//(int)(.75*font_h),			// average character width
								0,									// closest match
								0,									// angle of escapement
								0,									// base-line orientation angle
								printer_boldness,						// font weight								140
								TRUE,								// italic attribute option
								0,									// underline attribute option
								0,									// strikeout attribute option
								ANSI_CHARSET,					// character set identifier
								OUT_STRING_PRECIS,			// output precision
								CLIP_TT_ALWAYS,				// clipping precision
								//PROOF_QUALITY,				// output quality
								ANTIALIASED_QUALITY,
								FIXED_PITCH | FF_SWISS,		// pitch and family
								"Arial");						// typeface name
	}
	//))))))))))))))))))))))))))))))))
#endif		//#if 0

	PRINTDLG printdlg = { sizeof(PRINTDLG) };
	DOCINFO di = {sizeof (DOCINFO), TEXT("Printing Report") };

	//SetCursor(LoadCursor (NULL, IDC_WAIT)) ;

	printdlg.nCopies = _copies;

	if (_dialog)  {
		printdlg.Flags = PD_RETURNDC;
	}
	else  {
		printdlg.Flags = PD_RETURNDC | PD_RETURNDEFAULT;		// just use default printer, no dialog
	}

	if (!PrintDlg(&printdlg))  {
		// eg, cancel hit in print dialog
		//SetCursor(LoadCursor (NULL, IDC_ARROW)) ;
		fclose(stream);
		if (_cursorhidden)  {
			ShowCursor(FALSE);
		}
		return 1;
	}

	if (landscape)  {
		DEVMODE *dm = (DEVMODE *)GlobalLock(printdlg.hDevMode);
		dm->dmOrientation = DMORIENT_LANDSCAPE;
		ResetDC(printdlg.hDC, dm);
	}


	hdc = printdlg.hDC;

	if (!hdc)  {
		MessageBox (NULL, TEXT ("Cannot obtain Printer DC"), "Chart", MB_ICONEXCLAMATION | MB_OK) ;
		//SetCursor(LoadCursor (NULL, IDC_ARROW)) ;
		fclose(stream);
		if (_cursorhidden)  {
			ShowCursor(FALSE);
		}
		return 1;
	}

	// Get size of printable area of page

	xres = GetDeviceCaps(hdc, HORZRES);
	yres = GetDeviceCaps(hdc, VERTRES);			// 3074

	logpixelsy = GetDeviceCaps(hdc, LOGPIXELSY);

	if (landscape)  {
		h = 24;
		h = MulDiv(h, logpixelsy, 72);
		h = yres / 50;
	}
	else  {
		h = 24;
		h = MulDiv(h, logpixelsy, 72);
		h = yres / 68;								// 45
	}


	HFONT hfont = CreateFont(
				-h,								// height
				0,									// width
				0,									// escapement
				0,									// orientation
				400,									// 1, weight
				FALSE,							// italic flag
				0,									// underline flag
				0,									// strikeout flag
				ANSI_CHARSET,					// char set
				OUT_STRING_PRECIS,			// output precision
				0,									// clip precision
				PROOF_QUALITY,					// output quality
				FIXED_PITCH | FF_MODERN,	// pitch and family
				"Courier"						// need this for FIXED PITCH!!!
			);

	HFONT oldfont = (HFONT)SelectObject(hdc, hfont);

	status = StartDoc(hdc, &di);
	if (status <= 0)  {
		//SetCursor(LoadCursor (NULL, IDC_ARROW)) ;
		fclose(stream);
		if (_cursorhidden)  {
			ShowCursor(FALSE);
		}
		SelectObject(hdc, oldfont);
		DeleteObject(hfont);
		return 1;
	}

	//--------------------
	// print it:
	//--------------------

	strcpy(buf, "Abc");
	GetTextExtentPoint32(hdc, buf, strlen(buf), &sz);

	if (_linesPerPage==-1)  {
		linesPerPage = (int) (float)(yres / (float)sz.cy);
	}
	else  {
		linesPerPage = _linesPerPage;
	}

	line = 0;

	while(1)  {
		if (line==0)  {
			status = StartPage(hdc);
			if (status <= 0)  {
				//SetCursor(LoadCursor (NULL, IDC_ARROW)) ;
				fclose(stream);
				if (_cursorhidden)  {
					ShowCursor(TRUE);
				}
				SelectObject(hdc, oldfont);
				DeleteObject(hfont);
				return 1;
			}
		}

		cptr = fgets(buf, 255, stream);
		if (cptr==NULL)  {
			break;
		}

		trimcrlf(buf);

		if (buf[0]==0x0c)  {				// form feed?
			line = 0;
			y = -sz.cy;
			status = EndPage(hdc);
			if (status <= 0)  {
				fclose(stream);
				if (_cursorhidden)  {
					ShowCursor(TRUE);
				}
				SelectObject(hdc, oldfont);
				DeleteObject(hfont);
				return 1;
			}
			memcpy(&buf[0], &buf[1], strlen(buf));				// rotate left
		}

		y += sz.cy;
		if (buf[0]==0)  {
			int bp = 1;
			line++;
			continue;
		}

		TextOut(hdc, x, y, buf, strlen(buf));
		line++;

	
		/*
		//if (line==linesPerPage)  {
		if (buf[0]==0x0c)  {				// form feed?
			line = 0;
			y = 0;
			status = EndPage(hdc);
			if (status <= 0)  {
				fclose(stream);
				if (_cursorhidden)  {
					ShowCursor(TRUE);
				}
				SelectObject(hdc, oldfont);
				DeleteObject(hfont);
				return 1;
			}
		}
		else  {
			y += sz.cy;
		}
		*/

	}

	SelectObject(hdc, oldfont);
	DeleteObject(hfont);
	status = EndDoc(hdc);
	DeleteDC (hdc) ;

	//SetCursor(LoadCursor (NULL, IDC_ARROW)) ;

	fclose(stream);
	if (_cursorhidden)  {
		ShowCursor(FALSE);
	}

	return 0;
}

/*
Here is a snippet on how to implement the UNIX
"ps" command (which walks the process list)
under Win NT:

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winperf.h>
*/

/***************************************************
 ps - List process names and id's for Win NT 
                                             
               Ernesto L. Aparcedo (c) 12/96
The only piece missing is GetObjectCounterText which is
a "standard" routine to read the Title Database.  You can
find one in MSDN.   You can plug it right into my program. 

Enjoy,

Ernesto L. Aparcedo
nfsex@ccmail.ceco.com

***************************************************/

/*
#define STARTSIZE    8192
#define STEPSIZE     1024

LPSTR *lpNamesArray;
void GetObjectCounterText();

void ps(void)  {
   PPERF_DATA_BLOCK PerfData = NULL;
   PPERF_OBJECT_TYPE PerfObj;
   PPERF_INSTANCE_DEFINITION PerfInst;
   PPERF_COUNTER_DEFINITION  PerfCounter;
   DWORD i,j,BufferSize = STARTSIZE;
   LONG  k;

// Allocate storage for Title Database
   while (RegQueryValueEx( HKEY_PERFORMANCE_DATA,
                         "Global",
                         NULL,
                         NULL,
                         (LPBYTE) PerfData,
                         &BufferSize ) == ERROR_MORE_DATA)
    PerfData = (PPERF_DATA_BLOCK) realloc (PerfData,BufferSize+=STEPSIZE);

   RegCloseKey (HKEY_PERFORMANCE_DATA);
   
   
// Read Title Database
   GetObjectCounterText();
   
// Read process objects and ids 
   for (i=0,PerfObj  = (PPERF_OBJECT_TYPE) ((PBYTE) PerfData +
PerfData->HeaderLength);
        i<PerfData->NumObjectTypes;
  i++,PerfObj  = (PPERF_OBJECT_TYPE) ((PBYTE) PerfObj  +
PerfObj->TotalByteLength))
     if (!strcmp(lpNamesArray[PerfObj->ObjectNameTitleIndex],"Process")) {
      for (j=0,PerfCounter  = (PPERF_COUNTER_DEFINITION) ((PBYTE) PerfObj +
PerfObj->HeaderLength);
           j<PerfObj->NumCounters;
                 j++,PerfCounter  = (PPERF_COUNTER_DEFINITION) ((PBYTE)
PerfCounter + PerfCounter->ByteLength)) 
                 if
(!strcmp(lpNamesArray[PerfCounter->CounterNameTitleIndex],"ID Process"))
break;
   for (k=0,PerfInst  = (PPERF_INSTANCE_DEFINITION) ((PBYTE) PerfObj +
PerfObj->DefinitionLength);
           k<PerfObj->NumInstances;
                 k++,PerfInst  = (PPERF_INSTANCE_DEFINITION)
              ((PBYTE) PerfInst + PerfInst->ByteLength +
                     ((PPERF_COUNTER_BLOCK) ((PBYTE) PerfInst +
PerfInst->ByteLength))->ByteLength))
     printf ("%S (%Xh)\n",(char *) ((PBYTE) PerfInst+PerfInst->NameOffset),
          *(PDWORD) ((PBYTE)PerfInst + PerfInst->ByteLength +
PerfCounter->CounterOffset));
      break;
  }
}


*/


/*********************************************************************

I'm using some C code that was published in a magazine called
"Computer Language" Volume 6 No 9, September 1989. The article
is called Well-Tempered Linear Scales on p49. It works really
well in my graphing program for all the data that I've tried.
It also refers to the following references:

Dixon W.J. "The Choice of Origin and Scale for Graphs", Journal
of the ACM 12(2):259-261, Apr. 1965.

Lewart C.R. "Algorithms SCALE1, SCALE2, SCALE3 for Determination
of Scales on Computer-Generated Plots", Communications of the
ACM 16(10):639-640, Oct 1973.

Cleveland, W. S. _The Elements of Graphing Data_, Belmont,
Calif.:Wadsworth, 1985.

Giammo T. "A Mathematical Method for the Automatic Scaling
of a function", Journal of the ACM 11(1):79-83, Jan 1964.

If you can't find the article, or want a copy of the code,
please email me.

Andrew.

-----------------------

There is something about this in the Graphic Gems books, but as I recall
they are pretty minimal solutions.  This is a problem where you really
need to encode human knowledge, and the best way to do this is with
a table (otherwise you end up with a very complicated chain of conditional
expressions).  

The following is the C code I use to implement intelligent axis
choices.  You call this function with span = maxvalue - minvalue, and
the maximum number of ticks and numbers you want to see on the axis
(a measure of the display resolution).  The function returns a struct
containing the intervals at which to plot short, medium, and long ticks,
and to write numbers, and "scale" is the power of ten by which these 
integer values must be multiplied.  With this information, you should 
be able to quickly write a routine that chooses endpoint values and 
draws and labels the axis.  The ticks are chosen from a 1-2-5-10
set

*********************************************************************/

//typedef struct TICKSUMMARY {
//    int s, m, l, n;
//} ticksummary;


ticksummary linaxspec (double span, int maxtix, int maxnums, double* scale) {
	static ticksummary ltiksum[7][3] = {
		{ { 1,  0,  1,   1 }, { 2,  0,  2,   2 }, { 5,  0,  5,   5 } },
		{ { 1,  0,  1,   2 }, { 2,  0,  2,   2 }, { 5,  0,  5,   5 } },
		{ { 1,  0,  5,   5 }, { 2,  0, 10,  10 }, { 5,  0,  5,   5 } },
		{ { 1,  5, 10,  10 }, { 2,  0, 10,  10 }, { 5,  0, 10,  10 } },
		{ { 1,  5, 10,  20 }, { 2,  0, 10,  20 }, { 5,  0, 10,  20 } },
		{ { 1,  5, 10,  50 }, { 2, 10, 50,  50 }, { 5, 10, 50,  50 } },
		{ { 1, 10, 50, 100 }, { 2, 10, 50, 100 }, { 5, 10, 50, 100 } } 
	};

	int tickindx, numindx;
	//real normspan;
	//real charact = floor (log10 (span / maxtix) + 0.3);
	double normspan;
	double charact = floor (log10 (span / maxtix) + 0.3);
	//*scale = exp10 (charact);
	*scale = pow(10, charact);

	normspan = span / *scale;
	tickindx = (int) ceil (3 * log10 (normspan / maxtix ));

	if (tickindx > 2)  {
		charact += 1.0;
		*scale *= 10.0;
		normspan = span / *scale;
		tickindx = (int) ceil (3 * log10 (normspan / maxtix ));
	}

	numindx = (int) ceil (3 * log10 (normspan / maxnums));
	numindx = (numindx > 6) ? 6 : numindx;
	return ltiksum[numindx][tickindx];

}


/*

Here is a subroutine in QBASIC. I hope it will be useful. A complete program 
using this routine is available from Compuserve (Science/Math forum, file 
NLR.ZIP)

'------------------------------------------------------------------------
'                       Determine scale on coordinate axis
' -----------------------------------------------------------------------
' Input     : Z()    = array of values to be plotted
' -----------------------------------------------------------------------
' Output    : Z1, Z2 = bounds on axis  
'             DZ     = interval
' -----------------------------------------------------------------------
SUB AutoScale (Z() AS DOUBLE, Z1 AS DOUBLE, Z2 AS DOUBLE, DZ AS DOUBLE)

CONST MINSUB = 2, MAXSUB = 6 ' Minimal and maximal number of subdivisions

DIM I AS INTEGER, N AS INTEGER
DIM Zmin AS DOUBLE, Zmax AS DOUBLE, H AS DOUBLE, R AS DOUBLE

N = UBOUND(Z)                   ' Number of points

Zmin = Z(1): Zmax = Z(1)        ' Minimum and maximum of array Z
FOR I = 2 TO N
 IF Z(I) < Zmin THEN
  Zmin = Z(I)
 ELSEIF Z(I) > Zmax THEN
  Zmax = Z(I)
 END IF
NEXT I

H = Zmax - Zmin
R = INT(LOG(H) / LOG(10))
DZ = 10 ^ R

DO
 I = INT(H / DZ)
 IF I < MINSUB THEN DZ = .5 * DZ
 IF I > MAXSUB THEN DZ = 2 * DZ
LOOP UNTIL I >= MINSUB AND I <= MAXSUB

Z1 = DZ * INT(Zmin / DZ)
Z2 = DZ * INT(Zmax / DZ)
WHILE Z1 > Zmin: Z1 = Z1 - DZ: WEND
WHILE Z2 < Zmax: Z2 = Z2 + DZ: WEND
END SUB

Algorithm 463 from the "Collected Algorithms from CACM"
does exactly this for both linear and logarithmic scales.

I have used these routines in my own plotting systems
quite successfully.

There are 3 routines, each less than 100 lines of Fortran.

There's also one in the first volume of _Graphics Gems_.  See Paul
Heckbert's "Nice numbers of graph labels", p. 61.  There's also C
code starting on p. 657.

BTW, I just recently got GGIV and it's *great*!  I especially like the
fact that they've cleaned up the typography and layout considerably
and put the C code up with the algorithms rather than stuffing it into
the appendices.  Kudos to Paul Heckbert, Andrew Glassner, and the GG
team.

 */

 /*
 * Nice Numbers for Graph Labels
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * label.c: demonstrate nice graph labeling
 *
 * Paul Heckbert	2 Dec 88
 */

//#include <stdio.h>
//#include <math.h>
//#include "GraphicsGems.h"

double nicenum();

/*
// expt(a,n)=a^n for integer n

#ifdef POW_NOT_TRUSTWORTHY
	#error "POW_NOT_TRUSTWORTHY"
	//if roundoff errors in pow cause problems, use this:

	double expt(double a, int n)  {
		double x;
		x = 1.0;

		if (n>0)  {
			for (; n>0; n--)  {
				x *= a;
			}
		}
		else  {
			for (; n<0; n++)  {
				x /= a;
			}
		}

		return x;
	}

#else
	#define expt(a, n) pow(a, (double)(n))
#endif
*/


/*********************************************************************

	from graphics gems 1:

	loose_label: demonstrate loose labeling of data range from min to max.
	(tight method is similar)

	eg, 
		double min = 0.0;
		double max = 97.0;
		loose_label(min, max);

*********************************************************************/


void loose_label(double min, double max)  {
	#define XXXNTICK 5			// desired number of tick marks
	char format[6];
	//char temp[20];
	int nfrac;
	double d;							// tick mark spacing
	double graphmin, graphmax;		// graph range min and max
	double range, x;

	// we expect min != max

	range = nicenum(max-min, 0);
	d = nicenum(range/(XXXNTICK-1), 1);
	graphmin = floor(min/d)*d;
	graphmax = ceil(max/d)*d;

	//nfrac = MAX(-floor(log10(d)), 0);			// # of fractional digits to show
	// tlm: the above line could be:
	nfrac = MAX((int)-floor(log10(d)), 0);		// # of fractional digits to show

	sprintf(format, "%%.%df", nfrac);			// simplest axis labels

	//printf("graphmin = %g graphmax = %g increment = %g\n", graphmin, graphmax, d);

	for (x=graphmin; x< graphmax +.5*d; x+=d)  {
		//sprintf(temp, str, x);
		//printf("(%s)\n", temp);
	}
	return;
}

/*********************************************************************
	from graphics gems 1:

	nicenum: find a "nice" number approximately equal to x.
	Round the number if round=1, take ceiling if round=0
*********************************************************************/

double nicenum(double x, int round)  {
	int expv;				// exponent of x
	double f;				// fractional part of x
	double nf;				// nice, rounded fraction

	expv = (int) floor(log10(x));

	//f = x / expt(10.0, expv);		// between 1 and 10
	f = x / pow(10.0, expv);			// between 1 and 10

	if (round)  {
		if (f<1.5) 
			nf = 1.;
		else if (f<3.)
			nf = 2.;
		else if (f<7.) 
			nf = 5.;
		else 
			nf = 10.;
	}
	else  {
		if (f<=1.) 
			nf = 1.;
		else if (f<=2.) 
			nf = 2.;
		else if (f<=5.) 
			nf = 5.;
		else 
			nf = 10.;
	}

	//return nf*expt(10., expv);
	return nf*pow(10.0, expv);

}



/*********************************************************************

*********************************************************************/

int DeleteFolder(char *path)  {

	char fullPath[MAX_PATH+1];

	GetFullPathName(path, MAX_PATH, fullPath, NULL);

	if( !PathIsDirectory(fullPath) )  {
		return 1;				// NOT FOUND
	}

	int n = strlen(fullPath);
	fullPath[n+1] = 0;			// the file list needs to be doubly-terminated!!!

	SHFILEOPSTRUCT fo;

	fo.hwnd = 0;
	fo.wFunc = FO_DELETE;
	fo.pFrom = fullPath;
	fo.pTo = 0;
	fo.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
	fo.fAnyOperationsAborted = 0;
	fo.hNameMappings = 0;
	fo.lpszProgressTitle = 0;


	if( SHFileOperation(&fo) )  {
		#ifdef _DEBUG
#ifndef WEBAPP
			Err *err = new Err();
			DEL(err);
#endif
		#endif
		return 2;				// FAIL
	}

	return 0;					// OK

}

/*********************************************************************
	MessageName: this function returns the text name of the message.

	http://wiki.winehq.org/List_Of_Windows_Messages

*********************************************************************/

const char *MessageName(UINT msg) { 
	static char str[256];

	switch (msg) { 

		case WM_SYNCPAINT: return "WM_SYNCPAINT";			// 0x0088
		case WM_NULL: return "WM_NULL";
		case WM_CREATE: return "WM_CREATE";
    	case WM_DESTROY: return "WM_DESTROY";
    	case WM_MOVE: return "WM_MOVE";
		case WM_SIZE: return "WM_SIZE";
		case WM_ACTIVATE: return "WM_ACTIVATE";
		case WM_SETFOCUS: return "WM_SETFOCUS";
		case WM_KILLFOCUS: return "WM_KILLFOCUS";
		case WM_ENABLE: return "WM_ENABLE";
		case WM_SETREDRAW: return "WM_SETREDRAW";
		case WM_SETTEXT: return "WM_SETTEXT";
		case WM_GETTEXT: return "WM_GETTEXT";
		case WM_GETTEXTLENGTH: return "WM_GETTEXTLENGTH";
		case WM_PAINT: return "WM_PAINT";
		case WM_CLOSE: return "WM_CLOSE";
		case WM_QUERYENDSESSION: return "WM_QUERYENDSESSION";
		case WM_QUIT: return "WM_QUIT";
		case WM_QUERYOPEN: return "WM_QUERYOPEN";
		case WM_ERASEBKGND: return "WM_ERASEBKGND";
		case WM_SYSCOLORCHANGE: return "WM_SYSCOLORCHANGE";
		case WM_ENDSESSION: return "WM_ENDSESSION";
		case WM_SHOWWINDOW: return "WM_SHOWWINDOW";
		case WM_SETTINGCHANGE: return "WM_SETTINGCHANGE";
		case WM_DEVMODECHANGE: return "WM_DEVMODECHANGE";
		case WM_ACTIVATEAPP: return "WM_ACTIVATEAPP";
		case WM_FONTCHANGE: return "WM_FONTCHANGE";
		case WM_TIMECHANGE: return "WM_TIMECHANGE";
		case WM_CANCELMODE: return "WM_CANCELMODE";
		case WM_SETCURSOR: return "WM_SETCURSOR";
		case WM_MOUSEACTIVATE: return "WM_MOUSEACTIVATE";
		case WM_CHILDACTIVATE: return "WM_CHILDACTIVATE";
		case WM_QUEUESYNC: return "WM_QUEUESYNC";
		case WM_GETMINMAXINFO: return "WM_GETMINMAXINFO";
		case WM_PAINTICON: return "WM_PAINTICON";
		case WM_ICONERASEBKGND: return "WM_ICONERASEBKGND";
		case WM_NEXTDLGCTL: return "WM_NEXTDLGCTL";
		case WM_SPOOLERSTATUS: return "WM_SPOOLERSTATUS";
		case WM_DRAWITEM: return "WM_DRAWITEM";
		case WM_MEASUREITEM: return "WM_MEASUREITEM";
		case WM_DELETEITEM: return "WM_DELETEITEM";
		case WM_VKEYTOITEM: return "WM_VKEYTOITEM";
		case WM_CHARTOITEM: return "WM_CHARTOITEM";
		case WM_SETFONT: return "WM_SETFONT";
		case WM_GETFONT: return "WM_GETFONT";
		case WM_SETHOTKEY: return "WM_SETHOTKEY";
		case WM_GETHOTKEY: return "WM_GETHOTKEY";
		case WM_QUERYDRAGICON: return "WM_QUERYDRAGICON";
		case WM_COMPAREITEM: return "WM_COMPAREITEM";
		case WM_COMPACTING: return "WM_COMPACTING";
		case WM_COMMNOTIFY: return "WM_COMMNOTIFY";
		case WM_WINDOWPOSCHANGING: return "WM_WINDOWPOSCHANGING";
		case WM_WINDOWPOSCHANGED: return "WM_WINDOWPOSCHANGED";
		case WM_POWER: return "WM_POWER";
		case WM_COPYDATA: return "WM_COPYDATA";
		case WM_CANCELJOURNAL: return "WM_CANCELJOURNAL";
		case WM_NOTIFY: return "WM_NOTIFY";
		case WM_INPUTLANGCHANGEREQUEST: return "WM_INPUTLANGCHANGEREQUEST";
		case WM_INPUTLANGCHANGE: return "WM_INPUTLANGCHANGE";
		case WM_TCARD: return "WM_TCARD";
		case WM_HELP: return "WM_HELP";
		case WM_USERCHANGED: return "WM_USERCHANGED";
		case WM_NOTIFYFORMAT: return "WM_NOTIFYFORMAT";
		case WM_CONTEXTMENU: return "WM_CONTEXTMENU";
		case WM_STYLECHANGING: return "WM_STYLECHANGING";
		case WM_STYLECHANGED: return "WM_STYLECHANGED";
		case WM_DISPLAYCHANGE: return "WM_DISPLAYCHANGE";
		case WM_GETICON: return "WM_GETICON";
		case WM_SETICON: return "WM_SETICON";
		case WM_NCCREATE: return "WM_NCCREATE";
		case WM_NCDESTROY: return "WM_NCDESTROY";
		case WM_NCCALCSIZE: return "WM_NCCALCSIZE";
		case WM_NCHITTEST: return "WM_NCHITTEST";
		case WM_NCPAINT: return "WM_NCPAINT";
		case WM_NCACTIVATE: return "WM_NCACTIVATE";
		case WM_GETDLGCODE: return "WM_GETDLGCODE";
		case WM_NCMOUSEMOVE: return "WM_NCMOUSEMOVE";
		case WM_NCLBUTTONDOWN: return "WM_NCLBUTTONDOWN";
		case WM_NCLBUTTONUP: return "WM_NCLBUTTONUP";
		case WM_NCLBUTTONDBLCLK: return "WM_NCLBUTTONDBLCLK";
		case WM_NCRBUTTONDOWN: return "WM_NCRBUTTONDOWN";
		case WM_NCRBUTTONUP: return "WM_NCRBUTTONUP";
		case WM_NCRBUTTONDBLCLK: return "WM_NCRBUTTONDBLCLK";
		case WM_NCMBUTTONDOWN: return "WM_NCMBUTTONDOWN";
		case WM_NCMBUTTONUP: return "WM_NCMBUTTONUP";
		case WM_NCMBUTTONDBLCLK: return "WM_NCMBUTTONDBLCLK";
		case WM_KEYDOWN: return "WM_KEYDOWN";
		case WM_KEYUP: return "WM_KEYUP";
		case WM_CHAR: return "WM_CHAR";
		case WM_DEADCHAR: return "WM_DEADCHAR";
		case WM_SYSKEYDOWN: return "WM_SYSKEYDOWN";
		case WM_SYSKEYUP: return "WM_SYSKEYUP";
		case WM_SYSCHAR: return "WM_SYSCHAR";
		case WM_SYSDEADCHAR: return "WM_SYSDEADCHAR";
		case WM_IME_STARTCOMPOSITION: return "WM_IME_STARTCOMPOSITION";
		case WM_IME_ENDCOMPOSITION: return "WM_IME_ENDCOMPOSITION";
		case WM_IME_COMPOSITION: return "WM_IME_COMPOSITION";
		case WM_INITDIALOG: return "WM_INITDIALOG";
		case WM_COMMAND: return "WM_COMMAND";
		case WM_SYSCOMMAND: return "WM_SYSCOMMAND";
		case WM_TIMER: return "WM_TIMER";
		case WM_HSCROLL: return "WM_HSCROLL";
		case WM_VSCROLL: return "WM_VSCROLL";
		case WM_INITMENU: return "WM_INITMENU";
		case WM_INITMENUPOPUP: return "WM_INITMENUPOPUP";
		case WM_MENUSELECT: return "WM_MENUSELECT";
		case WM_MENUCHAR: return "WM_MENUCHAR";
		case WM_ENTERIDLE: return "WM_ENTERIDLE";
		case WM_CTLCOLORMSGBOX: return "WM_CTLCOLORMSGBOX";
		case WM_CTLCOLOREDIT: return "WM_CTLCOLOREDIT";
		case WM_CTLCOLORLISTBOX: return "WM_CTLCOLORLISTBOX";
		case WM_CTLCOLORBTN: return "WM_CTLCOLORBTN";
		case WM_CTLCOLORDLG: return "WM_CTLCOLORDLG";
		case WM_CTLCOLORSCROLLBAR: return "WM_CTLCOLORSCROLLBAR";
		case WM_CTLCOLORSTATIC: return "WM_CTLCOLORSTATIC";
		case WM_MOUSEMOVE: return "WM_MOUSEMOVE";
		case WM_LBUTTONDOWN: return "WM_LBUTTONDOWN";
		case WM_LBUTTONUP: return "WM_LBUTTONUP";
		case WM_LBUTTONDBLCLK: return "WM_LBUTTONDBLCLK";
		case WM_RBUTTONDOWN: return "WM_RBUTTONDOWN";
		case WM_RBUTTONUP: return "WM_RBUTTONUP";
		case WM_RBUTTONDBLCLK: return "WM_RBUTTONDBLCLK";
		case WM_MBUTTONDOWN: return "WM_MBUTTONDOWN";
		case WM_MBUTTONUP: return "WM_MBUTTONUP";
		case WM_MBUTTONDBLCLK: return "WM_MBUTTONDBLCLK";
		//case WM_MOUSEWHEEL: return "WM_MOUSEWHEEL";
		case WM_PARENTNOTIFY: return "WM_PARENTNOTIFY";
		case WM_ENTERMENULOOP: return "WM_ENTERMENULOOP";
		case WM_EXITMENULOOP: return "WM_EXITMENULOOP";
		case WM_NEXTMENU: return "WM_NEXTMENU";
		case WM_SIZING: return "WM_SIZING";
		case WM_CAPTURECHANGED: return "WM_CAPTURECHANGED";
		case WM_MOVING: return "WM_MOVING";
		case WM_POWERBROADCAST: return "WM_POWERBROADCAST";
		case WM_DEVICECHANGE: return "WM_DEVICECHANGE";
		case WM_IME_SETCONTEXT: return "WM_IME_SETCONTEXT";
		case WM_IME_NOTIFY: return "WM_IME_NOTIFY";
		case WM_IME_CONTROL: return "WM_IME_CONTROL";
		case WM_IME_COMPOSITIONFULL: return "WM_IME_COMPOSITIONFULL";
		case WM_IME_SELECT: return "WM_IME_SELECT";
		case WM_IME_CHAR: return "WM_IME_CHAR";
		case WM_IME_KEYDOWN: return "WM_IME_KEYDOWN";
		case WM_IME_KEYUP: return "WM_IME_KEYUP";
		case WM_MDICREATE: return "WM_MDICREATE";
		case WM_MDIDESTROY: return "WM_MDIDESTROY";
		case WM_MDIACTIVATE: return "WM_MDIACTIVATE";
		case WM_MDIRESTORE: return "WM_MDIRESTORE";
		case WM_MDINEXT: return "WM_MDINEXT";
		case WM_MDIMAXIMIZE: return "WM_MDIMAXIMIZE";
		case WM_MDITILE: return "WM_MDITILE";
		case WM_MDICASCADE: return "WM_MDICASCADE";
		case WM_MDIICONARRANGE: return "WM_MDIICONARRANGE";
		case WM_MDIGETACTIVE: return "WM_MDIGETACTIVE";
		case WM_MDISETMENU: return "WM_MDISETMENU";
		case WM_ENTERSIZEMOVE: return "WM_ENTERSIZEMOVE";
		case WM_EXITSIZEMOVE: return "WM_EXITSIZEMOVE";
		case WM_DROPFILES: return "WM_DROPFILES";
		case WM_MDIREFRESHMENU: return "WM_MDIREFRESHMENU";
		//case WM_MOUSEHOVER: return "WM_MOUSEHOVER";
		//case WM_MOUSELEAVE: return "WM_MOUSELEAVE";
		case WM_CUT: return "WM_CUT";
		case WM_COPY: return "WM_COPY";
		case WM_PASTE: return "WM_PASTE";
		case WM_CLEAR: return "WM_CLEAR";
		case WM_UNDO: return "WM_UNDO";
		case WM_RENDERFORMAT: return "WM_RENDERFORMAT";
		case WM_RENDERALLFORMATS: return "WM_RENDERALLFORMATS";
		case WM_DESTROYCLIPBOARD: return "WM_DESTROYCLIPBOARD";
		case WM_DRAWCLIPBOARD: return "WM_DRAWCLIPBOARD";
		case WM_PAINTCLIPBOARD: return "WM_PAINTCLIPBOARD";
		case WM_VSCROLLCLIPBOARD: return "WM_VSCROLLCLIPBOARD";
		case WM_SIZECLIPBOARD: return "WM_SIZECLIPBOARD";
		case WM_ASKCBFORMATNAME: return "WM_ASKCBFORMATNAME";
		case WM_CHANGECBCHAIN: return "WM_CHANGECBCHAIN";
		case WM_HSCROLLCLIPBOARD: return "WM_HSCROLLCLIPBOARD";
		case WM_QUERYNEWPALETTE: return "WM_QUERYNEWPALETTE";
		case WM_PALETTEISCHANGING: return "WM_PALETTEISCHANGING";
		case WM_PALETTECHANGED: return "WM_PALETTECHANGED";
		case WM_HOTKEY: return "WM_HOTKEY";
		case WM_PRINT: return "WM_PRINT";
		case WM_PRINTCLIENT: return "WM_PRINTCLIENT";
		case WM_HANDHELDFIRST: return "WM_HANDHELDFIRST";
		case WM_HANDHELDLAST: return "WM_HANDHELDLAST";
		case WM_AFXFIRST: return "WM_AFXFIRST";
		case WM_AFXLAST: return "WM_AFXLAST";
		case WM_PENWINFIRST: return "WM_PENWINFIRST";
		case WM_PENWINLAST: return "WM_PENWINLAST";
		case WM_NCMOUSELEAVE: return "WM_NCMOUSELEAVE";
		case WM_USER: return "WM_USER";
		case WM_USER+1: return "WM_USER+1";
		case WM_USER+2: return "WM_USER+2";
		case WM_USER+3: return "WM_USER+3";
		case WM_USER+4: return "WM_USER+4";

#if(WINVER >= 0x0600)
		case WM_CHANGEUISTATE: return "WM_CHANGEUISTATE";
		case WM_DWMNCRENDERINGCHANGED: return "WM_DWMNCRENDERINGCHANGED";
#endif


#ifdef __VCL
		case CM_ACTIVATE: return "CM_ACTIVATE";
		case CM_DEACTIVATE: return "CM_DEACTIVATE";
		case CM_GOTFOCUS: return "CM_GOTFOCUS";
		case CM_LOSTFOCUS: return "CM_LOSTFOCUS";
		case CM_CANCELMODE: return "CM_CANCELMODE";
		case CM_DIALOGKEY: return "CM_DIALOGKEY";
		case CM_DIALOGCHAR: return "CM_DIALOGCHAR";
		case CM_FOCUSCHANGED: return "CM_FOCUSCHANGED";
		case CM_PARENTFONTCHANGED: return "CM_PARENTFONTCHANGED";
		case CM_PARENTCOLORCHANGED: return "CM_PARENTCOLORCHANGED";
		case CM_HITTEST: return "CM_HITTEST";
		case CM_VISIBLECHANGED: return "CM_VISIBLECHANGED";
		case CM_ENABLEDCHANGED: return "CM_ENABLEDCHANGED";
		case CM_COLORCHANGED: return "CM_COLORCHANGED";
		case CM_FONTCHANGED: return "CM_FONTCHANGED";
		case CM_CURSORCHANGED: return "CM_CURSORCHANGED";
		case CM_CTL3DCHANGED: return "CM_CTL3DCHANGED";
		case CM_PARENTCTL3DCHANGED: return "CM_PARENTCTL3DCHANGED";
		case CM_TEXTCHANGED: return "CM_TEXTCHANGED";
		case CM_MOUSEENTER: return "CM_MOUSEENTER";
		case CM_MOUSELEAVE: return "CM_MOUSELEAVE";
		case CM_MENUCHANGED: return "CM_MENUCHANGED";
		case CM_APPKEYDOWN: return "CM_APPKEYDOWN";
		case CM_APPSYSCOMMAND: return "CM_APPSYSCOMMAND";
		case CM_BUTTONPRESSED: return "CM_BUTTONPRESSED";
		case CM_SHOWINGCHANGED: return "CM_SHOWINGCHANGED";
		case CM_ENTER: return "CM_ENTER";
		case CM_EXIT: return "CM_EXIT";
		case CM_DESIGNHITTEST: return "CM_DESIGNHITTEST";
		case CM_ICONCHANGED: return "CM_ICONCHANGED";
		case CM_WANTSPECIALKEY: return "CM_WANTSPECIALKEY";
		case CM_INVOKEHELP: return "CM_INVOKEHELP";
		case CM_WINDOWHOOK: return "CM_WINDOWHOOK";
		case CM_RELEASE: return "CM_RELEASE";
		case CM_SHOWHINTCHANGED: return "CM_SHOWHINTCHANGED";
		case CM_PARENTSHOWHINTCHANGED: return "CM_PARENTSHOWHINTCHANGED";
		case CM_SYSCOLORCHANGE: return "CM_SYSCOLORCHANGE";
		case CM_WININICHANGE: return "CM_WININICHANGE";
		case CM_FONTCHANGE: return "CM_FONTCHANGE";
		case CM_TIMECHANGE: return "CM_TIMECHANGE";
		case CM_TABSTOPCHANGED: return "CM_TABSTOPCHANGED";
		case CM_UIACTIVATE: return "CM_UIACTIVATE";
		case CM_UIDEACTIVATE: return "CM_UIDEACTIVATE";
		case CM_DOCWINDOWACTIVATE: return "CM_DOCWINDOWACTIVATE";
		case CM_CONTROLLISTCHANGE: return "CM_CONTROLLISTCHANGE";
		case CM_GETDATALINK: return "CM_GETDATALINK";
		case CM_CHILDKEY: return "CM_CHILDKEY";
		case CM_DRAG: return "CM_DRAG";
		case CM_HINTSHOW: return "CM_HINTSHOW";
		case CM_DIALOGHANDLE: return "CM_DIALOGHANDLE";
		case CM_ISTOOLCONTROL: return "CM_ISTOOLCONTROL";
		case CM_RECREATEWND: return "CM_RECREATEWND";
		case CM_INVALIDATE: return "CM_INVALIDATE";
		case CM_SYSFONTCHANGED: return "CM_SYSFONTCHANGED";
		case CM_CONTROLCHANGE: return "CM_CONTROLCHANGE";
		case CM_CHANGED: return "CM_CHANGED";
		case CN_CHARTOITEM: return "CN_CHARTOITEM";
		case CN_COMMAND: return "CN_COMMAND";
		case CN_COMPAREITEM: return "CN_COMPAREITEM";
		case CN_CTLCOLORBTN: return "CN_CTLCOLORBTN";
		case CN_CTLCOLORDLG: return "CN_CTLCOLORDLG";
		case CN_CTLCOLOREDIT: return "CN_CTLCOLOREDIT";
		case CN_CTLCOLORLISTBOX: return "CN_CTLCOLORLISTBOX";
		case CN_CTLCOLORMSGBOX: return "CN_CTLCOLORMSGBOX";
		case CN_CTLCOLORSCROLLBAR: return "CN_CTLCOLORSCROLLBAR";
		case CN_CTLCOLORSTATIC: return "CN_CTLCOLORSTATIC";
		case CN_DELETEITEM: return "CN_DELETEITEM";
		case CN_DRAWITEM: return "CN_DRAWITEM";
		case CN_HSCROLL: return "CN_HSCROLL";
		case CN_MEASUREITEM: return "CN_MEASUREITEM";
		case CN_PARENTNOTIFY: return "CN_PARENTNOTIFY";
		case CN_VKEYTOITEM: return "CN_VKEYTOITEM";
		case CN_VSCROLL: return "CN_VSCROLL";
		case CN_KEYDOWN: return "CN_KEYDOWN";
		case CN_KEYUP: return "CN_KEYUP";
		case CN_CHAR: return "CN_CHAR";
		case CN_SYSKEYDOWN: return "CN_SYSKEYDOWN";
		case CN_SYSCHAR: return "CN_SYSCHAR";
		case CN_NOTIFY: return "CN_NOTIFY";
#endif
		default:
			sprintf(str, "UNKNOWN WINDOWS MESSAGE **** %lu  0x%lx", msg, msg);
			return str;

  }
}



/******************************************************************************************

******************************************************************************************/

int csv_add(char *buf, int buflen, int type, void *p, int _precision, bool _last)  {
	char format[32];
	int len;
	
	len = strlen(buf);
	if (len >= (buflen-1) )  {
		//throw(fatalError(__FILE__, __LINE__, "CSV Buffer Out Of Memory") );
		return 1;
	}

	switch(type)  {
		case CSV_STRING:								// char string
			if (_last)  {
				sprintf(&buf[len], "\x22%s\x22", (char *)p);
			}
			else  {
				sprintf(&buf[len], "\x22%s\x22,", (char *)p);
			}
			break;
		case CSV_FLOAT:  {							// float
			if (_last)  {
				sprintf(format, "\x22%%.%df\x22", _precision);
			}
			else  {
				sprintf(format, "\x22%%.%df\x22,", _precision);
			}
			float *f = (float *)p;
			float f2 = *f;
			sprintf(&buf[len], format, f2);
			break;
		}

		case CSV_INT:  {								// int
			int *i = (int *)p;
			int i2 = *i;
			if (_last)  {
				sprintf(&buf[len], "\x22%d\x22", i2);
			}
			else  {
				sprintf(&buf[len], "\x22%d\x22,", i2);
			}
			break;
		}

		default:
			//throw(fatalError(__FILE__, __LINE__));
			return 1;
	}

	return 0;
}


//#ifdef _DEBUG
#if 1

/******************************************************************************************

******************************************************************************************/

void checkMsg(UINT msg, FILE *stream)  {

	/*
	if (stream==NULL)  {
		char str[256];
		sprintf(str, ">>>>>>>> chkMsg: file closed, msg = 0x%04x <<<<<<<<<\n", msg);
		OutputDebugString(str);
		return;
	}
	*/

	switch(msg)  {
		case WM_CREATE:								// 0x0001
			if (stream) fprintf(stream, "WM_CREATE\n");
			break;
		case WM_DESTROY:								// 0x0002
			if (stream) fprintf(stream, "WM_DESTROY\n");
			break;
		case WM_MOVE:									// 0x0003
			if (stream) fprintf(stream, "WM_MOVE\n");
			break;
		case WM_SIZE:									// 0x0005
			if (stream) fprintf(stream, "WM_SIZE\n");
			break;
		case WM_ACTIVATE:								// 0x0006
			if (stream) fprintf(stream, "WM_ACTIVATE\n");
			break;
		case WM_SETFOCUS:								// 0x0007
			if (stream) fprintf(stream, "WM_SETFOCUS\n");
			break;
		case WM_KILLFOCUS:							// 0x0008
			if (stream) fprintf(stream, "WM_KILLFOCUS\n");
			break;
		case WM_GETTEXT:								// 0x000d
			if (stream) fprintf(stream, "WM_GETTEXT\n");
			break;
		case WM_PAINT:									// 0x000f
			if (stream) fprintf(stream, "WM_PAINT\n");
			break;
		case WM_CLOSE:									// 0x0010
			if (stream) fprintf(stream, "WM_CLOSE\n");
			break;
		case WM_QUERYOPEN:							// 0x0013
			if (stream) fprintf(stream, "WM_QUERYOPEN\n");
			break;
		case WM_ERASEBKGND:							// 0x0014
			if (stream) fprintf(stream, "WM_ERASEBKGND\n");
			break;
		case WM_SHOWWINDOW:							// 0x0018
			if (stream) fprintf(stream, "WM_SHOWWINDOW\n");
			break;
		case WM_ACTIVATEAPP:							// 0x001c
			if (stream) fprintf(stream, "WM_ACTIVATEAPP\n");
			break;
		case WM_SETCURSOR:							// 0x0020
			if (stream) fprintf(stream, "WM_SETCURSOR\n");
			break;
		case WM_MOUSEACTIVATE:						// 0x0021
			if (stream) fprintf(stream, "WM_\n");
			break;
		case WM_GETMINMAXINFO:						// 0x0024
			if (stream) fprintf(stream, "WM_GETMINMAXINFO\n");
			break;
		case WM_DRAWITEM:								// 0x002b
			if (stream) fprintf(stream, "WM_DRAWITEM\n");
			break;
		case WM_WINDOWPOSCHANGING:					// 0x0046
			if (stream) fprintf(stream, "WM_WINDOWPOSCHANGING\n");
			break;
		case WM_WINDOWPOSCHANGED:					// 0x0047
			if (stream) fprintf(stream, "WM_WINDOWPOSCHANGED\n");
			break;
		case WM_NOTIFY:								// 0x004e
			if (stream) fprintf(stream, "WM_NOTIFY\n");
			break;
		case WM_NOTIFYFORMAT:						// 0x0055
			if (stream) fprintf(stream, "WM_NOTIFIYFORMAT\n");
			break;
		case WM_CONTEXTMENU:							// 0x007b
			if (stream) fprintf(stream, "WM_CONTEXTMENU\n");
			break;
		case WM_GETICON:								// 0x007f
			if (stream) fprintf(stream, "WM_GETICON\n");
			break;

		case WM_NCCREATE:								// 0x0081
			if (stream) fprintf(stream, "WM_NCCREATE\n");
			break;
		case WM_NCDESTROY:							// 0x0082
			if (stream) fprintf(stream, "WM_NCDESTROY\n");
			break;
		case WM_NCCALCSIZE:							// 0x0083
			if (stream) fprintf(stream, "WM_NCCALCSIZE\n");
			break;
		case WM_NCHITTEST:							// 0x0084
			if (stream) fprintf(stream, "WM_NCHITTEST\n");
			break;
		case WM_SYNCPAINT:							// 0x0088
			if (stream) fprintf(stream, "WM_SYNCPAINT\n");
			break;
		case WM_NCPAINT:								// 0x0085
			if (stream) fprintf(stream, "WM_NCPAINT\n");
			break;
		case WM_NCACTIVATE:							// 0x0086
			if (stream) fprintf(stream, "WM_NCACTIVATE\n");
			break;
		case WM_NCMOUSEMOVE:							// 0x00a0
			if (stream) fprintf(stream, "WM_NCMOUSEMOVE\n");
			break;
		case WM_NCLBUTTONDOWN:						// 0x00a1
			if (stream) fprintf(stream, "WM_NCLBUTTONDOWN\n");
			break;
		case WM_KEYFIRST:								// 0x0100
			if (stream) fprintf(stream, "WM_KEYFIRST\n");
			break;
		case WM_KEYUP:									// 0x0101
			if (stream) fprintf(stream, "WM_KEYUP\n");
			break;
		case WM_COMMAND:								// 0x0111
			if (stream) fprintf(stream, "WM_COMMAND\n");
			break;
		case WM_SYSCOMMAND:							// 0x0112
			if (stream) fprintf(stream, "WM_SYSCOMMAND\n");
			break;
		case WM_HSCROLL:								// 0x0114
			if (stream) fprintf(stream, "WM_HSCROLL\n");
			break;
		case WM_INITMENU:                      // 0x0116
			if (stream) fprintf(stream, "WM_INITMENU\n");
			break;
		case WM_INITMENUPOPUP:						// 0x0117
			if (stream) fprintf(stream, "WM_INITMENUPOPUP\n");
			break;
		case WM_MENUSELECT:                    // 0x011f
			if (stream) fprintf(stream, "WM_MENUSELECT\n");
			break;
		case WM_ENTERIDLE:							// 0x0121
			if (stream) fprintf(stream, "WM_ENTERIDLE\n");
			break;
		case WM_UNINITMENUPOPUP:					// 0x0125
			if (stream) fprintf(stream, "WM_UNINITMENUPOPUP\n");
			break;
		case 0x127:										// ???
			if (stream) fprintf(stream, "WM_0X127\n");
			break;
		case 0x129:										// ???
			if (stream) fprintf(stream, "WM_0X129\n");
			break;
		/*
#define WM_CTLCOLORMSGBOX               0x0132
#define WM_CTLCOLOREDIT                 0x0133
#define WM_CTLCOLORLISTBOX              0x0134
#define WM_CTLCOLORBTN                  0x0135
#define WM_CTLCOLORDLG                  0x0136
#define WM_CTLCOLORSCROLLBAR            0x0137
#define WM_CTLCOLORSTATIC               0x0138
		*/
		case WM_CTLCOLOREDIT:						// 0x0133
			if (stream) fprintf(stream, "WM_CTLCOLOREDIT\n");
			break;
		case WM_CTLCOLORLISTBOX:             	// 0x0134
			if (stream) fprintf(stream, "WM_CTLCOLORLISTBOX\n");
			break;
		case WM_CTLCOLORBTN:							// 0x0135
			if (stream) fprintf(stream, "WM_CTLCOLORBTN\n");
			break;
		case WM_CTLCOLORSTATIC:						// 0x0138
			if (stream) fprintf(stream, "WM_CTLCOLORSTATIC\n");
			break;
		case WM_MOUSEFIRST:							// 0x0200
			if (stream) fprintf(stream, "WM_MOUSEFIRST\n");
			break;
		case WM_LBUTTONDOWN:							// 0x0201
			if (stream) fprintf(stream, "WM_LBUTTONDOWN\n");
			break;
		case WM_LBUTTONUP:							// 0x0202
			if (stream) fprintf(stream, "WM_LBUTTONUP\n");
			break;
		case WM_RBUTTONDOWN:							// 0x0204
			if (stream) fprintf(stream, "WM_RBUTTONDOWN\n");
			break;
		case WM_RBUTTONUP:							// 0x0205
			if (stream) fprintf(stream, "WM_RBUTTONUP\n");
			break;
		case WM_RBUTTONDBLCLK:						// 0x0206
			if (stream) fprintf(stream, "WM_RBUTTONDBLCLK\n");
			break;
		case WM_PARENTNOTIFY:						// 0x0210
			if (stream) fprintf(stream, "WM_PARENTNOTIFY\n");
			break;
		case WM_ENTERMENULOOP:	               // 0x0211
			if (stream) fprintf(stream, "WM_ENTERMENULOOP\n");
			break;
		case WM_EXITMENULOOP:						// 0x0212
			if (stream) fprintf(stream, "WM_EXITMENULOOP\n");
			break;
		case WM_CAPTURECHANGED:						// 0x0215
			if (stream) fprintf(stream, "WM_CAPTURECHANGED\n");
			break;
		case WM_MOVING:								// 0x0216
			if (stream) fprintf(stream, "WM_MOVING\n");
			break;
		case WM_ENTERSIZEMOVE:						// 0x0231
			if (stream) fprintf(stream, "WM_ENTERSIZEMOVE\n");
			break;
		case WM_EXITSIZEMOVE:						// 0x0232
			if (stream) fprintf(stream, "WM_EXITSIZEMOVE\n");
			break;

		default:  {
			if (stream) fprintf(stream, "default: 0x%04x\n", msg);
			break;
		}
	}

	return;
}


#endif

#if 0
/************************************************************************************

************************************************************************************/

int doof(const char *_fname)  {
	FILE *instream=NULL;
	FILE *outstream=NULL;
	int status;
	unsigned char c;
	#define NEW_R_NAME "ygrzdsl"
	CRF crf;

	crf.init();

	instream = fopen(_fname, "rb");
	if (instream==NULL)  {
		//cleanup();
		//throw (fatalError(__FILE__, __LINE__));
		return 1;
	}

	outstream = fopen(NEW_R_NAME, "wb");

	while(1)  {
		status = fgetc(instream);
		if (status == EOF)  {
			break;
		}
		c = (unsigned char) status;
		crf.doo(&c, 1);
		fputc(c, outstream);
	}

	fclose(instream);
	fclose(outstream);

	if (!rm(_fname))  {								// unlink the encrypted file
		//cleanup();
		//throw (fatalError(__FILE__, __LINE__));
		return 2;
	}

	if (rename(NEW_R_NAME, _fname))  {
		//cleanup();
		//throw (fatalError(__FILE__, __LINE__));
		return 3;
	}

	return 0;
}
#endif

/************************************************************************************

************************************************************************************/

//void get_temp_file_name(char *fname, int size)  {
void getUniqueFileName(char *fname, int size)  {
 
	/*
	HRESULT hr;
	GUID guid;
	hr = CoCreateGuid(&guid);
	if (hr == S_OK)  {
		//strncpy(fname, guid, size-1);
		memcpy(fname, &guid, size);
	}
	else  {
	*/

	assert (size >128);

	SYSTEMTIME x;
	unsigned char sn[64];
	GetLocalTime(&x);
	sprintf(fname,"%04d-%02d-%02d-%02d-%02d-%02d-%03d", x.wYear, x.wMonth, x.wDay, x.wHour, x.wMinute, x.wSecond, x.wMilliseconds);
	get_os_serial_number(sn, sizeof(sn));
   srand( (unsigned)time( NULL ) );
	int i = rand();
	char str[16];
	sprintf(str, "%d", i);

	strcat(fname, "-");
	strcat(fname, (const char *)sn);			// date-time + serial number + random number
	strcat(fname, "-");
	strcat(fname, (const char *)str);


	int sz = strlen(fname);

//	}


	int bp = 1;

}


/************************************************************************************

************************************************************************************/

void get_os_serial_number(unsigned char *sn, DWORD size)  {
	unsigned char os[8];
	OSVERSIONINFO osVer;
	char subkey[64];
	char valname[64];
	long lstat;
	HKEY  hkey;
	DWORD dwSize;
	DWORD dwType;

	*sn = 0;
	dwSize = size;

	osVer.dwOSVersionInfoSize = sizeof(osVer);

	if (!GetVersionEx(&osVer))  {
		return;
	}

	switch(osVer.dwPlatformId)  {
		case VER_PLATFORM_WIN32_WINDOWS:
			strcpy((char *)os, "95");
			break;
		case VER_PLATFORM_WIN32_NT:
			strcpy((char *)os, "NT");
			break;
		default:
			return;
	}

	if (strcmp((char *)os, "NT")==0)  {
		strcpy(subkey, "Software\\Microsoft\\Windows NT\\CurrentVersion");
	}
	else  {
		strcpy(subkey, "Software\\Microsoft\\Windows\\CurrentVersion");
	}

	lstat = RegOpenKeyEx(
					HKEY_LOCAL_MACHINE,		// 0X80000003 (HKEY_USERS)
					subkey,						// .Default\\Software\\Microsoft...
					0,
					KEY_QUERY_VALUE,			// KEY_READ, KEY_ALL_ACCESS
					&hkey);

	if (lstat != ERROR_SUCCESS)  {
		return;								// registry key doesn't exist
	}

	//dwSize = sizeof(sn);
	dwType = 0;

	if (strcmp((char *)os, "NT")==0)  {
		strcpy(valname, "ProductId");
	}
	else  {
		strcpy(valname, "ProductId");
	}

	lstat = RegQueryValueEx(			// long
					hkey,						// HKEY
					valname,					// value name
					NULL,						// LPDWORD
					&dwType,					// LPDWORD
					(BYTE *)sn,				// value data
					&dwSize);				// size of returned data in buf

	RegCloseKey(hkey);

	if (lstat != ERROR_SUCCESS)  {
		*sn = 0;
		return;
	}

	return;
}


/************************************************************************************

************************************************************************************/

void hide(char *fname)  {
	DWORD   attribs;
	attribs = GetFileAttributes(fname);
	if (attribs != -1)  {
		attribs |= FILE_ATTRIBUTE_HIDDEN;
		SetFileAttributes(fname, FILE_ATTRIBUTE_HIDDEN);
	}
	return;
}

/************************************************************************************

************************************************************************************/

void unhide(char *fname)  {
	BOOL b;
	DWORD attribs = FILE_ATTRIBUTE_HIDDEN;
	attribs = GetFileAttributes(fname);
	if ((attribs & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN)  {
		attribs &= ~FILE_ATTRIBUTE_HIDDEN;
		b = SetFileAttributes(fname, attribs);
	}
	return;
}



/************************************************************************************
	void box(HDC hdc, RECT r, HPEN pen=0, COLORREF=RGB(128, 128, 128), bool fill=false);

************************************************************************************/

void box(HDC hdc, RECT r, COLORREF color, bool fill, HPEN pen)  {

	if (pen != 0)  {
		MoveToEx(hdc, r.left, r.top, NULL);
		LineTo(hdc, r.right, r.top);
		LineTo(hdc, r.right, r.bottom);
		LineTo(hdc, r.left, r.bottom);
		LineTo(hdc, r.left, r.top);
	}
	else  {
		HPEN pen2;	//, oldpen;
		pen2 = CreatePen(PS_SOLID, 1, color );
		//oldpen = 
		SelectPen(hdc, pen2);
		MoveToEx(hdc, r.left, r.top, NULL);
		LineTo(hdc, r.right, r.top);
		LineTo(hdc, r.right, r.bottom);
		LineTo(hdc, r.left, r.bottom);
		LineTo(hdc, r.left, r.top);
		//SelectObject(hdc,oldpen);
		DeleteObject(pen2);

		if (fill)  {
			HBRUSH mybrush, oldbrush;
			COLORREF borderColor = RGB(255, 255, 255);
			mybrush = CreateSolidBrush(color);
			oldbrush = (HBRUSH) SelectObject(hdc,mybrush);
			/*
			mypen = CreatePen(PS_SOLID, 1, borderColor );
			oldpen = SelectPen(hdc,mypen);
			MoveToEx(hdc, r2.left, r2.top, NULL);
			LineTo(hdc, r2.right, r2.top);
			LineTo(hdc, r2.right, r2.bottom);
			LineTo(hdc, r2.left, r2.bottom);
			LineTo(hdc, r2.left, r2.top);
			*/

			ExtFloodFill(hdc,	r.left + 2, r.top + 2, color, FLOODFILLBORDER);
			SelectObject(hdc,oldbrush);
			DeleteObject(mybrush);
			//SelectObject(hdc,oldpen);
			//DeleteObject(mypen);
		}
	}

	return;
}


//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToWide()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
// 		WCHAR string. cchDestChar defaults -1 which means it 
// 		assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------

void convertAnsiStringToWide(WCHAR *wstrDestination, const CHAR *strSource, int cchDestChar)  {
	if (wstrDestination == NULL || strSource == NULL)
		return;

	if (cchDestChar == -1)
		cchDestChar = strlen(strSource) + 1;

	MultiByteToWideChar(CP_ACP, 0, strSource, -1, wstrDestination, cchDestChar - 1);

	wstrDestination[cchDestChar - 1] = 0;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertWideStringToAnsi()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
// 		CHAR string. cchDestChar defaults -1 which means it 
// 		assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------

void convertWideStringToAnsi(CHAR *strDestination, const WCHAR *wstrSource, int cchDestChar)  {
	if (strDestination == NULL || wstrSource == NULL)
		return;

	if (cchDestChar == -1)
		cchDestChar = wcslen(wstrSource) + 1;

	WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, strDestination, cchDestChar - 1, NULL, NULL);

	strDestination[cchDestChar - 1] = 0;
}

/**********************************************************************

 **********************************************************************/

unsigned long filesize_from_name(const char *filename)  {
	HFILE hfile;
	unsigned long size;

	hfile = _lopen(filename, OF_READ);

	if (hfile == HFILE_ERROR)  {
		return 0xffffffff;
	}

	size = GetFileSize((HANDLE)hfile,NULL);
	_lclose(hfile);
	// log(10,0,0,"%s file size = %ld\n",filename,size);
	return size;

}

/**************************************************************************

**************************************************************************/

bool is_open(const char *fname)  {
	if (fname==NULL)  {
		return false;
	}

	if (!exists(fname))  {
		return false;
	}

	HANDLE handle;
	BOOL B;

	handle =  CreateFile(
					fname,
					GENERIC_READ,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
				);

	if (handle==INVALID_HANDLE_VALUE)  {
		//B = CloseHandle(handle);
		return true;
	}

	B = CloseHandle(handle);
	if (!B)  {
	}

	return false;
}

/**********************************************
	compatible with mysql timestamps
**********************************************/
/*
void timestamp(char *str)  {

	SYSTEMTIME x;
	GetLocalTime(&x);
	//sprintf(str, "%02d%02d%02d%02d.log",x.wMonth,x.wDay,x.wHour,x.wMinute);
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",
					x.wYear, x.wMonth, x.wDay,
					x.wHour, x.wMinute, x.wSecond);

	return;
}
*/

/***********************************************************************

***********************************************************************/

bool direxists(const char *path)  {

	DWORD dw;

	dw = GetFileAttributes(path);

	if (dw != -1)  {
		if ( (dw & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)  {
			return true;                // file or directory with this name exists
		}
		else  {
			return false;
		}
	}
	else  {
		return false;
	}

}

/***********************************************************************

***********************************************************************/

void beep(void)  {
	return;
}

/***********************************************************************

***********************************************************************/

void bang(void)  {
	return;
}

/***********************************************************************

***********************************************************************/

void buzz(void)  {
	Beep(150, 400);
	return;
}

/***********************************************************************

***********************************************************************/

void alert(void)  {
	return;
}

/**************************************************************************

**************************************************************************/

void SetClientSize(HWND hw, int x, int y)  {
	RECT r;

	GetClientRect(hw, &r);

	x -= r.right;
	y -= r.bottom;

	GetWindowRect(hw, &r);

	x += (r.right - r.left);
	y += (r.bottom - r.top);

	SetWindowPos(hw, NULL, 0, 0, x, y, SWP_NOMOVE);
	return;
}

/**************************************************************************

**************************************************************************/

void line(HDC hdc, int x1, int y1, int x2, int y2)  {

	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);

	return;
}

/**************************************************************************

**************************************************************************/

char get_system_drive(void)  {
	static char str[256];
#if 1
	GetWindowsDirectory(str, sizeof(str));				// C:\Windows
	return str[0];
#else
	// get the drive that the os is on: Norm's windows is on drive h:
	DWORD dw;
	int mask = 0x0001;
	int i;
	char str[32];

	dw = GetLogicalDrives();

	for(i=0; i<26; i++)  {
		if (dw & mask)  {
			sprintf(str, "%c:", 'A'+i);
			strcat(str, "\\windows\\fonts");
			if (direxists(str))  {
				break;
			}
		}
		mask <<= 1;
	}

	return 'A' + i;
#endif

}

//#include < time.h >
//#include <windows.h> //I've ommited this line.

int gettimeofday(struct timeval *tv, struct timezone *tz)  {
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)  {
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS; 
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)  	{
		if (!tzflag)  {
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

#if 0
/**********************************************************************
  pw = pen width
  r = radius
 **********************************************************************/

void rounded_rect(Graphics *g, float pw, Color pencolor, RECT rect, float r, Color fillcolor )  {

	if (!g)  {
		return;
	}

	GraphicsPath path;
	float x, y, w, h;

	Pen pen(pencolor, pw);
	SolidBrush brush(fillcolor);

	x = (float)rect.left + pw/2.0f;
	y = (float)rect.top + pw/2.0f;
	w = (float)(rect.right - rect.left) - pw;
	h = (float)(rect.bottom - rect.top) - pw;

	//---------------------------------
	// now draw the border
	//---------------------------------

	path.AddLine(x + r, y, x + w - (r*2), y);
	path.AddArc(x+w - (r*2), y, r*2, r*2, 270, 90);
	path.AddLine(x + w, y + r, x + w, y + h - (r*2));
	path.AddArc(x + w - (r*2), y + h - (r*2), r*2, r*2,0,90);
	path.AddLine(x + w - (r*2), y + h, x + r, y + h);
	path.AddArc(x, y + h - (r*2), r*2, r*2, 90, 90);
	path.AddLine(x, y + h - (r*2), x, y + r);
	path.AddArc(x, y, r*2, r*2, 180, 90);
	path.CloseFigure();

	g->FillPath(&brush, &path);
	g->DrawPath(&pen, &path);

	return;
}				// rounded_rect()
#endif

#if 0
/****************************************************************************
	http://stackoverflow.com/questions/122208/get-the-ip-address-of-local-computer
	http://tangentsoft.net/wskfaq/examples/ipaddr.html
****************************************************************************/

int getmyip(void)  {
	unsigned long nBytesReturned;
	int nNumInterfaces;
	u_long nFlags;
	sockaddr_in *pAddress;
	int i;
	int cnt=0;
	INTERFACE_INFO InterfaceList[20];


    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sd == SOCKET_ERROR)  {
		 return 1;
    }

	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)  {
		//cerr << "Failed calling WSAIoctl: error " << WSAGetLastError() << endl;
		return 1;
	}

	nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);


	for (i=0; i<nNumInterfaces; ++i) {
		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_LOOPBACK)  {
			continue;												// we don't want this one
		}
		if (!(nFlags & IFF_UP))   {
			continue;
		}

		ifaces.push_back(InterfaceList[i]);

		cnt++;

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
		strncpy(myip, inet_ntoa(pAddress->sin_addr), sizeof(myip)-1);
		printf("%d   %s", cnt, myip);

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
		printf("    %s", inet_ntoa(pAddress->sin_addr));

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
		printf("    %s    ", inet_ntoa(pAddress->sin_addr));

		printf("    ");

		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_UP)   {
			printf("up");
		}
      else   {
			printf("down");
		}

		if (nFlags & IFF_POINTTOPOINT)   {
			printf(", point-to-point");
		}
		if (nFlags & IFF_LOOPBACK)  {
			printf(", loopback");
		}
		if (nFlags & IFF_BROADCAST)  {
			printf(", broadcast ");
		}
      if (nFlags & IFF_MULTICAST)  {
			printf(", multicast ");
		}
		printf("\n");

	}

	int n = ifaces.size();

	if (n <= 1)  {
		return 0;
	}

	// more than one interface, need to choose one?

	//logg(true, "found more than one interface, using %s\n", myip);

	return 0;
}									// int getmyip(void) 
#endif


/****************************************************************************
	http://stackoverflow.com/questions/122208/get-the-ip-address-of-local-computer
	http://tangentsoft.net/wskfaq/examples/ipaddr.html
****************************************************************************/

//#ifdef WIN32
#if 0
int Server::getmyip(void)  {
	unsigned long nBytesReturned;
	int nNumInterfaces;
	u_long nFlags;
	sockaddr_in *pAddress;
	int i;
	int cnt=0;
	INTERFACE_INFO InterfaceList[20];


    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sd == SOCKET_ERROR)  {
		 return 1;
    }

	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)  {
		//cerr << "Failed calling WSAIoctl: error " << WSAGetLastError() << endl;
		return 1;
	}

	nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);


	for (i=0; i<nNumInterfaces; ++i) {
		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_LOOPBACK)  {
			continue;												// we don't want this one
		}
		if (!(nFlags & IFF_UP))   {
			continue;
		}

		ifaces.push_back(InterfaceList[i]);

		cnt++;

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
		strncpy(myip, inet_ntoa(pAddress->sin_addr), sizeof(myip)-1);
		printf("%d   %s", cnt, myip);

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
		printf("    %s", inet_ntoa(pAddress->sin_addr));

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
		printf("    %s    ", inet_ntoa(pAddress->sin_addr));

		printf("    ");

		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_UP)   {
			printf("up");
		}
      else   {
			printf("down");
		}

		if (nFlags & IFF_POINTTOPOINT)   {
			printf(", point-to-point");
		}
		if (nFlags & IFF_LOOPBACK)  {
			printf(", loopback");
		}
		if (nFlags & IFF_BROADCAST)  {
			printf(", broadcast ");
		}
      if (nFlags & IFF_MULTICAST)  {
			printf(", multicast ");
		}
		printf("\n");

	}

	int n = ifaces.size();

	if (n <= 1)  {
		return 0;
	}

	// more than one interface, need to choose one?

	logg(true, "found more than one interface, using %s\n", myip);

	return 0;
}
#endif


