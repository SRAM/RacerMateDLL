
#include <stdlib.h>
#include <assert.h>

#include <glib_defines.h>
#include <fatalerror.h>

#include <dll_globals.h>
#include <errors.h>
#include <internal.h>

#include "trainer.h"

int Trainer::Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };																			// up to 5 front gears
int Trainer::cogset[MAX_REAR_GEARS] = { 11, 12, 13, 14, 15, 16, 17, 19, 21, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };			// up to 20 rear gears

/****************************************************************************

****************************************************************************/

Trainer::Trainer(void)  {
	what = DEVICE_NOT_SCANNED;
	appkey = "zzz";
	iport = -1;
	//memset(portname, 0, sizeof(portname));
	memset(keyboard_keys_string, 0, sizeof(keyboard_keys_string));
	//udpkey = NULL;
	comtype = BAD_INPUT_TYPE;
	fw = 0;
	cal = 0;
	keys = 0x40;
	watts = 0.0f;
	rpm = 0.0f;
	hr = 0.0f;
	mph = 0.0f;
	slope = 0;									// %grade * 100
	fix = 0;										// index of velotron front gear (0-2)
	rix = 0;										// index of velotron rear gear (0-9)
	maxfix = -1;								// highest non-zero index for front gear
	maxrix = -1;								// highest non-zero index for rear gear
	bars = NULL;
	average_bars = NULL;
	bike_kgs = (float)(TOKGS*20.0f);		// weight of bike in kgs
	wheeldiameter = 700.0f;					// in mm, 27.0 inches
	ActualChainring = 62;					// physical number of teeth on velotron front gear
	Actualcog = 14;							// physical number of teeth on velotron rear gear
	person_kgs = (float)(TOKGS*150.0f);
	drag_factor = 100;
	manwatts = 100;							// integer
	calmode = false;
	memset(gteststring, 0, sizeof(gteststring));
	wind = 0.0f;								// float, kph
	draft_wind = 0.0f;						// float, kph

	bp = 0;

	return;
}							// constructor

/****************************************************************************

****************************************************************************/

int Trainer::init(const char * _id)  {
	appkey = _id;

	const char *cptr;
	int status;
	bool b;

	if (strstr(appkey, "UDP-")) {
		comtype = TRAINER_IS_CLIENT;																			// trainer is client
	}
	else if (strstr(appkey, "COM")) {
		comtype = WIN_RS232;																			// trainer is client
	}
	else {
		throw("yee");
	}


#ifdef _DEBUG
	int n;
	n = check_maps("trainer.cpp, init() before GetRacerMateDeviceID()");		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#endif

	what = GetRacerMateDeviceID(appkey);


	/*
	DWORD start;
	start = timeGetTime();
	while((timeGetTime() - start) < 5000) {
		check_maps("trainer.cpp, init(), after GetRacerMateDeviceID()");		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
		Sleep(10);
	}
	*/

	//check_maps("trainer.cpp, init(), after GetRacerMateDeviceID()");		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^



	if (what == DEVICE_COMPUTRAINER)  {
		printf("found computrainer on port %s\r\n", appkey);
	}
	else if (what == DEVICE_VELOTRON)  {
		printf("found velotron on port %s\n", appkey);
	}
	else if (what == DEVICE_EXISTS)  {
		bp = 1;
		//sprintf(gteststring, "don't see trainer on port %s\r\n", id);
		//throw(fatalError(__FILE__, __LINE__, gteststring));
	}
	else  {
		sprintf(gteststring, "no trainer on port %s", appkey);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		//check_maps("Trainer::init() before GetFirmWareVersion");

		fw = GetFirmWareVersion(appkey);
		if (comtype==TRAINER_IS_CLIENT)  {						// trainer is a client
			assert(fw==4543);
		}
		else  {
			//assert(fw==4095);
		}

		if (FAILED(fw))  {
			cptr = get_errstr(fw);
			sprintf(gteststring, "%s", cptr);
			throw(fatalError(__FILE__, __LINE__, gteststring));
		}
		if (what==DEVICE_VELOTRON)  {
			if (fw != 190)  {
				sprintf(gteststring, "unknown firmware version ( %d )", fw);
				throw(fatalError(__FILE__, __LINE__, gteststring));
			}
		}
		printf("firmware = %d\r\n", fw);
	}

	/*
	DWORD start;
	start = timeGetTime();
	while((timeGetTime() - start) < 5000) {
		check_maps("Trainer::init() before GetIsCalibrated");
		Sleep(10);
	}
	*/
	//check_maps("Trainer::init() before GetIsCalibrated");

	b = GetIsCalibrated(appkey, fw);									// false
	printf("calibrated = %s\r\n", b?"true":"false");

	//check_maps("Trainer::init() after GetIsCalibrated");

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		cal = GetCalibration(appkey);										// 200
		printf("cal = %d\r\n", cal);

		status = set_ftp(appkey, fw, 1.0f);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}

		printf("ftp set\r\n");

		if (what==DEVICE_VELOTRON)  {
			maxfix = -1;
			for(int i=0; i<MAX_FRONT_GEARS; i++)  {
				if (Chainrings[i]==0)  {
					break;
				}
				maxfix++;
			}

			maxrix = -1;
			for(int i=0; i<MAX_REAR_GEARS; i++)  {
				if (cogset[i]==0)  {
					break;
				}
				maxrix++;
			}

			bp = 1;

			fix = maxfix;									// whatever you want the default gears to be
			rix = 0;											// whatever you want the default gears to be

			status = SetVelotronParameters(
				appkey,
								fw,
								maxfix+1,
								maxrix+1,
								Chainrings, 
								cogset, 
								wheeldiameter,				// mm
								ActualChainring, 
								Actualcog,
								bike_kgs,
								//person_kgs,
								fix,						// 2, start off in gear 56/11
								rix						// 9, start off in gear 56/11
								);												// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			if (status!=ALL_OK)  {
				cptr = get_errstr(status);
				throw(fatalError(__FILE__, __LINE__, cptr));
			}

			printf("velotron parameters set\r\n");

		}					// if (what==DEVICE_VELOTRON)  {
	}						// if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {

	//n = check_maps("trainer.cpp, init(), before startTrainer()");							//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	status = startTrainer(appkey);										// start trainer <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (status!=ALL_OK && status != DEVICE_ALREADY_RUNNING)  {
		cptr = get_errstr(status);
		racermate_close();
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	if(what == DEVICE_VELOTRON) {
		status = setGear(appkey, fw, fix, rix);				// go in to starting gear
		if(status != ALL_OK) {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
	}

	//n = check_maps("trainer.cpp, init(), after startTrainer()");							//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	strncpy(udpkey, get_udpkey(appkey), sizeof(udpkey)-1);

	if (what==DEVICE_COMPUTRAINER)  {
		printf("computrainer started\r\n");
	}
	else  {
		printf("velotron started\r\n");
	}

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		status = resetTrainer(appkey, fw, cal);									// resets ds, decoder, etc
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
		printf("trainer reset\r\n");
	}

	status = ResetAverages(appkey, fw);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("averages reset\r\n");

	slope = 100;			// set slope to 1%

	status = SetSlope(appkey, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);	// sets windload mode
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("wind load mode set\r\n");

	status = setPause(appkey, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	
	printf("paused\r\n");

	Sleep(500);

	//n = check_maps("trainer.cpp, init() after setPaused 1");			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	status = setPause(appkey, false);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	//n = check_maps("trainer.cpp, init() after setPaused 2");			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifdef _DEBUG
	printf("unpaused\r\n");
#endif

	float wind_mph = 10.0f;		// in mph

	status = set_wind(appkey, fw, (float)(TOKPH*wind_mph));
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	//n = check_maps("trainer.cpp, init() after setwind");			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	status =  SetHRBeepBounds(appkey, fw, 39, 139, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	//n = check_maps("trainer.cpp, init() after SetHRBeepBounds");			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	keys = GetHandleBarButtons(appkey, fw);			// keys should be accumulated keys pressed since last call
	if (keys==BAD_FIRMWARE_VERSION)  {
		cptr = get_errstr(keys);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	//n = check_maps("trainer.cpp, init() after GetHandleBarButtons");			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	return 0;
}


