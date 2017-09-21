

#ifdef WIN32
    #pragma warning(disable:4996)					// for vista strncpy_s
	#define STRICT
	#define WIN32_LEAN_AND_MEAN
#else
	#include <string.h>
	#include <time.h>
	#include <unistd.h>
	#include <ctype.h>
	#include <stdarg.h>			// for va_start
#endif

#include <comglobs.h>
#include <globals.h>
#include <oldsdirs.h>
#include <utils.h>
#include <logger.h>
#include <assert.h>

#ifdef _DEBUG
	#include <err.h>
#endif


//extern "C" {

	int sort_function(const void *a, const void *b);

	/**********************************************************************

	**********************************************************************/


	Logger::Logger(const char *stubname, const char *_folder)  {

		#ifdef WIN32
		SYSTEMTIME x;
		#else
		struct tm *x;
		time_t aclock;
		#endif
		
		strncpy(stub, stubname, sizeof(stub)-1);
		strncpy(folder, _folder, sizeof(folder)-1);
		//strcat(stub, ".log");

		if (strlen(folder) != 1)  {
			#ifdef WIN32
			//	myreplace(folder, "/", "\\");
			#endif

			if (!direxists(folder))  {
				CreateDirectory(folder, NULL);
			}
			strncpy(stub, _folder, sizeof(stub)-1);
			#ifdef WIN32
				#define DELIMITER "\\"
			#else
				#define DELIMITER "/"
			#endif

			if ( stub[strlen(stub)-1] != DELIMITER[0] )  {
				strcat(stub, DELIMITER);
			}
			strcat(stub, stubname);
		}
		else  {
			//int bp = 1;
		}

		#ifdef WIN32
		GetLocalTime(&x);
		sprintf(file,"%02d%02d%02d%02d.log",x.wMonth,x.wDay,x.wHour,x.wMinute);
		#else
		time(&aclock);						// Get time in seconds
		x = localtime(&aclock);			// Convert time to struct tm form
		sprintf(file,"%02d%02d%02d%02d.log",x->tm_mon+1, x->tm_mday, x->tm_hour, x->tm_min);
		#endif


		// open stub, so that more than one object in a program can open a log file!
		// (instead of "file").

		stream = fopen(stub, "wt");
		if (!stream)  {
#ifdef _DEBUG
	#ifdef WIN32
			Err *err = new Err();
			DEL(err);
	#endif
#endif

			loglevel = 10;

			#ifdef WIN32
				GetCurrentDirectory(_MAX_PATH-1,currentDirectory);
			#else
				getcwd( currentDirectory, _MAX_PATH );
			#endif
		}

		//fprintf(stream, "%s open\n", stub);
		//fprintf(stream, "stub = %s\n\n", stub);

		loglevel = 0;

		//GetCurrentDirectory(_MAX_PATH-1,currentDirectory);
		#ifdef WIN32
			GetCurrentDirectory(_MAX_PATH-1,currentDirectory);
		#else
			getcwd( currentDirectory, _MAX_PATH );
		#endif

strcpy(currentDirectory, folder);

		closed = FALSE;
		//write(10,0,1,"%s is open\n", stub);

		return;
	}


	/**********************************************************************

	**********************************************************************/

	Logger::~Logger()  {

#ifdef _DEBUG
		if (indexIgnoreCase(stub, "decoder")!= -1)  {
			//int bp = 2;
		}
#endif

		if (stream)  {
			close();
		}

		cleanup_logfiles();							// COMMENTED OUT DUE TO MEMORY LEAK TLM2002???
		
		//assert((dogtag).isValid());				// destructor will automatically do this
		stream = NULL;

	}



	/*
	void Logger::write(char *format, ...)  {
		write(10,0,0,format);
		return;
	}
	*/

	/**********************************************************************

	**********************************************************************/

	void Logger::close(void)  {

	char string[256];		// for debugging

		if (stream)  {
			strcpy(string, stub);
			FCLOSE(stream);
		}

		else  {
			stream = fopen(stub, "a+t");
			//fprintf(stream,"\nlogfile closed, reopened to close\n");
			fclose(stream);
			stream = NULL;
		}


		#ifdef WIN32
		/*
		// can't do this any more because I now allow use of multiple log files in the same
		// program, so "file" may be the same for many Loggers.
		if (!CopyFile(
					stub,						// existing file
					file,						// new file
					FALSE))  {
     		write(10,1,1,"copy file error %ld\n", GetLastError());
		}
		*/
			//fcopy(stub, file);
		#else
			#ifdef WIN32
			#endif
		#endif

		closed = TRUE;

		return;
	}


	/**********************************************************************
		entry:

	**********************************************************************/

	#ifndef WIN32

	void Logger::cleanup_logfiles(void)  {
		return;
	}

	#endif

	#ifdef WIN32

	void Logger::cleanup_logfiles(void)  {
		WIN32_FIND_DATA FindFileData = {0};
		HANDLE handle;
		BOOL bstatus;
		int count = 0;
		char *array;
		int i,n;
		char fname[256];
		char path[260];

#ifdef DOGLOBS
		if (dirs.size()==0)  {
			return;
		}
		sprintf(path, "%s%s*.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
#else
		if (SDIRS::dirs.size()==0)  {
			return;
		}
		sprintf(path, "%s%s*.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR);
#endif



		//handle = FindFirstFile("*.log",&FindFileData);
		handle = FindFirstFile(path, &FindFileData);

		if (handle == INVALID_HANDLE_VALUE)  {
			//write(10,1,0,"bad FindFirstFile handle\n");
			return;
		}


		array = new char[1000*13];							// allow for 1000 8.3 files
		if (array==NULL)  {
			//write(10,1,1,"error allocating memory in cleanup_logfiles\n");
			FindClose(handle);
			return;
		}


		//write(0,0,0,"\n\n*.LOG FILES:\n\n");

		if (FindFileData.cAlternateFileName[0] == 0)  {
			strncpy(fname, FindFileData.cFileName, 15);
		}
		else  {
			strncpy(fname, FindFileData.cAlternateFileName, 15);
		}


		if (isdigit(fname[0]))  {
			//write(0,1,0,"%s\n",FindFileData.cAlternateFileName);
			strncpy(&array[count*13], fname, 13);
			count++;
		}



		while(1)  {
			bstatus = FindNextFile(handle,&FindFileData);
			if (!bstatus)  {
				break;								// no more files
			}
			if (FindFileData.cAlternateFileName[0] == 0)  {
				strncpy(fname, FindFileData.cFileName, 15);
			}
			else  {
				strncpy(fname, FindFileData.cAlternateFileName, 15);
			}

			if (fname[0]==NULL)  {
				//write(0,1,0,"no more files found\n");
				break;
			}

			if (isdigit(fname[0]))  {
				//write(0,1,0,"%s\n",FindFileData.cAlternateFileName);
				strncpy(&array[count*13],fname,13);
				count++;
				if (count==999)  {
					//write(0,1,1,"error: reached max file limit\n");
					break;
				}
			}
		}

		FindClose(handle);

		//write(0,1,0,"log file count = %d\n",count);
		n = count - 10;

	//----------------------------------------------------------------------
	// IF WE HAVE MORE THAN 10 FILES, THEN DEL THE FIRST COUNT-10 FILES
	// BUT WE NEED TO SORT THE FILES
	//----------------------------------------------------------------------

		qsort((void *)array, (size_t)count, (size_t)13, sort_function);

		for(i=0;i<count;i++)  {
			//write(0,1,0,"%4d - %s",i,&array[i*13]);
			if (i<n)  {
				DeleteFile(&array[i*13]);
				//write(0,0,0," (deleted)\n");
			}
			else  {
				//write(0,0,0,"\n");
			}
		}


		delete[] array;

		//write(0,0,0,"\n");

		return;
	}

	#else

	/**********************************************************************
		f.name
		f.size
	**********************************************************************/

	typedef struct {
		char name[13];
	} DIR;

	#ifdef WIN32

	void Logger::cleanup_logfiles(void)  {
		struct _find_t  f;
		int k = 0;
		DIR *buf;
		int i;
		char str[8];
		int status;
		unsigned int size = 20;



		buf = (DIR *) malloc(size*13);			// allocat the first 20 array elements
		if (buf==NULL)  {
			write(10, 0, 1, "allocation error in cleanup_logfiles\n");
			return;
		}

		//sprintf(str, "*.%03d", regnum);
		strcpy(str, "*.log");

		status = _dos_findfirst(str, _A_NORMAL, &f);
		if (status)  {
			free(buf);
			return;
		}

		if (islogfile(f.name))  {
			strcpy(buf[k++].name, f.name);
		}


		while(_dos_findnext(&f) == 0)  {
			if (islogfile(f.name))  {
				strcpy(buf[k++].name, f.name);
				if (k==(int)size)  {
					size += 20;

					buf = (DIR *)realloc(buf, size*sizeof(DIR));		// another 20

					if (buf==NULL)  {
						write(10, 0, 1, "\nre-allocation error\n");
						free(buf);
						return;
					}
				}
			}
		}


		/***********************************
		logg(0,0,"\nbefore qsort:\n");
		for(i=0;i<k;i++)  {
			logg(0,0,"%3d  %s\n", i, buf[i].name);
		}
		***********************************/


		qsort((void *)buf, (size_t)k, (size_t)sizeof(DIR), compare);


		for(i=0;i<k;i++)  {
			if (i < (k-10) )  {
				write(0, 0, 0, "%3d  %s (deleting)\n", i, buf[i].name);
				rm(buf[i].name);
			}
			else  {
				write(0,0,0,"%3d  %s\n", i, buf[i].name);
			}
		}


		free(buf);

		return;
	}
	#endif


	/************************************************************************

	************************************************************************/

	int islogfile(char *fname)  {
		int i;

		if (strlen(fname)!=12)  {
			return 0;
		}

		for(i=0;i<8;i++)  {
			if (!isdigit(fname[i]))  {
				return 0;
			}
		}

		for(i=9;i<12;i++)  {
			if (!isdigit(fname[i]))  {
				return 0;
			}
		}

		return 1;
	}

	/**********************************************************************

	**********************************************************************/

	int compare(const void *a, const void *b)  {
		//return ( strcmp((char *)a, (char *)b) );
	#ifdef WIN32
		return ( _fstrcmp((char *)a, (char *)b) );
	#else
		return ( strcmp((char *)a, (char *)b) );
	#endif
		
	}


	#endif

	/**********************************************************************

	**********************************************************************/

	int sort_function(const void *a, const void *b)  {
		return (strcmp((char *)a, (char *)b));
	}


	/**********************************************************************

	**********************************************************************/

	void Logger::dump(unsigned char *mb, int cnt)  {
		int fullrows, remainder;
		int i, j, k;
			
			
		fullrows = (cnt/16);
		remainder = (cnt - fullrows*16);


		for (i=0; i<fullrows; i++)  {
			for (j=0;j<16;j++)  {
				k = i*16 + j;
   			fprintf(stream, "%02x ", mb[k]);
			}
    				
			fprintf(stream, "    ");
    				
			for (j=0;j<16;j++)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					fprintf(stream, ".");
				}
				else  {
   				fprintf(stream, "%c", (char)mb[k]);
				}
			}
    				
    		fprintf(stream, "\n");
		}

		//----------------------
		// now the remainder:
		//----------------------
            
            
		for (j=0;j<16;j++)  {
			k = i*16 + j;
			if (j<remainder)  {
				fprintf(stream, "%02x ", mb[k]);
			}
			else  {
				fprintf(stream,"   ");
			}
		}
    			
		fprintf(stream,"    ");
    				
		for (j=0;j<16;j++)  {
	  		if (j<remainder)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					fprintf(stream, ".");
				 }
				 else  {
    				fprintf(stream, "%c", (char)mb[k]);
				}
			}
			else  {
				//fprintf(stream," ");
			}
		}
    			
		fprintf(stream, "\n");
	}

	/**********************************************************************

		log levels:
		-----------

		 0		all log messages
		 1
		 2
		 3		all directx messages
		 4		major directx messages

		 5		master/slave communications
		 6
		 7
		 8		major state changes
		 9
		10		major initialization/shutdown messages and errors

	***********************************************************************/

	void Logger::write(int level, int printdepth, int reset, const char *format, ...)  {	// Other arguments as necessary
		char savedir[_MAX_PATH];
		BOOL wrongdir = FALSE;
		#ifdef _DEBUG
			int bp;
		#endif

		if (!stream)  {
	#ifdef _DEBUG
			bp = 1;
	#endif
			return;
		}

#ifdef WIN32
		GetCurrentDirectory(_MAX_PATH-1, savedir);
#else
		getcwd( savedir, _MAX_PATH );
#endif

		/*
		#ifdef WIN32
		GetCurrentDirectory(_MAX_PATH-1,currentDirectory);
		#else
		_getcwd( currentDirectory, _MAX_PATH );
		#endif
		*/


		if (strcmp(savedir, currentDirectory) != 0)  {
			//fprintf(stream,"error: directory = %s\n", str);
			//fprintf(stream,"current directory = %s\n", currentDirectory);
			//fclose(stream);

			#ifdef _DEBUG
			//CopyFile(file, stub, FALSE);	// make a backup copy for safe keeping
			#endif
 			//stream = fopen(stub, "a+t");
			SetCurrentDirectory(currentDirectory);		// go back to correct directory
			wrongdir = TRUE;
		}	


		if (closed)  {
			if (wrongdir)  {
				SetCurrentDirectory(savedir);
			}
			return;
		}


	//	BOOL reclose=FALSE;

		if (level<loglevel)  {
			if (wrongdir)  {
				SetCurrentDirectory(savedir);
			}
			return;
		}


		if (stream==NULL)  {
			//reclose = TRUE;
			stream = fopen(stub, "a+t");
		}


		va_list	ap;													// Argument pointer
		char		s[2048];												// Output string
		int i;


		if (format == NULL)  {
			fprintf(stream,"Error: format = NULL!!!!\n");
			fclose(stream);
			stream = fopen(stub, "a+t");
			//if (reclose)  {
      		fclose(stream);
				stream = NULL;
			//}
			if (wrongdir) SetCurrentDirectory(savedir);
			return;
		}

		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);

		if (printdepth<10)  {
			for(i=0;i<printdepth;i++)  {
				fprintf(stream,"   ");
			}
		}
		else  {
			fprintf(stream,"log: printdepth = %d\n",printdepth);
		}

	//	strcat(s,"\n");
		fprintf(stream, "%s", s);		//cgerr, boundschecker error

		if (reset)  {
	#ifdef WIN32
			int status = fclose(stream);
			assert(status==0);
        #else
		fclose(stream);

	#endif

			#ifdef _DEBUG
			//CopyFile(file, "x.log", FALSE);	// make a backup copy for safe keeping
			#endif

			stream = fopen(stub, "a+t");
		}

	//   if (reclose)  {
	//	  	fclose(stream);
	//      stream = NULL;
	//   }

		if (wrongdir) SetCurrentDirectory(savedir);
		return;

	}

	/**********************************************************************

	***********************************************************************/

	void Logger::write(const char *format, ...)  {
		va_list ap;													// Argument pointer
		char s[1024];												// Output string
		int len;
		
		len = (int)strlen(format);
		if (len>1023)  {
			fprintf(stream, "\n(string to long in Logger::write())\n");
			return;
		}

		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);

		fprintf(stream, "%s", s);

		return;
	}


	/**********************************************************************

	**********************************************************************/

	void Logger::flush(void)  {
		fclose(stream);
		stream = fopen(stub, "a+t");
		return;
	}

	/**********************************************************************

	**********************************************************************/

	int Logger::import_file(const char *_fname)  {
		FILE *stream;
		char *cptr;

		stream = fopen(_fname, "rt");
		if (stream==NULL)  {
			return 1;
		}

		while(1)  {
			cptr = fgets(gstring, 256, stream);
			if (cptr==NULL)  {
				break;
			}
			trimcrlf(gstring);
			write("%s\n", gstring);
		}
		FCLOSE(stream);

		return 0;
	}

//}						// extern "C" {


