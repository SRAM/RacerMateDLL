
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#pragma warning(disable:4786)
	#include <windows.h>
	#include <mmsystem.h>
#endif


#include <assert.h>

#include <err.h>
#include <types.h>
#include <config.h>
#include <vdefines.h>
//#include <regularcourse.h>
//#include <ergocourse.h>

#include <mfd.h>
#include <string.h>
#include <utils.h>
#include <fatalerror.h>
#include <progressbar.h>
//#include <odbc.h>

//#define DO_DATABASE
//extern EXPORT_TYPE xpt[MAX_EXPORT_ITEMS];


#ifdef _DEBUG
	//extern bool g_oktobreak;			//temporary!!!
#endif

//extern bool gMetric;


/**********************************************************************
	constructor
**********************************************************************/

#ifdef WIN32
MFD::MFD(char *_fname, HWND _phwnd, bool _enc) : dataSource(_phwnd)  {
#else
MFD::MFD(char *_fname, bool _enc) : dataSource()  {
#endif

	strcpy(fname, _fname);
	enc = _enc;
	array = NULL;


#ifdef _DEBUG
	log->write("MFD: _DEBUG IS TRUE\n\n");
#else
	log->write("MFD: _DEBUG IS FALSE\n\n");
#endif

	stream = NULL;

	// don't open in parent class, since 3d encryption needs to open & close it

	file_size = filesize_from_name(fname);

	memset(&udata, 0, sizeof(USER_DATA));
#ifdef WIN32
	memset(&cheader, 0, sizeof(Course::OLD_COURSE_HEADER));
#endif
	
	nextReadTime = 0;

	readRate = 1;
	lastdir = -1;

	dir = -1;
	course = NULL;

	firstRecordOffset = 0L;
	connected = TRUE;
	dsmetric = false;					// a record of whether 'metric' was checked on the menu

	lastnow = 0;

	nextrec = 0;
	fileStartTime = 0;
	fileEndTime = 0;

	lastReadTime = 0;

	//t1 = new fastTimer("t1");
	now1 = 0;

#ifdef WIN32
	freq1 = t1->getFreq();
#else
	freq1 = 0;
#endif
	/*
	if (course)  {								// manual charts has no course!!!!
		switch(course->type)  {
			case Course::ERGO:
				totalMS = 0L;								// in ms
				break;
			case Course::THREED:
			case Course::NONERGO:
				totalLen = 0.0f;							// in miles
				break;
			default:
				break;
		}
	}
	*/

	return;
}


/**********************************************************************
	destructor
**********************************************************************/

MFD::~MFD()  {
//lint -save -e1740
	destruct();
//lint -restore
}

/**********************************************************************

**********************************************************************/

int MFD::get_nRecords(void)  {
	return records;
}

/**********************************************************************
	exports the data to a file "_fname"
	stream is open and points to ".tmp" (the meta file)

  gets short_format and comma_delimited from 'export.ini'
*******************************************************************/

int MFD::myexport(char *_fname)  {								// tlm20060126
	int i;
	int status;
	METADATA lmeta;
	Ini *ini;
	bool comma_delimited;
	bool short_format;
	//bool export_metric;
	int toss = 0;

#ifdef _DEBUG
//	int sz = MAX_EXPORT_ITEMS;						// 22
#endif


const char *shortFormats[MAX_EXPORT_ITEMS]  =  {
	"%ld",			// time
	"%.2f",     	// speed
	"%.0f",    		// power
	"%.0f",     	// rpm
	"%.0f",     	// hr
	"%.0f",     	// ss
	"%.0f",     	// lss
	"%.0f",     	// rss
	"%.0f",     	// lpower
	"%.0f",     	// rpower
	"%.4f",     	// miles
	"%.0f",     	// wind
	"%.2f",     	// grade
	"%.0f",      	// load
	"%d",				// lata
	"%d",				// rata
#ifdef VELOTRON
	"%d",				// gear
	"%d",				// front teeth
	"%d",				// rear teeth
#endif
	"%.0f",			// pulse power
	"%d",				// cadence
	"%.5f"			// ssraw
};

const char *longFormats[MAX_EXPORT_ITEMS]  =  {
	"%10ld",			// time
	"%6.2f",       // speed
	"%5.0f",       // power
	"%3.0f",       // rpm
	"%3.0f",       // hr
	"%3.0f",       // ss
	"%3.0f",       // lss
	"%3.0f",       // rss
	"%4.0f",       // lpower
	"%4.0f",       // rpower
#ifdef _DEBUG
	"%10.6f",       // miles
#else
	"%8.4f",       // miles
#endif

	"%4.0f",       // wind
	"%6.2f",       // grade
	"%4.0f",       // load
	"%5d",			// lata
	"%5d",			// rata
#ifdef VELOTRON
	"%4d",			// gear
	"%4d",			// front teeth
	"%4d",			// rear teeth

#endif
	"%6.0f",			// pulse power
	"%2d",			// cadence
	"%8.5f"			// ssraw
};


int j;
char formats[MAX_EXPORT_ITEMS][16];


	ini = new Ini("export.ini");
	comma_delimited = ini->getBool("options", "comma delimited", "false");
	short_format = ini->getBool("options", "short format", "false");
	//export_metric = ini->getBool("options", "export metric", "false");
	toss = ini->getInt("options", "toss", "0", true);

	for(i=0;i<MAX_EXPORT_ITEMS;i++)  {
		//xpt[i].flag = ini->getBool("options", xpt[i].label, "true");
	}

	DEL(ini);


	FILE *outstream = fopen(_fname, "wt");
	if (outstream==NULL)  {
		return -2;
	}


	if (short_format)  {
		for(i=0;i<MAX_EXPORT_ITEMS;i++)  {
			if (comma_delimited)  {
				sprintf(formats[i], "\"%s\",", shortFormats[i]);
			}
			else  {
				sprintf(formats[i], "%s ", shortFormats[i]);
			}
		}
	}
	else   {
		for(i=0;i<MAX_EXPORT_ITEMS;i++)  {
			if (comma_delimited)  {
				sprintf(formats[i], "\"%s\",", longFormats[i]);
			}
			else  {
				sprintf(formats[i], "%s ", longFormats[i]);
			}
		}
	}

	//----------------------------------------------------------
	// here, we need to write the user and course information:
	//----------------------------------------------------------

	fprintf(outstream, "[USER DATA]\n");
	fprintf(outstream, "%s\n", udata.name);
	fprintf(outstream, "AGE=%d\n", udata.age);

	float f;

	/*
	if (gMetric)  {
		if (udata.metric)  {
			f = udata.kgs;
		}
		else  {
			f = (float) (TOKGS * udata.lbs);
		}
		fprintf(outstream, "WEIGHT=%.1f kg\n", f);
	}
	else  {
		if (udata.metric)  {
			f = (float) (TOPOUNDS * udata.kgs);
		}
		else  {
			f = udata.lbs;
		}
		fprintf(outstream, "WEIGHT=%.1f pounds\n", f);
	}
	*/
	

	fprintf(outstream, "LOWER HR=%.0f\n", udata.lower_hr);
	fprintf(outstream, "UPPER HR=%.0f\n", udata.upper_hr);

	float watts_factor;
	//tlm20061117+++
	if (udata.drag_aerodynamic==0.0f)  {
		watts_factor = 1.0f;
		udata.drag_tire = .006f;
		udata.drag_aerodynamic = 8.0f;
	}
	else  {
		udata.drag_tire = .006f;
		watts_factor = udata.drag_aerodynamic / 8.0f;;
	}
	//tlm20061117---

	fprintf(outstream, "drag factor=%.0f\n", 100.0f*watts_factor);

	fprintf(outstream, "[END USER DATA]\n");

	if (course)  {				// .ss files don't have a course!
		fprintf(outstream, "\n[COURSE HEADER]\n");
		fprintf(outstream, "VERSION = 2\n");
		/*
		if (gMetric)  {
			fprintf(outstream, "UNITS = METRIC\n");
		}
		else  {
			fprintf(outstream, "UNITS = ENGLISH\n");
		}
		*/

#ifdef WIN32
		fprintf(outstream, "DESCRIPTION = %s\n", cheader.desc);

		if (course->type ==Course::NONERGO)  {
			fprintf(outstream, "FILE NAME = %s\n", cheader.fname);
			fprintf(outstream, "MILES  GRADE  WIND\n");
		}
		else if (course->type==Course::ERGO)  {
			fprintf(outstream, "FILE NAME = %s\n", cheader.fname);
			fprintf(outstream, "MINUTES  WATTS\n");
		}

		fprintf(outstream, "[END COURSE HEADER]\n");

		fprintf(outstream, "\n[COURSE DATA]\n");
		course->exportLegs(outstream);
		fprintf(outstream, "[END COURSE DATA]\n\n");
#endif
	}
	else  {
		fprintf(outstream, "\n");
	}


#ifdef _DEBUG
	/*
	bp = 1;
	fprintf(outstream, "decoder->user->watts_factor = %.2f\n", decoder->user->get_watts_factor());
	fprintf(outstream, "decoder->user->data.drag_aerodynamic = %.2f\n", decoder->user->data.drag_aerodynamic);
	fprintf(outstream, "\n");
	*/
#endif


	//----------------------------------------------------------
	// now write the data:
	//----------------------------------------------------------

#ifdef _DEBUG
	log->write(10,0,0,"\n");
	for(i=0;i<MAX_EXPORT_ITEMS;i++)  {
		log->write(10,0,0,"formats[%2d] = %s\n", i, &formats[i][0]);
	}
	log->write(10,0,0,"\n");
#endif


	fprintf(outstream, "number of records = %ld\n\n", records);

#if 0
	for(i=0;i<MAX_EXPORT_ITEMS;i++)  {
		if (xpt[i].flag)  {
			char fmt[32];
			if (indexIgnoreCase(xpt[i].label, "miles") != -1)  {
				//if (export_metric)  {
				/*
				if (gMetric)  {
					fprintf(outstream, "KM ");
				}
				else  {
					sprintf(fmt, "%%%ds ", strlen(xpt[i].label));
					fprintf(outstream, fmt, xpt[i].label);
				}
				*/
			}
			else  {
				sprintf(fmt, "%%%ds ", strlen(xpt[i].label));
				fprintf(outstream, fmt, xpt[i].label);
			}
		}
	}
#endif
	
	fprintf(outstream, "\n\n");


	char str[256];
	char s[2048];


	fseek(stream, 0L, SEEK_SET);

	// need to show the parent window, so that the progress bar will appear?


//lint -save -e505
	//progressBar *pb = new progressBar(CLASSNAME, WINDOWNAME, RGB(70, 70, 70), RGB(100, 150, 200));
/*
class progressBar(char *_classname, char *_windowname, COLORREF bg, COLORREF fg);
class progressBar(HWND _phwnd, COLORREF bg, COLORREF fg, int _min=0, int _max=100);
*/
#ifdef WIN32
	progressBar *pb = new progressBar(
				phwnd, 
				RGB(70, 70, 70), 
				RGB(100, 150, 200)
			);
#else
	progressBar *pb = new progressBar("xyzzy", RGB(70,70,70), RGB(100, 150, 200) );
#endif

//lint -restore
	float m, b;
	map(0.0f, (float) (records -1), 0.0f, 100.0f, &m, &b);
	int percent;
	int lastpercent = -1;


	long dataStart = ftell(stream);

#ifdef _DEBUG
//	int count = 0;
#endif

//int msz = sizeof(METADATA);

	for(i=0; i<(int)records; i+=(toss+1))  {
		percent = (int) (.5 + m*i + b);
		if (percent != lastpercent)  {
			lastpercent = percent;
			pb->draw(percent);
		}

		long offset = dataStart + i*(long)sizeof(METADATA);
		fseek(stream, offset, SEEK_SET);

		status = fread(&lmeta, sizeof(METADATA), 1, stream);

#ifdef _DEBUG
		if (i==0)  {
			bp = 9;
		}
#endif

		if (status != 1)  {
			bp = 1;
			fclose(outstream);
			DEL(pb);
			throw(fatalError(__FILE__, __LINE__));
		}

		s[0] = NULL;

#if 0

		for(j=0;j<MAX_EXPORT_ITEMS;j++)  {

			switch(j)  {
				case 0:										// time
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.time);
						strcat(s, str);
					}
					break;

				case 1:										// speed
					if (xpt[j].flag)  {
						//if (export_metric)  {
						/*
						if (gMetric)  {
							sprintf(str, formats[j], MPHTOKPH*lmeta.mph);
						}
						else  {
							sprintf(str, formats[j], lmeta.mph);
						}
						strcat(s, str);
						*/
					}
					break;

				case 2:										// power
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.watts);
						strcat(s, str);
					}
					break;

				case 3:										// rpm
					if (xpt[j].flag)  {
						if (lmeta.pedalrpm > 0)  {
							sprintf(str, formats[j], lmeta.rpm);
						}
						else  {
							sprintf(str, formats[j], lmeta.pedalrpm);
						}
						strcat(s, str);
					}
					break;

				case 4:										// hr
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.hr);
						strcat(s, str);
					}
					break;

				case 5:										// ss
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.ss);
						strcat(s, str);
					}
					break;

				case 6:										// lss
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.lss);
						strcat(s, str);
					}
					break;

				case 7:										// rss
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.rss);
						strcat(s, str);
					}
					break;

				case 8:										// lpower
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.lwatts);
						strcat(s, str);
					}
					break;

				case 9:										// rpower
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.rwatts);
						strcat(s, str);
					}
					break;

				case 10:										// tmiles
					if (xpt[j].flag)  {
						//if (export_metric)  {
						/*
						if (gMetric)  {
							sprintf(str, formats[j], MILESTOKM*lmeta.miles);
						}
						else  {
							sprintf(str, formats[j], lmeta.miles);
						}
						strcat(s, str);
						*/
					}
					break;

				case 11:										// twind
					//if (xpt[j-2].flag)  {
					if (xpt[j].flag)  {
						//if (export_metric)  {
						/*
						if (gMetric)  {
							sprintf(str, formats[j], MPHTOKPH*lmeta.wind);
						}
						else  {
							sprintf(str, formats[j], lmeta.wind);
						}
						strcat(s, str);
						*/
					}
					break;

				case 12:										// tgrade
					//if (xpt[j-2].flag)  {
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.grade);
						strcat(s, str);
					}
					break;

				case 13:										// load
					//if (xpt[j-2].flag)  {
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.load);
						strcat(s, str);
					}
					break;

				case 14:										// lata
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.lata);
						strcat(s, str);
					}
					break;

				case 15:										// rata
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.rata);
						strcat(s, str);
					}
					break;

#ifdef VELOTRON
				case 16:										// gear
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.gear);
						strcat(s, str);
					}
					break;

				case 17:										// front teeth
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.virtualFrontGear);
						strcat(s, str);
					}
					break;

				case 18:										// rear teeth
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.virtualRearGear);
						strcat(s, str);
					}
					break;
#endif

#ifdef VELOTRON
				case 19:										// pulse power
#else
				case 16:
#endif
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.pp);
						strcat(s, str);
					}
					break;

#ifdef VELOTRON
				case 20:										// cadence
#else
				case 17:
#endif
					if (xpt[j].flag)  {
						sprintf(str, formats[j], lmeta.tdc);
						strcat(s, str);
					}
					break;

#ifdef VELOTRON
				case 21:										// ssraw
#else
				case 18:
#endif
					if (xpt[j].flag)  {

#ifdef VELOTRON
						for(int kk=0; kk<72; kk++)  {
#else
						for(int kk=0; kk<24; kk++)  {
#endif
							sprintf(str, formats[j], (float) (lmeta.bars[kk]/10.0f) );
							strcat(s, str);
						}
					}
					break;

				default:
					throw(fatalError(__FILE__, __LINE__));
			}
		}					// for (j...
#endif			// #if 0

		// remove trailing ','
		int len = strlen(s);
		if (s[len-1]==',')  {
			s[len-1] = 0;
		}
		fprintf(outstream, "%s\n", s);
	}

		
	fprintf(outstream, "\n");
	fclose(outstream);

#ifdef DO_DATABASE

	{

	//---------------------------------------------
	// now write a summary to computrainer.gdb
	//---------------------------------------------

	Odbc *conn = NULL;
	SQLRETURN ret;
	char command[1024];
	SQLHANDLE hstmt = INVALID_HANDLE_VALUE;
	char edate[16];
	SQLINTEGER nep;
	SQLSMALLINT si;
	char szState[6];
	char buffer[256];

	conn = new Odbc();
	ret = conn->connect( Odbc::INTERBASE, "localhost:c:\\", "computrainer.gdb", "SYSDBA", "masterkey");
	if (ret)  {
		DEL(conn);
	}

	bool create_user_table = false;

	if (create_user_table)  {
		strcpy(command, "DROP TABLE USERS");
		ret = conn->execute(command, false);
		if (SQLFailed(ret))  {
			bp = 1;
		}

		strcpy(command, "\
CREATE TABLE USERS(\
NAME CHAR(40),\
EDATE DATE,\
GENDER CHAR(1),\
WEIGHT NUMERIC(8,2),\
AGE NUMERIC(8,2),\
LOWERHR INTEGER,\
UPPERHR INTEGER,\
RFDRAG NUMERIC(8,2)\
);");

		ret = conn->execute(command);
		if (SQLFailed(ret))  {
			DEL(conn);
			return;
		}

		ret = conn->execute("COMMIT;");
		if (SQLFailed(ret))  {
			throw (fatalError(__FILE__, __LINE__));
		}
	}						// if (create_user_table)





	SYSTEMTIME x;
	GetLocalTime(&x);
	sprintf(edate,"%d-%02d-%02d",x.wYear, x.wMonth, x.wDay);

	sprintf(command, "INSERT INTO USERS VALUES ('%s', '%s', '%s', %f, %d, %f, %f, %f);",
udata.name,
edate,
udata.sex,
udata.lbs,
udata.age,
udata.lower_hr,
udata.upper_hr,
udata.rfdrag
);

	ret = SQLAllocStmt(conn->getHdbc(), &hstmt);
	if (SQLFailed(ret))  {
		throw (fatalError(__FILE__, __LINE__));
	}

	ret = SQLPrepare(hstmt, (unsigned char*) command, SQL_NTS);
	if (SQLFailed(ret))  {
		SQLGetDiagRec(   
					SQL_HANDLE_STMT,				// SQLSMALLINT HandleType, 
					hstmt,							// SQLHANDLE Handle, 
					1,									// SQLSMALLINT RecNumber, 
					(unsigned char *)szState,	// SQLCHAR * Sqlstate, 
					&nep,								// SQLINTEGER * NativeErrorPtr, 
					(unsigned char *)buffer,							// SQLCHAR * MessageText,
					sizeof(buffer),				// SQLSMALLINT BufferLength, 
					&si								// SQLSMALLINT * TextLengthPtr
			);

		MessageBox(0, (const char *)buffer, "Error", MB_OK | MB_ICONEXCLAMATION);
		goto finis;
	}

	ret = SQLExecute(hstmt);
	if (SQLFailed(ret))  {
		SQLGetDiagRec(   
			SQL_HANDLE_STMT,				// SQLSMALLINT HandleType, 
			hstmt,							// SQLHANDLE Handle, 
			1,									// SQLSMALLINT RecNumber, 
			(unsigned char *)szState,	// SQLCHAR * Sqlstate, 
			&nep,								// SQLINTEGER * NativeErrorPtr, 
			(unsigned char *)buffer,							// SQLCHAR * MessageText,
			sizeof(buffer),				// SQLSMALLINT BufferLength, 
			&si								// SQLSMALLINT * TextLengthPtr
		); 

		MessageBox(0, (const char *)buffer, "Error", MB_OK | MB_ICONEXCLAMATION);
		goto finis;
	}

finis:
	SQLFreeStmt(hstmt, SQL_DROP);
	hstmt = INVALID_HANDLE_VALUE;
	conn->disconnect();							// with no parameter implies rollback
	DEL(conn);

	}

#endif							// #ifdef DO_DATABASE

	DEL(pb);

	return 0;
}


/***********************************************************************************
	// note: we're reading out of ".tmp" so, firstRecordOffset is 0!
	// last record is 'records-1'
***********************************************************************************/

int MFD::seek(long rec)  {
	long offset;
	int status;

	offset = firstRecordOffset + rec*(long)sizeof(METADATA);
	status = fseek(stream, offset, SEEK_SET);

	return status;
}


/**********************************************************************

	returns -1 if bof, 0 otherwise

	assumes sequential access, not random access!!!
	For random access, use read(int k)!!!

**********************************************************************/

int MFD::getPreviousRecord(DWORD _delay)  {		// dataSource pure virtual
	int status;

	assert(_delay!=1);

	switch(dir)  {
		case 1:				// already going in reverse
			break;
		case -1:
			dir = 1;
			bof = false;
			bofedge = false;
			lastbof = false;
			eof = false;
			eofedge = false;
			lasteof = false;
			break;
		case 0:								// been going forward?
			dir = 1;
			// we've autodecremented, so we need to undo that and increment one more
			left -= 2;
			right -=2;
			bof = false;
			bofedge = false;
			lastbof = false;
			eof = false;
			eofedge = false;
			lasteof = false;

			break;
	}

	if (left < 0)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	status = seek(left);
	if (status)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	status = fread(&decoder->meta, sizeof(METADATA), 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	// autodecrement, although caller may override:

	if (left>0)  {
		left--;
		right--;
	}
	else  {
		bof = true;
		bofedge = false;
		if (!lastbof)  {
			lastbof = true;
			bofedge = true;
		}
		return -1;
	}

	return 0;
}


/**********************************************************************

	returns 
		-1 if last record
		1 if it is not time for data
		0 if there is new data at this time

	assumes sequential access, not random access!!!
	For random access, use read(int k)!!!


**********************************************************************/

int MFD::getNextRecord(DWORD _dly)  {
	int status;
	DWORD now;

	if (decoder->get_paused())  {
		return 1;
	}

	switch(_dly)  {

		case 0:
			break;

		case 0xffffffff:  {				// -1
			//now = t1->getTime();
			now = timeGetTime();

			if (now < nextReadTime)  {
				return 1;
			}
			#ifdef _DEBUG
				//t1->average();							// 28.9214 ms
			#endif
			break;
		}

		default:  {
			//timeBeginPeriod(1);
			now = timeGetTime();;
			//timeEndPeriod(1);
			//now = t1->getTime();
			//now = t1->getms();

			if ((now - lastReadTime) < _dly)  {			// tlm20050307 was 30
				bp = 1;
				return 1;
			}
			lastReadTime = now;
			break;
		}
	}

	#ifdef _DEBUG
	//avgTimer->update();
	#endif

	switch(dir)  {
		case 0:				// already going forward
			break;
		case -1:
			dir = 0;
			bof = false;
			bofedge = false;
			lastbof = false;
			eof = false;
			eofedge = false;
			lasteof = false;
			break;
		case 1:								// been going in reverse?
			dir = 0;
			// we've autodecremented, so we need to undo that and increment one more
			left += 2;
			right +=2;
			bof = false;
			bofedge = false;
			lastbof = false;
			eof = false;
			eofedge = false;
			lasteof = false;

			break;
	}

	if (right >= (int)records)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	status = seek(right);
	if (status)  {
		throw(fatalError(__FILE__, __LINE__));
	}


	status = fread(&decoder->meta, sizeof(METADATA), 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__));
	}

#ifdef _DEBUG
//	if (decoder->meta.ss > 0.0f)  {
//		bp = 1;
//	}
//	long offs = ftell(stream);
//	bp = 2;
//	// right, records
//	if (decoder->meta.miles >= .02f)  {
//		bp = 1;
//	}
#endif

#ifdef VELOTRON
	decoder->ss->leftwatts = decoder->meta.lwatts;
	decoder->ss->rightwatts = decoder->meta.rwatts;
	decoder->ss->lata = (float)decoder->meta.lata;
	decoder->ss->rata = (float)decoder->meta.rata;
	decoder->ss->leftss = decoder->meta.lss;
	decoder->ss->rightss = decoder->meta.rss;
	decoder->ss->totalss = decoder->meta.ss;
	decoder->ss->average_lss = decoder->meta.average_lss;
	decoder->ss->average_rss = decoder->meta.average_rss;
	decoder->ss->average_ss = decoder->meta.average_ss;
#endif

	int i;

#ifdef VELOTRON
	for(i=0;i<72;i++)  {
		decoder->ss->filteredTorques[i] = decoder->meta.bars[i];
	}
#else

	// here we're simulating realtime
	for(i=0;i<24;i++)  {
		int k = 2*i + 1;
		decoder->ss->thrust[k].fval = decoder->meta.bars[i];
		// the even fvals will be interpolated later
	}
	// decoder->ss* is what is displayed in bars::, eg, so fill these in
	decoder->ss->leftss = decoder->meta.lss;
	decoder->ss->rightss = decoder->meta.rss;
	decoder->ss->totalss = decoder->meta.ss;
	decoder->ss->ilata = decoder->meta.lata;
	decoder->ss->lata = (float)decoder->meta.lata;
	decoder->ss->irata = decoder->meta.rata;
	decoder->ss->rata = (float)decoder->meta.rata;
	decoder->ss->leftwatts = decoder->meta.lwatts;
	decoder->ss->rightwatts = decoder->meta.rwatts;
	decoder->ss->average_lss = decoder->meta.lss;
	decoder->ss->average_rss = decoder->meta.rss;
	decoder->ss->average_ss = decoder->meta.ss;

	bp = 1;
#endif


	// autoincrement (this is the SEQUENTIAL access method)!!
	if (right < (int)records-1)  {
		right++;
		left++;
		if (_dly==0xffffffff)  {
			long offs = ftell(stream);
			METADATA tmeta;
			status = fread(&tmeta, sizeof(METADATA), 1, stream);
			if (status != 1)  {
				throw(fatalError(__FILE__, __LINE__));
			}
			fseek(stream, offs, SEEK_SET);
			nextReadTime = tmeta.time;
		}
	}
	else  {
		eof = true;
		eofedge = false;
		if (!lasteof)  {
			lasteof = true;
			eofedge = true;
		}
		if (_dly==0xffffffff)  {
			bp = 17;
		}
		if (course)  {								// manual charts has no course!!!!
			switch(course->type)  {
				case Course::ERGO:
					if (decoder->meta.time >= decoder->get_totalMS())  {
						if (!decoder->get_finished())  {
							decoder->set_finished(true);
							decoder->set_finishEdge(true);
						}
						else  {
							// let the app clear the edge flag, since this class could clear it before the app sees it!!!
						}
					}
					break;
				case Course::THREED:
				case Course::NONERGO:
					//if (decoder->meta.miles >= decoder->totalLen)  {
						decoder->meta.miles = decoder->get_totalLen();
						if (!decoder->get_finished())  {
							decoder->set_finished(true);
							decoder->set_finishEdge(true);
						}
					//}
					break;
				default:
					break;
			}
		}
		return -1;								// returns -1 on last record!!! not really eof
	}

	/**************
	// check for finish here!!!!!!!!

	if (course)  {								// manual charts has no course!!!!
		switch(course->type)  {
			case Course::ERGO:
				if (decoder->meta.time >= decoder->totalMS)  {
					if (!decoder->finished)  {
						decoder->finished = true;
						decoder->finishEdge = true;
					}
					else  {
						// let the app clear the edge flag, since this class could clear it before the app sees it!!!
					}
				}
				break;
			case Course::THREED:
			case Course::NONERGO:
				if (decoder->meta.miles >= decoder->totalLen)  {
					decoder->meta.miles = decoder->totalLen;	// tlm20030812, last logging is EXACTLY totalLen
					if (!decoder->finished)  {
						decoder->finished = true;
						decoder->finishEdge = true;
					}
				}
				break;
			default:
				break;
		}
	}
	********************/

	return 0;
}


/**********************************************************************
	reads immediately with no delay
**********************************************************************/

int MFD::read(int k)  {
	int status;
	status = seek(k);

	if (status)  {
		return status;
	}


	status = fread(&decoder->meta, sizeof(METADATA), 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	return 0;
}

/***********************************************************************************

	returns 0 if ok
	returns -1 if eof
	otherwise returns an error int

***********************************************************************************/

int MFD::getRecord(int k)  {
	long offset;
	int status;

	offset = (long) (firstRecordOffset + (int)sizeof(METADATA)*k);
	status = fseek(stream, offset, SEEK_SET);
	if (status==-1)  {
		return -1;							// out of data
	}


	status = fread(&decoder->meta, sizeof(METADATA), 1, stream);
	if (status != 1)  {
		return -1;
	}

	return 0;
}

/***********************************************************************************
	returns the current RECORD number that would be read
***********************************************************************************/

long MFD::getOffset(void)  {
	long offset = ftell(stream);
//lint -save -e573
	long record = (offset - firstRecordOffset) / (long)sizeof(METADATA);
//lint -restore
	return record;
}


/**********************************************************************

**********************************************************************/

void MFD::destruct(void)  {

	DELARR(array);
	FCLOSE(stream);
	DEL(course);		// owned by parent, created by child!!

	rm(tmpfilename);				// tlm20040817 (moved from preveldata)
	//DEL(t1);

	return;
}

/***********************************************************************************

***********************************************************************************/

void MFD::reset(void)  {

	nextReadTime = 0;
//	t1->reset();

	fseek(stream, firstRecordOffset, SEEK_SET);
	decoder->reset();								// sets decoder->paused to false

	timeBeginPeriod(1);
		resetTime = timeGetTime();
	timeEndPeriod(1);

	right = 0;

	memset(&decoder->meta, 0, sizeof(decoder->meta));
	decoder->meta.runSwitch = 1;

#ifdef VELOTRON
	for(int i=0;i<72;i++)  {
		decoder->ss->filteredTorques[i] = 0;
	}
#endif

	decoder->ss->leftwatts = 0;
	decoder->ss->rightwatts = 0;
	decoder->ss->lata = 0;
	decoder->ss->rata = 0;
	decoder->ss->leftss = 0;
	decoder->ss->rightss = 0;
	decoder->ss->totalss = 0;
	decoder->ss->average_lss = 0;
	decoder->ss->average_rss = 0;
	decoder->ss->average_ss = 0;

	return;
}
