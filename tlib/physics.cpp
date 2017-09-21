#ifdef WIN32
    #pragma warning(disable:4996)					// for vista strncpy_s
#endif


#ifdef WIN32
	#define STRICT
	#define WIN32_LEAN_AND_MEAN

	#ifdef VISTA
		#pragma warning(disable:4101 4786)				// "str" unreferenced
		#pragma warning(disable: 4146)					// mso9.tlh(931) warning
	#endif

	#include <windows.h>
	#include <mmsystem.h>

	#ifdef _DEBUG
		#include <time.h>
		#ifndef VISTA
			//#include <xl.h>
		#else
xxxxxxxxxxxx
			//#error "VISTA DEFINED"
		#endif
	#endif
#else
	#include <minmax.h>
#endif


#include <math.h>
#include <assert.h>

#include <comutils.h>

#include <glib_defines.h>
#include <crf.h>
#include <comglobs.h>
#include <globals.h>
#include <oldsdirs.h>
#include <tglobs.h>
#include <fatalerror.h>
#include <utils.h>
#include <tglobs.h>
#include <physics.h>

Physics::MODE Physics::mode = WINDLOAD;

std::vector<std::string> hdrstrings;

const char *Physics::modestrs[5] = {
	"WINDLOAD",
	"CONSTANT_TORQUE",
	"CONSTANT_WATTS",
	"CONSTANT_CURRENT",
	"NOMODE"
};

extern bool oktobreak;


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


bool Physics::gblog = false;
bool Physics::bini = false;


/**********************************************************************
	fps93 = fps / (bike->state.gear / 93.0);
	dv93 = fps93 - lastfps93;
	lastfps93 = fps93;
	rawAcceleration93 = dv93 / dt;
	faccel93 = accfilter93->calc(rawAcceleration93);		// 10 point filter
	if93 = (LMEF * POUNDSTOSLUGS) * faccel93;
	force93 = tireForce + masterCurveForce + if93 + gradeForce;   <<<<<<<<<<<<<<<<< THIS IS WHAT I PLOTTED

**********************************************************************/

Physics::Physics(Bike *_bike, double _person_lbs, double _watts_factor) :
		Kw((44.0*44.0)/8.0),
		TOFPS(5280.0 / 3600.0),
		TOWATTS(746.0 / 550.0),
		DLOC_SCL(1.0/1000.0),
		POUNDS_TO_SLUGS(0.03108),			// 1 lb / 32.2 fps/s
		CALL_HZ(20.0)
	{

	bike = _bike;
	person_lbs = _person_lbs;

	wingate_inertial_force = true;
	wingate_flip_sign = false;
	ifgain = 1.0;


	startTime = timeGetTime();						// tlm20050727

	measured_ergo_watts = 0.0;

	pausedTime = 0;
	pauseStartTime = 0;

	e = 0.0;
	lookupRPM = 0;
	mamps = 0;
	slope = 0;
	currents = NULL;
	string[0] = 0;
	bp = 0;
	ipm = 0;
	fpsFromPedals = 0;
	omega = 0;
	omega_0 = 0;
	alpha = torque = power = 0;
	riderInput = 0;
	diffrpm = 0;
	//monarch_constant_pounds = 0.0;
	torqueFP = 0.0;
	torqueNM = 0.0;
	memset(&state, 0, sizeof(state));
//	formula = 1;
#ifdef _DEBUG
		resets = 0;
#endif

	//-------------------------------
	// setup velotron parameters:
	//-------------------------------

	rpmLookupFactor = bike->state.params.real_front_teeth / 62.0;
	MYG = 32.1393;
	LMEF = 22.9;
	flywheelRadius = 1.5;					// radius of the flywheel in feet

	Cd = (double)(3600.0*3600.0*60.0*33000*23.7e-6) / (double)(5280.0*5280.0*5280.0);
	flywheelMass = 50.0 / MYG;			// flywheel weighs 50 lbs
	I = .9004;							// slug-ft^2
	clippingCurrent = 2.67;				// 2.94
	hysteresis = .0255;							// hard code hysteresis

	Q = 1.0;

	lbs = (float) (TOPOUNDS*bike->get_kgs()) + person_lbs;

	map(0.0, 3.0, 0.0, 1023.0, &mCurrent, &bCurrent);

	mode = WINDLOAD;			//WIND_LOAD;

	ini = NULL;
	if (bini)  {
		ini = new Ini("physics.ini");
	}
	else  {
		ini = NULL;
	}

	logging = false;

	if (gblog)  {
#ifdef DOGLOBS
		if (dirs[DIR_DEBUG][0]==0)  {
			log = NULL;
		}
		else  {
			sprintf(string, "%s%sphysics%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, bike->getid());
#else
		if (SDIRS::dirs[DIR_DEBUG][0]==0)  {
			log = NULL;
		}
		else  {
			sprintf(string, "%s%sphysics%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, bike->getid());
#endif
			log = new Logger(string);
			logging = true;

			fish_scale_reading = 10.0*POUNDSTOKGS * monarch.radius * (52.0/14.0) / (DYNO_ARM_LENGTH * INCHESTOMETERS);
			log->write("monarch radius = %.3lf meters\n", monarch.radius);
			log->write("fish scale = %.2lf\n", fish_scale_reading);
	}
	}
	else  {
		log = NULL;
	}

	/*
	if (ini)  {
		logging = ini->getBool("options", "logging", "false", false);
	}
	else  {
		//logging = false;
	}
	*/

/*
	if (logging)  {
		ini->writeBool("options", "logging", false);		// logging only for this session, must be re-enabled manually
	}
	else  {
		rm("physics.log");
	}
*/








	first = TRUE;
	accfilter = NULL;
#ifdef DO93_STUFF
	accfilter93 = NULL;
#endif

#ifdef DOSS
	ss_acc_filter = NULL;
#endif

	rpmFilter = NULL;
	mphFilter = NULL;
	picCurrentFilter = NULL;
	avgTimer = NULL;

	paused = false;
	reset();

	if (bike->get_nrear()==0)  {
		throw("zzz1");
	}

	if (bike->get_nfront()==0)  {
		throw("zzz2");
	}


	bike->computeGearRatio();


	//----------------------------------------------------------------------
	// things that CAN be read, but aren't created if they are not there:
	//----------------------------------------------------------------------

	if (ini)  {
		k1 = (double)ini->getFloat("options", "k1", ".0000237", false);
	}
	else  {
		k1 = .0000237;
	}
	kf = 60*33000*k1/5280;					// .0088875
	set_watts_factor(_watts_factor);

	ifenabled = TRUE;
	if (ini)  {
		gain = (double)ini->getFloat("options", "gain", ".65", false);
	}
	else  {
		gain = .65;
	}

	/*
	if (ini)  {
		formula = ini->getInt("options", "formula", "1", false);
	}
	else  {
		formula = 1;
	}
	*/


	int itemp;

#ifdef _DEBUG
	accfilter = new LPFilter(30);		// acceleration filter
#else
	if (ini)  {
		itemp = ini->getInt("options", "acceleration filtering", "70", false);
	}
	else  {
		itemp = 70;
	}

	accfilter = new LPFilter(itemp);		// acceleration filter
#endif

#ifdef DOSS
	ss_acc_filter = new LPFilter(10);		// acceleration filter for spinscan
#endif

#ifdef DO93_STUFF
	accfilter93 = new LPFilter(30);			// acceleration filter
#endif

	picCurrentFilter = new LPFilter(5);			// picCurrentFilter
	constantCurrent = 0.0f;
	constantWatts = 0.0;





	if (ini)  {
		pgain = ini->getFloat("physics", "pgain", ".5", false);
	}
	else  {
		pgain = .5;
	}

	if (ini)  {
		igain = ini->getFloat("physics", "igain", ".03", false);
	}
	else  {
		igain = .03;
	}

	if (ini)  {
		itemp = ini->getInt("options", "rpm filtering", "200", false);
	}
	else  {
		itemp = 200;
	}


	rpmFilter = new LPFilter(itemp);		// rpm filter

	if (ini)  {
		itemp = ini->getInt("options", "mph filtering", "200", false);
	}
	else  {
		itemp = 200;
	}

	mphFilter = new LPFilter(itemp);		// rpm filter

	if (ini)  {
		itemp = ini->getInt("options", "watts filtering", "10", false);
	}
	else  {
		itemp = 10;
	}

	wfilter = new LPFilter(itemp);		// acceleration filter
	fwfilter1 = new LPFilter(10);			// flywheel watts filter
	fwfilter2 = new LPFilter(30);			// flywheel watts filter

	dwfilter = new LPFilter(WFILTERCONSTANT);			// use this for the displayed watts

	attackFactor = 2.0;
	decayFactor = 1.2;

	attackDelay = 1;
	decayDelay = 66;

	degaussEnabled = TRUE;
	if (ini)  {
		degaussEnabled = ini->getBool("options", "degauss", "true", false);
	}
	else  {
		degaussEnabled = true;
	}

	// constant current degaussing:

	if (ini)  {
		ccd = ini->getBool("options", "cc degauss", "true", false);
	}
	else  {
		ccd = true;
	}

	if (ini)  {
		ergoDegaussAmplitude = ini->getInt("options", "ergo degauss full swing amplitude (milliamps)", "300", false);
	}
	else  {
		ergoDegaussAmplitude = 300;
	}

	ergoDegaussAmplitude = (int) (.5 + .5*((double)ergoDegaussAmplitude/1000.0) * (1023.0 / 3.0) );		// convert to number of adc counts (to pic)

	if (ini)  {
		ergoDegaussPeriod = ini->getInt("options", "ergo degauss half period (milliseconds)", "233", false);
	}
	else  {
		ergoDegaussPeriod = 233;
	}
	ergoDegaussPeriod = (int) (.5 + (double)ergoDegaussPeriod / 5.0);		// convert to 5 ms counts

	if (ini)  {
		windDegaussAmplitude = ini->getInt("options", "wind degauss full swing amplitude (milliamps)", "100", false);
	}
	else  {
		windDegaussAmplitude = 100;
	}
	windDegaussAmplitude = (int) (.5 + .5*((double)windDegaussAmplitude/1000.0) * (1023.0 / 3.0) );		// convert to number of adc counts (to pic)

	if (ini)  {
		windDegaussPeriod = ini->getInt("options", "wind degauss half period (milliseconds)", "179", false);
	}
	else  {
		windDegaussPeriod = 179;
	}
	windDegaussPeriod = (int) (.5 + (double)windDegaussPeriod / 5.0);		// convert to 5 ms counts

	computeMass();						// get mass in slugs

	avgTimer = new AverageTimer("physics");
	counter = 0;
	loggingStartTime = 0;
	loggingEndTime = 0;


}

/**********************************************************************
	destructor
**********************************************************************/

Physics::~Physics(void)  {
	cleanup();
}

/**********************************************************************

**********************************************************************/

void Physics::setDistance(double _meters)  {
	kilometers = _meters / 1000.0;
	miles = _meters * METERSTOMILES;
	feet = miles * 5280.0;
	return;
}


/**********************************************************************

**********************************************************************/

void Physics::setmiles(double _miles)  {
	miles = _miles;
	feet = 5280.0 * _miles;
	kilometers = MILESTOKM*_miles;

	return;
}

/********************************************************************

********************************************************************/

int Physics::map(double x1, double x2, double y1, double y2, double *m, double *b)  {
	double d;

	d = x2 - x1;

	if (d == 0.0) {		// prevent divide by 0
		*m = 0.0;
		*b = y1;
		return -1;
   }
   *m = (y2-y1) / (x2-x1);
   *b = (y1 - *m*x1);
	return 0;
}

/**********************************************************************

**********************************************************************/

void Physics::updatePicCurrent(void)  {
	currentCountToPic = (unsigned short) (.5 + mCurrent*current + bCurrent);
	return;
}

/*************************************************************************

*************************************************************************/

void Physics::computeMass(void)  {
	mass = POUNDSTOSLUGS * lbs;
	return;
}

/******************************************************************************

******************************************************************************/

void Physics::run(void)  {

	if (!paused)  {
		if ( ms == (0xffffffff - 4) )  {
			startTime = timeGetTime();
			pausedTime = 0;
			ms += 5;
		}
		else  {
			DWORD dw = timeGetTime() - startTime - pausedTime;
			ms = dw;
		}
	}

	if (mcur > 1023)  {
		mcur = 1023;
	}
	mamps = 3.0 * ((double)mcur/1023.0);

	if (stopFlag)  {
		if (frpm < 1.0) {
			stopFlag = false;
		}
	}

	switch(mode)  {
		case WINDLOAD:
			runWindLoad();
			break;

		case CONSTANT_TORQUE:
			run_constant_force();
			break;
		case CONSTANT_WATTS:
			manualLoadControl();
			return;

		case CONSTANT_CURRENT:
			runConstantCurrent();
			return;

		default:
			cleanup();
			throw(fatalError(__FILE__, __LINE__, "Bad Physics Mode"));
	}

	return;
}

/******************************************************************************

******************************************************************************/

BOOL Physics::computeAcceleration(void)  {

	/*
#ifdef _DEBUG
	if (!paused)  {
		if (rpmFromHoles==0.0)  {				// shouldn't happen!
			bp = 1;
		}
	}
#endif
	*/

	frpm = rpmFilter->calc(rpmFromHoles);		// filtered acceleration
	fpsFromPedals = (PI * rpmFromPedals * bike->state.gear_inches) / (12*60);		// doesn't work! (flat line)
	mphFromPedals = (3600.0/5280.0) * fpsFromPedals;			// based on pedal rpm

	if (!paused)  {
		if (rpmFromPedals > 0)  {
			sumAverageMPH += mphFromPedals;
			averageMPHcount++;
			averageMPH = sumAverageMPH / averageMPHcount;
		}
		if (rpmFromHoles > peakRPM)  {
			peakRPM = rpmFromHoles;
		}
		/*
#ifdef _DEBUG
		if (rpmFromHoles==0.0)  {
			bp = 2;							// shouldn't happen
		}
#endif
		*/
	}

	ipm = PI * bike->state.gear_inches * rpmFromHoles;		// factor used to compute speed
	fps = ipm / (12.0*60.0);

#ifdef DO93_STUFF
	fps93 = fps / (bike->state.gear_inches / 93.0);
#endif

	mph = (3600.0/5280.0) * fps;

#ifdef _DEBUG
	/*
	if (!paused)  {
		if (mph > 5.0)  {				// shouldn't happen!
			bp = 1;
		}
	}
	*/
#endif


	lookupRPM = rpmLookupFactor * rpmFromHoles;
	fmph = mphFilter->calc(mph);		// filtered mph

	if (calibrating)  {
		// now that we have fmph, we can get out of here
		return false;
	}
	if (!paused)  {
		if (fmph > peakMPH)  {
			peakMPH = fmph;
		}
	}

	if (first)  {
		first = false;
		lastfps = fps;
		lastfps93 = fps93;


		rawAcceleration = 0.0;
		dv = 0.0;
		faccel = 0.0;
		return true;				// returns true if first call
	}

	if (!paused)  {
		if (ms != 0)  {							// don't start integrating at time 0
			miles += mph*(.005/3600.0);			// integrate the speed to get miles
		}
	}
	feet = miles*5280.0;

	kilometers = miles*TOKM;

	dv = fps - lastfps;
#ifdef DO93_STUFF
	dv93 = fps93 - lastfps93;
#endif

#ifdef _DEBUG
	/*
	if (!paused)  {
		if (fps==0.0)  {
			bp = 2;
		}
		//if (dv > 12.0f)  {
		if (lastfps==0.0)  {				// shouldn't happen!
			bp = 1;
		}
	}
	*/
#endif

	lastfps = fps;
	rawAcceleration = dv / dt;					// spinscan uses this raw acceleration
	faccel = accfilter->calc(rawAcceleration);		// filtered acceleration

#ifdef DO93_STUFF
	lastfps93 = fps93;
	rawAcceleration93 = dv93 / dt;					// spinscan uses this raw acceleration
	faccel93 = accfilter93->calc(rawAcceleration93);		// filtered acceleration
#endif

	faccel *= gain;
#ifdef DOSS
	ss_accel = ss_acc_filter->calc(rawAcceleration);		// filtered acceleration for spinscan
#endif

	return false;
}					// computeAcceleration()

/**********************************************************************

**********************************************************************/

void Physics::gradeUp(void)  {
	grade += .1;
	if (grade > VELMAXGRADE) grade = VELMAXGRADE;
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::gradeDown(void)  {
	grade -= .1;
	if (grade < VELMINGRADE) grade = VELMINGRADE;
	return;
}

#define MAXWATTS 2017.5
#define NN 17

/*************************************************************************************

*************************************************************************************/

double Physics::getTableCurrent3(double _rpm, double _watts)  {
	int rpmlo, rpmhi;
	double m, b;
	int i;
	int klo, khi;
	int i1, i2, i3, i4;
	double highRPMcurrent;


	if (_rpm < 40.0)  {
		current = lowCurrent(_rpm, _watts);
		return current;
	}

	if (_rpm > 250)  {
		current = 3.0;
		return current;
	}
	if (_watts <= 0.0)  {
		current = 0.0;
		return current;
	}
	if (_watts > MAXWATTS)  {
		current = 3.0;
		return current;
	}


	//--------------------------
	// check for mod 10 rpms:
	//--------------------------

	i = (int) (.5 + _rpm*10.0);
	if (i%100==0)  {
		// we are on an even rpm boundary (within a 10th of an rpm), so just do a single dimension interpolation
		rpmlo = i/10;
		klo = rpmlo/10;

		//-------------------------------------------------------
		// find the ammps interval we're in for this mod 10 rpm:
		//-------------------------------------------------------

		for(i=1; i<NN; i++)  {
			if (_watts >= table[klo][i-1] && _watts < table[klo][i])  {
				break;
			}
		}

		if (i==NN)  {			// we're off the chart
			current = 3.0;
			return current;
		}

		i1 = i-1;						// left amps
		i2 = i;							// right amps

		dmap(table[klo][i1], table[klo][i2], ampVals[i1], ampVals[i2], &m, &b);
		current = m*_watts + b;

		if(current < 0.0)  {
			current = 0.0;						// doesn't get here
		}
		else if (current > 3.0)  {
			current = 3.0;						// doesn't get here
		}

		return current;

	}

	//------------------------
	// non mod 10 routine:
	//------------------------

	rpmlo = 10*( ((int)_rpm) / 10);
	klo = rpmlo / 10;						// klo is the rpm index
	if (klo == 25)  {
		current = 3.0;
		return current;
	}

#define ZERO_RPM_CURRENT 0.0

	if (klo==0)  {				// special case below 10 rpm because all columns are ZERO_RPM_CURRENT
		khi = 1;
		if (_watts <= table[khi][0])  {
			current = 0.0;
			return current;
		}
		if (_watts > table[khi][NN-1])  {
			current = 3.0;
			return current;
		}

		// the _watts is in the range of the 10 rpm row

		for(i=1; i<NN; i++)  {
			if (_watts >= table[khi][i-1] && _watts < table[khi][i])  {
				break;
			}
		}

		if (i==NN)  {
			cleanup();
			throw(fatalError(__FILE__, __LINE__));
		}

		i3 = i-1;
		i4 = i;

		// the _watts is between i3 & i4 on the 10 rpm line

		dmap(table[khi][i3], table[khi][i4], ampVals[i3], ampVals[i4], &m, &b);
		highRPMcurrent = m*_watts + b;

		// now just map our _rpm to the 0 amps to highRPMcurrent and we're done

		dmap(0.0, 10.0, ZERO_RPM_CURRENT, highRPMcurrent, &m, &b);
		current = m*_rpm + b;

		if(current < 0.0)  {
			current = 0.0;						// doesn't get here
		}
		else if (current > 3.0)  {
			current = 3.0;						// doesn't get here
		}

		return current;
	}

	//----------------------------
	// handle the low rpm
	//----------------------------

	if (_watts <= table[klo][0])  {
		current = 0.0;
		return current;
	}

	//-------------------------------------------------------
	// find the amps interval we're in for the low rpm:
	//-------------------------------------------------------

	for(i=1; i<NN; i++)  {
		if (_watts >= table[klo][i-1] && _watts < table[klo][i])  {
			break;
		}
	}

	if (i==NN)  {
		current = 3.0;
		return current;
	}

	i1 = i-1;
	i2 = i;

	// i1, i2 is the current interval index for the low rpm range

	//---------------------------------------------------
	// find the amps interval we're in for the high rpm:
	//---------------------------------------------------

	rpmhi = rpmlo + 10;
	khi = rpmhi / 10;						// khi is the rpm index
	if (khi > 25)  {
		current = 3.0;
		return current;
	}


	if (_watts <= table[khi][0])  {
		current = 0.0;
		return current;
	}

	if (_watts > table[khi][NN-1])  {
		if (khi==0)  {						// tlm 0430
			current = 0.0;
		}
		else  {
			current = 3.0;
		}
		return current;
	}


	for(i=1; i<NN; i++)  {
		if (_watts >= table[khi][i-1] && _watts < table[khi][i])  {
			break;
		}
	}

	if (i==NN)  {
		current = 3.0;
		return current;
	}

	i3 = i-1;
	i4 = i;

	// i3, i4 is the current interval for the high rpm range

	dmap(table[klo][i1], table[klo][i2], ampVals[i1], ampVals[i2], &m, &b);
	double lowRPMcurrent = m*_watts + b;

	dmap(table[khi][i3], table[khi][i4], ampVals[i3], ampVals[i4], &m, &b);
	highRPMcurrent = m*_watts + b;

	dmap(rpmlo, rpmhi, lowRPMcurrent, highRPMcurrent, &m, &b);
	current = m*_rpm + b;

	if(current < 0.0)  {
		current = 0.0;						// doesn't get here
	}
	else if (current > 3.0)  {
		current = 3.0;						// doesn't get here
	}

	return current;
}								// double Physics::getTableCurrent3(double _rpm, double _watts)  {

/**********************************************************************

**********************************************************************/

void Physics::reset(void)  {

#ifdef _DEBUG
	resets++;
	if (resets==2)  {
		bp = 5;
	}
#endif

	calibrating = false;
	resetIntegrators();
	ergoDegaussCounter = 10000000;
	ergoPolarity = 0;
	ergoDegaussValue = 0;
	windDegaussCounter = 10000000;
	windPolarity = 0;
	windDegaussValue = 0;
	ie = 0.0;
	mph = 0.0;						// bike speed in mph
	grade = 0.0;						// grade in percentage
	wind = 0.0;						// head/tail wind in mph (- for headwind)
	draftwind = 0.0;				// wind effect due to drafting
	mcur = 0;
	lastcurrent = 0.0;
	lastslope = 0.0;
	then = timeGetTime();
	rpmFromPedals = 0.0;
	rpmFromHoles = 0.0;
	mphFromPedals = 0.0;
	displayedWatts = 0.0;
	rpmDiffCount = 0;
	frpm = 0;
	fmph = 0;
	wattseconds = 0.0;
	Calories = 0.0;
	rawAcceleration = 0.0;
	testing = FALSE;
	currentFromPic = 0.0;							// in amps
	cerr = 0.0;							// current error
	dv = 0.0;
	watts = 0.0;
	tireForce = 1.0;						// constant 1.0 lbs of tire drag
	faccel = 0.0;
	current = 0.0;						// default to no current
	lastOmega = 0.0;
	fps = 0.0;							// speed in fps
	lastfps = 0.0;
	masterCurveForce = 0.0;						// wind drag force in pounds
	totalwind = 0.0;					// sum of headwind and dragwind in fps
	gradeForce = 0.0;					// grade drag in lbs
	force = 0.0;							// sum of all of the drags
	inertialForce = 0.0;				// in pounds
	windForce = 0.0;
	dt = .005;
	attackCounter = 0;
	decayCounter = 0;
	currentCountToPic = (unsigned short) (.5f + (mCurrent*current + bCurrent));
	stopFlag = 0;
	return;
}					// reset()

/**********************************************************************
	leaves physics running normally, but resets time and distance to 0.
	Also resets the averages and peaks.
**********************************************************************/

void Physics::resetIntegrators(void)  {

	miles = kilometers = feet = 0.0;
	ms = 0;
	sumAverageMPH = sumAverageWatts = sumAverageRPM = 0.0;
	averageMPH = averageWatts = averageRPM = 0.0;
	averageMPHcount = averageWattsCount = averageRPMcount = 0;
	peakMPH = peakWatts = peakRPM = 0.0;
	wattseconds = 0;
	memset(&state, 0, sizeof(STATE));
	currentCountToPic = 0;
	current = 0.0;
	return;
}

/**********************************************************************
	pauses time and distance, averages, peaks
**********************************************************************/

void Physics::pause(void)  {
	pauseStartTime = timeGetTime();
	paused = true;
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::resume(void)  {
	pausedTime += (timeGetTime() - pauseStartTime);
	paused = false;
	//lastfps = fps;
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::adjustCurrent(void)  {
	int islope;
	static int lastislope=0;
	static unsigned short lastCurrentCountToPic = 0;
	static bool first = true;
	static bool firstslope = true;

	if (first)  {
		first = false;
	}
	else  {
		if (firstslope)  {
			firstslope = false;
			islope = currentCountToPic - lastCurrentCountToPic;
		}
		else  {
			islope = currentCountToPic - lastCurrentCountToPic;
			if (islope > 0)  {
				if (lastislope <= 0)  {
					currentCountToPic -= 9;
				}
			}
			else if (islope < 0)  {
				if (lastislope >= 0)  {
					currentCountToPic += 9;
				}
			}
		}
		lastislope = islope;
	}

	lastCurrentCountToPic = currentCountToPic;

	if (currentCountToPic > 1023)  {
		if (current > 1.5)  {
			currentCountToPic = 1023;
		}
		else  {
			currentCountToPic = 0;
		}
	}

	return;
}

/**********************************************************************

**********************************************************************/

void Physics::ergoDegauss(void)  {

	if (!degaussEnabled)  {
		return;
	}

	//avgTimer->update();			// 4.89 ms
	ergoDegaussCounter++;

	if (ergoDegaussCounter >= ergoDegaussPeriod)  {
		ergoDegaussCounter = 0;
		ergoPolarity ^= 1;

		if (ergoPolarity)  {
			ergoDegaussValue = ergoDegaussAmplitude;
		}
		else  {
			ergoDegaussValue = -ergoDegaussAmplitude;
		}
	}

	currentCountToPic += ergoDegaussValue;

	if (currentCountToPic > 1023)  {
		if (ergoPolarity)  {
			currentCountToPic = 1023;
			bp = 1;
		}
		else  {
			currentCountToPic = 0;
		}
	}
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::windDegauss(void)  {

#if 1
 	if (!degaussEnabled)  {
 		return;
 	}

 	//avgTimer->update();			// 4.89 ms

 	windDegaussCounter++;

 	if (windDegaussCounter >= windDegaussPeriod)  {
 		windDegaussCounter = 0;
 		windPolarity = !windPolarity;
 	}


 	if (currentCountToPic > 511) {
 		if (currentCountToPic > 1023)  {		// may not need this if you BELIEVE!
 			currentCountToPic = 1023;
 		}
 		windDegaussValue = MIN( (unsigned short)(1023 - currentCountToPic), windDegaussAmplitude);
 	}
 	else {
 		if (currentCountToPic < 0)  {			// may not need this if you BELIEVE!
 			currentCountToPic = 0;			// or this is really an unsigned short
 		}
 		windDegaussValue = MIN(currentCountToPic, windDegaussAmplitude);
 	}

 	if (windPolarity)  {
 		currentCountToPic += windDegaussValue;
 	}
 	else  {
 		currentCountToPic -= windDegaussValue;
 	}


#else
	if (!degaussEnabled)  {
		return;
	}

	//avgTimer->update();			// 4.89 ms
	windDegaussCounter++;

	if (windDegaussCounter >= windDegaussPeriod)  {
		windDegaussCounter = 0;
		windPolarity ^= 1;

		if (windPolarity)  {
			windDegaussValue = windDegaussAmplitude;
		}
		else  {
			windDegaussValue = -windDegaussAmplitude;
		}
	}

	currentCountToPic += windDegaussValue;

	if (currentCountToPic > 1023)  {
		if (windPolarity)  {
			currentCountToPic = 1023;
			bp = 1;
		}
		else  {
			currentCountToPic = 0;
		}
	}
#endif


	return;
}

/**********************************************************************

**********************************************************************/

void Physics::manualLoadControl(void)  {

	computeAcceleration();
	watts = displayedWatts = constantWatts;
	
	if (current > 0.0)  {
		measured_ergo_watts = (mamps / current) * constantWatts;
	}
	else  {
		measured_ergo_watts = constantWatts;
	}

	if (!paused)  {
		if (displayedWatts > peakWatts)  {
			peakWatts = displayedWatts;
		}
		if (!paused)  {
			if (rpmFromPedals > 0)  {
				sumAverageWatts += displayedWatts;
				averageWattsCount++;
				averageWatts = sumAverageWatts / averageWattsCount;

				sumAverageRPM += rpmFromHoles;
				averageRPMcount++;
				averageRPM = sumAverageRPM / averageRPMcount;
			}
			if (rpmFromPedals != 0)  {
				wattseconds += (displayedWatts*.005);
			}
			Calories = (wattseconds / 4186) / .26;		// 1 watt-sec = 1 Joule
		}
	}

	if (stopFlag)  {
		current = 3.0;
	}
	else  {
		getTableCurrent3(lookupRPM, constantWatts);
	}

	e = current - mamps;			// reverse action
	ie += e;
	current = pgain*e + igain*ie;			// proportional + integral


	if (stopFlag)  {
		current = 3.0;
	}
	else  {
		if (current<0)  {
			current = 0;
		}
		else if (current > 3)  {
			current = 3.0;
		}

	}
	currentCountToPic = (unsigned short) (.5 + mCurrent*current + bCurrent);
	currentCountToPic = (unsigned short)picCurrentFilter->calc(currentCountToPic);

	if (degaussEnabled)  {
		ergoDegauss();
	}

	if (logging)  {
		logData();
	}
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::cleanup(void)  {
	DEL(log);
	DEL(wfilter);
	DEL(fwfilter1);
	DEL(fwfilter2);
	DEL(accfilter);
#ifdef DO93_STUFF
	DEL(accfilter93);
#endif
#ifdef DOSS
	DEL(ss_acc_filter);
#endif

	DEL(picCurrentFilter);
	DEL(rpmFilter);
	DEL(mphFilter);
	DEL(dwfilter);
	DEL(avgTimer);
	bike = NULL;
	DEL(ini);
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::logData(void)  {

/*
	if (paused)  {
		return;
	}

if (log) 
	log->write("%10ld  %.2lf  %.2lf  %.2lf  %.2lf\n",
		ms,
		frpm, 
		fmph,
		watts,
		displayedWatts
		);
*/

	return;
}

/**********************************************************************

**********************************************************************/

double Physics::lowCurrent(double _rpm, double _watts)  {
	int ilo, ihi;							// index of rpmlo & rpmhi
	int rpmlo, rpmhi;
	int jlo, jhi;							// index of wattslo & wattshi
	double wattslo, wattshi;

	int j;
	double frac1, frac2, ampslo, ampshi, amps;

	if (_rpm<.10)  {
		amps = 0.0;
		goto finis;
	}
	if (_watts < 1.0)  {				// tlm20080709
		amps = 0.0;
		goto finis;
	}

	//---------------------------------------
	// get the watts interval
	//---------------------------------------

	for(j = 1; j<NWATTVALS;j++)  {
		if (_watts>=wattsVals[j-1] && _watts <wattsVals[j])  {
			break;
		}
	}
	if (j==NWATTVALS)  {
		amps = 3.0;
		goto finis;
	}
	jlo = j-1;
	jhi = j;

	wattslo = wattsVals[jlo];
	wattshi = wattsVals[jhi];


	//---------------------------------------
	// get the rpm interval we're in (i)
	//---------------------------------------

	for(j = 1; j<NRPMVALS;j++)  {
		if (_rpm>=rpmVals[j-1] && _rpm <rpmVals[j])  {
			break;
		}
	}

	ilo = j-1;						// ilo is the rpm index
	ihi = j;

	rpmlo = rpmVals[ilo];
	rpmhi = rpmVals[ihi];

	frac1 = (_watts - wattslo ) / (wattshi - wattslo);	// get the watts fraction
	ampslo = lowTable[ilo][jlo] + frac1*(lowTable[ilo][jhi] - lowTable[ilo][jlo]);
	ampshi = lowTable[ihi][jlo] + frac1*(lowTable[ihi][jhi] - lowTable[ihi][jlo]);

	frac2 = (_rpm - rpmlo) / (rpmhi - rpmlo);			// get the rpm fraction
	amps = ampslo + frac2 * (ampshi - ampslo);
	//amps = ampshi + (1-frac2) * (ampslo - ampshi);


	if (amps < 0.0)  {
		amps = 0.0;
	}
	if (amps > 3.0)  {
		amps = 3.0;
	}

finis:
	return amps;
}

/**********************************************************************

**********************************************************************/

double Physics::getAcceleration(void)  {
	return faccel;
}

/**********************************************************************

**********************************************************************/

double Physics::getRPM(void)  {
	return frpm;
}

/**********************************************************************

**********************************************************************/

double Physics::getMPH(void)  {
	return fmph;
}

/**********************************************************************

**********************************************************************/

double Physics::getWatts(void)  {
	return watts;
}

/**********************************************************************

**********************************************************************/

float Physics::getConstantWatts(void)  {
	return constantWatts;
}

/**********************************************************************

**********************************************************************/

double Physics::getDisplayedWatts(void)  {
	return displayedWatts;
}

/**********************************************************************

**********************************************************************/

double Physics::getPicCurrent(void)  {
	return currentCountToPic;
}

/*************************************************************************

*************************************************************************/

void Physics::runWindLoad(void)  {

	if (computeAcceleration())  {
		return;
	}

	if (calibrating)  {
		return;
	}

	decayCounter++;
	if (decayCounter >= decayDelay)  {
		decayCounter = 0;
		if (Q > 1.0)  {
			Q /= decayFactor;
			if (Q < 1.0)  {
				Q = 1.0;
			}
		}
	}
	inertialForce = (mass / Q) * faccel;
#ifdef DOSS
	ss_inertialForce = mass * ss_accel;
#endif

#ifdef DO93_STUFF
	if93 = (LMEF * POUNDSTOSLUGS) * faccel93;
#endif

	if (!ifenabled)  {
		inertialForce = 0.0;
	}
	gradeForce = lbs*grade/100.0;
	tireForce = 1.0;
	totalwind = wind + draftwind;		// tailwind is -, headwind is +, all draftwind is <= 0
	double d = mph+totalwind;
	if (d < 0.0)  {
		d = -(d*d);
	}
	else  {
		d = d*d;
	}
	masterCurveForce = kf * d;				// wind drag in pounds from "master curve"
	force = tireForce + masterCurveForce + inertialForce + gradeForce;
#ifdef DOSS
	ss_force = tireForce + masterCurveForce + ss_inertialForce + gradeForce;
#endif

	/*
#ifdef _DEBUG
	if (mph > 5.0)  {
		bp = 1;
	}
#endif
	*/


	if (force < 0.0)  {
		force = 0.0;
	}
	watts = fps * force * (746.0/550.0);	// get power in watts, this must be the power

	/*
		62 / 14 = 4.43
		circumference = 84.78 inches
		1 rev of pedal = 4.43 revs of flywheel = 376 inches
		data was taken in 'gear 93' (93 inches)
	*/

	double dd = 93.0/bike->state.gear_inches;
	flywheel_watts = fwfilter2->calc(fps*dv*LMEF*dd*dd*(746.0/550.0));			// 70 point smav
	flywheel_watts = fwfilter1->calc(flywheel_watts);									// 10 point smav

#ifdef _DEBUG
	static int cnt = 0;
	cnt++;
	if (watts > .01)  {
		bp = 6;
	}
#endif

#ifdef _DEBUG
	watts = wfilter->calc(watts, 1);
#else
	watts = wfilter->calc(watts);
#endif

#ifdef _DEBUG
	if (watts > .01)  {
		bp = 6;
	}
#endif

	wf_watts = watts;

	//wf_watts = watts = wfilter->calc(watts);
	net_watts = watts - flywheel_watts;

//xxx

#ifdef DO93_STUFF
	//force93 = tireForce + masterCurveForce + if93 + gradeForce;
	if (fps > 0.0)  {
		force93 = (net_watts*550.0 / 746.0 / fps93) + if93;
	}
	else  {
		force93 = 0.0;
	}

#endif

	//watts = wfilter->calc(watts);
	displayedWatts = dwfilter->calc(watts);

//#ifdef _DEBUG
#if 0
	if (displayedWatts > .01)  {
		bp = 6;
	}
#endif

	if (!paused)  {
		//oktobreak = true;
		if (displayedWatts > peakWatts)  {
			peakWatts = displayedWatts;
		}

		if (rpmFromPedals > 0)  {
			sumAverageWatts += displayedWatts;
			averageWattsCount++;
			averageWatts = sumAverageWatts / averageWattsCount;
		}
	}


	if (!paused)  {
		if (rpmFromPedals != 0)  {
			wattseconds += (displayedWatts*.005);
		}

		Calories = (wattseconds / 4186) / .26;		// 1 watt-sec = 1 Joule tlm20060627
		if (rpmFromPedals > 0)  {
			sumAverageRPM += rpmFromHoles;
			averageRPMcount++;
			averageRPM = sumAverageRPM / averageRPMcount;
		}
	}

	if (stopFlag)  {
		current = 3.0;
	}
	else  {
		//xxx
		//getTableCurrent3(lookupRPM, watts);		// computes member var 'current'
		getTableCurrent3(lookupRPM, net_watts);		// computes member var 'current'
	}

	e = current - mamps;			// reverse action
	ie += e;
	current = pgain*e + igain*ie;			// proportional + integral

	if (ifenabled)  {
		if (attackCounter >= attackDelay)  {
			attackCounter = 0;
			if (current >= clippingCurrent)  {
				Q *= attackFactor;
				if (Q > 32.0)  {
					Q = 32.0;
				}
			}
		}
	}


	if (stopFlag)  {
		current = 3.0;
	}
	else  {
		if (current<0)  {
			current = 0;
		}
		else if (current > 3)  {
			current = 3.0;
		}

	}
	currentCountToPic = (unsigned short) (.5 + mCurrent*current + bCurrent);
	currentCountToPic = (unsigned short)picCurrentFilter->calc(currentCountToPic);

	windDegauss();

	if (logging)  {
		logData();
	}

//#ifdef _DEBUG
#if 0
	if (displayedWatts > .01)  {
		bp = 6;
	}
#endif

	return;

}			// runWindLoad()

/*************************************************************************

*************************************************************************/

void Physics::runConstantCurrent(void)  {

	if (computeAcceleration())  {
		current = constantCurrent;
		currentCountToPic = (unsigned short) (.5 + mCurrent*current + bCurrent);
		return;
	}

	current = constantCurrent;
	currentCountToPic = (unsigned short) (.5 + mCurrent*current + bCurrent);
	if (ccd)  {
		windDegauss();
	}

	if (logging)  {
		logData();
	}
	return;
}		// runConstantCurrent()

/**********************************************************************

**********************************************************************/

void Physics::setMode(MODE _mode)  {
	mode = _mode;
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::setConstantCurrent(float _constantCurrent)  {
	mode = CONSTANT_CURRENT;
	constantCurrent = _constantCurrent;
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::writeState(FILE *stream)  {
	int status;
	int i;

	if (stream==NULL)  {
		return;
	}

	i = sizeof(STATE);
	assert(i==256);

	CRF *s = new CRF();
	s->init();

	state.acc = accfilter->getfval();
	state.rpm = rpmFilter->getfval();
	state.mph = mphFilter->getfval();
	state.watts = wfilter->getfval();
	state.displayedWatts = dwfilter->getfval();
	state.grade = grade;

	STATE tmpstate;
	memcpy(&tmpstate, &state, sizeof(STATE));
	s->doo((unsigned char *)&tmpstate, sizeof(STATE));
	status = (int)fwrite(&tmpstate, sizeof(STATE), 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	DEL(s);
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::readState(FILE *stream)  {
	int status;

	if (stream==NULL)  {
		memset(&state, 0, sizeof(STATE));
		return;
	}

	CRF *s = new CRF();
	s->init();

	status = (int)fread(&state, sizeof(STATE), 1, stream);
	if (status != 1)  {
		cleanup();
		throw (fatalError(__FILE__, __LINE__, "Error Reading PS"));
	}

	s->doo((unsigned char *)&state, sizeof(STATE));

	DEL(s);

	faccel = state.acc;
	frpm = state.rpm;
	fmph = state.mph;
	watts = state.watts;
	displayedWatts = state.displayedWatts;
	grade = state.grade;

	accfilter->setfval(faccel);
	rpmFilter->setfval(frpm);
	mphFilter->setfval(fmph);
	wfilter->setfval(watts);
	dwfilter->setfval(displayedWatts);

	return;
}

/**********************************************************************

**********************************************************************/

void Physics::setConstantWatts(float _constantWatts)  {
	mode = CONSTANT_WATTS;
	constantWatts = _constantWatts;
	//monarch_constant_pounds = 0.0;					// for constant torque mode, eg, if we just came out of that mode
	return;
}

/**********************************************************************

**********************************************************************/

void Physics::crowbarRPM(void)  {
	frpm = 0.0;
	rpmFilter->setfval(0.0);
	return;
}



/*************************************************************************************

*************************************************************************************/

void Physics::set_weight_lbs(double _d)  {

	person_lbs = _d;
	lbs = TOPOUNDS*bike->get_kgs() + person_lbs;
	return;
}

/**********************************************************************

**********************************************************************/

double Physics::getTableWatts(double _rpm, double _amps)  {
	double rpmlo, rpmhi;
	float ampslo, ampshi;
	int ilo, ihi;
	int jlo, jhi;

	rpmlo = 10*( ((int)_rpm) / 10);
	ilo = (int) (rpmlo / 10);						// klo is the rpm index

	rpmhi = rpmlo + 10;
	ihi = ilo + 1;

	assert(ilo >= 0 && ilo<=25);
	assert(ihi >= 0 && ihi<=25);

	int i;

	for(i=0; i<NN; i++)  {
		if (ampVals[i] >= _amps)  {
			break;
		}
	}

	if (i>=NN-1)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	jlo = i;
	ampslo = (float)ampVals[jlo];

	jhi = jlo + 1;
	ampshi = (float)ampVals[jhi];

	// now that we have ilo, ihi and jlo, jhi we can do the 2d interpolation to get the answer
	
	double yf, xf;

	yf = (_rpm - rpmlo) / (rpmhi-rpmlo);							// rpm fraction
	xf = (_amps - ampslo) / (ampshi-ampslo);						// amps fraction

	double ywatts1, ywatts2;

	// interpolate between the rows:

	ywatts1 = table[ilo][jlo] + yf*(table[ihi][jlo] - table[ilo][jlo]);

	ywatts2 = table[ilo][jhi] + yf*(table[ihi][jhi] - table[ilo][jhi]);

	double w;

	w = ywatts1 + xf*(ywatts2-ywatts1);

	bp = 1;

	return w;
}


/**********************************************************************
		enum  {
			WIND_LOAD_MODE,
			CONSTANT_TORQUE_MODE,
			CONSTANT_WATTS_MODE,
			CONSTANT_CURRENT_MODE
		};

**********************************************************************/

void Physics::set_grade(float _f)  {
	setMode(WINDLOAD);
	grade = _f;
	return;
}

/*************************************************************************

*************************************************************************/

void Physics::set_constant_force(double _constant_pounds)  {										// this is the dropped weight
	constant_pounds = _constant_pounds;
	constant_kgs = (float)(POUNDSTOKGS * constant_pounds);
//constant_kgs = 1.0;

	monarch.set_dropped_weight(constant_kgs);

	mode = CONSTANT_TORQUE;

	DEL(rpmFilter);
	rpmFilter = new LPFilter(200);		// using a different rpm filter for wingate
	rpmFilter->setfval(frpm);

	DEL(accfilter);

	int itemp;

	if (ini)  {
		itemp = ini->getInt("options", "wingate acceleration filtering", "35", false);
	}
	else  {
		itemp = 35;
	}

	accfilter = new LPFilter(itemp);			// using a new acceleration filter for wingate
	accfilter->setfval(faccel);

	return;
}						// set_constant_force(double _monarch_force)

/**********************************************************************

**********************************************************************/

void Physics::run_constant_force(void)  {
	double alpha = 0.0;
	static double lastomega = 0.0;
	static bool first = true;
	static double t = 0.0;


	computeAcceleration();					// used to compute lookupRPM, faccel

	// rpmFromHoles = instantaneous rpm
	// lookupRPM = rpmLookupFactor * rpmFromHoles; (a constant * instanteous rpm)
	// frpm = filtered rpmFromHoles

	double flywheelRPM = rpmFromHoles * bike->state.params.real_front_teeth / 14;				// instantaneous real flywheel rpm
	double omega = 2.0 * PI * flywheelRPM / 60.0;										// instantaneous radians per second
	//double monarch_watts;

//frpm = 60;
	watts = ( (52.0/14.0)*2.0*PI*monarch.radius)*9.8*constant_kgs*(frpm/60.0);			// 58.8
	//double d = DYNO_ARM_LENGTH * INCHESTOMETERS;			// 1.08 meters

	fish_scale_reading = monarch.dropped_weight * monarch.radius * (52.0/14.0) / (DYNO_ARM_LENGTH * INCHESTOMETERS);
	fish_scale_reading *= KGSTOPOUNDS;


	if (first)  {
		omega_0 = omega;
		lastomega = omega;
		t = 0.0;
		first = false;
		return;
	}
		
	t += dt;

	alpha = (omega-lastomega) / dt;
	lastomega = omega;

	omega = omega_0 - alpha * t;
	inertialForce = POUNDSTOSLUGS * LMEF  * faccel;

	//watts = torqueNM / dt;
	//watts = fps * (monarch_constant_pounds + inertialForce) * (746.0/550.0);

	if (watts < 0.0)  {
		watts = 0.0;
	}
	displayedWatts = dwfilter->calc(watts);

	if (watts > peakWatts)  {
		peakWatts = watts;
	}

	sumAverageWatts += displayedWatts;
	averageWattsCount++;
	averageWatts = sumAverageWatts / averageWattsCount;

	sumAverageRPM += rpmFromHoles;
	averageRPMcount++;
	averageRPM = sumAverageRPM / averageRPMcount;

	if (stopFlag)  {
		current = 3.0;
	}
	else  {
		getTableCurrent3(lookupRPM, watts);		// computes member var 'current'
	}

	if (stopFlag)  {
		current = 3.0;
	}
	else  {
		if (current<0)  {
			current = 0;
		}
		else if (current > 3)  {
			current = 3.0;
		}
	}
	currentCountToPic = (unsigned short) (.5 + mCurrent*current + bCurrent);
	currentCountToPic = (unsigned short)picCurrentFilter->calc(currentCountToPic);		// 5

	/*
	if (logging)  {
		log->write("%.2lf  %.2lf\n",
			inertialForce,
			current
		);
	}
	*/

	return;
}							// run_constant_force()

