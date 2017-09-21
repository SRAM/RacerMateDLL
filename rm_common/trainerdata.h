#ifndef _TRAINER_DATA_H_
#define _TRAINER_DATA_H_

//#ifdef QT_CORE_LIB
#if 1
namespace RACERMATE  {
#endif

struct TrainerData	{
	float kph;			// ALWAYS in KPH, application will metric convert. <0 on error
	float cadence;		// in RPM, any number <0 if sensor not connected or errored.
	float HR;			// in BPM, any number <0 if sensor not connected or errored.
	float Power;		// in Watts <0 on error
};


//#ifdef QT_CORE_LIB
#if 1
}
#endif
#endif

