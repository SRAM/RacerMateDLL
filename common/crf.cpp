
#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
	#define STRICT
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef QT_CORE_LIB
	#include <Windows.h>
#endif


#include <iostream>
#include <stdio.h>
#include <stack>				// using stl stack

using namespace std;

#include "crf.h"

#ifdef WIN32
//	#include <err.h>
#else
	#include <memory.h>
#endif

//extern "C" {

stack<TCRF> rc4stack;


/**********************************************************************

  encryption parameters when creating .srf & .rce files

calling write_user_data

   i =    0, j =    0,     23
   i =  208, j =   38,    231

calling write_header

   before writing course header: i =  208, j =   38, offset =    231
   after writing course header:  i =  228, j =  173, offset =    763
   after writing course data:    i =   68, j =   86, offset =    859

encryption parameters when reading .srf & .rce files

**********************************************************************/


/**********************************************************************
	constructor for rc4 class
**********************************************************************/

CRF::CRF(bool _disabled)  {
//	init();
	disabled = _disabled;
	return;
}


/**********************************************************************
	Destructor for rc4 class
**********************************************************************/

CRF::~CRF()  {

}


/**********************************************************************

s[256] = linear
k[256] = seed, password

j = 0
for(i=0;i<256)  {
	j = (j + s[i]) + k[i]) % 256
	swap s[i] & s[j]
}

**********************************************************************/

void CRF::init(unsigned char *seed, unsigned char *password)  {
	int i,j,n;
	unsigned char k[256];
	unsigned char ctemp;

	if (seed[0] == 0)  {
		generate_seedstr();
		memcpy(seed, seedstr, 7);
	}

	//--------------------
	// load s linearly:
	//--------------------

	for(i=0;i<256;i++)  {
		x.s[i] = (unsigned char)i;
	}

	//----------------------------------------------------------
	// load k with SEEDLEN byte random string and then the password:
	//----------------------------------------------------------

	j = 0;
	n = (int)strlen((char *)password);

	for(i=0;i<SEEDLEN;i++) k[i] = seed[i];

	for(i=SEEDLEN;i<256;i++)  {
		k[i] = password[j];
		j = (j+1)%n;
	}

	//-------------
	// finish up:
	//-------------

	j = 0;

	for(i=0;i<256;i++)  {
		j = (j + x.s[i] + k[i]) % 256;
		ctemp = x.s[i];
		x.s[i] = x.s[j];
		x.s[j] = ctemp;
	}


	x.i = x.j = 0;

	/*********************
	// now run the table for 9977 cycles:

	unsigned char c;

	c = 0xaa;

	for(i=0;i<9977;i++)  {
		encrypt(&c, 1);
	}
	*********************/


	return;
}


/**********************************************************************

**********************************************************************/

void CRF::doo(unsigned char *buf, int len)  {
	int k,t;
	unsigned char ctemp, b;


	if (disabled)  {
		return;
	}

	for(k=0; k<len; k++)  {
		x.i = (x.i+1) % 256;						// 11
		x.j = (x.j + x.s[x.i]) % 256;				// 144

		ctemp = x.s[x.i];							// 0
		x.s[x.i] = x.s[x.j];						// 202
		x.s[x.j] = ctemp;							// 0

		t = (x.s[x.i] + x.s[x.j]) % 256;			// 202
		b = x.s[t];									// 22
		buf[k] = (unsigned char) (buf[k] ^ b);		// 'a'
	}

	return;
}


/**********************************************************************

**********************************************************************/

void CRF::unwind(int n)  {
	unsigned char ctemp;
	int k;

	for(k=0;k<n;k++)  {
		ctemp = x.s[x.j];
		x.s[x.j] = x.s[x.i];
		x.s[x.i] = ctemp;
		x.j = (x.j - x.s[x.i]) & 0xff;
		x.i = (x.i-1) & 0xff;
	}

	return;
}

/**********************************************************************

**********************************************************************/

bool CRF::do_file(char *infile, char *outfile)  {

	FILE *instream, *outstream;
	int status;
	unsigned char c;

	instream = fopen(infile,"rb");
	if (instream==NULL)  {
		//err(__FILE__,__LINE__);
		//logg.write(10,0,1,"Error in %s at %d\n", __FILE__, __LINE__);
		return false;
	}

	outstream = fopen(outfile,"wb");
	if (outstream==NULL)  {
		fclose(instream);
		//err(__FILE__,__LINE__);
		//logg.write(10,0,1,"Error in %s at %d\n", __FILE__, __LINE__);
		return false;
	}


	while(1)  {
		status = fgetc(instream);
		if (status==EOF)  {
			break;
		}
		c = (unsigned char) status;
		doo(&c, 1);
		fputc(c,outstream);
	}

	fclose(instream);
	fclose(outstream);

	return true;
}


/**********************************************************************

**********************************************************************/

bool CRF::initialized(void)  {

	if ((x.i==0) && (x.j==0))  {
		return true;
	}

	return false;
}

/**********************************************************************

**********************************************************************/

void CRF::save_encrypt_state(void)  {

	rc4stack.push(x);

	return;
}

/**********************************************************************

**********************************************************************/

void CRF::restore_encrypt_state(void)  {

	x = rc4stack.top();
	rc4stack.pop();

	return;
}

/**********************************************************************

**********************************************************************/

int CRF::geti(void)  {
	return x.i;
}

/**********************************************************************

**********************************************************************/

int CRF::getj(void)  {
	return x.j;
}

/**********************************************************************

	keep this one for compatibility only. Use the other one for new code.


	Perhaps the simplest cipher to understand and implement, in my opinion,
	is RC4.  It was not designed as a one-way hash, but it could be used as
	one.
	Given key k with length KEYLEN bytes, initialize a 256-byte block s by
	filling it linearly from 0 to 255.  Scramble it with the following:
**********************************************************************/

void CRF::init(void)  {
	const unsigned char key1[KEYLEN/2] =  {
		0x91, 0xcb, 0x2e, 0x85, 0x14, 0x21, 0xb2, 0x83,
		0xf2, 0x10, 0x18, 0x89, 0xfb, 0x71, 0xe2, 0xf8,
		0xd0, 0x5c, 0x69, 0x3e, 0x30, 0xa6, 0x39, 0xb1,
		0x6b, 0x5e, 0xc0, 0x94, 0xab, 0x8d, 0x33, 0xae
	};
	unsigned char ctemp;
	int m,n;
	unsigned char key[64] = {									// INIT WITH SOME GARBAGE
		0x88, 0x66, 0x84, 0x8f, 0xdb, 0x76, 0x7a, 0x2b,
		0x57, 0xa7, 0x4e, 0x9b, 0xbd, 0xda, 0x4c, 0xe1,
		0x17, 0x9c, 0x97, 0x44, 0x58, 0xa4, 0x03, 0x93,
		0x07, 0x6f, 0x54, 0x12, 0x2c, 0x45, 0x68, 0xc8,
		0xd0, 0x5c, 0x69, 0x3e, 0x30, 0xa6, 0x39, 0xb1,
		0x6b, 0x5e, 0xc0, 0x94, 0xab, 0x8d, 0x33, 0xae,
		0xf2, 0x10, 0x18, 0x89, 0xfb, 0x71, 0xe2, 0xf8,
		0x91, 0xcb, 0x2e, 0x85, 0x14, 0x21, 0xb2, 0x83,
	};

	const unsigned char key2[KEYLEN/2] =  {
		0x57, 0xa7, 0x4e, 0x9b, 0xbd, 0xda, 0x4c, 0xe1,
		0x88, 0x66, 0x84, 0x8f, 0xdb, 0x76, 0x7a, 0x2b,
		0x07, 0x6f, 0x54, 0x12, 0x2c, 0x45, 0x68, 0xc8,
		0x17, 0x9c, 0x97, 0x44, 0x58, 0xa4, 0x03, 0x93
	};


	for(m=0;m<256;m++)  {
		x.s[m] = (unsigned char)m;
	}

	//--------------------------------------------------------------
	// construct key[64] out of key1[32] and key2[32].
	// The two halves are separated to make hacking more difficult.
	//--------------------------------------------------------------

	for(m=0;m<32;m++)  {
		key[m] = key1[m];
		key[m+32] = key2[m];
	}


	n = 0;

	for (m=0;m<256;m++)  {
		n = (n + x.s[m] + key[m%KEYLEN]) % 256;	// bc error
		ctemp = x.s[m];			// bc error
		x.s[m] = x.s[n];
		x.s[n] = ctemp;			// bc error
	}

	x.i = x.j = 0;

	return;
}


/**************************************************************************************

**************************************************************************************/

void CRF::generate_seedstr(void)  {

#ifdef QT_CORE_LIB
	//seed = 0;
#else
	#ifdef WIN32
		int i;
		BOOL bstatus;
		LARGE_INTEGER performanceCount;
		unsigned int seed;

		bstatus = QueryPerformanceCounter(&performanceCount);

		if (bstatus)  {
			seed = performanceCount.u.LowPart & 0x0000ffff;
		}
		else  {
			srand(9829);
			seed = rand();
		}

		srand(seed);



		for(i=0;i<SEEDLEN;i++)  {
			seedstr[i] = (unsigned char) (rand() % 256);
		}
	#endif
#endif

	return;
}

/**************************************************************************************

**************************************************************************************/

void CRF::getstate(TCRF *_x)  {

	memcpy(_x, &x, sizeof(TCRF));
	return;

}

/**************************************************************************************

**************************************************************************************/

void CRF::setstate(TCRF *_x)  {
	memcpy(&x, _x, sizeof(x));
	return;
}



#ifdef MYDBG
	void CRF::logg(FILE *_stream)  {
		fprintf(_stream, "i = %3d, j = %3d\n", x.i, x.j);
		return;
	}
#endif		// _DEBUG

//}				// extern "C" {
