#ifndef _TRINERD_H_
#define _TRINERD_H_



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


	
#define get_errstr                     xd29718a410de4e24bc2b7c80699fd68c
#define GetRacerMateDeviceID           x379c4334f20ad0925b2f1836e4357fe5
#define Setlogfilepath                 x8a81d30a79579095f53eb6ff62848424
#define Enablelogs                     x980f79149138b9da21365f6498e1b5d9
#define GetFirmWareVersion             x80172ec06c7141ec8b6a4a011ebb01bc
#define get_dll_version                xd906a4757e9b8883d7319d98457d0c17
#define GetAPIVersion                  xc74f13e9f08b035e559cd55e2e77c62f
#define check_for_trainers             x71853b1064fe1699d8b098ce24cedd7c
#define GetIsCalibrated                x5911f1a9d9182b4f600f076f5322244c
#define GetCalibration                 xd4bb8d206ab94cf212992c363a51b59a
#define resetTrainer                   x0356ea7a367a0811610173b702de5395
#define startTrainer                   x638ba5f3f83074353b7b7f2e88a7999a
#define ResetAverages                  x5c4ffecd0365378c65734951ca534c22
#define GetTrainerData                 x4c0a3bcbb8b9039878827394cd5b3421
#define get_average_bars               x30a6028d6ae2a8812cd2372ab5abac32
#define get_ss_data                    x7675ecc3ba190d0b3c0cda28947907ee
#define SetSlope                       xe73b9a7c2930d91563bd79d6848f41f5
#define setPause                       x5c677ef01daf9f602d4f37997fa9a63b
#define stopTrainer                    xda524892cbb781cbb499eb0ed3012288
#define ResettoIdle                    xc758c0ea78f3b57ffa6cc33c1afd4550
#define GetHandleBarButtons            x77775df056d6e9f22eb42164217d4a11
#define SetErgModeLoad                 xfd1c9725aa44336449e6372cfd102f9c
#define SetHRBeepBounds                x427f4cb3bdf4c6dcd1cc72636c689135

//-------------------------------
// prototypes:
//-------------------------------

__declspec(dllexport) const char *xd29718a410de4e24bc2b7c80699fd68c(int);
__declspec(dllexport) EnumDeviceType x379c4334f20ad0925b2f1836e4357fe5(int);
__declspec(dllexport) int x8a81d30a79579095f53eb6ff62848424(const char *);
__declspec(dllexport) int x980f79149138b9da21365f6498e1b5d9(bool, bool, bool, bool, bool, bool);
__declspec(dllexport) int x80172ec06c7141ec8b6a4a011ebb01bc(int);
__declspec(dllexport) const char *xd906a4757e9b8883d7319d98457d0c17(void);
__declspec(dllexport) const char *xc74f13e9f08b035e559cd55e2e77c62f(void);
__declspec(dllexport) EnumDeviceType x71853b1064fe1699d8b098ce24cedd7c(int);
__declspec(dllexport) bool x5911f1a9d9182b4f600f076f5322244c(int, int);
__declspec(dllexport) int xd4bb8d206ab94cf212992c363a51b59a(int);
__declspec(dllexport) int x0356ea7a367a0811610173b702de5395(int, int, int);
__declspec(dllexport) int x638ba5f3f83074353b7b7f2e88a7999a(int);
__declspec(dllexport) int x5c4ffecd0365378c65734951ca534c22(int, int);
__declspec(dllexport) struct TrainerData x4c0a3bcbb8b9039878827394cd5b3421(int, int);
__declspec(dllexport) float *x30a6028d6ae2a8812cd2372ab5abac32(int, int);
__declspec(dllexport) struct SSDATA x7675ecc3ba190d0b3c0cda28947907ee(int, int);
__declspec(dllexport) int xe73b9a7c2930d91563bd79d6848f41f5(int, int, int, float, float, int, float);
__declspec(dllexport) int x5c677ef01daf9f602d4f37997fa9a63b(int, bool);
__declspec(dllexport) int xda524892cbb781cbb499eb0ed3012288(int);
__declspec(dllexport) int xc758c0ea78f3b57ffa6cc33c1afd4550(int);
__declspec(dllexport) int x77775df056d6e9f22eb42164217d4a11(int, int);
__declspec(dllexport) int xfd1c9725aa44336449e6372cfd102f9c(int, int, int, float);
__declspec(dllexport) int x427f4cb3bdf4c6dcd1cc72636c689135(int, int, int, int, bool);

#endif				// #ifdef _TRINERD_H_

