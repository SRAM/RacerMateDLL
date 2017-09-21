
/*********************************************************************

*********************************************************************/

#include <glib_defines.h>
#include <crf.h>
#include <md5.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <algorithm>

#include <utils.h>
#include <fatalerror.h> 

#include <sys/stat.h>
//#include <unistd.h>

     
static struct termios stored_settings;

	//xxx

#if 0
/**********************************************************************************************************
	encrypts
**********************************************************************************************************/

int enc(char *fullname)  {
	FILE *stream=NULL;
//#pragma pack(push, 8)			// causes structure to be packed
	ENCREC rec;
//#pragma pack(pop)

	int status;
	unsigned long fsize;
	CRF sout;
	int size;
	unsigned char buf[1000];
	char newname[MAX_PATH];
	char stub[MAX_PATH];
	char path[MAX_PATH];
	char relpath[MAX_PATH];
	char cwd[MAX_PATH];
	char str[8];
	int pos;
	const char *cptr;

	strcpy(path, fullname);
	strip_filename(path);

	strcpy(stub, fullname);
	strip_path(stub);

	cptr = getcwd(cwd, sizeof(cwd));

	strncpy((char *)buf, stub, sizeof(buf)-1);
	MD5 *md5 = new MD5();
	md5->string((unsigned char *)buf, strlen((char *)buf) );

	memset(newname, 0, sizeof(newname));

	sprintf(newname, "%s/", path);

	for (int i = 0; i < 16; i++)  {
		sprintf(str, "%02x", md5->digest[i]);
		strcat(newname, str);
	}
	strcat(newname, ".bob");
	DEL(md5);

/*
	printf("\nfullname = %s\n", fullname);
	printf("path = %s\n", path);
	printf("newname = %s\n", newname);
	printf("cwd = %s\n", cwd);

return 0;
*/

	fsize = filesize_from_name(fullname);
	memset(&rec, 0, sizeof(rec));
	rec.size = fsize;

printf("rec.size = %ld\n", rec.size);

	//strncpy(rec.fname, path, sizeof(rec.fname)-1);				// rec.fname is stored without the path, for portability
	strncpy(rec.fname, fullname, sizeof(rec.fname)-1);				// rec.fname is stored without the path, for portability

	stream = fopen(fullname, "r+b");
	size = fread(buf, 1, sizeof(buf), stream);

	if (size == 0)  {
		fclose(stream);
		printf("\nsize = 0\n");
		return 1;
	}
	sout.init();

	unsigned char tmpbuf[63];
	sout.doo(tmpbuf, 63);						// run encryptor for 63 cycles

	sout.doo(buf, size);

	status = fseek(stream, 0, SEEK_SET);
	if (status != 0)  {
		fclose(stream);
		printf("\nerror converting %s\n", fullname);
		return 1;
	}

	status = fwrite(buf, 1, size, stream);
	if (status != size)  {
		printf("\nerror writing to %s\n", fullname);
	}

	status = fseek(stream, 0L, SEEK_END);
	if (status != 0)  {
		printf("\nerror 32\n");
		fclose(stream);
		return 1;
	}

	sout.doo((unsigned char *)&rec, sizeof(rec));
	status = fwrite(&rec, sizeof(rec), 1, stream);
	if (status != 1)  {
		printf("\nerror writing record to %s\n", fullname);
		fclose(stream);
		return 1;
	}

	fclose(stream);

	strncpy(stub, fullname, sizeof(stub)-1);
	pos = indexIgnoreCase(stub, ".", 0);
	if (pos==-1)  {
		return 1;
	}

	rm(newname);
	rename(fullname, newname);
	printf("%s --> %s\n", fullname, newname);

	return 0;
}



/*******************************************************************************

*******************************************************************************/

int dec(char *fname, bool _recursive)  {
	FILE *stream=NULL;
//#pragma pack(push, 8)
	ENCREC rec;
//#pragma pack(pop)
	int status;
	unsigned long fsize;
	unsigned char buf[1000];
	CRF sout;
	int size;
	bool makecopy = false;
	const char *savename = { "xxxdec.x" };
	int rc = 0;
	long offset;
	int bp = 0;


	if (makecopy)  {
		CopyFile(fname, savename, false);
	}

	fsize = filesize_from_name(fname);
	memset(&rec, 0, sizeof(rec));

	stream = fopen(fname, "r+b");
	if (stream==NULL)  {
		printf("can't open %s (check permission)\n", fname);
		//return 1;
		rc = 1;
		goto finis;
	}
	
	size = fread(buf, 1, sizeof(buf), stream);
	if (size == 0)  {
		printf("fread error\n");
		//FCLOSE(stream);
		//return 1;
		rc = 1;
		goto finis;
	}
	sout.init();
	unsigned char tmpbuf[63];
	sout.doo(tmpbuf, 63);						// run encryptor for 63 cycles

	sout.doo(buf, size);

#define GETERDONE

#ifdef GETERDONE
	status = fseek(stream, 0, SEEK_SET);
	if (status != 0)  {
		printf("\nerror converting %s\n", fname);
		rc = 1;
		goto finis;

		//rm(fname);
		//return 1;
	}
	status = fwrite(buf, 1, size, stream);
	if (status != size)  {
		printf("\nerror writing to %s\n", fname);
		//rm(fname);
		//return 1;
		rc = 1;
		goto finis;
	}
#endif


	offset = sizeof(rec);
	offset = -offset;
	status = fseek(stream, offset, SEEK_END);
	if (status != 0)  {
		printf("\nerror 33\n");
		//rm(fname);
		rc = 1;
		goto finis;
		//return 1;
	}

	size = fread(&rec, sizeof(rec), 1, stream);
	if (size != 1)  {
		//rm(fname);
		//return 1;
		rc = 1;
		goto finis;
	}

	sout.doo((unsigned char *)&rec, sizeof(rec));

	FCLOSE(stream);				// close x.bob

	// sanity checks:

	/*
	if (rec.size > 5 000 000 000)  {
		printf("file size error\n");
		rc = 1;
		goto finis;
	}
	*/

	if (strlen(rec.fname) > 254)  {
		printf("file name error 1\n");
		rc = 1;
		goto finis;
	}

	if (!valid_filename(rec.fname))  {
		printf("file name error 2\n");
		rc = 1;
		goto finis;
	}

	//------------------------------------------------
	// now fix the filesize of fname to 'rec.size'
	//------------------------------------------------

#ifdef GETERDONE

#ifdef WIN32
	fsize = filesize_from_name(fname);

   int fh;

	fh = _open( fname, _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE );
	if (fh==-1)  {
		printf("\nerror 43\n");
		return 1;
	}

	//fsize = filelength(fh);
	//fsize = filesize_from_stream(fh);

	status = _chsize(fh, rec.size);
	if (status != 0)  {
		printf( "Problem in changing the size\n" );
		_close( fh );
		return 1;
	}

	_close( fh );

#else
	status = truncate(fname, rec.size);
#endif

	if (_recursive)  {
		char str[256];
		strncpy(str, rec.fname, sizeof(str)-1);
		strip_filename(str);
		/*
		if (!direxists(str))  {
			bp = 1;
		}
		*/

	}
	else  {
		strip_path(rec.fname);
	}

	rename(fname, rec.fname);

#endif

finis:

	FCLOSE(stream);

	if (makecopy)  {
		rm(savename);
	}

	if (rc == 0)  {
		printf("%s --> %s\n", fname, rec.fname);
	}

//printf("\nsize = %ld\n", rec.size);
//printf("name = %s\n", rec.fname);

	//rm(fname);

	return rc;
}

#endif


/**************************************************************************

**************************************************************************/

#if 0
bool valid_filename(const char *fname)  {
	int i, len;
	len = strlen(fname);

	for(i=0; i<len; i++)  {
		if (fname[i] <' ' || fname[i]>0x7e)  {
			return false;
		}
	}

	return true;
}
#endif


/**************************************************************************

**************************************************************************/

bool CopyFile(const char *src, const char *dest, bool bFailIfExists)  {

	/*
	//void copy_file(const char *src_file, const char *des_file) {

	//int pid = fork();
	pid_t pid = fork();

//	if (pid) {
//		int s; 
//		waitpid(pid, &s, 0);
//	}
//	else {
		//execl("/bin/cp", "/bin/cp", src_file, des_file, 0);
		execlp("cp", "cp", "--", src, dest, (char *)0); 
//	}
	*/
	char cmd[256];
	sprintf(cmd, "cp %s %s", src, dest);
	system(cmd);
	return true;
}


#define INI_MAX_LINE_LENGTH    80

	/**************************************************************************
	 * Function:     get_private_profile_string()
	 * Arguments:    <char *> section - the name of the section to search for
	 *               <char *> entry - the name of the entry to find the value of
	 *               <char *> def - default string in the event of a failed read
	 *               <char *> buffer - a pointer to the buffer to copy into
	 *               <int> buffer_len - the max number of characters to copy
	 *               <char *> file_name - the name of the .ini file to read from
	 * Returns:      the number of characters copied into the supplied buffer
	 ***************************************************************************/

#if 0
	int GetPrivateProfileString(char *section, char *entry, char *def, char *buffer, int buffer_len, char *file_name) {
		FILE *fp = fopen(file_name,"r");
		char buff[INI_MAX_LINE_LENGTH];
		char *ep;
		char t_section[INI_MAX_LINE_LENGTH];
		int len = strlen(entry);
		if( !fp ) return(0);
		sprintf(t_section,"[%s]",section);    /* Format the section name */
		/*  Move through file 1 line at a time until a section is matched or EOF */
		do {
			if( !read_line(fp,buff) ) {
				fclose(fp);
				strncpy(buffer,def,buffer_len);    
				return(strlen(buffer));
			}
		}
		while( strcmp(buff,t_section) );
		/* Now that the section has been found, find the entry.
		 * Stop searching upon leaving the section's area. */

		printf("found section: [%s]\n", section);

		do	{
			if( !read_line(fp,buff) || buff[0] == '\0' )  {
				fclose(fp);
				strncpy(buffer,def,buffer_len);
				printf("\nreturning default: %s\n", def);
				return(strlen(buffer));
			 }
		}  while( strncmp(buff,entry,len) );

		ep = strrchr(buff,'=');    /* Parse out the equal sign */
		ep++;

		printf("found buff: %s, ep = %s\n", buff, ep);

		/* Copy up to buffer_len chars to buffer */
		strncpy(buffer,ep,buffer_len - 1);

		buffer[buffer_len] = '\0';
		fclose(fp);               /* Clean up and return the amount copied */
		return(strlen(buffer));

	}
#else
int GetPrivateProfileString(const char *section, const char *entry, const char *def, char *buffer, int buffer_len, char *file_name) {
	FILE *fp;
	char buf[INI_MAX_LINE_LENGTH];						// 80
	char t_section[INI_MAX_LINE_LENGTH];				// 80
	//char *ep;
	char *cptr;
	int pos;
	bool dbg=false;


	replace(file_name, "\\", "/", 1);

	fp = fopen(file_name,"r");
	if (fp==NULL)  {
		printf("can't open %s\n", file_name);
	}

	if (dbg)  {
		printf("\ngpps: section = %s\n", section);
		printf("gpps: entry = %s\n", entry);
		printf("gpps: def = %s\n", def);
		printf("gpps: file_name = %s\n", file_name);
	}

	//int len = strlen(entry);
	if(!fp)  {
		if (dbg)  {
			printf("no entry!\n");
		}
		return(0);
	}

	sprintf(t_section,"[%s]",section);    // Format the section name

	//  Move through file 1 line at a time until a section is matched or EOF

	while(1)  {
		memset(buf,0,sizeof(buf));
		cptr = fgets(buf, sizeof(buf), fp);
		if (dbg)  {
			printf("gpss: %s\n", buf);
		}
		if (cptr==NULL)  {
			strncpy(buffer, def, buffer_len);    
			return((int)strlen(buffer));
		}
		trimcrlf(buf);
		if (indexIgnoreCase(buf, t_section, 0, false) != -1)  {
			break;
		}
		else  {
			if (dbg)  {
				printf("gpss: rejected %s\n", buf);
			}
		}
	}

	if (dbg)  {
		printf("gpps: found section: %s\n", buf);
	}

	// Now that the section has been found, find the entry. Stop searching upon leaving the section's area.

	while(1)  {
		memset(buf,0,sizeof(buf));
		cptr = fgets(buf, sizeof(buf), fp);
		if (cptr==NULL)  {
			strncpy(buffer, def, buffer_len);    
			return((int)strlen(buffer));
		}
		//printf("buf = %s\n", buf);
		trimcrlf(buf);
		pos = indexIgnoreCase(buf, entry, 0, false);
		if (pos == 0)  {
			break;
		}
	}


	if (dbg)  {
		printf("gpps: found entry: %s\n", buf);
	}

	fclose(fp);

	pos = indexIgnoreCase(buf, "=");

	if (pos==-1)  {
		strncpy(buffer, def, buffer_len);    
		return((int)strlen(buffer));
	}

	strcpy(buf, &buf[pos+1]);				// causes valgrind warning
	//len = strlen(&buf[pos+1]);
	//memcpy(buf, &buf[pos+1], len);			// also causes valgrind warning
	ltrim(buf, 1);

	pos = indexIgnoreCase(buf, ":");
	if (pos==1)  {
		pos = indexIgnoreCase(buf, "\\");
		if (pos != -1)  {
			replace(buf, "\\", "/", 1);
			//memcpy(buff, &buff[2], strlen(buff)-2);			// removes the drive letter & colon
			strcpy(buf, &buf[2]);
			insert(buf, "/home/larry", 0);
		}
	}

	strcpy(buffer, buf);

	if (dbg)  {
		printf("gpps: returning %s\n", buffer);
	}

	return((int)strlen(buffer));

}


#endif



/**************************************************************************
	 * Function:     get_private_profile_int()
	 * Arguments:    <char *> section - the name of the section to search for
	 *               <char *> entry - the name of the entry to find the value of
	 *               <int> def - the default value in the event of a failed read
	 *               <char *> file_name - the name of the .ini file to read from
	 * Returns:      the value located at entry
 ***************************************************************************/

int GetPrivateProfileInt ( char *section, char *entry, int def, char *file_name) {
	FILE *fp;
	char buff[INI_MAX_LINE_LENGTH];
	char *ep;
	char t_section[INI_MAX_LINE_LENGTH];
	char value[6];
	int len = (int)strlen(entry);
	int i;

	fp = fopen(file_name,"r");
	if(!fp)  {
	  return(0);
	}

	sprintf(t_section,"[%s]",section);		// Format the section name

	//  Move through file 1 line at a time until a section is matched or EOF

	do	{		
		if( !read_line(fp,buff) ) {
			fclose(fp);
			return def;
		}
	} while( strcmp(buff,t_section) );

	printf("gppi: found section %s\n", t_section);

	// Now that the section has been found, find the entry.
	// Stop searching upon leaving the section's area

	do {
		if( !read_line(fp,buff) || buff[0] == '\0' ) {
			fclose(fp);
			return(def);
		}
	}  while( strncmp(buff,entry,len) );

	printf("gppi: found entry %s\n", entry);

	ep = strrchr(buff,'=');    // Parse out the equal sign
	ep++;

	if( !strlen(ep) )  {          // No setting?
		return(def);
	}

	// Copy only numbers fail on characters

	for(i=0; isdigit(ep[i]); i++ )  {
		value[i] = ep[i];
	}

	value[i] = '\0';
	fclose(fp);                // Clean up and return the value

	i = atoi(value);
	return i;
}


	/***************************************************************************
	 * Function:    write_private_profile_string()
	 * Arguments:   <char *> section - the name of the section to search for
	 *              <char *> entry - the name of the entry to find the value of
	 *              <char *> buffer - pointer to the buffer that holds the string
	 *              <char *> file_name - the name of the .ini file to read from
	 * Returns:     TRUE if successful, otherwise FALSE
	 ***************************************************************************/
	int WritePrivateProfileString(const char *section, const char *entry, const char *buffer, const char *file_name)  {
		FILE *rfp, *wfp;
		char tmp_name[15];
		char buff[INI_MAX_LINE_LENGTH];
		char t_section[INI_MAX_LINE_LENGTH];
		int len = (int)strlen(entry);
		strcpy(tmp_name, "utils.XXXXXX");
		mkstemp(tmp_name);
		//tmpnam(tmp_name); /* Get a temporary file name to copy to */
		sprintf(t_section,"[%s]",section);/* Format the section name */

		if( !(rfp = fopen(file_name,"r")) )  /* If the .ini file doesn't exist */
		{   if( !(wfp = fopen(file_name,"w")) ) /*  then make one */
			{   return(0);   }
			fprintf(wfp,"%s\n",t_section);
			fprintf(wfp,"%s=%s\n",entry,buffer);
			fclose(wfp);
			return(1);
		}

		if( !(wfp = fopen(tmp_name,"w")) ) {
			fclose(rfp);
			return(0);
		}

		/* Move through the file one line at a time until a section is
		 * matched or until EOF. Copy to temp file as it is read. */
		do
		{   if( !read_line(rfp,buff) )
			{   /* Failed to find section, so add one to the end */
				fprintf(wfp,"\n%s\n",t_section);
				fprintf(wfp,"%s=%s\n",entry,buffer);
				/* Clean up and rename */
				fclose(rfp);
				fclose(wfp);
				unlink(file_name);
				rename(tmp_name,file_name);
				return(1);
			}
			fprintf(wfp,"%s\n",buff);
		} while( strcmp(buff,t_section) );
		/* Now that the section has been found, find the entry. Stop searching
		 * upon leaving the section's area. Copy the file as it is read
		 * and create an entry if one is not found.  */
		while( 1 )
		{   if( !read_line(rfp,buff) )
			{   /* EOF without an entry so make one */
				fprintf(wfp,"%s=%s\n",entry,buffer);
				/* Clean up and rename */
				fclose(rfp);
				fclose(wfp);
				unlink(file_name);
				rename(tmp_name,file_name);
				return(1);

			}
			if( !strncmp(buff,entry,len) || buff[0] == '\0' )
				break;
			fprintf(wfp,"%s\n",buff);
		}
		if( buff[0] == '\0' )
		{   fprintf(wfp,"%s=%s\n",entry,buffer);
			do
			{
				fprintf(wfp,"%s\n",buff);
			} while( read_line(rfp,buff) );
		}
		else
		{   fprintf(wfp,"%s=%s\n",entry,buffer);
			while( read_line(rfp,buff) )
			{
				fprintf(wfp,"%s\n",buff);
			}
		}
		/* Clean up and rename */
		fclose(wfp);
		fclose(rfp);
		unlink(file_name);
		rename(tmp_name,file_name);
		return(1);
	}

	/*****************************************************************
	 * Function:     read_line()
	 * Arguments:    <FILE *> fp - a pointer to the file to be read from
	 *               <char *> bp - a pointer to the copy buffer
	 * Returns:      TRUE if successful FALSE otherwise
	 ******************************************************************/

	int read_line(FILE *fp, char *bp) {
		char c = '\0';
		int i = 0;
		/* Read one line from the source file */
		while( (c = getc(fp)) != '\n' )
		{   if( c == EOF )         /* return FALSE on unexpected EOF */
			return(0);
			bp[i++] = c;
		}
		bp[i] = '\0';
		return(1);
	}


	/*********************************************************************

	 *********************************************************************/

	void SetRect(RECT *_r, int l, int r, int t, int b)  {

		return;
	}


	/*********************************************************************

	 *********************************************************************/

	bool SetCurrentDirectory(char *dir)  {
		return true;
	}


	/*********************************************************************
	  in <unistd.h>

	  get_current_dir_name [getcwd] (3)  - Get current working directory
	  getcwd               (3)  - Get current working directory
	  getcwd [Cwd]         (3pm)  - get pathname of current working directory
	  getdents             (2)  - get directory entries
	  getdirentries        (3)  - get directory entries in a filesystem
	  independent format
	  getwd [getcwd]       (3)  - Get current working directory

	 *********************************************************************/

	unsigned long GetCurrentDirectory(int _size, char *_curdir)  {

		char *cptr = getcwd(_curdir, _size);
		if (cptr==0)  {
			// errno will be
			// EACCES
			// EINVAL
			// ENOMEM
			// ERANGE
			return 0;
		}
		//_curdir = "./";

		/*

			If the function succeeds, the return value specifies the number of characters
			written to the buffer, not including the terminating null character. 

			If the function fails, the return value is zero. To get extended error
			information, call GetLastError. 

			If the buffer pointed to by lpBuffer is not large enough, the return value
			specifies the required size of the buffer, including the number of bytes
			necessary for a terminating null character.

*/

		return strlen(_curdir);
	}




	/*********************************************************************

	 *********************************************************************/

	unsigned long timeGetTime(void)  {
		struct timeb now;
		//	int	dsec;
		//	int	dms;

		ftime(&now);

		//	dsec = now.time - OpenTime.time;
		//	dms = now.millitm - OpenTime.millitm;
		//	return dsec * 1000 + dms;

		return (now.time*1000 + now.millitm);
	}



	/**********************************************************************

	 **********************************************************************/

	void echo_off(void)  {
		struct termios new_settings;
		tcgetattr(0,&stored_settings);
		new_settings = stored_settings;
		new_settings.c_lflag &= (~ECHO);
		tcsetattr(0,TCSANOW,&new_settings);
		return;
	}

	/**********************************************************************

	 **********************************************************************/

	void echo_on(void)  {
		tcsetattr(0,TCSANOW,&stored_settings);
		return;
	}

	/**********************************************************************
	  from unix programing faq:

	  3.3 How can I check and see if a key was pressed?
	  =================================================

	  How can I check and see if a key was pressed? On DOS I use the
	  `kbhit()' function, but there doesn't seem to be an equivalent?

	  If you set the terminal to single-character mode (see previous answer),
	  then (on most systems) you can use `select()' or `poll()' to test for
	  readability.


	 **********************************************************************/

	void set_keypress(void)  {
		struct termios new_settings;

		tcgetattr(0,&stored_settings);

		new_settings = stored_settings;

		// Disable canonical mode, and set buffer size to 1 byte
		new_settings.c_lflag &= (~ICANON);
		new_settings.c_cc[VTIME] = 0;
		new_settings.c_cc[VMIN] = 1;

		tcsetattr(0,TCSANOW,&new_settings);
		return;
	}

	/**********************************************************************

	 **********************************************************************/

	void reset_keypress(void)  {
		tcsetattr(0,TCSANOW,&stored_settings);
		return;
	}

#if 0
	/**********************************************************************
	  strupr is a microsoft invention
	 **********************************************************************/

	char *strupr(char *str)  {
		for(int i=0; i<(int)strlen(str); i++)  {
			if (str[i]>='a' && str[i]<='z')  {
				str[i] = str[i] - 0x20;
			}
		}

		return str;
	}

	/**********************************************************************
	  strlwr is a microsoft invention
	**********************************************************************/

	char *strlwr(char *str)  {
		for(int i=0; i<(int)strlen(str); i++)  {
			if (str[i]>='A' && str[i]<='Z')  {
				str[i] = str[i] + 0x20;
			}
		}

		return str;
	}
#endif


/**********************************************************************
	http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1047077601&id=1045780608#04
**********************************************************************/


BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)  {

	if (mkdir(lpPathName, S_IRWXU) != 0)  {
		// mkdir( ) error
		return FALSE;
	}

	return TRUE;
}


/**********************************************************************
	typedef struct _SYSTEMTIME {
		 WORD wYear;
		 WORD wMonth;
		 WORD wDayOfWeek;
		 WORD wDay;
		 WORD wHour;
		 WORD wMinute;
		 WORD wSecond;
		 WORD wMilliseconds;
	} SYSTEMTIME, *PSYSTEMTIME;

    struct tm {
        int tm_sec;         // seconds
        int tm_min;         // minutes
        int tm_hour;        // hours
        int tm_mday;        // day of the month
        int tm_mon;         // month
        int tm_year;        // year
        int tm_wday;        // day of the week
        int tm_yday;        // day in the year
        int tm_isdst;       // daylight saving time
    };


**********************************************************************/

void GetLocalTime( SYSTEMTIME *st )  {
	struct tm *x;
	time_t aclock;

   time(&aclock);						// Get time in seconds
   x = localtime(&aclock);			// Convert time to struct tm form

	/*
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",
			x->tm_year+1900, x->tm_mon+1, x->tm_mday,
			x->tm_hour, x->tm_min, x->tm_sec);
	*/

	st->wYear = x->tm_year+1900;
	st->wMonth = x->tm_mon+1;
	st->wDayOfWeek = x->tm_wday;
	st->wDay = x->tm_mday;
	st->wHour = x->tm_hour;
	st->wMinute = x->tm_min;
	st->wSecond = x->tm_sec;
	st->wMilliseconds = 0;

	return;
}


/**********************************************************************

 **********************************************************************/

unsigned long filesize_from_name(const char *filename)  {
#ifdef WIN32
	HFILE hfile;
	unsigned long size;

	hfile = _lopen(filename, OF_READ);

	if (hfile == HFILE_ERROR)  {
		return 0xffffffff;
	}
	else  {
		size = GetFileSize((HANDLE)hfile,NULL);
		_lclose(hfile);

		// log(10,0,0,"%s file size = %ld\n",filename,size);

		return size;
	}
#else
	struct stat s;
	if (0==stat(filename, &s)) {
		return s.st_size;
	}
	return 0L;
#endif

}


#ifdef DOGTK

int MessageBox(int _hwnd, const char *_msg, const char *_title, unsigned long _style)  {
	GtkWidget *dialog;
	int win_button_style;
	int win_type;
	GtkMessageType gtktype;

	gtk_init(NULL, NULL);

	/*

		#define MB_OK                       0x00000000L
		#define MB_OKCANCEL                 0x00000001L
		#define MB_ABORTRETRYIGNORE         0x00000002L
		#define MB_YESNOCANCEL              0x00000003L
		#define MB_YESNO                    0x00000004L
		#define MB_RETRYCANCEL              0x00000005L

		#define MB_ICONHAND                 0x00000010L
		#define MB_ICONQUESTION             0x00000020L
		#define MB_ICONEXCLAMATION          0x00000030L
		#define MB_ICONASTERISK             0x00000040L

		typedef enum  {
		  GTK_MESSAGE_INFO,
		  GTK_MESSAGE_WARNING,
		  GTK_MESSAGE_QUESTION,
		  GTK_MESSAGE_ERROR,
		  GTK_MESSAGE_OTHER
		} GtkMessageType;
	*/

	win_button_style = _style & 0x0000000f;
	win_type = _style & 0x000000f0;

	switch(win_type)  {
		case MB_ICONHAND:
			gtktype = GTK_MESSAGE_INFO;
			break;
		case MB_ICONQUESTION:
			gtktype = GTK_MESSAGE_INFO;
			break;
		case MB_ICONEXCLAMATION:
			gtktype = GTK_MESSAGE_INFO;
			break;
		case MB_ICONASTERISK:
			gtktype = GTK_MESSAGE_INFO;
			break;
		default:
			gtktype = GTK_MESSAGE_INFO;
			break;
	}

	switch(win_button_style)  {
		case MB_OK:
		default:
			dialog = gtk_message_dialog_new( (GtkWindow *) 0, GTK_DIALOG_DESTROY_WITH_PARENT, gtktype, GTK_BUTTONS_OK, _msg, "xxx");
			break;
	}

	gtk_window_set_title(GTK_WINDOW(dialog), _title);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	//gtk_main();


	return MB_OK;
}

#endif				// #ifdef DOGTK



/**************************************************************************
	http://rabbit.eng.miami.edu/info/functions/unixio.html
**************************************************************************/

bool is_open(const char *fname)  {
	if (fname==NULL)  {
		return false;
	}

	if (!exists(fname))  {
		return false;
	}

	/*
	int status;
	struct stat st = {0};
	status = stat(fname, &st);
	if (status==-1)  {
		return false;
	}
	*/

	/*
	FILE *stream = fopen(fname, "rx");
	if (stream==0)  {
		return true;
	}
	fclose(stream);
	*/

#if 0
	char *cptr;
	char buf[PATH_MAX];
	cptr = realpath(fname, buf);

But there is no such function call. To make one, you would first need to get
the full path of a given file ath. This can be done with the realpath (3)
library function. Then scan all /proc/[0-9]\*/fd/\* and call readlink (2) on each symlink found and
compare with the full path you got from calling realpath (3) in the first step. If there is a match the file is open.

#endif


	return false;
}


/***********************************************************************
	http://linux.die.net/man/2/fstat
***********************************************************************/

bool direxists(const char *path)  {
	struct stat st = {0};
	int status;

	status = stat(path, &st);

	if (status==-1)  {
		return false;
	}

	return true;

}

/***********************************************************************

***********************************************************************/

void mybeep(void)  {

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

	return;
}

/***********************************************************************

***********************************************************************/

void alert(void)  {

	return;
}

/***********************************************************************

***********************************************************************/
/*
double deq(double d1, double d2, double delta)   {


}
*/

/**********************************************************************

**********************************************************************/

void send(Serial *port, char *str, FILE *_stream, int _flush_flag)  {
	int i=0;

	//logg->write(0,0,1,"\n****************************\n");
	//logg->write(0,0,1,"send:   %s   (len = %d)\n",str,strlen(str));

	while(str[i])  {
		port->tx( (const unsigned char *) &str[i++], 1);
	}

	if (_flush_flag)  {
		port->flush();
	}

/*
	if (port->txasleep)  {
		SetEvent(port->txControlEvent);				// wake up transmitter thread
	}
*/

	return;
}


/**********************************************************************

**********************************************************************/

#if 0
int getmyip(void)  {
	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];
	int ix;


	if(getifaddrs(&myaddrs) != 0) {
		perror("getifaddrs");
		exit(1);
	}

	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)  {
			continue;
		}

		if (!(ifa->ifa_flags & IFF_UP))  {
			continue;
		}

		switch (ifa->ifa_addr->sa_family) {
			case AF_INET: {
								  struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
								  in_addr = &s4->sin_addr;
								  break;
							  }

			case AF_INET6: {
									struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
									in_addr = &s6->sin6_addr;
									break;
								}

			default:
								continue;
		}

		if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf))) {
			printf("%s: inet_ntop failed!\n", ifa->ifa_name);
			continue;
		}

		/*
lo: 127.0.0.1
eth0: 192.168.1.20
wlan0: 192.168.1.21
lo: ::1
eth0: fe80::f2bf:97ff:fe57:d59b
wlan0: fe80::4225:c2ff:fe26:fe78
		 */
		ix = myindex((const char *)ifa->ifa_name, "lo");
		if (ix != -1)  {
			continue;
		}

		ix = myindex((const char *)buf, "::");
		if (ix != -1)  {
			continue;
		}

		printf("%s: %s\n", ifa->ifa_name, buf);

		ix = myindex((const char *)ifa->ifa_name, "eth");
		if (ix == 0)  {
			//break;
		}

		ix = myindex((const char *)ifa->ifa_name, "wlan");
		if (ix == 0)  {
		}

		IFACE intf;
		strncpy(intf.name, ifa->ifa_name, sizeof(intf.name)-1);
		strncpy(intf.addr, buf, sizeof(intf.addr)-1);
		ifaces.push_back(intf);
	}										// for()


	freeifaddrs(myaddrs);

	for(int i=0; i<ifaces.size(); i++)  {
		logg(true, "%d   %s   %s\n", i+1, ifaces[i].name, ifaces[i].addr);
	}

	if (ifaces.size() > 0)  {
		strncpy(myip, ifaces[0].addr, sizeof(myip)-1);
	}

	return 0;
}											// void getmyip(void)  {

#endif

/*******************************************************************************

*******************************************************************************/

#if 0
void dprintf(const char *format, ...)  {
	#ifdef _DEBUG
		va_list ap;													// Argument pointer
		char s[1024];												// Output string
		int len;

		len = (int)strlen(format);
		if (len>1023)  {
			return;
		}
		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);
		printf("%s", s);
	#else
		/*
		va_list ap;													// Argument pointer
		char s[1024];												// Output string
		int len;

		len = strlen(format);
		if (len>1023)  {
			return;
		}
		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);
		printf("%s", s);
		*/
	#endif

	return;
}
#endif


