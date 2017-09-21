#ifndef _TESTDATA_H_
#define _TESTDATA_H_

#include <comdefs.h>

#include <rmdefs.h>

#ifdef TESTING

namespace MYQT  {

	class X_EXPORT TESTDATA {

		public:
			TESTDATA(void);
			void reset(void);
			int dgs;											// count of datagrams received
			int len;											// length of last full datagram (all pending)
			int bad_msg;									// bad_msg counter
			int rmcts;										// total number of rmct pkts
			int seqnum;										// last sequence number received ????????????????
			int devnum;										// last devnum received
			int xseqnum;
			int histatus;
		private:
			void init(void);
	};

}

#endif		// #ifdef TESTING

#endif


