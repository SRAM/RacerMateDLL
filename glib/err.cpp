

#ifdef WIN32
	#include <windows.h>
	#include <err.h>

	//extern "C" {

		/*************************************************************

		*************************************************************/

		Err::Err(void)  {
			code = GetLastError();

			FormatMessage( 
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						code,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &gstring,
						0,
						NULL 
			);

		}

		/*************************************************************

		*************************************************************/

		Err::~Err()  {
			LocalFree(gstring);
		}

		/*************************************************************

		*************************************************************/

		char * Err::getString(void)  {
			return gstring;
		}

	//}						// extern "C" {
#endif					// #ifdef WIN32
