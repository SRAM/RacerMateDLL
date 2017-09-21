#ifndef _CRF_H_
#define _CRF_H_

//#include <windows.h>
#include <stdio.h>

#include "comdefs.h"

#ifdef WIN32
//	#include <config.h>
#endif

#define DEBUG_RC4

#define KEYLEN 64
#define SEEDLEN 7

//extern "C" {

//#include <pshpack1.h>
//#pragma pack(push, 1)			// causes structure to be packed

typedef struct {
	unsigned char s[256];
	int i;
	int j;
} TCRF;

//#pragma pack(pop)
//#include <poppack.h>


class X_EXPORT CRF  {			// obfuscated "RC4"
	private:
		TCRF x;
		char seedstr[SEEDLEN];
		CRF(const CRF&);
		CRF &operator = (const CRF&);		// unimplemented
		bool disabled;							// global class flag to disable encryption

	public:

		CRF(bool _disabled=false);
		virtual ~CRF(void);
		void setDis(bool _dis)  {
			disabled = _dis;
		}
		bool getDis(void)  {
			return disabled;
		}
		void init(unsigned char *seed, unsigned char *password);
		void init(void);
		void generate_seedstr(void);
		//void encrypt(unsigned char *buf, int len);
		void doo(unsigned char *buf, int len);
		void unwind(int n);
		//BOOL encrypt_file(char *infile, char *outfile);
		bool do_file(char *infile, char *outfile);			// was encrypt_file
		bool initialized(void);
		void save_encrypt_state(void);
		void restore_encrypt_state(void);
		int geti(void);
		int getj(void);
		void getstate(TCRF *);
		void setstate(TCRF *_x);

		#ifdef MYDBG
		void logg(FILE *_stream);
		#endif
};

//}			// extern "C" {

#endif
