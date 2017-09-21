
#ifdef _WIN32
	#define STRICT
	#include <windows.h>
#endif

#include <glib_defines.h>
#include <monarch.h>

/***************************************************************************
	_kgs = the weight in the basket in kgs
***************************************************************************/

Monarch::Monarch(double _kgs)  {

	frontTeeth = 52;
	rearTeeth = 14;
	//basketWeight_kgs = 1.0f;
	dropped_weight = _kgs;
	weightFactor = .133f;
	weights_kg[0] = 1.0f;
	weights_kg[1] = .5f;
	weights_kg[2] = .1f;
		
	// for each pedal revolution, a distance of 6.0 meters is covered.
	gear_ratio = (double)frontTeeth / (double)rearTeeth;
	double c = 6.0f / gear_ratio;
	double d = (double) (c / PI);										// diameter of monarch flywheel in meters = .514 meters
	radius = d/2.0f;											// radius of flywheel in meters, .257 meters
	//radius_feet = (double) (METERSTOFEET * radius_meters);			// .843 feet

	pedal_rpm = flywheel_rpm = kph = km = calories = watts = flywheel_torque = pedal_torque = flywheel_force = rider_weight = hr = 0.0f;


	//--------------------------
	// world record on Monarch:
	//--------------------------

	pedal_rpm = 163.0f;			// pedal rpm
	flywheel_rpm = pedal_rpm * gear_ratio;						// 605.429
	watts = 2034.0f;

	/***********************************

		torque = force * distance
		watts = newton*meters / sec

		newton*meters = watts * sec

	***********************************/

	// time for 1 flywheel rev

	flywheel_torque = watts / flywheel_rpm;					// 3.3596 Newton-Meters

	flywheel_force = flywheel_torque / radius;				// 13.0675 Newtons
	rider_weight = flywheel_force / weightFactor;			// 98.2518 kgs
	//double rider_lbs = (double) (KGTOLBS * rider_weight);	// 216.606 lbs


	return;

}


/***************************************************************************

***************************************************************************/

Monarch::~Monarch()  {

}


/***************************************************************************

***************************************************************************/

void Monarch::set_dropped_weight(double _kgs)  {

	dropped_weight = _kgs;
	torque = radius * dropped_weight;
	return;
}


