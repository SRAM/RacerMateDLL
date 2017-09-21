
#if LEVEL >= DLL_CANNONDALE_ACCESS
	#define Setlogfilepath                 x74c9025b3f7ab7bc6230b74976d7c7bd			// 1
	#define Enablelogs                     x02af87953982bfc4c62fdac243868fc9			// 2
	#define GetFirmWareVersion             x40754014802a450df95743b33d591d32			// 3
	#define get_dll_version                x81034afb392e89c38423be075652617d			// 4
	#define GetAPIVersion                  xe82fbf83a8831c8c12c5acdf70251584			// 5
	#define check_for_trainers             x421b6e27c30ae2ca08e4f3bfb7987def			// 6
	#define GetRacerMateDeviceID           xbb66c93359ecb5135514282824f98949			// 7
	#define GetIsCalibrated                xb66e38c42787dd9c3b8e6c5205bcd911			// 8
	#define GetCalibration                 x1145f89e37cec8227eb4d2a785bc2d98			// 9
	#define get_errstr                     x2118619319b7fb36d1cf6bde9678ac12			// 10
	#define resetTrainer                   xf06510dbbd22261f5cfc03064e95e2e0			// 11
	#define startTrainer                   x65a14bacf33e2ef037a154e0e87b2f16			// 12
	#define ResetAverages                  xf3c1a99e992cf6666024d7725446cd89			// 13
	#define GetTrainerData                 x91a1c1bc933e01f94e4db2cc50e46bbd			// 14
	#define get_average_bars               xc0e4d1dbbc943ebce556d112cf951d47			// 15
	#define get_ss_data                    x25dc2c270d1807359bad95e7fc428760			// 16
	#define SetSlope                       xf504076488f979f8f8d465374ef77920			// 17
	#define setPause                       xe59c3144df15a614c5745070c5236ffd			// 18
	#define stopTrainer                    xf4dac05a99cf4d5c6377c8e96b28617a			// 19
	#define ResettoIdle                    xd037666f9d225a93b2b30ec21727e7a9			// 20

#endif

#if LEVEL >= DLL_ERGVIDEO_ACCESS
	#define SetErgModeLoad                 xc365b9e522e7fcf6517a31125cd4976a			// 1
	#define is_connected                   xa7ad310fa3418cc8d76ba65276649f61			// 2
	#define start_trainer                  xde692f4b68511fc233926f9607ece31d			// 3
	#define SetHRBeepBounds                xa07458451708062997d55c763bfc8328			// 4
	#define GetHandleBarButtons            x3cc3df930af2fc5554278f248db4379b			// 5
	#define SetRecalibrationMode           x068a15fb67ebd42794d9a61e62a332cc			// 6
	#define EndRecalibrationMode           xd0cddfca285272ea307ff052cc890bcc			// 7
	#define ResetALLtoIdle                 x6e847ce2fe3949886717b74c448a6f83			// 8
	#define get_accum_tdc                  xdbd0e3460114b708f0af6d3d54a11759			// 9
	#define get_tdc                        xd58f7323bff01ae08ad372315ee2b2cf			// 10
	#define SetVelotronParameters          xfd815d11c787b3e30c808c381e6ec18b			// 11
	#define GetCurrentVTGear               xff984ffb9f21d11205b63f528db9737f			// 12
	#define setGear                        x7876cb3742a388601ac42cabe7d5f357			// 13
	#define get_bars                       xad676dea212f2861de5aa1c7d1dcd54d			// 14
	#define get_calories                   x6ef6ce266a10b8c3d0a910d4f3832d8d			// 15
	#define get_np                         x59134004758005f1a50d0ae9a258acc3			// 16
	#define get_if                         x491736ce3d755e65c9a620bc7d828540			// 17
	#define get_tss                        x0cc0b59e12a37fe9a02c167317265df6			// 18
	#define get_pp                         xa6081f9c03ab3570d3dc08f9e04411fc			// 19
	#define set_ftp                        x329a2bf92be71d1a788931471aedbd52			// 20
	#define set_wind                       xaecf726fc912c8370a80f7b76d2e77bd			// 21
	#define set_draftwind                  x405ce317679b7afe33330e55e1fd03eb			// 22
	#define update_velotron_current        xd4a43945960b96d054b1ab942f543c48			// 23
	#define set_velotron_calibration       x00b6e31a1a518d9c46fccbc582029cbe			// 24
	#define velotron_calibration_spindown  x471598df9ba9c1081d5a4ab76a91965b			// 25
	#define get_status_bits                x4cbdf4ef21b10f0d22acbcf3f38fb47c			// 26
	#define set_use_speed_cadence_sensor   xa7393cfa6e8ece6cf69b4440fc9fd032			// 27
	#define Setlogfilepath                 xc8a441c1aa24dffec0048333533f454f			// 28
	#define Enablelogs                     x6f6ca3bd077474d7a6105f048384db9c			// 29
	#define GetFirmWareVersion             x1cce93aea8b9495a5c755b900cf600f6			// 30
	#define get_dll_version                x9b6b182222a2749df84e05ba099682ca			// 31
	#define GetAPIVersion                  x702b3479f3144a6069e36ca09ab39405			// 32
	#define check_for_trainers             x20becc3356a6603d96606d0d31bd1e84			// 33
	#define GetRacerMateDeviceID           x08334dffa3cbc9a782fce16a69597468			// 34
	#define GetIsCalibrated                x2dec9b3df96cc77ea7559c7c477faa76			// 35
	#define GetCalibration                 xcf088e904a6f75e977857d006cccd189			// 36
	#define get_errstr                     x8a3833fd4c8b95922ac1034a62e6110c			// 37
	#define resetTrainer                   x10392736c453e83d803cdd504a208035			// 38
	#define startTrainer                   x1e46065acfc34347861203604a714b67			// 39
	#define ResetAverages                  x47a63c8f95133b5d8a3e8a9b5f640328			// 40
	#define GetTrainerData                 x4501f3bc0ffcab713cc76ff9a85c651a			// 41
	#define get_average_bars               x6663caec4b1cc534631d32e44e8ae1ee			// 42
	#define get_ss_data                    x0e9daea7c4150c237d3a09fe2d7a6bb8			// 43
	#define SetSlope                       xf6d9d3cbf5c69f219741cd66b2755e38			// 44
	#define setPause                       x8bd35ba7d63c1565cbb5bc107b076afa			// 45
	#define stopTrainer                    x0ee33208a327693b661381afd24d2c53			// 46
	#define ResettoIdle                    xb6b75634b4821a4952fa38e894e1f21b			// 47

#endif

#if LEVEL >= DLL_FULL_ACCESS
	#define get_instances                  xa1f692c20bfa9da91eae98861058f0c6			// 1
	#define get_control_byte               xd42b11ded3ba9720ff75ee43b7ab0593			// 2
	#define get_physics_mode               xa652a73bb7741c34a25e3fe772cfc53b			// 3
	#define get_computrainer_mode          x21eff89784e62a1c1b127aa3a06c11a2			// 4
	#define get_manual_watts               xa63335d7714eb687f97703d1265edf9e			// 5
	#define get_what                       x8492822adca5d5e00a42684b1ccd1303			// 6
	#define set_timeout                    x389cf4eed253d96b16dbaa85ec702dc3			// 7
	#define get_slope                      x09e706c13b0d16bf3e19886fe4e24cb5			// 8
	#define get_lower_hr                   x457ad5610a4e876b66fc422de09941d4			// 9
	#define get_upper_hr                   x43df50fff2de2c126d599bcb121b44dc			// 10
	#define get_hr_beep_enabled            x4745bde35b3fd46d193c74b2e78405d6			// 11
	#define get_gear_indices               x29e65e22ebfd84002d34c4e4da96d1f1			// 12
	#define get_meta                       x5658bf3155ddccd4d4b8d29fc471f499			// 13
	#define get_decoder                    xc1cf6cf7126b314f65a16d5ab3905812			// 14
	#define set_constant_force             x7038b61f6ff53e0dc38f187e01e77ef1			// 15
	#define get_physics                    x2f895dc8512e01fe7502c996a3967e0a			// 16
	#define get_packet_error1              xc24e0bf8505f56cf62206901b14b4097			// 17
	#define get_packet_error2              x73ccd66e37cc1c8471044b949886fad8			// 18
	#define get_packet_error3              xb90efaa5b55a5097ff817d7b7df2c87d			// 19
	#define get_packet_error4              x9e13b8d47b0a25354b6f30b6f41c3b89			// 20
	#define getds                          x93b321f24cdf69f9c2541c70bac0cd46			// 21
	#define set_manual_speed               x50e6bca6f1c8c7708c8dbf2e9c0f47fc			// 22
	#define set_performance_file_name      xcaac07325ac475b48234446baa31b2db			// 23
	#define get_cal_time                   x86df2b3a59361897da476906567cdeb7			// 24
	#define set_calibrating                x4c1e5304ed76721a7ff1e6a5375ca491			// 25
	#define set_logging                    x91fad733f5d45b909cc906440c835c2f			// 26
	#define get_perf_file_name             xd61cabaec76dd14f89c8d817d1c657fa			// 27
	#define set_file_mode                  xdee4375c796de8994c22c2dbe9e461a8			// 28
	#define set_rider_data                 x77e5fea6a5df28545f1ebea3e80918fc			// 29
	#define is_running                     x78bc0775b8c5fe877b23348210b8af15			// 30
	#define is_started                     x436718fceb49acfc49f14c237796cfb9			// 31
	#define is_paused                      x07cae3bd883ce78a1133766f08f2d9ab			// 32
	#define is_finished                    x1b4b298d56f4d0143fb20930fff6f08c			// 33
	#define set_dirs                       x158da92776cdfa9b241837eeef614a04			// 34
	#define startCal                       x19a50b3cf66ca4565a7a1921ef99937f			// 35
	#define get_calibration                xe0efbd601fd5b591f3454098b1cbf157			// 36
	#define get_shiftflag                  x8b51b438265235cfd6b578bbcd32158d			// 37
	#define set_bluetooth_delay            x337410d0d088d1d6d62389ef02c47e1a			// 38
	#define init_dll                       xb9ea04eee8517bb7e1752d02c5e6bf6a			// 39
	#define stop_dll                       x5164c6e3e3b91c4ad4def7cea55928d7			// 40
	#define set_logger                     x47871d5398b7245b9e193a8abe49f14b			// 41
	#define get_slip                       x9fa94bd41c77b68b22b71f5f3d73a2f5			// 42
	#define get_slipflag                   x21fbec23d001b37af156937a95ebf71a			// 43
	#define clear_slipflag                 xe39af934cbba13d6c3a8aea67294c091			// 44
	#define get_raw_rpm                    xe4df69424150ab581c60008eec38b3a2			// 45
	#define load_perf                      xb05742b82a21bbec94e2d42ed3f77782			// 46
	#define get_finish_ms                  x45475a367becbe40de69a93b1998be7b			// 47
	#define SetErgModeLoad                 xa96c6872f0568d19c902cf3577d7a4b3			// 48
	#define is_connected                   xcda243038fdee8c9d19828a86d0e792b			// 49
	#define start_trainer                  x5cb37813ccda2b577c6d74c2a3cd8697			// 50
	#define SetHRBeepBounds                x33b4f74b0206468a7c53a2cac8f4ff6c			// 51
	#define GetHandleBarButtons            x8733ec22f047c12cede6ebc02c374df0			// 52
	#define SetRecalibrationMode           x6b44124ac53433a8b45cff5b04d51087			// 53
	#define EndRecalibrationMode           x885864d9a5a5570632fd3a180455f6cb			// 54
	#define ResetALLtoIdle                 xc864d5024ee19e5bba21e93f0f11ea8f			// 55
	#define get_accum_tdc                  x04d4035f8ee6e6343318d5b9a976adf1			// 56
	#define get_tdc                        xe12b485b030ad42af2fb77c1c74aff38			// 57
	#define SetVelotronParameters          x7e56b19470b411e86a70818471da7edb			// 58
	#define GetCurrentVTGear               xfbf69a1ef3ddae55a07cb66cdf34da63			// 59
	#define setGear                        xc35eeed13934cc3419338ea4f2f2e495			// 60
	#define get_bars                       x2e99b03ec370936cc7fda44a210c4245			// 61
	#define get_calories                   x3c77adfeceaf9b592ca2b03b23feda3a			// 62
	#define get_np                         xfc9e4998a633e824bf81d0d8c32e37bc			// 63
	#define get_if                         x7820ec4bca118279ecfec6c7b8712454			// 64
	#define get_tss                        xf2d1d240ab85a1da27344d4872a1ddd8			// 65
	#define get_pp                         xe04a57c33e3c72fef7fe244c06162ba7			// 66
	#define set_ftp                        x0082230aee9aea72f160043d0f03dfb6			// 67
	#define set_wind                       xe0072a0b13d72f163a7b5e2ef4ddecc2			// 68
	#define set_draftwind                  x44d5eb603d00fd6d6197d1c12d043f8b			// 69
	#define update_velotron_current        x2a4781fab1a41d774b44708d062fea36			// 70
	#define set_velotron_calibration       xee7c5fff084dfc09b855e7be1fcde9c7			// 71
	#define velotron_calibration_spindown  xc8cfcb2bb4d3dc5e578591997623fa6e			// 72
	#define get_status_bits                x006697bce07af781c1560896b3b9f4d3			// 73
	#define set_use_speed_cadence_sensor   x1163deaeb337b3fddf96c664d1d20621			// 74
	#define Setlogfilepath                 x37278a9fb79fbd0169724fe4e8fca3b8			// 75
	#define Enablelogs                     x5cae91b94b76a3b2628045b0c5e1db64			// 76
	#define GetFirmWareVersion             xe459f891d4f9cefa0d2a5eb4874558d8			// 77
	#define get_dll_version                x21e992cb6f59d2db2af60bb3ee0cb0d5			// 78
	#define GetAPIVersion                  x333b65b496af6255cfa08ebce06c3085			// 79
	#define check_for_trainers             xd815a1306edf096f90cd3bbb6de0acfb			// 80
	#define GetRacerMateDeviceID           xc17f820c49c4fb1542658ba84bb101d8			// 81
	#define GetIsCalibrated                x589d1e1bda41ca503acb93dd123aca38			// 82
	#define GetCalibration                 xed3d40f3aa3914748d6dd170cb1e4160			// 83
	#define get_errstr                     xf41694876cadece280cfd7a313e97048			// 84
	#define resetTrainer                   x0db85ce903024d90d995a08f998f29f5			// 85
	#define startTrainer                   x2ece0bebb018ddac45a7b7a2399cb138			// 86
	#define ResetAverages                  x7f231eef07fa3382cb918008a5004b97			// 87
	#define GetTrainerData                 x0c4c38271c2f9da2b1d2f56f00bb860d			// 88
	#define get_average_bars               x3debb40befe0b18031672ac3909db2f3			// 89
	#define get_ss_data                    x8d1feedf5784143d46b68e07b8263914			// 90
	#define SetSlope                       xb2c9324504984d8b57b81ee3e19a54d3			// 91
	#define setPause                       xdab9714965c73d54c5095604ca8aa1de			// 92
	#define stopTrainer                    xfd46ef5858af74a3f870745f6a454418			// 93
	#define ResettoIdle                    x08d835fe8edfc97c440285ddb53ad721			// 94

#endif
