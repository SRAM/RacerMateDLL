#ifndef _PUBLIC_H_
#define _PUBLIC_H_

/*
struct TrainerData	{
	float kph;			// ALWAYS in KPH, application will metric convert. <0 on error
	float cadence;		// in RPM, any number <0 if sensor not connected or errored.
	float HR;			// in BPM, any number <0 if sensor not connected or errored.
	float Power;		// in Watts <0 on error
};
*/

// MAKKE SURE THAT devstrs IN DLL_GLOBALS.CPP MATCHES THIS!
//#ifndef EnumDeviceType
enum EnumDeviceType {
	DEVICE_NOT_SCANNED,					// unknown, not scanned
	DEVICE_DOES_NOT_EXIST,				// serial port does not exist
	DEVICE_EXISTS,							// exists, openable, but no RM device on it
	DEVICE_COMPUTRAINER,
	DEVICE_VELOTRON,
	DEVICE_SIMULATOR,
	DEVICE_RMP,
	//DEVICE_WIFI,
	DEVICE_ACCESS_DENIED,				// port present but can't open it because something else has it open
	DEVICE_OPEN_ERROR,					// port present, unable to open port
	DEVICE_NO_URL_AND_OR_TCP_PORT,
	DEVICE_SERVER_NOT_RUNNING,
	DEVICE_OTHER_ERROR					// prt present, error, none of the above
};
//#endif

#define DEVICE_PERF_FILE DEVICE_RMP		// tlm20130829, for dll_vs2008 compatibility
#endif


