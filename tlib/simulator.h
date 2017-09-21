
#ifndef _Simulator_H_
#define _Simulator_H_


//#include <windows.h>

//#include <config.h>

#include <datasource.h>
#ifndef NEWUSER
	#include <user.h>
#else
#endif

#include <course.h>

/**********************************************************************************************
	class to simulate realtime data. The data is generated randomly. This class uses
	simDecoder to actually generate the data.
**********************************************************************************************/

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
class __declspec(dllexport) Simulator : public dataSource  {
	#else
class Simulator : public dataSource  {
	#endif
#else
class Simulator : public dataSource  {
#endif



	private:
		DWORD lastTime;
		float manual_mph;

#ifndef NEWUSER
		User *user;
#endif

	public:
#ifndef NEWUSER
		Simulator(User *_user, bool _metric, Course *_course, const char *_logfname="ds.log");
#else
		Simulator(RIDER_DATA &_rd, bool _metric, Course *_course, const char *_logfname="ds.log");
#endif
		~Simulator(void);
		virtual int getNextRecord(void);
		virtual void reset();
		virtual void start(void);
		virtual void finish(void);
		virtual void set_manual_mph(float _manual_mph);

		/*
		virtual void set_draft_wind(float _kph)  {
			decoder->draft_wind = _kph;
			return; 
		}

		//virtual void pause(void);
		//virtual void resume(void);

		virtual float get_draft_wind(void)  {
			return (float)decoder->draft_wind;
		}
		*/

};

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		}									//extern "C" {
	#endif
#endif

#endif		//#ifndef _Simulator_H_


