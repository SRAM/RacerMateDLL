#pragma once
#pragma warning(disable:4251)

#include <windows.h>
#include <vector>
#include <string>

#include <logger.h>

	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
	class __declspec(dllexport) SysInfo  {

	public:
		SysInfo(Logger *_logg, HDC _hdc);
		virtual ~SysInfo();
		DWORD dwDxVersion;
		DWORD dwDxPlatform;
		OSVERSIONINFO osVer;

	private:
		typedef struct  {
			int Count;
			HWND hWindow;
		} ENUMSTRUCT;

		ENUMSTRUCT EnumStruct;

		static int CALLBACK FontCallBack( LPENUMLOGFONT lpnlf, LPNEWTEXTMETRIC lpntm, int FontType, ENUMSTRUCT FAR * lpData);
		//static int CALLBACK font_func(LOGFONT  *plf, TEXTMETRIC *ptm, DWORD dwType, LPARAM p_my_object);
		char str[256];
		int bp;
		std::vector<std::string> fontname;
		HINSTANCE hinstance;


};

		}


