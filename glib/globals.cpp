
#include <glib_defines.h>

#ifdef DOGLOBS
	#ifdef WIN32
		#include <windows.h>
	#endif

	#include <vector>
	#include <string>

	//__declspec(dllexport) char gstring[2048];
	//char gstring[2048];

	#ifdef _DEBUG
		//bool oktobreak = false;
		//int bp = 0;
	#endif

	//char dirs[8][MAX_PATH] = { 0 };
//	std::vector<std::string> dirs;

	bool ogl_log = false;
	bool main_course_log = false;

#endif

