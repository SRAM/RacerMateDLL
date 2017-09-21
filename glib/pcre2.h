#ifndef _Pcre_H_
#define _Pcre_H_

#include <stdio.h>

#include "../pcre7.0/include/regex.h"				// linux has many regex.h's
//#include "../pcre-8.36/pcrecpp.h"				// linux has many regex.h's

class Pcre {

	private:
#ifdef _DEBUG
		int bp;
#endif
		FILE *stream;
		pcre *re;
		pcre_extra *extra;

		#if !defined NOPOSIX					// There are still compilers that require no indent
			//regex_t preg;
			int do_posix;
		#endif

		#define OVECCOUNT 30			// should be a multiple of 3
		int options;
		const char *subject;
		const char *pat;
		//FILE *stream;
		int erroffset;
		const char *error;
		int ovector[OVECCOUNT];
		int sublen;		// status, i;
		int namecount;
		unsigned char *name_table;
		int name_entry_size;
		//int k;
		bool find_all;
		int matches;
		void init(void);

	public:
		Pcre(const char *_subject, const char *_pat, FILE *_stream, int _options=0, bool _find_all=false );
		~Pcre();
		inline int get_matches(void)  { return matches; }
		int get_offset(int _match);							// _match counts from 1
		int get_len(int _match);							// _match counts from 1

};

#endif


