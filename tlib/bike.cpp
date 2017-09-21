
#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
#else
#include <string>
#endif

// in glib:
#include <oldsdirs.h>

// in <common>
#include <comdefs.h>
#include <comglobs.h>

// in tlib:

#include "bike.h"

int Bike::front_defaults[MAX_FRONT_GEARS] = { 28, 39, 56, 0, 0 };
int Bike::rear_defaults[MAX_REAR_GEARS] = { 23, 21, 19, 17, 16, 15, 14, 13, 12, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool Bike::gblog=false;
bool Bike::gbgearslog=false;

/**********************************************************************

**********************************************************************/

Bike::Bike(int _id, PARAMS *_params)  {
	int i;

	id = _id;
	memset(&state, 0, sizeof(state));
	override_gear_tables = false;
	//gear_inches = 0.0;

	if (_params)  {
		memcpy(&state.params, _params, sizeof(state.params));
	}
	else  {											// set up default params;
		default_params.nfront = 3;
		default_params.nrear = 10;
		default_params.front_ix = 2;
		default_params.rear_ix = 9;
		default_params.real_front_teeth = 62;
		default_params.real_rear_teeth = 13;
		default_params.kgs = (float)(TOKGS*20);
		default_params.tire_diameter_mm = 700.0f;
		for(i=0; i<MAX_FRONT_GEARS; i++)  {
			default_params.front_gears[i] = front_defaults[i];
		}

		for(i=0; i<MAX_REAR_GEARS; i++)  {
			default_params.rear_gears[i] = rear_defaults[i];
		}

		memcpy(&state.params, &default_params, sizeof(state.params));
	}


	if (state.params.real_front_teeth==0)  {
		state.params.real_front_teeth = 62;
	}



	logstream = NULL;

	if (gblog)  {
		if (id != 0)  {										// computrainer doesn't have a real bike
#ifdef DOGLOBS
			sprintf(str, "%s%sbike%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
			sprintf(str, "%s%sbike%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
			logstream = fopen(str, "wt");
		}
	}

	gear_inches_array = NULL;
	realtime = true;

	state.length = 6.5;
	state.gear_inches_ix = 0;
	state.COUNT6 = 2.17008002e-6;
	state.segments = 12;
	state.holesPerSegment = 6;
	state.totalHoles = state.segments * state.holesPerSegment;
	state.timePerPicCount = state.COUNT6;

	upshiftkey = false;
	downshiftkey = false;
	edgetime=0;
	now = 0;
	shifted = false;

	state.rearTireCircumferenceInFeet = (PI * MMTOINCHES*state.params.tire_diameter_mm) / 12.0;
	state.rearTireCircumferenceInMiles = (PI * MMTOINCHES*state.params.tire_diameter_mm) / (12.0 * 5280.0);

	setupVirtualGearInches();

	computeGearRatio();
	state.gear_inches = gear_inches_array[state.gear_inches_ix];
	state.K0 = (PI * state.params.tire_diameter_mm) / (12*5280) * (3600.0 / (12.0 * state.COUNT6));	// constant used to compute mph
	state.K1 = 60.0f / (state.gearRatio*12.0*state.COUNT6);	// constant used to compute rpm:

}

/**********************************************************************

**********************************************************************/

Bike::~Bike() {

	if (gear_inches_array)  {
		delete [] gear_inches_array;
		gear_inches_array = NULL;
	}

	FCLOSE(logstream);
}

/**********************************************************************

**********************************************************************/

void Bike::computeGearRatio(void)  {
	state.gearRatio = (double)state.params.real_front_teeth / (double)state.params.real_rear_teeth;
	// constant used to compute rpm (function of gear ratio)
	state.PERIOD_TO_RPM = 60.0f / (state.gearRatio * 12.0 * state.COUNT6);

	return;
}


/**********************************************************************
	new front up shifter
	nca+++ 2017/09/20 ver 4.1.0
  	redefined "up" as going to greater inches/turn,
	bigger front chainring, which is smaller index.
	Swapped frontUp and frontDown function names
**********************************************************************/

void Bike::frontDown(void)  {				// was frontUp

	state.params.front_ix++;

	if (state.params.front_ix >= state.params.nfront)  {
		state.params.front_ix = state.params.nfront - 1;
	}
	state.gp.front = state.params.front_gears[state.params.front_ix];

	state.gear_inches_ix = state.params.front_ix * state.params.nrear + state.params.rear_ix;

	state.gear_inches = gear_inches_array[state.gear_inches_ix];

	return;
}

/**********************************************************************
	new rear up shifter
**********************************************************************/

void Bike::rearUp(void)  {

	state.params.rear_ix++;
	if (state.params.rear_ix >= state.params.nrear)  {
		state.params.rear_ix = state.params.nrear - 1;
	}
	state.gp.rear = state.params.rear_gears[state.params.rear_ix];

	state.gear_inches_ix = state.params.front_ix * state.params.nrear + state.params.rear_ix;
	state.gear_inches = gear_inches_array[state.gear_inches_ix];

	return;
}

/**********************************************************************
	new front down shifter
**********************************************************************/

void Bike::frontUp(void)  {				// was frontDown

	state.params.front_ix--;

	if (state.params.front_ix < 0)  {
		state.params.front_ix = 0;
	}
	state.gp.front = state.params.front_gears[state.params.front_ix];

	state.gear_inches_ix = state.params.front_ix * state.params.nrear + state.params.rear_ix;
	state.gear_inches = gear_inches_array[state.gear_inches_ix];

	return;
}

/**********************************************************************
	new rear down shifter
**********************************************************************/

void Bike::rearDown(void)  {

	state.params.rear_ix--;
	if (state.params.rear_ix < 0)  {
		state.params.rear_ix = 0;
	}
	state.gp.rear = state.params.rear_gears[state.params.rear_ix];

	state.gear_inches_ix = state.params.front_ix * state.params.nrear + state.params.rear_ix;
	state.gear_inches = gear_inches_array[state.gear_inches_ix];

	return;
}

/**********************************************************************

**********************************************************************/



/**********************************************************************
[Virtual Front Teeth]
1=28
2=39
3=56
[Virtual Rear Teeth]
1=23
2=21
3=19
4=17
5=16
6=15
7=14
8=13
9=12
10=11
**********************************************************************/

void Bike::setupVirtualGearInches(void)  {
	int i, j;

	state.gp.front = state.params.front_gears[state.params.front_ix];
	state.gp.rear = state.params.rear_gears[state.params.rear_ix];

	state.ngears = state.params.nfront * state.params.nrear;

	DEL(gear_inches_array);

	gear_inches_array = new double[(unsigned int)state.ngears];

	// now set up the gear table:

	double ratio;
	int index = 0;

	for(i=0;i<state.params.nfront;i++)  {
		for(j=0; j<state.params.nrear;j++)  {
			ratio = (double)state.params.front_gears[i] / (double)state.params.rear_gears[j];
			gear_inches_array[i*state.params.nrear + j] = MMTOINCHES*state.params.tire_diameter_mm * ratio;
		}
	}

	state.gear_inches_ix = state.params.front_ix * state.params.nrear + state.params.rear_ix;
	state.gear_inches = gear_inches_array[state.gear_inches_ix];
	{

		if (gbgearslog)  {
			if (id != 0)  {
				FILE *stream;

				char str[256];
#ifdef DOGLOBS
				sprintf(str, "%s%sgears%d.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#else
				sprintf(str, "%s%sgears%d.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR, id);
#endif
				stream = fopen(str, "wt");

				fprintf(stream, "\n");

				for(i=0;i<state.params.nfront;i++)  {
					for(j=0;j<state.params.nrear;j++)  {
						index = i*state.params.nrear + j;
						fprintf(stream, "%2d %6.2f			%5d / %-4d\n", index, gear_inches_array[i*state.params.nrear + j], state.params.front_gears[i], state.params.rear_gears[j]);
					}
				}

				fprintf(stream, "\n\nFront Gears\n\n");

				for(i=0; i<state.params.nfront; i++)  {
					fprintf(stream, "%2d   %d\n", i, state.params.front_gears[i]);
				}

				fprintf(stream, "\n\nRear Gears\n\n");

				for(i=0; i<state.params.nrear; i++)  {
					fprintf(stream, "%2d   %d\n", i, state.params.rear_gears[i]);
				}

				fclose(stream);
			}
		}

	}

	return;
}

/***************************************************************************

***************************************************************************/

int Bike::run(void)  {

	if (upshiftkey)  {
		if (!shifted)  {
			if (realtime)  {
				now = timeGetTime();
				if ((now-edgetime) >= 1000)  {
					frontUp();
					shifted = true;
					return 1;
				}
			}
			else  {
				now++;
				if ((now-edgetime) >= 200)  {
					now = 0;
					frontUp();
					shifted = true;
					return 1;
				}
			}
		}
	}
	else if (downshiftkey)  {
		if (!shifted)  {

			if (realtime)  {
				now = timeGetTime();
				if ((now-edgetime) >= 1000)  {
					frontDown();
					shifted = true;
					return 1;
				}
			}
			else  {
				now++;
				if ((now-edgetime) >= 200)  {
					now = 0;
					frontDown();
					shifted = true;
					return 1;
				}
			}
		}
	}

	return 0;
}				// run

/***************************************************************************

***************************************************************************/

void Bike::upkeydown(void)  {

	if (!upshiftkey)  {
		upshiftkey = true;
		if (realtime)  {
			edgetime = timeGetTime();
		}
		else  {
			edgetime = 0;
		}
	}
	return;
}

/***************************************************************************

***************************************************************************/

int Bike::upkeyup(void)  {

	if (!shifted)  {
		rearUp();
		upshiftkey = false;
		shifted = false;
		return 1;
	}
	upshiftkey = false;
	shifted = false;

	return 0;
}

/***************************************************************************

***************************************************************************/

int Bike::downkeyup(void)  {

	if (!shifted)  {
		rearDown();
		downshiftkey = false;
		shifted = false;
		return 1;
	}
	downshiftkey = false;
	shifted = false;
	return 0;
}


/***************************************************************************

***************************************************************************/

void Bike::downkeydown(void)  {

	if (!downshiftkey)  {
		downshiftkey = true;
		if (realtime)  {
			edgetime = timeGetTime();
		}
		else  {
			edgetime = 0;
		}
	}
	return;
}


/***************************************************************************

***************************************************************************/

void Bike::set_gear(int _front_index, int _rear_index)  {
	//int ix;

	override_gear_tables = false;

	state.params.rear_ix = _rear_index;
	if (state.params.rear_ix < 0 || state.params.rear_ix >= state.params.nrear )  {
		state.params.rear_ix = 0;
	}
	state.gp.rear = state.params.rear_gears[state.params.rear_ix];

	state.params.front_ix = _front_index;
	if (state.params.front_ix < 0 || state.params.front_ix >= state.params.nfront )  {
		state.params.front_ix = 0;
	}
	state.gp.front = state.params.front_gears[state.params.front_ix];

	state.gear_inches_ix = state.params.front_ix * state.params.nrear + state.params.rear_ix;
	state.gear_inches = gear_inches_array[state.gear_inches_ix];
	if (state.gear_inches_ix < state.ngears && state.gear_inches_ix >= 0)  {
	}


	return;
}


/***************************************************************************

***************************************************************************/

void Bike::set_gear(double _gear_inches)  {
	override_gear_tables = true;
	//gear_inches = _gear_inches;
	state.gear_inches = _gear_inches;
	return;
}


