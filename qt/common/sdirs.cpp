
#include <QString>
#ifdef WIN32
	#include <shlobj.h>
#endif


#include "sdirs.h"

namespace RACERMATE  {


/***********************************************************************************

***********************************************************************************/

SDIRS::SDIRS(int _debug_level, QString _base)  {

	if (_debug_level <= 0) {
		return;
	}

	bool b;

#ifdef WIN32
	wchar_t gstr[512] = { 0 };
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, gstr);  // "C:\Program Files"
	program_dir = QString::fromWCharArray(gstr) + "\\";
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, gstr);       // "C:\\Users\\larry\\Documents"
	personal_dir = QString::fromWCharArray(gstr);
	personal_dir += "\\" + _base +  + "\\";                               // "C:\\Users\\larry\\Documents\\RacerMate"
	settings_dir = personal_dir + "Settings\\";                 // "C:\\Users\\larry\\Documents\\RacerMate\\Settings"
	debug_dir = personal_dir + "Debug\\";                       // "C:\\Users\\larry\\Documents\\RacerMate\\Debug"

#else
	program_dir = QCoreApplication::applicationDirPath() + "/";                            // .../qt/radio/Desktop_Qt_5_6_0_GCC_64bit-Debug
	personal_dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);   // /home/larry
	personal_dir = personal_dir + "/" + _base.toLower() + "/";
	debug_dir = personal_dir + "debug/";                                             // + "/..."
	settings_dir = personal_dir + "settings/";
#endif

	if (!QDir(personal_dir).exists()) {
		b = QDir().mkpath(personal_dir);
		if (!b)  {
			rc = -1;
		}
	}
	if (!QDir(settings_dir).exists()) {
		b = QDir().mkdir(settings_dir);
		if (!b)  {
			rc = -2;
		}
	}
	if (!QDir(debug_dir).exists()) {
		b =QDir().mkdir(debug_dir);
		if (!b)  {
			rc = -3;
		}
	}

	return;
}							// sdirs()

/***********************************************************************************

***********************************************************************************/

SDIRS::~SDIRS()  {
	//file.close();
	return;
}

}




