#ifdef WIN32
#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#include <math.h>
#include <assert.h>

#ifdef WIN32
#else
	#include <memory.h>
#endif

//#include <minmax.h>
#include <qtformula.h>

//int qtFormula::count = 0;

/**********************************************************************

**********************************************************************/

qtFormula::qtFormula(float _ftp)  {
	ftp = _ftp;

#ifdef _DEBUG
	if (ftp<0.01f)  {
		bp = 1;
	}
#endif


//	id = count;
//	count++;

	t = new RACERMATE::Tmr();

	reset();

}

/**********************************************************************

**********************************************************************/

qtFormula::~qtFormula()  {
	#ifdef _DEBUG
	//qDebug("************************ maxdq (%d) = %d *********************\n", id, maxdq);		// 784
	#endif

	dq.clear();
	DEL(t);

//	count--;
}

/**********************************************************************

**********************************************************************/

void qtFormula::reset(void) {

	dq.clear();

	total_watts = 0.0;
	avg_watts = 0.0f;
	np = 0.0f;
	IF = 0.0f;
	tss = 0.0f;

	memset(str, 0, sizeof(str));

#ifdef _DEBUG
	maxdq = -1;
#endif

	fourth_avg = 0.0f;
	fourth_total = 0.0;
	fourth_count = 0L;
	startms = QDateTime::currentMSecsSinceEpoch();

	bp = 1;
}

/**********************************************************************
	no need to call more that once per second

qtFormulas are: 
 
Calculate Normalized Power (NP) by 
	(1) starting at 30 seconds into the wattage data and calculating a 30-second rolling average for power;
	(2) raising the values obtained in step 1 to the fourth power;
	(3) taking the average of all the values obtained in step 2; and
	(4) taking the fourth root of the number obtained in step 3. This is Normalized Power.
 
Intensity Factor (IF) is simply the ratio of your NP to your functional
	threshold power - that is, the fraction of your functional threshold power
	that you maintained for that workout or part of a workout (IF = NP/FTP).

	Ratios can always be expressed as fractions or decimals: For example,
	the ratio 1 to 5, or 1:5, could be written 1/5, that is, 1 divided by 5,
	or 0.2. NP to FTP (NP:FTP) means the same thing as NP/FTP, or NP divided
	by FTP. 
	So, for example, if the NP for a long training ride that you
	performed early in the year is 210 watts, and your threshold power at
	the time was 280 watts, then the IF for that workout would be 210
	divided by 280, or 0.75. 
 
The formula for TSS is:

	TSS = [(s x W x IF)/(FTP x 3,600)]x 100

	where "s" is for duration in seconds, 
	W is Normalized Power in watts, 
	IF is Intensity Factor, 
	FTP is functional threshold power, and 3,600 is for the number of seconds in 1 hour.

**********************************************************************/

void qtFormula::calc(float watts) {

	now = QDateTime::currentMSecsSinceEpoch();

	if ((now-lastms) < 1000)  {
		return;
	}

	lastms = now;

	FORMULA2_PAIR p;

	#ifdef _DEBUG
	//t->update();								// 15.6 ms
	//watts = 100.0f;							// for testing
	#endif

	p.ms = now;
	p.watts = watts;
	dq.push_back(p);

	total_watts += watts;

#ifdef _DEBUG
	if (now < 30000)  {
#else
	if (now < 30000)  {
#endif
		return;
	}

	// (1) starting at 30 seconds into the wattage data and calculating a 30-second rolling average for power;

#ifdef _DEBUG
	//size = (int)dq.size();					// 992
#endif

	int cnt = 0;

	while(1)  {
		if ( (now - dq[0].ms) >= 30000)  {
			total_watts -= dq[0].watts;
			dq.pop_front();
			cnt++;
		}
		else  {
			break;
		}
	}

	//size = (int)dq.size();					// 991

	avg_watts = (float)(total_watts / dq.size());			// average for last 30 seconds

	// (2) raising the values obtained in step 1 to the fourth power;

	fourth_total += pow(avg_watts, 4);
	fourth_count++;

	// (3) taking the average of all the values obtained in step 2; and

	fourth_avg = (float)(fourth_total / fourth_count);

	// (4) taking the fourth root of the number obtained in step 3. This is Normalized Power.

	np = (float)pow((double)fourth_avg, .25);

	if (ftp > 0.0f)  {
		IF = np / ftp;
		double s = now / 1000;
		tss = (float)(( (s*np*IF)/(ftp*3600)) * 100.0f);
	}
	else  {
		IF = 0.0f;
		tss = 0.0f;
	}

	//total_watts -= dq[0].watts;
	//dq.pop_front();
	//(1) starting at 30 seconds into the wattage data and calculating a 30-second rolling average for power;
	//avg_watts = total_watts / dq.size();			// average for last 30 seconds

	#ifdef _DEBUG
	//int sz = dq.size();
	maxdq = qMax(maxdq, (int)dq.size());			// 1910*.0156 = 30 seconds
	#endif

	return;
}						// calc()


