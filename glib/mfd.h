
#ifndef _MFD_H_
#define _MFD_H_

#ifdef WIN32
	//#pragma warning(disable:4786)
	#include <windows.h>
#endif


#include <stdio.h>
#include <datasource.h>
//#include <fasttimer.h>


class MFD : public dataSource {

	friend class scrollingChart;

	private:
		void destruct(void);
		long nextrec;
		int lastdir;
		__int64 now1, freq1;
		//fastTimer *t1;
		//double s1;

	protected:
		int dir;			// 0 = forward, 1 = reverse, -1 = not selected
		DWORD lastnow;
		FILE *stream;
		char tmpfilename[256];		//tlm20040817
		char fname[256];
		unsigned long fileStartTime;
		unsigned long fileEndTime;
		float *array;
		bool enc;						// encryption flag

	public:
#ifdef WIN32
		MFD(char *stub, HWND _phwnd=NULL, bool _enc=true);
#else
		MFD(char *stub, bool _enc=true);
#endif

		virtual ~MFD(void);
		int get_nRecords(void);

		int seek(long rec);
		int getPreviousRecord(DWORD _delay);
		int read(int k);
		int getNextRecord(DWORD _delay);
		int getRecord(int k);
		long getOffset(void);
		unsigned long getFileEndTime(void)  {
			return fileEndTime;
		}

		virtual void reset(void);
		virtual int myexport(char *_fname);
		virtual void pause(void) {	return;}
		virtual void start(void)  {
			//t1->reset();
			return; 
		}

};

#endif			// #ifndef _MFD_H_


