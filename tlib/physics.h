#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <glib_defines.h>

#include <ini.h>
#include <lpfilter.h>
#include <bike.h>
#include <float2d.h>
#include <averagetimer.h>
#include <tdefs.h>
#include <monarch.h>


#define NEWHYSTERESIS
#define TABLEVER 2
//#ifdef DOSS
#define DO93_STUFF

#if 0
static double ampVals[17] = {
	0.00, 
	0.05,
	0.10,
	0.15,
	0.20,
	0.25,
	0.50,
	0.75,
	1.00,
	1.25,
	1.50,
	1.75,
	2.00,
	2.25,
	2.50,
	2.75,
	3.00
};

#define NWATTVALS 33
#define NRPMVALS 7
static int rpmVals[NRPMVALS] = {	0, 10, 15, 20, 25, 30, 40 };
static double wattsVals[NWATTVALS] = {
	1.0,
	2.0,
	3.0,
	4.0,
	5.0,
	6.0,
	7.0,
	8.0,
	9.0,
	10.0,
	11.0,
	12.0,
	13.0,
	14.0,
	15.0,
	17.5,
	20.0,
	30.0,
	40.0,
	50.0,
	60.0,
	70.0,
	80.0,
	90.0,
	100.0,
	120.0,
	140.0,
	160.0,
	180.0,
	200.0,
	220.0,
	240.0,
	260.0
};

static double lowTable[NRPMVALS][NWATTVALS] = {
//					 1      2      3      4      5      6      7      8      9      10     11     12     13     14     15     17.5   20     30     40     50     60     70     80     90     100    120    140    160    180    200    220    240    260
/*  0 rpm */  	{1.356, 1.893, 2.340, 2.775, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000 },		//  0 rpm
/* 10 rpm */  	{0.452, 0.631, 0.780, 0.925, 1.037, 1.140, 1.239, 1.341, 1.439, 1.527, 1.660, 1.749, 1.849, 1.940, 2.032, 2.361, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000 },		// 10 rpm
/* 15 rpm */  	{0.280, 0.409, 0.487, 0.577, 0.661, 0.728, 0.801, 0.856, 0.908, 0.956, 1.014, 1.063, 1.110, 1.172, 1.212, 1.313, 1.433, 1.832, 2.410, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000 },		// 15 rpm
/* 20 rpm */  	{0.195, 0.307, 0.358, 0.416, 0.475, 0.537, 0.584, 0.642, 0.678, 0.716, 0.772, 0.811, 0.846, 0.879, 0.883, 0.985, 1.040, 1.313, 1.552, 1.790, 2.064, 2.460, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000 },		// 20 rpm
/* 25 rpm */  	{0.147, 0.227, 0.280, 0.334, 0.379, 0.425, 0.451, 0.487, 0.544, 0.567, 0.592, 0.633, 0.655, 0.691, 0.710, 0.762, 0.829, 1.034, 1.208, 1.373, 1.533, 1.699, 1.850, 2.041, 2.270, 2.650, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000, 3.000 },		// 25 rpm
/* 30 rpm */  	{0.119, 0.184, 0.251, 0.288, 0.330, 0.367, 0.393, 0.431, 0.460, 0.482, 0.512, 0.542, 0.570, 0.592, 0.594, 0.670, 0.693, 0.867, 1.011, 1.148, 1.261, 1.365, 1.492, 1.597, 1.713, 1.990, 2.270, 2.650, 3.000, 3.000, 3.000, 3.000, 3.000 },		// 30 rpm
/* 40 rpm */  	{0.080, 0.139, 0.197, 0.226, 0.255, 0.298, 0.311, 0.333, 0.352, 0.359, 0.389, 0.416, 0.428, 0.452, 0.442, 0.508, 0.532, 0.665, 0.761, 0.868, 0.958, 1.029, 1.113, 1.187, 1.257, 1.400, 1.550, 1.670, 1.810, 1.960, 2.130, 2.340, 2.570 }		// 40 rpm
};


static double table[26][17] = {

// amps:
//             0.0      .05     .10     .15     .20     .25      .50      .75     1.0      1.25     1.50      1.75      2.00      2.25      2.50      2.75      3.00
//             0       1       2       3       4       5        6        7        8        9        10        11        12        13        14        15        16

/*   0 */	{  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,   0.000,   0.000,   0.000,   0.000,   0.000,    0.000,    0.000,    0.000,    0.000,    0.000,    0.000 },
/*  10 */	{  0.050,  0.050,  0.155,  0.240,  0.310,  0.315,   1.409,   2.813,   4.531,   6.563,   8.750,   11.409,   13.906,   15.469,   16.409,   19.375,   20.156 },
/*  20 */	{  0.190,  0.200,  0.610,  0.790,  1.250,  1.450,   5.313,  10.625,  17.813,  26.250,  34.063,   45.938,   55.625,   61.250,   70.313,   77.500,   80.313 },
/*  30 */	{  0.510,  0.525,  1.283,  1.650,  2.580,  3.150,  11.250,  22.969,  38.906,  57.656,  75.000,   99.844,  119.063,  132.656,  151.406,  160.781,  172.031 },
/*  40 */	{  0.940,  1.000,  2.180,  2.800,  4.300,  5.320,  18.125,  38.125,  65.635,  97.500, 125.635,  168.750,  198.750,  226.250,  254.375,  273.750,  286.880 },
/*  50 */	{  1.475,  1.625,  3.125,  4.050,  6.250,  7.975,  26.563,  57.044,  95.313, 143.750, 191.406,  245.313,  289.063,  333.594,  373.438,  396.094,  421.875 },
/*  60 */	{  2.130,  2.340,  4.200,  5.520,  8.400, 10.890,  36.563,  75.938, 127.500, 188.453, 255.000,  324.375,  387.188,  446.250,  504.375,  540.000,  568.140 },
/*  70 */	{  2.905,  3.255,  5.495,  7.035, 10.675, 14.000,  45.938,  94.063, 159.688, 236.250, 318.281,  405.781,  487.813,  563.281,  636.563,  678.672,  714.210 },
/*  80 */	{  3.760,  4.360,  6.880,  8.760, 13.040, 17.160,  55.000, 111.250, 188.750, 280.000, 382.500,  487.500,  587.500,  682.500,  767.500,  831.250,  867.520 },
/*  90 */	{  4.815,  5.625,  8.460, 10.575, 15.435, 20.430,  63.281, 129.375, 216.563, 322.031, 441.563,  572.344,  686.250,  797.344,  896.484,  972.422, 1018.125 },
/* 100 */	{  5.950,  7.000, 10.150, 12.500, 18.000, 23.950,  71.875, 146.875, 245.313, 364.088, 498.438,  648.438,  784.375,  918.750, 1032.813, 1121.875, 1165.650 },
/* 110 */	{  7.260,  8.580, 12.045, 14.630, 20.625, 27.500,  80.781, 163.281, 268.125, 402.188, 550.000,  720.156,  874.844, 1023.344, 1154.141, 1252.969, 1302.840 },
/* 120 */	{  8.640, 10.440, 13.980, 16.800, 23.340, 30.840,  88.125, 174.375, 290.625, 435.000, 598.125,  787.500,  957.180, 1130.250, 1278.750, 1387.500, 1441.920 },
/* 130 */	{ 10.270, 12.415, 16.185, 19.110, 26.130, 34.580,  93.438, 186.875, 312.813, 467.188, 645.938,  849.063, 1040.000, 1229.719, 1391.406, 1516.328, 1582.360 },
/* 140 */	{ 12.040, 14.560, 18.480, 21.630, 29.120, 38.220,  98.438, 199.063, 334.688, 494.375, 686.875,  907.813, 1115.625, 1330.000, 1505.000, 1647.188, 1706.250 },
/* 150 */	{ 14.100, 16.950, 21.000, 24.150, 32.250, 42.150, 103.125, 208.594, 351.563, 522.656, 726.563,  958.594, 1184.775, 1416.797, 1607.813, 1761.328, 1830.450 },
/* 160 */	{ 16.320, 19.360, 23.760, 27.040, 35.360, 45.920, 108.750, 220.000, 367.500, 550.000, 765.000, 1004.427, 1252.500, 1502.500, 1710.000, 1875.000, 1952.480 },
/* 170 */	{ 18.870, 22.100, 26.520, 29.835, 38.760, 49.725, 114.219, 231.094, 382.500, 571.094, 802.188, 1048.023, 1317.500, 1583.231, 1811.563, 1976.250, 2063.885 },
/* 180 */	{ 21.600, 25.020, 29.520, 33.030, 42.210, 53.730, 120.938, 239.063, 399.375, 596.250, 829.688, 1089.362, 1380.938, 1658.700, 1905.210, 2075.580, 2175.290 },
/* 190 */	{ 24.510, 28.120, 32.680, 36.100, 45.695, 57.855, 126.172, 246.406, 409.688, 611.563, 852.031, 1128.444, 1429.465, 1732.266, 1997.280, 2075.580, 2182.765 },
/* 200 */	{ 27.800, 31.200, 36.000, 39.500, 49.200, 62.000, 131.250, 256.250, 421.875, 628.150, 876.600, 1165.270, 1475.000, 1793.750, 2017.500, 2075.580, 2182.765 },
/* 210 */	{ 31.080, 34.650, 39.585, 43.050, 53.025, 66.255, 136.198, 265.781, 433.125, 643.125, 899.063, 1199.839, 1521.408, 1850.625, 2017.500, 2075.580, 2182.765 },
/* 220 */	{ 34.760, 38.280, 43.120, 46.750, 56.870, 70.620, 140.993, 275.000, 446.875, 663.438, 921.250, 1232.152, 1565.212, 1904.375, 2017.500, 2075.580, 2182.765 },
/* 230 */	{ 38.870, 41.975, 46.920, 50.600, 60.835, 74.865, 147.315, 283.906, 456.406, 675.625, 941.563, 1262.208, 1606.406, 1962.188, 2017.500, 2075.580, 2182.765 },
/* 240 */	{ 43.080, 46.080, 50.760, 54.600, 64.800, 79.200, 153.600, 288.750, 465.000, 690.000, 963.750, 1290.000, 1645.080, 1962.188, 2017.500, 2075.580, 2182.765 },
/* 250 */	{ 47.500, 50.000, 55.000, 58.750, 69.000, 83.750, 160.000, 292.969, 476.563, 703.125, 984.250, 1315.500, 1681.125, 1962.188, 2017.500, 2075.580, 2182.765 }
};


#endif                                  // #if 0




#include <logger.h>


#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
	class __declspec(dllexport) Physics  {
#else
	class Physics  {
#endif

	//friend class EV;
	friend class Velotron;
	friend class velotronDecoder;
	friend class Calibrator;
	friend class EV;
	
	public:
		/*
		enum PHYSICS_MODE  {
			PHYSICS_WIND_LOAD_MODE,					// old WINDLOAD mode
			PHYSICS_CONSTANT_TORQUE_MODE,
			PHYSICS_CONSTANT_WATTS_MODE,			// old ERGO mode
			PHYSICS_CONSTANT_CURRENT_MODE,
			PHYSICS_NOMODE
		};
		*/

		/*
		// from computrainer.h
		#define	HB_WIND_RUN		0x2c
		#define	HB_WIND_PAUSE	0x28
		#define	HB_RFTEST		0x1c
		#define	HB_ERGO_RUN		0x14
		#define	HB_ERGO_PAUSE	0x10
		*/

		enum MODE  {				// MAKE SURE THAT modestrs is in sync with this!
			WINDLOAD,
			CONSTANT_TORQUE,
			CONSTANT_WATTS,			// old ERGO mode
			CONSTANT_CURRENT,
			NOMODE
		};
		static MODE mode;

		static const char *modestrs[5];
		/*
		= {
			"WINDLOAD",
			"CONSTANT_TORQUE",
			"CONSTANT_WATTS",
			"CONSTANT_CURRENT",
			"NOMODE"
		};
		*/
		static bool gblog;
		static bool bini;

	private:
		#define DYNO_ARM_LENGTH 42.5				// inches
		#define NEW_PHYSICS_TIMER
		double measured_ergo_watts;
		Monarch monarch;
		double constant_pounds;
		double constant_kgs;

#ifdef _DEBUG
		int resets;
#endif

		double watts_factor;
		double net_watts;
		double wf_watts;
		double flywheel_watts;
		DWORD startTime;
		DWORD pausedTime;				// accumulates paused time
		DWORD pauseStartTime;

		//int formula;
		//double constantTorque;
		double torqueFP;							// torque in foot pounds
		double torqueNM;							// torque in newton meters
		double flywheelRadius;

		float constantCurrent;
		float constantWatts;

		void runWindLoad(void);
		unsigned long counter;						// for debugging
		unsigned long loggingStartTime;
		unsigned long loggingEndTime;
		double lowCurrent(double _rpm, double _watts);

		void cleanup(void);
		double lookupRPM, rpmLookupFactor;
		void logData(void);
		void manualLoadControl(void);
		void ergoDegauss(void);
		int ergoDegaussAmplitude;
		int ergoDegaussPeriod;
		int ergoDegaussCounter;
		int ergoPolarity;
		unsigned short ergoDegaussValue;
		void windDegauss(void);
		//int windDegaussAmplitude;
		unsigned short windDegaussAmplitude;
		int windDegaussPeriod;
		int windDegaussCounter;
		int windPolarity;
		unsigned short windDegaussValue;
		BOOL degaussEnabled;
		bool ccd;							// constant current degaussing

		void run_constant_force(void);
		bool paused;
		AverageTimer *avgTimer;
		double e;
		double ie;
		double mamps;
		double hysteresis;
		double lastcurrent;
		double slope, lastslope;
		void adjustCurrent(void);
		Ini *ini;
		float2d *currents;
		void runConstantCurrent(void);
		double gain;
		char string[256];
		int attackCounter;
		int decayCounter;
		double wattseconds;
		double cerr;
		LPFilter *dwfilter;
		LPFilter *picCurrentFilter;
		LPFilter *accfilter;
		LPFilter *rpmFilter;
		LPFilter *mphFilter;
		double Cd;
		BOOL testing;
		double MYG;							// was G, but that is defined in antlib.h
		double I;
		double flywheelMass;
		Logger *log;
		int bp;
		double mCurrent, bCurrent;
		BOOL first;
		double LMEF;
		double Kw;
		double TOFPS;
		double TOWATTS;
		double DLOC_SCL;
		double POUNDS_TO_SLUGS;
		double CALL_HZ;
		DWORD now;
		DWORD then;
		double getTableCurrent3(double _rpm, double _watts);
		double getTableWatts(double _rpm, double _amps);
		int map(double x1, double x2, double y1, double y2, double *m, double *b);
		double k1;
		double kf;

	public:
		#define VELMINGRADE -10
		#define VELMAXGRADE 25

		typedef struct  {
			double acc;
			double rpm;
			double mph;
			double watts;
			double displayedWatts;
			double grade;
			unsigned char reserved[204];
		} STATE;

		void setDistance(double _meters);
		void setmiles(double _miles);

		void set_grade(float _f);

		void inc_factor(void)  {
			watts_factor += .01;
			if (watts_factor > 2.0)  {
				watts_factor = 2.0;
			}
			k1 = watts_factor * 23.7e-6;
			kf = 60*33000*k1/5280;					// .0088875
			return;
		}
		void dec_factor(void)  {
			watts_factor -= .01;
			if (watts_factor < 0.0)  {
				watts_factor = 0.0;
			}
			k1 = watts_factor * 23.7e-6;
			kf = 60*33000*k1/5280;					// .0088875
			return;
		}
		double inline get_measured_ergo_watts(void)  { return measured_ergo_watts; }
		double getLowTableWatts(double _rpm, double _amps);
		inline bool is_paused(void)  { return paused; }
		void set_paused(bool _b)  { paused = _b; }

		STATE state;
		inline double getWindPowerConstant(void)  {
			return k1;
		}
		inline double getWindForceConstant(void)  {
			return kf;
		}
		inline double get_watts_factor(void)  {
			return watts_factor;
		}

		bool wingate_inertial_force;
		bool wingate_flip_sign;
		double ifgain;
		double fish_scale_reading;

		void setwgif(bool _in)  {
			wingate_inertial_force = _in;
		}

		void setflip(bool _flip)  {
			wingate_flip_sign = _flip;
		}

		void setifgain(double _gain)  {
			ifgain = _gain;
		}

		void set_watts_factor(double _watts_factor)  {
			watts_factor = _watts_factor;
			if (watts_factor > 2.0)  {
				watts_factor = 2.0;
			}
			else if (watts_factor < 0.0)  {
				watts_factor = 0.0;
			}
			k1 = watts_factor * 23.7e-6;
			kf = 60*33000*k1/5280;					// .0088875
			return;
		}

		bool calibrating;
		Physics( Bike *_bike, double _personweight, double _watts_factor=1.0);
		virtual ~Physics(void);
		BOOL computeAcceleration(void);
		Bike *bike;
		void run(void);
		void test(void);

		void crowbarRPM(void);
		void updatePicCurrent(void);
		double draftwind;
		double kilometers;
		BOOL logging;
		unsigned short currentCountToPic;
		double current;
		double currentFromPic;
		double mph;
		double mphFromPedals;

		double lbs;
		double person_lbs;
		void set_weight_lbs(double _d);

#ifdef DO93_STUFF
		double fps93;
		double lastfps93;
		double rawAcceleration93;
		double dv93;
		double faccel93;
		LPFilter *accfilter93;
		double if93;
		double force93;
#endif

		double fps;
		double ipm;			// inches per minute
		double fpsFromPedals;
		double masterCurveForce;
		double totalwind;
		double gradeForce;
		double force;
		double inertialForce;
		double lastfps;
		double dt;
		double windForce;
		double tireForce;
		BOOL ifenabled;
		void computeMass(void);
		double omega;
		double omega_0;
		double lastOmega;
		double alpha;
		double torque;
		double power;
		double riderInput;
		double dv;
		double mass;
		unsigned short mcur;
		LPFilter *wfilter;
		LPFilter *fwfilter1;
		LPFilter *fwfilter2;
		int decayDelay;
		int attackDelay;
		double decayFactor;
		double attackFactor;
		double Q;
		double clippingCurrent;
		void gradeUp(void);
		void gradeDown(void);
		BOOL diffrpm;
		DWORD rpmDiffCount;
		double sumAverageMPH, sumAverageWatts, sumAverageRPM;
		DWORD averageMPHcount, averageWattsCount, averageRPMcount;
		int stopFlag;
		double pgain;
		double igain;
		void reset(void);
		double getGrade(void)  { return grade; }
		double rpmFromHoles;			// the raw (from the pic) rpm computed from the flywheel holes
		double rpmFromPedals;		// the raw (from the pic) rpm computed from the cadence sensor
		double frpm;					// low pass filtered 'rpmFromHoles'
		double fmph;					// low pass filtered 'mph', which is computed from 'rpmFromHoles'
		double grade;					// grade in percent, 1% = '1', eg.
		double wind;					// wind speed in mph, - for headwind, + for tailwind
		double feet;					// distance in feet moved, computed from 'rpmFromHoles', gear, etc.
		double watts;					// lp filtered raw watts
		double displayedWatts;		// further lp filtering of watts, suitable for human display

		double averageMPH;
		double averageWatts;
		double averageRPM;
		double miles;					// same as 'feet' above converted to miles
		double Calories;				// Food Calories computed from highly LP filtered watts (displayedWatts)

		double peakMPH;
		double peakRPM;
		double peakWatts;
		double rawAcceleration;		// raw acceleration as it comes from the PIC in feet per second per second
		double faccel;					// low pass filtered rawAcceleration
		DWORD ms;						// current time in miliseconds
		void resetIntegrators(void);
		void pause(void);
		void resume(void);
		double getAcceleration(void);
		double getRPM(void);
		double getMPH(void);
		double getWatts(void);
		float getConstantWatts(void);
		double getDisplayedWatts(void);
		double getPicCurrent(void);

		void setMode(MODE _mode);

		void setConstantCurrent(float _constantCurrent);
		void setConstantWatts(float _constantWatts);

		void set_constant_force(double _constant_pounds);
		void writeState(FILE *stream);
		void readState(FILE *stream);

};

//#else
//	#define VELMINGRADE -10
//	#define VELMAXGRADE 25

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif


#endif		// #ifndef _PHYSICS_H_
