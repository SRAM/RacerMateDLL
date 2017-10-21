
#ifdef WIN32
	#define STRICT
	#define WIN32_LEAN_AND_MEAN
	#pragma warning(disable:4996)					// for vista strncpy_s
	#include <windows.h>
	#include <mmsystem.h>
#else
	#include <minmax.h>
#endif

#include <float.h>
#include <math.h>
#include <assert.h>

#include <comutils.h>

#include <glib_defines.h>
#include <round.h>
#include <utils.h>
#include <fatalerror.h>
#include <ss.h>
#include <globals.h>

/**********************************************************************

**********************************************************************/

SS::SS(void)  {
	int i;
	//double dtemp;

	filteredTorques = new float[NBARS];
	thisRevForces = new float[NBARS];
	thrust = new THRUST[NBARS];


	memset(bars2, 0, NBARS*sizeof(float));
	memset(total_bars2, 0, NBARS*sizeof(double));
	memset(bars2_count, 0, NBARS*sizeof(unsigned long));

	reset_flag = true;

	this_rev_max = -FLT_MAX;

	at = new AverageTimer("SS");

	#ifdef _DEBUG
		//log = new Logger("ss.log", logpath);
		log = NULL;
	#else
		log = NULL;
	#endif

	SSShift = 6;		//nca: was 18;

	middle = NBARS / 2;
	degperseg = 360.0 / (double)NBARS;				// 15 degrees
	metric = FALSE;
	TCD = 16.0f;		// denominator (16.0f)
	TCN = .27f;			// numerator (.27f)

	force = new float[600];
	lastTwoRevForces = new float[(unsigned int) (NBARS*2) ];
	counts = new int[(unsigned int)NBARS];
	rpmMav = new sMav(7);
	wattsFilter = new LPFilter(30);		// was 30

	tpf = new twoPoleFilter *[NBARS];
	for(i=0;i<NBARS;i++)  {
		tpf[i] = new twoPoleFilter(100);		// argument is unused, so it doesn't matter
	}

	reset();

#if 1
	double beginning_radians, center_radians;
	double beginning_degrees, center_degrees;

	for(i=0;i<NBARS;i++)  {
		beginning_degrees = degperseg*i;
		center_degrees = degperseg*i + .5*degperseg;

		beginning_radians = DEGTORAD*beginning_degrees;
		center_radians = DEGTORAD*center_degrees;

		thrust[i].beginning_degrees = (float)beginning_degrees;
		thrust[i].center_degrees = (float)center_degrees;

		thrust[i].beginning_radians = (float)beginning_radians;
		thrust[i].center_radians = (float)center_radians;

		thrust[i].beginning_sin = (float)sin(beginning_radians);
		thrust[i].beginning_cos = (float)cos(beginning_radians);
		thrust[i].beginning_sin_rotated = (float)sin(beginning_radians+(PI/2.0));
		thrust[i].beginning_cos_rotated = (float)cos(beginning_radians+(PI/2.0));

		thrust[i].center_sin = (float)sin(center_radians);
		thrust[i].center_cos = (float)cos(center_radians);
		thrust[i].center_sin_rotated = (float)sin(center_radians+(PI/2.0));
		thrust[i].center_cos_rotated = (float)cos(center_radians+(PI/2.0));
	}
#endif


	size = sizeof(*this);		// 4
}								// constructor



/**********************************************************************
	destructor
**********************************************************************/

SS::~SS()  {

	DEL(rpmMav);

	DELARR(lastTwoRevForces);
	DELARR(counts);
	DELARR(force);
	DEL(wattsFilter);
	DEL(at);
	DEL(log);

	if (tpf)  {
		for(int i=0;i<NBARS;i++)  {
			DEL(tpf[i]);
		}
		delete [] tpf;
		tpf = NULL;
	}

	DELARR(filteredTorques);
	DELARR(thisRevForces);
	DELARR(thrust);

	return;
}

/*-----------------------------------------------------------------------------------------
	called every 5 ms when a new record has arrived.

	ONLY called by velotronDecoder::decode():
		run(physics->rpmFromHoles, physics->watts, meta->tdc);		// make sure to use the unfiltered watts for spinscan!!!

-----------------------------------------------------------------------------------------*/

//float SS::run(double _watts, double _force, double _rpm, unsigned char _tdc)  {
float SS::run(double _watts, double _force, double _rpm, METADATA *_meta)  {

	//avgTimer->update();							// result was 5 ms

	ssrec++;

#if 1
	thisforce = (float)_force;
#else
	double watts;
	watts = wattsFilter->calc(_watts);			// filter length is 0 (no filtering)
	//watts = amp->calc(watts);

	if (_rpm > 0)  {
		/*
			kw = nm * 2*pi * rpm / 60000
			nm*2*pi*rpm = kw *60000
			nm*2*pi*rpm = w*60
			nm = (w*60) / (2*pi*rpm)		newton meters
		*/

		thisforce = (float) (watts / _rpm);
		//thisforce *= 1.3f;
		//thisforce = (float) (.737562175569 * ( (60.0f*watts) / (2.0*PI*rpm) ));			// this should be the actual formula, but it doesn't matter
	}
	else  {
		thisforce = 0.0f;
	}
#endif


	if (_meta->tdc)  {
		if (peddlingTimeout)  {
			peddlingTimeout = false;
			inptr = 0;
			force[inptr++] = thisforce;
		}
		else  {
			load(_rpm);
			inptr = 0;
			force[inptr++] = thisforce;
		}
	}
	else  {
		if (peddlingTimeout)  {
		}
		else  {
			force[inptr] = thisforce;
			inptr++;
			if (inptr == 600)  {			// no crank signal for 3 seconds?
				peddlingTimeout = true;
				primed = false;
				loadcount = 0;
				for(int i=0;i<NBARS;i++)  {
					thisRevForces[i] = 0;							// no more data, settle to 0
				}
			}
		}
	}

	int i;


	//-------------------------
	// COMPUTE LEFT ATA
	//-------------------------

	float ftemp1, ftemp2;
	
	ftemp1 = ftemp2 = 0.0f;

	for (i=0; i<middle; i++)  {
		ftemp1 += (float) (filteredTorques[i] * thrust[i].center_degrees);
		ftemp2 += (float) (filteredTorques[i]);
	}

	if (ftemp2!=0.0f)  {
		lata = ftemp1/ftemp2;
	}
	else  {
		lata = 90.0f;
	}

	//-------------------------
	// COMPUTE RIGHT ATA
	//-------------------------

	ftemp1 = ftemp2 = 0.0f;
	for(i=middle; i<NBARS; i++)  {
		ftemp1 += (float) (filteredTorques[i]*thrust[i].center_degrees);
		ftemp2 += (float) (filteredTorques[i]);
	}
	if (ftemp2!=0.0f)  {
		rata = ftemp1/ftemp2 - 180.0f;
	}
	else  {
		rata = 90.0f;
	}

	if (_rpm<=30.0)  {
		lata = rata = 90.0f;
	}

	filter();					// computes leftss and rightss

	_meta->ss = totalss;
	_meta->lss = leftss;
	_meta->rss = rightss;
	_meta->lwatts = leftwatts;
	_meta->rwatts = rightwatts;
	_meta->lata = iround(lata);
	_meta->rata = iround(rata);
	_meta->rescale = rescale;

	return thisforce;
}									// run()

/******************************************************************************
	a new force array is ready which contains about 200 force samples
******************************************************************************/

void SS::load(double rpm)  {
	int i, k;

	loadcount++;

	for(k=0; k<NBARS; k++)  {
		thisRevForces[k] = 0.0f;
		counts[k] = 0;
	}

	// divide into NBARS NBARS

	if (inptr<NBARS)  {
		bp =1;
		return;
	}

	float m, b;
	map(0.0f, (float)inptr, 0.0f, (float)NBARS, &m, &b);


	for(i=0; i<inptr; i++)  {
		k = (int)(m*i + b);
		thisRevForces[k] += force[i];
		counts[k]++;
	}

	for(k=0;k<NBARS;k++)  {
		//nca+++ protect from div zero
		if(counts[k]) {
			thisRevForces[k] /= counts[k];
		}
	}
	
	for(i=0;i<NBARS;i++) lastTwoRevForces[i] = lastTwoRevForces[NBARS+i];
	for(i=0;i<NBARS;i++) lastTwoRevForces[NBARS+i] = thisRevForces[i];

	for(i=0;i<NBARS;i++)  {
		thisRevForces[i] = lastTwoRevForces[SSShift + i];
	}

	
	if (!primed)  {
		if (loadcount >= 2)  {			// give filteredTorques[] time to get two valid cycles
			primed = true;
		}
	}

	//-------------------------------------------
	// detect one foot out of the pedal here:
	//-------------------------------------------

	float lsum = 0.0f;
	float rsum = 0.0f;

	for(i=0;i<NBARS;i++)  {
		if (i<NBARS/2)  {
			lsum += thisRevForces[i];
		}
		else  {
			rsum += thisRevForces[i];
		}
	}

	if (rpm < CUTOFF_RPM)  {
		for(i=0;i<NBARS;i++)  {
			thisRevForces[i] = 0;							// no more data, settle to 0
		}
	}

	return;
}


/**********************************************************************
called by SS::run() every 5 ms with new data only by VELOTRON
**********************************************************************/

void SS::filter(void)  {
	static int count=0;

	// ASSUME that we're being called every 5 ms!!!!

	count++;
	if (count<20)  {
		return;
	}
	count = 0;

	#ifdef _DEBUG
	//avgTimer->update();							// 100 ms, checked july 1, 2008
	#endif

	int k;

	minlthrust = FLT_MAX;
	minrthrust = FLT_MAX;
	maxlthrust = -FLT_MAX;
	maxrthrust = -FLT_MAX;
	float thismax = -FLT_MAX;
	
	if (primed)  {
		for(k=0;k<NBARS;k++)  {
			thrust[k].fval = filteredTorques[k] = (float)tpf[k]->calc(thisRevForces[k]);
			if (k < middle)  {
				if (filteredTorques[k] > maxlthrust)  {
					maxlthrust = filteredTorques[k];
				}
				if (filteredTorques[k] < minlthrust)  {
					minlthrust = filteredTorques[k];
				}
			}
			else  {
				if (filteredTorques[k] > maxrthrust)  {
					maxrthrust = filteredTorques[k];
				}
				if (filteredTorques[k] < minrthrust)  {
					minrthrust = filteredTorques[k];
				}
			}
							
			if (filteredTorques[k] > thismax)  {
				thismax = filteredTorques[k];
			}

			averageForce += filteredTorques[k];
		}			// for each segment

		//------------------------------
		// check for rescaling here:
		// we get here every 100 ms
		//------------------------------

		if (thismax > MAX_SS_FORCE)  {
			belowcount = 0;
			abovecount++;
			
			rescale = false;
//xxx
			if (abovecount >= 20)  {
				MAX_SS_FORCE += 10.0;
				rescale = true;
				abovecount = 0;
			}
		}
		else  {
			abovecount = 0;
			
			if (MAX_SS_FORCE > 10.0)  {
				rescale = false;
				if (thismax < (MAX_SS_FORCE-10.0) )  {
					belowcount++;
					if (belowcount > 20)  {
						MAX_SS_FORCE -= 10.0;
						rescale = true;
						belowcount = 0;
					}
				}
				else  {
					belowcount = 0;
				}
			}
		}
		

	}						// primed
	else  {
		for(k=0;k<NBARS;k++)  {
			thrust[k].fval = filteredTorques[k] = (float)tpf[k]->calc(thisRevForces[k]);			// tlm2008-08-26, added thrust[k].fval
			averageForce += filteredTorques[k];
		}
	}		// not primed

	averageForce /= NBARS;
// ok here:
#ifdef _DEBUG
//log->write("%.4f %.4f %.4f\n", averageForce, filteredTorques[18], filteredTorques[19]);
#endif

	//----------------------------------------------------------------
	// compute the spinscan numbers and the left/right power split
	//----------------------------------------------------------------

	leftss = rightss = 0.0f;
	float lsum=0.0f, rsum=0.0f;
	float maxleft, maxright;

	maxleft = maxright = -FLT_MAX;		//-Float.MAX_VALUE;

	leftwatts = rightwatts = 0.0f;


	for(int i=0; i<NBARS; i++)	{
		if(i<NBARS/2)  {
			lsum += filteredTorques[i];
			if (filteredTorques[i]>maxleft) maxleft = filteredTorques[i];
		}
		else	{
			rsum += filteredTorques[i];
			if (filteredTorques[i]>maxright) maxright = filteredTorques[i];
		}
	}

	float ftemp = lsum + rsum;
	if (ftemp>0.1f)  {
		leftwatts = 100.0f*lsum / ftemp;
		rightwatts = 100.0f - leftwatts;
	}

	if (maxleft>0.1f)  {
		leftss = (float) ((100.0/(NBARS/2)) * lsum / maxleft);
	}
	if (maxright>0.1f)  {
		rightss = (float) ((100.0/(NBARS/2)) * rsum / maxright);
	}

	// tlm20081124:
	if (leftss < 50.0f)  {
		leftss = 50.0f;
	}
	if (rightss < 50.0f)  {
		rightss = 50.0f;
	}

	totalss = (leftss + rightss)/2.0f;
	return;
}

/**********************************************************************

**********************************************************************/

void SS::reset(void)  {
	int i;

	if (reset_flag==false)  {			// this flag is controlled at the application level. It allows the app to
												// cause the normal reset to be not done. This is so that the average spinscan
												// in the reports is not 0.
		return;
	}

	averageCounter = 0L;
	lastavgtime = 0;

	accum_ss = 0.0;
	accum_lss = 0.0;
	accum_rss = 0.0;

	average_lss = 0.0f;
	average_rss = 0.0f;
	average_ss = 0.0f;

	minlthrust = FLT_MAX;
	minrthrust = FLT_MAX;
	maxlthrust = -FLT_MAX;
	maxrthrust = -FLT_MAX;

	totalss = 0.0;
	leftss = 0;
	rightss = 0;
	leftwatts = 0;
	rightwatts = 0;

	bp = 0;
	thisforce = 0.0f;

	MIN_SS_FORCE = 0.0;				// 0 watts
	MAX_SS_FORCE = 10.0;				// 10 lbs
	rescale = FALSE;
//	lastFilterTime = 0;

	leftwatts = 0.0;
	rightwatts = 0.0;
	leftss = 0.0;
	rightss = 0.0;
	totalss = 0.0;

	peddlingTimeout = true;
	primed = false;
	inptr = 0;
	loadcount = 0;
	averageForce = 0;
	belowcount = abovecount = 0;

	ssrec = 0;
	leftss = 0.0f;
	rightss = 0.0f;
	totalss = 0.0f;
	firstValidTDC = FALSE;

	for(i=0;i<NBARS;i++)  {
		filteredTorques[i] = 0.0f;
		thisRevForces[i] = 0;
		thrust[i].fval = thrust[i].raw = thrust[i].a = 0.0f;
		tpf[i]->reset();
	}

//	for(i=0;i<NBARS;i++)  {
//		tpf[i] = new twoPoleFilter(100);		// argument is unused, so it doesn't matter
//	}

	for(i=0;i<NBARS*2;i++)  {
		lastTwoRevForces[i] = 0;
	}

	rpmMav->reset();
	wattsFilter->reset();

	lata = 90;
	rata = 90;
	ilata = 90;
	irata = 90;

	return;
}


/**********************************************************************

**********************************************************************/

void SS::snapshot(char *_fname)  {
	FILE *stream;
	int i;

	stream = fopen(_fname, "a+t");

	for(i=0; i<NBARS; i++)  {
		if ( i < (NBARS-1) )  {
			fprintf(stream, "%8.4f ", filteredTorques[i]);
		}
		else  {
			fprintf(stream, "%8.4f\n", filteredTorques[i]);
		}
	}


	fclose(stream);

	return;
}

/**********************************************************************

**********************************************************************/

void SS::start(void)  {

	return;
}

/**********************************************************************

**********************************************************************/

void SS::set_degperseg(double _dps)  {
	degperseg = _dps;
	return;
}

/**********************************************************************
	MAKE SURE THAT THIS IS CALLED EVERY 100 MS!!!
**********************************************************************/

void SS::do_ten_hz_filter(METADATA *_meta, bool _started, bool _finished, bool _paused)  {
	float ftemp, lsum=0.0f, rsum=0.0f;
	float maxleft = -FLT_MAX, maxright=-FLT_MAX;
	int i;

	/*
	DWORD ms;
	ms = timeGetTime();
	if ( (ms-lastFilterTime) < 100 )   {
		return;
	}
	lastFilterTime = ms;
	*/

#ifdef _DEBUG
	//at->update();							// 100.00 ms. GOOD! 2011-09-30
#endif


	//-----------------------
	// filter the spinscan
	//-----------------------

	this_rev_max = -FLT_MAX;

	for (i = 0; i < NBARS; i++) {

		if (tpf[i]) {
			thrust[i].fval = (float)(tpf[i]->calc(thrust[i].raw));
		}

		if (thrust[i].fval<0.0f)  {
			thrust[i].fval = 0.0f;
		}
		this_rev_max = MAX(this_rev_max, thrust[i].fval);

		_meta->bars[i] = thrust[i].fval;

		bars2_count[i]++;
		total_bars2[i] += _meta->bars[i];
		bars2[i] = (float) (total_bars2[i] / bars2_count[i]);

	}

	leftwatts = rightwatts = leftss = rightss = 0.0f;

	for(i=0; i<NBARS; i++)  {
		//ftemp = thrust[i].fval + thrust[(i+2)%NBARS].fval;			//wrap 49 to 1

		ftemp = thrust[i].fval;
		if(i<NBARS/2)  {
			lsum += ftemp;
			maxleft = MAX(maxleft, ftemp);
		}
		else	{
			rsum += ftemp;
			maxright = MAX(maxright, ftemp);
		}

		#ifdef _DEBUG
		//logg->write("%.2f ", thrust[i].fval);
		#endif
	}

	#ifdef _DEBUG
	//logg->write("\n");
	#endif


	ftemp = lsum + rsum;
	if (ftemp>0.1f)  {
		leftwatts = 100.0f *lsum / ftemp;
		rightwatts = 100.0f - leftwatts;
	}

	if (maxleft>0.000001f)  {
		leftss = (float) ((100.0/12.0) * lsum / maxleft);
	}

	if (maxright>0.000001f)  {
		rightss = (float) ((100.0/12.0) * rsum / maxright);
	}

	totalss = (leftss + rightss)/2.0f;

	maxlthrust = maxleft;
	maxrthrust = maxright;

	//----------------------------
	// compute the left "ata"
	//----------------------------

	float ftemp1, ftemp2;
	float langle, rangle;

	ftemp1 = ftemp2 = 0.0f;

	//for (i=1;i<=23;i+=2)  {
	for(i=0; i<NBARS/2; i++)  {
		// tlm20130825+++
		//ftemp1 += (float) (thrust[i].fval*thrust[i].beginning_radians);
		ftemp1 += (float) (thrust[i].fval*thrust[i].center_radians);
		// tlm20130825---
		ftemp2 += (float) (thrust[i].fval);
	}
	if (ftemp2!=0.0f)  {
		langle = ftemp1/ftemp2;
	}
	else  {
		langle = (float) (PI / 2.0);
	}
	lata = (float)((360.0/(2.0*PI))*langle);
	ilata = (int) (.5 + (360.0/(2.0*PI))*langle);

	//----------------------------
	// compute the right "ata"
	//----------------------------

	ftemp1 = ftemp2 = 0.0f;

	for(i=NBARS/2; i<NBARS; i++)  {
		// tlm20130825+++
		//ftemp1 += (float) (thrust[i].fval * thrust[i].beginning_radians);
		ftemp1 += (float) (thrust[i].fval * thrust[i].center_radians);
		// tlm20130825---
		ftemp2 += (float) (thrust[i].fval);
	}

	if (ftemp2!=0.0f)  {
		rangle = ftemp1/ftemp2;			// don't subtract 180 here or the bars ata lines will be screwed up!!
	}
	else  {
		rangle = (float) (3.0*PI / 2.0);					// 270 degrees
	}

	rata = (float)((360.0/(2.0*PI))*rangle);
	irata = (int) (.5 + (360.0/(2.0*PI))*rangle);
	rata -= 180.0;
	irata -= 180;

	_meta->ss = totalss;
	_meta->lss = leftss;
	_meta->rss = rightss;
	_meta->lata = (int) (.5 + lata);
	_meta->rata = (int) (.5 + rata);
	_meta->lwatts = leftwatts;
	_meta->rwatts = rightwatts;

	//------------------------------
	// check for rescaling here:
	// we get here every 100 ms
	//------------------------------

	if (this_rev_max > MAX_SS_FORCE)  {
		belowcount = 0;
		abovecount++;
		
		rescale = FALSE;
		_meta->rescale = false;

		//polar->setupbg(ds->decoder->MIN_SS_FORCE, ds->MAX_SS_FORCE);

		if (abovecount >= 20)  {
			MAX_SS_FORCE += 10.0;
			rescale = true;
			abovecount = 0;
			_meta->rescale = true;									// for prevelotrondata file playback rescaling
			_meta->maxforce = (float)(MAX_SS_FORCE);						// so prevelotrondata file playback knows what new scale to use
		}
	}
	else  {
		abovecount = 0;
		
		if (MAX_SS_FORCE > 10.0)  {
			rescale = FALSE;
			_meta->rescale = false;

			if (this_rev_max < (MAX_SS_FORCE-10.0) )  {
				belowcount++;
				if (belowcount > 20)  {
					MAX_SS_FORCE -= 10.0;
					rescale = TRUE;
					belowcount = 0;
					_meta->rescale = true;							// for prevelotrondata file playback rescaling
					_meta->maxforce = (float)(MAX_SS_FORCE);						// so prevelotrondata file playback knows what new scale to use
				}
			}
			else  {
				belowcount = 0;
			}
		}
	}

	//--------------------------------------
	// compute averages if running:
	//--------------------------------------

	if (!_started)  {
		return;
	}
	if (_finished)  {
		return;
	}
	if (_paused)  {
		return;
	}

	averageCounter++;

	accum_lss += leftss;
	accum_rss += rightss;
	accum_ss += totalss;

	average_lss = (float) (accum_lss / averageCounter);
	average_rss = (float) (accum_rss / averageCounter);
	average_ss = (float) (accum_ss / averageCounter);

	_meta->average_lss = average_lss;
	_meta->average_rss = average_rss;
	_meta->average_ss = average_ss;

	return;
}					// void SS::do_ten_hz_filter(METADATA *_meta, bool _started, bool _finished, bool _paused)


/**********************************************************************

**********************************************************************/

void SS::reset_averages(void)  {

	averageCounter = 0L;

	accum_ss = 0.0;
	accum_lss = 0.0;
	accum_rss = 0.0;

	return;
}


#if 0
/**********************************************************************
MAKE SURE THAT THIS IS CALLED EVERY 100 MS!!!
**********************************************************************/

void SS::do_ten_hz_filter(float *_fbars, bool _started, bool _finished, bool _paused) {
	float ftemp, lsum = 0.0f, rsum = 0.0f;
	float maxleft = -FLT_MAX, maxright = -FLT_MAX;
	int i;

	/*
	DWORD ms;
	ms = timeGetTime();
	if ( (ms-lastFilterTime) < 100 )   {
	return;
	}
	lastFilterTime = ms;
	*/

#ifdef _DEBUG
	//at->update();							// 100.00 ms. GOOD! 2011-09-30
#endif


	//-----------------------
	// filter the spinscan
	//-----------------------

	this_rev_max = -FLT_MAX;

	for (i = 0; i<NBARS; i++) {
		thrust[i].fval = (float)(tpf[i]->calc(_fbars[i]));

		if (thrust[i].fval<0.0f) {
			thrust[i].fval = 0.0f;
		}
		this_rev_max = MAX(this_rev_max, thrust[i].fval);

		fbars[i] = thrust[i].fval;

		bars2_count[i]++;
		total_bars2[i] += fbars[i];
		bars2[i] = (float)(total_bars2[i] / bars2_count[i]);

	}

	leftwatts = rightwatts = leftss = rightss = 0.0f;

	for (i = 0; i<NBARS; i++) {
		//ftemp = thrust[i].fval + thrust[(i+2)%NBARS].fval;			//wrap 49 to 1

		ftemp = thrust[i].fval;
		if (i<NBARS / 2) {
			lsum += ftemp;
			maxleft = MAX(maxleft, ftemp);
		}
		else {
			rsum += ftemp;
			maxright = MAX(maxright, ftemp);
		}

#ifdef _DEBUG
		//logg->write("%.2f ", thrust[i].fval);
#endif
	}

#ifdef _DEBUG
	//logg->write("\n");
#endif


	ftemp = lsum + rsum;
	if (ftemp>0.1f) {
		leftwatts = 100.0f *lsum / ftemp;
		rightwatts = 100.0f - leftwatts;
	}

	if (maxleft>0.000001f) {
		leftss = (float)((100.0 / 12.0) * lsum / maxleft);
	}

	if (maxright>0.000001f) {
		rightss = (float)((100.0 / 12.0) * rsum / maxright);
	}

	totalss = (leftss + rightss) / 2.0f;

	maxlthrust = maxleft;
	maxrthrust = maxright;

	//----------------------------
	// compute the left "ata"
	//----------------------------

	float ftemp1, ftemp2;
	float langle, rangle;

	ftemp1 = ftemp2 = 0.0f;

	//for (i=1;i<=23;i+=2)  {
	for (i = 0; i<NBARS / 2; i++) {
		// tlm20130825+++
		//ftemp1 += (float) (thrust[i].fval*thrust[i].beginning_radians);
		ftemp1 += (float)(thrust[i].fval*thrust[i].center_radians);
		// tlm20130825---
		ftemp2 += (float)(thrust[i].fval);
	}
	if (ftemp2 != 0.0f) {
		langle = ftemp1 / ftemp2;
	}
	else {
		langle = (float)(PI / 2.0);
	}
	lata = (float)((360.0 / (2.0*PI))*langle);
	ilata = (int)(.5 + (360.0 / (2.0*PI))*langle);

	//----------------------------
	// compute the right "ata"
	//----------------------------

	ftemp1 = ftemp2 = 0.0f;

	for (i = NBARS / 2; i<NBARS; i++) {
		// tlm20130825+++
		//ftemp1 += (float) (thrust[i].fval * thrust[i].beginning_radians);
		ftemp1 += (float)(thrust[i].fval * thrust[i].center_radians);
		// tlm20130825---
		ftemp2 += (float)(thrust[i].fval);
	}

	if (ftemp2 != 0.0f) {
		rangle = ftemp1 / ftemp2;			// don't subtract 180 here or the bars ata lines will be screwed up!!
	}
	else {
		rangle = (float)(3.0*PI / 2.0);					// 270 degrees
	}

	rata = (float)((360.0 / (2.0*PI))*rangle);
	irata = (int)(.5 + (360.0 / (2.0*PI))*rangle);
	rata -= 180.0;
	irata -= 180;

	_meta->ss = totalss;
	_meta->lss = leftss;
	_meta->rss = rightss;
	_meta->lata = (int)(.5 + lata);
	_meta->rata = (int)(.5 + rata);
	_meta->lwatts = leftwatts;
	_meta->rwatts = rightwatts;

	//------------------------------
	// check for rescaling here:
	// we get here every 100 ms
	//------------------------------

	if (this_rev_max > MAX_SS_FORCE) {
		belowcount = 0;
		abovecount++;

		rescale = FALSE;
		_meta->rescale = false;

		//polar->setupbg(ds->decoder->MIN_SS_FORCE, ds->MAX_SS_FORCE);

		if (abovecount >= 20) {
			MAX_SS_FORCE += 10.0;
			rescale = true;
			abovecount = 0;
			_meta->rescale = true;									// for prevelotrondata file playback rescaling
			_meta->maxforce = (float)(MAX_SS_FORCE);						// so prevelotrondata file playback knows what new scale to use
		}
	}
	else {
		abovecount = 0;

		if (MAX_SS_FORCE > 10.0) {
			rescale = FALSE;
			_meta->rescale = false;

			if (this_rev_max < (MAX_SS_FORCE - 10.0)) {
				belowcount++;
				if (belowcount > 20) {
					MAX_SS_FORCE -= 10.0;
					rescale = TRUE;
					belowcount = 0;
					_meta->rescale = true;							// for prevelotrondata file playback rescaling
					_meta->maxforce = (float)(MAX_SS_FORCE);						// so prevelotrondata file playback knows what new scale to use
				}
			}
			else {
				belowcount = 0;
			}
		}
	}

	//--------------------------------------
	// compute averages if running:
	//--------------------------------------

	if (!_started) {
		return;
	}
	if (_finished) {
		return;
	}
	if (_paused) {
		return;
	}

	averageCounter++;

	accum_lss += leftss;
	accum_rss += rightss;
	accum_ss += totalss;

	average_lss = (float)(accum_lss / averageCounter);
	average_rss = (float)(accum_rss / averageCounter);
	average_ss = (float)(accum_ss / averageCounter);

	_meta->average_lss = average_lss;
	_meta->average_rss = average_rss;
	_meta->average_ss = average_ss;

	return;
}					// void SS::do_ten_hz_filter(METADATA *_meta, bool _started, bool _finished, bool _paused)
#endif			// #if 0


/**********************************************************************

**********************************************************************/

void SS::setraw(float * _bars)  {
	for (int i = 0; i < NBARS; i++) {
		thrust[i].raw = _bars[i];
	}
	return;
}
