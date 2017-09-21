

#ifndef _QT_SS_H_
#define _QT_SS_H_

#include <comdefs.h>

#ifdef _DEBUG
	#include "tmr.h"
#endif

#define DOSMAV
//#define DOBARS2

#ifdef DOSMAV
	#include <smav.h>
#endif

//#include "qt_twopolefilter.h"
#include <twopolefilter.h>
//#include "qt_tdefs.h"

namespace RACERMATE  {

class X_EXPORT qt_SS {
	public:
		#define CUTOFF_RPM 30

		// the data that gets sent upstream

		class X_EXPORT SSD  {
			public:
				//SSD(void);
				SSD(int _nbars);
				~SSD();

				void reset(void);

			private:

				int nbars;

			public:
				float ss;								// instantaneous ss
				float lss;
				float rss;
				float lwatts;
				float rwatts;
				float lata;
				float rata;
				float avgss;
				float avglss;
				float avgrss;

				float *val;
				float *avgval;
				int getNbars() const;
				void setNbars(int value);
		};												// SSD

		//private:

		class X_EXPORT BARINFO  {							// one for each bar

			public:
				BARINFO(int _ix);
				void init(void);
				void reset(void);
				float raw;
				float a;
				float beginning_degrees;
				float center_degrees;

				float beginning_radians;			// for computing lata & rata
				float center_radians;				// for computing lata & rata

				float beginning_sin;
				float beginning_cos;
				float beginning_sin_rotated;		// rotated
				float beginning_cos_rotated;		// rotated

				float center_sin;
				float center_cos;
				float center_sin_rotated;			// rotated
				float center_cos_rotated;			// rotated

				static void setNbars(int _n);

			private:
				static int nbars;
				static double degperseg;			// function of nbars
				int ix;									// 0 to nbars-1

				#ifdef _DEBUG
					//int bp;
				#endif
		};								// BARINFO


	public:
		//SS(int _nbars, bool _metric=false);
		qt_SS(void);
		~qt_SS();

		SSD *ssd;

		float getlata(void) {return ssd->lata;}
		float getrata(void) {return ssd->rata;}
		float getss(void) {return ssd->ss;}
		float getlss(void) {return ssd->lss;}
		float getrss(void) {return ssd->rss;}
		float getlwatts(void) {return ssd->lwatts;}
		float getrwatts(void) {return ssd->rwatts;}

//#ifdef DOSMAV
#if 1
		float getavgss(void) {return ssd->avgss;}
		float getavglss(void) {return ssd->avglss;}
		float getavgrss(void) {return ssd->avgrss;}
#endif
		float *getval()  {return ssd->val;}


		BARINFO *get_barinfo(int i);
		bool get_rescale(void) { return rescale; }
		void start(void);
		void reset(void);
		//void set_degperseg(double _dps);
		float get_max_force(void)  { return maxforce; }
		void set_max_force(float f)  { maxforce = f; }

#ifdef DOBARS2
		inline float *get_x_bars2(void)  { return bars2; }
#endif

		void reset_averages(void);
		void do_ten_hz_filter(void);
		bool getStarted() const;
		void setStarted(bool value);
		bool getFinished() const;
		void setFinished(bool value);
		bool getPaused() const;
		void setPaused(bool value);
		void setraw(int, float);


		SSD *getdata(void);
		bool getRescale() const;

		//bool getMetric() const;
		//void setMetric(bool value);

private:
		//SSD *ssd;
		BARINFO **bi;
		int nbars;
		//bool metric;				// used only for rescaling in newtons vs lbs, defaults to false

		double accum_ss;
		double accum_lss;
		double accum_rss;

		float average_lss;
		float average_rss;
		float average_ss;

#ifdef DOBARS2
		float *bars2;
		double *total_bars2;
		unsigned long *bars2_count;
#endif

		float minforce;
		float maxforce;


		// functions
		void load(double rpm);
		void init(void);
		void set_reset(bool _b)  { reset_flag = _b; }
		void snapshot(char *_fname);
		int get_shift(void) { return SSShift; }
		void set_shift(int _i) { SSShift = _i; }

		bool started;
		bool finished;
		bool paused;

		bool reset_flag;
		bool rescale;
		int SSShift;

		float this_rev_max;

		twoPoleFilter **tpf;
#ifdef DOSMAV
		sMav **smav;
#endif

		unsigned long averageCounter;
		int abovecount, belowcount;

#ifdef _DEBUG
		int bp;
		RACERMATE::Tmr *at;
#endif


};
}


#endif		// #ifndef _X_H_

