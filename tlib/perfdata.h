    
#ifndef _PERFDATA_H2_
#define _PERFDATA_H2_

//#include <defines.h>
#ifdef WIN32
	#include <Windows.h>
#endif

#include <string>

#include <flags.h>


#pragma pack(push, 1)

    //struct PerfData  {                           // packed
class PerfData  {
	public:
		// always set for data
		UINT64 StatFlags;
		UINT64 TimeMS;
		INT32 KeyFrame; // not set for info
		// data that may not be set
		double TimeAcc;
		double Distance;
		INT32 Lap;
		float LapTime;
		float Lead;
		float Grade;
		float Wind;
		float Speed;
		float Speed_Avg;
		float Speed_Max;
		float Watts;
		float Watts_Avg;
		float Watts_Max;
		float Watts_Wkg;
		float Watts_Load;
		float HeartRate;
		float HeartRate_Avg;
		float HeartRate_Max;
		float Cadence;
		float Cadence_Avg;
		float Cadence_Max;
		float Calories;
		float PulsePower;
		float DragFactor;
		float SS;
		float SSLeft;
		float SSRight;
		float SSLeftSplit;
		float SSRightSplit;
		float SSLeftATA;
		float SSRightATA;
		float SSLeft_Avg;
		float SSRight_Avg;
		float LeftPower;
		float RightPower;
		float PercentAT;
		INT32 FrontGear;
		INT32 RearGear;
		INT32 GearInches;
		float RawSpinScan;
		float CadenceTiming;
		float TSS;
		float IF;
		float NP;
		INT32 Bars_Shown; // bool
		float* Bars; // 24
		float* AverageBars; // 24

		INT32 CourseScreenX;
		std::string RiderName;
		std::string Course;

		float SS_Stats; // = SS | SSLeft | SSRight | SSLeftSplit | SSRightSplit | SSLeftATA | SSRightATA | SSLeft_Avg | SSRight_Avg,
		float Gear; // = FrontGear | RearGear,
		float TSS_IF_NP; // = TSS | IF | NP,

		// added for header version - v1.02
		bool Drafting;
		bool Disconnected;
		INT16 RawCalibrationValue;
		//bool HardwareStatus;

		PerfData(void);
		~PerfData();
	};

#pragma pack(pop)

#endif			// #ifndef _PERFDATA_H2_



