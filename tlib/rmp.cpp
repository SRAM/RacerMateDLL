
#include <memory.h>
#include <assert.h>
#include <float.h>

#ifndef WIN32
	#include <minmax.h>
#endif

#include <globals.h>
#include <utils.h>
#include <fatalerror.h>
#include <md5.h>
#include <earth.h>

#include <rmp.h>
#include <pcre2.h>

const char *RMP::COPYRIGHT = "(c) 2012, RacerMate, Inc.";

#ifdef _DEBUG
long RMP::data_start_finalize = 0L;
long RMP::data_start_load = 0L;
long RMP::data_end_finalize = 0L;
long RMP::data_end_load = 0L;
#endif

unsigned char RMP::hash[48] = {0};			// binary
char RMP::asciihexhash[48] = {0};			// ascii
char RMP::vidname_from_file[64] = {0};
char RMP::gstr[256] = {0};

/*******************************************************************************************************************
  constructor
 *******************************************************************************************************************/

#ifdef _DEBUG
RMP::RMP(const char *_logfilename)  {
#else
RMP::RMP(void)  {
#endif

	pd = new PerfData();

	memset(&lastdata, 0, sizeof(lastdata));
	memset(outfilename, 0, sizeof(outfilename));

#ifdef _DEBUG
	logg = NULL;
	if (_logfilename)  {
		logg = new Logger(_logfilename);
	}
	writecount = 0;
#endif

	basename = NULL;
	course_section_count = 0;
	course_header_start = 0L;
	course_data_start = 0L;
	//drag_factor = 100;
	first = true;
	instream = NULL;
	outstream = NULL;
	data_start = 0L;
	data_end = 0L;
	fname = NULL;
	perfcount = 0;
	bp = 0;
	version = 0.0f;
	start_time = 0L;						// time that data collection starts
	load_start_time = 0L;
	data_start_time = 0L;
	next_ms = 0xffffffff;
	records_in = 0L;
	records_out = 0L;
	tag = 0;
	hdrVersion = 0.0f;
	XUnits = 0;
	YUnits = 0;
	load_dur = 0L;
	perf_dur = 0L;
	memset(&td, 0, sizeof(td));
	avg_kph = 0.0f;
	max_kph = -FLT_MAX;
	total_kph = 0.0f;
	avg_rpm = 0.0f;
	max_rpm = 0;
	total_rpm = 0.0f;
	group2_count = 0L;

	// group5:

	avg_hr = 0.0f;
	max_hr = 0;
	total_hr = 0.0f;
	group5_count = 0L;


	// group1:

	avg_watts = 0.0f;
	max_watts = 0;
	total_watts = 0.0f;
	group1_count = 0L;

#ifdef _DEBUG
	oktobreak = false;
	resets = 0;
#endif

	memset(&exe, 0, sizeof(exe));
	memset(&m_OriginalHash, 0, sizeof(m_OriginalHash));
	memset(&m_CourseHash, 0, sizeof(m_CourseHash));
	memset(&m_HeaderHash, 0, sizeof(m_HeaderHash));
	memset(&errstr, 0, sizeof(errstr));
	memset(&rmheader, 0, sizeof(rmheader));
	memset(&info, 0, sizeof(info));
	memset(&infoext, 0, sizeof(infoext));
	memset(&ch, 0, sizeof(ch));
	memset(&data, 0, sizeof(data));
	//memset(&hdr, 0, sizeof(hdr));
	memset(&group0, 0, sizeof(group0));           	// these always change
	memset(&group1, 0, sizeof(group1));             // group 1 changes often
	memset(&group2, 0, sizeof(group2));
	memset(&group3, 0, sizeof(group3));
	memset(&group4, 0, sizeof(group4));
	memset(&group5, 0, sizeof(group5));
	memset(&group6, 0, sizeof(group6));
	memset(&group7, 0, sizeof(group7));
	memset(&group7x, 0, sizeof(group7x));
	memset(&group8, 0, sizeof(group8));
	memset(&group9, 0, sizeof(group9));
	memset(&group10, 0, sizeof(group10));
	memset(&group11, 0, sizeof(group11));
	memset(&group12, 0, sizeof(group12));

	memset(&result, 0, sizeof(group12));

	now = 0L;
	offs = 0L;
	changedflags = 0;
	GroupFlags = 0;

	return;
}

/*******************************************************************************************************************
	destructor
*******************************************************************************************************************/

    RMP::~RMP()  {
	bp = 0;
	FCLOSE(instream);
	FCLOSE(outstream);
	DEL(pd);
	bp = 1;
	nodes.clear();

#ifdef _DEBUG
	DEL(logg);
#endif

	return;
    }


	/*************************************************************************************************

	 *************************************************************************************************/

	INT32 RMP::get_next_tag(FILE *instream)  {
		int n;
		INT32 tag;

		n = (int)fread(&tag, sizeof(INT32), 1, instream);
		if (n != 1)  {
			return 0xffffffff;
		}
		fseek(instream, -(int)sizeof(INT32), SEEK_CUR);

#ifdef _DEBUG
		//long offs;
		char str[32];
		//offs = ftell(instream);
		//xx = mmioStringToFOURCC("RM1X", MMIO_TOUPPER);
		sprintf(str, "%c%c%c%c", (char)(tag >> 24), (char)(tag >>16), (char)(tag >> 8), (char)(tag));
		//const FOURCC RM1X = mmioStringToFOURCC("RM1X", MMIO_TOUPPER);
		//#define RM1X mmioStringToFOURCC("RM1X", MMIO_TOUPPER)
		//const char *cptr = fourcc(tag);
#endif

		return tag;
	}



/*************************************************************************************************

*************************************************************************************************/

void RMP::compute_date(char date[24])  {

    memset(date, 0, 24);
    
    #ifdef WIN32
	SYSTEMTIME x;
        GetLocalTime(&x);
		//sprintf(file,"%02d%02d%02d%02d.log",x.wMonth,x.wDay,x.wHour,x.wMinute);
		sprintf(date,"%04d-%02d-%02dT%02d:%02d:%02d", x.wYear, x.wMonth, x.wDay, x.wHour, x.wMinute, x.wSecond);
    #else
        struct tm *x;
	time_t aclock;
	time(&aclock);						// Get time in seconds
	x = localtime(&aclock);			// Convert time to struct tm form
        //date = "2012-03-30T17:50:38"

        
		sprintf(date,"%04d-%02d-%02dT%02d:%02d:%02d", 1900+x->tm_year, x->tm_mon+1, x->tm_mday, x->tm_hour, x->tm_min, x->tm_sec);
        bp = 2;
    #endif

                return;
}                               // compute_date()



/*************************************************************************************************
    loads all of the sections, except the data

	general structure:
		RMHeader				RM1X
		PerfInfo				RMPI

		if (version > 1.01f)  {
			PerfInfoExt1		RMPX
		}

		perf data

		RMPC

*************************************************************************************************/

int RMP::load(const char *_fname, FILE *_dbgstream)  {
    int i, n;
    long offs;
	//long size;

	if (outfilename[0])  {
		if (!exists(fname))  {
			printf("can't open %s\n", fname);
			        return GENERIC_ERROR;
		}

		//size = filesize_from_name(fname);
	}

	fname = _fname;
	instream = fopen(fname, "rb");

	if (instream==NULL)  {
		printf("can't open %s\n", fname);
                return GENERIC_ERROR;
	}

	if (_dbgstream)  {
		fprintf(_dbgstream, "loading %s\n", mybasename(fname));
	}

	basename = mybasename(fname);

	load_start_time = timeGetTime();


	tag = get_next_tag(instream);
	if (tag==(INT32)0xffffffff)  {
		sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
		if (_dbgstream)  {
			fprintf(_dbgstream, "RM1X tag is 0xffffffff\n");
		}
		return WRONG_TAG;
	}
#ifdef _DEBUG
	//const char *cptr;
	//cptr = fourcc(tag);
#endif

	if (tag != (INT32)fourcc("RM1X"))  {                                                       // "RM1X", offs = 0
		sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
		if (_dbgstream)  {
			fprintf(_dbgstream, "RM1X tag is %s\n", fourcc(tag));
		}
		return WRONG_TAG;
	}

	if (_dbgstream)  {
		fprintf(_dbgstream, "\nfound RM1X tag\n");
	}

	n = (int)fread(&rmheader, sizeof(rmheader), 1, instream);
	if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
		if (_dbgstream)  {
			fprintf(_dbgstream, "error reading rmheader, n = %d\n", n);
		}
        return BAD_COUNT;
	}

	if (_dbgstream)  {
		fprintf(_dbgstream, "read rmheader, n = %d\n", n);
	}

	version = rmheader.version / 1000.0f;
	if (version < 1.0f || version > 20.0f)  {
		sprintf(errstr, "file: %s line: %d  %f", __FILE__, __LINE__, version);
		if (_dbgstream)  {
			fprintf(_dbgstream, "bad version = %.2f\n", version);
		}
        return BAD_VERSION;
    }
	if (_dbgstream)  {
		fprintf(_dbgstream, "version = %.2f\n", version);
	}

	///////////////////////////////////////////////////////////

	tag = get_next_tag(instream);
	if (tag==(INT32)0xffffffff)  {
            sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
		if (_dbgstream)  {
			fprintf(_dbgstream, "RMPI tag is 0xffffffff\n");
		}
            return WRONG_TAG;
	}
#ifdef _DEBUG
	//cptr = fourcc(tag);
#endif

	if (tag != (INT32)fourcc("RMPI"))  {
            sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
		if (_dbgstream)  {
			fprintf(_dbgstream, "RMPI tag is %s\n", fourcc(tag));
		}
            return WRONG_TAG;
	}
	if (_dbgstream)  {
		fprintf(_dbgstream, "\nfound RMPI tag\n");
	}

	n = (int)fread(&info, sizeof(RMP::PerfInfo), 1, instream);
	if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
		if (_dbgstream)  {
			fprintf(_dbgstream, "error reading info, n = %d\n", n);
		}
            return BAD_COUNT;
	}
	if (_dbgstream)  {
		fprintf(_dbgstream, "read info data\n");
	}

//#ifdef _DEBUG
#if 0
    FILE *dbginstream = fopen("info.log", "wt");
    dump_perfinfo(dbginstream, info);
    FCLOSE(dbginstream);
#endif

	perfcount = info.PerfCount;

	if (_dbgstream)  {
		fprintf(_dbgstream, "perfcount = %d\n", perfcount);
	}

	///////////////////////////////////////////////////////////

	if (version > 1.01f)  {

		tag = get_next_tag(instream);
		if (tag==(INT32)0xffffffff)  {
            sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
			if (_dbgstream)  {
				fprintf(_dbgstream, "RMPX tag is 0xffffffff\n");
			}
            return WRONG_TAG;
		}
#ifdef _DEBUG
		//cptr = fourcc(tag);
#endif

        if (tag != (INT32)fourcc("RMPX"))  {
			sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
			if (_dbgstream)  {
				fprintf(_dbgstream, "RMPX tag is %s\n", fourcc(tag));
			}
			return WRONG_TAG;
		}

		if (_dbgstream)  {
			fprintf(_dbgstream, "\nfound RMPX tag\n");
		}

		n = (int)fread(&infoext, sizeof(RMP::PerfInfoExt1), 1, instream);
		if (n != 1)  {
                    sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			if (_dbgstream)  {
				fprintf(_dbgstream, "error reading infoext, n = %d\n", n);
			}
                    return BAD_COUNT;
		}
//#ifdef _DEBUG
#if 0
                FILE *dbginstream = fopen("pix1.log", "wt");
                dump_pix1(dbginstream, infoext);
                FCLOSE(dbginstream);
#endif
	}						// if (version > 1.01f)  {

	if (_dbgstream)  {
		fprintf(_dbgstream, "read infoext\n");
	}

	///////////////////////////////////////////////////////////
	// now read the perf data:
	///////////////////////////////////////////////////////////


	data_start = ftell(instream);

#ifdef _DEBUG
	data_start_load = data_start;
#endif

	if (_dbgstream)  {
		fprintf(_dbgstream, "data_start = %ld\n", data_start);
	}

	bp = 0;
	int status;

	for(i=0; i<perfcount; i++)  {
#ifdef _DEBUG
		//long offs2;
		if (logg)  {
			writecount++;
			//offs2 = ftell(instream);
			//logg->write("%8d  %ld  %04x\n", writecount, offs2, group0.GroupFlags);
		}
		if (i==perfcount-1)  {
			bp = 1;
		}
#endif
		status = get_next_record();

#ifdef _DEBUG
		if (logg)  {
			//logg->write("%8d  %ld  %04x\n", writecount, offs2, group0.GroupFlags);
		}
#endif

		if (i==perfcount-1)  {
			bp = 1;
		}

		switch(status)  {
			case NIL_DATA:						// timestamp, but the data hasn't changed yet
                bp = 0;
                continue;
            case 0:
					if (i==0)  {
						if (group0.ms != 0)  {
							return GENERIC_ERROR;
						}
					}
                    break;
                default:
                    //bp = 7;
                    return status;
                    //break;
            }

            bp = 2;
	}

	if (_dbgstream)  {
		fprintf(_dbgstream, "read perf data\n");
	}

	perf_dur = group0.ms;

#ifdef _DEBUG
	offs = ftell(instream);				// 6154946
	data_end_load = offs;
#endif

        data_end = ftell(instream);                     // course_start

		if (_dbgstream)  {
			fprintf(_dbgstream, "data_end = %ld, perf_dur = %ld ms\n", data_end, perf_dur);
		}

#ifdef _DEBUG
		//if (data_end_load != data_end_finalize)  {
		//	FCLOSE(instream);
		//	return -1;
		//}
		//fseek(instream, data_end_finalize, SEEK_SET);
#endif

	//---------------------------------------------
	// now read the course
	//---------------------------------------------

        tag = get_next_tag(instream);
        if (tag==(INT32)0xffffffff)  {
            sprintf(errstr, "file: %s line: %d %x",  __FILE__, __LINE__,  tag);
			if (_dbgstream)  {
				fprintf(_dbgstream, "\nRMPC tag is 0xffffffff\n");
			}
            return WRONG_TAG;
	}
#ifdef _DEBUG
        //cptr = fourcc(tag);
#endif

    if (tag != (INT32)fourcc("RMPC"))  {
           sprintf(errstr, "file: %s line %d, bad tag: %x for %s",  __FILE__, __LINE__,  tag, fname);
			if (_dbgstream)  {
				fprintf(_dbgstream, "\nRMPC tag is %s (%x)\n", fourcc(tag), tag);
			}
           return WRONG_TAG;
	}

	if (_dbgstream)  {
		fprintf(_dbgstream, "\nfound RMPC tag\n");
	}

	course_header_start = ftell(instream);
	//memset(&ch, 0, sizeof(ch));
	n = (int)fread(&ch, sizeof(ch), 1, instream);
	if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			if (_dbgstream)  {
				fprintf(_dbgstream, "error reading ch, n = %d\n", n);
			}
            return BAD_COUNT;
	}


	if (_dbgstream)  {
		fprintf(_dbgstream, "read ch\n");
	}

//#ifdef _DEBUG
#if 0
        FILE *dbginstream = fopen("ch.log", "wt");
        dump_ch(dbginstream, &ch);
        FCLOSE(dbginstream);
#endif

	course_section_count = ch.Count;			// number of course sections, 4289
	//int status;

	hdrVersion = version;
	if (_dbgstream)  {
		fprintf(_dbgstream, "hdrVersion = %.2f, section count = %d\n", hdrVersion, course_section_count);
	}

	if (hdrVersion > 1.01f)  {
            XUnits = ch.XUnits;
            YUnits = ch.YUnits;
			if (_dbgstream)  {
				fprintf(_dbgstream, "XUnits = %d, YUnits = %d\n", XUnits, YUnits);
			}

	}

	strncpy((char *)m_OriginalHash, (char *)ch.OriginalHash, sizeof(m_OriginalHash)-1);
	strncpy((char *)m_CourseHash, (char *)ch.CourseHash, sizeof(m_CourseHash)-1);
	strncpy((char *)m_HeaderHash, (char *)ch.HeaderHash, sizeof(m_HeaderHash));

	bp = -1;
#ifdef _DEBUG
        //FILE *dbginstream = fopen("data.log", "wt");
#endif

	// CourseDataRCV data

	course_data_start = ftell(instream);

#ifdef _DEBUG
	int nn = sizeof(data);					// 176
	bp = 2;
	nn = nn * course_section_count;			// 754,864
#endif

	Node tnode, lastnode;

#ifdef _DEBUG
	if (logg)  {
		//logg->write("           LATITUDE               LONGITUDE               LENGTH                    FAZ\n\n");
	}
#endif

	Earth *earth = NULL;
	double clat, clon, minlat, minlon, maxlat, maxlon;
	//double firstlat, firstlon;
	//EARTHPOINT a, b;
	//double meters;
	//double faz, baz;
	double deg, min, sec;
	double minnorthing, maxnorthing, mineasting, maxeasting, northing, easting, firstnorthing, firsteasting;
	//double dx, dy;
	double start_northing, start_easting, end_northing, end_easting;
#ifdef _DEBUG
	//double lat, lon;
#endif

	//-----------------------------------------------------------------
	// first pass to get clat, clon, minlat, maxlat, minlon, maxlon
	//-----------------------------------------------------------------

#ifdef _DEBUG
	//const char *ccptr = mybasename(_fname);
	//ccptr = ch.Name;
	offs = ftell(instream);
	fseek(instream, course_data_start, SEEK_SET);
	offs = ftell(instream);
#endif

	minlat = minlon = FLT_MAX;
	maxlat = maxlon = -FLT_MAX;

	for(i=0; i<(int)course_section_count; i++)  {
		status = (int)fread(&data, sizeof(data), 1, instream);
		if (status != 1)  {
			sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			if (_dbgstream)  {
				fprintf(_dbgstream, "error reading course data, status = %d, i = %d\n", status, i);
			}
			return BAD_COUNT;
		}

#ifdef _DEBUG
		if (logg)  {
			//logg->write("%20.16lf   %20.16lf  %20.16lf  %20.16lf\n", data.gd.lat, data.gd.lon, data.Length, data.gd.faz);
		}
		if (data.gd.x != 0.0)  {
			bp = 2;
		}
		if (data.gd.y != 0.0)  {
			bp = 2;
		}
		if (data.gd.z != 0.0)  {
			bp = 2;
		}
#endif
		if (i==0)  {
			//firstlat = data.gd.lat;
			//firstlon = data.gd.lon;
		}
		minlat = MIN(data.gd.lat, minlat);
		maxlat = MAX(data.gd.lat, maxlat);
		minlon = MIN(data.gd.lon, minlon);
		maxlon = MAX(data.gd.lon, maxlon);
	}										// first pass

	clat = (minlat + maxlat) / 2.0;			// calc center lat in degrees
	clon = (minlon + maxlon) / 2.0;			// calc center lon in degrees

	earth = new Earth();

#ifdef _DEBUG
	/*
	//earth->test();
	earth->initTM(45.0, 0.0, 0.0, 0.0);
	lat = earth->ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(45.0, 5.0, 23.87361);
	lon = earth->ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(0.0, 3.0, 48.64881);
	earth->project(lat, lon, &northing, &easting);
	// northing should be 10000.000
	// easting should be   5000.000
	bp = 0;
	*/
#endif

	earth->initTM(clat, clon, 0.0, 0.0);	// lat, lon, northing, easting

	//----------------------------------------------
	// second pass to get the min northing, etc
	//----------------------------------------------

	minnorthing = mineasting = FLT_MAX;
	maxnorthing = maxeasting = -FLT_MAX;

	fseek(instream, course_data_start, SEEK_SET);

	for(i=0; i<course_section_count; i++)  {
		status = (int)fread(&data, sizeof(data), 1, instream);
		if (status != 1)  {
			sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			if (_dbgstream)  {
				fprintf(_dbgstream, "error reading course data, status = %d, i = %d\n", status, i);
			}
			DEL(earth);
            return BAD_COUNT;
		}

		earth->project(data.gd.lat, data.gd.lon, &northing, &easting);				// in meters

		if (i==0)  {
			firsteasting = easting;
			firstnorthing = northing;
		}
		minnorthing = MIN(minnorthing, northing);
		maxnorthing = MAX(maxnorthing, northing);
		mineasting = MIN(mineasting, easting);
		maxeasting = MAX(maxeasting, easting);
		bp = i;
	}												// second pass

	//dx = maxeasting - mineasting;					// in meters
	//dy = maxnorthing - minnorthing;					// in meters


	bp = 3;

	//------------------------------------------------------------
	// third pass is to load the normalized course:
	//------------------------------------------------------------

	fseek(instream, course_data_start, SEEK_SET);

	for(i=0; i<course_section_count; i++)  {
		status = (int)fread(&data, sizeof(data), 1, instream);
		if (status != 1)  {
			sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			if (_dbgstream)  {
				fprintf(_dbgstream, "error reading course data, status = %d, i = %d\n", status, i);
			}
			DEL(earth);
			return BAD_COUNT;
		}

		if (i==0)  {
			last_rcv_data = data;
			continue;
		}

		// from point:

		earth->ConvertDecimalDegreesToDegreesMinutesSeconds(last_rcv_data.gd.lat, deg, min, sec);
		/*
		a.lat.deg = (short)ROUND(deg);
		a.lat.min = (short)ROUND(min);
		a.lat.sec = sec;
		*/

		earth->ConvertDecimalDegreesToDegreesMinutesSeconds(last_rcv_data.gd.lon, deg, min, sec);
		/*
		a.lon.deg = (short)ROUND(deg);
		a.lon.min = (short)ROUND(min);
		a.lon.sec = sec;
		*/

		// to point:

		earth->ConvertDecimalDegreesToDegreesMinutesSeconds(data.gd.lat, deg, min, sec);
		/*
		b.lat.deg = (short)ROUND(deg);
		b.lat.min = (short)ROUND(min);
		b.lat.sec = sec;
		*/

		earth->ConvertDecimalDegreesToDegreesMinutesSeconds(data.gd.lon, deg, min, sec);
		/*
		b.lon.deg = (short)ROUND(deg);
		b.lon.min = (short)ROUND(min);
		b.lon.sec = sec;
		*/

		// cdamini:
		// 0.000,0.746,0.000,0.005
		// 0.746,2.238,0.005,0.016
		// 2.238,4.475,0.016,0.032

		// Lengths from RM1:
		//  .7458
		// 1.49175
		// 2.2376

		//meters = earth->getDist(a, b, &faz, &baz);				// 3.729

		earth->project(last_rcv_data.gd.lat, last_rcv_data.gd.lon, &start_northing, &start_easting);		// in meters
		earth->project(data.gd.lat, data.gd.lon, &end_northing, &end_easting);								// in meters

		start_northing -= firstnorthing;
		start_easting -= firsteasting;

		end_northing -= firstnorthing;
		end_easting -= firsteasting;

		// cdamini:
		// 0.000,0.746,0.000,0.005
		// 0.746,2.238,0.005,0.016
		// 2.238,4.475,0.016,0.032

		if (i==1)  {
			lastnode.count = 0L;
			lastnode.startx = 0.0;
			lastnode.starty = 0.0;
			lastnode.endx = start_easting;
			lastnode.endy = start_northing;
			nodes.push_back(lastnode);
		}

		tnode.count = (UINT32)i;
		tnode.startx = start_easting;
		tnode.starty = start_northing;
		tnode.endx = end_easting;
		tnode.endy = end_northing;

		// cdamini:
		// 0.000,0.746,0.000,0.005
		// 0.746,2.238,0.005,0.016
		// 2.238,4.475,0.016,0.032
		// return String.Format("{1:F3},{2:F3},{3:F3},{4:F3}",
		// Length, StartX, EndX, StartY, EndY );

		nodes.push_back(tnode);
		lastnode = tnode;
		bp = 0;
		last_rcv_data = data;
		bp = i;
	}								// third pass


	DEL(earth);


//#ifdef _DEBUG
#if 0
	int nnn = nodes.size();

	// 2.rmp:
	//		m_OriginalHash	0x0046dc12 "7C0FB597286935B39D6B8C66D335313"
	//		m_CourseHash	0x0046dc12 "7C0FB597286935B39D6B8C66D335313"
	//		m_HeaderHash	0x00d0dc32 "27BAE13D5166F7A7C25C0378CC3821D4"

	ccptr = compute_course_hash(true);

	//
	// _fname =				2012-03-19@13-55-54_1-Paris_5_NYCtri_1h16m32.1s.rmp
	//		ch.Name =		NYCtri
	//		ch.CourseHash =	7C0FB597286935B39D6B8C66D335313B
	//		ccptr =			F1D3FF8443297732862DF21DC4E57262

	// _fname =				2012-04-12@10-08-00_1-bob smith_5_CDAmini_0h01m04.3s.rmp
	//		ch.Name =		CDAmini
	//		ch.CourseHash =	E734FBB3B6E80349A05FF8535E93D19C
	//		ccptr =			083EC935DADA115615D615AE2F97C283

	// _fname =				2.rmp
	//		ch.Name =		CDAmini
	//		ch.CourseHash =	E734FBB3B6E80349A05FF8535E93D19C
	//		ccptr =			083EC935DADA115615D615AE2F97C283

	//FCLOSE(dbginstream);
#endif
	offs = ftell(instream);
	if (_dbgstream)  {
		fprintf(_dbgstream, "end of file offset = %ld\n", offs);
	}

	status = fgetc(instream);
	if (status != EOF)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			if (_dbgstream)  {
				fprintf(_dbgstream, "ERROR: EOF not there\n");
			}
            return EOF_NOT_FOUND;
	}

	//FCLOSE(instream);
	//fseek(instream, data_start, SEEK_SET);
	status = reset_input();
	if (status != OK)  {
		return status;
	}

	load_dur = timeGetTime() - load_start_time;				// .331 seconds vs 20.5 seconds for c#....  (.027 seconds on linux!!!)

	// test the file name:

	if (perf_dur != info.TimeMS)  {
#ifdef WIN32
		sprintf(gstr, "perf_dur != info.TimeMS, %ld vs %I64", perf_dur, info.TimeMS);
#elif defined(__MACH__)
		sprintf(gstr, "perf_dur != info.TimeMS, %ld vs %llu", perf_dur, info.TimeMS);
#else
		sprintf(gstr, "perf_dur != info.TimeMS, %ld vs %lu", perf_dur, info.TimeMS);
#endif
		throw(fatalError(__FILE__, __LINE__, gstr));
	}

    return 0;
}                                   // load()

/*************************************************************************************************

*************************************************************************************************/

int RMP::get_next_record(bool _freerun)  {
    int n;

#ifdef _DEBUG
	if (oktobreak)  {
		oktobreak = false;
	}
#endif

    if (instream==NULL)  {
        sprintf(errstr, "File not open: %s line: %d", __FILE__, __LINE__);
		return FILE_NOT_OPEN;
	}


	if (!_freerun)  {
		unsigned int ms;
		
		ms = (int)(next_ms + data_start_time);
		now = timeGetTime();
		if (now < ms)  {
			return NO_DATA_YET;
		}
		else  {
			bp = 2;
		}
	}

    //-----------------------------------------------------------
    // always get the time and GroupFlags
    //-----------------------------------------------------------

    offs = ftell(instream);					// 376

    n = (int)fread(&group0, sizeof(group0), 1, instream);
    if (n != 1)  {
        sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
        return BAD_COUNT;
    }

	records_in++;
	offs = ftell(instream);					// 382

#ifdef _DEBUG
	//if (group0.GroupFlags != 0x7fe)  {
	//	bp = 2;
	//}
#endif

    changedflags = group0.GroupFlags;

    if ((changedflags & RawStatFlags::Group_Mask) == RawStatFlags::Zero)  {
        //continue;
#ifdef _DEBUG
		if (!_freerun)  {
			bp = 1;
		}
#endif
        return NIL_DATA;
    }

    if ((changedflags & RawStatFlags::Group1) != RawStatFlags::Zero)  {
        n = (int)fread(&group1, sizeof(group1), 1, instream);
        if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
        }
#ifdef _DEBUG
		offs = ftell(instream);
#endif
		group1_count++;

		td.Power = group1.Watts;

		total_watts += group1.Watts;
		avg_watts = total_watts / group1_count;
		max_watts = MAX(max_watts, group1.Watts);
    }

    if ((changedflags & RawStatFlags::Group2) != RawStatFlags::Zero)  {
        n = (int)fread(&group2, sizeof(group2), 1, instream);
		if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
        }
#ifdef _DEBUG
		offs = ftell(instream);
#endif
		group2_count++;
		td.cadence = group2.Cadence;
		td.kph = group2.Speed;

		total_kph += group2.Speed;
		avg_kph = total_kph / group2_count;
		max_kph = MAX(max_kph, group2.Speed);

		total_rpm += group2.Cadence;
		avg_rpm = total_rpm / group2_count;
		max_rpm = MAX(max_rpm, group2.Cadence);
    }

    if ((changedflags & RawStatFlags::Group3) != RawStatFlags::Zero)  {
		n = (int)fread(&group3, sizeof(group3), 1, instream);
		if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
		td.HR = group5.HeartRate;
    }

    if ((changedflags & RawStatFlags::Group4) != RawStatFlags::Zero)  {
		n = (int)fread(&group4, sizeof(group4), 1, instream);
		if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
    }

    if ((changedflags & RawStatFlags::Group5) != RawStatFlags::Zero)  {
        n = (int)fread(&group5, sizeof(group5), 1, instream);
        if (n != 1)  {
             sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
              return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
		total_hr += group5.HeartRate;
		avg_hr = total_rpm / group5_count;
		max_hr = MAX(max_hr, group5.HeartRate);

    }

    if ((changedflags & RawStatFlags::Group6) != RawStatFlags::Zero)  {
		n = (int)fread(&group6, sizeof(group6), 1, instream);
		if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
    }

    if ((changedflags & RawStatFlags::Group7) != RawStatFlags::Zero)  {
		if (version > 1.01f)  {
            n = (int)fread(&group7x, sizeof(group7x), 1, instream);
            if (n != 1)  {
                sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
                return BAD_COUNT;
            }
#ifdef _DEBUG
			offs = ftell(instream);
#endif
	}
	else  {
		n = (int)fread(&group7, sizeof(group7), 1, instream);
        if (n != 1)  {
			sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
        }
#ifdef _DEBUG
		offs = ftell(instream);
#endif
        }

    }

    if ((changedflags & RawStatFlags::Group8) != RawStatFlags::Zero)  {
		n = (int)fread(&group8, sizeof(group8), 1, instream);
		if (n != 1)  {
             sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
              return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
    }

    if ((changedflags & RawStatFlags::Group9) != RawStatFlags::Zero)  {
	n = (int)fread(&group9, sizeof(group9), 1, instream);
		if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
    }

    if ((changedflags & RawStatFlags::Group10) != RawStatFlags::Zero)  {
		n = (int)fread(&group10, sizeof(group10), 1, instream);
		if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_COUNT;
		}
#ifdef _DEBUG
		offs = ftell(instream);
#endif
    }					// group 10

    if (version > 1.01f)  {
		if ((changedflags & RawStatFlags::Group11) != RawStatFlags::Zero)  {
            n = (int)fread(&group11, sizeof(group11), 1, instream);
            if (n != 1)  {
                sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
                return BAD_COUNT;
            }
#ifdef _DEBUG
			offs = ftell(instream);
#endif
		}

		if ((changedflags & RawStatFlags::Group12) != RawStatFlags::Zero)  {
            n = (int)fread(&group12, sizeof(group12), 1, instream);
            if (n != 1)  {
                 sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
                  return BAD_COUNT;
            }
#ifdef _DEBUG
			offs = ftell(instream);
#endif
		}
    }					// if (version > 1.01f)  {



	if (!_freerun)  {
		GROUP0 tgroup0;

		offs = ftell(instream);

		n = (int)fread(&tgroup0, sizeof(tgroup0), 1, instream);
	    if (n != 1)  {
		  sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
			return BAD_COUNT;
	    }
		next_ms = tgroup0.ms;
		fseek(instream, offs, SEEK_SET);
		/*
		 i           ms      next_ms     miles      MPH

		 0            0          254     0.000    15.82
		 1          254          257     0.001    16.03
		 2          257          288     0.001    16.22
		 3          288          321     0.001    16.22
		 4          321          354     0.001    16.22
		 5          354          388     0.002    16.22
		 6          388          421     0.002    16.22

		 ....

	137562      4591829      4591863    24.991    21.50
	137563      4591863      4591896    24.991    21.42
	137564      4591896      4591929    24.992    21.42
	137565      4591929      4591963    24.992    21.42
	137566      4591963      4591996    24.992    21.42
	137567      4591996      4592029    24.992    21.58
	137568      4592029      4592063    24.992    21.74
	137569      4592063      4592063    24.993    21.74
		*/

//#ifdef _DEBUG
#if 0
		sprintf(gstr, "%6d   %6ld   %6ld\n", records, group0.ms, tgroup0.ms);
		OutputDebugString(gstr);
		if (next_ms > 0)  {
			bp = 3;
		}
#endif
	}			// if (!_freerun)


    return 0;

}                                       // int RMP::get_next_record(void)  {

/*************************************************************************************************

*************************************************************************************************/

int RMP::save(const char *_fname)  {
    int n;

    fname = _fname;

    if (exists(fname))  {
        //sprintf(errstr, "file: %s line: %d %s", __FILE__, __LINE__,  fname);
        //return FILE_EXISTS;
    }


    outstream = fopen(fname, "wb");

    if (outstream==NULL)  {
        sprintf(errstr, "file: %s line: %d %s", __FILE__, __LINE__,  fname);
        return FILE_OPEN_ERROR;
    }

    // rmheader     RM1X
    // INFO         RMPI
    // infoext      RMPX
    // ch

    //-----------------------------------------------------
    // RMHEADER
    //-----------------------------------------------------

    memset(&rmheader, 0, sizeof(rmheader));

    rmheader.tag = fourcc("RM1X");
    rmheader.version = VERSION;
    //strncpy(rmheader.date, "now", sizeof(rmheader.date)-1);
    compute_date(rmheader.date);
    strncpy(rmheader.creator_exe, exe, sizeof(rmheader.creator_exe)-1);
    strncpy(rmheader.copyright, COPYRIGHT, sizeof(rmheader.copyright)-1);
    strncpy(rmheader.comment, "Performance File", sizeof(rmheader.comment)-1);
    rmheader.compress_type = 0;

#ifdef _DEBUG
    FILE *dbgstream = fopen("rmp.log", "wt");
    dump_header(dbgstream, rmheader);
    FCLOSE(dbgstream);
#endif

    n = (int)fwrite(&rmheader, sizeof(rmheader), 1, outstream);
    if (n != 1)  {
        sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
        return BAD_WRITE_COUNT;
    }

    //-----------------------------------------------------
    // PERFINFO
    //-----------------------------------------------------

    memset(&info, 0, sizeof(info));

    info.tag = fourcc("RMPI");      // INT32
    info.Age = 44;                  // INT16
    info.Height = 0;                // INT16
    info.Weight = 189;              // INT16
    info.Gender = 'M';              // BYTE
    info.HeartRate = 0;             // BYTE
    info.Upper_HeartRate = 70;      // BYTE
    info.Lower_HeartRate = 60;      // BYTE
    info.CourseType = 5;            // INT16
    info.Distance = 0.0;            // double
    info.RFDrag = 0.0f;             // float
    info.RFMeas = 0.0f;             // float
    info.Watts_Factor = 0.0f;       // float
    info.FTP = 0.0f;                // float
    info.PerfCount = 0;             // INT32
    info.TimeMS = 0LL;              // UINT64
    info.CourseOffset = 0LL;        // INT64
    strncpy(info.RiderName, "bob smith", sizeof(info.RiderName));
    strncpy(info.CourseName, "coursename", sizeof(info.CourseName));

    n = (int)fwrite(&info, sizeof(info), 1, outstream);
    if (n != 1)  {
        sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
        return BAD_WRITE_COUNT;
    }

#ifdef _DEBUG
//#if 0
    dbgstream = fopen("info.log", "wt");
    //dump_perfinfo(dbgstream, info);
    dump_perfinfo(dbgstream, info);
    FCLOSE(dbgstream);

#endif


    if (rmheader.version > 1.01f)  {
        infoext.Tag = fourcc("RMPX");               // INT32
        infoext.Mode = 0;                           // UINT16
	infoext.RawCalibrationValue = 0;            // INT16
	infoext.DragFactor = 0;                     // INT16
	infoext.DeviceType = 0;                     // UINT16
	infoext.DeviceVersion = 0;                  // UINT16
	infoext.PowerAeT = 0;                       // UINT16
	infoext.PowerFTP = 0;                       // UINT16
	infoext.HrAeT = 0;                          // BYTE
	infoext.HrMin = 0;                          // BYTE
	infoext.HrMax = 0;                          // BYTE
	infoext.HrZone1 = 0;                        // BYTE
	infoext.HrZone2 = 0;                        // BYTE
	infoext.HrZone3 = 0;                        // BYTE
	infoext.HrZone4 = 0;                        // BYTE
	infoext.HrZone5 = 0;                        // BYTE

        n = (int)fwrite(&infoext, sizeof(infoext), 1, outstream);
        if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_WRITE_COUNT;
        }
    }                                  // if (rmheader.version > 1.01f)  {

    INT32 i;

    for(i=0; i<info.PerfCount; i++)  {
        //write_record();
    }

    ch.Tag = fourcc("RMPC");                                        // INT32
    strncpy(ch.Name, "Name", sizeof(ch.Name)-1);                    // char
    strncpy(ch.FileName, "filename", sizeof(ch.FileName)-1);        // char
    strncpy(ch.Description, "desc", sizeof(ch.Description)-1);      // char
    ch.StartAt = 0;                                                 // double
    ch.EndAt = 0;                                                   // double
    ch.Length = 0;                                                  // float
    ch.Laps = 0;                                                    // int
    ch.Count = 0;                                                   // int
    ch.Type = 0;                                                    // BYTE
    ch.Attributes = 0;                                              // BYTE
    ch.XUnits = 0;                                                  // BYTE
    ch.YUnits = 0;                                                  // BYTE
    memset(ch.OriginalHash, 0, sizeof(ch.OriginalHash));            // BYTE
    memset(ch.CourseHash, 0, sizeof(ch.CourseHash));                // BYTE
    memset(ch.HeaderHash, 0, sizeof(ch.HeaderHash));                // BYTE

    n = (int)fwrite(&ch, sizeof(ch), 1, outstream);
    if (n != 1)  {
       sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
       return BAD_WRITE_COUNT;
    }

    FCLOSE(outstream);


    return 0;
}                                       // save()

/*************************************************************************************************

*************************************************************************************************/

int RMP::dump_perf_data(FILE *stream)  {
	int i, status, n;
	GROUP0 tgroup0;
	float avgdt;
	unsigned long totaldt = 0L;

	if (instream==NULL)  {
		return 1;
	}
	if (perfcount==0)  {
		return 1;
	}
	if (data_start==0L)  {
		return 1;
	}

	fprintf(stream, "\n     i           ms      next_ms     miles      MPH         offs\n\n");


	for(i=0; i<perfcount; i++)  {
		if (i==perfcount-1)  {
			offs = ftell(instream);
			bp = 0;
		}

		status = get_next_record();
		offs = ftell(instream);

		switch(status)  {
			case NIL_DATA:
                bp = 0;
                continue;
            case 0:									// there is data
				if (i==0)  {
					if (group0.ms != 0)  {
						return GENERIC_ERROR;
					}
				}

				n = (int)fread(&tgroup0, sizeof(tgroup0), 1, instream);
				if (n != 1)  {
					sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
					return BAD_COUNT;
				}


				next_ms = tgroup0.ms;
				fseek(instream, offs, SEEK_SET);

				/*
				if (i>0 && i<(perfcount-1))  {
					totaldt += (next_ms - group0.ms);
				}
				else  {
					bp = i;
				}
				*/


				/*

					 i           ms      next_ms     miles      MPH

					 0            0          254     0.000    15.82
					 1          254          257     0.001    16.03
					 2          257          288     0.001    16.22
					 3          288          321     0.001    16.22
					 4          321          354     0.001    16.22
					 5          354          388     0.002    16.22
					 6          388          421     0.002    16.22

					 ....

				137562      4591829      4591863    24.991    21.50
				137563      4591863      4591896    24.991    21.42
				137564      4591896      4591929    24.992    21.42
				137565      4591929      4591963    24.992    21.42
				137566      4591963      4591996    24.992    21.42
				137567      4591996      4592029    24.992    21.58
				137568      4592029      4592063    24.992    21.74
				137569      4592063      4592063    24.993    21.74

				*/

				fprintf(stream, "%6d   %10u   %10u   %7.3f   %6.2f   %10lu\n", i, group0.ms, next_ms, (float)TOMILES*group1.Distance, (float)TOMPH*group2.Speed, offs );
                break;
            default:
               //bp = 7;
               return status;
                    //break;
         }

         bp = 2;
	}

	assert(i==perfcount);

	avgdt = totaldt / (float)(perfcount-2);				// 33.37 ms

	fprintf(stream, "\navgdt = %.6f\n", avgdt);

	fseek(instream, data_start, SEEK_SET);

	return 0;
}												// dump_perf_data()


/*************************************************************************************************
 last called from main() / run() / firstrun (resets == 3)
*************************************************************************************************/

int RMP::reset_input(void)  {
	int status;

#ifdef _DEBUG
	resets++;
	if (resets==3)  {
		bp = 3;
	}
#endif

	avg_kph = 0.0f;
	max_kph = -FLT_MAX;
	total_kph = 0.0f;
	avg_rpm = 0.0f;
	max_rpm = 0;
	total_rpm = 0.0f;
	group2_count = 0L;

	avg_watts = 0.0f;
	max_watts = 0;
	total_watts = 0.0f;
	group1_count = 0L;

	avg_hr = 0.0f;
	max_hr = 0;
	total_hr = 0.0f;
	group5_count = 0L;


	if (!instream)  {
		instream = fopen(fname, "rb");
		if (instream==NULL)  {
            return FILE_OPEN_ERROR;
		}
	}

	if (data_start==0)  {
		return GENERIC_ERROR;
	}

	status = fseek(instream, data_start, SEEK_SET);
	if (status)  {
		return GENERIC_ERROR;
	}

	status = get_next_record();							// get the first record
	switch(status)  {
		case NIL_DATA:
			return GENERIC_ERROR;
		case 0:
			break;
		default:
			return GENERIC_ERROR;
	}

#ifdef _DEBUG
	if (resets==3)  {
		oktobreak = true;
	}
#endif

	// we have group0 at least

	next_ms = group0.ms;
	if (next_ms != 0)  {
		return GENERIC_ERROR;
	}

	status = fseek(instream, data_start, SEEK_SET);
	if (status)  {
		return GENERIC_ERROR;
	}

	offs = ftell(instream);

	records_in = 0L;
	data_start_time = timeGetTime();

	return OK;
}											// reset_input()

#if 0
/*************************************************************************************************

*************************************************************************************************/

int RMP::init_output(const char *_fname)  {
    int n;

    fname = _fname;

    if (exists(fname))  {
        //sprintf(errstr, "file: %s line: %d %s", __FILE__, __LINE__,  fname);
        //return FILE_EXISTS;
    }


    outstream = fopen(fname, "wb");

    if (outstream==NULL)  {
        sprintf(errstr, "file: %s line: %d %s", __FILE__, __LINE__,  fname);
        return FILE_OPEN_ERROR;
    }

    // rmheader     RM1X
    // INFO         RMPI
    // infoext      RMPX
    // ch

    //-----------------------------------------------------
    // RMHEADER
    //-----------------------------------------------------

    memset(&rmheader, 0, sizeof(rmheader));

    rmheader.tag = fourcc("RM1X");
    rmheader.version = VERSION;
    //strncpy(rmheader.date, "now", sizeof(rmheader.date)-1);
    compute_date(rmheader.date);
    strncpy(rmheader.creator_exe, exe, sizeof(rmheader.creator_exe)-1);
    strncpy(rmheader.copyright, COPYRIGHT, sizeof(rmheader.copyright)-1);
    strncpy(rmheader.comment, "Performance File", sizeof(rmheader.comment)-1);
    rmheader.compress_type = 0;

#ifdef _DEBUG
//    FILE *dbgstream = fopen("rmp.log", "wt");
//    dump_header(dbgstream, rmheader);
//    FCLOSE(dbgstream);
#endif

    n = fwrite(&rmheader, sizeof(rmheader), 1, outstream);
    if (n != 1)  {
        sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
        return BAD_WRITE_COUNT;
    }

    //-----------------------------------------------------
    // PERFINFO
    //-----------------------------------------------------

    memset(&info, 0, sizeof(info));

    info.tag = fourcc("RMPI");      // INT32
    info.Age = 44;                  // INT16
    info.Height = 0;                // INT16
    info.Weight = 189;              // INT16
    info.Gender = 'M';              // BYTE
    info.HeartRate = 0;             // BYTE
    info.Upper_HeartRate = 70;      // BYTE
    info.Lower_HeartRate = 60;      // BYTE
    info.CourseType = 5;            // INT16
    info.Distance = 0.0;            // double
    info.RFDrag = 0.0f;             // float
    info.RFMeas = 0.0f;             // float
    info.Watts_Factor = 0.0f;       // float
    info.FTP = 0.0f;                // float
    info.PerfCount = 0;             // INT32
    info.TimeMS = 0LL;              // UINT64
    info.CourseOffset = 0LL;        // INT64
    strncpy(info.RiderName, "bob smith", sizeof(info.RiderName));
    strncpy(info.CourseName, "coursename", sizeof(info.CourseName));

    n = fwrite(&info, sizeof(info), 1, outstream);
    if (n != 1)  {
        sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
        return BAD_WRITE_COUNT;
    }

//#ifdef _DEBUG
#if 0
    dbgstream = fopen("info.log", "wt");
    //dump_perfinfo(dbgstream, info);
    dump_perfinfo(dbgstream, info);
    FCLOSE(dbgstream);

#endif


    if (rmheader.version > 1.01f)  {
        infoext.Tag = fourcc("RMPX");               // INT32
        infoext.Mode = 0;                           // UINT16
		infoext.RawCalibrationValue = 0;            // INT16
		infoext.DragFactor = 0;                     // INT16
		infoext.DeviceType = 0;                     // UINT16
		infoext.DeviceVersion = 0;                  // UINT16
		infoext.PowerAeT = 0;                       // UINT16
		infoext.PowerFTP = 0;                       // UINT16
		infoext.HrAeT = 0;                          // BYTE
		infoext.HrMin = 0;                          // BYTE
		infoext.HrMax = 0;                          // BYTE
		infoext.HrZone1 = 0;                        // BYTE
		infoext.HrZone2 = 0;                        // BYTE
		infoext.HrZone3 = 0;                        // BYTE
		infoext.HrZone4 = 0;                        // BYTE
		infoext.HrZone5 = 0;                        // BYTE

        n = fwrite(&infoext, sizeof(infoext), 1, outstream);
        if (n != 1)  {
            sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
            return BAD_WRITE_COUNT;
        }
    }                                  // if (rmheader.version > 1.01f)  {

	/*
    INT32 i;
	for(i=0; i<info.PerfCount; i++)  {
        write_record();
    }
	*/

	/*
    ch.Tag = fourcc("RMPC");                                        // INT32
    strncpy(ch.Name, "Name", sizeof(ch.Name)-1);                    // char
    strncpy(ch.FileName, "filename", sizeof(ch.FileName)-1);        // char
    strncpy(ch.Description, "desc", sizeof(ch.Description)-1);      // char
    ch.StartAt = 0;                                                 // double
    ch.EndAt = 0;                                                   // double
    ch.Length = 0;                                                  // float
    ch.Laps = 0;                                                    // int
    ch.Count = 0;                                                   // int
    ch.Type = 0;                                                    // BYTE
    ch.Attributes = 0;                                              // BYTE
    ch.XUnits = 0;                                                  // BYTE
    ch.YUnits = 0;                                                  // BYTE
    memset(ch.OriginalHash, 0, sizeof(ch.OriginalHash));            // BYTE
    memset(ch.CourseHash, 0, sizeof(ch.CourseHash));                // BYTE
    memset(ch.HeaderHash, 0, sizeof(ch.HeaderHash));                // BYTE

    n = fwrite(&ch, sizeof(ch), 1, outstream);
    if (n != 1)  {
       sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
       return BAD_WRITE_COUNT;
    }
	*/

    //FCLOSE(outstream);


    return 0;
}                                       // init_output()

#endif

/*************************************************************************************************

*************************************************************************************************/

int RMP::finalize(void)  {
	int n;
#ifdef _DEBUG
	long endoffs, offs;
#endif

	if (!outstream)  {
		return 1;
	}


	//----------------------------------------
	// update the perfinfo
	//----------------------------------------

	//info.TimeMS = timeGetTime() - start_time;
	info.TimeMS = group0.ms;
	info.PerfCount = (int)records_out;
	info.CourseOffset = ftell(outstream);

#ifdef _DEBUG
	endoffs = ftell(outstream);
#endif
	/*
	general structure:
		RMHeader				RM1X
		PerfInfo				RMPI

		if (version > 1.01f)  {
			PerfInfoExt1		RMPX
		}

		perf data

		RMPC
	*/

	//fseek(outstream, sizeof(RMP_HEADER), SEEK_SET);

	fseek(outstream, sizeof(RMHeader), SEEK_SET);
#ifdef _DEBUG
	offs = ftell(outstream);				// 128
	n = sizeof(info);						// 220
	n = sizeof(PerfInfo);
											// 348
#endif

    n = (int)fwrite(&info, sizeof(info), 1, outstream);
    if (n != 1)  {
        sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
        return BAD_WRITE_COUNT;
    }

#ifdef _DEBUG
	offs = ftell(outstream);
	assert(offs==sizeof(RMHeader)+sizeof(PerfInfo));
#endif

	//fseek(outstream, sizeof(RMHeader)+sizeof(PerfInfo), SEEK_SET);
#ifdef _DEBUG
	data_start_finalize = data_start;
	//offs = ftell(outstream);
	//assert(offs==sizeof(RMHeader)+sizeof(PerfInfo));
	fseek(outstream, sizeof(RMHeader)+sizeof(PerfInfo)+sizeof(infoext), SEEK_SET);
	offs = ftell(outstream);					// 376
	if (offs != data_start)  {
		bp = 5;
	}
#endif
	fseek(outstream, 0L, SEEK_END);

	offs = ftell(outstream);
	
#ifdef _DEBUG
	if (offs != endoffs)  {
		bp = 5;
	}
	data_end_finalize = offs;
#endif

	data_end = offs;

    ch.Tag = fourcc("RMPC");                                        // INT32
    strncpy(ch.Name, "Name", sizeof(ch.Name)-1);                    // char
    strncpy(ch.FileName, "filename", sizeof(ch.FileName)-1);        // char
    strncpy(ch.Description, "desc", sizeof(ch.Description)-1);      // char
    ch.StartAt = 0;                                                 // double, distance in meters
    ch.EndAt = 1000.0*group1.Distance;                              // double, distance in meters

#ifdef _DEBUG
	//double d = TOMILES*group1.Distance;
#endif

    ch.Length = 0;                                                  // float
    ch.Laps = 1;                                                    // int
    ch.Count = 0;                                                   // int

	/*

	from tdefs.h:

enum CourseType  {
	Zero = 0,
	Distance = 0x01,
	Watts = 0x02,
	Video = 0x04,
	ThreeD = 0x08,
	GPS = 0x10,
	//Performance = 0x20
	Perf = 0x20
};
	*/


    ch.Type = Video;                                                    // BYTE
    ch.Attributes = 0;                                              // BYTE
    ch.XUnits = 0;                                                  // BYTE
    ch.YUnits = 0;                                                  // BYTE
    memset(ch.OriginalHash, 0, sizeof(ch.OriginalHash));            // BYTE
    memset(ch.CourseHash, 0, sizeof(ch.CourseHash));                // BYTE
    memset(ch.HeaderHash, 0, sizeof(ch.HeaderHash));                // BYTE

    n = (int)fwrite(&ch, sizeof(ch), 1, outstream);
    if (n != 1)  {
       sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
       return BAD_WRITE_COUNT;
    }

#ifdef _DEBUG
	offs = ftell(outstream);
#endif

    FCLOSE(outstream);

	// now rename the file to show the finish time
	// info.TimeMS, outfilename

	/*
	cptr = options.sites[options.six].name.c_str();
	pcre = new Pcre(cptr, "[A-z0-9]", NULL, opts, false);
	matches = pcre->get_matches();
	DEL(pcre);
	if (matches != 1)  {
		return "Malformed Test Site";
	}
	*/

	//	"2012-06-26@08-49-55_1-John Manley_5_NYCtri_0h00m00.0s.rmp",		"\\dh\\d\\dm\\d\\d\\.\\ds\\.rmp$"
	Pcre *pcre = NULL;
	int start;
	int len;
	int options=0;
	int matches=0;

	options = PCRE_CASELESS;

	pcre = new Pcre(outfilename, "\\dh\\d\\dm\\d\\d\\.\\ds\\.rmp$", NULL, options, false);
	matches = pcre->get_matches();

	char str[MAX_PATH];

	if (matches==1)  {
		start = pcre->get_offset(1);							// get first match offset
		len = pcre->get_len(1);									// get first match length
		if (start>=0)  {
			if (len != 14)  {
				DEL(pcre);
				throw(fatalError(__FILE__, __LINE__));
				//return 0;
			}
		}
		else  {
			DEL(pcre);
			throw(fatalError(__FILE__, __LINE__));
			//return 0;
		}

		//strreplace();
		char newstr[256];
		unsigned long ms = (unsigned long)info.TimeMS;
		int hours, minutes, seconds, tenths;

		strncpy(str, outfilename, sizeof(str)-1);

		hours = (int)(ms / (3600*1000));
		minutes = (int)(ms - hours * 3600 * 1000);
		minutes /= 60*1000;
		if (minutes==60)  {
			minutes = 0;
			hours++;
		}

		seconds = (int)(ms - (hours * 3600 * 1000) - (minutes * 60 * 1000));
		seconds /= 1000;
		if (seconds==60)  {
			seconds = 0;
			minutes++;
			if (minutes==60)  {
				minutes = 0;
				hours++;
			}
		}
		
		tenths = (int)(ms - (hours * 3600 * 1000) - (minutes * 60 * 1000) - (seconds*1000));
		tenths = ROUND((float)tenths / 100.0f);

		if (tenths==10)  {
			tenths = 0;
			seconds++;
			if (seconds==60)  {
				seconds = 0;
				minutes++;
				if (minutes==60)  {
					minutes = 0;
					hours++;
				}
			}
		}

		sprintf(newstr, "%1dh%02dm%02d.%1ds", hours, minutes, seconds, tenths);
		replace(str, "0h00m00.0s", newstr);

		rename(outfilename, str);			// (old, new)
		strncpy(outfilename, str, sizeof(outfilename)-1);

	}
	else  {
		//DEL(pcre);
		//throw(fatalError(__FILE__, __LINE__));
	}

	DEL(pcre);

#ifdef _DEBUG
	dump("final.txt");
#endif

	return 0;
}								// finalize()

/*************************************************************************************************
 last called from main() / run() / firstrun (resets == 3)
*************************************************************************************************/

int RMP::reset_output(void)  {

	records_out = 0L;
	//start_time = timeGetTime();
	first = true;

	return 0;
}										// reset_output()


/*************************************************************************************************

*************************************************************************************************/

int RMP::write_record(RMPDATA *_data)  {
	int i;

	if (!outstream)  {
		return 2;
	}

	/*
        typedef struct  {               	// these always change
            UINT32 ms;
            UINT16 GroupFlags;
        } GROUP0;
	*/

	if (!first)  {
		group0.ms = (UINT32)(timeGetTime() - start_time);
		group0.GroupFlags = 0;									// UINT16
	}
	else  {
		first = false;
		group0.ms = 0L;
		records_out = 0L;
		memset(&lastdata, 0, sizeof(RMPDATA));
		group0.GroupFlags = 
			RawStatFlags::Group1 |
			RawStatFlags::Group2 |
			RawStatFlags::Group3 |
			RawStatFlags::Group4 |
			RawStatFlags::Group5 |
			RawStatFlags::Group6 |
			RawStatFlags::Group7 |
			RawStatFlags::Group8 |
			RawStatFlags::Group9 |
			RawStatFlags::Group10 |
			RawStatFlags::Group11 |
			RawStatFlags::Group12
			;
		start_time = timeGetTime();
		goto after_first_time;
	}


	/*
        typedef struct  {                   // group 1 changes often
            float Distance;
            UINT16 Watts;
            float Watts_Load;
        } GROUP1;
	*/

	if ( _data->Distance != lastdata.Distance || _data->Watts != lastdata.Watts || _data->Watts_Load != lastdata.Watts_Load)  {
		group0.GroupFlags |= RawStatFlags::Group1;
		group1.Distance = _data->Distance;
		group1.Watts = _data->Watts;
		group1.Watts_Load = _data->Watts_Load;
	}

	/*
    typedef struct  {
		float Speed;
        BYTE Cadence;
    } GROUP2;
	*/

	if (_data->Speed != lastdata.Speed || _data->Cadence != lastdata.Cadence)  {
		group0.GroupFlags |= RawStatFlags::Group2;
		group2.Speed = _data->Speed;
		group2.Cadence = _data->Cadence;
	}

	/*
        typedef struct  {
            INT16 Grade;
        } GROUP3;
	*/

	if (_data->Grade != lastdata.Grade)  {
		group0.GroupFlags |= RawStatFlags::Group3;
		group3.Grade = _data->Grade;
	}

	/*
        typedef struct  {
            float Wind;
        } GROUP4;
	*/

	if (_data->Wind != lastdata.Wind)  {
		group0.GroupFlags |= RawStatFlags::Group4;
		group4.Wind = _data->Wind;
	}

	/*
        typedef struct  {
            BYTE HeartRate;
        } GROUP5;
	*/

	if (_data->HeartRate != lastdata.HeartRate)  {
		group0.GroupFlags |= RawStatFlags::Group5;
		group5.HeartRate = _data->HeartRate;
	}

	/*
        typedef struct  {
            UINT16 Calories;
        } GROUP6;
	*/

	if (_data->Calories != lastdata.Calories)  {
		group0.GroupFlags |= RawStatFlags::Group6;
		group6.Calories = _data->Calories;
	}

	/*
        typedef struct  {
            BYTE PulsePower;
            BYTE DragFactor;											<<<<< todo
        } GROUP7;

        typedef struct  {
            BYTE PulsePower;
            BYTE DragFactor;
            INT16 RawCalibrationValue;
        } GROUP7X;

		version
	*/

	//version = rmheader.version / 1000.0f;
#ifdef _DEBUG
	if (version < 1.0f || version > 20.0f)  {
		throw(fatalError(__FILE__, __LINE__, "Bad Version"));
	}
#endif

	/*
    if ((changedflags & RawStatFlags::Group7) != RawStatFlags::Zero)  {
		if (version > 1.01f)  {
            n = fread(&group7x, sizeof(group7x), 1, instream);
            if (n != 1)  {
                sprintf(errstr, "file: %s line: %d %d", __FILE__, __LINE__,  n);
                return BAD_COUNT;
            }
#ifdef _DEBUG
			offs = ftell(instream);
#endif
	}
	else  {
		n = fread(&group7, sizeof(group7), 1, instream);
    }
	*/

	if (_data->PulsePower != lastdata.PulsePower || _data->DragFactor != lastdata.DragFactor)  {
		group0.GroupFlags |= RawStatFlags::Group7;
		if (version > 1.01f)  {
			group7x.PulsePower = _data->PulsePower;
			group7x.DragFactor = _data->DragFactor;
			group7x.RawCalibrationValue = _data->RawCalibrationValue;
		}
		else  {
			group7.PulsePower = _data->PulsePower;
			group7.DragFactor = _data->DragFactor;
		}
	}


	/*
        typedef struct  {
            INT16 FrontGear;							<<<<<<<<<<<<<<<<<<<<
            INT16 RearGear;								<<<<<<<<<<<<<<<<<<<<
            INT16 GearInches;
        } GROUP8;
	*/

	if (_data->FrontGear != lastdata.FrontGear || _data->RearGear != lastdata.RearGear || _data->GearInches != lastdata.GearInches)  {
		group0.GroupFlags |= RawStatFlags::Group8;
		group8.GearInches = _data->GearInches;
		group8.FrontGear = _data->FrontGear;
		group8.RearGear = _data->RearGear;
	}

	/*
        typedef struct  {
            float TSS;
            float IF;
            float NP;
        } GROUP9;
	*/

	if (_data->TSS != lastdata.TSS || _data->IF != lastdata.IF || _data->NP != lastdata.NP)  {
		group0.GroupFlags |= RawStatFlags::Group9;
		group9.TSS = _data->TSS;
		group9.IF = _data->IF;
		group9.NP = _data->NP;
	}

	/*
        typedef struct  {
            BYTE SS;
            BYTE SSLeftATA;
            BYTE SSRightATA;
            BYTE SSRight;
            BYTE SSLeft;
            BYTE SSLeftSplit;
            BYTE SSRightSplit;
            BYTE bars[24];
        } GROUP10;
	*/

	if (_data->SS != lastdata.SS)  {
		group0.GroupFlags |= RawStatFlags::Group10;
		group10.SS = _data->SS;
		group10.SSLeftATA = _data->SSLeftATA;
		group10.SSRightATA = _data->SSRightATA;
		group10.SSRight = _data->SSRight;
		group10.SSLeft = _data->SSLeft;
		group10.SSLeftSplit = _data->SSLeftSplit;
		group10.SSRightSplit = _data->SSRightSplit;
		for(i=0; i<24; i++)  {
			group10.bars[i] = _data->bars[i];
		}
	}

	/*
        typedef struct  {
            UINT16 m_flags1;
        } GROUP11;
	*/

	if (_data->m_flags1 != lastdata.m_flags1)  {
		group0.GroupFlags |= RawStatFlags::Group11;
		group11.m_flags1 = _data->m_flags1;
	}

	/*
        typedef struct  {
            UINT16 m_flags2;
        } GROUP12;
	*/
	
	if (_data->m_flags2 != lastdata.m_flags2)  {
		group0.GroupFlags |= RawStatFlags::Group12;
		group12.m_flags2 = _data->m_flags2;
	}


after_first_time:

#ifdef _DEBUG
	long offs2;
	if (logg)  {
		writecount++;
		if (writecount==2)  {
			bp = 2;
		}
		offs2 = ftell(outstream);							// 376, 461
		logg->write("%8d  %ld   %04x", writecount, offs2, group0.GroupFlags);
	}
#endif

	fwrite(&group0, sizeof(GROUP0), 1, outstream);

#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  0:%ld", offs2);
#endif

    //if ((group0.GroupFlags & RawStatFlags::Group1) != RawStatFlags::Zero)  {
    if (group0.GroupFlags & RawStatFlags::Group1)  {
		fwrite(&group1, sizeof(GROUP1), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  1:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group2)  {
		fwrite(&group2, sizeof(GROUP2), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  2:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group3)  {
		fwrite(&group3, sizeof(GROUP3), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  3:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group4)  {
		fwrite(&group4, sizeof(GROUP4), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  4:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group5)  {
		fwrite(&group5, sizeof(GROUP5), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  5:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group6)  {
		fwrite(&group6, sizeof(GROUP6), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  6:%ld", offs2);
#endif
	}

    if (group0.GroupFlags & RawStatFlags::Group7)  {
		if (version > 1.01f)  {
			fwrite(&group7x, sizeof(GROUP7X), 1, outstream);
		}
		else  {
			fwrite(&group7, sizeof(GROUP7), 1, outstream);
		}
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  7:%ld", offs2);
#endif
	}

    if (group0.GroupFlags & RawStatFlags::Group8)  {
		fwrite(&group8, sizeof(GROUP8), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  8:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group9)  {
		fwrite(&group9, sizeof(GROUP9), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  9:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group10)  {
		fwrite(&group10, sizeof(GROUP10), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  10:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group11)  {
		fwrite(&group11, sizeof(GROUP11), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  11:%ld", offs2);
#endif
	}
    if (group0.GroupFlags & RawStatFlags::Group12)  {
		fwrite(&group12, sizeof(GROUP12), 1, outstream);
#ifdef _DEBUG
		offs2 = ftell(outstream);
		if (logg) logg->write("  12:%ld", offs2);
#endif
	}

#ifdef _DEBUG
		if (logg) logg->write("\n");
#endif

#ifdef _DEBUG
	//i = sizeof(*_data);
	//i = sizeof(lastdata);
	//i = sizeof(RMPDATA);
#endif

	memcpy(&lastdata, _data, sizeof(lastdata));
	records_out++;

    return 0;
}						// write_record()

/*************************************************************************************************

*************************************************************************************************/

const char *RMP::compute_course_hash(bool _ascii_hex)  {
	int i;			//, status, k;
	char s[64];
	int nn, nodesize, len;

	len = (int)nodes.size();				// 4289
	nodesize = sizeof(Node);		// 36
	nn = nodesize * len;			// 154,404
	// make buffer size plenty large:
	nn *= 2;
	nn += 8;

	unsigned char *buf = new unsigned char[nn];
	memset(buf, 0, nn);

	// 2.rmp:
	//		m_OriginalHash	0x0046dc12 "7C0FB597286935B39D6B8C66D335313"
	//		m_CourseHash	0x0046dc12 "7C0FB597286935B39D6B8C66D335313"
	//		m_HeaderHash	0x00d0dc32 "27BAE13D5166F7A7C25C0378CC3821D4"

	// cdamini:
	// 0.000,0.746,0.000,0.005
	// 0.746,2.238,0.005,0.016
	// 2.238,4.475,0.016,0.032
	// return String.Format("{1:F3},{2:F3},{3:F3},{4:F3}",
	// Length, StartX, EndX, StartY, EndY );

	//int n = nodes.size();
	int totlen = 0;

	for(i=0; i<len; i++)  {
#ifdef WIN32
		sprintf(s, "%d%.3lf,%.3lf,%.3lf,%.3lf", i, nodes[i].startx, nodes[i].endx, nodes[i].starty, nodes[i].endy);
#else
		sprintf(s, "%d%.3f,%.3f,%.3f,%.3f", i, nodes[i].startx, nodes[i].endx, nodes[i].starty, nodes[i].endy);
#endif
		totlen += strlen(s);
		if (totlen>nn)  {
			bp = 1;
			delete[] buf;
			sprintf(gstr, "t = %d, n = %d", totlen, nn);
			throw(fatalError(__FILE__, __LINE__, gstr));
		}
		strcat((char *)buf, s);
	}

	//strcat((char *)buf, "0");

	MD5 *md5 = NULL;
	md5 = new MD5();
	//md5->string((unsigned char *)buf, sizeof(buf)-1 );
	md5->string((unsigned char *)buf, (int)strlen((const char *)buf) );
	// md5->digest[16] has the code
	memcpy(hash, md5->digest, 16);
	DEL(md5);
	delete[] buf;

	//----------------------------------------------------------------------------
	// 2.rmp:
	//		m_OriginalHash	0x0046dc12 "7C0FB597286935B39D6B8C66D335313"
	//		m_CourseHash	0x0046dc12 "7C0FB597286935B39D6B8C66D335313"
	//		m_HeaderHash	0x00d0dc32 "27BAE13D5166F7A7C25C0378CC3821D4"
	//----------------------------------------------------------------------------

	if (_ascii_hex)  {
		char str[32];
		memset(asciihexhash, 0, sizeof(asciihexhash));
		for(i=0; i<16; i++)  {
			sprintf(str, "%02X", hash[i]);
			strcat(asciihexhash, str);
		}
		return (const char *)asciihexhash;
	}
	else  {
		return (const char *)hash;
	}
}											// const char *compute_course_hash(void)


/*************************************************************************************************

*************************************************************************************************/

const char *RMP::compute_header_hash(bool _ascii_hex)  {
	int i;		// , status, k;

	MD5 *md5 = NULL;
	char str[256];
	char course_hash[48];

	memset(course_hash, 0, sizeof(course_hash));
	memset(str, 0, sizeof(str));

	// 2012-03-19@13-55-54_1-Paris_5_NYCtri_1h16m32.1s.rmp:
	//		sss = String.Format("{0},{1:F3},{2:F3},{3},{4}", CourseHash, StartAt, EndAt, (byte)Attributes, Laps);
	//		7C0FB597286935B39D6B8C66D335313B,0.000,40221.788,0,1		C++

	// 2012-04-12@10-08-00_1-bob smith_5_CDAmini_0h01m04.3s.rmp
	//		59EEB4F5FEB143A68C5E7F30622177EC,0.000,269.098,0,1			C#
	//		E734FBB3B6E80349A05FF8535E93D19C,0.000,269.098,0,1			C++

	memcpy(course_hash, ch.CourseHash, 32);			// to have a null termination in the string
	sprintf(str, "%32s,%.3f,%.3f,%d,%d", course_hash, ch.StartAt, ch.EndAt, ch.Attributes, ch.Laps);

	md5 = new MD5();
	md5->string((unsigned char *)str, sizeof(str) );
	// md5->digest[16] has the code
	memcpy(hash, md5->digest, 16);
	DEL(md5);

	if (_ascii_hex)  {
		char str[32];
		memset(result, 0, sizeof(result));
		for(i=0; i<16; i++)  {
			sprintf(str, "%02X", hash[i]);
			strcat(result, str);
		}
		return result;
	}
	else  {
		return (const char *)hash;
	}

}											// const char *compute_header_hash(void)


#ifdef _DEBUG

/*************************************************************************************************

*************************************************************************************************/

void RMP::dump(const char *_fname, const char *_mode)  {
	//int status;

	FILE *stream = fopen(_fname, _mode);
	
	if (!strcmp(_mode, "a+t"))  {
		fprintf(stream, "\n\n*****************************************************\n\n");
	}

	fprintf(stream, "%s\n\n", mybasename(outfilename));

	dump_header(stream, rmheader);
	dump_perfinfo(stream, info);
	dump_pix1(stream, infoext);

	fprintf(stream, "\n%d performance records are here\n", info.PerfCount);

#if 0
	//dump_perf_records();
	instream = stream;				// so get_next_record() will work

	while(1)  {
		status = get_next_record(true);
		if (status)  {
			break;
		}
	}

	instream = NULL;

	offs = ftell(stream);
	if (offs != data_end)  {
		bp = 1;
	}
#endif

	dump_ch(stream, ch);


	FCLOSE(stream);

	return;
}										// dump()


/*************************************************************************************************

*************************************************************************************************/

void RMP::dump_header(FILE *instream, RMHeader _rmheader)  {

    /*
        typedef struct  {
            INT32 tag;                              //   4 bytes "RM1X"
            INT16 version;                          //   2 bytes - version of this format / 1000
            char date[24];                          //  24 bytes date created
            char creator_exe[32];                   //  32 bytes program that created
            char copyright[32];                     //  32 bytes RacerMate copyright
            char comment[32];                       //  32 bytes description of this file
            INT16 compress_type;                    //   2 bytes different compression type
        } RMHeader;									// 128 bytes
    */

    fprintf(instream, "\n");
    //fprintf(instream, "tag = %08x\n", _rmheader.tag);

	const char *cptr = fourcc(_rmheader.tag);
    fprintf(instream, "tag = %s\n", cptr);
    fprintf(instream, "version = %d\n", _rmheader.version);
    fprintf(instream, "date = %s\n", _rmheader.date);
    fprintf(instream, "creator_exe = %s\n", _rmheader.creator_exe);
    fprintf(instream, "copyright = %s\n", _rmheader.copyright);
    fprintf(instream, "comment = %s\n", _rmheader.comment);
	fprintf(instream, "compress_type = %d\n", _rmheader.compress_type);

	return;
}                                       // dump_header()

/*************************************************************************************************

*************************************************************************************************/

void RMP::dump_perfinfo(FILE *instream, PerfInfo pi)  {
    const char *cptr;

    fprintf(instream, "\n");

    cptr = fourcc(pi.tag);
    fprintf(instream, "tag = %s\n", cptr);

    fprintf(instream, "RiderName = %s\n", pi.RiderName );                  // char
    fprintf(instream, "Age = %d\n", pi.Age );                              // INT16
    fprintf(instream, "Height = %d\n", pi.Height );                        // INT16
    fprintf(instream, "Weight = %d\n", pi.Weight );                        // INT16
    fprintf(instream, "Gender = %c\n", pi.Gender );                        // BYTE
    fprintf(instream, "HeartRate = %d\n", pi.HeartRate );                  // BYTE
    fprintf(instream, "Upper_HeartRate = %d\n", pi.Upper_HeartRate );      // BYTE
    fprintf(instream, "Lower_HeartRate = %d\n", pi.Lower_HeartRate );      // BYTE
    fprintf(instream, "CourseName = %s\n", pi.CourseName );                // char
    fprintf(instream, "CourseType = %d\n", pi.CourseType );                // INT16
    fprintf(instream, "Distance = %f\n", pi.Distance );                    // double
    fprintf(instream, "RFDrag = %f\n", pi.RFDrag );                        // float
    fprintf(instream, "RFMeas = %f\n", pi.RFMeas );                        // float
    fprintf(instream, "Watts_Factor = %f\n", pi.Watts_Factor );            // float
    fprintf(instream, "FTP = %f\n", pi.FTP );                              // float
    fprintf(instream, "PerfCount = %d\n", pi.PerfCount );                  // INT32
#if defined(WIN32)
    fprintf(instream, "TimeMS = %I64\n", pi.TimeMS );                       // UINT64
    fprintf(instream, "CourseOffset = %I64\n", pi.CourseOffset );           // INT64
#elif defined (__MACH__)
    fprintf(instream, "TimeMS = %llu\n", pi.TimeMS );                       // UINT64
    fprintf(instream, "CourseOffset = %lld\n", pi.CourseOffset );           // INT64
#else
    fprintf(instream, "TimeMS = %lu\n", pi.TimeMS );                       // UINT64
    fprintf(instream, "CourseOffset = %ld\n", pi.CourseOffset );           // INT64
#endif	 

    return;
}                   // dump_perfinfo()

/*************************************************************************************************

*************************************************************************************************/

void RMP::dump_pix1(FILE *instream, PerfInfoExt1 pix1)  {
    const char *cptr;

    fprintf(instream, "\n");

    cptr = fourcc(pix1.Tag);
    fprintf(instream, "tag = %s\n", cptr);
    fprintf(instream, "Mode = %d\n", pix1.Mode );                                         // UINT16
    fprintf(instream, "RawCalibrationValue = %d\n", pix1.RawCalibrationValue );		// INT16
    fprintf(instream, "DragFactor = %d\n", pix1.DragFactor );                             // INT16
    fprintf(instream, "DeviceType = %d\n", pix1.DeviceType );                             // UINT16
    fprintf(instream, "DeviceVersion = %d\n", pix1.DeviceVersion );                       // UINT16
    fprintf(instream, "PowerAeT = %d\n", pix1.PowerAeT );                 		// UINT16
    fprintf(instream, "PowerFTP = %d\n", pix1.PowerFTP );                                 // UINT16
    fprintf(instream, "HrAeT = %d\n", pix1.HrAeT );                                       // BYTE
    fprintf(instream, "HrMin = %d\n", pix1.HrMin );                                       // BYTE
    fprintf(instream, "HrMax = %d\n", pix1.HrMax );                                       // BYTE
    fprintf(instream, "HrZone1 = %d\n", pix1.HrZone1 );                                   // BYTE
    fprintf(instream, "HrZone2= %d\n", pix1.HrZone2 );                                    // BYTE
    fprintf(instream, "HrZone3 = %d\n", pix1.HrZone3 );                                   // BYTE
    fprintf(instream, "HrZone4 = %d\n", pix1.HrZone4 );                                   // BYTE
    fprintf(instream, "HrZone5 = %d\n", pix1.HrZone5 );                                   // BYTE

    return;
}                               // dump_pix1

/*************************************************************************************************

*************************************************************************************************/

void RMP::dump_ch(FILE *instream, CourseHeader _ch)  {

    fprintf(instream, "\n");
    const char *cptr;
    cptr = fourcc(_ch.Tag);
    fprintf(instream, "tag = %s\n", cptr);

    fprintf(instream, "Name = %s\n", _ch.Name  );
    fprintf(instream, "FileName = %s\n", _ch.FileName  );
    fprintf(instream, "Description = %s\n", _ch.Description );
    fprintf(instream, "StartAt = %f\n", _ch.StartAt  );                               // double
    fprintf(instream, "EndAt = %f\n", _ch.EndAt  );                                    // double
    fprintf(instream, "Length = %f\n", _ch.Length  );                                  // float
    fprintf(instream, "Laps = %d\n", _ch.Laps  );                                      // int
    fprintf(instream, "Count = %d\n", _ch.Count );                                     // int
    fprintf(instream, "Type = %d\n", _ch.Type );                                       // BYTE
    fprintf(instream, "Attributes = %02x\n", _ch.Attributes  );                          // BYTE
    fprintf(instream, "XUnits = %d\n", _ch.XUnits );                                   // BYTE
    fprintf(instream, "YUnits = %d\n", _ch.YUnits );                                   // BYTE
    fprintf(instream, "OriginalHash = %s\n", _ch.OriginalHash );                       // BYTE
    fprintf(instream, "CourseHash = %s\n", _ch.CourseHash );                           // BYTE
    fprintf(instream, "HeaderHash = %s\n", _ch.HeaderHash );                           // BYTE

    return;
}                   // dump_ch()

#endif

/*************************************************************************************************
	returns the name of the video from the name of the performance file. Eg, 'cdamini' or 'NYCtri'
*************************************************************************************************/

const char *RMP::get_filename_course_name(const char *_fname)  {
	char str[MAX_PATH];
	//char str2[MAX_PATH];
	Pcre *pcre = NULL;
	int start;
	int len;
	int pos;
	int options=0;
	int matches=0;

	if (strstr(_fname, "Ride Alone"))  {
		return NULL;
	}

	//	"2012-06-26@08-49-55_1-John Manley_5_NYCtri_0h00m00.0s.rmp",		"\\dh\\d\\dm\\d\\d\\.\\ds\\.rmp$"

	options = PCRE_CASELESS;

	pcre = new Pcre(_fname, "_[A-z]+_\\dh", NULL, options, false);
	matches = pcre->get_matches();

	if (matches==1)  {
		start = pcre->get_offset(1);							// get first match offset
		len = pcre->get_len(1);									// get first match length
		DEL(pcre);
		memset(str, 0, sizeof(str));
		memcpy(str, &_fname[start], len);						// "_cdamini_0h"
	}
	else  {
		DEL(pcre);
		return NULL;
	}
	
	memset(vidname_from_file, 0, sizeof(vidname_from_file));
	strcpy(vidname_from_file, &str[1]);
	pos = myindex(vidname_from_file, "_");
	if (pos==-1)  {
		return NULL;
	}
	vidname_from_file[pos] = 0;
	//strcat(vidname_from_file, ".avi");

	return vidname_from_file;
}


