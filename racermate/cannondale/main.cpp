
#pragma warning(disable:4996 4006)

#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <string>

#include "cannondale.h"

#define COMPUTRAINER_PORT 6
#define TOKGS	(1.0 / 2.2046)
#define TOMPH	(1000.0 / (.3048 * 5280.0))		// approx .62137

char gstring[2048];
std::vector<std::string> dirs;


/*********************************************************************************************************

*********************************************************************************************************/

int main(int argc, char *argv[])  {
	int rc = 0;
	int port, ix, fw, cal;
	int i, status, cnt;
	bool b;
    bool flag = true;
	float watts;
	float rpm, hr, mph;
	float *average_bars;
	float seconds;
	const char *cptr;
	unsigned char cb = 0;
	unsigned long display_count = 0L;
	unsigned long start_time;
	unsigned long ms = 50L;
	unsigned long sleep_ms = 5L;
	unsigned long now, last_display_time = 0;
	char c;
	EnumDeviceType what;
	SSDATA ssdata;
	TrainerData td;


	try  {
		for(i=0; i<NDIRS; i++)  {
			dirs.push_back(".");
		}

		dirs[DIR_PROGRAM] = ".";
		dirs[DIR_PERSONAL] = ".";
		dirs[DIR_SETTINGS] = ".";
		dirs[DIR_REPORT_TEMPLATES] = ".";
		dirs[DIR_REPORTS] = ".";
		dirs[DIR_COURSES] = ".";
		dirs[DIR_PERFORMANCES] = ".";
		dirs[DIR_DEBUG] = ".";
		dirs[DIR_HELP] = ".";
		status = Setlogfilepath(".");
		if (status != ALL_OK)  {
			printf("error in %s at %d\n", __FILE__, __LINE__);
			exit(1);
		}

		bool _bikelog = false;
		bool _courselog = false;
		bool _decoderlog = false;
		bool _dslog = false;
		bool _gearslog = false;
		bool _physicslog = false;


		status = Enablelogs(_bikelog, _courselog, _decoderlog, _dslog, _gearslog, _physicslog);
		if (status != ALL_OK)  {
			printf("error in %s at %d\n", __FILE__, __LINE__);
			exit(1);
		}

		port = COMPUTRAINER_PORT;
		ix = port - 1;

		what = check_for_trainers(port);
		assert(what==DEVICE_COMPUTRAINER);

		printf("getting device id\r\n");

		what = GetRacerMateDeviceID(ix);
		if (what != DEVICE_COMPUTRAINER)  {
			printf("error in %s at %d, cant' find computrainer on port %d\n", __FILE__, __LINE__, port);
			exit(1);
		}


		printf("found computrainer\r\n");

		fw = GetFirmWareVersion(ix);
		if (FAILED(fw))  {
			cptr = get_errstr(fw);
			sprintf(gstring, "%s", cptr);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}
		printf("firmware = %d\r\n", fw);

		cptr = GetAPIVersion();

		cptr = get_dll_version();

		b = GetIsCalibrated(ix, fw);

		cal = GetCalibration(ix);

		status = startTrainer(ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

		printf("computrainer started\r\n");

		status = resetTrainer(ix, fw, cal);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

		status = ResetAverages(ix, fw);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

		float bike_kgs = (float)(TOKGS*20.0f);
		float person_kgs = (float)(TOKGS*150.0f);
		int drag_factor = 100;

		status = setPause(ix, true);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

		Sleep(500);

		status = setPause(ix, false);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

		status = SetSlope(ix, fw, cal, bike_kgs, person_kgs, drag_factor, 1.0f);		// set 1% grade
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

		cnt = 0;

		start_time = timeGetTime();

		while(flag)  {
			if (kbhit())  {
				c = getch();
				if (c==0)  {
					c = getch();
					continue;
				}
				if (c==VK_ESCAPE)  {
					flag = false;
					break;
				}
			}

			now = timeGetTime();

			td = GetTrainerData(ix, fw);
			watts = td.Power;
			mph = (float)(TOMPH*td.kph);
			rpm = td.cadence;
			hr = td.HR;

			average_bars = get_average_bars(ix, fw);
			ssdata = get_ss_data(ix, fw);

			if ( (now - last_display_time) >= 500)  {
				last_display_time = now;
				seconds = (float) ((now - start_time)/1000.0f) ;
				printf("%6.1f  %6.1f  %6.1f  %6.1f  %6.1f\r\n",seconds, rpm, mph, watts, hr);
			}

			Sleep(sleep_ms);
		}


		status = stopTrainer(ix);
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}
		printf("computrainer stopped\n");

		status = ResettoIdle(ix);										// not really necessary, but doesn't hurt
		if (status!=ALL_OK)  {
			cptr = get_errstr(status);
			printf("error in %s at %d: %s\n", __FILE__, __LINE__, cptr);
			exit(1);
		}

	}						// try
	catch (const char *str) {
		rc = 1;
		printf("\nconst char *exception: %s\n\n", str);
	}
	catch (int &i)  {
		rc = 1;
		printf("\nInteger Exception (%d)\n\n", i);
	}
	catch (...)  {
		rc = 1;
		printf("\nUnhandled Exception\n\n");
	}

	printf("\nhit a key...");

	_getch();

	return rc;
}






