#ifndef _DLL_H_
#define _DLL_H_

#define WIN32_LEAN_AND_MEAN


#include <public.h>
#include <physics.h>
#include <datasource.h>
#include <decoder.h>

#ifdef WIN32
	#if 0
	//#ifdef __cplusplus
		extern "C" {
	#endif
#else
	#ifdef __cplusplus
		extern "C" {
	#endif
#endif

#include <glib_defines.h>

#include "levels.h"


/*
#define API_VERSION "1.1.0"		// bump this only when the API changes
#define DLL_VERSION "1.0.13"		// bump this "always", will wanted it to be 1.0.3, but it was really 1.0.4
*/

#include "versions.h"
#include "ev.h"

#ifdef LOGDLL
	#error "LOGDLL ALREADY DEFINED"
#endif

#ifdef _DEBUG
	#ifdef WIN32
		//#define LOGDLL
	#else
		//#define LOGDLL
	#endif
#else
	//#define LOGDLL
#endif

#ifdef LOGDLL
	#define DLLFILE "dll.log"
#endif

#ifdef DO_OBFUSCATE
	#if (LEVEL == DLL_CANNONDALE_ACCESS)
		#define get_errstr                     x2118619319b7fb36d1cf6bde9678ac12
		#ifdef __MACH__
			#define set_portname						xyzzy
		#endif

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
	#endif								// #if (LEVEL >= DLL_CANNONDALE_ACCESS)

	#if (LEVEL == DLL_TRINERD_ACCESS)
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
		#define SetRecalibrationMode           xba24ed771045b8b94fa9098818607b52
		#define EndRecalibrationMode           x5290e6deedba385222517b1c032d39e8
		#define get_computrainer_mode          xc656ca72b7553536c8ef9be8fff49ec8
	#endif												// #if (LEVEL == DLL_TRINERD_ACCESS)

	#if (LEVEL == DLL_ERGVIDEO_ACCESS)
		#define get_errstr                     x7e4b8460b7f70589b12fe02c7727033a
		#define GetRacerMateDeviceID           xe44f5940639d19c7d75af4cc50f52627
		#define Setlogfilepath                 x2e5184f2acbfbf02e412b63f9c437a2b
		#define Enablelogs                     xf3caa7adf283cef08bf50e2817dbd1fa
		#define GetFirmWareVersion             xf6ff6978145da30507d7273b30d57048
		#define get_dll_version                x57d4e6345c7eca5164a0ed125b0a7947
		#define GetAPIVersion                  x77ae24db08410418b007f2f78197b12f
		#define check_for_trainers             x010fb505c40bbb260c9bf6a981018cfb
		#define GetIsCalibrated                xead20f21bf8a0c75768469ae3c81899f
		#define GetCalibration                 x52eb2146582e00a11868691a63684ce8
		#define resetTrainer                   xdc58940a269093ef4f3b24b198d4f5cd
		#define startTrainer                   xcb05602f8585015d427f8bc21aa04dd2
		#define ResetAverages                  xd88900273b45f82781a39d724d6a0155
		#define GetTrainerData                 xdd0a789ee158e3f3cc2af3a6c2c27bfa
		#define get_average_bars               xf03c385eb37d7ec91949d131f8151067
		#define get_ss_data                    x11e0d99d8b3db35c8c9c143c7e4604eb
		#define SetSlope                       x95a11b34b4272974c3080293ceb2d706
		#define setPause                       xeef3cffd0410c6adf570e602cc190497
		#define stopTrainer                    x3a7d4fd38f0687e5896e1d50efeb698e
		#define ResettoIdle                    x4cd79449513da2a2d6cc6d7261ea71ff

		#define GetHandleBarButtons            xbe6cdadfa51b89ea48e8847cb65682a0
		#define SetErgModeLoad                 x07d4ba4cbf768356c8e7a487c0ac8620
		#define SetHRBeepBounds                xb87a4e56c4cd11a1c169ffacb57a4953
		#define SetRecalibrationMode           x6b7fe11c2bdf03a307fc4b89f0aa6849
		#define EndRecalibrationMode           xc1f918211cf81d12231e306ec1a3401e
		#define get_computrainer_mode          x81f88c857ac37f0db9992bf6b3cc118e

		#define SetVelotronParameters          x720de7788bf90a8cff74e683633c97e3
		#define GetCurrentVTGear               x7223a05782894100c28046fe70e8f80d
		#define setGear                        xf2dcb410127d9fb73f8d7ece38f484d3
		#define get_bars                       xc5d2185cd957432a236590dff37b698f
		#define ResetAlltoIdle                 xc5388f539055d3f170a8b7ec914e1246
		#define start_trainer                  x40557f2e2f33d3fbcbbf35d798d09a75
		#define set_ftp                        x3d32f489c08b5fbb1d8987d798be96ee
		#define set_wind                       x93fe436d52bee5eb1c4f760618a86f1d
		#define set_draftwind                  x4c7ff5b24822d1b052932681e21078c8
		#define update_velotron_current        xe815ad58306a19844849ee0cf9f3423e
		#define set_velotron_calibration       xe86b1d45fea29400e685ca372e6b3df2
		#define velotron_calibration_spindown  x912cedc9091a9a97d83000156d70dbb1
		#define get_status_bits                xbea472536fcf1f4128b1c870a71c0fb6
		#define set_use_speed_cadence_sensor   xe831533163b52827d6fda0d40c838427
		#define set_use_heartrate_sensor       x836216c45871a8b1cc6bfdb06e02d264
		#define set_circumference              x131cb3e8b4e6f61b91dc5a112f7c01a3
		#define GetPortNames							x7a3f1a877cbf4ac500ff32a493d9ef51
	#endif												// #if (LEVEL == DLL_ERGVIDEO_ACCESS)

	#if (LEVEL == DLL_FULL_ACCESS)
		#define get_errstr                     xca5388cd4a121e45e1b3dd6491077908
		#define GetRacerMateDeviceID           x0815e8afabec35c6d166a459c85ac8a2
		#define Setlogfilepath                 x56dd52ecb9176116d059f22f6f34812c
		#define Enablelogs                     xe8d0ab79647dd2d165b9b6093a59967d
		#define GetFirmWareVersion             x39b36bdcea653b7914aa107791711072
		#define get_dll_version                x23d4c8df5e7b13bc35d542842689ba49
		#define GetAPIVersion                  x16ad7e8b0e871fcb3e192d893a800d67
		#define check_for_trainers             x6c15402fa349e747fde5947d4d09dc9a
		#define GetIsCalibrated                x872d11921b33d51c522d682785910587
		#define GetCalibration                 x21b191d7fa975fd49dedc502afdbb793
		#define resetTrainer                   xd1c3e6fb690bd1b11ad74f094b07eed7
		#define startTrainer                   x28b7a2ce07274b84609b250feaaf091f
		#define ResetAverages                  xa328a11f3736bfb251e41d73d4457a02
		#define GetTrainerData                 x0303cc6eff0d9636570c5e888ac55c50
		#define get_average_bars               xb0466468335c5e61442eaff84b5a5953
		#define get_ss_data                    xb742646e9fe1e01c15290cd0205835b6
		#define SetSlope                       x6e07a672ddc17d12089511101b355c90
		#define setPause                       x69088363979474d56aa1cbef0bbb644a
		#define stopTrainer                    xbf02f7ac3a384037a6360b1c5bde8ea2
		#define ResettoIdle                    xeea34a9ac9d877fef6490f64e18a6a74

		#define GetHandleBarButtons            x9d4575199c0e16a7e6dafc65219209e0
		#define SetErgModeLoad                 x159e54c9cfdea5e67e0ad374c9a6f54d
		#define SetHRBeepBounds                x9cc8611fbefec1316e203818a1544ec7
		#define SetRecalibrationMode           xde0d2778a3e81613937acd08ddaaf119
		#define EndRecalibrationMode           x4a6a4beb7764cfec115b460aad4ad8ac
		#define get_computrainer_mode          xbc68e7651d0e529f9283bb3fdc17a376

		#define SetVelotronParameters          x1f58e0841afe348bfb6427bf2033cb8e
		#define GetCurrentVTGear               x15d517a6f4cb4c26fe108d0fe0b71ad3
		#define setGear                        xd134b955f0bb1291720a8924388f5b1b
		#define get_bars                       xd8123e09c165a598cbd1129579036bb8
		#define ResetAlltoIdle                 x880e064c7d614b415ff7e447d1c8651d
		#define start_trainer                  x67dfd5f788bea900edc8511f80dc3caa
		#define set_ftp                        xec67aa5b5b815ac60d284c1aac172ed7
		#define set_wind                       xdfe948e8d876b0a9fb9f21d9064be0b5
		#define set_draftwind                  xab4afdaaf10c0cb5e3d8cfa538a6c4a2
		#define update_velotron_current        x9d176e07de202579a8f172e207aaea47
		#define set_velotron_calibration       x149dd4e1aee4d8a2fbe3253870d5bfdd
		#define velotron_calibration_spindown  x22af99f6d6fe56a4e31e74047a906bd6
		#define get_status_bits                xf5878be882ef5b9347424b6dab5c79b4
		#define set_use_speed_cadence_sensor   x652eae5004548967116abfcb7b2d3a19
		#define set_use_heartrate_sensor       x5077003836f8de3c4e21a16f95d74d03
		#define set_circumference              xd276fd0580c7fb78da7383299dc24c33

		#define set_timeout                    xe69627be88d15e9b76c54c5ccc985589
		#define set_manual_speed               x63861bbe114fd427ea005b1be602c6a3
		#define get_finish_ms                  x7d73b502984d9f894c04be0123ab72af
		#define get_shiftflag                  x245dd798886b5417a74038b8c15a383c
		#define is_connected                   x95f87de588c4348dc6ce6aeca8023cb3
		#define get_accum_tdc                  xea92f16f41581df7f75827069abd4014
		#define get_tdc                        xa2dd585cf183d394452671a451d637c5
		#define get_calories                   xfdee599f30a2bd6ba19d36b71761fa9a
		#define get_np                         x75e2e1d0da1377828cdbd4d00e9cf5de
		#define get_if                         xb64c5e10a5a12b8bee28f86089626f0b
		#define get_tss                        x98ec8c85729ece61a2be14063df64ee6
		#define get_pp                         x045892074e3f5ed8e5f776d411777dd9
		#define get_instances                  x0d4c09b4e1ca7b0fe715936bff83e2d5
		#define get_control_byte               xd877141fa3817ec6cf7164aa21750220
		#define get_physics_mode               x935293b90b84c23ae5e28af68248e75f
		#define get_manual_watts               x3faf331f83f30bc3f70327736ce1f981
		#define get_slope                      xf7a5e280aed23100a4d40f9241bce8ab
		#define get_gear_indices               xd91788f8f04998c6666072f59b6b144d
		#define get_meta                       xdfefb6096f21290cd3ebcdcfa7d6b89b
		#define get_decoder                    x30543c8d686cced59d4ad5537a92af20
		#define set_constant_force             x6aee23ef71f694b5684e797c4520ac1d
		#define get_physics                    x0f94a89d85bb2db1eefb564240434f4f
		#define get_packet_error1              x4e8ea0d95733532b54b8040bf4f6ba9a
		#define get_packet_error2              xcc788036b0811a0711d5f11952964eb5
		#define get_packet_error3              x24606b402f67c101f820a5ecc143e317
		#define get_packet_error4              x3f805700b6f085d2989e7c1235f0c517
		#define getds                          xe18e0956a66b8b416c91a31995ad9f8f
		#define set_performance_file_name      x4fe866d7cb0c22261874c8317ed9dcf3
		#define set_logging                    x8c0d25cd9f5960a97667997e827c0498
		#define get_perf_file_name             x1e62ca66aa84f3c4ce7d445a5873e4f8
		#define set_file_mode                  xffa6320b1c03dd5258a52d76ed8ebfdd
		#define set_rider_data                 xea01c858e36c009c7a3796a0d4534acd
		#define is_running                     x5fd4e6e4523af5b8c43941cf857ad45f
		#define is_started                     x481162445055dd3b376fd80d2fcd77d2
		#define is_paused                      xfebdc505608d9c65f5ff7ac8d77f7df9
		#define is_finished                    x31484747c577b76c938c86bc00d08704
		#define set_dirs                       x7270045764b636fcadf93ed7660fcbb3
		#define set_bluetooth_delay            xce1a32c06b39b80688f14e1dbfb32f33
		#define set_logger                     x2445899def058da89a2520dc46c965e4
		#define get_slip                       x30a0fccb042379da38f0f0144c331111
		#define get_slipflag                   x2038af4b9487a1976f5c99a3374522df
		#define clear_slipflag                 x1389e0fdd1403a9529fba7ffca183fb0
		#define get_raw_rpm                    x77de26c0955341c3f7fd4b2ec56c7b54
		#define load_perf                      x0cde46e27b6da444d4817ccd519580fc
		#define startTrainer2                  x582dcea2902ff55285754a362a7cf121
	#endif								//	#if (LEVEL == DLL_FULL_ACCESS)

#endif									// #ifdef DO_OBFUSCATE


#if (LEVEL >= DLL_CANNONDALE_ACCESS)

	#ifdef NEWPORT
		DLLSPEC int set_port_info(int, PORT_INFO);
	#endif

	#ifdef __MACH__
		DLLSPEC const char *get_errstr(int);
		DLLSPEC struct TrainerData GetTrainerData(int, int);
		DLLSPEC struct SSDATA get_ss_data(int, int);
	#else
		#ifdef WIN32
			DLLSPEC const char *get_errstr(int) throw(...);
			DLLSPEC inline struct TrainerData GetTrainerData(int, int) throw(...);
			//};								// extern "C" {
			DLLSPEC inline struct SSDATA get_ss_data(int, int) throw(...);
			//extern "C" {
		#else
			DLLSPEC const char *get_errstr(int);
			DLLSPEC struct TrainerData GetTrainerData(int, int);
			DLLSPEC struct SSDATA get_ss_data(int, int);
		#endif

	#endif

	DLLSPEC EnumDeviceType GetRacerMateDeviceID(int);
	DLLSPEC int Setlogfilepath(const char *);
	DLLSPEC int Enablelogs(bool, bool, bool, bool, bool, bool);
	DLLSPEC int GetFirmWareVersion(int);						// optional
	DLLSPEC const char *get_dll_version(void);
	DLLSPEC const char *GetAPIVersion(void);
	DLLSPEC const char *get_build_date(void);
	//DLLSPEC EnumDeviceType check_for_trainers(int);
	DLLSPEC bool GetIsCalibrated(int, int);
	DLLSPEC int GetCalibration(int);
	DLLSPEC int resetTrainer(int, int, int);
	//DLLSPEC int startTrainer(int ix, Course * = NULL, LoggingType=NO_LOGGING);
	DLLSPEC int startTrainer(int ix);
	DLLSPEC int ResetAverages(int, int);
#ifdef WIN32
	DLLSPEC inline float *get_average_bars(int, int);
#else
	DLLSPEC float *get_average_bars(int, int);
#endif
	DLLSPEC int SetSlope(int, int, int, float, float, int, float);
	DLLSPEC int setPause(int, bool);
	DLLSPEC int stopTrainer(int);
	DLLSPEC int ResettoIdle(int);

	// new functions
	DLLSPEC int racermate_init(void);
	DLLSPEC int racermate_close(void);

#ifdef WIN32
	DLLSPEC const char * GetPortNames(void) throw(...);
#else
	DLLSPEC const char * GetPortNames(void);
#endif

	//DLLSPEC int set_client_network(int, const char *, int);
	DLLSPEC int set_server_network(int, int, bool=false, bool=false);
	#ifdef _DEBUG
		DLLSPEC const char *get_state(int);
	#endif
#endif								// #if (LEVEL >= DLL_CANNONDALE_ACCESS)

#if (LEVEL >= DLL_TRINERD_ACCESS)
#ifdef WIN32
	DLLSPEC inline int GetHandleBarButtons(int, int);
#else
	DLLSPEC int GetHandleBarButtons(int, int);
#endif
	DLLSPEC int SetErgModeLoad(int, int, int, float);
	DLLSPEC int SetHRBeepBounds(int, int, int, int, bool);
	DLLSPEC int SetRecalibrationMode(int, int);
	DLLSPEC int EndRecalibrationMode(int, int);
	DLLSPEC int get_computrainer_mode(int);
#endif									// #if (LEVEL >= DLL_TRINERD_ACCESS)

#if (LEVEL >= DLL_ERGVIDEO_ACCESS)

	//DLLSPEC const char ** GetPortNames(void);
	//DLLSPEC int * GetPortNames(void);
	//DLLSPEC char * GetPortNames(void);
	//DLLSPEC std::vector< std::string> GetPortNames(void);

	DLLSPEC int SetVelotronParameters(int, int, int, int, int*, int*, float, int, int, float, int, int );
	#ifdef __MACH__
		DLLSPEC struct Bike::GEARPAIR GetCurrentVTGear(int, int);
	#else
		#ifdef WIN32
			DLLSPEC struct Bike::GEARPAIR GetCurrentVTGear(int, int) throw(...);
		#else
			DLLSPEC struct Bike::GEARPAIR GetCurrentVTGear(int, int);
		#endif
	#endif

	DLLSPEC int setGear(int, int, int, int);
#ifdef WIN32
	DLLSPEC inline float *get_bars(int, int);
#else
	DLLSPEC float *get_bars(int, int);
#endif

	DLLSPEC int ResetAlltoIdle(void);
	DLLSPEC int start_trainer(int, bool);				// sets the started flag so averages will update
	DLLSPEC int set_wind(int, int, float);
	DLLSPEC int set_draftwind(int, int, float);
	DLLSPEC int update_velotron_current(int, unsigned short);
	DLLSPEC int set_velotron_calibration(int, int, int);
	DLLSPEC int velotron_calibration_spindown(int, int);
	DLLSPEC int get_status_bits(int, int);

#ifdef WIN32
	DLLSPEC inline int set_ftp(int, int, float);
	DLLSPEC inline float get_pp(int, int);
	DLLSPEC inline float get_calories(int, int);
	DLLSPEC inline float get_np(int, int);
	DLLSPEC inline float get_if(int, int);
	DLLSPEC inline float get_tss(int, int);
	#ifdef _DEBUG
		#ifdef DO_ANT
			DLLSPEC inline bool get_ant_stick_initialized(int);
		#endif
	#endif
#else
	DLLSPEC int set_ftp(int, int, float);
	DLLSPEC float get_pp(int, int);
	DLLSPEC float get_calories(int, int);
	DLLSPEC float get_np(int, int);
	DLLSPEC float get_if(int, int);
	DLLSPEC float get_tss(int, int);
	#ifdef _DEBUG
		#ifdef DO_ANT
			DLLSPEC bool get_ant_stick_initialized(int);
		#endif
	#endif
#endif

#endif								// #if (LEVEL >= DLL_ERGVIDEO_ACCESS)

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

//#ifdef WIN32
#if 0
	}							// extern "C" {
#else
}
#endif

#endif			// #define _DLL_H_

