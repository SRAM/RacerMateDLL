
#include <glib_defines.h>
#include <signalstrengthmeter.h>


/****************************************************************************

****************************************************************************/

signalStrengthMeter::signalStrengthMeter(int _n)  {			// eg, 300
	n = _n;
	min = FLT_MAX;
	max = -FLT_MAX;
	first = true;
	samplecount = n;
	strength = 0.0;
	filter = new LPFilter(3);
}

		/****************************************************************************

		****************************************************************************/

	signalStrengthMeter::~signalStrengthMeter()  {
			DEL(filter);
		}

		/****************************************************************************

		****************************************************************************/

	double signalStrengthMeter::compute(double in)  {
			samplecount--;
			if (samplecount==0)  {
				if (first)  {
					first = false;
					filter->setfval(0.0);				// prime the filter with the first reading, so it won't ramp
					strength = 0.0;
				}
				else  {
					strength = filter->calc(max-min);
				}

				samplecount = n;
				max = -FLT_MAX;
				min = FLT_MAX;
			}
			if (in > max)  {
				max = in;
			}
			if (in < min)  {
				min = in;
			}

			return strength;
		}



