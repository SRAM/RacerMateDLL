
#ifndef _MONARCH_H_
#define _MONARCH_H_

#ifdef WIN32
#include <windows.h>
#endif


/******************************************************************************
	for each pedal revolution, a distance of 6.0 meters is covered.
******************************************************************************/

#ifdef WIN32
class __declspec(dllexport) Monarch  {
#else
class Monarch  {
#endif

	friend class Wingate;
	friend class Physics;


	private:
		#define MINSECS 5
		#define MAXSECS 300
		#define MAXKGS 12.0f
		#define MODEL 864

		//double basketWeight_kgs;
		//double total_kgs;						// basketWeight_kgs + kgs

		int frontTeeth;
		int rearTeeth;

		double pedal_rpm;
		double flywheel_rpm;
		double hr;
		int seconds;
		double kph;
		double km;		// kilometers
		double calories;
		double watts;
		double flywheel_torque;
		double pedal_torque;
		double torque;


		double weightFactor;
		double weights_kg[3];			// = {1.0f, .5f, .1f };
		double gear_ratio;				// front teeth / rear teeth
		double radius;					// flywheel radius in meters
		double flywheel_force;
		double rider_weight;			// in kgs
		double dropped_weight;			// in kgs

	public:
		Monarch(double _kgs=2.0f);
		virtual ~Monarch();
		void set_dropped_weight(double _kgs);

};



#endif		// #ifndef _MONARCH_H_
