

#ifdef WIN32
	#pragma warning(disable:4101 4786)				// "str" unreferenced
	#include <windows.h>
	#include <windowsx.h>
	#include <assert.h>
	#include <seh.h>
	#include <eh.h>
	#include <commctrl.h>
	#include <new.h>
	#include <shlobj.h>
#endif

#include <glib_defines.h>

#include <comglobs.h>

#ifdef DOGLOBS
	#include <globals.h>
#endif

#include <utils.h>

#include <oldsdirs.h>

char OLDSDIRS::gstr[512] = {0};

#ifndef DOGLOBS
	std::vector<std::string> SDIRS::dirs;
#endif



/*************************************************************************************************

*************************************************************************************************/

//void setup_standard_dirs(const char *_appname, Logger *logg)  {

	OLDSDIRS::OLDSDIRS(const char *_appname, Logger *logg)  {

	char str[256];
	int i;
	const char *cptr;

#ifndef WIN32
	if (logg) logg->write(10,0,1,"setup_standard_dirs\n");
#endif


	if (dirs.size() != 0)  {
		if (logg) logg->write(10,0,1,"program directory already set up: %s\n", dirs[DIR_PROGRAM].c_str());
		return;
	}
	/*
	enum DIRTYPE {
		DIR_PROGRAM,
		DIR_PERSONAL,
		DIR_SETTINGS,
		DIR_REPORT_TEMPLATES,
		DIR_REPORTS,
		DIR_COURSES,
		DIR_PERFORMANCES,
		DIR_DEBUG
	};
	*/

/*
	enum DIRTYPE {
	DIR_PROGRAM,                    // 0
	DIR_PERSONAL,
	DIR_SETTINGS,
	DIR_REPORT_TEMPLATES,
	DIR_REPORTS,
	DIR_COURSES,
	DIR_PERFORMANCES,
	DIR_DEBUG,
	DIR_HELP,                       // 8
	NDIRS                           // 9
};
*/

#ifdef WIN32
	SHGetFolderPath(
			NULL,							// HWND hwndOwner,
			CSIDL_PROGRAM_FILES,			// int nFolder,
			NULL,							// HANDLE hToken,
			0,								// DWORD dwFlags,
			gstr
			);																				// "C:\Program Files"
	dirs.push_back(gstr);
	cptr = dirs[DIR_PROGRAM].c_str();


	SHGetFolderPath(
			NULL,							// HWND hwndOwner,
			CSIDL_PERSONAL,					// int nFolder,
			NULL,							// HANDLE hToken,
			0,								// DWORD dwFlags,
			gstr
			);																				// "C:\Users\larry\Documents"
	dirs.push_back(gstr);
	cptr = dirs[DIR_PERSONAL].c_str();


	if (logg)  {
		logg->write(10, 0, 1, "\n");
		logg->write("program directory = %s\n", dirs[DIR_PROGRAM].c_str());
		logg->write("personal directory = %s\n", dirs[DIR_PERSONAL].c_str());
		logg->write(10, 0, 1, "\n");
	}

	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}

	DIRTYPE dt;

	dt = (DIRTYPE)DIR_SETTINGS;							// 2
	assert(DIR_SETTINGS==2);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Settings");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_SETTINGS].c_str();

	dt = (DIRTYPE)DIR_REPORT_TEMPLATES;							// 3
	assert(DIR_REPORT_TEMPLATES==3);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Report Templates");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_REPORT_TEMPLATES].c_str();

	dt = (DIRTYPE)DIR_REPORTS;									// 4
	assert(DIR_REPORTS==4);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Reports");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_REPORTS].c_str();

	dt = (DIRTYPE)DIR_COURSES;								// 5
	assert(DIR_COURSES==5);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Courses");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_COURSES].c_str();

	dt = (DIRTYPE)DIR_PERFORMANCES;							// 6
	assert(DIR_PERFORMANCES==6);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Performances");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_PERFORMANCES].c_str();

	dt = (DIRTYPE)DIR_DEBUG;								// 7
	assert(DIR_DEBUG==7);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Debug");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_DEBUG].c_str();

	dt = (DIRTYPE)DIR_HELP;								// 8
	assert(DIR_HELP==8);
	sprintf(gstr, "%s\\%s", dirs[DIR_PERSONAL].c_str(), _appname);
	strcat(gstr, "\\Help");
	dirs.push_back(gstr);
	if (!direxists(gstr))  {
		CreateDirectory(gstr, NULL);
	}
	cptr = dirs[DIR_HELP].c_str();

#else
	//strcpy(personal_folder, "zzz");
	//strcpy(program_files_folder, "zzz");

	//bp = -1;
	for(i=0; i<9; i++)  {
		dirs.push_back(".");
	}

	for(i=0; i<9; i++)  {
		cptr = dirs[i].c_str();
		if (!direxists(cptr))  {
			CreateDirectory(cptr, NULL);
		}
	}
	//strcpy(logpath, dirs[DIR_DEBUG]);
	sprintf(str, "%s%smain.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
//	logg = new Logger(str);
#endif


	if (logg)  {
		for(i=0; i<9; i++)  {
			logg->write("%d   %s\n", i, dirs[i].c_str());
		}
		logg->write(10,0,1,"\n");
	}

	return;
}				// constructor

/*************************************************************************************************

*************************************************************************************************/



OLDSDIRS::~OLDSDIRS()  {

}


