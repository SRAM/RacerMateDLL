
#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
	#define STRICT
	#define WIN32_LEAN_AND_MEAN
#else
#endif

#include <stdlib.h>
#include <stdio.h>

#include <glib_defines.h>
#include <utils.h>
#include <ini.h>

//extern "C" {

/**********************************************************************

**********************************************************************/

Ini::Ini(const char *_fname)  {

//#ifdef WIN32
	DWORD dw;

	if (indexIgnoreCase( _fname, "\\", 0) != -1)  {
	//if (indexIgnoreCase( _fname, FILESEP, 0) != -1)  {
		strncpy(path, _fname, sizeof(path)-1);
	}
	else  {
		dw = GetCurrentDirectory(120L, curdir);		// curdir is char[128] !!!!
		if (dw==0)  {
			//int bp = 1;
		}

		strcpy(path, curdir);
		char sepstr[4] = {0};
		sepstr[0] = FILESEPCHAR;
		strcat(path, sepstr);
		strncat(path, _fname, 127);

	}

	if (!exists(path))  {
		//throw("can't find .ini file");
		FILE *stream = fopen(path, "wt");
		fclose(stream);
	}

	bp = 0;
//#endif
	memset(cursection, 0, sizeof(cursection));

	return;

}


/**********************************************************************

 **********************************************************************/

Ini::~Ini()  {


}

/**********************************************************************
  always creates a .ini entry if the item doesn't exist
 **********************************************************************/

bool Ini::getBool(const char *section, const char *key, bool b)  {
//#ifdef WIN32

	if (b) strcpy(def, "TRUE");
	else strcpy(def, "FALSE");

	GetPrivateProfileString(
			section,
			key,
			def, 
			buf,
			255,
			path);

	WritePrivateProfileString(section, key, buf, path);

	strupr(buf);

	return (strcmp(buf, "TRUE") == 0);
//#else
//	return false;
//#endif
}

/**********************************************************************

**********************************************************************/

bool Ini::getBool(const char *key, bool b)  {


	if (cursection[0] == 0)  {
		return false;
	}

	if (b) strcpy(def, "TRUE");
	else strcpy(def, "FALSE");

	GetPrivateProfileString(
			cursection,
			key,
			def, 
			buf,
			255,
			path);

	WritePrivateProfileString(cursection, key, buf, path);

	strupr(buf);

	return (strcmp(buf, "TRUE") == 0);
}

/**********************************************************************
  always creates a .ini entry if the item doesn't exist
 **********************************************************************/

bool Ini::getBool(const char *section, const char *key, const char * def)  {

//#ifdef WIN32


	GetPrivateProfileString(
			section,
			key,
			def, 
			buf,
			255,
			path);

	WritePrivateProfileString(section, key, buf, path);

	strupr(buf);

	return (strcmp(buf, "TRUE") == 0);
//#else
//	return false;
//#endif
}


/**********************************************************************

 **********************************************************************/

bool Ini::getBool(const char *section, const char *key, const char * def, bool makeIfNotThere)  {
	//	DWORD dw;


//#ifdef WIN32
	GetPrivateProfileString(
			section,
			key,
			def, 
			buf,
			255,
			path);

	if (makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}

	strupr(buf);

	return (strcmp(buf, "TRUE") == 0);
//#else
//	return false;
//#endif

}

/**********************************************************************

 **********************************************************************/

bool Ini::getBool(const char *section, const char *key, bool def, bool makeIfNotThere)  {
	//	DWORD dw;


//#ifdef WIN32
	GetPrivateProfileString(
			section,
			key,
			def?"true":"false", 
			buf,
			255,
			path);

	if (makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}

	strupr(buf);

	return (strcmp(buf, "TRUE") == 0);
//#else
//	return false;
//#endif

}


/**********************************************************************

**********************************************************************/

float Ini::getFloat(const char *key, float f)  {


	if (cursection[0] == 0)  {
		return 0.0f;
	}

	sprintf(def, "%f", f);

	dw = GetPrivateProfileString(cursection, key, def, buf, 255, path );
	WritePrivateProfileString(cursection, key, buf, path);

	float ftemp = (float)atof(buf);
	return ftemp;
}

/**********************************************************************

**********************************************************************/

float Ini::getFloat(const char *section, const char *key, float f)  {

	sprintf(def, "%f", f);

//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );
	WritePrivateProfileString(section, key, buf, path);

	float ftemp = (float)atof(buf);
	return ftemp;
//#else
//	return false;
//#endif
}

/**********************************************************************

 **********************************************************************/

float Ini::getFloat(const char *section, const char *key, const char *def)  {

	//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );
	WritePrivateProfileString(section, key, buf, path);

	float ftemp = (float)atof(buf);
	return ftemp;
//#else
//	return false;
//#endif
}

/**********************************************************************

 **********************************************************************/

float Ini::getFloat(const char *section, const char *key, const char *def, bool makeIfNotThere)  {


//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );

	if (makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}


	float ftemp = (float)atof(buf);
	return ftemp;
//#else
//	return false;
//#endif
}

/**********************************************************************

**********************************************************************/

int Ini::getInt(const char *key, const char *def)  {

	if (cursection[0] == 0)  {
		return 0;
	}

	dw = GetPrivateProfileString(cursection, key, def, buf, 255, path );
	WritePrivateProfileString(cursection, key, buf, path);
	int itemp = atoi(buf);
	return itemp;
}

/**********************************************************************

**********************************************************************/

int Ini::getInt(const char *key, int _i)  {
	sprintf(def, "%d", _i);

	dw = GetPrivateProfileString(cursection, key, def, buf, 255, path );
	int itemp = atoi(buf);

	return itemp;
}

/**********************************************************************

**********************************************************************/

int Ini::getInt(const char *section, const char *key, const char *def)  {
//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );
	WritePrivateProfileString(section, key, buf, path);


	int itemp = atoi(buf);

	return itemp;
//#else
//	return false;
//#endif
}

/**********************************************************************

**********************************************************************/

int Ini::getInt(const char *section, const char *key, const char *def, bool makeIfNotThere)  {
//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );

	if (makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}


	int itemp = atoi(buf);

	return itemp;
//#else
//	return false;
//#endif
}

/**********************************************************************

**********************************************************************/

int Ini::getInt(const char *section, const char *key, int i, bool _makeIfNotThere)  {
	sprintf(def, "%d", i);
//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );
	//WritePrivateProfileString(section, key, buf, path);

	if (_makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}

	int itemp = atoi(buf);

	return itemp;
//#else
//	return false;
//#endif
}

/**********************************************************************

**********************************************************************/

unsigned long Ini::getul(const char *section, const char *key, unsigned long i, bool _makeIfNotThere)  {
	unsigned long itemp;
	int n;

	sprintf(def, "%lu", i);

	dw = GetPrivateProfileString(section, key, def, buf, 255, path );

	if (_makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}

	n = sscanf(buf, "%lu", &itemp);
	if (n==1)  {
		return itemp;
	}
	
	return 0L;
}


/**********************************************************************

**********************************************************************/

const char * Ini::getString(const char *section, const char *key, const char *def)  {
	BOOL b;
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );
	b = WritePrivateProfileString(section, key, buf, path);
	if (b==FALSE)  {

	}
	return buf;
}

/**********************************************************************

**********************************************************************/

const char * Ini::getString(const char *key, const char *def)  {
	BOOL b;
	if (cursection[0] == 0)  {
		return NULL;
	}

	dw = GetPrivateProfileString(cursection, key, def, buf, 255, path );
	b = WritePrivateProfileString(cursection, key, buf, path);
	if (b==FALSE)  {

	}
	return buf;
}

/**********************************************************************

 **********************************************************************/

const char * Ini::getString(const char *section, const char *key, const char *def, bool makeIfNotThere)  {
	BOOL b;
	buf[0] = 0;

//#ifdef WIN32
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );

	if (dw==0)  {
		if (makeIfNotThere)  {
			WritePrivateProfileString(section, key, buf, path);
		}
	}
	else  {
		b = WritePrivateProfileString(section, key, buf, path);
		if (b==FALSE)  {

		}
	}

	//lint -e1536
	return buf;
//#else
//	return false;
//#endif
}

/**********************************************************************

 **********************************************************************/

void Ini::writeString(const char *section, const char *key, const char *str)  {

//#ifdef WIN32
	WritePrivateProfileString(section, key, str, path);
//#endif

	return;
}


/**********************************************************************

**********************************************************************/

void Ini::writeBool(const char *section, const char *key, BOOL b)  {

//#ifdef WIN32
	if (b) {
		WritePrivateProfileString(section, key, "true", path);
	}
	else  {
		WritePrivateProfileString(section, key, "false", path);
	}
//#endif
	return;
}

/**********************************************************************

**********************************************************************/

void Ini::writeBool(const char *key, BOOL b)  {

	if (cursection[0] == 0)  {
		return;
	}

	if (b) {
		WritePrivateProfileString(cursection, key, "true", path);
	}
	else  {
		WritePrivateProfileString(cursection, key, "false", path);
	}

	return;
}

/**********************************************************************

**********************************************************************/

void Ini::set_section(const char *_sec)  {
	strncpy(cursection, _sec, sizeof(cursection)-1);
	return;
}

/**********************************************************************

**********************************************************************/

void Ini::writeInt(const char *section, const char *key, int i)  {
	char str[32];

	sprintf(str, "%d", i);

//#ifdef WIN32
	WritePrivateProfileString(section, key, str, path);
//#endif
	return;
}

/**********************************************************************

**********************************************************************/

void Ini::writeInt(const char *key, int i)  {
	char str[32];

	sprintf(str, "%d", i);
	WritePrivateProfileString(cursection, key, str, path);

	return;
}

/**********************************************************************

**********************************************************************/

void Ini::writeul(const char *section, const char *key, unsigned long i)  {
	char str[32];

	sprintf(str, "%lu", i);

//#ifdef WIN32
	WritePrivateProfileString(section, key, str, path);
//#endif
	return;
}

/**********************************************************************

**********************************************************************/

void Ini::write_long_hex(const char *section, const char *key, DWORD _dw)  {
	char str[32];

	sprintf(str, "%08lx", _dw);

//#ifdef WIN32
	WritePrivateProfileString(section, key, str, path);
//#endif
	return;
}

/**********************************************************************

 **********************************************************************/

void Ini::writeFloat(const char *section, const char *key, float f)  {
	char str[32];

	sprintf(str, "%f", f);

//#ifdef WIN32
	WritePrivateProfileString(section, key, str, path);
//#endif
	return;
}

/**********************************************************************

 **********************************************************************/

void Ini::writeFloat(const char *key, float f)  {
	char str[32];

	sprintf(str, "%f", f);
	WritePrivateProfileString(cursection, key, str, path);
	return;
}



/**********************************************************************

 **********************************************************************/

const char *Ini::getName(void)  {
	return path;
}

/**********************************************************************
  _section should have surrounding brackets
  deletes an entire section from the .ini file
 **********************************************************************/

void Ini::clear_section(const char *_section)  {
	//	DWORD dw;
	//	char buf2[2048];
	//	dw = GetPrivateProfileSectionNames(buf2, sizeof(buf2), path);
	//	int bp = 2;

	char *cptr;
	bool insection = false;
#define TMPININM "lkj.lkj"

	FILE *instream = fopen(path, "a+t");
	FILE *outstream = fopen(TMPININM, "wt");

	while (1)  {
		cptr = fgets(buf, sizeof(buf)-1, instream);
		if (cptr==NULL)  {
			break;
		}
		trimcrlf(cptr);
		ltrim(cptr, 1);

		if (cptr[0]=='[')  {
			if (strcmp(_section, cptr)==0)  {
				insection = true;
			}
			else  {
				insection = false;
			}
		}

		if (insection)  {
			continue;
		}

		fprintf(outstream, "%s\n", cptr);
	}

	fclose(instream);
	fclose(outstream);

//#ifdef WIN32
	CopyFile(TMPININM, path, FALSE);
//#endif

	unlink(TMPININM);

	return;
}




//(((((((((((((((((((((((((((((((((((((((((((((((((((((((((
#if 0
/******************************************************************************
  PORTABLE ROUTINES FOR WRITING PRIVATE PROFILE STRINGS --  by Joseph J. Graf
  Header file containing prototypes and compile-time configuration.
******************************************************************************/

#define MAX_LINE_LENGTH    80

int get_private_profile_int(const char *, const char *, int,    const char *);
int get_private_profile_string(const char *, const char *, const char *, const char *, int, const char *);
int write_private_profile_string(const char *, const char *, const char *, const char *);

Listing Two

/***** Routines to read profile strings --  by Joseph J. Graf ******/
#include <stdio.h>
#include <string.h>
#include "profport.h"   /* function prototypes in here */

/*****************************************************************
 * Function:     read_line()
 * Arguments:    <FILE *> fp - a pointer to the file to be read from
 *               <const char *> bp - a pointer to the copy buffer
 * Returns:      TRUE if successful FALSE otherwise
 ******************************************************************/
int read_line(FILE *fp, const char *bp)
{   char c = '\0';
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

/**************************************************************************
 * Function:     get_private_profile_int()
 * Arguments:    <const char *> section - the name of the section to search for
 *               <const char *> entry - the name of the entry to find the value of
 *               <int> def - the default value in the event of a failed read
 *               <const char *> file_name - the name of the .ini file to read from
 * Returns:      the value located at entry
***************************************************************************/

int get_private_profile_int(
		const char *section,
		const char *entry, int def, const char *file_name) {
	FILE *fp = fopen(file_name,"r");
	char buff[MAX_LINE_LENGTH];
	const char *ep;
	char t_section[MAX_LINE_LENGTH];
	char value[6];
	int len = strlen(entry);
	int i;
	if( !fp ) return(0);
	sprintf(t_section,"[%s]",section); /* Format the section name */
	/*  Move through file 1 line at a time until a section is matched or EOF */
	do
	{   if( !read_line(fp,buff) )
		{   fclose(fp);
			return(def);
		}
	} while( strcmp(buff,t_section) );
	/* Now that the section has been found, find the entry.
	 * Stop searching upon leaving the section's area. */
	do
	{   if( !read_line(fp,buff) || buff[0] == '\0' )
		{   fclose(fp);
			return(def);
		}
	}  while( strncmp(buff,entry,len) );
	ep = strrchr(buff,'=');    /* Parse out the equal sign */
	ep++;
	if( !strlen(ep) )          /* No setting? */
		return(def);
	/* Copy only numbers fail on characters */

	for(i = 0; isdigit(ep[i]); i++ )
		value[i] = ep[i];
	value[i] = '\0';
	fclose(fp);                /* Clean up and return the value */
	return(atoi(value));
}

/**************************************************************************
 * Function:     get_private_profile_string()
 * Arguments:    <const char *> section - the name of the section to search for
 *               <const char *> entry - the name of the entry to find the value of
 *               <const char *> def - default string in the event of a failed read
 *               <const char *> buffer - a pointer to the buffer to copy into
 *               <int> buffer_len - the max number of characters to copy
 *               <const char *> file_name - the name of the .ini file to read from
 * Returns:      the number of characters copied into the supplied buffer
 ***************************************************************************/
int get_private_profile_string(const char *section, const char *entry, const char *def,
		const char *buffer, int buffer_len, const char *file_name)
{   FILE *fp = fopen(file_name,"r");
	char buff[MAX_LINE_LENGTH];
	const char *ep;
	char t_section[MAX_LINE_LENGTH];
	int len = strlen(entry);
	if( !fp ) return(0);
	sprintf(t_section,"[%s]",section);    /* Format the section name */
	/*  Move through file 1 line at a time until a section is matched or EOF */
	do
	{   if( !read_line(fp,buff) )
		{   fclose(fp);
			strncpy(buffer,def,buffer_len);    
			return(strlen(buffer));
		}
	}
	while( strcmp(buff,t_section) );
	/* Now that the section has been found, find the entry.
	 * Stop searching upon leaving the section's area. */
	do
	{   if( !read_line(fp,buff) || buff[0] == '\0' )
		{   fclose(fp);
			strncpy(buffer,def,buffer_len);    
			return(strlen(buffer));
		}
	}  while( strncmp(buff,entry,len) );
	ep = strrchr(buff,'=');    /* Parse out the equal sign */
	ep++;
	/* Copy up to buffer_len chars to buffer */
	strncpy(buffer,ep,buffer_len - 1);

	buffer[buffer_len] = '\0';
	fclose(fp);               /* Clean up and return the amount copied */
	return(strlen(buffer));

}

Listing Three

/***** Routine for writing private profile strings --- by Joseph J. Graf *****/
#include <stdio.h>
#include <string.h>
#include "profport.h"

/***************************************************************************
 * Function:    write_private_profile_string()
 * Arguments:   <const char *> section - the name of the section to search for
 *              <const char *> entry - the name of the entry to find the value of
 *              <const char *> buffer - pointer to the buffer that holds the string
 *              <const char *> file_name - the name of the .ini file to read from
 * Returns:     TRUE if successful, otherwise FALSE
 ***************************************************************************/
int write_private_profile_string(const char *section,
		const char *entry, const char *buffer, const char *file_name)

{   FILE *rfp, *wfp;
	char tmp_name[15];
	char buff[MAX_LINE_LENGTH];
	char t_section[MAX_LINE_LENGTH];
	int len = strlen(entry);
	tmpnam(tmp_name); /* Get a temporary file name to copy to */
	sprintf(t_section,"[%s]",section);/* Format the section name */
	if( !(rfp = fopen(file_name,"r")) )  /* If the .ini file doesn't exist */
	{   if( !(wfp = fopen(file_name,"w")) ) /*  then make one */
		{   return(0);   }
		fprintf(wfp,"%s\n",t_section);
		fprintf(wfp,"%s=%s\n",entry,buffer);
		fclose(wfp);
		return(1);
	}
	if( !(wfp = fopen(tmp_name,"w")) )
	{   fclose(rfp);
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
#endif


/**********************************************************************

**********************************************************************/

unsigned long Ini::get_long_hex(const char *section, const char *key, unsigned long uldef, bool makeIfNotThere)  {

	char def[16];
	unsigned long dw;

	sprintf(def, "%lx", uldef);
	dw = GetPrivateProfileString(section, key, def, buf, 255, path );

	if (makeIfNotThere)  {
		WritePrivateProfileString(section, key, buf, path);
	}

	//int itemp = atoi(buf);
	//return itemp;

	int n = sscanf(buf, "%lx", &dw);

	if (n==1)  {
		return dw;
	}
	else  {
		return 0;
	}

}
//}			// extern "C" {
