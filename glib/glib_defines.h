
#ifndef _GLIB_DEFINES_H_
#define _GLIB_DEFINES_H_

#include <string>

//#define FLASHLIGHT
#define NEWANT

#ifdef MEMTEST
	xxxxxxxx
#else
	//#define MEMTEST			// used to find memory leak
#endif

#define DOGLOBS

	/*
enum TRAINER_COMMUNICATION_TYPE {
	BAD_INPUT_TYPE = -1,
	WIN_RS232,
	UNIX_RS232,
	TRAINER_IS_SERVER,									// trainer is a server
	TRAINER_IS_CLIENT									// trainer is a client
};
	*/

/*
typedef struct {
	//std::string name;
	//char comm_name[256];								// had to add for RM1
	//int namelen;									// had to add for RM1
	TRAINER_COMMUNICATION_TYPE type;
	int portnum;								// only for windows, 1-256, -1 for linux/mac
} PORT_INFO;
*/

//#define DO_PI

#ifdef DO_PI
typedef struct {
	std::string name;
	TRAINER_COMMUNICATION_TYPE type;
	int portnum;								// only for windows, 1-256, -1 for linux/mac
} PORT_INFO;
#endif


// THIS HAS TO BE IN SAME ORDER AS THE STRINGS ARE PUSHED ON the dirs vector:
enum DIRTYPE {
	DIR_PROGRAM,                    // 0
	DIR_PERSONAL,
	DIR_SETTINGS,
	DIR_REPORT_TEMPLATES,
	DIR_REPORTS,
	DIR_COURSES,
	DIR_PERFORMANCES,
	DIR_DEBUG,
	DIR_HELP,                       // 8
	NDIRS                           // 9
};


typedef struct  {
	 double x;
	 double y;
	 double z;
} DPOINT;

typedef struct  {
	 DPOINT pt[3];
} TRIANGLE;

typedef struct  {
	 DPOINT pt[2];
} EDGE;

#ifdef WIN32

	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
		#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
		#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif

	struct timezone   {
		int  tz_minuteswest; /* minutes W of Greenwich */
		int  tz_dsttime;     /* type of dst correction */
	};



	#define FILESEPCHAR 0x5c
	#define FILESEPSTR "\\"
	//#define null NULL
	//#define null 0
	#define DISABLE_TOOLBAR_BUTTON(button) { SendMessage(toolbar, TB_SETSTATE, button, MAKELONG(TBSTATE_INDETERMINATE, 0)); }
	#define ENABLE_TOOLBAR_BUTTON(button) { SendMessage(toolbar, TB_SETSTATE, button, MAKELONG(TBSTATE_ENABLED, 0)); }

	//#define png_byte unsigned char

	template <class typ> inline typ MIN(typ a, typ b) {
	  if(a < b)
		 return a;
	  return b;
	}

	template <class Type> inline Type MAX(Type a, Type b) {
	  if(a > b)
		 return a;
	  return b;
	}



#else								// linux defines
	#include <stdio.h>
	#include <unistd.h>		// usleep
	#include <stdint.h>		// int64_t
	#define FILESEPCHAR '/'
	#define FILESEPSTR "/"
	#ifdef _DEBUG
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	#endif


	#define TEXT(f) (char *)((f))

//#define _DEBUG

	#define Sleep(x) usleep((useconds_t)((float)x*1000.0f))

	#define	OutputDebugString(str) printf("%s", str)

	#define ESC			27
	#define ESCAPE		27

	#define _MAX_PATH 256
	#define MAX_PATH 256
	#define MAX_FILE 256
	#define _MAX_FILE 256
	//#define MAX_FNAME 256
	#define _MAX_FNAME 256
	//#define MAX_EXT 32
	#define _MAX_EXT 32

	#ifndef NULL
	#define NULL 0
	#endif

	//#define FLT_MAX 1000000.0f	include <float.h> instead

	/*
	#define WORD unsigned short
	typedef struct _SYSTEMTIME {
	  WORD wYear;
	  WORD wMonth;
	  WORD wDayOfWeek;
	  WORD wDay;
	  WORD wHour;
	  WORD wMinute;
	  WORD wSecond;
	  WORD wMilliseconds;
	} SYSTEMTIME, *PSYSTEMTIME;
	*/

	typedef struct {
		int top;
		int bottom;
		int left;
		int right;
	} RECT;

	typedef struct {
		int cx;
		int cy;
	} SIZE;


	#define WORD unsigned short

	typedef struct _SYSTEMTIME {
		 WORD wYear;
		 WORD wMonth;
		 WORD wDayOfWeek;
		 WORD wDay;
		 WORD wHour;
		 WORD wMinute;
		 WORD wSecond;
		 WORD wMilliseconds;
	} SYSTEMTIME, *PSYSTEMTIME;


	//#define MB_OK 1
	//#define MB_ICONEXCLAMATION 2

	typedef struct {
		int x;
		int y;
	} POINT;


	typedef struct D3DXVECTOR3 {
		float x;
		float y;
		float z;
	} D3DXVECTOR3, *LPD3DXVECTOR3;

	#ifndef FALSE
		#define  FALSE 0
	#endif

	#ifndef TRUE
		#define  TRUE 1
	#endif

//	typedef bool BOOL;
	#define BOOL bool

	typedef unsigned long DWORD;
	typedef uint32_t UINT32;
	typedef int32_t INT32;
	typedef unsigned char BYTE;
	typedef unsigned char byte;
	typedef int16_t INT16;
	typedef uint16_t UINT16;
	typedef int64_t INT64;
	typedef uint64_t UINT64;

	//typedef int64_t __int64;
//	#define __int64		int64_t
//	#define _int64			int64_t


	//#ifndef _MAX_PATH								// for resource files conflict
	//	#ifdef GNU									// tlm20060627
	//		#define _MAX_PATH 256					// for linux, commented out 20060126
	//	#endif
	//#endif

	#define IDNO								1
	#define IDYES								1

	#define MB_OK                       0x00000000L
	#define MB_OKCANCEL                 0x00000001L
	#define MB_ABORTRETRYIGNORE         0x00000002L
	#define MB_YESNOCANCEL              0x00000003L
	#define MB_YESNO                    0x00000004L
	#define MB_RETRYCANCEL              0x00000005L


	#define MB_ICONHAND                 0x00000010L
	#define MB_ICONQUESTION             0x00000020L
	#define MB_ICONEXCLAMATION          0x00000030L
	#define MB_ICONASTERISK             0x00000040L

	#define MB_USERICON                 0x00000080L
	//#define MB_ICONWARNING              MB_ICONEXCLAMATION
	#define MB_ICONERROR                MB_ICONHAND

	#define MB_ICONINFORMATION          MB_ICONASTERISK
	#define MB_ICONSTOP                 MB_ICONHAND

	#define MB_DEFBUTTON1               0x00000000L
	#define MB_DEFBUTTON2               0x00000100L
	#define MB_DEFBUTTON3               0x00000200L
	#define MB_DEFBUTTON4               0x00000300L

	#define MB_APPLMODAL                0x00000000L
	#define MB_SYSTEMMODAL              0x00001000L
	#define MB_TASKMODAL                0x00002000L
	#define MB_HELP                     0x00004000L // Help Button

	#define MB_NOFOCUS                  0x00008000L
	#define MB_SETFOREGROUND            0x00010000L
	#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L

	#define MB_TOPMOST                  0x00040000L
	#define MB_RIGHT                    0x00080000L
	#define MB_RTLREADING               0x00100000L

	#define UINT unsigned short
	#define LPCSTR const char *
	#define LPVOID void *
	#define LPCTSTR		const char *
	//#ifndef BYTE
	//	#define BYTE unsigned char							// conflicts with xmd.h !!!!
	//#endif


	typedef struct _SECURITY_ATTRIBUTES {
		/*
		DWORD  nLength;
		LPVOID lpSecurityDescriptor;
		BOOL   bInheritHandle;
		*/
		int dummy;
	} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

	#define COLORREF	DWORD			// from    http://subversion.gvsig.org/gvSIG/trunk/libraries/libjni-readecw-linux/include/NCSDefs.h
	typedef DWORD *LPCOLORREF;

	COLORREF RGB(
		unsigned char r,		// red component of color			least sig byte
		unsigned char g,		// green component of color
		unsigned char b,		// blue component of color
		unsigned char a		//											most sig byte
	);
	// from    http://subversion.gvsig.org/gvSIG/trunk/libraries/libjni-readecw-linux/include/NCSDefs.h  :
	#define RGB(r,g,b) ((COLORREF)(((unsigned char)(r)|((WORD)((unsigned char)(g))<<8))|(((DWORD)(unsigned char)(b))<<16)))

	//#define Sleep(x) usleep((useconds_t)((float)x*1000.0f))
	//#define GetRValue(x) ((x>>24) & 0x000000ff)
	//#define GetGValue(x) ((x>>16) & 0x000000ff)
	//#define GetBValue(x) ((x>>8) & 0x000000ff)

	#define GetRValue(x) (x & 0x000000ff)
	#define GetGValue(x) ((x>>8) & 0x000000ff)
	#define GetBValue(x) ((x>>16) & 0x000000ff)
#endif			// #ifndef WIN32


	/*
	typedef  struct  {							// defined in gentypes.h
		float x;
		float y;
	} FPOINT;
	*/

	/*
	typedef struct  {							// moved to gentypes.h
		float top;
		float bottom;
		float left;
		float right;
	} FRECT;
	*/

#define MAX_FNAME 256
#define MAX_EXT 32
//#define _MAX_FNAME 256

#define DLLExport   __declspec( dllexport )

//Detect if there is a zero byte in a 32bit word:
#define has_null_byte(x) (((x) - 0x01010101) & ~(x) & 0x80808080)


// determines if a number is a power of 2:
#define ISPOW2_A(x)  (x && !(x & (x-1)))
#define ISPOW2_B(n)  (!( n & (n-1))
#define ISPOW2_C(x)  ((~(~0U >> 1) | x) & x - 1)



#define DEL(p)  { if(p) { delete (p);     (p)=NULL; } }
#define FCLOSE(stream)  { if((stream)!=NULL) { fclose(stream); (stream)=NULL; } }
#define DELARR(p)  { if(p) { delete [] (p); (p)=NULL;} }
#define TOGGLE(b)  { if(b)  {b = false;} else {b = true; } }

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }


#ifndef WIN32
	 //#define FAILED(p) { if (p&0x80000000) return true; return false; }
	 //#define FAILED(p) { (int)(p < 0)  }
	 //#define FAILED(p) { (p) & 0x80000000  }
	 #define FAILED(p) ((int)(p)<0)
#endif

//#define PI	3.14159265358979323846f
#ifndef PI
	//#ifdef QT
		#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164
	//#endif

	#define TWOPI (2.0*PI)
#endif

//#define ROUND(f)  { (f) = (int)(.5 + (f));  }
//#define ROUND(f)  (int)(.5 + (f));
#define ROUND(f) (int)((f) + ((f) >= 0 ? 0.5 : -0.5))


// returns the linear interpolated value b. All values are floats.
// a1, a, a2 is the interval we are interpolating onto the [b1,b2] interval.
// first used in the video project (movie7).
#define FINTERP(a1, a, a2, b1, b2)  ( b1 + ((a - a1) * (b2-b1)) / (a2-a1) )
//#define ULINTERP(a1, a, a2, b1, b2)  ( b1 + ((a - a1) * (b2-b1)) / (a2-a1) )
//#define FINTERP(a1, a, a2, b1, b2)  { if(a2==a1) return 0.0f; b1 + ((a - a1) * (b2-b1)) / (a2-a1) }

/*
A meter is the Length traveled by light in vacuum during 1/299 792 458 of a second.
1 inch = 25.4 millimeters								exactly
1 foot = 12*25.4 mm = 304.8 mm = .3048 meter		exactly
1 meter = 1.0 / .3048 feet								as exact as possible
1 KM = 1000.0 / .3048 feet								as exact as possible
1 KM = 1000.0 / (.3048 * 5280.0)	miles				as exact as possible

  1000/1609.344

*/

//-------------
// length:
//-------------

#define FEETTOMETERS		.3048										// exact definition
#define INCHESTOMETERS	(FEETTOMETERS/12.0)					// .3048/12 = .0254

//#define FEET_TO_METERS	FEETTOMETERS						// conflicts with 3d definition
#define METERSTOFEET		(1.0 / FEETTOMETERS)					// as "exact" as it can be
//#define METERS_TO_FEET	METERSTOFEET						// conflicts with 3d definition
#define METERSTOINCHES		(12.0*(1.0 / FEETTOMETERS))	// as "exact" as it can be

#define CMTOINCHES		(12.0 * METERSTOFEET / 100.0)		// .393701 (= 1/2.54)
#define CMTOFEET			(METERSTOFEET / 100.0)				// .393701 (= 1/2.54)
#define MMTOINCHES		(12.0 * METERSTOFEET / 1000.0)	// .393701 (= 1/2.54)
#define INCHES_TO_CM		(1.0 / CMTOINCHES)
#define KMTOMILES			(1000.0 / (.3048 * 5280.0))		// approx .62137
#define TOMILES			KMTOMILES

#define MILESTOKM			((.3048 * 5280.0) / 1000.0)		// approx 1.60935
#define TOKM				MILESTOKM
#define MILESTOMETERS	(1000.0 * MILESTOKM)
#define MILESTOFEET		5280.0

#define KMTOFEET			(KMTOMILES*5280.0)					// 3280.8336
#define METERSTOMILES	(KMTOMILES / 1000.0)					// .0006214

/*
	mathematica version:

FEETTOMETERS = .3048;
INCHESTOMETERS	= FEETTOMETERS/12.0;
METERSTOFEET = 1.0 / FEETTOMETERS;
METERSTOINCHES = 12.0*(1.0 / FEETTOMETERS);
CMTOINCHES = 12.0 * METERSTOFEET / 100.0;
INCHESTOCM = 1.0 / CMTOINCHES;
KMTOMILES = 1000.0 / (.3048 * 5280.0);
TOMILES = KMTOMILES;
MILESTOKM = (.3048 * 5280.0) / 1000.0;
TOKM = MILESTOKM;
MILESTOMETERS = 1000.0 * MILESTOKM;
MILESTOFEET = 5280.0;
KMTOFEET = KMTOMILES*5280.0;
METERSTOMILES = KMTOMILES / 1000.0;

*/

//----------
// mass
//----------

#define KGTOLBS 2.2046
#define TOPOUNDS 2.2046
#define KGSTOLBS 2.2046
#define KGSTOPOUNDS 2.2046
#define TOKGS (1.0 / KGTOLBS)
#define POUNDSTOKGS (1.0 / KGTOLBS)
#define POUNDSTOSLUGS 0.03108			// 1 lb / 32.2 fps/s

/*
	mathematica version:

KGTOLBS = 2.2046
TOPOUNDS = 2.2046
KGSTOLBS = 2.2046
KGSTOPOUNDS = 2.2046
TOKGS = 1.0 / KGTOLBS;
POUNDSTOKGS = 1.0 / KGTOLBS;
POUNDSTOSLUGS = 0.03108
*/

//-----------
// force
//-----------

#define POUNDSTONEWTONS		4.44824
#define NEWTONSTOPOUNDS		(1.0/POUNDSTONEWTONS)

//-----------
// pressure
//-----------

#define MBAR_TO_PSI .014504					// millibar to psi
#define MBAR_TO_INHG 0.02953				// millibar to inches of mercury

//--------------------------------------------------------------
// acceleration due to gravity conversion of Kgs to Newtons
//--------------------------------------------------------------

//#define KGSTONEWTONS 9.8
#define KGSTONEWTONS		9.80665	// http://www.chemie.fu-berlin.de/chemistry/general/constants_en.html
#define  GMETRIC			9.80665	// http://www.chemie.fu-berlin.de/chemistry/general/constants_en.html

//----------
// torque
//----------

#define NMTOFP (NEWTONSTOPOUNDS*METERSTOFEET)							// NEWTON-METERS TO FOOT-POUNDS .737559
#define FPTONM (1.0/NMTOFP)

//---------------------
// speed conversions
//---------------------

#define KPH_TO_METERS_PER_SEC	(1000.0 / 3600.0)			// kilometers per hour --> meters per second
#define KPHTOMPS					(1000.0 / 3600.0)			// kilometers per hour --> meters per second
#define MPH_TO_METERS_PER_SEC (KPH_TO_METERS_PER_SEC * MPHTOKPH)
#define MPH_TO_FPS (5280.0 / 3600.0)
#define FPS_TO_MPH (3600.0 / 5280.0)
#define MPSTOKPH (3600.0 / 1000.0)							// meters per second --> kilometers per hour
#define MPS_TO_KPH (3600.0 / 1000.0)						// meters per second --> kilometers per hour
#define KPHTOMPH			KMTOMILES
#define TOMPH				KMTOMILES
#define TOKPH				MILESTOKM
#define MPHTOKPH			MILESTOKM
#define MPHTOMPS			( MPHTOKPH * 1000.0 / 3600.0 )
#define MPSTOMPH (3600 / (.3048 * 5280.0) )

/*
	mathematica version:

KPHTOMETERSPERSEC = 1000.0 / 3600.0;
KPHTOMPS = 1000.0 / 3600.0;
MPHTOMETERSPERSEC = (KPHTOMETERSPERSEC * MPHTOKPH);
MPHTOFPS = 5280.0 / 3600.0;
FPSTOMPH = 3600.0 / 5280.0;
MPSTOKPH = 3600.0 / 1000.0;
KPHTOMPH = KMTOMILES;
TOMPH = KMTOMILES;
TOKPH = MILESTOKM;
MPHTOKPH = MILESTOKM;
MPHTOMPS = MPHTOKPH * 1000.0 / 3600.0;
MPSTOMPH = 3600 / (.3048 * 5280.0);

*/

//------------
// angle
//------------

#define DEGTORAD (PI / 180.0)
#define RADTODEG (180.0 / PI)

//------------
// power
//------------

#define HP_TO_WATTS	745.699
#define WATTS_TO_HP	(1.0 / 745.699)

//------------
// volume
//------------

#define LITERS_TO_GALLONS .264172051
#define GALLONS_TO_LITERS 3.7854118

//------------
// temperature
//------------

#define C_TO_DEG (9.0/5.0 + 32.0)
#define DEG_TO_C (1.0 / C_TO_DEG)


//-------------------------
// from 3d riderdata.h:
//-------------------------

#define INCHES_TO_METERS(i)	((i) * 0.0254f)
#define FEET_TO_METERS(i)	(((i)*12.0f) * 0.0254f)
#define METERS_TO_FEET(m)	((m) * 3.281f)
#define METERS_TO_INCHES(m)	((m) * (3.281f * 12.0f))
#define FEET_TO_MILES(f)	((f) / 5280.0f)
#define METERS_TO_MILES(m)	(((m) * 3.281f) / 5280.0f)					// .000621401
#define MILES_TO_METERS(m)	((m) * 1609.34f)
#define KILOGRAMS_TO_POUNDS(k)	((k) * 2.204586f)
#define POUNDS_TO_KILOGRAMS(p)	((p) * 0.4536f)
#define MILES_TO_KM(m)		((m) * 1.609344f)
#define KM_TO_MILES(m)		((m) * 0.621371192237334f)




#define STATUS_WINDOW	2229


// The following macros set and clear, respectively, given bits
// of the C runtime library debug flag, as specified by a bitmask.
#ifdef   _DEBUG
	#define  SET_CRT_DEBUG_FIELD(a) \
				_CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
	#define  CLEAR_CRT_DEBUG_FIELD(a) \
				_CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
	#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
	#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

//enum eSHFOP  {
//	SHFOP_NOTFOUND, SHFOP_OK, SHFOP_FAIL, SHFOP_EXIST
//};

#define DIALOG_GRAY RGB(212, 208, 200)

#define ACK 0x06
#define NAK 0x15

/*
typedef struct  {
	int w;
	int h;
	int *data;
} BMPIMAGE;
*/

#endif	// #ifndef _DEFINES_H_

