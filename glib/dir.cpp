

#ifdef WIN32
	#pragma warning(disable:4786)
	#pragma warning(disable:4996)					// for vista strncpy_s
	#include <windows.h>
#endif

#include <stdio.h>
#include <assert.h>

#include <glib_defines.h>
#include <utils.h>
#include <dir.h>

#ifndef WIN32
#include <minmax.h>
#endif


/******************************************************************************************

******************************************************************************************/

#ifdef WIN32
Dir::Dir(const char *_dir, char *_filespec, bool _recurseFlag, HWND _hTree)  {
#else
Dir::Dir(const char *_dir, char *_filespec, bool _recurseFlag)  {
#endif

	char dir[256];

	strncpy(filespec, _filespec, sizeof(filespec)-1);
	//myremove("*", filespec);
#ifdef WIN32
	r.RegComp(filespec);				// set pattern
#endif

	recurseFlag = _recurseFlag;

#ifdef WIN32
	hTree = _hTree;
#endif

	strncpy(dir, _dir, sizeof(dir)-1);

	count = 0;
	level = 0;
	maxlevel = -1;
	index = 0;

#ifdef WIN32
	Parent = NULL;
	//Before = NULL;
	memset(&tvinsert, 0, sizeof(tvinsert));
#endif

	
#ifdef _DEBUG
	logstream = fopen("dir.log", "wt");
	fprintf(logstream, "\nPATH                                LEVEL   PARENT     THIS\n\n");
	bp = 0;
#endif

	recurse(dir);

#ifdef WIN32
	assert(nodes.empty());
#endif

	n = (int)vec.size();

}

/******************************************************************************************

******************************************************************************************/

Dir::~Dir()  {

	while (vec.size())  {
		vec.pop_back();
	}

#ifdef _DEBUG
	fprintf(logstream, "\n\nmaxlevel = %d\n", maxlevel);
	fclose(logstream);
#endif

}

/******************************************************************************************

******************************************************************************************/

const char *Dir::next(void)  {

	if (index < n)  {
		curds = vec[index];
		//return vec[index++].c_str();
		return vec[index++].s.c_str();
		//return &vec[index++];
	}

	return NULL;
}

/******************************************************************************************

******************************************************************************************/

/*
DSTRUCT *Dir::next2(void)  {

//	if (index < n)  {
//		curds = vec[index];
//		return &vec[index++];
//	}

	return NULL;
}
*/

/******************************************************************************************

******************************************************************************************/


void Dir::recurse(char *dir)  {

	char path[MAX_PATH];
#ifdef WIN32
	WIN32_FIND_DATA fd;
#endif

#ifdef WIN32
	BOOL more;
	int pos;
#endif

	DSTRUCT ds;

#ifdef WIN32
	nodes.push(Parent);
#endif

	level++;
	maxlevel = MAX(level, maxlevel);

	sprintf(path, "%s\\%s", dir, filespec);

#ifdef WIN32
	HANDLE handle = FindFirstFile(path, &fd);
	more = (handle!=INVALID_HANDLE_VALUE);
#endif
	
#ifdef _DEBUG
	/*
	char str2[256];
	str[0] = 0;
	for(int i=1;i<level;i++)  {
		sprintf(str2, "   ");
		strcat(str, str2);
	}
	sprintf(str2, "%s\\%s", path, fd.cFileName);
	strcat(str, str2);
	fprintf(logstream, "%-40s%1d   %08x", str, level, Parent);
	 */
#endif

#ifdef WIN32

	while(more)  {
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  {
			if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") )  {
				std::string s(fd.cFileName);
				ds.s = s;
				ds.attributes = fd.dwFileAttributes;
				ds.level = level;
				vec.push_back(ds);

				sprintf(path, "%s\\%s", dir, fd.cFileName);

				if (hTree)  {
					tvinsert.hParent = Parent;		// NULL
					tvinsert.hInsertAfter = TVI_ROOT;
					tvinsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
					tvinsert.item.pszText = fd.cFileName;
					tvinsert.item.iImage = 0;
					tvinsert.item.iSelectedImage = 1;
					Parent = (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
#ifdef _DEBUG
					//fprintf(logstream, "   %08lx\n", Parent);
#endif
				}

				if (recurseFlag)  {
					recurse(path);
				}
				more = FindNextFile(handle, &fd);
				continue;
			}
			else  {
				more = FindNextFile(handle, &fd);
				continue;
			}
		}

		//pos = r.RegFind(fd.cFileName);
		if (filespec[0]=='*' && filespec[1]==0)  {					// *		(find everything)
			pos = 0;
		}
		else if (filespec[0]=='*' && filespec[1]=='.')  {			// *.
			pos = indexIgnoreCase(fd.cFileName, &filespec[2], 0, false);
		}
		else  {
			pos = indexIgnoreCase(fd.cFileName, filespec, 0, false);
			//pos = indexIgnoreCase(filespec, fd.cFileName, 0, false);				// str, pattern
		}

		count++;

		if (pos != -1)  {
			std::string s(dir);
			s += "\\";
			s += fd.cFileName;
			ds.s = s;
			ds.attributes = fd.dwFileAttributes;
			ds.level = level;


			if (indexIgnoreCase(fd.cFileName, ".vel", 0, false) != -1)  {
				if (hTree)  {
					tvinsert.hParent = Parent;
					tvinsert.hInsertAfter = TVI_LAST;
					tvinsert.item.pszText = fd.cFileName;
					SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvinsert);
				}
				vec.push_back(ds);
			}
			else  {
				vec.push_back(ds);
			}
		}

		more = FindNextFile(handle, &fd);
    }

    FindClose(handle);
#endif				//#ifdef WIN32

	 level--;

	 //Parent = before;
#ifdef WIN32
	 Parent = nodes.top();
	 nodes.pop();
#endif

#ifdef _DEBUG
	 //fprintf(logstream, "*");
#endif

	 //int bp = 1;

	 return;
}


/******************************************************************************************

******************************************************************************************/

/*
void process(char *path, WIN32_FIND_DATA *fd)  {

	DWORD size = fd->nFileSizeLow;

	filecount++;

	//---------------------------------------------------------------
//		add sernum, path, fd->cFileName, and size to the database
	//---------------------------------------------------------------

	char fullpath[256];
	strcpy(fullpath, cdDrive);
	strcat(fullpath, path);
	strcat(fullpath, "\\");
	strcat(fullpath, fd->cFileName);

	memset(&rec, 0, sizeof(rec));

	strncpy(rec.sernum, sernumString, sizeof(rec.sernum));
	strncpy(rec.path, path, sizeof(rec.path));
	strncpy(rec.fname, fd->cFileName, sizeof(rec.fname));
	rec.size = size;


	FILETIME  local_time ;
	SYSTEMTIME  sys_time ;

	FileTimeToLocalFileTime(&fd->ftCreationTime, &local_time);
	FileTimeToSystemTime (&local_time, &sys_time);
	rec.creationTime = sys_time;

	return;
}
*/


