
#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#else
	#include <stdlib.h>
#endif


/****************************************************************************************
** FILE:  scale.c
** PURPOSE:  functions for computing well-tempered linear scales
** FUNCTIONS:  MaxIntervalScale - max number of intervals algorithms
**             Logscale         - a flog10 scale
**             power            - integer exponent pow
** REVISION HISTORY:
** NOTES: from B. Gossage at NRC (gift from NRC STB team)
**        MaxIntervalScale is from "Composing Well Tempered Linear Scales",
**        by Tom Steppe in Computer Language, Sept 1989
****************************************************************************************/

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include <scaler.h>



#define SET_LEN (sizeof(WTNSet) / sizeof(double) -1)

/*****  file globals *****/
static double WTNSet[] = {1.0, 2.0, 5.0, 10.0};

// local function prototypes

static double FirstNiceNum(double IntervalSize, int *Index, double *PowerOfTen);
static double NextNiceNum(double *WTNSet, int SetSize, int *Index, double *PowerOfTen);
static void CalcExtLabel(double DataMin, double DataMax, double NiceNum, int *LoMult, int *HiMult);
//static void CalcIntLabel(double DataMin, double DataMax, double NiceNum, int *LoMult, int *HiMult);
static double power (double Root, int Exponent);
static double dround(double z);

/****************************************************************************************
 FUNCTION:  MaxIntervalScale
 PURPOSE:  scale data in a graph with a maximum number of intervals
 PARAMETERS:  DataMin - (I) Data minimum
              DataMax - (I) Data maximum
              DataMaxIntervals - (I) Max number of possible intervals
              ScaleMin - (O) Scale minimum
              ScaleMax - (O) Scale maximum
              Actual - (O) Actual number of intervals used
 RETURNS: none
 CALLS: FirstNiceNum (scale.c)
        CalcExtLabel (scale.c)
        NextNiceNum  (scale.c)
 SYNTAX:  void MaxIntervalScale(double DataMin,
                                double DataMax,
                                int MaxIntervals,
                                double *ScaleMin,
                                double *ScaleMax,
                                int *Actual);
 PROTOTYPE IN: scale.h
 NOTES:
****************************************************************************************/
		/*
		__declspec(dllexport) void MaxIntervalScale(
				double DataMin, 
				double DataMax, 
				int MaxIntervals, 
				double *ScaleMin, 
				double *ScaleMax, 
				int *Actual
			);
		*/

void MaxIntervalScale(
					double DataMin, 
					double DataMax, 
					int MaxIntervals, 
					double *ScaleMin, 
					double *ScaleMax, 
					int *Actual)  {
	double intervalSize;
   double powerOfTen;
   double niceNum;

   int index;
   int loMult;
   int hiMult;

   assert( DataMin <= DataMax);
	//double d = dround(DataMax);
	DataMin = dround(DataMin);
	DataMax = dround(DataMax);

   if (MaxIntervals <= 0)  {
		*Actual = 0;
		return;
   }

   if( MaxIntervals < 2)
		MaxIntervals = 2;

   if ( DataMax == DataMin)  {
		DataMin = 0.0;
		DataMax *= 2.0;
   }

   int bp;

#ifdef _DEBUG
	if (MaxIntervals != 20)  {
		bp = 2;
	}
#endif

   intervalSize = (DataMax - DataMin) / MaxIntervals;			// calculate smallest potential interval size

   if (intervalSize == 0.0)  {
	   bp = 1;
   }

   niceNum = FirstNiceNum( intervalSize, &index, &powerOfTen);			// calculate smallest nice number

   while ( niceNum < intervalSize)  {
		niceNum = NextNiceNum( WTNSet, SET_LEN, &index, &powerOfTen);
   }

   CalcExtLabel( DataMin, DataMax, niceNum, &loMult, &hiMult);				// Produce the scale using the specified nice number

   // continue to rescale with new nice numbers until the requested
   // number of intervals is not exceeded

   while ( hiMult - loMult > MaxIntervals)  {
     niceNum = NextNiceNum( WTNSet, SET_LEN, &index, &powerOfTen);
     CalcExtLabel( DataMin, DataMax, niceNum, &loMult, &hiMult);
   }

   // calculate scale limits

   *ScaleMin = (float)(loMult * niceNum);
   *ScaleMax = (float)(hiMult * niceNum);
   *Actual = hiMult - loMult;

	return;
}

/****************************************************************************************
 FUNCTION:  FirstNiceNum
 PURPOSE:  calculate an initial value for the nice number
 PARAMETERS:  IntervalSize - (I) Interval size
              Index        - (O) Index of WTNSet
              PowerOfTen   - (O) for a nice number
 RETURNS:  the first nice number
 CALLS: power (scale.c)
        floor (math.h)
        log10 (math.h)
 SYNTAX: static double FirstNiceNum(double IntervalSize,
                                    int    *Index,
                                    double *PowerOfTen);
 PROTOTYPE IN:  (local)
 NOTES:
****************************************************************************************/

static double FirstNiceNum(double IntervalSize, int *Index, double *PowerOfTen)  {
   int exponent;

   exponent = (int) floor( log10( (double)IntervalSize));			// calculate an initial power of 10
   *PowerOfTen = power( 10.0, exponent);									// extra checking
	if ( *PowerOfTen * 10.0 <= IntervalSize)
		*PowerOfTen *= 10.0;
   *Index = 0;																		// Initial index is always zero
   return(WTNSet[*Index] * *PowerOfTen);									// my fix here
}

/****************************************************************************************
 FUNCTION:  NextNiceNum
 PURPOSE:  calculate the next nice number
 PARAMETERS:  WTNSet       - (I) Set of multipliers
              SetSize      - (I) Size of WTNSet
              Index        - (IO) Index to nice number
              PowerOfTen   - (IO) power of ten for nice number
 RETURNS:  the next nice number
 CALLS: none
 SYNTAX: static double NextNiceNum(double *WTNSet,
                                   int    SetSize,
                                   int    *Index,
                                   double *PowerOfTen);
 PROTOTYPE IN: (local)
 NOTES:
****************************************************************************************/

static double NextNiceNum(double *WTNSet, int SetSize, int *Index, double *PowerOfTen)  {
   (*Index)++;							// increment index

   // if maximum index has been exceeded, reset the index to zero
   // and increase the power of 10

   if ( *Index >= SetSize)  {
     *Index = 0;
     *PowerOfTen *= 10.0;
   }
   return(WTNSet[*Index] * *PowerOfTen);
}

/****************************************************************************************
 FUNCTION:  CalcExtLabel
 PURPOSE:  calculate an externmally labeled scale
 PARAMETERS: DataMin - (I) data minimum
             DataMax - (I) data maximum
             NiceNum - (I) Nice Number
             LoMult  - (O) Multiplier for scale minimum
             HiMult  - (O) Multiplier for scale maximum
 RETURNS:  none
 CALLS: floor (math.h)
        ceil  (math.h)
 SYNTAX:  static void CalcExtLabel(double DataMin,
                                   double DataMax,
                                   double NiceNum,
                                   int    *LoMult,
                                   int    *HiMult);
 PROTOTYPE IN: (local)
 NOTES:
****************************************************************************************/

static void CalcExtLabel(double DataMin, double DataMax, double NiceNum, int *LoMult, int *HiMult)  {
   *LoMult = (int) floor( DataMin/NiceNum);				// calculate low multiple

   if ( (double)(*LoMult +1) * NiceNum <= DataMin)		// perform extra checking
     (*LoMult)++;

   *HiMult = (int) ceil( DataMax / NiceNum);				// calculate high multiple

   if ( (double)(*HiMult -1) * NiceNum >= DataMax)		// perform extra checking
     (*HiMult)--;

}

/****************************************************************************************
 FUNCTION:  CalcIntLabel
 PURPOSE:  Calculate an internally labeled scale
 PARAMETERS:  DataMin - (I) data minimum
              DataMax - (I) data maximum
              NiceNum - (I) NiceNumber
              LoMult  - (O) multiplier for min ref value
              HiMult  - (O) multiplier for max ref value
 RETURNS: none
 CALLS:  floor (math.h)
 SYNTAX:  static void CalcIntLabel(double DataMin,
                                   double DataMax,
                                   double NiceNum,
                                   int    *LoMult,
                                   int    *HiMult);
 PROTOTYPE IN: (local)
 NOTES:
****************************************************************************************/

/*
//static void CalcIntLabel(double DataMin, double DataMax, double NiceNum, int *LoMult, int *HiMult)  {
void CalcIntLabel(double DataMin, double DataMax, double NiceNum, int *LoMult, int *HiMult)  {

   *LoMult = (int) floor( DataMax / NiceNum);				// calculate the low multiple

   if ( (double)(*LoMult -1) * NiceNum >= DataMin)  {		// perform extra checking
		(*LoMult)--;
	}

   *HiMult = (int)floor( DataMax / NiceNum);					// calculate the high multiple
   if ( (double)(*HiMult +1) * NiceNum <= DataMax)  {		// perform extra checking
		(*HiMult)++;
	}

	return;
}
*/



/*********************************************************************************
 FUNCTION:  power
 PURPOSE:  raise a double to an integer power
 PARAMETERS:  Root     - (I) root to be raised to a power
              Exponent - (I) power to raise root
 RETURNS: Root ^ Exponent
 CALLS:  none
 SYNTAX:  double power( double Root, int Exponent);
 PROTOTYPE IN: (local)
 NOTES:
***********************************************************************************/

double power (double Root, int Exponent)  {
   double result;

   // for negative exponents, invert root and use a positive exponent

   if ( Exponent < 0) {
     Root = 1 / Root;
     Exponent = -Exponent;
   }

#ifdef _DEBUG
   if (Exponent < 0.0f)  {
	   //int bp = 1;
   }
#endif

   // perform multiplications

	result = 1.0;

	while ( Exponent)  {
		if ( Exponent & 1)
			result *= Root;
		Exponent >>= 1;
		if ( Exponent)
			Root *= Root;
   }

   return result;

}

/*
double fround( double Value, int Digits ) {

	if( Value > 0.0 )  {
		return ( (long)( Value * Faktor + 0.5 ) ) / pow( 10.0, Digits);
	}

	return ( (long)( Value * Faktor - 0.5 ) ) / pow( 10.0, Digits);
}
*/

double dround(double z)  {
	char s[64];
	double d;

	sprintf(s,"%.6f", z); // s is z rounded to nearest 0.1
	d = atof(s);
	return d;
}



