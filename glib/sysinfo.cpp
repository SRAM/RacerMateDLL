
#pragma warning(disable:4101 4786 4996)				// "str" unreferenced

#include <sysinfo.h>

#include <defines.h>
//#include <dx7.h>
#include <err.h>
#include <assert.h>
#include <utils.h>

//VOID GetDXVersion( DWORD* pdwDXVersion, DWORD* pdwDXPlatform, OSVERSIONINFO *osVer);


/*************************************************************************

*************************************************************************/

SysInfo::SysInfo(Logger *_logg, HDC _hdc)  {
	DWORD volsernum = 0;
	DWORD maxfilelen = 0;
	DWORD volflags = 0;
	char systemName[64] = {0};
	char volname[64] = {0};
	BOOL b;

	if (_logg==NULL)  {
		return;
	}

	if (_hdc==0)  {
		return;
	}

	dwDxVersion = 0L;
	dwDxPlatform = 0L;
	memset(&EnumStruct, 0, sizeof(EnumStruct));
	memset(&osVer, 0, sizeof(osVer));
	bp = 0;

	hinstance = GetModuleHandle(NULL);

	_logg->write(10, 0, 1, "\n****** START OF SYSTEM INFO *******\n");

	b = GetVolumeInformation(
			"c:\\",					// LPCTSTR lpRootPathName,           // root directory
			volname,					// LPTSTR lpVolumeNameBuffer,        // volume name buffer
			sizeof(volname),		// DWORD nVolumeNameSize,            // length of name buffer
			&volsernum,				// LPDWORD lpVolumeSerialNumber,     // volume serial number
			&maxfilelen,			// LPDWORD lpMaximumComponentLength, // maximum file name length
			&volflags,				// LPDWORD lpFileSystemFlags,        // file system options
			systemName,				// LPTSTR lpFileSystemNameBuffer,    // file system name buffer
			sizeof(systemName)	// DWORD nFileSystemNameSize         // length of file system name buffer
		);
	if (!b)  {
		#ifdef _DEBUG
		Err *err = new Err();
		DEL(err);
		#endif
	}

	_logg->write("volume serial number = %08lX\n", volsernum);
	_logg->write("volume name = %s\n", volname);
	_logg->write("system name = %s\n", systemName);


	if (volflags &	FS_CASE_IS_PRESERVED)				// The file system preserves the case of file names when it places a name on disk. 
		_logg->write("\tpreserves case\n");

	if (volflags &	FS_CASE_SENSITIVE)					// The file system supports case-sensitive file names. 
		_logg->write("\tsupports case\n");

	if (volflags &	FS_UNICODE_STORED_ON_DISK)			// The file system supports Unicode in file names as they appear on disk. 
		_logg->write("\tsupports Unicode\n");

	if (volflags &	FS_PERSISTENT_ACLS)					// The file system preserves and enforces ACLs. For example, NTFS preserves and enforces ACLs, and FAT does not.  
		_logg->write("\tsupports ACLs\n");

	if (volflags &	FS_FILE_COMPRESSION)					// The file system supports file-based compression. 
		_logg->write("\tsupports file compression\n");

	if (volflags &	FS_VOL_IS_COMPRESSED)				// The specified volume is a compressed volume; for example, a DoubleSpace volume. 
		_logg->write("\tvolume is compressed\n");

	if (volflags &	FILE_NAMED_STREAMS)					// The file system supports named streams. 
		_logg->write("\tsupports named streams\n");

	if (volflags &	FILE_SUPPORTS_ENCRYPTION)			// The file system supports the Encrypted File System (EFS). 
		_logg->write("\tsupports EFS\n");

	if (volflags &	FILE_SUPPORTS_OBJECT_IDS)			// The file system supports object identifiers. 
		_logg->write("\tsupports object identifiers\n");

	if (volflags &	FILE_SUPPORTS_REPARSE_POINTS)		// The file system supports reparse points. 
		_logg->write("\tsupports reparse points\n");

	if (volflags &	FILE_SUPPORTS_SPARSE_FILES)		// The file system supports sparse files. 
		_logg->write("\tsupports sparse files\n");

	if (volflags &	FILE_VOLUME_QUOTAS)					// The file system supports disk quotas. 
		_logg->write("\tsupports disk quotas\n");

	_logg->write(10,0,1, "\n");

	//-------------------------------------------------
	// FIND OUT WHAT DIRECTDRAW VERSION WE'RE RUNNING:
	//-------------------------------------------------


#if 0
	GetDXVersion(&dwDxVersion, &dwDxPlatform, &osVer);
//	sprintf(gstring,"\nrunning DirectDraw version %d.%02d",
//		(dwDxVersion & 0x0000ff00)>>8, dwDxVersion & 0x000000ff);
//	logg->write(5, 0, 1, "%s\n",gstring);


	if (dwDxPlatform==VER_PLATFORM_WIN32_NT)  {
        if( osVer.dwMajorVersion < 5 )  {
			_logg->write(10, 0, 1,"error: running Windows NT %d.%d\n", osVer.dwMajorVersion, osVer.dwMinorVersion);
			//return;
		}
		else  {
			_logg->write(10, 0, 1,"running Windows NT %d.%d\n", osVer.dwMajorVersion, osVer.dwMinorVersion);
		}
	}
	else if (dwDxPlatform==VER_PLATFORM_WIN32_WINDOWS)  {
		_logg->write(5, 0, 1,"running windows9x\n");
	}
	else  {
		_logg->write(10, 0, 1,"Unknown Platform\n");
		//strcpy(caption, "Error");
		MessageBox(NULL, "Unknown Platform", "Error", MB_OK);
	}
#endif

/*
	getdeviceinfo(_logg);			// fills in g_DeviceInfo[]
*/

	DEVMODE devmode;
	memset(&devmode, 0, sizeof(devmode));
	devmode.dmSize = sizeof(devmode);
	DWORD dwflags=0;
//	int mode;
	int i = 0;

	while(1)  {
		b = EnumDisplaySettings(NULL, i, &devmode);
		if (!b)  {
			break;
		}
		_logg->write("%d x %d x %d\n", devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmBitsPerPel);
		i++;
	}

	_logg->write(10,0,1,"\n");

	//-------------------------------------------------
	// enumerate the serial ports from the registry:
	//-------------------------------------------------

	char sKeyName[MAX_PATH];
	char sKeyValue[MAX_PATH];
	HKEY key;

	if (RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			"Hardware\\DeviceMap\\SerialComm",
			0,
			KEY_QUERY_VALUE,
			&key) == ERROR_SUCCESS) {

		LONG retval = ERROR_SUCCESS;
		DWORD index = MAXDWORD;
		DWORD nameSize;
		DWORD valSize;
		DWORD type;

		_logg->write("\ncomm ports listed in registry:\n");

		while (retval == ERROR_SUCCESS)  {
			++index;
			nameSize = MAX_PATH;
			valSize = MAX_PATH;
			retval = RegEnumValue(
							key,
							index,
							sKeyName,
							&nameSize,
							NULL,
							&type,
							(unsigned char *)sKeyValue,
							&valSize);

			if (retval == ERROR_SUCCESS)  {
				// sKeyValue now contains one of the COM ports available
				// on the machine.
				_logg->write("%s\n", sKeyValue);
			}
			else  {
			}
		}
	}
	else  {
		_logg->write(10,0,1,"error opening registry\n");
	}

	if (RegCloseKey(key) != ERROR_SUCCESS)  {
		_logg->write(10,0,1,"error closing registry\n");
	}

	//--------------------------------------------------------
	// find the basic drive information for future reference:
	//--------------------------------------------------------

	_logg->write("\n");
	UINT type;
	char drive[8];


	for(i=0;i<26;i++)  {
		sprintf(drive,"%c", 'A'+i);


		strcpy(str,drive);
		strcat(str,":\\");

		type = GetDriveType(str);

		_logg->write(5,0,0,"drive %s\n", str);

		switch(type)  {					// The drive type cannot be determined
			case 0:
				_logg->write(10,1,0,"drive type cannot be determined\n");
				break;

			case 1:							// The root directory does not exist
				_logg->write(10,1,0,"the root directory does not exist\n");
				break;

			case DRIVE_REMOVABLE:		//	The drive can be removed from the drive
				_logg->write(5,1,0,"The drive can be removed from the drive\n");
				break;

			case DRIVE_FIXED:				// The disk cannot be removed from the drive
				_logg->write(10,1,0,"The disk cannot be removed from the drive\n");
				break;

			case DRIVE_REMOTE:			// The drive is a remote (network) drive
				_logg->write(10,1,0,"The drive is a remote (network) drive\n");
				break;

			case DRIVE_CDROM:				// The drive is a CD-ROM drive
				_logg->write(10,1,0,"The drive is a CD-ROM drive\n");
				goto done;

			case DRIVE_RAMDISK:			//	The drive is a RAM disk
				_logg->write(10,1,0,"The drive is a RAM disk\n");
				break;
		}
   };

done:
	_logg->write(10,0,1,"\n");


	//--------------------------------------
	// get the general machine information:
	//--------------------------------------

	GetMachineCaps(_logg);


	//-----------------------------------
	// FONT INFORMATION:
	//-----------------------------------

	/*
    EnumFonts(
        _hdc,
        0,									// address of font typeface name string
        (FONTENUMPROC)font_func,		// address of callback function
        (long)this						// address of application-supplied data
    );
	 _logg->write(10, 0, 1, "\n");
	*/

/*
	// memory leak here, commentd out until I have time to look for it:

	FONTENUMPROC lpfnEnumProc;
	EnumStruct.Count = 0;

	lpfnEnumProc = (FONTENUMPROC)MakeProcInstance((FARPROC)FontCallBack, hInstance);

	//SetWindowLong (hwndEdit, GWL_USERDATA, (long)this);

	_logg->write("calling enumfontfamilies\n");
	EnumFontFamilies(_hdc, NULL, lpfnEnumProc,(LONG)this);
	_logg->write("back from enumfontfamilies\n");

	FreeProcInstance((FARPROC)lpfnEnumProc);
	_logg->write("\n   %d FONTS FOUND:\n\n",EnumStruct.Count);


	for(i=0;i<EnumStruct.Count;i++)  {
		_logg->write(0,1,1,"%2d %s\n",i, fontname[i].c_str());
	}

*/

	/*
	#define TARGET_RESOLUTION 1         // 1-millisecond target resolution
	TIMECAPS tc;
	UINT     wTimerRes;
	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)   {
		// Error; application can't continue.
	}
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	timeBeginPeriod(wTimerRes); 
	*/

	_logg->write("\n");
	_logg->write(10, 0, 1, "\n\n****** END OF SYSTEM INFO *******\n\n");

	return;
}

/*************************************************************************

*************************************************************************/

SysInfo::~SysInfo()  {
	bp = 1;
	while(fontname.size())  {
		fontname.pop_back();
	}
}


/*************************************************************************

*************************************************************************/

int CALLBACK SysInfo::FontCallBack(LPENUMLOGFONT elf, LPNEWTEXTMETRIC ntm, int FontType, ENUMSTRUCT FAR * lpData)  {

	SysInfo *si = (SysInfo *) lpData;
	/*
	typedef struct tagENUMLOGFONT { 
	  LOGFONT elfLogFont; 
	  TCHAR   elfFullName[LF_FULLFACESIZE]; 
	  TCHAR   elfStyle[LF_FACESIZE]; 
	} ENUMLOGFONT, *LPENUMLOGFONT; 
	*/

	/*
		typedef struct tagLOGFONT { 
		  LONG lfHeight; 
		  LONG lfWidth; 
		  LONG lfEscapement; 
		  LONG lfOrientation; 
		  LONG lfWeight; 
		  BYTE lfItalic; 
		  BYTE lfUnderline; 
		  BYTE lfStrikeOut; 
		  BYTE lfCharSet; 
		  BYTE lfOutPrecision; 
		  BYTE lfClipPrecision; 
		  BYTE lfQuality; 
		  BYTE lfPitchAndFamily; 
		  TCHAR lfFaceName[LF_FACESIZE]; 
		} LOGFONT, *PLOGFONT; 
	*

	/*
	typedef struct tagNEWTEXTMETRIC { 
	  LONG   tmHeight; 
	  LONG   tmAscent; 
	  LONG   tmDescent; 
	  LONG   tmInternalLeading; 
	  LONG   tmExternalLeading; 
	  LONG   tmAveCharWidth; 
	  LONG   tmMaxCharWidth; 
	  LONG   tmWeight; 
	  LONG   tmOverhang; 
	  LONG   tmDigitizedAspectX; 
	  LONG   tmDigitizedAspectY; 
	  TCHAR  tmFirstChar; 
	  TCHAR  tmLastChar; 
	  TCHAR  tmDefaultChar; 
	  TCHAR  tmBreakChar; 
	  BYTE   tmItalic; 
	  BYTE   tmUnderlined; 
	  BYTE   tmStruckOut; 
	  BYTE   tmPitchAndFamily; 
	  BYTE   tmCharSet; 
	  DWORD  ntmFlags; 
	  UINT   ntmSizeEM; 
	  UINT   ntmCellHeight; 
	  UINT   ntmAvgWidth; 
	} NEWTEXTMETRIC, *PNEWTEXTMETRIC; 
	*/

	sprintf(si->str, "%s / %s / %ld / %ld", elf->elfLogFont.lfFaceName, elf->elfStyle, elf->elfLogFont.lfWidth, elf->elfLogFont.lfHeight );

#ifdef _DEBUG
	if (strstr(elf->elfLogFont.lfFaceName, "Nintendo"))  {
		si->bp = 1;
	}
#endif

	si->fontname.push_back(si->str);
	si->EnumStruct.Count++;
	return 1;
}

/*************************************************************************

*************************************************************************/

/*
int CALLBACK SysInfo::font_func(
	LOGFONT  *plf,					// address of logical-font data
	TEXTMETRIC *ptm,				// address of physical-font data
	DWORD dwType,					// font type
	LPARAM p_my_object)  {		// address of application-defined data

	SysInfo *p = (SysInfo *)p_my_object;
    
	//f->lf = *plf;
	//f->tm = *ptm;
	//f->type = dwType;
	//my_class *p = (my_class *)p_my_object; // looks bad but works

    // add font to list
	//f->next = p->font_list;
	//p->font_list = f;

	return 1; // to continue enumeration
}

*/
