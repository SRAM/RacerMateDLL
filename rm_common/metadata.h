#ifndef _METADATA_H_
#define _METADATA_H_

typedef struct  {
	float bars[24];								// stored as 10 * torque
	float mph;
	float watts;
	float hr;
	float rpm;
	float averageMPH;
	float peakMPH;


	unsigned short rfdrag;
	unsigned short rfmeas;
	unsigned long time;
	float ss;
	float lss;
	float rss;
	float lwatts;
	float rwatts;
	float pp;							// pulse power
	int lata;
	int rata;
	float miles;						// was miles
	float feet;
	unsigned short hrflags;
	unsigned short rpmflags;
	unsigned short minhr;				// what the handlebar sends back! meta.lower_hr is the setting sent to the hb
	unsigned short maxhr;				// what the handlebar sends back! meta.upper_hr is the setting sent to the hb
	float grade;							// -100.00 to +100.00
	float wind;
	float averageWatts;
	float averageRPM;
	float averageHR;
	float averagePP;

	float inZoneHr;

	float peakWatts;
	float peakHR;
	float peakRPM;
	float peakPP;

	float load;
	float faccel;
	float rawAcceleration;
	unsigned char tdc;
	unsigned char hrNoiseCode;
	unsigned short mcur;
	unsigned short volts;
	float rawRPM;
	float calories;
	float pedalrpm;

	unsigned char runSwitch;
	unsigned char brakeSwitch;

	unsigned char keys;					// the 6 hb keys + the polar heartrate bit
	unsigned char keydown;
	unsigned char keyup;
	unsigned char spareChar;
	float minutes;
	unsigned char gear;					// 0 - 255 (gear inches)
	unsigned char rpmValid;
	unsigned char virtualFrontGear;
	unsigned char virtualRearGear;

	float average_lss;
	float average_rss;
	float average_ss;
	//char spare2[5];
	bool rescale;								// spinscan rescale flag
	float maxforce;							// spinscan force to scale to if rescale flag is set
} METADATA;

#endif


