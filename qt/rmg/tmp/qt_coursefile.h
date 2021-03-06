#ifndef __COURSEFILE_H
#define __COURSEFILE_H


//#define COURSEFILE_VERSION		6	// Default version - unless we have speical data.
//#define COURSEFILE_VERSION_MAX	7	// Max recongized version

// =============================================
// SPINSCAN DEFINES
//
// Kept here so that the course data can be
// self contained.
// =============================================
#define SPIN_SEGS				24  // Number of sping segs in the spinscan snapshot
#define SPIN_AVEDIV				(SPIN_SEGS/2)
#define SPIN_SEG_FIRST_LEFT		0
#define SPIN_SEG_LAST_LEFT		(SPIN_SEGS/2 - 1)
#define SPIN_SEG_FIRST_RIGHT	(SPIN_SEGS/2)
#define SPIN_SEG_LAST_RIGHT		(SPIN_SEGS-1)


// =============================================
// PREFORMANCE SECTION NAMES
// =============================================

#define COURSEFILE_PERFORMANCE	"perf"
    // PerformanceInfo
    // PerfPoint[ count ]

// =====================================
// struct PerformanceInfo
// =====================================

struct PerformanceInfo  {
    //-----------------------------------------
    // 1 = original version
    // 2 = version with grade scaling fixed
    //-----------------------------------------
    long		version;

    // Rider information
    char			name[64];
    unsigned char	gender;
    unsigned char	age;
    char			_pad1[2];

    float			height;		// In meters.
    float			weight;		// In kilograms
    long			upper_hr;
    long			lower_hr;

    // Begin time
    unsigned long		year;	// i.e. 2001
    unsigned char		month;	// 1 - 12
    unsigned char		day;	// 1 - 31
    unsigned char		hour;	// 0 - 23
    unsigned char		minute;	// 0 - 60

    long		perfcount;	// Number of performance points.
    long		racetime;	// How long it took to compleat this race.


    // Course Summery
    // ==============
    float		length;		// Course length in KM

    unsigned short rfDrag;
    unsigned short rfMeas;
    float watts_factor;
    float ftp;									// functional threshold power
    long		_pad2[29];	// Extra information - set this pad to zero.

};			// size must be 236 bytes


/*
struct PerformanceInfo2  {					// size will be 1024 bytes
    int version;
    float rr;									// rolling resistance
    unsigned char dummy[1024-12];
};
*/


#define GENDER_MALE		0
#define GENDER_FEMALE	1

// =====================================
// struct PerfPoint
//
// Size of a perfPoint 40 (16+24) bytes.
// Assuming 1 perfpoint every 1/2 second - that would be 281K an hour.
// =====================================

struct PerfPoint  {

    unsigned char hr;
    unsigned char rpm;
    unsigned short watts;
    float kph;
    float kilometers;
    unsigned long ms;
    short pp_s_grade_t_100;				// convert to float and divide this by 100 to get grade
    unsigned char left_ata;
    unsigned char right_ata;
    unsigned char rightss;				// right spinscan
    unsigned char leftss;				// left spinscan
    unsigned char leftsplit;			// left split
    unsigned char rightsplit;			// right split.
    unsigned char spin[SPIN_SEGS];		// 0-255, 255 = 1.0f

    PerfPoint()  {
        clear();
    }

    void clear()  {
        hr = 0;
        rpm = 0;
        watts = 0;
        kph = 0;
        kilometers = 0;
        ms = 0;
        pp_s_grade_t_100 = 0;				// convert to float and divide this by 100 to get grade
        left_ata = 0;
        right_ata = 0;
        rightss = 0;				// right spinscan
        leftss = 0;				// left spinscan
        leftsplit = 0;			// left split
        rightsplit = 0;			// right split.

        int n = sizeof(spin);
        memset(spin, 0, n);
    }
};

#endif // __COURSEFILE_H

