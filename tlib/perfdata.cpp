
#include <glib_defines.h>

#include <perfdata.h>

PerfData::PerfData(void)  {
	StatFlags = StatFlags::Mask;
	Bars = new float[24];
	AverageBars = new float[24];
	return;
}


PerfData::~PerfData()  {
	DELARR(Bars);
	DELARR(AverageBars);
	RiderName.clear();
	Course.clear();
}


