
#ifndef _QT_TDEFS_H_
#define _QT_TDEFS_H_

#include <vector>

//#include <qt_defines.h>
//#include <qt_person.h>

namespace MYQT  {

//#define TEMP222
//#define DO_RTD_SERVER

#define GRXSIZE 512
#define PACKETLEN 10
#define TENBIT
#define NEWERGO

// now RMP:: in tlib uses this structure
/*
struct TrainerData	{
	 float kph;			// ALWAYS in KPH, application will metric convert. <0 on error
	 float cadence;		// in RPM, any number <0 if sensor not connected or errored.
	 float HR;			// in BPM, any number <0 if sensor not connected or errored.
	 float Power;		// in Watts <0 on error
	 //bool tdc;			// accumulated tdc
};
*/

#define NEWUSER

// !!!!!!!!!!!!!!!!!!!! MAKE SURE THAT THE GLOBAL const char *devstrs[] (in ergvid.cpp) agrees with this enum !!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!! MAKE SURE THAT THE GLOBAL const char *devstrs[] (in ergvid.cpp) agrees with this enum !!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!! MAKE SURE THAT THE GLOBAL const char *devstrs[] (in ergvid.cpp) agrees with this enum !!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!! MAKE SURE THAT THE GLOBAL const char *devstrs[] (in ergvid.cpp) agrees with this enum !!!!!!!!!!!!!!

/*
enum EnumDeviceType {
	 DEVICE_NOT_SCANNED,					// unknown, not scanned
	 DEVICE_DOES_NOT_EXIST,				// serial port does not exist
	 DEVICE_EXISTS,							// exists, openable, but no RM device on it
	 DEVICE_COMPUTRAINER,
	 DEVICE_VELOTRON,
	 DEVICE_SIMULATOR,
	 DEVICE_RMP,
	 DEVICE_ACCESS_DENIED,				// port present but can't open it because something else has it open
	 DEVICE_OPEN_ERROR,					// port present, unable to open port
	 DEVICE_OTHER_ERROR					// prt present, error, none of the above
};
*/

enum EnumTrainerType {
	 COMPUTRAINER,
	 VELOTRON,
	 SIMULATOR
};


enum Special_Devices {
	 PERF_DEVICE=254,
	 SIMULATOR_DEVICE						// 255
};


enum LoggingType {
	 NO_LOGGING,
	 RAW_PERF_LOGGING,
	 THREE_D_PERF_LOGGING,
	 RM1_PERF_LOGGING
};

#pragma pack(push, 1)						// for old borland compatibility
	 typedef struct  {
		  unsigned char gear;
		  unsigned char frontGear;
		  unsigned char rearGear;
		  unsigned char fill;									// to get a 4-byte structure
	 } GEARINFO;
#pragma pack(pop)

#define MAX_EXPORT_ITEMS 19

#define COMMSTRING "NO COMMUNICATION DETECTED"
#define OFFSTRING "OFF"

#define MINWEIGHT 30
#define MAXWEIGHT 350

#define VELOTRONMAXCHANS 7
#define HBMAXCHANS 10

/*
enum  {
	 IDLE,
	 HALT
};
*/

enum  {
	 SOURCE_VELOTRON,
	 SOURCE_COMPUTRAINER,
	 SOURCE_VELOTRON_FILE,
	 SOURCE_CDF_FILE,
	 SOURCE_JIM_FILE,
	 SOURCE_PACER
};

#define BACKGROUND_COLOR  RGB(0, 0, 100)
#define STATUS_BACKGROUND RGB(0, 200, 200)
#define STATUS_FOREGROUND RGB( 0, 0, 0)

#define WFILTERCONSTANT 1200

//#define NEWMETA

#ifdef NEWMETA
typedef struct  {
	 float kph;
	 float watts;
	 float hr;
	 float rpm;
	 float meters;
	 unsigned char keys;					// the 6 hb keys + the polar heartrate bit
} METADATA;

#else
/*
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
*/

#endif


	 typedef struct  {
		  unsigned short bars[72];							// stored as 10 * torque
		  unsigned char tdc;

		  float miles;							// distance in miles
		  unsigned long ms;						// time in milliseconds

		  // 0 - 255 rpm

		  unsigned char rpm;
		  unsigned char pedalrpm;
		  unsigned char avgrpm;
		  unsigned char maxrpm;

		  // 0 - 255 hr

		  unsigned char hr;
		  unsigned char avghr;
		  unsigned char maxhr;

		  // watts: 0 - 65535 watts

		  unsigned short watts;
		  unsigned short avgwatts;
		  unsigned short maxwatts;

		  // 0 - 6553.5 mph: stored as mph * 10

		  unsigned short mph;
		  unsigned short avgmph;
		  unsigned short maxmph;

		  short grade;			// -10.0 to +25.0, stored as grade*10
		  short wind;				// -3200.0 + 3200.0, stored as wind_mph*10
		  unsigned char gear;	// 0 - 255
		  unsigned char frontgear;
		  unsigned char reargear;

		  unsigned char ss;
		  unsigned char lss;
		  unsigned char rss;
		  unsigned char avgss;
		  unsigned char avglss;
		  unsigned char avgrss;

		  unsigned char lpower;
		  unsigned char rpower;

		  unsigned char inzone_hr;		// 0 - 100
		  unsigned short pounds;			// 0 - 6553.5 pounds, stored as pounds*10

		  float pp;
		  float calories;

		  unsigned char lata;
		  unsigned char rata;

	 } VEL_LOGPACKET;			// 128 bytes


#define KEY1		0x02
#define KEY2		0x04
#define KEY3		0x08
#define KEY4		0x10
#define KEY5		0x20
#define KEY6		0x40

enum  {
	 REALTIME,
	 FILEMODE
};

typedef struct  {
	 float meters;
	 float grade;						// this is %grade (-15.00 to +15.00)
	 float wind_kph;
} CRSLEG;

#include <string>
#include <vector>

#define MAX_MULTI_RIDERS 8

#define MULTIPLE_SCREENS

typedef struct  {
	 int id;						// 1-8, the physical position of the rider
	 float sortmiles;			// arrays get sorted by this
	 bool finished;				// true if the rider has finished (means to quit updating the sortmiles)
} POS;

enum  {
	 NORMAL_SCORING,
	 FASTEST_AVERAGE,
	 FASTEST_SECOND,
	 FASTEST_THIRD,
	 FASTEST_FOURTH
};

enum  {
	 Invalid,
	 Human,
	 PacerHeartRate,
	 PacerPower,
	 PacerFixedPower,
	 PacerTimeDelay,
	 PacerFixedSpeed,
	 PacerRoadRacer,
	 PacerSavedPerformance
};

/*
enum PHYSICS_MODE  {
	 PHYSICS_WIND_LOAD_MODE,					// old WINDLOAD mode
	 PHYSICS_CONSTANT_TORQUE_MODE,
	 PHYSICS_CONSTANT_WATTS_MODE,			// old ERGO mode
	 PHYSICS_CONSTANT_CURRENT_MODE
};


enum  MODE {
	 ERGO,
	 WINDLOAD,
	 NOMODE
};											// mode
*/

typedef struct  {
	 float length;
	 float grade;
	 float wind;
	 float accumMiles;
	 float startel;
	 float endel;
	 float minutes;
	 float dm;
	 float load;
} REGULARLEG;

enum SecType {
	 //extern const char *sec_type_str[8];			// should agree with SecType!!!

	 SEC_PERF_FILE_HEADER,
	 SEC_PERSON_INFO,
	 SEC_RIDER_INFO,
	 SEC_RESULT,
	 SEC_TIME,											// time in ms... size should be same as the number of records of perf points
	 SEC_COURSE,
	 SEC_COMPUTRAINER_RAW_DATA,
	 SEC_VELOTRON_RAW_DATA,
	 SEC_NONE
};

#pragma pack(push, 1)
	 struct SEC {
		  SEC(void)  {
				type = SEC_NONE;
				structsize = 0L;
				version = 0L;
				n = 0L;
				struct_start_offset = 0L;
				data_start_offset = 0L;
				data_size = 0L;
				encrypted = false;
		  }

		  SecType type;
		  unsigned long structsize;						// size of the header struct, eg sizeof(COURSE_HEADER)
		  unsigned long version;
		  unsigned long n;								// number of perfpoints or the number of course FPOINTS
		  unsigned long struct_start_offset;

		  unsigned long data_start_offset;				// if there is data (perf and course, eg)
		  unsigned long data_size;						// if there is data
		  bool encrypted;									// whether the rest of the structure is encrypted
	 };

	 /*
	 typedef struct  {
		  unsigned long offset;
		  SecType type;
		  unsigned long size;								// size of the entire file section, not the header size!
	 } SEC2;
	 */

	 typedef struct  {
		  SEC sec;
	 } PERF_FILE_HEADER;									// sizeof(RAWHEADER) = 1024

	 typedef struct  {
		  SEC sec;
		  char name[MAX_PATH];							// name of course or description
		  char video[MAX_PATH];							// the video name that goes with this course
	 } COURSE_HEADER;


	 typedef struct  {
		  SEC sec;
	 } TIME_HEADER;

	 // constant size structures that get written to raw performance files

	 typedef struct  {
		  SEC sec;
		  double age;
		  double kgs;
		  char sex;
		  char lastname[40];
		  char firstname[40];
		  char username[40];
		  char email[64];
		  char city[24];
		  char country[24];
		  char state[16];
		  char zip[16];
	 } PERSON_INFO;

	 typedef struct  {
		  SEC sec;
		  PERSON_INFO pinf;
		  float lower_hr;
		  float upper_hr;
		  bool hr_beep_enabled;
		  bool draft_enable;
		  float drag_aerodynamic;
		  float drag_tire;
		  unsigned short rfdrag;
		  float watts_factor;
		  float ftp;
	 } RIDER_INFO;

	 struct RESULT {
		  RESULT(void)  {
				id = 0;
				year = month = day = 0;
				uid = 0;
				rank = 0;
				gender = 'M';
				age = 0.0f;
				kgs = 0.0f;
				test_kgs = 0.0f;
				bike_kgs = 0.0f;

				ms = 0L;								// elapsed ms
				average_kph = 0.0f;
				max_kph = 0.0f;
				average_watts = 0.0f;
				max_watts = 0.0f;
				average_wpkg = 0.0f;
				average_ss = 0.0f;
				max_ss = 0.0f;

				memset(edate, 0, sizeof(edate));

				memset(username, 0, sizeof(username));
				memset(lname, 0, sizeof(lname));
				memset(fname, 0, sizeof(fname));
				memset(email, 0, sizeof(email));

				memset(test_city, 0, sizeof(test_city));
				memset(test_state, 0, sizeof(test_state));
				memset(test_country, 0, sizeof(test_country));
				memset(test_zip, 0, sizeof(test_zip));
				memset(test_site, 0, sizeof(test_site));
		  }

		  int id;
		  int uid;
		  int rank;

		  char gender;
		  float age;				// birthdate converted to age, eg: 27.356
		  int year;				// birth year
		  int month;				// birth month
		  int day;				// birth day
		  float kgs;				// body weight of the rider. Does not include bike weight.
		  float bike_kgs;
		  float test_kgs;			// body weight of the rider during the actual test (might have changed). Does not include bike weight.

		  unsigned long ms;
		  float average_kph;
		  float max_kph;
		  float average_watts;
		  float max_watts;
		  float average_wpkg;
		  float average_ss;
		  float max_ss;
		  float test_cal;

		  float average_rpm;
		  float max_rpm;
		  int cal;									// press on force

		  char edate[20];

		  char username[40];
		  char lname[40];
		  char fname[40];
		  char email[128];
		  char test_city[40];
		  char test_state[20];
		  char test_country[32];
		  char test_zip[20];
		  char test_site[256];			// Race Site Designation by Home, MultiRider Center
		  SEC sec;
	 };							// struct RESULT {

	 /*
	 struct TTRESULT {
		  TTRESULT(void)  {
				id = 0;
				uid = 0;
				ms = 0L;								// elapsed ms
				average_kph = 0.0f;
				max_kph = 0.0f;
				average_watts = 0.0f;
				max_watts = 0.0f;
				average_wpkg = 0.0f;
				average_ss = 0.0f;
				max_ss = 0.0f;
				memset(edate, 0, sizeof(edate));
		  }
		  int id;
		  int uid;
		  unsigned long ms;
		  float average_kph;
		  float max_kph;
		  float average_watts;
		  float max_watts;
		  float average_wpkg;
		  float average_ss;
		  float max_ss;
		  char edate[20];
	 };							// struct TTRESULT {
	 */

#pragma pack(pop)

#ifdef _DEBUG
	 #ifdef WIN32
	 #define COMPUTRAINER_PORT1 1				// real serial port (with the bike)
	 #define VELOTRON_PORT 10					// usb serial port on desktop
	 #define COMPUTRAINER_PORT2 11				// usb serial port on desktop
	 #define COMPUTRAINER_PORT3 35				// serial port on laptop
	 #else
	 #define COMPUTRAINER_PORT1 1				// real serial port (with the bike)
	 #define VELOTRON_PORT 1					// usb serial port on desktop
	 #define COMPUTRAINER_PORT2 1				// usb serial port on desktop
	 #define COMPUTRAINER_PORT3 1				// serial port on laptop
	 #endif
#else
	 #ifdef WIN32
		  #define COMPUTRAINER_PORT3 35				// serial port on laptop
	 #endif
#endif

// from rm1:

enum CourseType  {
	 Zero = 0,
	 Distance = 0x01,
	 Watts = 0x02,
	 Video = 0x04,
	 ThreeD = 0x08,
	 GPS = 0x10,
	 //Performance = 0x20
	 Perf = 0x20
};

#define DEFAULT_TCP_PORT 9072
#define SERVER_SERIAL_PORT_BASE 201							// 201 - 216
#define CLIENT_SERIAL_PORT_BASE 231							// 231 - 246
}

#endif		// _TDEFS_H_