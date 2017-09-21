
#ifndef _PERFORMANCE_DECODER_H_
#define _PERFORMANCE_DECODER_H_

/****************************************************************************

****************************************************************************/

#include <lpfilter.h>

#include <course.h>
#include <decoder.h>

class performanceDecoder : public Decoder  {
	friend class Performance;
	friend class Rider2;

	public:

	private:
		int instance;
		bool manual;
		float manual_mph;

		double miles;
		DWORD startTime;
		DWORD perfTime;

		int mode;
		DWORD total_ms;
		float min_mph;
		float max_mph;

		float min_watts;
		float max_watts;

		float min_hr;
		float max_hr;

		float min_rpm;
		float max_rpm;

		DWORD lastIntegrateTime;
		DWORD lastnow;
		bool firststart;

		void integrate(void);
		int lastGrade_i;

		LPFilter *mphFilter;
		LPFilter *wattsFilter;
		LPFilter *rpmFilter;
		LPFilter *hrFilter;

	public:
		performanceDecoder(Course *_course, RIDER_DATA &_rd);
		~performanceDecoder();
		virtual int decode(unsigned char *packet, DWORD _ms=0);
		virtual void reset(void);

};

#endif		// #ifndef _PERFORMANCE_DECODER_H_

