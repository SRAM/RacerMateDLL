
#ifndef _DIR_H_
#define _DIR_H_

#ifdef WIN32
	#include <commctrl.h>
#endif

#include <vector>
#include <string>
#include <stack>

#ifdef WIN32
	#include <cregexp.h>
#endif


/************************************************************************************************************

	Eg,

	const char *ccptr;

	Dir *dir = new Dir(".\\courses", "*.crs", true, NULL);

	while(ccptr=dir->next())  {
		printf("%s\n", ccptr);
	}

	DEL(dir);


************************************************************************************************************/

class Dir  {

	public:
		typedef struct  {
			std::string s;
			DWORD attributes;
			int level;
		} DSTRUCT;

#ifdef WIN32
		Dir(const char *_dir, char *_filespec, bool _recurseFlag=false, HWND _hTree=NULL);
#else
		Dir(const char *_dir, char *_filespec, bool _recurseFlag=false);
#endif

		virtual ~Dir();
		const char *next(void);
		DWORD getAttributes(void)  {
			return curds.attributes;
		}
		int getLevel(void)  {
			return curds.level;
		}
		int getCount(void)  {
			return count;
		}

	private:

		#ifdef _DEBUG
		int bp;
		FILE *logstream;
		#endif

#ifdef WIN32
		HWND hTree;
		std::stack <HTREEITEM> nodes;
		TV_INSERTSTRUCT tvinsert;
		HTREEITEM Parent;
		//HTREEITEM Before;
#endif
		
		DSTRUCT curds;
		char filespec[256];
		//char str[256];
		int level;
		int maxlevel;
		void recurse(char *dir);
		//std::vector<std::string> vec;
		std::vector<DSTRUCT> vec;
		int n;
		int index;
#ifdef WIN32
		CRegExp r;
#endif

		int count;
		bool recurseFlag;
};

#endif		//#ifndef _DIR_H_


