
#ifndef _INTEL_H_
#define _INTEL_H_

#include <stdio.h>

class INTEL  {

	public:
		INTEL(void);
		virtual ~INTEL(void);
		void makebin(const char *_infname);
		void makehex(const char *_infname, unsigned long _addr);
		void loadhex(const char *_infname, const char *_outfname=NULL);
		int makehex(unsigned char *buf, int size, unsigned long addr, const char *outfilename);
		unsigned long get_blockmask(void)  { return blockmask; }
		int get_bin_size(void)  { return binsize; }

	private:
		enum  {
			DATR,				// 00 - Data Record (8-, 16-, or 32-bit formats)
			EOFR,				// 01 - End of File Record (8-, 16-, or 32-bit formats)
			ESAR,				// 02 - Extended Segment Address Record (16- or 32-bit formats)
			SSAR,				// 03 - Start Segment Address Record (16- or 32-bit formats)
			ELAR,				// 04 - Extended Linear Address Record (32-bit format only)
			SLAR				// 05 - Start Linear Address Record (32-bit format only)
		};

		#ifdef _DEBUG
			int bp;
		#endif

		unsigned long gethexnum(char *buf, int offset, int len);

		/*
		FILE *instream,*outstream;
		unsigned char buf[8];
		char str[256];
		char inbuf[128];
		unsigned char outbuf[128];
		char infname[256];
		char outfname[256];
		unsigned short special_checksum,checksum;
		unsigned long address;
		unsigned long addr;
		char line[128];
		unsigned long gethexnum(char *buf, int offset, int len);
		*/

		unsigned long blockmask;
		int binsize;
};


#endif		// #ifndef _INTEL_H_

