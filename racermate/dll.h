#ifndef _DLL_H_
#define _DLL_H_

#define WIN32_LEAN_AND_MEAN

#include <glib_defines.h>

#include <ctsrv.h>
#include <antdefs.h>


#include <public.h>
#include <physics.h>
#include <datasource.h>
#include <decoder.h>
#include <trainerdata.h>
#include "ev.h"

#include <glib_defines.h>
#include "dlldefs.h"
#include "levels.h"
#include "versions.h"

extern "C" {
	//DLLSPEC int check_device_keys(void);

#if (LEVEL >= DLL_CANNONDALE_ACCESS)
	#ifdef DO_PI
		DLLSPEC int set_port_info(int, const char *, int, int);
	#endif
	DLLSPEC const char *get_udpkey(const char * _id);

	DLLSPEC const char *get_errstr(int) throw(...);
	DLLSPEC inline struct RACERMATE::TrainerData GetTrainerData(const char *, int) throw(...);
	DLLSPEC inline struct SSDATA get_ss_data(const char *, int) throw(...);
	DLLSPEC EnumDeviceType GetRacerMateDeviceID(const char *) throw(...);
	DLLSPEC int GetFirmWareVersion(const char *);						// optional
	DLLSPEC int get_status_bits(const char *, int);

	DLLSPEC int startTrainer(const char *);
	DLLSPEC int SetSlope(const char *, int, int, float, float, int, float);
	DLLSPEC bool GetIsCalibrated(const char *, int);
	DLLSPEC int GetCalibration(const char *);
	DLLSPEC int resetTrainer(const char *, int, int);
	DLLSPEC int ResetAverages(const char *, int);
	DLLSPEC inline float *get_average_bars(const char *, int);
	DLLSPEC int setPause(const char *, bool);
	DLLSPEC int stopTrainer(const char *);
	DLLSPEC int ResettoIdle(const char *);
	DLLSPEC int Setlogfilepath(const char *);
	DLLSPEC int Enablelogs(bool, bool, bool, bool, bool, bool);
	DLLSPEC const char *get_dll_version(void);
	DLLSPEC const char *GetAPIVersion(void);
	DLLSPEC const char *get_build_date(void);
	DLLSPEC int racermate_init(void);
	DLLSPEC int start_server(int, int=-1, const char * = NULL, int=0);							// listen port, broadcast port, debug_level
#ifdef NEWANT
	DLLSPEC int start_ant(int);
	DLLSPEC int stop_ant(void);
	DLLSPEC const char * get_ant_sensors_string(void);
	DLLSPEC const SENSORS *get_ant_sensors(void);
	DLLSPEC unsigned char get_ant_hr(unsigned short _sn);
	//DLLSPEC int scan_for_ant_devices(void);
	DLLSPEC int associate(const char *_portname, const unsigned short _sn);
	DLLSPEC int unassociate(const char *_portname, const unsigned short _sn);
#endif


	DLLSPEC int stop_server(void);
	DLLSPEC int racermate_close(void);
	DLLSPEC EnumDeviceType get_trainer_type(int);

	DLLSPEC const char * get_rs232_ports(void) throw(...);
	DLLSPEC const char * get_udp_trainers(void) throw(...);
	DLLSPEC const char * getPortNames(void) throw(...);


	DLLSPEC int get_n_udp_clients(void);
	DLLSPEC int get_n_devices(void);
	//DLLSPEC const char * get_udp_key(const char *_appkey);					// use get_udpkey()!

	//DLLSPEC int set_network_parameters(int, int, bool, bool, const char *, int);				// _debug_level
	#ifdef _DEBUG
		DLLSPEC const char *get_state(int);
	#endif
#endif								// #if (LEVEL >= DLL_CANNONDALE_ACCESS)

#if (LEVEL >= DLL_TRINERD_ACCESS)
	DLLSPEC inline int GetHandleBarButtons(const char *, int);
	DLLSPEC int SetErgModeLoad(const char *, int, int, float);
	DLLSPEC int SetHRBeepBounds(const char *, int, int, int, bool);
	DLLSPEC int SetRecalibrationMode(const char *, int);
	DLLSPEC int EndRecalibrationMode(const char *, int);
	DLLSPEC int get_computrainer_mode(const char *);
#endif									// #if (LEVEL >= DLL_TRINERD_ACCESS)

#if (LEVEL >= DLL_ERGVIDEO_ACCESS)
	DLLSPEC int SetVelotronParameters(const char *, int, int, int, int*, int*, float, int, int, float, int, int );
	DLLSPEC struct Bike::GEARPAIR GetCurrentVTGear(const char *, int) throw(...);
	DLLSPEC int setGear(const char *, int, int, int);
	DLLSPEC inline float *get_bars(const char *, int);
#ifdef _DEBUG
	DLLSPEC const RACERMATE::UDPClient::COMSTATS *get_comstats(const char *, int);
#endif

	DLLSPEC int ResetAlltoIdle(void);
	DLLSPEC int start_trainer(const char *, bool);				// sets the started flag so averages will update
	DLLSPEC int set_wind(const char *, int, float);
	DLLSPEC int set_draftwind(const char *, int, float);
	DLLSPEC int update_velotron_current(int, unsigned short);
	DLLSPEC int set_velotron_calibration(int, int, int);
	DLLSPEC int velotron_calibration_spindown(int, int);
	//DLLSPEC int get_status_bits(int, int);

	DLLSPEC inline int set_ftp(const char *, int, float);
	DLLSPEC inline float get_pp(const char *, int);
	DLLSPEC inline float get_calories(const char *, int);
	DLLSPEC inline float get_np(const char *, int);
	DLLSPEC inline float get_if(const char *, int);
	DLLSPEC inline float get_tss(const char *, int);
	DLLSPEC inline void calc_tp(const char *, int, unsigned long, float);
#ifdef _DEBUG
		#ifdef DO_ANT
			DLLSPEC inline bool get_ant_stick_initialized(int);
		#endif
	#endif
#endif


#if (LEVEL >= DLL_FULL_ACCESS)

	DLLSPEC void set_timeout(unsigned long _ms);
	DLLSPEC int set_manual_speed(int ix, int fw, float _manual_mph);
	DLLSPEC unsigned long get_finish_ms(int ix, int fw);
	DLLSPEC bool get_shiftflag(int ix);

	DLLSPEC inline bool is_connected(void);
	DLLSPEC inline int get_accum_tdc(int, int);
	DLLSPEC inline int get_tdc(int, int);

	DLLSPEC inline int get_instances(void);
	DLLSPEC unsigned char get_control_byte(int);
	DLLSPEC Physics::MODE get_physics_mode(int);
	DLLSPEC float get_manual_watts(int);
	DLLSPEC float get_slope(int);
	DLLSPEC struct Bike::GEARPAIR get_gear_indices(int);
	DLLSPEC METADATA *get_meta(int);
	DLLSPEC Decoder *get_decoder(int);
	DLLSPEC int set_constant_force(int, float);
	DLLSPEC Physics *get_physics(int);
	DLLSPEC unsigned long get_packet_error1(int);
	DLLSPEC unsigned long get_packet_error2(int);
	DLLSPEC unsigned long get_packet_error3(int);
	DLLSPEC unsigned long get_packet_error4(int);
	DLLSPEC dataSource *getds(int, int);
	DLLSPEC void set_performance_file_name(int, const char *);
	DLLSPEC inline int set_logging(int, int, LoggingType);
	DLLSPEC const char *get_perf_file_name(int);
	DLLSPEC void set_file_mode(int, bool _mode);
	DLLSPEC int set_rider_data(int, RIDER_DATA);
	DLLSPEC bool is_running(int);
	DLLSPEC inline bool is_started(int);
	DLLSPEC inline bool is_paused(int);
	DLLSPEC inline bool is_finished(int);
	DLLSPEC int set_dirs(std::vector<std::string> &);
	DLLSPEC void set_bluetooth_delay(bool);
	DLLSPEC void set_logger(Logger *);
	DLLSPEC inline float get_slip(int, int);
	DLLSPEC inline bool get_slipflag(int, int);
	DLLSPEC inline void clear_slipflag(int, int);
	DLLSPEC inline float get_raw_rpm(int, int);
	DLLSPEC int load_perf(int, int, const char *);
	DLLSPEC int startTrainer2(int ix, Course *, LoggingType );
#endif									// #if (LEVEL >= DLL_FULL_ACCESS)

	}							// extern "C" {

#endif			// #define _DLL_H_

