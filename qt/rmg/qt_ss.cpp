
#include <math.h>
#include <float.h>

#include <QtGlobal>

#include <comdefs.h>
#include <comutils.h>

#include "qt_ss.h"

namespace RACERMATE  {

int qt_SS::BARINFO::nbars = 0;
double qt_SS::BARINFO::degperseg = 0.0;

/**********************************************************************

**********************************************************************/

//SS::SS(int _nbars, bool _metric)  {
//qt_SS::qt_SS(int _nbars)  {
qt_SS::qt_SS(void)  {
	int i;

	nbars = 24;
	//nbars = _nbars;

	//metric = _metric;

	BARINFO::setNbars(nbars);				// also sets degperseg
	bi = new BARINFO *[nbars];

	for(i=0; i<nbars; i++)  {
		bi[i] = new BARINFO(i);
	}


	ssd = new SSD(nbars);

	started = false;
	paused = false;
	finished = false;

#ifdef DOBARS2
	bars2 = new float[nbars];
	total_bars2 = new double[nbars];
	bars2_count = new unsigned long[nbars];
#endif


	reset_flag = true;

	this_rev_max = -FLT_MAX;

#ifdef _DEBUG
	at = new Tmr("SS");
#endif

	SSShift = 18;



	tpf = new twoPoleFilter *[nbars];
	for(i=0;i<nbars;i++)  {
		tpf[i] = new twoPoleFilter(100);		// argument is unused, so it doesn't matter
	}

#ifdef DOSMAV
	//xxx
	smav = new sMav *[nbars];
	for(i=0;i<nbars;i++)  {
		smav[i] = new sMav(90);
	}
#endif

	reset();
}								// constructor



/**********************************************************************
	destructor
**********************************************************************/

qt_SS::~qt_SS()  {
	int i;

#ifdef _DEBUG
	DEL(at);
	//at = NULL;
#endif

#ifdef DOBARS2
	XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	DELARR(bars2);
	//bars2 = 0;
	xxx
	delete [] total_bars2;
	total_bars2 = 0;
	delete [] bars2_count;
	bars2_count = 0
#endif

	if (tpf)  {
		for(i=0;i<nbars;i++)  {
			DEL(tpf[i]);
			//tpf[i] = 0;
		}
		DELARR(tpf);
		//tpf = 0;
	}

#ifdef DOSMAV
	if (smav)  {
		for(i=0;i<nbars;i++)  {
			DEL(smav[i]);
			//smav[i] = 0;
		}
		DELARR(smav);
		//smav = 0;
	}
#endif

	if (bi)  {
		for(i=0; i<nbars; i++)  {
			DEL(bi[i]);
			//bi[i] = 0;
		}
		DELARR(bi);
		//bi = 0;
	}

	DEL(ssd);

	return;
}							// destructor



/**********************************************************************

**********************************************************************/

void qt_SS::reset(void)  {
	int i;

	if (reset_flag==false)  {			// this flag is controlled at the application level. It allows the app to
												// cause the normal reset to be not done. This is so that the average spinscan
												// in the reports is not 0.
		return;
	}

	ssd->reset();



	averageCounter = 0L;

	accum_ss = 0.0;
	accum_lss = 0.0;
	accum_rss = 0.0;

	average_lss = 0.0f;
	average_rss = 0.0f;
	average_ss = 0.0f;

	//totalss = 0.0;

#ifdef _DEBUG
	bp = 0;
#endif

	minforce = 0.0;				// 0 watts
	maxforce = 10.0;				// lbs

	rescale = false;

	belowcount = abovecount = 0;

	for(i=0;i<nbars;i++)  {
		bi[i]->reset();
		tpf[i]->reset();
#ifdef DOSMAV
		smav[i]->reset();
#endif

	}

	return;
}											// SS::reset()




/**********************************************************************

**********************************************************************/

//float SS::getTotalss() const  {
//	return totalss;
//}


/**********************************************************************

**********************************************************************/

void qt_SS::start(void)  {
	return;
}

/**********************************************************************

**********************************************************************/

void qt_SS::BARINFO::setNbars(int _n)  {
	nbars = _n;
	degperseg = 360.0 / (double)nbars;				// 15 degrees
	return;
}

/**********************************************************************
	MAKE SURE THAT THIS IS CALLED EVERY 100 MS!!!
**********************************************************************/

void qt_SS::do_ten_hz_filter(void)  {
	float ftemp, lsum=0.0f, rsum=0.0f;
	float maxleft = -FLT_MAX, maxright=-FLT_MAX;
	int i;


#ifdef _DEBUG
	//at->update();							// 100.00 ms. GOOD! 2011-09-30
#endif


	//-----------------------
	// filter the spinscan
	//-----------------------

	this_rev_max = -FLT_MAX;

	for(i=0; i<nbars; i++)  {
		ssd->val[i] = (float)(tpf[i]->calc(bi[i]->raw));
		if (ssd->val[i]<0.0f)  {
			//tlm
			ssd->val[i] = 0.0f;
		}
		//data->val[i] = bi[i]->fval;
		this_rev_max = qMax(this_rev_max, ssd->val[i]);
#ifdef DOBARS2
		bars2_count[i]++;
		total_bars2[i] += ssd->val[i];
		bars2[i] = (float) (total_bars2[i] / bars2_count[i]);
		ssd->avgval[i] = bars2[i];
#endif

#ifdef DOSMAV
		ssd->avgval[i] = smav[i]->compute(ssd->val[i]);
#endif

	}

	ssd->lwatts = ssd->rwatts = ssd->lss = ssd->rss = 0.0f;

	for(i=0; i<nbars; i++)  {
		ftemp = ssd->val[i];
		if(i<nbars/2)  {
			lsum += ftemp;
			maxleft = qMax(maxleft, ftemp);
		}
		else	{
			rsum += ftemp;
			maxright = qMax(maxright, ftemp);
		}
	}


	ftemp = lsum + rsum;
	if (ftemp>0.1f)  {
		ssd->lwatts = 100.0f *lsum / ftemp;
		ssd->rwatts = 100.0f - ssd->lwatts;
	}

	if (maxleft>0.000001f)  {
		ssd->lss = (float) ((100.0/12.0) * lsum / maxleft);
	}

	if (maxright>0.000001f)  {
		ssd->rss = (float) ((100.0/12.0) * rsum / maxright);
	}

	//totalss = (ssd->lss + ssd->rss)/2.0f;
	ssd->ss = (ssd->lss + ssd->rss)/2.0f;

#ifdef DOSMAV
	lsum = 0.0f;
	rsum=0.0f;
	maxleft = -FLT_MAX;
	maxright=-FLT_MAX;
	ssd->avglss = ssd->avgrss = 0.0f;

	for(i=0; i<nbars; i++)  {
		ftemp = ssd->avgval[i];
		if(i<nbars/2)  {
			lsum += ftemp;
			maxleft = qMax(maxleft, ftemp);
		}
		else	{
			rsum += ftemp;
			maxright = qMax(maxright, ftemp);
		}
	}

	if (maxleft>0.000001f)  {
		ssd->avglss = (float) ((100.0/12.0) * lsum / maxleft);
	}

	if (maxright>0.000001f)  {
		ssd->avgrss = (float) ((100.0/12.0) * rsum / maxright);
	}

	ssd->avgss = (ssd->avglss + ssd->avgrss)/2.0f;
#endif

	//----------------------------
	// compute the left "ata"
	//----------------------------

	float ftemp1, ftemp2;
	float langle, rangle;
	float rftemp1, rftemp2;


	ftemp1 = ftemp2 = 0.0f;

	for(i=0; i<nbars/2; i++)  {
		ftemp1 += (float) (ssd->val[i]*bi[i]->center_radians);
		ftemp2 += (float) (ssd->val[i]);
	}
	if (ftemp2!=0.0f)  {
		langle = ftemp1/ftemp2;
	}
	else  {
		langle = (float) (PI / 2.0);
	}
	ssd->lata = (float)(360.0/(2.0*PI))*langle;

	//----------------------------
	// compute the right "ata"
	//----------------------------

	rftemp1 = rftemp2 = 0.0f;

	for(i=nbars/2; i<nbars; i++)  {
		rftemp1 += (float) (ssd->val[i] * bi[i]->center_radians);
		rftemp2 += (float) (ssd->val[i]);
	}

	if (ftemp2!=0.0f)  {
		rangle = rftemp1/rftemp2;			// don't subtract 180 here or the bars ata lines will be screwed up!!
	}
	else  {
		rangle = (float) (3.0*PI / 2.0);					// 270 degrees
	}

	ssd->rata = (float)(360.0f/(2.0*PI))*rangle;
	ssd->rata -= 180.0f;

#ifdef _DEBUG
	bp = 2;
#endif

	//------------------------------
	// check for rescaling here:
	// we get here every 100 ms
	//------------------------------

		if (this_rev_max > maxforce)  {
			belowcount = 0;
			abovecount++;

			rescale = false;

			if (abovecount >= 20)  {
				maxforce += 10.0;
				rescale = true;
				abovecount = 0;
			}
		}
		else  {
			abovecount = 0;

			if (maxforce > 10.0)  {
				rescale = false;

				if (this_rev_max < (maxforce-10.0) )  {
					belowcount++;
					if (belowcount > 20)  {
						maxforce -= 10.0;
						rescale = true;
						belowcount = 0;
						maxforce = (float)(maxforce);						// so prevelotrondata file playback knows what new scale to use
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

	if (!started)  {
		return;
	}
	if (finished)  {
		return;
	}
	if (paused)  {
		return;
	}

	averageCounter++;

	accum_lss += ssd->lss;
	accum_rss += ssd->rss;
	accum_ss += ssd->ss;

	average_lss = (float) (accum_lss / averageCounter);
	average_rss = (float) (accum_rss / averageCounter);
	average_ss = (float) (accum_ss / averageCounter);

	return;
}

/****************************************************/

bool qt_SS::getStarted() const {
	return started;
}

/****************************************************/

void qt_SS::setStarted(bool value) {
	started = value;
}

/****************************************************/

bool qt_SS::getFinished() const  {
	return finished;
}

/****************************************************/

void qt_SS::setFinished(bool value) {
	finished = value;
}

/****************************************************/

bool qt_SS::getPaused() const  {
	return paused;
}

/****************************************************/

void qt_SS::setPaused(bool value)  {
	paused = value;
}


/**********************************************************************

**********************************************************************/

void qt_SS::reset_averages(void)  {

	averageCounter = 0L;

	accum_ss = 0.0;
	accum_lss = 0.0;
	accum_rss = 0.0;

	return;
}


/**********************************************************************

**********************************************************************/

void qt_SS::setraw(int _j, float _val)  {
	if (_j<0 || _j>nbars-1)  {
		return;
	}
	bi[_j]->raw = _val;
	return;
}

/**********************************************************************

**********************************************************************/

qt_SS::SSD *qt_SS::getdata(void)  {
	// tlm
	return ssd;
}

/**********************************************************************

**********************************************************************/

bool qt_SS::getRescale() const {
	return rescale;
}

/**********************************************************************
	default constructor
**********************************************************************/
/*
qt_SS::SSD::SSD(void)  {

#ifdef _DEBUG
	qDebug() << "qt_SS::SSD() default constructor";
#endif

	nbars = 24;
	val = new float[nbars];
	avgval = new float[nbars];

	reset();

	return;
}
*/

/**********************************************************************

**********************************************************************/

qt_SS::SSD::SSD(int _nbars)  {

#ifdef _DEBUG
	//qDebug() << "qt_SS::SSD() non-default constructor";
#endif

	nbars = _nbars;
	val = new float[nbars];
	avgval = new float[nbars];

	reset();
}


/**********************************************************************
	destructor
**********************************************************************/

qt_SS::SSD::~SSD()  {
	if (val)  {
		delete[] val;
		val = 0;
	}

	if (avgval)  {
		delete[] avgval;
		avgval = 0;
	}
}

/**********************************************************************

**********************************************************************/

void qt_SS::SSD::reset(void)  {
	int i;

	ss = 0.0f;
	lss = 0.0f;
	rss = 0.0f;
	lata = 90.0f;
	rata = 90.0f;
	avgss = 0.0f;
	avglss = 0.0f;
	avgrss = 0.0f;

	for(i=0; i<nbars; i++)  {
		val[i] = 0.0f;
		avgval[i] = 0.0f;
	}

	return;
}
int qt_SS::SSD::getNbars() const
{
	return nbars;
}

/**********************************************************************

**********************************************************************/

void qt_SS::SSD::setNbars(int value)  {
	int i;

	nbars = value;
	ss = 0.0f;								// instantaneous ss
	lss = 0.0f;
	rss = 0.0f;
	lwatts = 0.0f;
	rwatts = 0.0f;
	lata = 0.0f;
	rata = 0.0f;
	avgss = 0.0f;
	avglss = 0.0f;
	avgrss = 0.0f;

	if (val)  {
		delete[] val;
		val = 0;
	}

	if (avgval)  {
		delete[] avgval;
		avgval = 0;
	}

	val = new float[nbars];
	avgval = new float[nbars];

	for(i=0; i<nbars; i++)  {
		val[i] = 0.0f;
		avgval[i] = 0.0f;
	}

	return;
}


/**********************************************************************
	default constructor
	one for each bar
**********************************************************************/

qt_SS::BARINFO::BARINFO(int _ix)  {
	ix = _ix;
	init();

}
/**********************************************************************

**********************************************************************/

void qt_SS::BARINFO::reset(void)  {
	raw = 0.0f;
	return;
}

/**********************************************************************

**********************************************************************/

void qt_SS::BARINFO::init(void)  {

	beginning_degrees = degperseg*ix;
	center_degrees = degperseg*ix + .5*degperseg;

	beginning_radians = DEGTORAD*beginning_degrees;
	center_radians = DEGTORAD*center_degrees;

	beginning_sin = (float)sin(beginning_radians);
	beginning_cos = (float)cos(beginning_radians);
	beginning_sin_rotated = (float)sin(beginning_radians+(PI/2.0));
	beginning_cos_rotated = (float)cos(beginning_radians+(PI/2.0));

	center_sin = (float)sin(center_radians);
	center_cos = (float)cos(center_radians);
	center_sin_rotated = (float)sin(center_radians+(PI/2.0));
	center_cos_rotated = (float)cos(center_radians+(PI/2.0));

	reset();

	return;
}								// init()

/**********************************************************************

**********************************************************************/

qt_SS::BARINFO *qt_SS::get_barinfo(int i)  {

	return bi[i];
}


}

