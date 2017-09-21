#ifndef _ERGVIDEO_H_
#define _ERGVIDEO_H_

#include <public.h>

#include <bike.h>

#define get_errstr                     x8a3833fd4c8b95922ac1034a62e6110c
#define GetRacerMateDeviceID           x08334dffa3cbc9a782fce16a69597468
#define Setlogfilepath                 xc8a441c1aa24dffec0048333533f454f
#define Enablelogs                     x6f6ca3bd077474d7a6105f048384db9c
#define GetFirmWareVersion             x1cce93aea8b9495a5c755b900cf600f6
#define get_dll_version                x9b6b182222a2749df84e05ba099682ca
#define GetAPIVersion                  x702b3479f3144a6069e36ca09ab39405
#define check_for_trainers             x20becc3356a6603d96606d0d31bd1e84
#define GetIsCalibrated                x2dec9b3df96cc77ea7559c7c477faa76
#define GetCalibration                 xcf088e904a6f75e977857d006cccd189
#define resetTrainer                   x10392736c453e83d803cdd504a208035
#define startTrainer                   x1e46065acfc34347861203604a714b67
#define ResetAverages                  x47a63c8f95133b5d8a3e8a9b5f640328
#define GetTrainerData                 x4501f3bc0ffcab713cc76ff9a85c651a
#define get_average_bars               x6663caec4b1cc534631d32e44e8ae1ee
#define get_ss_data                    x0e9daea7c4150c237d3a09fe2d7a6bb8
#define SetSlope                       xf6d9d3cbf5c69f219741cd66b2755e38
#define setPause                       x8bd35ba7d63c1565cbb5bc107b076afa
#define stopTrainer                    x0ee33208a327693b661381afd24d2c53
#define ResettoIdle                    xb6b75634b4821a4952fa38e894e1f21b
#define GetHandleBarButtons            x3cc3df930af2fc5554278f248db4379b
#define SetErgModeLoad                 xc365b9e522e7fcf6517a31125cd4976a
#define SetHRBeepBounds                xa07458451708062997d55c763bfc8328
#define SetRecalibrationMode           x068a15fb67ebd42794d9a61e62a332cc
#define EndRecalibrationMode           xd0cddfca285272ea307ff052cc890bcc
#define SetVelotronParameters          xfd815d11c787b3e30c808c381e6ec18b
#define GetCurrentVTGear               xff984ffb9f21d11205b63f528db9737f
#define setGear                        x7876cb3742a388601ac42cabe7d5f357
#define get_bars                       xad676dea212f2861de5aa1c7d1dcd54d
#define ResetALLtoIdle                 x6e847ce2fe3949886717b74c448a6f83
#define start_trainer                  xde692f4b68511fc233926f9607ece31d
#define set_ftp                        x329a2bf92be71d1a788931471aedbd52
#define set_wind                       xaecf726fc912c8370a80f7b76d2e77bd
#define set_draftwind                  x405ce317679b7afe33330e55e1fd03eb
#define update_velotron_current        xd4a43945960b96d054b1ab942f543c48
#define set_velotron_calibration       x00b6e31a1a518d9c46fccbc582029cbe
#define velotron_calibration_spindown  x471598df9ba9c1081d5a4ab76a91965b
#define get_status_bits                x4cbdf4ef21b10f0d22acbcf3f38fb47c
#define set_use_speed_cadence_sensor   xa7393cfa6e8ece6cf69b4440fc9fd032
#define set_use_heartrate_sensor       x3b1e8ad4f27162bc73b5f8155254c149

//-------------------------------
// prototypes:
//-------------------------------

__declspec(dllexport) const char *x8a3833fd4c8b95922ac1034a62e6110c(int);
__declspec(dllexport) EnumDeviceType x08334dffa3cbc9a782fce16a69597468(int);
__declspec(dllexport) int xc8a441c1aa24dffec0048333533f454f(const char *);
__declspec(dllexport) int x6f6ca3bd077474d7a6105f048384db9c(bool, bool, bool, bool, bool, bool);
__declspec(dllexport) int x1cce93aea8b9495a5c755b900cf600f6(int);
__declspec(dllexport) const char *x9b6b182222a2749df84e05ba099682ca(void);
__declspec(dllexport) const char *x702b3479f3144a6069e36ca09ab39405(void);
__declspec(dllexport) EnumDeviceType x20becc3356a6603d96606d0d31bd1e84(int);
__declspec(dllexport) bool x2dec9b3df96cc77ea7559c7c477faa76(int, int);
__declspec(dllexport) int xcf088e904a6f75e977857d006cccd189(int);
__declspec(dllexport) int x10392736c453e83d803cdd504a208035(int, int, int);
__declspec(dllexport) int x1e46065acfc34347861203604a714b67(int);
__declspec(dllexport) int x47a63c8f95133b5d8a3e8a9b5f640328(int, int);
__declspec(dllexport) struct TrainerData x4501f3bc0ffcab713cc76ff9a85c651a(int, int);
__declspec(dllexport) float *x6663caec4b1cc534631d32e44e8ae1ee(int, int);
__declspec(dllexport) struct SSDATA x0e9daea7c4150c237d3a09fe2d7a6bb8(int, int);
__declspec(dllexport) int xf6d9d3cbf5c69f219741cd66b2755e38(int, int, int, float, float, int, float);
__declspec(dllexport) int x8bd35ba7d63c1565cbb5bc107b076afa(int, bool);
__declspec(dllexport) int x0ee33208a327693b661381afd24d2c53(int);
__declspec(dllexport) int xb6b75634b4821a4952fa38e894e1f21b(int);
__declspec(dllexport) int x3cc3df930af2fc5554278f248db4379b(int, int);
__declspec(dllexport) int xc365b9e522e7fcf6517a31125cd4976a(int, int, int, float);
__declspec(dllexport) int xa07458451708062997d55c763bfc8328(int, int, int, int, bool);
__declspec(dllexport) int x068a15fb67ebd42794d9a61e62a332cc(int, int);
__declspec(dllexport) int xd0cddfca285272ea307ff052cc890bcc(int, int);
__declspec(dllexport) int xfd815d11c787b3e30c808c381e6ec18b(int, int, int, int, int*, int*, float, int, int, float, int, int );
__declspec(dllexport) struct Bike::GEARPAIR xff984ffb9f21d11205b63f528db9737f(int, int);
__declspec(dllexport) int x7876cb3742a388601ac42cabe7d5f357(int, int, int, int);
__declspec(dllexport) float *xad676dea212f2861de5aa1c7d1dcd54d(int, int);
__declspec(dllexport) int x6e847ce2fe3949886717b74c448a6f83(void);
__declspec(dllexport) int xde692f4b68511fc233926f9607ece31d(int, bool);
__declspec(dllexport) int x329a2bf92be71d1a788931471aedbd52(int, int, float);
__declspec(dllexport) int xaecf726fc912c8370a80f7b76d2e77bd(int, int, float);
__declspec(dllexport) int x405ce317679b7afe33330e55e1fd03eb(int, int, float);
__declspec(dllexport) int xd4a43945960b96d054b1ab942f543c48(int, unsigned short);
__declspec(dllexport) int x00b6e31a1a518d9c46fccbc582029cbe(int, int, int);
__declspec(dllexport) int x471598df9ba9c1081d5a4ab76a91965b(int, int);
__declspec(dllexport) int x4cbdf4ef21b10f0d22acbcf3f38fb47c(int, int);
__declspec(dllexport) void xa7393cfa6e8ece6cf69b4440fc9fd032(int, int);
__declspec(dllexport) void x3b1e8ad4f27162bc73b5f8155254c149(int, int);

#endif				// #ifdef _ERGVIDEO_H_

