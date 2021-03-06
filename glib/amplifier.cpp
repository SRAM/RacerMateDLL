#include <glib_defines.h>
#include <amplifier.h>

/*********************************************************************

********************************************************************/

Amplifier::Amplifier(double _gain)  {
	gain = _gain;
	avg = 0.0;
	output = 0.0;
	filter = new LPFilter(100);

}

/*********************************************************************

********************************************************************/

Amplifier::~Amplifier()  {
	DEL(filter);
}

/*********************************************************************

********************************************************************/

double Amplifier::calc(double in)  {

	avg = filter->calc(in);

	output = -gain * (avg - in) + avg;

	return output;

}

