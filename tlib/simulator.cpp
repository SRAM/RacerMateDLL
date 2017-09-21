
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
#endif

#include <assert.h>

#include <utils.h>
#include <fatalerror.h>
#include <simulator.h>
#include <simdecoder.h>


/**********************************************************************

**********************************************************************/

Simulator::Simulator(RIDER_DATA &_rd, bool _metric, Course *_course, const char *_logfname) : dataSource(_rd, _course, NULL, NO_LOGGING)  {
	dsmetric = _metric;
	course = _course;
	decoder = new simDecoder(course, rd);

	Simulator::reset();

	if (course)  {
		decoder->set_total_miles((float)(METERSTOMILES*course->get_total_meters()));	//>cs.total_miles;
	}

	initialized = true;
	bp = 1;

}

/**********************************************************************
	destructor
**********************************************************************/

Simulator::~Simulator()  {
	DEL(decoder);
}

/**********************************************************************
	returns 
		-1 if eof
		1 if it is not time for data
		0 if there is new data at this time

**********************************************************************/

//int Simulator::getNextRecord(DWORD _dly)  {
int Simulator::getNextRecord(void)  {

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

void Simulator::reset(void)  {

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

void Simulator::start(void)  {

	decoder->set_started(true);

	simDecoder *sd;
	sd = (simDecoder *)decoder;
	sd->lastIntegrateTime = timeGetTime();

	return;
}

/**********************************************************************

**********************************************************************/

void Simulator::finish(void)  {
	// let highest app clear the decoder->finishEdge flag
	return;
}

/**********************************************************************

**********************************************************************/
#if 0
void Simulator::pause(void)  {
	decoder->set_paused(true);
	return;
}

/**********************************************************************

**********************************************************************/

void Simulator::resume(void)  {
	decoder->set_paused(false);
	return;
}
#endif

/**************************************************************************************

**************************************************************************************/

void Simulator::set_manual_mph(float _manual_mph)  {
	if (decoder)  {
		decoder->set_manual_mph(_manual_mph);
	}
	return;
}

