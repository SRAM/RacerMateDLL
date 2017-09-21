#ifndef _CANNONDALE_H_
#define _CANNONDALE_H_



struct TrainerData	{
	float kph;			// ALWAYS in KPH, application will metric convert. <0 on error
	float cadence;		// in RPM, any number <0 if sensor not connected or errored.
	float HR;			// in BPM, any number <0 if sensor not connected or errored.
	float Power;		// in Watts <0 on error
};

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

struct SSDATA	{
	float ss;
	float lss;
	float rss;
	float lsplit;
	float rsplit;
};

enum  {
	ALL_OK = 0,
	DEVICE_NOT_RUNNING = INT_MIN,			// 0x80000000
	WRONG_DEVICE,							// 0x80000001
	DIRECTORY_DOES_NOT_EXIST,
	DEVICE_ALREADY_RUNNING,
	BAD_FIRMWARE_VERSION,
	VELOTRON_PARAMETERS_NOT_SET,
	BAD_GEAR_COUNT,
	BAD_TEETH_COUNT,
	PORT_DOES_NOT_EXIST,
	PORT_OPEN_ERROR,
	PORT_EXISTS_BUT_IS_NOT_A_TRAINER,
	DEVICE_RUNNING,
	BELOW_UPPER_SPEED,
	ABORTED,
	TIMEOUT,
	BAD_RIDER_INDEX,
	DEVICE_NOT_INITIALIZED,
	CAN_NOT_OPEN_FILE,
	GENERIC_ERROR
};

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


	
#define get_errstr                     x2118619319b7fb36d1cf6bde9678ac12
#define GetRacerMateDeviceID           xbb66c93359ecb5135514282824f98949
#define Setlogfilepath                 x74c9025b3f7ab7bc6230b74976d7c7bd
#define Enablelogs                     x02af87953982bfc4c62fdac243868fc9
#define GetFirmWareVersion             x40754014802a450df95743b33d591d32
#define get_dll_version                x81034afb392e89c38423be075652617d
#define GetAPIVersion                  xe82fbf83a8831c8c12c5acdf70251584
#define check_for_trainers             x421b6e27c30ae2ca08e4f3bfb7987def
#define GetIsCalibrated                xb66e38c42787dd9c3b8e6c5205bcd911
#define GetCalibration                 x1145f89e37cec8227eb4d2a785bc2d98
#define resetTrainer                   xf06510dbbd22261f5cfc03064e95e2e0
#define startTrainer                   x65a14bacf33e2ef037a154e0e87b2f16
#define ResetAverages                  xf3c1a99e992cf6666024d7725446cd89
#define GetTrainerData                 x91a1c1bc933e01f94e4db2cc50e46bbd
#define get_average_bars               xc0e4d1dbbc943ebce556d112cf951d47
#define get_ss_data                    x25dc2c270d1807359bad95e7fc428760
#define SetSlope                       xf504076488f979f8f8d465374ef77920
#define setPause                       xe59c3144df15a614c5745070c5236ffd
#define stopTrainer                    xf4dac05a99cf4d5c6377c8e96b28617a
#define ResettoIdle                    xd037666f9d225a93b2b30ec21727e7a9

//-------------------------------
// prototypes:
//-------------------------------

__declspec(dllexport) const char *x2118619319b7fb36d1cf6bde9678ac12(int);
__declspec(dllexport) EnumDeviceType xbb66c93359ecb5135514282824f98949(int);
__declspec(dllexport) int x74c9025b3f7ab7bc6230b74976d7c7bd(const char *);
__declspec(dllexport) int x02af87953982bfc4c62fdac243868fc9(bool, bool, bool, bool, bool, bool);
__declspec(dllexport) int x40754014802a450df95743b33d591d32(int);
__declspec(dllexport) const char *x81034afb392e89c38423be075652617d(void);
__declspec(dllexport) const char *xe82fbf83a8831c8c12c5acdf70251584(void);
__declspec(dllexport) EnumDeviceType x421b6e27c30ae2ca08e4f3bfb7987def(int);
__declspec(dllexport) bool xb66e38c42787dd9c3b8e6c5205bcd911(int, int);
__declspec(dllexport) int x1145f89e37cec8227eb4d2a785bc2d98(int);
__declspec(dllexport) int xf06510dbbd22261f5cfc03064e95e2e0(int, int, int);
__declspec(dllexport) int x65a14bacf33e2ef037a154e0e87b2f16(int);
__declspec(dllexport) int xf3c1a99e992cf6666024d7725446cd89(int, int);
__declspec(dllexport) struct TrainerData x91a1c1bc933e01f94e4db2cc50e46bbd(int, int);
__declspec(dllexport) float *xc0e4d1dbbc943ebce556d112cf951d47(int, int);
__declspec(dllexport) struct SSDATA x25dc2c270d1807359bad95e7fc428760(int, int);
__declspec(dllexport) int xf504076488f979f8f8d465374ef77920(int, int, int, float, float, int, float);
__declspec(dllexport) int xe59c3144df15a614c5745070c5236ffd(int, bool);
__declspec(dllexport) int xf4dac05a99cf4d5c6377c8e96b28617a(int);
__declspec(dllexport) int xd037666f9d225a93b2b30ec21727e7a9(int);

#endif				// #ifdef _CANNONDALE_H_

