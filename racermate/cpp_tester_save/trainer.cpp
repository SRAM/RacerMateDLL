
#include <stdlib.h>
#include <assert.h>

#include <glib_defines.h>
#include <fatalerror.h>

#include <dll_globals.h>
#include <errors.h>

#include "trainer.h"

int Trainer::Chainrings[MAX_FRONT_GEARS] = {	28, 39, 56, 0, 0 };																			// up to 5 front gears
int Trainer::cogset[MAX_REAR_GEARS] = { 11, 12, 13, 14, 15, 16, 17, 19, 21, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };			// up to 20 rear gears

/****************************************************************************

****************************************************************************/

Trainer::Trainer(void)  {
	what = DEVICE_NOT_SCANNED;
	ix = "zzz";
	iport = -1;
	memset(portname, 0, sizeof(portname));
	memset(keystr, 0, sizeof(keystr));
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
	broadcast_port = -1;
	listen_port = -1;
	wind = 0.0f;								// float, kph
	draft_wind = 0.0f;						// float, kph

	/*
	iwind_kph = 0;
	idraft_wind_kph = 0;

	iwind_mph = 0;
	idraft_wind_mph = 0;

	wind_kph = 0.0f;
	draft_wind_kph = 0.0f;

	wind_mph = 0.0f;
	draft_wind_mph = 0.0f;
	*/

	bp = 0;

	return;
}							// constructor

/****************************************************************************

****************************************************************************/

int Trainer::init(const char * _ix, int _broadcast_port, int _listen_port)  {
	ix = _ix;
	broadcast_port = _broadcast_port;
	listen_port = _listen_port;

	const char *cptr;
	int status;
	bool b;

	//iport = ix + 1;
	iport = 1;
	#ifdef WIN32
		sprintf(portname, "COM%d", iport);
	#elif defined __MACH__
	#else
	#endif

	if (iport>=CLIENT_SERIAL_PORT_BASE && iport <= CLIENT_SERIAL_PORT_BASE+16)  {			// trainer is server
		comtype = TRAINER_IS_SERVER;																			// trainer is a server
	}
	else if (iport>=SERVER_SERIAL_PORT_BASE && iport <= SERVER_SERIAL_PORT_BASE+16)  {	// trainer is client
		comtype = TRAINER_IS_CLIENT;																			// trainer is client
	}
	else if (iport >= UDP_SERVER_SERIAL_PORT_BASE && iport <= UDP_SERVER_SERIAL_PORT_BASE + 16) {								// trainer is UDP client
		comtype = TRAINER_IS_CLIENT;																			// trainer is client
	}
	else  {
		comtype = WIN_RS232;
	}


	if (comtype==TRAINER_IS_CLIENT)  {						// trainer is a client
		int debug_level = 2;

		status = set_network_parameters(
			broadcast_port,				// 9071
			listen_port,					// 9099
			false,								// ip_discover
			true,								// udp flag
			ix,
			debug_level
			);
			assert(status==ALL_OK);
	}

	bp = 17;

	what = GetRacerMateDeviceID(ix);

	if (what == DEVICE_COMPUTRAINER)  {
		printf("found computrainer on port %s\r\n", portname);
	}
	else if (what == DEVICE_VELOTRON)  {
		printf("found velotron on port %s\n", portname);
	}
	else if (what == DEVICE_EXISTS)  {
		sprintf(gteststring, "don't see trainer on port %s\r\n", portname);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}
	else  {
		sprintf(gteststring, "no trainer on port %s", portname);
		throw(fatalError(__FILE__, __LINE__, gteststring));
	}

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		fw = GetFirmWareVersion(ix);
		if (comtype==TRAINER_IS_CLIENT)  {						// trainer is a client
			assert(fw==4543);
		}
		else  {
			assert(fw==4095);
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

	b = GetIsCalibrated(ix, fw);									// false
	printf("calibrated = %s\r\n", b?"true":"false");

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		cal = GetCalibration(ix);										// 200
		printf("cal = %d\r\n", cal);

		status = set_ftp(ix, fw, 1.0f);
		if (status!=DEVICE_NOT_RUNNING)  {
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
								ix,
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

			status = setGear(ix, fw, fix, rix);				// go in to starting gear

			if (status!=ALL_OK)  {
				cptr = get_errstr(status);
				throw(fatalError(__FILE__, __LINE__, cptr));
			}
		}					// if (what==DEVICE_VELOTRON)  {
	}						// if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {

	status = startTrainer(ix);										// start trainer <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (status!=ALL_OK && status != DEVICE_ALREADY_RUNNING)  {
		cptr = get_errstr(status);
		racermate_close();
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	if (what==DEVICE_COMPUTRAINER)  {
		printf("computrainer started\r\n");
	}
	else  {
		printf("velotron started\r\n");
	}

	if (what==DEVICE_COMPUTRAINER || what==DEVICE_VELOTRON)  {
		status = resetTrainer(ix, fw, cal);									// resets ds, decoder, etc
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			throw(fatalError(__FILE__, __LINE__, cptr));
		}
		printf("trainer reset\r\n");
	}

	status = ResetAverages(ix, fw);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	printf("averages reset\r\n");

	slope = 100;			// set slope to 1%

	status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, slope/100.0f);	// sets windload mode
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("wind load mode set\r\n");

	status = setPause(ix, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}
	
	printf("paused\r\n");

	Sleep(500);

	status = setPause(ix, false);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	printf("unpaused\r\n");

	float wind_mph = 10.0f;		// in mph

	status = set_wind(ix, fw, (float)(TOKPH*wind_mph));
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	status =  SetHRBeepBounds(ix, fw, 39, 139, true);
	if (status!=ALL_OK)  {
		cptr = get_errstr(status);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}


	keys = GetHandleBarButtons(ix, fw);			// keys should be accumulated keys pressed since last call
	if (keys==BAD_FIRMWARE_VERSION)  {
		cptr = get_errstr(keys);
		throw(fatalError(__FILE__, __LINE__, cptr));
	}

	return 0;
}


