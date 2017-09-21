
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
#endif

#include <assert.h>

#include <utils.h>
//#include <vutils.h>
#include <computrainer_decoder.h>
#include <fatalerror.h>
#include <performance.h>
#include <performance_decoder.h>


/**********************************************************************

**********************************************************************/

Performance::Performance(RIDER_DATA &_rd, bool _metric, const char *_logfname) : dataSource(_rd, NULL, NULL, NO_LOGGING)  {
	dsmetric = _metric;
	course = NULL;

	//strncpy(perfname, _perfname, sizeof(perfname)-1);
	memset(perfname, 0, sizeof(perfname));

	//dump_perf(perfname);

	decoder = new performanceDecoder(course, rd);

	Performance::reset();

	if (course)  {
		decoder->set_total_miles((float)(METERSTOMILES*course->get_total_meters()));	//>cs.total_miles;
	}

	initialized = true;
	bp = 1;

}

/**********************************************************************
	destructor
**********************************************************************/

Performance::~Performance()  {
	DEL(decoder);
	DEL(course);
}

/**********************************************************************
	returns 
		-1 if eof
		1 if it is not time for data
		0 if there is new data at this time

**********************************************************************/

//int Performance::getNextRecord(DWORD _dly)  {
int Performance::getNextRecord(void)  {

//return 1;

	if ( decoder->get_paused() )  {
		return 1;
	}
/*
	if (_dly!=0)  {
		DWORD now = timeGetTime();
		if ((now-lastTime) < _dly)  {
			return 1;
		}
		lastTime = now;
	}
*/

	#ifdef _DEBUG
	//avgTimer->update();		// 39 ms
	//if (decoder->started)  {
	//	bp = 1;
	//}
	#endif

	decoder->decode(NULL);		// just fills meta with dummy data

	if ( decoder->get_finishEdge() )  {
		finish();
	}

	return 1;

}

/**********************************************************************

**********************************************************************/

void Performance::reset(void)  {

	lastTime = 0;

	decoder->reset();		// to reset meta

#ifndef NEWUSER
	decoder->meta.minhr = (unsigned short) (.5 + user->data.lower_hr);
	decoder->meta.maxhr = (unsigned short) (.5 + user->data.upper_hr);
#else
	decoder->meta.minhr = (unsigned short) (.5 + rd.lower_hr);
	decoder->meta.maxhr = (unsigned short) (.5 + rd.upper_hr);
#endif

	decoder->meta.runSwitch = 1;
	connected = true;
	decoder->meta.time = 0;

#ifndef VELOTRON
	decoder->meta.rfmeas = 0x0008;
	decoder->meta.rfdrag = 512;
#endif

	return;
}

/**********************************************************************

**********************************************************************/

void Performance::start(void)  {

	decoder->set_started(true);

	performanceDecoder *pd;
	pd = (performanceDecoder *)decoder;
	pd->lastIntegrateTime = timeGetTime();

	return;
}

/**********************************************************************

**********************************************************************/

void Performance::finish(void)  {
	// let highest app clear the decoder->finishEdge flag
	return;
}

/**********************************************************************

**********************************************************************/
#if 0
void Performance::pause(void)  {
	decoder->set_paused(true);
	return;
}

/**********************************************************************

**********************************************************************/

void Performance::resume(void)  {
	decoder->set_paused(false);
	return;
}
#endif

/**************************************************************************************

**************************************************************************************/

void Performance::set_manual_mph(float _manual_mph)  {
	if (decoder)  {
		decoder->set_manual_mph(_manual_mph);
	}
	return;
}

/**************************************************************************************

**************************************************************************************/

void Performance::set_performance_file_name(const char *_perfname)  {
	int status;
	unsigned long course_offset;

	strncpy(perfname, _perfname, sizeof(perfname)-1);

	status = check_perf(perfname, course_offset);								// checks perf file for integrity

	if (status != 0)  {
		return;
	}

	FILE *stream;
	int i, n;
	COURSE_HEADER ch;
	FPOINT fp;
	std::vector<FPOINT> points;

	stream = fopen(perfname, "rb");

	fseek(stream, course_offset, SEEK_SET);

	n = (int)fread(&ch, sizeof(ch), 1, stream);
	if (n != 1)  {
		fclose(stream);
		return;
	}
	if (ch.sec.type != SEC_COURSE)  {
		fclose(stream);
		return;
	}

	//course_data_len = ch.npoints*sizeof(FPOINT);


	for(i=0; i<(int)ch.sec.n; i++)  {
		status = (int)fread(&fp, sizeof(fp), 1, stream);
		if (status != 1)  {
			fclose(stream);
			return;
		}
		points.push_back(fp);
	}

	course = new Course(points);

	FCLOSE(stream);

	return;
}

/****************************************************************************************
	reference decoder for new perf file format
****************************************************************************************/

int Performance::check_perf(const char *_fname, unsigned long &course_offset)  {
	FILE *stream;
	unsigned long fsize, size;
	SecType sectype;
	int i, n, status;
	SEC tsec;
	std::vector<SEC> secs;
	PERF_FILE_HEADER perf_file_header;
	RESULT result;
	RIDER_INFO rinf;
	COURSE_HEADER ch;
	unsigned long data_len = 0;
	unsigned long course_data_len = 0;
	//int packet_len = 0;
	//computrainerDecoder::RAW_COMPUTRAINER_LOGPACKET lp = {0};
	unsigned long total_len = 0L;
	FPOINT fp;
	
#ifdef _DEBUG
	unsigned long offs2;
	unsigned long offs;
#endif
	
	memset(&tsec, 0, sizeof(tsec));

	fsize = filesize_from_name(_fname);

	stream = fopen(_fname, "rb");
	if (stream==NULL)  {
		return 1;
	}

	i = 0;

	while(1)  {
		tsec.struct_start_offset = ftell(stream);
		n = (int)fread(&sectype, sizeof(unsigned long), 1, stream);
		if (n != 1)  {
			break;
		}
		n = (int)fread(&size, sizeof(unsigned long), 1, stream);
		if (n != 1)  {
			fclose(stream);
			return 2;
		}

		tsec.type = sectype;
		tsec.structsize = size;

		secs.push_back(tsec);

		if (sectype==SEC_PERF_FILE_HEADER)  {
			status = fseek(stream, -8, SEEK_CUR);
			if (status)  {
				fclose(stream);
				return 3;
			}
			n = (int)fread(&perf_file_header, sizeof(perf_file_header), 1, stream);
			if (n != 1)  {
				fclose(stream);
				return 4;
			}
			total_len += sizeof(perf_file_header);
		}
		else if (sectype==SEC_RIDER_INFO)  {
			status = fseek(stream, -8, SEEK_CUR);
			if (status)  {
				fclose(stream);
				return 5;
			}
			n = (int)fread(&rinf, sizeof(rinf), 1, stream);
			if (n != 1)  {
				fclose(stream);
				return 6;
			}
			total_len += sizeof(rinf);
		}
		else if (sectype==SEC_RESULT)  {
			status = fseek(stream, -8, SEEK_CUR);
			if (status)  {
				fclose(stream);
				return 7;
			}
			n = (int)fread(&result, sizeof(result), 1, stream);
			if (n != 1)  {
				fclose(stream);
				return 8;
			}
			total_len += sizeof(result);
		}
		else if (sectype==SEC_COMPUTRAINER_RAW_DATA)  {
#ifdef _DEBUG
			offs = ftell(stream);
#endif

			//packet_len = sizeof(lp);								// 12
			data_len = perf_file_header.sec.n*sizeof(computrainerDecoder::RAW_COMPUTRAINER_LOGPACKET);
#ifdef _DEBUG
			offs = ftell(stream) + data_len;			// where we should end up
#endif
			secs[i].structsize = (data_len + 8);
			total_len += secs[i].structsize;

			/*
			for(i=0; i<(int)perf_file_header.records; i++)  {
				status = fread(&lp, sizeof(lp), 1, stream);
				if (status != 1)  {
					fclose(stream);
					return 9;
				}
			}
			*/
//			fseek(stream, offs, SEEK_SET);
#ifdef _DEBUG
			unsigned long offs2 = ftell(stream);
			if (offs != offs2)  {
				fclose(stream);
				return 10;
			}
#endif
		}
		else if (sectype==SEC_COURSE)  {
			status = fseek(stream, -8, SEEK_CUR);
			if (status)  {
				fclose(stream);
				return 11;
			}
			course_offset = ftell(stream);
			n = (int)fread(&ch, sizeof(ch), 1, stream);
			if (n != 1)  {
				fclose(stream);
				return 12;
			}
			total_len += sizeof(ch);


			course_data_len = ch.sec.n*sizeof(FPOINT);
			total_len += course_data_len;

#ifdef _DEBUG
			offs = ftell(stream) + course_data_len;					// where we should end up
#endif

			for(i=0; i<(int)ch.sec.n; i++)  {
				status = (int)fread(&fp, sizeof(fp), 1, stream);
				if (status != 1)  {
					fclose(stream);
					return 13;
				}
			}

#ifdef _DEBUG
			offs2 = ftell(stream);
			if (offs != offs2)  {
				fclose(stream);
				return 14;
			}
#endif
		}
		else  {
		}

		i++;

	}

	FCLOSE(stream);

	if (total_len != fsize)  {
		return 15;
	}

	return 0;
}											// check_perf()
