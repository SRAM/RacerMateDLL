
#ifndef _PERFORMANCE_H_
#define _PERFORMANCE_H_

#include <datasource.h>
#include <course.h>

/**********************************************************************************************

**********************************************************************************************/

class Performance : public dataSource  {

	private:
		DWORD lastTime;
		float manual_mph;
		char perfname[MAX_PATH];

	public:
		Performance(RIDER_DATA &_rd, bool _metric, const char *_logfname="ds.log");
		~Performance(void);
		virtual int getNextRecord(void);
		virtual void reset();
		virtual void start(void);
		virtual void finish(void);
		virtual void set_manual_mph(float _manual_mph);
		virtual void set_performance_file_name(const char *_perfname);
		int check_perf(const char *_fname, unsigned long &course_offset);

};

#endif		//#ifndef _Performance_H_


