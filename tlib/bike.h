
#ifndef _BIKE_H_
#define _BIKE_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <stdio.h>

class Bike  {
	friend class EV;

	public:
		#define MAX_FRONT_GEARS 5
		#define MAX_REAR_GEARS 20

		struct GEARPAIR  {
			int front;					// number of front teeth
			int rear;					// number of rear teeth
		};

		typedef struct  {
			int nfront;					// number of virtual front teeth
			int nrear;					// number of virtual rear teeth
			int front_gears[MAX_FRONT_GEARS];			// front teeth array
			int rear_gears[MAX_REAR_GEARS];		// rear teeth array
			int front_ix;				// front index
			int rear_ix;				// rear index
			int real_front_teeth;
			int real_rear_teeth;
			float kgs;											// weight of the bike in kgs
			float tire_diameter_mm;
		} PARAMS;

		static bool gblog;
		static bool gbgearslog;

	private:

		typedef struct  {
			PARAMS params;
			GEARPAIR gp;
			int gear_inches_ix;
			int ngears;						// number of virtual gears = params.nfront * params.nrear

			double gearRatio;
			float length;
			double PERIOD_TO_RPM;
			double COUNT6;
			int segments, holesPerSegment, totalHoles;
			double rearTireCircumferenceInMiles;
			double rearTireCircumferenceInFeet;
			double timePerPicCount;
			double K0;
			double K1;
			double gear_inches;
		} STATE;											// 512 bytes
		
		int id;
		char str[256];
		bool upshiftkey;
		bool downshiftkey;
		DWORD edgetime;
		DWORD now;
		bool shifted;
		FILE *logstream;
		void setupVirtualGearInches(void);
		PARAMS default_params;
		bool override_gear_tables;
		//double gear_inches;

	public:
		Bike(int _id=0, PARAMS *_params=NULL);
		virtual ~Bike();

		static int front_defaults[MAX_FRONT_GEARS];
		static int rear_defaults[MAX_REAR_GEARS];

		bool realtime;
		STATE state;
		double *gear_inches_array;

		void computeGearRatio(void);

		// get

		inline double get_kgs(void) { return state.params.kgs; }
		inline int get_nfront(void)  { return state.params.nfront; }
		inline int get_nrear(void)  { return state.params.nrear; }
		inline int get_front_gear(void)  { return state.gp.front; }									// returns the virtual front gear teeth we are currently on
		inline int get_rear_gear(void)  { return state.gp.rear; }										// returns the virtual rear gear teeth we are currently on

		inline int get_front_index(void)  { return state.params.front_ix; }									// returns the virtual front gear teeth we are currently on
		inline int get_rear_index(void)  { return state.params.rear_ix; }										// returns the virtual rear gear teeth we are currently on

		inline float get_gear_inches(void)  { return (float)gear_inches_array[state.gear_inches_ix];  }
		inline int getid(void)  { return id; }

		// set

		void set_gear(int _front_index, int _rear_index);
		void set_gear(double _gear_inches);
		void set_kgs(float _kgs) { state.params.kgs = _kgs; }

		void frontUp(void);
		void rearUp(void);
		void frontDown(void);
		void rearDown(void);
		void upkeydown(void);
		int upkeyup(void);
		void downkeydown(void);
		int downkeyup(void);
		int run(void);
		bool changed;

};

#endif		// #ifndef _BIKE_H_

