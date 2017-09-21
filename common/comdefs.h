
#ifndef _COMDEFS_H_
#define _COMDEFS_H_

#ifdef QT_CORE_LIB
	#include <QtCore>
#endif


#ifdef QT_CORE_LIB
	#define X_EXPORT
	//#define X_EXPORT Q_DECL_EXPORT

	//#if (defined(QT_DLL) || defined(QT_SHARED)) && !defined(QT_PLUGIN)		// lib being compiled shared
	//#define X_EXPORT Q_DECL_EXPORT
	//#else								// lib being embedded
	//	#define X_EXPORT
	//#endif
#else
	#define X_EXPORT
	//#define X_EXPORT __declspec(dllexport)
#endif

// lib being linked against (must be shared on Window$!)
//#define X_EXPORT Q_DECL_IMPORT

#ifndef FALSE
	#define FALSE false
#endif

#ifndef DEL
	#define DEL(p)  { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef FCLOSE
	#define FCLOSE(stream)  { if((stream)!=NULL) { fclose(stream); (stream)=NULL; } }
#endif

#ifndef DELARR
	#define DELARR(p)  { if(p) { delete [] (p); (p)=NULL;} }
#endif


#ifndef TOGGLE
	#define TOGGLE(b)  { if(b)  {b = false;} else {b = true; } }
#endif

#ifndef DWORD
	#ifndef WIN32
		#define DWORD unsigned long
	#endif
#endif

#ifndef NULL
	#define NULL 0
#endif

#ifndef nullptr
	//#define nullptr 0
#endif

#ifndef WIN32
	#ifndef MAX_PATH
		#define MAX_PATH 256
	#endif
#endif

#ifndef PI
	#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164
	#define TWOPI (2.0*PI)
#endif


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




#endif

