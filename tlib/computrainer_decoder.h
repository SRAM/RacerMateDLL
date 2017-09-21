
/****************************************************************************

****************************************************************************/

#ifndef _computrainerDecoder_H_
#define _computrainerDecoder_H_

#ifdef _DEBUG
	//#include <averagetimer.h>
#endif


#include <logger.h>
#include <decoder.h>

#include <course.h>
#include <user.h>
#include <twopolefilter.h>

//#define TDC_DECODER			// use for norm's motion sensor bit setting that replaces the parity bit of the keys byte

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
		class __declspec(dllexport) computrainerDecoder : public Decoder  {
	#else
class computrainerDecoder : public Decoder  {
	#endif
#else
class computrainerDecoder : public Decoder  {
#endif

private:
	#define MINGRADE -10.0f
	#define MAXGRADE 10.0f

	public:
		//#pragma pack(push, 1)						// for old borland compatibility
		typedef struct {
			unsigned long time;
			unsigned char buf[6];
		} RAW_COMPUTRAINER_LOGPACKET;
		//#pragma pack(pop)

	private:
		#ifdef _DEBUG
			int averagedms;
			//AverageTimer *avgtimer;
		#endif

		void runkeys(void);

		#define DT .030295900					// determined empirically from avgtimer->update()

		#define RPM_VALID	0x08
		#define XORVAL 0xff

		DWORD lastms;

		twoPoleFilter **tpf;

		unsigned char newmask[6];
		virtual void peaks_and_averages(void);
		int sscount;
		unsigned char ssbuf[24];
		unsigned char ssraw[24];
		unsigned long lastmphtime;
		float unpack(unsigned short p);
		unsigned char last_keys;
		//void do_ss_filter(void);
		double wattseconds;


	public:
#ifndef NEWUSER
		computrainerDecoder(Course *_course, User *_user=NULL, int _id=0);
#else
		computrainerDecoder(Course *_course, RIDER_DATA &_rd, int _id=0);
#endif
		~computrainerDecoder();

		virtual void set_watts_factor(float _f);
		//virtual void set_weight_lbs(float _pounds);
		//virtual void set_grade(float _grade);
		//virtual void seed_avgs(void);

		virtual int decode(unsigned char *packet, DWORD _ms=0);
		virtual void integrate(void);					// added for 3d software to stop jerking
		virtual void gradeUp(void);
		virtual void gradeDown(void);

		unsigned char ssRaw[24];
		unsigned char ssRawDisplay[24];
		unsigned char hb_DLE;
		bool ssFound;
		unsigned char HB_ERGO_RUN;
		unsigned char HB_ERGO_PAUSE;
		unsigned short heartRateFlags;
		unsigned short minHeartRate;
		unsigned short maxHeartRate;
		unsigned short hbStatus;
		virtual void reset(void);

};

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		}									//extern "C" {
	#endif
#endif

#endif		//#ifndef _computrainerDecoder_H_



