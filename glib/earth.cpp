#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
	//#include <warnings.h>
#endif


#include <stdio.h>
#include <math.h>
#include <float.h>
#include <memory.h>

#include <string>

#include <glib_defines.h>

#include <earth.h>

Earth::Earth(void)  {


	major = 6378137.0;						// WGS84 semi major axis radius
	//minor = 6356752.3141;					// WGS84 semi minor axis radius from .pdf file
	minor = 6356752.314245;					// WGS84 semi minor axis radius from another source

	// for testing against the .pdf file:
	//major = 6377563.396;						// AIRY 1830 semi-major axis in meters
	//minor = 6356256.910;						// Airy 1830 semi-minor axis in meters

	F = 1.0 / 298.25722210088;
	pi = Pi();
	TORAD = 180.0 / pi;		// divisor!!!!!
	e = 2.0*F - F*F;		// eccentricity squared .0066943800229035
	TWO_PI = 2.0*Pi();
	ind = 0.0;


	//initTM(49.0, -2.0, -100000.0, 400000.0);		// for Airy 1830 test
	initTM(45.0, -40.0, 0.0, 0.0);


	double clat, clon;
	double falseE, falseN;
	double sfac;

	falseE = 0.0;				// 2611 km
	falseN = 0.0;			// 763,464 km

	clat = 45.0000/TORAD;
	clon = -90.000/TORAD;

	sfac = .9996;				//???

	//tmforint(major, minor,
	tmforint(
		sfac,						// double scale_fact scale factor

		clat,						// center longitude		
		clon,						// center latitude

		falseE,					// double false_east x offset in meters	
		falseN);					// double false_north y offset in meters	

	double lat, lon;

	lat = clat;
	lon = clon;
	double x, y;

	int status = tmfor(lat, lon, &x, &y);

	falseE = -x;
	falseN = -y;
	//tmforint(major, minor, sfac, clat, clon, falseE, falseN);
	tmforint(sfac, clat, clon, falseE, falseN);

	status = tmfor(lat, lon, &x, &y);

	lat = 45.1 / TORAD;
	lon = -89.9 / TORAD;
	status = tmfor(lat, lon, &x, &y);

	// x and y should now be 0.0
}

/***************************************************************************

***************************************************************************/

Earth::~Earth()  {

}

/***************************************************************************

***************************************************************************/


double Earth::getDist(EARTHPOINT _a, EARTHPOINT _b, double *faz, double *baz)  {
	int lat1sign;
	int lon1sign;
	int lat2sign;
	int lon2sign;

	double dist;
	double lat1, lon1;
	double lat2, lon2;
	EARTHPOINT a, b;


	memcpy(&a, &_a, sizeof(EARTHPOINT));
	memcpy(&b, &_b, sizeof(EARTHPOINT));

	//-----------------
	// first point:
	//-----------------


	if(a.lat.deg < 0)	 {
		a.lat.deg = (short) fabs((double)a.lat.deg);
		a.lat.min = (short) fabs((double)a.lat.min);
		a.lat.sec = fabs(a.lat.sec);
		lat1sign = -1;
	}
	else  {
		lat1sign = 1;
	}

	// here is where my bearing calculation was off:

	if(a.lon.deg < 0) {
		a.lon.deg = (short) fabs((double)a.lon.deg);
		a.lon.min = (short) fabs((double)a.lon.min);
		a.lon.sec = fabs(a.lon.sec);
		lon1sign = -1;
	}
	else  {
		lon1sign = 1;
	}

	lat1 = getrad(a.lat, lat1sign);
	lon1 = getrad(a.lon, lon1sign);

	//-----------------
	// second point:
	//-----------------


	if(b.lat.deg < 0)	{
		b.lat.deg = (short) fabs((double)b.lat.deg);
		b.lat.min = (short) fabs((double)b.lat.min);
		b.lat.sec = fabs(b.lat.sec);
		lat2sign = -1;
	}
	else  {
		lat2sign = 1;
	}

	if(b.lon.deg < 0.0) {
		b.lon.deg = (short) fabs((double)b.lon.deg);
		b.lon.min = (short) fabs((double)b.lon.min);
		b.lon.sec = fabs(b.lon.sec);
		lon2sign = -1;
	}
	else  {
		lon2sign = 1;
	}

	lat2 = getrad(b.lat, lat2sign);
	lon2 = getrad(b.lon, lon2sign);

	inver1(lat1, lon1, lat2, lon2, faz, baz, &dist);

	// faz & baz are in radians
	// this works, but I'd rather have decimal degrees
	//ANGLE angle;
	//todms(*faz, &angle);

	// convert to decimal degrees:

	*faz = ConvertRadiansToDegrees(*faz);
	*baz = ConvertRadiansToDegrees(*baz);


	return dist;

}




/***************************************************************************
	comvert deg, min, sec to radians

To convert Degrees-Minutes-Seconds (D M S) to decimal degrees (dd.ddddd),
  dd.ddddd = D + M/60 + S/3600

***************************************************************************/

double Earth::getrad(ANGLE angle, int isign)  {
	double val;

	val = ((double)angle.deg + (double)angle.min/60.0 +  angle.sec/3600.0) / TORAD;
	val *= isign;
	return val;
}

/***************************************************************************

  SOLUTION OF THE GEODETIC INVERSE PROBLEM AFTER T.Earth
  MODIFIED RAINSFORD'S METHOD WITH HELMERT'S ELLIPTICAL TERMS
  EFFECTIVE IN ANY AZIMUTH AND AT ANY DISTANCE SHORT OF ANTIPODAL
  STANDPOINT/FOREPOINT MUST NOT BE THE GEOGRAPHIC POLE

  A IS THE SEMI-MAJOR AXIS OF THE REFERENCE ELLIPSOID
  F IS THE FLATTENING (NOT RECIPROCAL) OF THE REFERNECE ELLIPSOID
  LATITUDES AND LONGITUDES IN RADIANS POSITIVE NORTH AND EAST

  FORWARD AZIMUTHS AT BOTH POINTS RETURNED IN RADIANS FROM NORTH

  PROGRAMMED FOR CDC-6600 BY LCDR L.PFEIFER NGS ROCKVILLE MD 18FEB75
  MODIFIED FOR IBM SYSTEM 360 BY JOHN G GERGEN NGS ROCKVILLE MD 7507

	from http://www.ngs.noaa.gov/TOOLS/Inv_Fwd/Inv_Fwd.html:

  Program INVERSE is the tool for computing the geodetic azimuth and ellipsoidal 
  distance between two points given their latitudes and longitudes in degrees, 
  minutes, and seconds. The user may select from a variety of reference ellipsoids, 
  including Clarke 1866 (NAD 27) and GRS80/WGS84 (NAD 83), or may define their own 
  reference ellipsoid in whatever units they prefer. The units for the listed 
  ellipsoids are given in meters, and therefore the distances are also given in 
  meters. Note that for the purpose of this application GRS80 and WGS84 are considered 
  to be equivalent. The program reads input geodetic positions as positive north and 
  positive west. 


*****************************************************************************/

void Earth::inver1(
				double lat1, 
				double lon1, 
				double lat2, 
				double lon2, 
				double *faz, 
				double *baz, 
				double *S)  {

	double R;
	double TU1, TU2, CU1, SU1, CU2;
	double X;
	double SX, CX, SY, CY;
	double Y, SA, C2A, CZ;
	double E, C, D;
//	double EPS = .5e-13;				// error allowed
	double EPS = .5e-15;				// error allowed

#ifdef _DEBUG
	static int count=0;
	FILE *stream;
	count++;
	if (count==239)  {
		stream = fopen("earth.log", "a+t");
		//fprintf(stream, "%10f %10f %10f %10f\n", lat1, lon1, lat2, lon2);
	}
#endif

	R = 1.0 - F;

	TU1 = R * sin(lat1) / cos(lat1);
	TU2 = R * sin(lat2) / cos(lat2);
	CU1 = 1.0 / sqrt(TU1*TU1 + 1.0);
	SU1 = CU1*TU1;
	CU2 = 1.0 / sqrt(TU2*TU2 + 1.0);
	*S = CU1*CU2;
	*baz = (*S) * TU2;
	*faz = (*baz) * TU1;
	X = lon2 - lon1;

#ifdef _DEBUG
	if (count==239)  {
		//fprintf(stream, "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f",
		//	lat1, lon1, lat2, lon2, X, *S, TU1, TU2, CU1, SU1, CU2);
		//fprintf(stream, "*faz = %12.6f   ", *faz);
	}	
#endif

	int loops = 0;

	do  {
		SX = sin(X);
		CX = cos(X);
		TU1 = CU2*SX;
		TU2 = (*baz) - SU1*CU2*CX;
		SY = sqrt(TU1*TU1 + TU2*TU2);
		CY = (*S)*CX + *faz;
		Y = atan2(SY, CY);
		SA = (*S)*SX/SY;
		C2A = -SA*SA + 1.;
		CZ = *faz + *faz;
		if(C2A > 0.0) {
			CZ = -CZ/C2A + CY;
		}

		E = CZ*CZ*2.0 - 1.0;
		C = ((-3.0*C2A + 4.0) * F + 4.0)*C2A*F/16.0;
		D = X;
		X = ((E*CY*C + CZ)*SY*C + Y) * SA;
		X = (1.0 - C) * X * F  +  lon2 - lon1;

		loops++;

	} while (fabs(D-X) > EPS);


	*faz = atan2(TU1,TU2);							// -2.5718458748280
	*baz = atan2(CU1*SX, (*baz)*CX-SU1*CU2) + pi;


	X = sqrt((1.0/R/R - 1.0)*C2A + 1.0) + 1.0;
	X = (X-2.0)/X;
	C = 1.0 - X;
	C = (X*X/4.0 + 1.0) / C;
	D = (0.375*X*X - 1.0)*X;
	X = E*CY;
	*S = 1.0 - E-E;
	*S = ((((SY*SY*4.0 - 3.0)*(*S)*CZ*D/6.0 - X)*D/4.0 + CZ)*SY*D+Y)*C*major*R;

#ifdef _DEBUG
	if (count==239)  {
		//fprintf(stream, " %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f\n",
		//	*S, SY, CZ, D, X, Y, C);
		ANGLE angle;
		todms(lat1, &angle);
		fprintf(stream, "\nlat1 = %d degrees, %d minutes, %.6f seconds\n", angle.deg, angle.min, angle.sec);

		todms(lon1, &angle);
		fprintf(stream, "lon1 = %d degrees, %d minutes, %.6f seconds\n", angle.deg, angle.min, angle.sec);

		todms(lat2, &angle);
		fprintf(stream, "lat2 = %d degrees, %d minutes, %.6f seconds\n", angle.deg, angle.min, angle.sec);

		todms(lon2, &angle);
		fprintf(stream, "lon2 = %d degrees, %d minutes, %.6f seconds\n", angle.deg, angle.min, angle.sec);

		fprintf(stream, "distance = %.6f meters\n", *S);

		fprintf(stream, "%.10f %.10f %.10f %.10f  %.10f\n", lat1, lon1, lat2, lon2, *S);

		// print it in decimal degrees
		double tlat1, tlon1, tlat2, tlon2;

	   tlat1 = ConvertRadiansToDegrees(lat1);
	   tlon1 = ConvertRadiansToDegrees(lon1);

		tlat2 = ConvertRadiansToDegrees(lat2);
	   tlon2 = ConvertRadiansToDegrees(lon2);

		fprintf(stream, "%.10f %.10f %.10f %.10f  %.10f\n", tlat1, tlon1, tlat2, tlon2, *S);

		fclose(stream);
	}	
#endif

	//test for QNAN
#ifdef WIN32
	if (_finite(*S)==0)  {
		*S = 0.0;
	}
#else
	if (!(isinf(*S)==0))  {
		*S = 0.0;
	}
#endif


	return;
}


/***************************************************************************
 convert position radians to deg,min,sec
 range is [-pi to +pi]
***************************************************************************/

void Earth::todms(double _val, ANGLE *angle)  {
	long is;
	double val;

	val = _val;

	while(val>pi)  {
		val = val - pi - pi;
	}

	while(val < -pi)  {
		val = val + pi + pi;
	}

	angle->sec = fabs(val * TORAD);
	angle->deg = (short) idint(angle->sec);
	angle->sec = (angle->sec - angle->deg)*60.0;
	angle->min = (short) idint(angle->sec);
	angle->sec = (angle->sec - angle->min)*60.0;

	// account for rounding error

	is = idnint( (angle->sec) * 1.5);

	if(is >= 6000000) {
		angle->sec = 0.0;
		angle->min = (short) (angle->min + 1);
	}

	if(angle->min >= 60) {
		angle->min = 0;
		angle->deg += 1;
	}


	if(val < 0.0)  {
		angle->deg = (short) (359 - angle->deg);
		angle->min = (short) (59 - angle->min);
		angle->sec = 60.0 - angle->sec;
	}

	return;
}

/***************************************************************************

IDINT (number)

 A function that returns the largest integer whose absolute value
 does not exceed the absolute value of the argument and has the same
 sign as the argument.

 +------+---------+----------+------------+-------------+        
 | Args | Generic | Specific |  Argument  | Result Type |
 +------+---------+----------+------------+-------------+
 |  1   | IDINT   | IIDINT   | REAL*8     | INTEGER*2   |
 |      |         | JIDINT   | REAL*8     | INTEGER*4   |
 +------+---------+----------+------------+-------------+
 The function returns an INTEGER*4 value if the /I4 command
 qualifier is in effect; otherwise it returns an INTEGER*2 value.

 See also the INT intrinsic function.


***************************************************************************/


long Earth::idint(double d)  {

	return (long) d;			// truncate
}

/***************************************************************************
 IDNINT (real-number)

 A function that returns the value of the integer nearest to the
 value of the argument.

 +------+---------+----------+------------+-------------+        
 | Args | Generic | Specific |  Argument  | Result Type |
 +------+---------+----------+------------+-------------+
 |  1   | IDNINT  | IIDNNT   | REAL*8     | INTEGER*2   |
 |      |         | JIDNNT   | REAL*8     | INTEGER*4   |
 +------+---------+----------+------------+-------------+
 The function returns an INTEGER*4 value if the /I4 command
 qualifier is in effect; otherwise it returns an INTEGER*2 value.

 See also the NINT intrinsic function.


***************************************************************************/

long Earth::idnint(double d)  {
	return (long) (.5 + d);			// round
}


/***************************************************************************

***************************************************************************/

double Earth::ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( double degrees, double minutes, double seconds )  {
	double decimal_degrees = 0.0;

	decimal_degrees = degrees;

	if ( decimal_degrees < 0.0 ) {
		decimal_degrees *= (-1.0);
	}

	decimal_degrees += (double) ( minutes / 60.0 );
	decimal_degrees += (double) ( seconds / 3600.0 );

	if ( degrees < 0.0 )  {
		decimal_degrees *= (-1.0);
	}

	return( decimal_degrees );

}

/***************************************************************************

***************************************************************************/


double Earth::GetSurfaceDistance(DP point_1, DP point_2, double * heading_from_point_1_to_point_2_p, double * heading_from_point_2_to_point_1_p ) {

   // This is a translation of the Fortran routine INVER1 found in the
   // INVERS3D program at:
   // ftp://ftp.ngs.noaa.gov/pub/pcsoft/for_inv.3d/source/invers3d.for

   // The ton of variables used...

   double c           = 0.0;
   double c_value_1   = 0.0;
   double c_value_2   = 0.0;
   double c2a         = 0.0;
   double cosine_of_x = 0.0;
   double cy          = 0.0;
   double cz          = 0.0;
   double d           = 0.0;
   double e           = 0.0;
   double r_value     = 0.0;
   double s           = 0.0;
   double s_value_1   = 0.0;
   double sa          = 0.0;
   double sine_of_x   = 0.0;
   double sy          = 0.0;
   double tangent_1   = 0.0;
   double tangent_2   = 0.0;
   double x           = 0.0;
   double y           = 0.0;

//   int loop_count = 0;

   double heading_from_point_1_to_point_2 = 0.0;
   double heading_from_point_2_to_point_1 = 0.0;

   // UpDown    == Latitude
   // LeftRight == Longitude

   double point_1_latitude_in_radians  = ConvertDegreesToRadians(point_1.lat);
   double point_1_longitude_in_radians = ConvertDegreesToRadians(point_1.lon);

   double point_2_latitude_in_radians  = ConvertDegreesToRadians( point_2.lat);
   double point_2_longitude_in_radians = ConvertDegreesToRadians(point_2.lon);

   //r_value = 1.0 - m_Flattening;
   r_value = 1.0 - F;
   tangent_1 = ( r_value * sin( point_1_latitude_in_radians ) ) / cos( point_1_latitude_in_radians );
   tangent_2 = ( r_value * sin( point_2_latitude_in_radians ) ) / cos( point_2_latitude_in_radians );
   c_value_1 = 1.0 / sqrt( ( tangent_1 * tangent_1 ) + 1.0 );
   s_value_1 = c_value_1 * tangent_1;
   c_value_2 = 1.0 / sqrt( ( tangent_2 * tangent_2 ) + 1.0 );
   s = c_value_1 * c_value_2;

   heading_from_point_2_to_point_1 = s * tangent_2; // backward_azimuth
   heading_from_point_1_to_point_2 = heading_from_point_2_to_point_1 * tangent_1;

   x = point_2_longitude_in_radians - point_1_longitude_in_radians;

   bool exit_loop = false;

   while(exit_loop != true)  {
      sine_of_x   = sin( x );
      cosine_of_x = cos( x );
      tangent_1 = c_value_2 * sine_of_x;
      tangent_2 = heading_from_point_2_to_point_1 - ( s_value_1 * c_value_2 * cosine_of_x );
      sy = sqrt( ( tangent_1 * tangent_1 ) + ( tangent_2 * tangent_2 ) );
      cy = ( s * cosine_of_x ) + heading_from_point_1_to_point_2;
      y = atan2( sy, cy );

      // Thanks to John Werner (werner@tij.wb.xerox.com) for
      // finding a bug where sy could be zero. Here's his fix:

      if ( ( s * sine_of_x ) == 0.0 && ( sy == 0.0 ) )    {
         sa = 1.0;
      }
      else   {
         sa = ( s * sine_of_x ) / sy;
      }

      c2a = ( (-sa) * sa ) + 1.0;
      cz = heading_from_point_1_to_point_2 + heading_from_point_1_to_point_2;

      if ( c2a > 0.0 )   {
         cz = ( (-cz) / c2a ) + cy;
      }

      e = ( cz * cz * 2.0 ) - 1.0;
      c = ( ( ( ( ( -3.0 * c2a ) + 4.0 ) * F ) + 4.0 ) * c2a * F ) / 16.0;
      d = x;
      x = ( ( ( ( e * cy * c ) + cz ) * sy * c ) + y ) * sa;
      x = ( ( 1.0 - c ) * x * F ) + point_2_longitude_in_radians - point_1_longitude_in_radians;

      if ( fabs( d - x ) > 0.00000000000000000000005 )   {
         exit_loop = false;
      }
      else   {
         exit_loop = true;
      }
   }

   heading_from_point_1_to_point_2 = atan2( tangent_1, tangent_2 );

//   double temp_degrees         = 0.0;
//   double temp_minutes         = 0.0;
//   double temp_seconds         = 0.0;
   double temp_decimal_degrees = 0.0;

   temp_decimal_degrees = ConvertRadiansToDegrees( heading_from_point_1_to_point_2 );

   if ( temp_decimal_degrees < 0.0 )  {
      temp_decimal_degrees += 360.0;
   }

   if ( heading_from_point_1_to_point_2_p != NULL )   {
      // The user passed us a pointer, don't trust it.
      // If you are using Visual C++ on Windows NT, the following
      // try/catch block will ensure you won't blow up when random
      // pointers are passed to you. If you are on a legacy operating
      // system like Unix, you are screwed.

      try   {
         *heading_from_point_1_to_point_2_p = temp_decimal_degrees;
      }
      catch( ... )     {
         // Do Nothing
      }
   }

   heading_from_point_2_to_point_1 = atan2( c_value_1 * sine_of_x, ( (heading_from_point_2_to_point_1 * cosine_of_x ) - ( s_value_1 * c_value_2 ) ) ) + pi;

   temp_decimal_degrees = ConvertRadiansToDegrees( heading_from_point_2_to_point_1 );

   if ( temp_decimal_degrees < 0 )  {
      temp_decimal_degrees += 360.0;
   }

   if ( heading_from_point_2_to_point_1_p != NULL )  {
      // The user passed us a pointer, don't trust it.
      // If you are using Visual C++ on Windows NT, the following
      // try/catch block will ensure you won't blow up when random
      // pointers are passed to you. If you are on a legacy operating
      // system like Unix, you are screwed.

      try  {
         *heading_from_point_2_to_point_1_p = temp_decimal_degrees;
      }
      catch( ... )  {
         // Do Nothing
      }
   }

   x = sqrt( ( ( ( 1.0 / r_value / r_value ) - 1 ) * c2a ) + 1.0 ) + 1.0;
   x = ( x - 2.0 ) / x;
   c = 1.0 - x;
   c = ( ( ( x * x ) / 4.0 ) + 1.0 ) / c;
   d = ( ( 0.375 * ( x * x ) ) - 1.0 ) * x;

   x = e * cy;

   s = ( 1.0 - e ) - e;

   double term_1 = 0.0;
   double term_2 = 0.0;
   double term_3 = 0.0;
   double term_4 = 0.0;
   double term_5 = 0.0;

   term_1 = ( sy * sy * 4.0 ) - 3.0;
   term_2 = ( ( s * cz * d ) / 6.0 ) - x;
   term_3 = term_1 * term_2;
   term_4 = ( ( term_3 * d ) / 4.0 ) + cz;
   term_5 = ( term_4 * sy * d ) + y;

   //s = term_5 * c * m_EquatorialRadiusInMeters * r_value;
	s = term_5 * c * major * r_value;

   return( s );
}

/***************************************************************************

***************************************************************************/

double Earth::ConvertDegreesToRadians(double degrees)  {
   double radians           = 0.0;

   double pi_divided_by_180 = pi / 180.0;
   
   radians = degrees * pi_divided_by_180;

   return(radians);
}

/***************************************************************************

***************************************************************************/

double Earth::Pi( void ) {
   return( 3.1415926535897932384626433832795028841971693993751058209749445923078164 );
}

/***************************************************************************

***************************************************************************/

double Earth::ConvertRadiansToDegrees(const double &radians)  {
   double degrees = 0.0;
   double conversion_factor = 180.0 / pi;
   degrees = radians * conversion_factor;
   return( degrees );
}

/***************************************************************************

***************************************************************************/


void Earth::ConvertDecimalDegreesToDegreesMinutesSeconds( double decimal_degrees, double& degrees, double& minutes, double& seconds ) {
   double fractional_part = 0.0;

   double integer_part = 0;

   fractional_part = ::modf( decimal_degrees, &integer_part );

   degrees = integer_part;

   if ( decimal_degrees < 0.0 )
   {
      fractional_part *= (-1.0);
   }

   minutes = fractional_part * 60.0;

   fractional_part = ::modf( minutes, &integer_part );

   minutes = integer_part;

   seconds = fractional_part * 60.0;
}


/***************************************************************************
	method from geodesy project:
***************************************************************************/

double Earth::getDist2(EARTHPOINT _a, EARTHPOINT _b, double *faz, double *baz)  {
	DP dp1, dp2;					// decimal degree points

	dp1.lat = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( _a.lat.deg, _a.lat.min, _a.lat.sec );
	dp1.lon = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( _a.lon.deg, _a.lon.min, _a.lon.sec );

	dp2.lat = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( _b.lat.deg, _b.lat.min, _b.lat.sec );
	dp2.lon = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( _b.lon.deg, _b.lon.min, _b.lon.sec );

	double d;
	double h1=0.0, h2=0.0;

	d = GetSurfaceDistance(dp1, dp2, &h1, &h2);
	*faz = h1;
	*baz = h2;
	return d;

}

/**********************************************************************

	convert to WGS84 data to earth-centered, earth-fixed (ECEF) coordinates:
	convert from ellipsoidal coordinates to cartesian coordinates:
	http://www.colorado.edu/geography/gcraft/notes/gps/gps_f.html
	http://www.spenvis.oma.be/spenvis/help/background/coortran/coortran.html
	http://apsg.home.texas.net/cref_0_3_3_C.html

  entry:
	angles are in degrees

**********************************************************************/

void Earth::ecef(double lat, double lon, double h, double *x, double *y, double *z)  {
	double phi, lambda;
	double cosphi, sinphi;
	double N;

	/*
	// for debugging:
	double latdeg, latmin, latsec;
	double londeg, lonmin, lonsec;
	ConvertDecimalDegreesToDegreesMinutesSeconds(lat, latdeg, latmin, latsec);
	ConvertDecimalDegreesToDegreesMinutesSeconds(lon, londeg, lonmin, lonsec);
	*/


	phi = (pi/180.0) * lat;					// convert latitude to radians
	lambda = (pi/180.0) * lon;				// convert logitude to radians

	sinphi = sin(phi);
	cosphi = cos(phi);

	N = major / sqrt(1.0 - e*e*sinphi*sinphi);

    
	*x = (N + h) * cosphi * cos(lambda);
	*y = (N + h) * cosphi * sin(lambda);
	*z = ( (N*(1.0 - e*e)) + h) * sinphi;

	return;
}

/***************************************************************************

  SOLUTION OF THE GEODETIC DIRECT PROBLEM AFTER T.VINCENTY
  MODIFIED RAINSFORD'S METHOD WITH HELMERT'S ELLIPTICAL TERMS
  EFFECTIVE IN ANY AZIMUTH AND AT ANY DISTANCE SHORT OF ANTIPODAL

  A IS THE SEMI-MAJOR AXIS OF THE REFERENCE ELLIPSOID
  F IS THE FLATTENING OF THE REFERENCE ELLIPSOID
  LATITUDES AND LONGITUDES IN RADIANS POSITIVE NORTH AND EAST
  AZIMUTHS IN RADIANS CLOCKWISE FROM NORTH
  GEODESIC DISTANCE S ASSUMED IN UNITS OF SEMI-MAJOR AXIS A

  PROGRAMMED FOR CDC-6600 BY LCDR L.PFEIFER NGS ROCKVILLE MD 20FEB75
  MODIFIED FOR SYSTEM 360 BY JOHN G GERGEN NGS ROCKVILLE MD 750608

	from http://www.ngs.noaa.gov/TOOLS/Inv_Fwd/Inv_Fwd.html:

	Program FORWARD is the tool for computing the geodetic position 
	(latitude and longitude in degrees, minutes, and seconds) of a point given 
	the geodetic position of a beginning point and the geodetic azimuth and 
	ellipsoidal distance between the two points. The user may select from a variety 
	of reference ellipsoids, including Clarke 1866 (NAD 27) and GRS80/WGS84 (NAD 83), 
	or may define their own reference ellipsoid in whatever units they prefer. 
	The units for the listed ellipsoids are given in meters, and therefore the 
	distances should also be given in meters. Note that for the purpose of this 
	application GRS80 and WGS84 are considered to be equivalent. The program reads 
	input geodetic positions as positive north and positive west. 


	entry:
		point A = lat1, lon1 (in radians)
		faz = angle from POINT A TO POINT B (in radians)
		s = distance from point a to point b

	exit:
		point b = lat2, lon2 (in radians)
		baz = angle from point b to point a? (in radians)


***************************************************************************/

void Earth::direct(double lat1, double lon1, double &lat2, double &lon2, double faz,double &baz, double s)  {

	double R, TU, SF, CF, CU, SU, SA, C2A, X, C, D, Y, SY, CY, CZ, E;
	double EPS=.5e-13;

	R = 1.0 - F;
	TU = R*sin(lat1) / cos(lat1);
	SF = sin(faz);
	CF = cos(faz);
	baz = 0.0;

	if(CF != 0.0)  {
		baz = atan2(TU,CF)*2.0;
	}

	CU = 1.0 / sqrt(TU*TU+1.0);
	SU = TU*CU;
	SA = CU*SF;
	C2A = -SA*SA+1.0;
	X = sqrt((1.0/R/R - 1.0)*C2A+1.0) + 1.0;
	X = (X-2.0)/X;
	C = 1.0 - X;
	C = (X*X/4.0 + 1)/C;
	D = (0.375*X*X - 1.0)*X;
	TU = s/R/major/C;
	Y = TU;

	do  {
		SY = sin(Y);
		CY = cos(Y);
		CZ = cos(baz + Y);
		E = CZ*CZ*2.0 - 1.0;
		C = Y;
		X = E*CY;
		Y = E + E - 1.0;
		Y = (((SY*SY*4.0 - 3.0)*Y*CZ*D/6.0 + X)*D/4.0-CZ)*SY*D+TU;
	}  while(fabs(Y-C) > EPS);


	baz = CU*CY*CF-SU*SY;
	C = R*sqrt(SA*SA + baz*baz);
	D = SU*CY+CU*SY*CF;
	lat2 = atan2(D,C);

	C = CU*CY - SU*SY*CF;
	X = atan2(SY*SF,C);
	C = ((-3.0*C2A + 4.0)*F + 4.0) * C2A * F / 16.0;
	D = ((E*CY*C+CZ)*SY*C+Y)*SA;
	lon2 = lon1 + X - (1.0 - C)*D*F;

	baz = atan2(SA, baz) + pi;

	return;
}



/*********************************************************************************
 Transverse Mercator forward equations--mapping lat,long to x,y
   Note:  The algorithm for TM is exactly the same as UTM and therefore
	  if a change is implemented, also make the change to UTMFOR.c
*********************************************************************************/

//xxx

int Earth::tmfor(double lat, double lon, double *x, double *y)  {
	double delta_lon;				// Delta longitude (Given longitude - center 	
	double sin_phi, cos_phi;	// sin and cos value				
	double al, als;				// temporary values				
	double b;						// temporary values				
	double c, t, tq;				// temporary values				
	double con, n, ml;			// cone constant, small m			

	// Forward equations

	delta_lon = adjust_lon(lon - lon_center);
	sincos(lat, &sin_phi, &cos_phi);

	// This part was in the fortran code and is for the spherical form 

	if (ind != 0)  {
		// this is spherical stuff
		b = cos_phi * sin(delta_lon);
		if ((fabs(fabs(b) - 1.0)) < .0000000001)  {
			//p_error("Point projects into infinity","tm-for");
			return 1;
		}
		else  {
			*x = .5 * r_major * scale_factor * log((1.0 + b)/(1.0 - b));
			con = acos(cos_phi * cos(delta_lon)/sqrt(1.0 - b*b));
			if (lat < 0)
				con = - con;
			*y = r_major * scale_factor * (con - lat_origin); 
			return 0;
		}
	}

	al  = cos_phi * delta_lon;
	als = SQUARE(al);
	c   = esp * SQUARE(cos_phi);
	tq  = tan(lat);
	t   = SQUARE(tq);
	con = 1.0 - es * SQUARE(sin_phi);
	n   = r_major / sqrt(con);
	ml  = r_major * mlfn(e0, e1, e2, e3, lat);

	*x  = scale_factor * n * al * (1.0 + als / 6.0 * (1.0 - t + c + als / 20.0 *
		   (5.0 - 18.0 * t + SQUARE(t) + 72.0 * c - 58.0 * esp))) + false_easting;

	*y  = scale_factor * (ml - ml0 + n * tq * (als * (0.5 + als / 24.0 *
		   (5.0 - t + 9.0 * c + 4.0 * SQUARE(c) + als / 30.0 * (61.0 - 58.0 * t
			+ SQUARE(t) + 600.0 * c - 330.0 * esp))))) + false_northing;

	return 0;
}

/*
	Function to adjust a longitude angle to range from -180 to 180 radians

	entry:
		x is angle in degrees

*/


double Earth::adjust_lon(double x)  {

	//long temp;
	long count = 0;

	for(;;)  {
		if (fabs(x)<=PI)
			break;
		else
			if (((long) fabs(x / PI)) < 2)
				x = x-(sign(x) *TWO_PI);
					else
						if (((long) fabs(x / TWO_PI)) < MAXLONGTLM)   {
							x = x-(((long)(x / TWO_PI))*TWO_PI);
						}
						else
							if (((long) fabs(x / (MAXLONGTLM * TWO_PI))) < MAXLONGTLM)   {
								x = x-(((long)(x / (MAXLONGTLM * TWO_PI))) * (TWO_PI * MAXLONGTLM));
							}
							else
								if (((long) fabs(x / (DBLLONG * TWO_PI))) < MAXLONGTLM)   {
									x = x-(((long)(x / (DBLLONG * TWO_PI))) * (TWO_PI * DBLLONG));
								}
								else
									x = x-(sign(x) *TWO_PI);
					count++;
		if (count > MAX_VAL)
			break;
	}

	return(x);
}

/*
 Function to calculate the sine and cosine in one call.  Some computer
   systems have implemented this function, resulting in a faster implementation
   than calling each function separately.  It is provided here for those
   computer systems which don`t implement this function
*/

void Earth::sincos(double val, double *sin_val, double *cos_val)  {
	*sin_val = sin(val); 
	*cos_val = cos(val); 
	return; 
}

/* Function computes the value of M which is the distance along a meridian
   from the Equator to latitude phi.
------------------------------------------------*/

double Earth::mlfn(double e0, double e1,double e2,double e3, double phi)  {
	return(e0*phi-e1*sin(2.0*phi)+e2*sin(4.0*phi)-e3*sin(6.0*phi));
}

/* Function to return the sign of an argument
  ------------------------------------------*/

int Earth::sign(double x) { 
	if (x < 0.0) 
		return(-1); 
	else 
		return(1);
}

/* Initialize the Transverse Mercator (TM) projection
  -------------------------------------------------*/

int Earth::tmforint(
	//double r_maj,				// major axis
	//double r_min,				// minor axis
	double scale_fact,		// scale factor
	double center_lat,		// center latitude		
	double center_lon,		// center longitude		
	double false_east,		// x offset in meters	
	double false_north)		// y offset in meters	
{

	double temp;			// temporary variable		
	//double e0fn(),e1fn(),e2fn(),e3fn(),mlfn(); 	// functions	

	// Place parameters in static storage for common use

	//r_major = r_maj;
	//r_minor = r_min;
	r_major = major;
	r_minor = minor;
	scale_factor = scale_fact;
	lon_center = center_lon;
	lat_origin = center_lat;
	false_northing = false_north;
	false_easting = false_east;

	temp = r_minor / r_major;
	es = 1.0 - SQUARE(temp);
	e = sqrt(es);
	e0 = e0fn(es);
	e1 = e1fn(es);
	e2 = e2fn(es);
	e3 = e3fn(es);
	ml0 = r_major * mlfn(e0, e1, e2, e3, lat_origin);
	esp = es / (1.0 - es);

	if (es < .00001)
		ind = 1;
	else 
		ind = 0;

	/* Report parameters to the user
	-----------------------------*/

	//ptitle("TRANSVERSE MERCATOR (TM)"); 
	//radius2(r_major, r_minor);			// tlm, just prints stuff
	//genrpt(scale_factor,"Scale Factor at C. Meridian:    ");
	//cenlonmer(lon_center);
	//origin(lat_origin);
	//offsetp(false_easting,false_northing);

	return 0;
}

/* Functions to compute the constants e0, e1, e2, and e3 which are used
   in a series for calculating the distance along a meridian.  The
   input x represents the eccentricity squared.
----------------------------------------------------------------*/

double Earth::e0fn(double x) {
	return(1.0-0.25*x*(1.0+x/16.0*(3.0+1.25*x)));
}

double Earth::e1fn(double x) {
	return(0.375*x*(1.0+0.25*x*(1.0+0.46875*x)));
}

double Earth::e2fn(double x)  {
	return(0.05859375*x*x*(1.0+0.75*x));
}

double Earth::e3fn(double x)  {
	return(x*x*x*(35.0/3072.0));
}


/**********************************************************************
           PROGRAM UTMS
*
*     THIS PROGRAM CONVERTS GPS TO UNIVERSIAL TRANSVERSE MERACTOR COORDINATES
*     AND VICE VERSA FOR THE NAD27 AND NAD83 DATUM.
*     THIS PROGRAM WAS WRITTEN BY E. CARLSON
*     SUBROUTINES TMGRID, TCONST, TMGEOD, TCONPC,
*     WERE WRITTEN BY T. VINCENTY, NGS, IN JULY 1984 .
*     THE ORGINAL PROGRAM WAS WRITTEN IN SEPTEMBER OF 1988.
*
*     THIS PROGRAM WAS UPDATED ON FEBUARY 16, 1989.  THE UPDATE WAS
*     HAVING THE OPTION OF SAVING AND *81* RECORD FILE.
*
*
*     THIS PROGRAM WAS UPDATED ON APRIL 3, 1990.  THE FOLLOWING UPDATE 
*     WERE MADE:
*      1. CHANGE FROM JUST A CHOICE OF NAD27 OF NAD83 REFERENCE
*         ELLIPSOIDS TO; CLARKE 1866, GRS80/WGS84, INTERNATIONAL, AND
*         ALLOW FOR USER DEFINED OTHER.
*      2. ALLOW USE OF LATITUDES IN SOUTHERN HEMISPHERE AND LONGITUDES
*         UP TO 360 DEGREES WEST.
*
*     THIS PROGRAM WAS UPDATED ON DECEMBER 1, 1993. THE FOLLOWING UPDATE
*     WAS MADE:
*      1. THE NORTHINGS WOULD COMPUTE THE RIGHT LATITUDE IN THE SOUTHERN
*         HEMISHPERE.
*      2. THE COMPUTED LATITUDE ON LONGIDUTES WOULD BE EITHER  IN E OR W.     
*
*****************************************************************     *
*                  DISCLAIMER                                         *
*                                                                     *
*   THIS PROGRAM AND SUPPORTING INFORMATION IS FURNISHED BY THE       *
* GOVERNMENT OF THE UNITED STATES OF AMERICA, AND IS ACCEPTED AND     *
* USED BY THE RECIPIENT WITH THE UNDERSTANDING THAT THE UNITED STATES *
* GOVERNMENT MAKES NO WARRANTIES, EXPRESS OR IMPLIED, CONCERNING THE  *
* ACCURACY, COMPLETENESS, RELIABILITY, OR SUITABILITY OF THIS         *
* PROGRAM, OF ITS CONSTITUENT PARTS, OR OF ANY SUPPORTING DATA.       *
*                                                                     *
*   THE GOVERNMENT OF THE UNITED STATES OF AMERICA SHALL BE UNDER NO  *
* LIABILITY WHATSOEVER RESULTING FROM ANY USE OF THIS PROGRAM.  THIS  *
* PROGRAM SHOULD NOT BE RELIED UPON AS THE SOLE BASIS FOR SOLVING A   *
* PROBLEM WHOSE INCORRECT SOLUTION COULD RESULT IN INJURY TO PERSON   *
* OR PROPERTY.                                                        *
*                                                                     *
*   THIS PROGRAM IS PROPERTY OF THE GOVERNMENT OF THE UNITED STATES   *
* OF AMERICA.  THEREFORE, THE RECIPIENT FURTHER AGREES NOT TO ASSERT  *
* PROPRIETARY RIGHTS THEREIN AND NOT TO REPRESENT THIS PROGRAM TO     *
* ANYONE AS BEING OTHER THAN A GOVERNMENT PROGRAM.                    *
*                                                                     *
***********************************************************************
 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      CHARACTER*1 YN,DATNUM,DATUM
      COMMON/CONST/RAD,ER,RF,ESQ,PI
      COMMON/DATUM/DATNUM
 
      PI=4.D0*DATAN(1.D0)
      RAD=180.D0/PI
 
      PRINT *, '  ***  PROGRAM UTMS   *****'
      PRINT *, '                           '
      PRINT *, '      WRITTEN ON 09/23/88  '
      PRINT *, '      UPDATED ON 04/03/90  '
      PRINT *, '      UPDATED ON 12/01/93  '
      PRINT *, '                           '
     
    5 WRITE(6,10)
   10 FORMAT(' DO YOU WANT TO COMPUTE: '//,
     &       ' 1  GEODETIC POSITIONS TO UTM COORDINATES '/,
     &       ' 2  UTM COORDINATES TO GEODETIC POSITIONS'/,
     &       ' 3  PRINT THE OUTPUT FILE ON THE PRINTER'//,
     &       ' TYPE NUMBER '\)
       READ(5,15) INUM
   15 FORMAT(I1)
 
      IF(INUM.EQ.3) THEN
          CALL LSTFTN

          WRITE(6,16)
          READ(5,21) YN
          IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
             CLOSE(2,STATUS='KEEP')
             GO TO 5
          ELSE
             GO TO 25
          ENDIF
       ENDIF
****
****
        CALL DATUMM(ER,RF,F,ESQ,DATNUM)

***
***   USE THE NUM TO DO THE CORRECT FUNCTION    

      IF(INUM.EQ.1) THEN
         CALL GPUT83
         WRITE(6,16)
   16    FORMAT('0  DO YOU WANT TO GO TO ANOTHER '/,
     &          '   FUNCTION (Y/N) '//,
     &           '  TYPE ANSWER '\)
         READ(5,21) YN
          IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
             CLOSE(2,STATUS='KEEP')
             GO TO 5
          ELSE
             GO TO 25
          ENDIF
      ELSEIF(INUM.EQ.2) THEN
         CALL UTGP83
         WRITE(6,16)
         READ(5,21) YN
          IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
             CLOSE(2,STATUS='KEEP')
             GO TO 5
          ELSE
             GO TO 25
          ENDIF

      ELSE
        WRITE(6,20)
   20   FORMAT('0  YOU ENTERED A WRONG NUMBER'/,
     &       ' DO WANT TO TRY AGAIN (Y/N) '//,
     &       ' TYPE ANSWER '\)
        READ(5,21) YN
   21   FORMAT(A1)
 
        IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
          GO TO 5
        ENDIF
      ENDIF



 
   25 WRITE(6,30)
   30 FORMAT('0END OF PROGRAM UMTS  ')
      STOP
      END
********************************************************************
       SUBROUTINE UTGP83
*
***********************************************************************
 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      CHARACTER*1 YN
      COMMON/CONST/RAD,ER,RF,ESQ,PI
 
 
      WRITE(6,10)
   10 FORMAT('0*** ROUTINE UTMs TO GP  ***'//,
     *       ' DO YOU WANT TO RUN INTERACTIVELY (Y/N)? '/,
     *       ' TYPE ANSWER '\)
      READ(5,20) YN
   20 FORMAT(A1)
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
        CALL IUTPC
      ELSE
        CALL BUTPC
      ENDIF
 
      WRITE(6,30)
   30 FORMAT('0END OF ROUTINE UTMs TO GP')
      RETURN
      END
*****************************************************************
       SUBROUTINE GPUT83
*
***********************************************************************
 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      CHARACTER*1 YN
      COMMON/CONST/RAD,ER,RF,ESQ,PI
 
 
      WRITE(6,10)
   10 FORMAT('0*** ROUTINE GPs TO UTMs ***'//,
     *       ' DO YOU WANT TO RUN INTERACTIVELY (Y/N)? '/,
     *       ' TYPE ANSWER: '\)
      READ(5,20) YN
   20 FORMAT(A1)
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
        CALL IUTGP
      ELSE
        CALL BUTGP
      ENDIF
 
      WRITE(6,30)
   30 FORMAT('0END OF ROUTINE GPs TO UTMs')
      RETURN
      END
******************************************************************
      SUBROUTINE BUTPC
***************************************************************
      IMPLICIT REAL*8 (A-H,O-Z)
      CHARACTER*30 GPPFIL,PCFIL,GPFIL
      CHARACTER*80 CARDR
      CHARACTER*1 ANS,YN
      CHARACTER*1 DATUM,DATNUM
      LOGICAL FILFLAG,FILPRT
      REAL*8 NORTH
      COMMON/XY/NORTH,EAST
      COMMON/FILES/I3,I4,I2,ICON
      COMMON/DATUM/DATNUM
 
      FILFLAG=.TRUE.
      FILPRT=.FALSE.
 
        ICON=0
 
    5 WRITE(6,10)
   10 FORMAT('0NAME OF INPUT BLUEBOOK FILE WITH *81* RECORDS- ')
      READ(5,20) PCFIL
   20 FORMAT(A30)
 
      OPEN(2,STATUS='OLD',FILE=PCFIL,ERR=900)
 
      GO TO 25
 
 900  WRITE(6,901)
 901  FORMAT('0 FILE DOES NOT EXISTS, DO YOU WANT TO '/,
     &       '  TRY AGAIN (Y/N) '/,
     &       '  TYPE ANSWER '\)
 
      READ(5,902) YN
 902  FORMAT(A1)
 
         IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
            GO TO 5
         ELSE
            GO TO 99
         ENDIF
 
   25 WRITE(6,30)
   30 FORMAT('0NAME OF BLUEBOOK OUTPUT FILE WITH *80* RECORDS- ')
      READ(5,20) GPFIL
 
      OPEN(3,STATUS='NEW',FILE=GPFIL,ERR=910)
      GO TO 400
 
 910  WRITE(6,911)
 911  FORMAT('0 FILE ALREADY EXIST, DO YOU WANT TO '/,
     &       '  WRITE OVER IT (Y/N) '/,
     &       '  TYPE ANSWER : '\)
 
      READ(5,902) YN
 
           IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
               OPEN(3,STATUS='UNKNOWN',FILE=PCFIL)
           ELSE
               GO TO 25
           ENDIF
 
 
 400  WRITE(6,31)
 31   FORMAT(' DO YOU WANT THE OUTPUT LISTING SAVED ON A FILE (Y/N)'\)
      READ(5,32) ANS
 32   FORMAT(A1)
 
       IF ((ANS.EQ.'Y').OR.(ANS.EQ.'y')) THEN
           FILPRT=.TRUE.
 401       WRITE(6,33)
 33        FORMAT('0 FILE NAME:')
           READ(5,34) GPPFIL
 34        FORMAT(A30)
           OPEN(4,STATUS='NEW',FILE=GPPFIL,ERR=920)
            GO TO 405
 
 920  WRITE(6,911)
 
      READ(5,902) YN
 
           IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
               OPEN(4,STATUS='UNKNOWN',FILE=GPPFIL)
           ELSE
               GO TO 401
           ENDIF
 
 405       CALL HDUTPC
 
       ENDIF
 
 
 
 
   40 READ(2,45,END=99) CARDR
   45 FORMAT(A80)
 
          IF(CARDR(7:10).EQ.'*81*') THEN

           IF(DATNUM.NE.'2') THEN
              READ(CARDR,50,ERR=9000) EAST,NORTH,ICODE
   50         FORMAT(T45,F10.3,T55,F11.3,T66,I4)
              GO TO 51
           ENDIF

           IF(DATNUM.EQ.'2') THEN
              READ(CARDR,52,ERR=9000) NORTH,EAST,ICODE
   52         FORMAT(T45,F11.3,T56,F10.3,T66,I4)
              GO TO 51
           ENDIF

   51     CALL DRUTGP(CARDR,ICODE,FILFLAG,FILPRT)
 
          GO TO 40
 
          ELSE
          WRITE(3,45) CARDR
 
 
          GO TO 40
 
          ENDIF
 
          GO TO 99
 
 9000 WRITE(6,9001) CARDR
 9001 FORMAT('0 ERROR IN BLUE BOOK INPUT RECORD '/,A80)
 
 
 
   99 RETURN
      END
***********************************************************
      SUBROUTINE BUTGP
**********************************************************
      IMPLICIT REAL*8 (A-H,O-Z)
      CHARACTER*5 GGVAL,GDVAL
      CHARACTER*8 TITLE,GNUM
      DIMENSION GDNUM(1001),GDVAL(1001)
      CHARACTER*30 NAME,PCFIL,GPFIL,FIL81
      CHARACTER*80 CARDR
      CHARACTER*1 YN
      LOGICAL FILFLAG
      LOGICAL FIL81FL
      COMMON/LATLON/LD,LM,SLAT,LOD,LOM,SLON
      COMMON/FILES/I3,I4,I2,ICON
      COMMON/IPRINT/IPRT
      COMMON/DONUM/ISN
      COMMON/GEODS/GDVAL,GDNUM
      COMMON/TITLE/GNUM
 
 
      FILFLAG=.FALSE.
      FIL81FL=.FALSE.

          ICON = 0
          ISN = 0
 
    5 WRITE(6,10)
   10 FORMAT(' NAME OF INPUT FILE OF *80* RECORDS - ')
      READ(5,20) GPFIL
   20 FORMAT(A30)
 
 
      OPEN(2,STATUS='OLD',FILE=GPFIL,ERR=900)
 
      GO TO 25
 
  900 WRITE(6,901)
  901 FORMAT('  FILE DOES NOT EXITS, DO YOU WANT TO '/,
     &       '  TRY AGAIN (Y/N)'/,
     &       '  TYPE ANSWER '\)
 
      READ(5,902) YN
  902 FORMAT(A1)
 
        IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
            GO TO 5
        ELSE
            GO TO 99
        ENDIF

  25  PRINT *,' DO YOU WANT THE OUTPUT LISTING SAVED ON A FILE (Y/N) '
      READ(*,902) YN

      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN

 
   27 WRITE(6,30)
   30 FORMAT(' FILE NAME -')
      READ(5,20) PCFIL
 
 
      OPEN(3,STATUS='NEW',FILE=PCFIL,ERR=910)
       FILFLAG = .TRUE.
      GO TO 400
 
  910 WRITE(6,911)
  911 FORMAT('  FILE ALREADY EXIST, DO YOU WANT TO '/,
     &       '  WRITE OVER IT  (Y/N) '/,
     &       '  TYPE ANSWER : '\)
 
      READ(5,902) YN
 
       IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
          OPEN(3,STATUS='UNKNOWN',FILE=PCFIL)
           FILFLAG = .TRUE.
       ELSE
          GO TO 27
       ENDIF
 
 
 
 400  PRINT *,'            '
      PRINT *,'            '
      PRINT *,' THE TYPE OF COORDINATE LISING :  '
      PRINT *,'             '
      PRINT *,' 1 - PRELIMINARY (BEFORE FINAL ADJUSTMENT)'
      PRINT *,' 2 - FINAL (AFTER A COMPLETED PROJECT)'
      PRINT *,'         '
      PRINT *,' TYPE NUMBER: '
 
      READ(5,FMT='(I1)') IPRT
 
       PRINT *,'            '
       PRINT *,'            '
       PRINT *,'  TYPE THE PROJECT NUMBER  IE; G-12345 OR'
       PRINT *,'   GPS-1234 (8 CHARACTERS MAX) '
       PRINT *,'              '
       PRINT *, '  TYPE THE NUMBER : '
 
       READ(5,FMT='(A8)') GNUM

      ENDIF

***
**    FIND THE NAME OF THE *81* RECORD FILE IF WANTED
**
 
 250  WRITE(6,310)
 310  FORMAT(' DO YOU WANT TO SAVED AN *81* RECORD FILE (Y/N)? ')
      READ(5,902)YN

       IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
 325    WRITE(6,330)
 330    FORMAT(' FILE NAME:')
        READ(5,20) FIL81

        OPEN(4,STATUS='NEW',FILE=FIL81,ERR=800)
        FIL81FL=.TRUE.

      ENDIF
 
      GO TO 326
 
 800  WRITE(6,801)
 801  FORMAT('  FILE ALREADY EXIST, DO YOU WANT TO '/,
     &       '  WRITE OVER IT (Y/N) '/,
     &       '  TYPE ANSWER  '\)
      READ(5,902) YN
 
           IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
             OPEN(4,STATUS='UNKNOWN',FILE=FIL81)
             FIL81FL=.TRUE.

           ELSE
             GO TO 325
           ENDIF
 
 
 326        PRINT *,'                  '
            PRINT *,'                  '
            PRINT *, '   PROGRAM IS EXCUTING NOW !!!!!!! '
            PRINT *,'                      '
  
            PRINT *,'                      '

      IF(FILFLAG) THEN
        CALL HDUTGP
      ENDIF

***    READ INPUT FILE AND SET ARRAYS FOR THE GEOD HTS
 
  100 READ(2,45,END=199) CARDR
 
        IF(CARDR(7:10).EQ.'*84*') THEN
           ISN = ISN + 1
 
           GGVAL='     '
 
           READ(CARDR,FMT='(T11,I3)') IGDNUM
           READ(CARDR,FMT='(T72,A5)') GGVAL
           GDNUM(ISN)= IGDNUM
           GDVAL(ISN)= GGVAL
           GO TO 100
        ELSE
           GO TO 100
        ENDIF
 
  199  REWIND(2)
 
 
   40 READ(2,45,END=99) CARDR
   45 FORMAT(A80)
 
          IF(CARDR(7:10).EQ.'*80*') THEN
 
          CALL DRGPUT(CARDR,FILFLAG,FIL81FL)
 
          GO TO 40
 
          ELSE
 
          GO TO 40
 
          ENDIF
 
          GO TO 99
 
 9000 WRITE(6,9001) CARDR
 9001 FORMAT('  ERROR IN BLUE BOOK INPUT RECORD '/,A80)
 
   99 RETURN
      END
*******************************************************
      SUBROUTINE HDUTGP
******************************************************
       INTEGER*4 IPRT
       CHARACTER*1 DATNUM,DATUM
       CHARACTER*8 TITLE,GNUM
       COMMON/FILES/I3,I4,I2,ICON
       COMMON/IPRINT/IPRT
       COMMON/TITLE/GNUM
       COMMON/DATUM/DATNUM
 
        IF(IPRT.EQ.1) THEN
           WRITE(3,5) GNUM
    5      FORMAT('1',/,T50,'PRELIMINARY COORDINATE LISTING ',/ ,
     &             T59,'FOR ',A8,//)
        ELSEIF(IPRT.EQ.2) THEN 
           WRITE(3,6) GNUM
    6      FORMAT('1',/,T54,'FINAL COORDINATE LISTING ',/,
     &             T60,'FOR ',A8,//)
        ELSE
           WRITE(3,7)
    7      FORMAT('1',//////)
        ENDIF
 
         IF(DATNUM.EQ.'1') THEN
      WRITE(3,10)
   10 FORMAT(T54,'NATIONAL GEODETIC SURVEY',/,
     *       T58,'GP TO UTMs PROGRAM',T118,'VERSION 2.0',/,T55,
     *       'CLARKE 1866 ELLIPSOID',//,
     *        1X,' STATION NAME',T34,'LATITUDE',
     *       T50,'LONGITUDE',T68,'NORTHING(Y)',T80,'EASTING(X)',
     *       T92,'ZONE',T97,'CONVERGENCE',T110,'SCALE',T121,'ELEV',
     *       T129,'GEOID',/,T68,'METER',
     *       T80,'METER',T97,'D',T100,'M',T104,'S',
     *       T110,'FACTOR',T121,'(M)',T129,'HT(M)',//)
         ENDIF

 
         IF(DATNUM.EQ.'2') THEN
      WRITE(3,12)
   12 FORMAT(T54,'NATIONAL GEODETIC SURVEY',/,
     *       T58,'GP TO UTMs PROGRAM',T118,'VERSION 2.0',/,T56,
     *       'GRS80/WGS84 ELLIPSOID',//,
     *        1X,' STATION NAME',T34,'LATITUDE',
     *       T50,'LONGITUDE',T68,'NORTHING(Y)',T80,'EASTING(X)',
     *       T92,'ZONE',T97,'CONVERGENCE',T110,'SCALE',T121,'ELEV',
     *       T129,'GEOID',/,T68,'METER',
     *       T80,'METER',T97,'D',T100,'M',T104,'S',
     *       T110,'FACTOR',T121,'(M)',T129,'HT(M)',//)
         ENDIF
 
         IF(DATNUM.EQ.'3') THEN
      WRITE(3,13)
   13 FORMAT(T54,'NATIONAL GEODETIC SURVEY',/,
     *       T58,'GP TO UTMs PROGRAM',T118,'VERSION 2.0',/,T53,
     *       'INTERNATIONAL 1910 ELLIPSOID',//,
     *        1X,' STATION NAME',T34,'LATITUDE',
     *       T50,'LONGITUDE',T68,'NORTHING(Y)',T80,'EASTING(X)',
     *       T92,'ZONE',T97,'CONVERGENCE',T110,'SCALE',T121,'ELEV',
     *       T129,'GEOID',/,T68,'METER',
     *       T80,'METER',T97,'D',T100,'M',T104,'S',
     *       T110,'FACTOR',T121,'(M)',T129,'HT(M)',//)
         ENDIF

 
         IF(DATNUM.EQ.'4') THEN
      WRITE(3,14)
   14 FORMAT(T54,'NATIONAL GEODETIC SURVEY',/,
     *       T58,'GP TO UTMs PROGRAM',T118,'VERSION 2.0',/,T59,
     *       'WGS72 ELLIPSOID',//,
     *        1X,' STATION NAME',T34,'LATITUDE',
     *       T50,'LONGITUDE',T68,'NORTHING(Y)',T80,'EASTING(X)',
     *       T92,'ZONE',T97,'CONVERGENCE',T110,'SCALE',T121,'ELEV',
     *       T129,'GEOID',/,T68,'METER',
     *       T80,'METER',T97,'D',T100,'M',T104,'S',
     *       T110,'FACTOR',T121,'(M)',T129,'HT(M)',//)
         ENDIF

 
         IF(DATNUM.EQ.'5') THEN
      WRITE(3,15)
   15 FORMAT(T54,'NATIONAL GEODETIC SURVEY',/,
     *       T58,'GP TO UTMs PROGRAM',T118,'VERSION 2.0',/,T59,
     *       'OTHER ELLIPSOID',//,
     *        1X,' STATION NAME',T34,'LATITUDE',
     *       T50,'LONGITUDE',T68,'NORTHING(Y)',T80,'EASTING(X)',
     *       T92,'ZONE',T97,'CONVERGENCE',T110,'SCALE',T121,'ELEV',
     *       T129,'GEOID',/,T68,'METER',
     *       T80,'METER',T97,'D',T100,'M',T104,'S',
     *       T110,'FACTOR',T121,'(M)',T129,'HT(M)',//)
         ENDIF

      RETURN
      END
******************************************************************
       SUBROUTINE HDUTPC
******************************************************************       
       CHARACTER*1 DATUM,DATNUM

       COMMON/DATUM/DATNUM
   
       IF(DATNUM.EQ.'1') THEN
 
      WRITE(4,10)
   10 FORMAT('1',//,T54,'NATIONAL GEODETIC SURVEY',/,'PROGRAM UTMS  ',
     *       T56,'UTMs TO GP PROGRAM',T121,'VERSION 2.0',/,T55,
     *       'CLARKE 1866 ELLIPSOID',//,
     *       'NAME',27X,'NORTH(Y)',9X,'EAST(X)',9X,
     *       'LATITUDE',9X,'LONGITUDE',9X,'ZONE',T106,' CONVERGENCE',
     *        T119,'SCALE FACTOR',/)
       ENDIF

       IF(DATNUM.EQ.'2') THEN
 
      WRITE(4,15)
   15 FORMAT('1',//,T54,'NATIONAL GEODETIC SURVEY',/,'PROGRAM UTMS  ',
     *       T56,'UTMs TO GP PROGRAM',T121,'VERSION 2.0',/,T56,
     *       'GRS80/WGS84 ELLIPSOID',//,
     *       'NAME',27X,'NORTH(Y)',9X,'EAST(X)',9X,
     *       'LATITUDE',9X,'LONGITUDE',9X,'ZONE',T106,' CONVERGENCE',
     *        T119,'SCALE FACTOR',/)
       ENDIF

       IF(DATNUM.EQ.'3') THEN
 
      WRITE(4,25)
   25 FORMAT('1',//,T54,'NATIONAL GEODETIC SURVEY',/,'PROGRAM UTMS  ',
     *       T56,'UTMs TO GP PROGRAM',T121,'VERSION 2.0',/,T53,
     *       'INTERNATIONAL 1910 ELLIPSOID',//,
     *       'NAME',27X,'NORTH(Y)',9X,'EAST(X)',9X,
     *       'LATITUDE',9X,'LONGITUDE',9X,'ZONE',T106,' CONVERGENCE',
     *        T119,'SCALE FACTOR',/)
       ENDIF

       IF(DATNUM.EQ.'4') THEN
 
      WRITE(4,35)
   35 FORMAT('1',//,T54,'NATIONAL GEODETIC SURVEY',/,'PROGRAM UTMS  ',
     *       T56,'UTMs TO GP PROGRAM',T121,'VERSION 2.0',/,T59,
     *       'WGS72 ELLIPSOID',//,
     *       'NAME',27X,'NORTH(Y)',9X,'EAST(X)',9X,
     *       'LATITUDE',9X,'LONGITUDE',9X,'ZONE',T106,' CONVERGENCE',
     *        T119,'SCALE FACTOR',/)
       ENDIF

       IF(DATNUM.EQ.'5') THEN
 
      WRITE(4,45)
   45 FORMAT('1',//,T54,'NATIONAL GEODETIC SURVEY',/,'PROGRAM UTMS  ',
     *       T56,'UTMs TO GP PROGRAM',T121,'VERSION 2.0',/,T59,
     *       'OTHER ELLIPSOID',//,
     *       'NAME',27X,'NORTH(Y)',9X,'EAST(X)',9X,
     *       'LATITUDE',9X,'LONGITUDE',9X,'ZONE',T106,' CONVERGENCE',
     *        T119,'SCALE FACTOR',/)
       ENDIF


      RETURN
      END
*********************************************************************
      SUBROUTINE IUTPC
******************************************************************
      IMPLICIT REAL*8(A-H,O-Z)

      LOGICAL FILFLAG,FILPRT
      CHARACTER*1 EORW
      CHARACTER*1 YN
      CHARACTER*1 DATUM,DATNUM
      CHARACTER*30 NAME,GPPFIL,GPFIL
      CHARACTER*80 CARDR
      REAL*8 NORTH
      COMMON/XY/NORTH,EAST
      COMMON/DATUM/DATNUM
 
      FILFLAG=.FALSE.
      FILPRT=.FALSE.
 
      WRITE(6,10)
   10 FORMAT(' DO YOU WANT THE OUTPUT LISTING SAVED ON A FILE (Y/N)? ')
      READ(5,20) YN
   20 FORMAT(A1)
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
   25   WRITE(6,30)
   30   FORMAT('0FILE NAME:')
        READ(5,40) GPPFIL
   40   FORMAT(A30)
        OPEN(4,STATUS='NEW',FILE=GPPFIL,ERR=900)
        FILPRT=.TRUE.
        CALL HDUTPC
      ENDIF
      GO TO 950
 
 900  WRITE(6,901)
 901  FORMAT('0 FILE ALREADY EXIST, DO YOU WANT TO'/,
     &       '  WRITE OVER IT (Y/N) '/,
     &       '  TYPE ANSWER  '\)
      READ(5,20) YN
 
         IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
           OPEN(4,STATUS='UNKNOWN',FILE=GPPFIL)
           FILPRT=.TRUE.
           CALL HDUTPC
         ELSE
           GO TO 25
         ENDIF
 
 
  950 WRITE(6,951)
  951 FORMAT('0 DO YOU WANT TO SAVE AN *80* RECORD FILE '/,
     &       '  OUTPUT FILE  (Y/N)'/,
     &       '  TYPE ANSWER  '\)
      READ(5,20) YN
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
  959    WRITE(6,960)
  960    FORMAT('0 TYPE FILE NAME: '\)
         READ(5,40) GPFIL
         OPEN(3,STATUS='NEW',FILE=GPFIL,ERR=1000)
         FILFLAG=.TRUE.
          FILPRT=.TRUE.
      ENDIF
 
        GO TO 50
 
 1000  WRITE(6,1001)
 1001  FORMAT('0 FILE ALREADY EXISTS, DO YOU WANT TO '/,
     &        '  WRITE OVER IT (Y/N) '/,
     &        '  TYPE ANSWER  '\)
 
       READ(5,20) YN
 
        IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
          OPEN(3,STATUS='UNKNOWN',FILE=GPFIL)
          FILFLAG=.TRUE.
          FILPRT=.TRUE.
          GO TO 50
        ELSE
          GO TO 959
        ENDIF
 
   50   WRITE(CARDR,45)
   45   FORMAT(T7,'*81*')
 
      IF(FILPRT) THEN
        WRITE(6,52)
   52   FORMAT('0ENTER STATION NAME:')
        READ(5,40) NAME
 
        WRITE(CARDR,41) NAME
   41     FORMAT(T15,A30)
      ELSE
        NAME='                              '
         WRITE(CARDR,41) NAME
      ENDIF
 
      WRITE(6,60)
   60 FORMAT('0ENTER NORTHING  IN METERS:'/,
     *       ' NNNNNNNN.NNN'/)
      READ(5,62) NORTH
   62 FORMAT(F12.3)

      PRINT *, ' IS THE NORTHING IN THE SOUTHERN HEMISPHERE ?'
      PRINT *, ' ANSWER Y/N '
      READ(*,FMT='(A1)') YN

 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
       NORTH = 20000000.D0 - NORTH
      ENDIF

      WRITE(6,64)
   64 FORMAT('0ENTER EASTING  IN METERS:'/,
     *       ' EEEEEEE.EEE'/)
      READ(6,66)EAST
   66 FORMAT(F11.3)

      PRINT *, ' DO YOU WANT THE LONGITUDE COORDINATES WRITTEN '
      PRINT *, ' AS EAST (E) OR WEST (W) '
      PRINT *, ' ANSWER E/W '
      READ(*,FMT='(A1)') EORW

      CARDR(69:69)= EORW
 
      WRITE(6,68)
   68 FORMAT('0ENTER THE UTM ZONE NUMBER ',/)
      READ(5,70)ICODE
   70 FORMAT(I4)
 
      CALL DRUTGP(CARDR,ICODE,FILFLAG,FILPRT)
 
      WRITE(6,80)
   80 FORMAT('0ANY MORE COMPUTATIONS (Y/N)? ')
      READ(5,20) YN
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) GO TO 50
 
      RETURN
      END
*******************************************************************
       SUBROUTINE IUTGP
*******************************************************************
      IMPLICIT REAL*8(A-H,O-Z)
      CHARACTER*1 YN
      CHARACTER*1 EORW
      CHARACTER*1 NORS
      CHARACTER*11 CLAT
      CHARACTER*12 CLON
      CHARACTER*30 NAME,PCFIL,FIL81
      CHARACTER*80 CARDR
      LOGICAL FILFLAG
      LOGICAL FIL81FL
 
      FILFLAG=.FALSE.
      FIL81FL=.FALSE.

 
      WRITE(6,10)
   10 FORMAT(' DO YOU WANT THE OUTPUT LISTING SAVED ON A FILE (Y/N)? ')
      READ(5,20) YN
   20 FORMAT(A1)
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
   25   WRITE(6,30)
   30   FORMAT('0FILE NAME:')
        READ(5,40) PCFIL
   40   FORMAT(A30)
        OPEN(3,STATUS='NEW',FILE=PCFIL,ERR=900)
        FILFLAG=.TRUE.
        CALL HDUTGP
      ENDIF
 
      GO TO 50
 
 900  WRITE(6,901)
 901  FORMAT('  FILE ALREADY EXIST, DO YOU WANT TO '/,
     &       '  WRITE OVER IT (Y/N) '/,
     &       '  TYPE ANSWER  '\)
      READ(5,20) YN
 
           IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
             OPEN(3,STATUS='UNKNOWN',FILE=PCFIL)
             FILFLAG=.TRUE.
             CALL HDUTGP
           ELSE
             GO TO 25
           ENDIF
***
**    FIND THE NAME OF THE *81* RECORD FILE IF WANTED
**
 
  50  WRITE(6,110)
 110  FORMAT(' DO YOU WANT TO SAVED AN *81* RECORD FILE (Y/N)? ')
      READ(5,20) YN

       IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
 125    WRITE(6,130)
 130    FORMAT('0FILE NAME:')
        READ(5,40) FIL81

        OPEN(4,STATUS='NEW',FILE=FIL81,ERR=800)
        FIL81FL=.TRUE.

      ENDIF
 
      GO TO 250
 
 800  WRITE(6,801)
 801  FORMAT('  FILE ALREADY EXIST, DO YOU WANT TO '/,
     &       '  WRITE OVER IT (Y/N) '/,
     &       '  TYPE ANSWER  '\)
      READ(5,20) YN
 
           IF((YN.EQ.'Y').OR.(YN.EQ.'y')) THEN
             OPEN(4,STATUS='UNKNOWN',FILE=FIL81)
             FIL81FL=.TRUE.

           ELSE
             GO TO 125
           ENDIF
 
 
  250 IF(FILFLAG) THEN
        WRITE(6,52)
   52   FORMAT('0ENTER STATION NAME:')
        READ(5,40) NAME
      ELSE
        NAME='                              '
      ENDIF
 
      WRITE(6,60)
   60 FORMAT('0ENTER LATITUDE:'/,
     *       ' DD MM SS.SSSSS'/)
      READ(5,62) LD,LM,SLAT
   62 FORMAT(I2,1X,I2,1X,F8.5)

      PRINT *,'                '
      PRINT *,'  DIRECTION OF LATITUDE  - N OR S '
      PRINT *,'  TYPE N OR S NOW  '
      READ(*,FMT='(A1)') NORS
 
      WRITE(6,64)
   64 FORMAT('0ENTER LONGITUDE:'/,
     *       ' NOTE IF THE DEGREES ARE LESS THAN 100 '/,
     *       ' YOU MUST ENTER A ZERO FIRST '/,
     *       ' 94 DEGREES WILL BE 094 '/,
     *       ' DDD MM SS.SSSSS'/)
      READ(6,66)LOD,LOM,SLON
   66 FORMAT(I3,1X,I2,1X,F8.5)

       PRINT *,'                      '
       PRINT *,'  DIRECTION OF LONGITUDE - E OR W '
       PRINT *,'  TYPE  E OR W NOW  '
       READ(*,FMT='(A1)') EORW
  
      ISEC = SLAT * 1.0D5 + 0.5D0
      JSEC = SLON * 1.0D5 + 0.5D0 

      WRITE(CLAT,67) LD,LM,ISEC
   67 FORMAT(I2.2,I2.2,I7.7)

      WRITE(CLON,68) LOD,LOM,JSEC
   68 FORMAT(I3.3,I2.2,I7.7)
  
      WRITE(CARDR,70) NAME,CLAT,NORS,CLON,EORW
   70 FORMAT(T7,'*80*',T15,A30,T45,A11,A1,T57,A12,A1)

 
      CALL DRGPUT(CARDR,FILFLAG,FIL81FL)
 
      WRITE(6,80)
   80 FORMAT('0ANY MORE COMPUTATIONS (Y/N)? ')
      READ(5,20) YN
 
      IF((YN.EQ.'Y').OR.(YN.EQ.'y')) GO TO 250
 
      RETURN
      END
************************************************************************
       SUBROUTINE DRGPUT(CARDR,FILFLAG,FIL81FL)
**********************************************************************
*
*
*      THIS IS THE DRIVER TO COMPUTE UTM NORTHINGS AND EASTINGS
*      FOR EACH PRIMARY ZONE AND THE AJACENT ZONE IF THE LONGITUDE
*      IS WITH 5 MINUTES OF THE ZONE BOUNDARIES
*
*      THE OUTPUT IS FOR THE DATA SHEET PROGRAM
*
*      VARIABLES
*      CARDR = A MODIFIED 80 RECORD CARD WITH A LENGTH OF 211 COLS
*      ER = EQUATORIAL RADIUS OF THE ELLIPSOID (SEMI-MAJOR AXIS)
*      RF = RECIPROCAL OF FLATTING OF THE ELLIPSOD
*      ESQ= E SQUARED
*      RAD = RADIAN CONVERSION FACTOR
*      CM = CENTRAL MERIDIAN ( COMPUTED USEING THE LONGITUDE)
*      SF = SCALE FACTOR OF CENTRAL MERIDIAN ( ALWAYS .9996 FOR UTM)
*      OR = SOUTHERNMOST PARALLEL OF LATITUDE ( ALWAYS ZERO FOR UTM)
*      R, A, B, C, U, V, W = ELLIPSOID CONSTANTS USED FOR COMPUTING
*                            MERIDIONAL DISTANCE FROM LATITUDE
*      SO = MERIDIONAL DISTANCE (MULTIPLIED BY SCALE FACTOR )
*           FROM THE EQUATOR TO THE SOUTHERNMOST PARALLEL OF LATITUDE
*           ( ALWAYS ZERO FOR UTM)
*

       IMPLICIT REAL*8 (A-H,O-Z)

       CHARACTER*80 CARDR
       CHARACTER*1 EORW
       CHARACTER*1 NORS
       CHARACTER*30 NAME
       CHARACTER*4 ZONE
       INTEGER*4 LD,LM,LOD,LOM,FOUND

       REAL*8 SLAT,SLON,FI,LAM,LCM,UCM
       REAL*8 KP
       REAL*8 GRIDAZ
       REAL*8 TD,TM,ND,NM
       REAL*8 NORTH
       REAL*8 LOD1
 
       LOGICAL FILFLAG
       LOGICAL FIL81FL

       COMMON/CONST/RAD,ER,RF,ESQ,PI

       READ(CARDR,50)LD,LM,SLAT,NORS,LOD,LOM,SLON,EORW
 50    FORMAT(T45,I2,I2,F7.5,A1,I3,I2,F7.5,A1)

  
*
*      CONVERT THE LATITUDE AND LONGITUDE TO PI AND LAM
*
       TD=DBLE(FLOAT(LD))
       TM=DBLE(FLOAT(LM))
       FI=(TD+(TM+SLAT/60.D0)/60.D0)/RAD

       ND=DBLE(FLOAT(LOD))
       NM=DBLE(FLOAT(LOM))

       IF((EORW.EQ.'E').OR.(EORW.EQ.'e')) THEN
         LAM=(360.D0-(ND+(NM+SLON/60.D0)/60.D0))/RAD
         LOD1=(360.D0-(ND+(NM+SLON/60.D0)/60.D0))
         LOD=DINT(LOD1)    
       ENDIF

       IF((EORW.EQ.'W').OR.(EORW.EQ.'w')) THEN
         LAM=(ND+(NM+SLON/60.D0)/60.D0)/RAD
         LOD=LOD
       ENDIF



*
*      FIND THE ZONE FOR LONGITUDE LESS THAN 180 DEGREES
*
        IF(LOD.LT.180) THEN
           IZ=LOD/6
           IZ= 30 -IZ

           ICM=(183-(6*IZ))
           CM=DBLE(FLOAT(ICM))/RAD
           UCM=(ICM+3)/RAD
           LCM=(ICM-3)/RAD
        ENDIF
*
*       FIND THE ZONE FOR LONGITUDE GREATER THAN 180 DEGREES
*
         IF(LOD.GE.180) THEN
            IZ=(LOD)/6
            IZ= 90 - IZ
            ICM=(543 - (6*IZ))
            CM= DBLE(FLOAT(ICM))/RAD
            UCM=(ICM+3)/RAD
            LCM=(ICM-3)/RAD
         ENDIF

   

       TOL=(5.0D0/60.0D0)/RAD
       
       FN = 0.D0

       IF((NORS.EQ.'S').OR.(NORS.EQ.'s')) THEN
          FN = 10000000.D0
       ENDIF

       IF((NORS.EQ.'N').OR.(NORS.EQ.'n')) THEN
          FN = 0.D0
       ENDIF


       FE=500000.0D0
       SF=0.9996D0
       OR=0.0D0

       FOUND=0

       CALL TCONST(ER,RF,SF,OR,ESQ,EPS,R,A,B,C,U,V,W,SO,
     &            CM,FE,FN)

*
*      COMPUTE THE NORTH AND EASTINGS

*

 200   CALL TMGRID(FI,LAM,NORTH,EAST,CONV,KP,ER,ESQ,EPS,CM,
     &            FE,FN,SF,SO,R,A,B,C,U,V,W)


*
*      WRITE THE ZONE NUMBER
*
       IF (IZ.GT.9) THEN
         WRITE(ZONE,600) IZ
 600     FORMAT(1X,I2)
       ELSE
         WRITE(ZONE,605) IZ
 605     FORMAT(1X,I2.2)
       ENDIF

*
*      WRITE THE OUTPUT TO THE PLANE FILE FOR THE DATA SHEET
*      PROGRAM
*
      CALL DATAUT(CARDR,NORTH,EAST,CONV,KP,ZONE,FILFLAG,FOUND,FIL81FL)

*
*      DO THE TEST TO SEE IF THE LONGITUDE IS WITHIN 5 MINUTES
*      OF THE BOUNDARIES FOR THE ZONE AND IF SO COMPUTE THE
*      NORHT AND EASTING FOR THE ADJACENT ZONE
*
       IF(FOUND.NE.0) THEN
          RETURN
       ENDIF

       IF(DABS(UCM-LAM).LE.TOL) THEN
          CM=DBLE(FLOAT(ICM+6))/RAD
          IZ=IZ-1
          IF(IZ.EQ.0) IZ=60
          FOUND=FOUND+1
          GO TO 200
       ENDIF

       IF(DABS(LCM-LAM).LE.TOL) THEN
          CM=DBLE(FLOAT(ICM-6))/RAD
          IZ=IZ+1
          IF(IZ.EQ.61) IZ=1
          FOUND=FOUND+1
          GO TO 200
       ENDIF


       RETURN
       END

*******************************************************************
       SUBROUTINE DRUTGP(CARDR,ICODE,FILFLAG,FILPRT)
*
*
*
*      THIS IS THE DRIVER TO COMPUTE LATITUDES AND LONGITUDES FROM
*      THE UTMs FOR EACH ZONE
*
*
*      VARIABLES
*      CARDR = A MODIFIED 80 RECORD CARD WITH A LENGTH OF 211 COLS
*      ER = EQUATORIAL RADIUS OF THE ELLIPSOID (SEMI-MAJOR AXIS)
*      RF = RECIPROCAL OF FLATTING OF THE ELLIPSOD
*      ESQ= E SQUARED
*      RAD = RADIAN CONVERSION FACTOR
*      CM = CENTRAL MERIDIAN ( COMPUTED USEING THE LONGITUDE)
*      SF = SCALE FACTOR OF CENTRAL MERIDIAN ( ALWAYS .9996 FOR UTM)
*      OR = SOUTHERNMOST PARALLEL OF LATITUDE ( ALWAYS ZERO FOR UTM)
*      R, A, B, C, U, V, W = ELLIPSOID CONSTANTS USED FOR COMPUTING
*                            MERIDIONAL DISTANCE FROM LATITUDE
*      SO = MERIDIONAL DISTANCE (MULTIPLIED BY SCALE FACTOR )
*           FROM THE EQUATOR TO THE SOUTHERNMOST PARALLEL OF LATITUDE
*           ( ALWAYS ZERO FOR UTM)
*

       IMPLICIT REAL*8 (A-H,O-Z)

       CHARACTER*80 CARDR
       CHARACTER*30 NAME
       CHARACTER*1 NORS
       CHARACTER*4 ZONE
       INTEGER*4 FOUND

       REAL*8 LAT,LON,LCM,UCM
       REAL*8 KP
       REAL*8 NORTH,EAST
 
       LOGICAL FILFLAG,FILPRT

       COMMON/CONST/RAD,ER,RF,ESQ,PI
       COMMON/XY/NORTH,EAST
 

*
*      FIND THE CENTRAL MERIDAIN IF THE ZONE NUMBER IS LESS THAN 30
*
        IF(ICODE.LT.30) THEN
           IZ=ICODE
           ICM=(183-(6*IZ))
           CM=DBLE(FLOAT(ICM))/RAD
           UCM=(ICM+3)/RAD
           LCM=(ICM-3)/RAD
        ENDIF
*
*       FIND THE CENTRAL MERIDAN IF THE ZONE NUMBER IS LARGER THAN 30
*
        IF(ICODE.GE.30) THEN
           IZ=ICODE
           ICM=(543 - (6*IZ))
           CM= DBLE(FLOAT(ICM))/RAD
           UCM=(ICM+3)/RAD
           LCM=(ICM-3)/RAD
        ENDIF

       TOL=(5.0D0/60.0D0)/RAD

       IF(NORTH.GT.10000000.0) THEN       
          FN= 10000000.0D0
          NORS= 'S'
       ELSE
          FN=0.D0
          NORS='N'
       ENDIF

       FE=500000.0D0
       SF=0.9996D0
       OR=0.0D0

       FOUND=0

       CALL TCONPC (SF,OR,EPS,R,SO,V0,V2,V4,V6,ER,ESQ,RF)

*
*      COMPUTE THE LATITUDES AND LONGITUDES
*

 200   CALL TMGEOD (NORTH,EAST,LAT,LON,EPS,CM,FE,SF,SO,R,V0,V2,
     &              V4,V6,FN,ER,ESQ,CONV,KP)

*
*      WRITE THE ZONE NUMBER
*
       IF (IZ.GT.9) THEN
         WRITE(ZONE,600) IZ
 600     FORMAT(1X,I2)
       ELSE
         WRITE(ZONE,605) IZ
 605     FORMAT(1X,I2.2)
       ENDIF

*
*      WRITE THE OUTPUT TO THE PLANE FILE FOR THE DATA SHEET
*      PROGRAM
*
      CALL DATAGP(CARDR,LAT,LON,FILFLAG,FOUND,FILPRT,ZONE,CONV,KP,NORS)

*
*      DO THE TEST TO SEE IF THE LONGITUDE IS WITHIN 5 MINUTES
*      OF THE BOUNDARIES FOR THE ZONE AND IF SO COMPUTE THE
*      NORHT AND EASTING FOR THE ADJACENT ZONE
*
       IF(FOUND.NE.0) THEN
          RETURN
       ENDIF

       IF(DABS(UCM-LAM).LE.TOL) THEN
          CM=DBLE(FLOAT(ICM+6))/RAD
          IZ=IZ-1
          IF(IZ.EQ.0) IZ=60
          FOUND=FOUND+1
          GO TO 200
       ENDIF

       IF(DABS(LCM-LAM).LE.TOL) THEN
          CM=DBLE(FLOAT(ICM-6))/RAD
          IZ=IZ+1
          IF(IZ.EQ.61) IZ=1
          FOUND=FOUND+1
          GO TO 200
       ENDIF


       RETURN
       END

**********************************************************************
      SUBROUTINE DATAUT(CARDR,NORTH,EAST,CONV,KP,ZONE,FILFLAG,J,
     &                  FIL81FL)
*********************************************************************
*
      IMPLICIT REAL*8 (A-H,O-Z)
      CHARACTER*5 XGD,GDVAL
      DIMENSION GDVAL(1001),GDNUM(1001)
      LOGICAL FILFLAG
      LOGICAL FIL81FL
      REAL*8 NORTH,KP
      REAL*8 SLAT,SLON
      INTEGER*4 LD,LM,LOD,LOM
      CHARACTER*1 PM,ESGN,GSGN
      CHARACTER*1 ELEVT
      CHARACTER*1 EORW
      CHARACTER*1 NORS
      CHARACTER*1 DATUM,DATNUM
      CHARACTER*2 ORDER
      CHARACTER*2 ELXX
      CHARACTER*6 GD
      CHARACTER*80 CARDR
      INTEGER*4 STANUM
      CHARACTER*4 ELEV
      CHARACTER*11 REST
      CHARACTER*7 ELNUM
      CHARACTER*4 ZONE
      CHARACTER*30 NAME
      CHARACTER*11 ANORTH
      CHARACTER*10 AEAST
      CHARACTER*1  BUFF(20)


      COMMON/FILES/I3,I4,I2,ICON
      COMMON/IPRINT/IPRT
      COMMON/DONUM/ISN
      COMMON/GEODS/GDVAL,GDNUM
      COMMON/DATUM/DATNUM


 
      IF (CONV.LT.0) THEN
        PM='-'
      ELSE
        PM=' '
      ENDIF
 
      CALL TODMS(DABS(CONV),IDEG,IMIN,CSEC)
 
       IF(FILFLAG) THEN 
                  IF(ICON.GE.48) THEN
                     CALL HDUTGP
                     ICON = 0
                  ENDIF
 
       READ(CARDR,50) STANUM,NAME,ELEV,ELXX,ELEVT,ORDER
  50   FORMAT(T11,I3,T15,A30,T70,A4,A2,A1,T79,A2)

       READ(CARDR,51) LD,LM,SLAT,NORS,LOD,LOM,SLON,EORW
  51   FORMAT(T45,I2,I2,F7.5,A1,I3,I2,F7.5,A1)

 
          IF((ORDER(1:1).EQ.'4').AND.(ELEVT.EQ.' ')) THEN
               ELNUM='       '
          ELSEIF((ORDER(1:1).EQ.' ').AND.(ELEVT.EQ.' ')) THEN
               ELNUM='       '
          ELSEIF((ELEVT.EQ.'B').OR.(ELEVT.EQ.'L')) THEN
               WRITE(ELNUM,200) ELEV,ELXX
  200          FORMAT(A4,'.',A2)
          ELSEIF((ELEVT.EQ.'R').OR.(ELEVT.EQ.'T')) THEN
               WRITE(ELNUM,205) ELEV,ELXX(1:1)
  205          FORMAT(A4,'.',A1,' ')
          ELSEIF((ELEVT.EQ.'P').OR.(ELEVT.EQ.'E').OR.
     &           (ELEVT.EQ.'V')) THEN
               WRITE(ELNUM,210) ELEV
  210          FORMAT(A4,'.  ')
          ELSE               
               WRITE(ELNUM,215) ELEV
  215          FORMAT(A4,' SC')
          ENDIF
 
*** DO THE DO LOOP TO FIND THE GEOD HT
            GD='      '
            XGD='     '
 
         DO 60 I=1,ISN
            IF(GDNUM(I).EQ.STANUM) THEN
               XGD=GDVAL(I)
            ENDIF
   60     CONTINUE
 
             IF(XGD.NE.'     ') THEN
                READ(XGD,FMT='(F5.1)') GEOD
                 GSGN=' '
              IF(GEOD.LE.0.0D0) THEN
                 GSGN='-'
              ENDIF
 
                  GEOD=DABS(GEOD)
                  WRITE(GD,FMT='(F6.2)') GEOD
                  GD(1:1)=GSGN
             ENDIF
 
 
        IF(J.EQ.0) THEN
          WRITE(3,10) NAME,LD,LM,SLAT,NORS,LOD,LOM,SLON,EORW,
     &             NORTH,EAST,ZONE,PM,IDEG,IMIN,CSEC,KP,ELNUM,
     &             GD

 10       FORMAT(1X,A30,T34,I2,1X,I2,1X,F8.5,A1,T50,I3.3,1X,I2.2,1X,
     &     F8.5,A1,T67,F12.3,T80,F11.3,T92,A4,T97,A1,I1,1X,I2,1X,F5.2,
     &     T109,F10.8,T120,A7,T128,A6)
                ICON= ICON + 1
        ELSE
          WRITE(3,20) NORTH,EAST,ZONE,PM,IDEG,IMIN,CSEC,KP
  20      FORMAT(T67,F12.3,T80,F11.3,T92,A4,T97,A1,I1,1X,I2,1X,
     &           F5.2,T109,F10.8)
               ICON = ICON + 1
        ENDIF
      ELSE
         IF(DATNUM.EQ.'1') THEN
           PRINT *, ' UTMS FOR THE CLARK 1866 ELLIPSOID (NAD27 DATUM) '
           PRINT *,'             '
         ENDIF

         IF(DATNUM.EQ.'2') THEN
            PRINT *,'  UTMS FOR THE GRS80/WGS84 ELLIP (NAD83 DATUM) '
            PRINT *,'                           '
         ENDIF

         IF(DATNUM.EQ.'3') THEN
            PRINT *,'  UTMS FOR THE INTERNATIONAL ELLIP (INT24 DATUM) '
            PRINT *,'                           '
         ENDIF

         IF(DATNUM.EQ.'4') THEN
            PRINT *,'  UTMS FOR THE WGS72 ELLIPSOID '
            PRINT *,'                           '
         ENDIF

         IF(DATNUM.EQ.'5') THEN
            PRINT *,'  UTMS FOR THE OTHER ELLIPSOID '
            PRINT *,'                           '
         ENDIF


        WRITE(*,30)
   30   FORMAT('   NORTH(Y)      EAST(X)   ZONE   ',
     *         ' CONVERGENCE  SCALE')
        WRITE(*,40)NORTH,EAST,ZONE,PM,IDEG,IMIN,CSEC,KP
   40   FORMAT(1X,F12.3,1X,F11.3,2X,A4,2X,A1,I1,1X,I2,1X,
     *         F5.2,3X,F10.8)
      ENDIF



      IF(FIL81FL) THEN

 
        CALL CHGDEC(11,3,EAST,BUFF)
     
          AEAST(1:1)  = BUFF(1)
          AEAST(2:2)  = BUFF(2)
          AEAST(3:3)  = BUFF(3)
          AEAST(4:4)  = BUFF(4)
          AEAST(5:5)  = BUFF(5)
          AEAST(6:6)  = BUFF(6)
          AEAST(7:7)  = BUFF(7)
          AEAST(8:8)  = BUFF(9)
          AEAST(9:9)  = BUFF(10)
          AEAST(10:10) = BUFF(11)

    
        CALL CHGDEC(12,3,NORTH,BUFF)
    
          ANORTH(1:1)  = BUFF(1)
          ANORTH(2:2)  = BUFF(2)
          ANORTH(3:3)  = BUFF(3)
          ANORTH(4:4)  = BUFF(4)
          ANORTH(5:5)  = BUFF(5)
          ANORTH(6:6)  = BUFF(6)
          ANORTH(7:7)  = BUFF(7)
          ANORTH(8:8)  = BUFF(8)
          ANORTH(9:9)  = BUFF(10)
          ANORTH(10:10) = BUFF(11)
          ANORTH(11:11) = BUFF(12)
          

       READ(CARDR,300) STANUM,NAME,REST
  300  FORMAT(T11,I3,T15,A30,T70,A11)
        IF(DATNUM.EQ.'1') THEN
        WRITE(4,310) STANUM,NAME,AEAST,ANORTH,ZONE(2:3),REST
  310   FORMAT(T7,'*81*',T11,I3,T15,A30,T45,A10,T55,A11,
     &         T66,'00',A2,T70,A11)
        ENDIF

        IF(DATNUM.EQ.'2') THEN
        WRITE(4,320) STANUM,NAME,ANORTH,AEAST,ZONE(2:3),REST
  320   FORMAT(T7,'*81*',T11,I3,T15,A30,T45,A11,T56,A10,
     &         T66,'00',A2,T70,A11) 
        ENDIF

        IF(DATNUM.EQ.'3') THEN
        WRITE(4,310) STANUM,NAME,AEAST,ANORTH,ZONE(2:3),REST
        ENDIF

       IF(DATNUM.EQ.'4') THEN
        WRITE(4,310) STANUM,NAME,AEAST,ANORTH,ZONE(2:3),REST
        ENDIF

       IF(DATNUM.EQ.'4') THEN
        WRITE(4,310) STANUM,NAME,AEAST,ANORTH,ZONE(2:3),REST
        ENDIF

       IF(DATNUM.EQ.'5') THEN
        WRITE(4,310) STANUM,NAME,AEAST,ANORTH,ZONE(2:3),REST
        ENDIF


      ENDIF

 
      RETURN
      END
*********************************************************************
      SUBROUTINE DATAGP(CARDR,LAT,LON,FILFLAG,J,FILPRT,ZONE,CONV,KP,
     &                  NORS)
**********************************************************************
***
      IMPLICIT REAL*8 (A-H,O-Z)
      LOGICAL FILFLAG,FILPRT
      REAL*8 NORTH,EAST,KP,LAT,LON,CONV
      INTEGER*4 IDEG,IMIN
      CHARACTER*2 AD1,AM1,AM2
      CHARACTER*3 AD2
      CHARACTER*1 NORS
      CHARACTER*1 WORE
      CHARACTER*7 AS1,AS2
      CHARACTER*1 ADIR1,ADIR2,PM
      CHARACTER*1 DATUM,DATNUM
      CHARACTER*4 ZONE
      CHARACTER*30 NAME
      CHARACTER*80 CARDR
      COMMON/XY/NORTH,EAST
      COMMON/FILES/I3,I4,I2,ICON
      COMMON/DATUM/DATNUM
      COMMON/CONST/RAD,ER,RF,ESQ,PI

      R360 = 360.D0/RAD

C     WORE = 'W'

      READ(CARDR,FMT='(T69,A1)') WORE

      IF((WORE.EQ.'E').OR.(WORE.EQ.'e')) THEN
        LON = R360 - LON
      ENDIF
 
      CALL TODMS(LAT,LD,LM,SLAT)
      CALL TODMS(LON,LOD,LOM,SLON)
 
      IF (CONV.LT.0) THEN
        PM='-'
      ELSE
        PM=' '
      ENDIF
 
      CALL TODMS(DABS(CONV),IDEG,IMIN,CSEC)
 
 
      IF(FILPRT) THEN
 
                IF(ICON.GE.48) THEN
                   CALL HDUTPC

                   ICON=0
                ENDIF
 
         READ(CARDR,5) NAME
  5      FORMAT(T15,A30)
 
       IF(J.EQ.0) THEN
        WRITE(4,10) NAME,NORTH,EAST,LD,LM,SLAT,NORS,LOD,LOM,SLON,
     &             WORE,ZONE,PM,IDEG,IMIN,CSEC,KP
 10     FORMAT(A30,T31,F12.3,T46,F11.3,T62,I2.2,1X,I2.2,1X,F8.5,T77,A1,
     &    T79,I3.3,1X,I2.2,1X,F8.5,T95,A1,T100,A4,T106,A1,1X,I1,1X,I2.2,
     &       1X,F5.2,T119,F10.8)
       ELSE
          WRITE(4,20) LD,LM,SLAT,NORS,LOD,LOM,SLON,WORE,ZONE,PM,
     &         IDEG,IMIN,CSEC,KP
  20      FORMAT(T62,I2.2,1X,I2.2,1X,F8.5,T77,A1,
     &           T79,I3.3,1X,I2.2,1X,F8.5,T95,A1,T100,A4,T106,A1,1X,I1,
     &           1X,I2.2,1X,F5.2,T119,F10.8)
       ENDIF
      ELSE
           IF(DATNUM.EQ.'1') THEN
             PRINT *, ' UTMS FOR THE NAD27 DATUM'
             PRINT *, '                         '
           ENDIF

           IF(DATNUM.EQ.'2') THEN
             PRINT *, ' UTMS FOR THE NAD83 DATUM'
             PRINT *, '                         '
           ENDIF

        WRITE(6,30)
   30   FORMAT('0  LATITUDE',T20,'LONGITUDE',T37,'ZONE ',
     &         T42,' CONVERGENCE ',T56,'SCALE FACTOR')
        WRITE(6,40)LD,LM,SLAT,NORS,LOD,LOM,SLON,WORE,ZONE,
     &             PM,IDEG,IMIN,CSEC,KP
   40   FORMAT(1X,I2.2,1X,I2.2,1X,F8.5,T17,A1,T19,I3.3,1X,I2.2,1X,F8.5,
     &         T35,A1,T37,A4,T42,A1,1X,I1,1X,I2.2,1X,F5.2,2X,F10.8)
      ENDIF
 
 
        IF(FILFLAG) THEN
 
 
*** UPDATE RECORD
 
      CARDR(7:10)='*80*'
 
      CALL TODMS(LAT,ID1,IM1,S1)

       ADIR1 = NORS

       IF(LON.LT.0) THEN
        WORE='E'
       ENDIF

      CALL TODMS(DABS(LON),ID2,IM2,S2)

       ADIR2 = WORE

      IS1=S1*100000.D0+0.5D0
      IS2=S2*100000.D0+0.5D0
 
      WRITE(AD1,4) ID1
      WRITE(AM1,4) IM1
      WRITE(AS1,2) IS1
      WRITE(AD2,3) ID2
      WRITE(AM2,4) IM2
      WRITE(AS2,2) IS2
 4    FORMAT(I2.2)
 2    FORMAT(I7.7)
 3    FORMAT(I3.3)
 
      CARDR(45:46)=AD1
      CARDR(47:48)=AM1
      CARDR(49:55)=AS1
      CARDR(56:56)=ADIR1
      CARDR(57:59)=AD2
      CARDR(60:61)=AM2
      CARDR(62:68)=AS2
      CARDR(69:69)=ADIR2
 
*** PROCESS THE NEW *80* RECORD
 
      WRITE(3,50) CARDR
 50    FORMAT(A80)
 
        ENDIF
 
      ICON= ICON + 1
      RETURN
      END
*********************************************************************
      SUBROUTINE TODMS(RAD,IDG,MIN,SEC)
*********************************************************************
C     RADIANS TO DEGREES,MINUTES AND SECONDS
C
      REAL*8 RAD,SEC,RHOSEC
      DATA RHOSEC/2.062648062471D05/
      SEC=RAD*RHOSEC
      IDG=SEC/3600.D0
      SEC=SEC-DBLE(IDG*3600)
      MIN=SEC/60.D0
      SEC=SEC-DBLE(MIN*60)
      IF((60.D0-DABS(SEC)).GT.5.D-6) GO TO 100
      SEC=SEC-DSIGN(60.D0,SEC)
      MIN=MIN+ISIGN(1,MIN)
  100 IF(IABS(MIN).LT.60) GO TO 101
      MIN=MIN-ISIGN(60,MIN)
      IDG=IDG+ISIGN(1,IDG)
  101 MIN=IABS(MIN)
      SEC=DABS(SEC)
      IF(RAD.GE.0.D0) GO TO 102
      IF(IDG.EQ.0) MIN=-MIN
      IF(IDG.EQ.0.AND.MIN.EQ.0)SEC=-SEC
  102 RETURN
      END
********************************************************************
*******************************************************************
      SUBROUTINE LSTFTN
***************************************************************** 
 
      CHARACTER*133 TXT
      CHARACTER*80  FOLD
      CHARACTER*1 FF
 
      FF=CHAR(12)
 
      PRINT *,'             '
      PRINT *,'                 '
      PRINT *,'   SUBROUTINE LSTFTN    '
      PRINT *,'                 '
      PRINT *,'   NOTE: MAKE SURE THE PRINTER IS TURNED ON '
      PRINT *,'                  '
 
      OPEN(2,FILE='PRN',FORM='FORMATTED')
 
      PRINT *,'   NAME OF INPUT FILE WRITTEN WITH '
      PRINT *,'   FORTRAN OPTIONS.                '
 
      PRINT *,'                                   '
      PRINT *,'   TYPE NAME: '
 
      READ(5,50) FOLD
 50   FORMAT(A80)
 
      OPEN(3,FILE=FOLD,STATUS='OLD')
 
 
 100  READ(3,150,END=900) TXT
 150  FORMAT(A133)
 
 
          IF(TXT(1:1).EQ.'1') THEN
             WRITE(2,200) FF
  200        FORMAT(A)
          ELSE
             WRITE(2,150) TXT
          ENDIF
             GO TO 100
 
 900  WRITE(2,200) FF
      WRITE(2,200) FF
 
      CLOSE(UNIT=3)
      CLOSE(UNIT=2)
 
      PRINT *,'  JOB COMPLETED   '
      PRINT *,'                  '
      RETURN
      END
**********************************************************************
      SUBROUTINE CHGDEC (NNN,MMM,SS,CHAR)
C     -----------------------------------------------------
      CHARACTER*1  DASH,ZERO,DOL,BLK1,CHAR(*),IB(20),TT
      CHARACTER*20 JB
      INTEGER*4   IDG,MIN
      REAL*8      S,SS,DEC,W,SEC,TEN,TOL
C
      EQUIVALENCE (IB,JB)
C
      DATA BLK1,DOL,ZERO,DASH/' ','$','0','-'/,TEN/10.0D0/
C
C        CHAR      1-16      LENGTH OF CHARACTER ARRAY FIELD
C        NR        1-13      LENGTH OF FIELD TO BE USED FROM LEFT
C        NP        7-13      LOCATION OF DECIMAL POINT FROM RIGHT
C        NEG       CHAR(1)   PUT THE MINUS SIGN HERE
C        EXAMPLE:            RANGE NR=14 AND WITH A POINT NP=6
C                            W=  -3600.376541
C
C CHAR FIELD     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
C BLANK FILLED   B  B  B  B  B  B  B  B  B  B  B  B  B  B  B  B BLANK
C NR ADDED      |B  B  B  B  B  B  B  B  B  B  B  B  B  B| B  B LIMIT
C NP ADDED      |B  B  B  B  B  B  B  .  B  B  B  B  B  B| B  B POINT
C W ENTERED     |B  B  -  3  6  0  0  .  3  7  6  5  4  1| 8  4
C DOL (SIGN)    |B  B  -  3  6  0  0  .  3  7  6  5  4  1| $  4
C NEG (SIGN)    |B  B  -  3  6  0  0  .  3  7  6  5  4  1| $  4
C
C  -------------------------------
C     SETUP OUTSIDE CONSTANTS
C
      NR=NNN
      NP=MMM
      DEC=SS
C
C     CHECK TO SEE IF NR AND NF ARE WITHIN LIMITS
C
      NNP=IABS(NP)
      KTST=0
      M=IABS(NR)
C
C     IF NR IS GREATER THAN ZERO -- DECIMAL NUMBER
C
      IF(NR.GT.0)GOTO 1
C -------------------------------------
C     THIS IS A DEG-MIN-SEC FORMAT
C
      IF(NNP.GT.5)NNP=5
C
C     ENTRY IS DDD-MM-SS.SS
C           OR  HH-MM-SS.SS
C
      DEC=DABS(DEC)
      SEC=DEC*3600.0D0
      IDG=SEC/3600.0D0
      SEC=SEC-DBLE(FLOAT(IDG))*3600.0D0
      MIN=SEC/60.0D0
      SEC=SEC-DBLE(FLOAT(MIN))*60.0D0
      DEC=DBLE(FLOAT(IDG))*1000000.0D0+DBLE(FLOAT(MIN))*1000.0D0+SEC
C
      KTST=1
      M=15
C
C     ROUND THE DECIMAL NUMBER
C
    1 IP=-1*(NNP+1)
      TOL=5.0D0*(TEN**IP)
      W=(DABS(DEC) + TOL)
      IF(DEC.LT.0.0D0) W=-W
      DEC=W
C
      N=NNP
      IF(M.GT.15)M=15
      IF(M.LT.4)M=4
      IF(N.GE.M)N=M-1
      W=DEC
C
C     CONVERT THE DECIMAL NUMBER
C
      WRITE(JB,100) W
  100 FORMAT(F20.10)
C
C     BLANK FILL THE ARRAY
C
      DO 5 IQ=1,16
    5 CHAR(IQ)=BLK1
C
C     LOOK FOR THE FIRST NON-BLANK CHARACTER
C
      DO 6 I=1,10
      TT=IB(I)
    6 IF(TT.NE.BLK1)GOTO 7
C
C     COMPUTE THE PROPER NUMBER LENGTH WITH THE PROPER DECIMALS
C
    7 K=11-I
      K=K+N
      IF(K.GT.M)M=K
      L=10-(M-N)
      IF(L.LE.0)L=0
C
      MM=M+1
      J=0
      IDEC=0
      DO 30 I=1,MM
      K=I+L
      IF(K.GT.20)K=20
      J=J+1
      CHAR(J)=IB(K)
   30 IF(CHAR(J).EQ.'.') IDEC=J
      N=IDEC+NNP+1
C
      IF(KTST.EQ.0)GOTO 40
C
C     FILL-OUT THE DEG-MIN-SEC FIELD
C
      CHAR(4)=DASH
      CHAR(7)=DASH
C
C     ZERO-OUT ALL BLANK CHARACTERS
C
      DO 10 I=1,9
   10 IF(CHAR(I).EQ.BLK1)CHAR(I)=ZERO
C
   40 CHAR(N)=DOL
C
      RETURN
      END
************************************************************
      SUBROUTINE DATUMM(ER,RF,F,ESQ,DATNUM)

      CHARACTER*1 ANS,DATNUM
      REAL*8 ER
      REAL*8 RF
      REAL*8 F
      REAL*8 ESQ



   50 PRINT *, '                                          '
      PRINT *, '   WHICH ELLIPSOID DO YOU WANT ANSWER:    '
      PRINT *, '   1.  CLARKE 1866                       '
      PRINT *, '   2.  GRS80/WGS84                       '
      PRINT *, '   3.  INTERNATIONAL 1910                '
      PRINT *, '   4.  WGS72                             '
      PRINT *, '   5.  OTHER ELLIPSOID                   '
      PRINT *, '   TYPE NUMBER:  '
      READ(5,FMT='(A1)') ANS
**  
**     FIND THE RIGHT SEMI MAJOR AXIS AND FLATTING 
**
         IF(ANS.EQ.'1') THEN
**      FOR THE NAD 27 DATUM
**
                ER=6378206.4D0
                RF=294.978698D0
                F=1.D0/RF
                ESQ=(F+F-F*F)
 
        ELSEIF (ANS.EQ.'2') THEN
**     FOR THE NAD83 DATUM 
**   
                ER=6378137.D0
                RF=298.257222101D0
                F=1.D0/RF
                ESQ=(F+F-F*F)

        ELSEIF (ANS.EQ.'3') THEN
**     FOR THE INT24 DATUM 
**   
                ER=6378388.D0
                RF=297.0D0
                F=1.D0/RF
                ESQ=(F+F-F*F)

        ELSEIF (ANS.EQ.'4') THEN
**     FOR THE WGS72 
**   
                ER=6378135.D0
                RF=298.26D0
                F=1.D0/RF
                ESQ=(F+F-F*F)

        ELSEIF (ANS.EQ.'5') THEN
**     FOR THE OTHER DATUM 
**   
 10             PRINT *,'                    '
                PRINT *,'  SEMIMAJOR AXIS (meters)  '
                PRINT *,'  TYPE VALUE NOW:  '
                READ(*,FMT='(F12.0)') ER

                IF((ER.LE.6376400.D0).OR.(ER.GT.6378500.D0)) THEN
                    PRINT *,'                  '
                    PRINT *,' SEMIMAJOR AXIS IS OUT OF RANGE - DO YOU'
                    PRINT *,' WANT TO TRY AGAIN '
                    PRINT *,' TYPE Y OR N '
                    READ(*,FMT='(A1)') ANS
                     IF((ANS.EQ.'Y').OR.(ANS.EQ.'y')) THEN
                         GO TO 10
                     ELSE
                         GO TO 50
                     ENDIF
                 ENDIF

 20             PRINT *,'                  '
                PRINT *,' FLATTENING            '
                PRINT *,' TYPE VALUE NOW:  '
                READ(*,FMT='(F11.0)') RF

                IF((RF.LE.290.D0).OR.(RF.GT.302.D0)) THEN
                    PRINT *,'                  '
                    PRINT *,' FLATTENING IS OUT OF RANGE - DO YOU '
                    PRINT *,' WANT TO TRY AGAIN '
                    PRINT *,' TYPE Y OR N '
                    READ(*,FMT='(A1)') ANS
                     IF((ANS.EQ.'Y').OR.(ANS.EQ.'y')) THEN
                         GO TO 20
                     ELSE
                         GO TO 50
                     ENDIF
                 ENDIF
                
                F=1.D0/RF
                ESQ=(F+F-F*F)

         ELSE
          PRINT *, ' YOU TYPED THE INCORRECT NUMBER   '
          PRINT *, ' SO LET TRY AGAIN '
          PRINT *, '                  '
                    GO TO 50
         ENDIF

           DATNUM=ANS
         
         RETURN
         END


**********************************************************************/

/*
Purpose...set center for grid calc
	Input...lat/lon of center of area, in decimal degrees
Output...center of grid, in radians, for use by .TauToXY and XYtoTau
*/

/*
void Earth::SetCenter(const double flat, double flon, double *theta0, double *phi0)  {

	*theta0 = flon * TODEG;	//DegRa;	// longitude in radians
	*phi0 =  flat * TODEG;	//DegRa;	// lattitude in radians

	return;
}
*/


/*
	Purpose.. convert lat/lon to XY
	Input.. lat/lon in telcontar units;

	Output.. approximate x/y distance from central meridian and equator, in
	centimeters.

	assumptions...:
	  spherical earth, no elevation, axes origin and orientation same as
		lat/lon

	PreRequisites...
   	must previously set the origin using the function Ttct.setorigin

	Algorithm:
		given, lat/lon = phi/theta in radians, sphere radius=R, then the distance
	    along the (parallels,meridians) from (0,0) to (phi,theta)  = (X,Y) =
   	 Y = R*phi; X = R*cos(phi)*theta.

    	To reduce distortion, the meridian is translated to the center of the
    	local grid =(phi0,theta0)

     	Y = r*(phi-phi0); 
		x = R*cos(phi-phi0)*(theta-theta0)

		If you want to make the coordinates positive over a given range,
    	set the origin's xy coordinate= to 1/2*the range.}

*/

/*
void Earth::TAUtoXY(double lat, double lon, double *x, double *y)  {

	*x = Ra*MtoCM*(lon * TauToRad - theta0) * cos(lat*TauToRad - phi0);
	*y = Ra*MtoCM * (lat * TauToRad - phi0);

	return;
}

*/




/*******************************************************************************
	testing the transverse mercator projection:
	this implementation is from 
	"Ordinance Survey"
	"A Guide To Coordinate Systems In Great Britain" (a .pdf file)

  initialize with:
		1. the datum
		2. phi0, lambda0		lat/lon of true origin
		3. N0, E0				northing / easting of true origin

		- phi, lambda			test lat/lon

  PERFORMS THE TRANSVERSE MERCATOR PROJECTION OF AIRY 1830 DATA

*******************************************************************************/

/*
//void Earth::testTM(void)  {
void Earth::project(double lat, double lon, double *northing, double *easting)  {

	//Earth *earth = new Earth();


	double a, b;
	double e2;			// eccentricity squared
//	double phi;			// latitude
//	double lambda;		// longitude
	double deg, min, sec;
	double n;
	double v;
	double F0;			// scale factor on central meridian
	double rho;
	double eta2;
	double M;
	double phi0 = 0.0;		// latitude of true origin
	double lambda0 = 0.0;	// lon of true origin
	double I, II, III, IIIA, IV, V, VI;
	//double N, E;
	double N0;						// northing of true origin
	double E0;						// easting of true origin

	// this is the Airy 1830 datum:

	a = 6377563.396;				// semi-major axis in meters
	b = 6356256.910;				// semi-minor axis in meters
	e2 = (a*a - b*b) / (a*a);	// .0066705397616, ok
	F0 = .9996012717;

	//---------------------------------------------------------------

	deg = 49.0;						// latitude of true origin
	min = 0.0;
	sec = 0.0;
	phi0 = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg, min, sec);
	phi0 *= (PI/180.0);			// convert latitude to radians .91904799045

	deg = -2.0;						// longitude of true origin
	min = 0.0;
	sec = 0.0;
	lambda0 = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg, min, sec);
	lambda0 *= (PI/180.0);		// convert latitude to radians

	N0 = -100000.0;				// northing of true origin
	E0 = 400000.0;					// easting of true origin

	//---------------------------------------------------------------

//	deg = 52.0;					// our test latitude
//	min = 39.0;
//	sec = 27.2531;

//	FILE *stream = fopen("earth.log", "wt");

//	phi = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg, min, sec);
//	fprintf(stream, "phi = %16.12lf\n", phi);

//	phi *= (PI/180.0);			// convert latitude to radians .91904799045



//	deg = 1.0;					// our test longitude
//	min = 43.0;
//	sec = 4.5177;
//	lambda = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg, min, sec);
//	fprintf(stream, "lambda = %16.12lf\n", lambda);
//	lambda *= (PI/180.0);			// convert latitude to radians


	double phi = lat * (PI/180.0);			// convert latitude to radians .91904799045
	double lambda = lon * (PI/180.0);			// convert latitude to radians

	n = (a-b) / (a+b);

	v = a*F0*pow(1.0-e2*sin(phi)*sin(phi) , -.5);		// 6 388 502.3333			// x^y

	rho = a*F0*(1.0-e2)*pow(1.0-e2*sin(phi)*sin(phi), -1.5);

	eta2 = v / rho - 1.0;

	M = b*F0*(
				(1.0 + n + (5.0/4.0)*n*n + (5.0/4.0)*n*n*n) * (phi - phi0) - 
				(3.0*n + 3.0*n*n + (21.0/8.0)*n*n*n) * sin(phi-phi0) * cos(phi+phi0) +
				((15.0/8.0)*n*n + (15.0/8.0)*n*n*n) * sin(2.0*(phi-phi0)) * cos(2.0*(phi+phi0)) -
				(35.0/24.0)*n*n*n * sin(3.0*(phi-phi0)) * cos(3.0*(phi+phi0))
				);

	I = M + N0;
	II = (v/2.0)*sin(phi)*cos(phi);
	III = (v/24.0)*sin(phi)*cos(phi)*cos(phi)*cos(phi) * (5.0 - tan(phi)*tan(phi) +9.0*eta2);

	IIIA = (v/720.0) * sin(phi) * pow(cos(phi), 5.0) * (61.0 - 58.0 * pow(tan(phi), 2.0) + pow(tan(phi), 4.0));

	IV = v*cos(phi);
	V = (v/6.0) * pow(cos(phi), 3.0) * (v/rho - pow(tan(phi), 2.0));
	VI = (v/120.0) * pow(cos(phi), 5.0) * 
			(
				5.0 - 18.0*pow(tan(phi), 2.0) + 
				pow(tan(phi), 4.0) + 14.0*eta2 - 58.0*(pow(tan(phi), 2.0)*eta2)
		);

	*northing = I + II*pow(lambda-lambda0, 2.0) + III*pow(lambda-lambda0, 4.0) + IIIA*pow(lambda-lambda0, 6.0);
	*easting = E0 + IV*(lambda-lambda0) + V*pow(lambda-lambda0, 3.0) + VI*pow(lambda-lambda0, 5.0);
	// N should be 313177.27034306
	// E should be 651409.90293819

	//fclose(stream);

	return;
}
*/


/*******************************************************************************
	testing the transverse mercator projection:
	this implementation is from "Ordinance Survey"
	"A Guide To Coordinate Systems In Great Britain" (a .pdf file)

  initialize with:
		1. the datum
		2. phi0, lambda0		lat/lon of true origin
		3. N0, E0				northing / easting of true origin

		- phi, lambda			test lat/lon

	PERFORMS THE TRANSVERSE MERCATOR PROJECTION OF ELLIPSOIDAL DATUMS
	------------------------------------------------------------------

  entry:
	lat is latitude in degrees
	lon is longitude in degrees


	another test:
	-------------

Test Data for TM projection on WGS84 Ellipsoid

Assume the centre (False Origin)of your test area is at:

phi0 = 45.0
lambda0 = 0.0;

Latitude N45 Deg and Long 0 Deg with coordinates of East 0.00 and
North 0.00 metres.

Scale Factor at Central Meridian (0 Degrees); is 1.00000

A point of coordinates of E 5,000.000 and North 10,000.000 on the
above TM grid
has a Latitude of N45 deg 05 min 23.87361 Sec and Longitude E0 Deg 03
min 48.64881 sec.

*******************************************************************************/

void Earth::project(double lat, double lon, double *northing, double *easting)  {
	double v;
	double rho;
	double eta2;
	double M;
	double I, II, III, IIIA, IV, V, VI;
	double phi, lambda;					// lat, lon in radians

	phi = lat * (PI/180.0);				// convert latitude to radians .91904799045
	lambda = lon * (PI/180.0);			// convert latitude to radians

	v = major*F0*pow(1.0-eSquared*sin(phi)*sin(phi) , -.5);		// 6 388 502.3333			// x^y

	rho = major*F0*(1.0-eSquared)*pow(1.0-eSquared*sin(phi)*sin(phi), -1.5);

	eta2 = v / rho - 1.0;

	double n = eRatio;		// just to make the following line more readable

	M = minor*F0*(
				(1.0 + n + (5.0/4.0)*n*n + (5.0/4.0)*n*n*n) * (phi - phi0) - 
				(3.0*n + 3.0*n*n + (21.0/8.0)*n*n*n) * sin(phi-phi0) * cos(phi+phi0) +
				((15.0/8.0)*n*n + (15.0/8.0)*n*n*n) * sin(2.0*(phi-phi0)) * cos(2.0*(phi+phi0)) -
				(35.0/24.0)*n*n*n * sin(3.0*(phi-phi0)) * cos(3.0*(phi+phi0))
				);

	I = M + N0;
	II = (v/2.0)*sin(phi)*cos(phi);
	III = (v/24.0)*sin(phi)*cos(phi)*cos(phi)*cos(phi) * (5.0 - tan(phi)*tan(phi) +9.0*eta2);

	IIIA = (v/720.0) * sin(phi) * pow(cos(phi), 5.0) * (61.0 - 58.0 * pow(tan(phi), 2.0) + pow(tan(phi), 4.0));

	IV = v*cos(phi);
	V = (v/6.0) * pow(cos(phi), 3.0) * (v/rho - pow(tan(phi), 2.0));
	VI = (v/120.0) * pow(cos(phi), 5.0) * 
			(
				5.0 - 18.0*pow(tan(phi), 2.0) + 
				pow(tan(phi), 4.0) + 14.0*eta2 - 58.0*(pow(tan(phi), 2.0)*eta2)
		);

	*northing = I + II*pow(lambda-lambda0, 2.0) + III*pow(lambda-lambda0, 4.0) + IIIA*pow(lambda-lambda0, 6.0);
	*easting = E0 + IV*(lambda-lambda0) + V*pow(lambda-lambda0, 3.0) + VI*pow(lambda-lambda0, 5.0);

	return;
}

/***************************************************************************

  set up the transverse mercator projection

	entry:
		lat = latitude of true origin
		lon = longitude of true origin

***************************************************************************/

void Earth::initTM(double lat, double lon, double northing, double easting)  {

	eSquared = (major*major - minor*minor) / (major*major);	// .0066705397616, ok
	//F0 = .9996012717;
	F0 = 1.0;
	eRatio = (major-minor) / (major+minor);

	phi0 = lat * (PI/180.0);			// latitude of true origin in radians
	lambda0 = lon * (PI/180.0);		// longitude of true origin in radians

	N0 = northing;							// northing of true origin
	E0 = easting;							// easting of true origin

	return;
}

/***************************************************************************

***************************************************************************/

void Earth::test(void)  {
	EARTHPOINT a, b;
	double deg, min, sec;
	//double lat1, lon1;		// in decimal degrees
	//double lat2, lon2;		// in decimal degrees
	double meters;
	double faz, baz;
	int i;
	double lat, lon;
	double northing, easting;

#define NPAIRS 2

	// define our test pairs
	double tp[NPAIRS][4] =  {
		   //first lat            first lon             2nd lat                2nd lon
		{ 40.7811472415924070, -73.9707168340682980, 40.7810900211334230, -73.9708083868026730 },
		{ 32.53454, -117.12475, 43.72559, -123.06690 }
	};


	//lat1 =  40.7811472415924070;
	//lon1 = -73.9707168340682980;

	//lat2 =  40.7810900211334230;
	//lon2 = -73.9708083868026730;

	FILE *stream = fopen("earth.txt", "wt");


	for(i=0;i<NPAIRS;i++)  {
		ConvertDecimalDegreesToDegreesMinutesSeconds(tp[i][0], deg, min, sec);
		a.lat.deg = (short)deg;
		a.lat.min = (short)min;
		a.lat.sec = sec;

		ConvertDecimalDegreesToDegreesMinutesSeconds(tp[i][1], deg, min, sec);
		a.lon.deg = (short)deg;
		a.lon.min = (short)min;
		a.lon.sec = sec;

		ConvertDecimalDegreesToDegreesMinutesSeconds(tp[i][2], deg, min, sec);
		b.lat.deg = (short)deg;
		b.lat.min = (short)min;
		b.lat.sec = sec;

		ConvertDecimalDegreesToDegreesMinutesSeconds(tp[i][3], deg, min, sec);
		b.lon.deg = (short)deg;
		b.lon.min = (short)min;
		b.lon.sec = sec;

		meters = getDist(a, b, &faz, &baz);

		fprintf(stream, "\n\n----------------------------------------\n");

		fprintf(stream, "lat1 = %.16f degrees\n", tp[i][0]);
		fprintf(stream, "lon1 = %.16f degrees\n", tp[i][1]);

		fprintf(stream, "\nlat2 = %.16f degrees\n", tp[i][2]);
		fprintf(stream, "lon2 = %.16f degrees\n", tp[i][3]);


		fprintf(stream, "\nlat1 DMS =  %d : %d : %.16f\n", a.lat.deg, a.lat.min, a.lat.sec);
		fprintf(stream, "lon1 DMS =  %d : %d : %.16f\n", a.lon.deg, a.lon.min, a.lon.sec);

		fprintf(stream, "lat2 DMS =  %d : %d : %.16f\n", b.lat.deg, b.lat.min, b.lat.sec);
		fprintf(stream, "lon2 DMS =  %d : %d : %.16f\n", b.lon.deg, b.lon.min, b.lon.sec);

		fprintf(stream, "\ndistance = %.16f\n", meters);
		fprintf(stream, "\nforward angle = %.16f degrees\n", faz);
		fprintf(stream, "\nbackward angle = %.16f degrees\n", baz);

	}

	fclose(stream);

	//---------------------------------------------------------------------
	// A point of coordinates of E 5000 and North 10000 on the
	// above TM grid has a Latitude of N45 deg 05 min 23.87361 Sec and 
	// Longitude E 0 Deg 03 min 48.64881 sec.
	//---------------------------------------------------------------------

	deg = 45.0;
	min = 5.0;
	sec = 23.87361;
	lat = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg,min,sec);

	deg = 0.0;
	min = 3.0;
	sec = 48.64881;
	lon = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg,min,sec);

	initTM(45.0, 0.0, 0.0, 0.0);
	project(lat, lon, &northing, &easting);
	// easting should be 5000.000 meters,		I got 5000.0000224080 meters
	// northing should be 10,000.000 meters, I got 10000.000081528 meters

	return;
}

/***********************************************************************************
	eg,
	degrees = convert_from_survey_heading_to_degrees('s', 21.0, 59.0, 40.0, 'w');

	converts survey type headings (eg, S 21 deg 59' 40" W) to degrees. 0 degrees is
	north, 90 degrees is east.
***********************************************************************************/

double Earth::convert_from_survey_heading_to_degrees(char dir1, int deg, int min, int sec, char dir2)  {
	double degrees=0.0;
	int bp = 0;

	degrees = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees(deg,min,sec);

	if (dir1=='n' || dir1=='N')  {
		if (dir2=='e' || dir2=='E')  {
			bp = 2;
		}
		else if (dir2=='w' || dir2=='W')  {
			degrees = 360.0 - degrees;
		}
		else  {
			return 0.0;
		}
	}
	else if (dir1=='s' || dir1=='S')  {
		if (dir2=='e' || dir2=='E')  {
			degrees = 180.0 - degrees;
		}
		else if (dir2=='w' || dir2=='W')  {
			degrees += 180.0;
		}
		else  {
			return 0.0;
		}
	}
	else  {
		return 0.0;
	}

	return degrees;
}

/**********************************************************************
         lat           lon  **** heading ***          feet       degrees        meters

   35.633257,   -82.536339, s, 21, 59, 40, w,   130.060000,   201.994444,    39.642288
   35.632926,   -82.536503, s, 21, 59, 40, w,    99.870000,   201.994444,    30.440376
   35.632671,   -82.536629, n, 70, 20,  0, w,   389.490000,    70.333333,   118.716552
   35.633031,   -82.535395, n, 18, 26,  0, e,   268.610000,    18.433333,    81.872328
   35.633731,   -82.535109, n, 89, 23,  0, e,   156.680000,    89.383333,    47.756064
   35.633736,   -82.534582, n, 19,  0,  0, e,    63.410000,    19.000000,    19.327368
   35.633901,   -82.534512, s, 71,  2,  0, e,    61.780000,   108.966667,    18.830544
   35.633846,   -82.534316, s, 15, 56,  0, w,   149.200000,   195.933333,    45.476160
   35.633452,   -82.534453, s, 49, 59,  0, e,    77.530000,   130.016667,    23.631144


				-82.536339, 35.633257, 0
				-82.536503, 35.632926, 0
				-82.536629, 35.632671, 0
				-82.535395, 35.633031, 0
				-82.535109, 35.633731, 0
				-82.534582, 35.633736, 0
				-82.534512, 35.633901, 0
				-82.534316, 35.633846, 0
				-82.534453, 35.633452, 0

	spring cove road:
**********************************************************************/

#define whichland 1

#if whichland == 0					// spring cove rd
	#define N 11
#elif whichland==1					// busick
	#define N 23
#endif

void Earth::land(void)  {
	Earth e;
	int bp = 0;

	typedef struct  {
		double slat;				// starting lat
		double slon;				// starting lon
		double elat;				// ending lat
		double elon;				// ending lon
		char dir1;
		int deg;
		int min;
		int sec;
		char dir2;
		double feet;				// the distance of this heading in feet
		double degrees;			// the heading in decimal degrees
		double meters;
	} HEADING;

#if whichland == 0					// spring cove rd
	HEADING h[N] = {
		{ 35.633257, -82.536339,  0.000000,  00.000000, 's', 21, 59, 40, 'w', 130.06, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 's', 21, 59, 40, 'w',  99.87, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 'n', 70, 20, 00, 'w', 389.49, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 'n', 18, 26, 00, 'e', 268.61, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 'n', 89, 23, 00, 'e', 156.68, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 'n', 19, 00, 00, 'e',  63.41, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 's', 71, 02, 00, 'e',  61.78, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 's', 15, 56, 00, 'w', 111.54, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 's', 49, 59, 00, 'e',  77.53, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 's', 70, 50, 00, 'e', 117.50, 0.0, 0.0 },
		{ 00.000000,  00.000000,  0.000000,  00.000000, 's', 21, 59, 40, 'w',  20.10, 0.0, 0.0 }
	};
#else
	HEADING h[N] = {
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'S',  3, 10, 15, 'E',  12.00, 0.0, 0.0 },          // L1
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'S',  3, 10, 15, 'E',  23.08, 0.0, 0.0 },          // L2
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'S',  3, 10, 15, 'E', 220.41, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'S',  3, 10, 15, 'E', 116.50, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'S',  0, 44,  4, 'E', 114.36, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'S',  0, 44,  3, 'E',  15.00, 0.0, 0.0 },          // L3

                // turns up the road here:

		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 84, 27, 30, 'W', 161.62, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 84, 27, 30, 'W',  93.42, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 84, 27, 30, 'W',  25.00, 0.0, 0.0 },          // L4

                // turns down the creek here:

		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 27,  0, 18, 'E',  48.25, 0.0, 0.0 },          // L5
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 18,  6, 21, 'E',  51.91, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N',  5, 56, 10, 'E',  39.94, 0.0, 0.0 },          // L6
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 26, 52,  1, 'E',  49.67, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N',  0, 44, 55, 'E',  54.28, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 24,  1,  0, 'E',  81.78, 0.0, 0.0 },
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 31, 21,  9, 'E',  23.79, 0.0, 0.0 },          // L7
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N',  4,  5, 50, 'E',  20.00, 0.0, 0.0 },          // L8
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 15,  0,  3, 'E',  36.42, 0.0, 0.0 },          // L9
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 61, 48, 58, 'E',  26.76, 0.0, 0.0 },          // L10
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 19, 34, 17, 'E',  26.34, 0.0, 0.0 },          // L11
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 85, 53, 35, 'E',  37.19, 0.0, 0.0 },          // L12
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 60,  6, 35, 'E',  45.48, 0.0, 0.0 },          // L13
		{ 0.000000,  0.000000,  0.000000,  0.000000, 'N', 40, 27, 22, 'E',  39.63, 0.0, 0.0 }           // L14

	};
#endif

	//double deg;
	//double min;
	double lat1, lon1;			// in radians
	double lat2, lon2;			// in radians
	double faz, baz;				// in radians
	double lat2deg, lon2deg;	// in degrees;
	int i;
	double lat, lon;
	const char *cptr;
        FILE *stream;

        // get busic starting coords:
        // note: each .01 sec is about .5 foot
        lat = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( 35.00, 46.00, 07.70 );
        lon = ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( 82.00, 11.00, 10.89 );
#if 1
        stream = fopen("x.x", "wt");
        fprintf(stream, "lat = %.16f\n", lat);
        fprintf(stream, "lon = %.16f\n", lon);
        FCLOSE(stream);
#endif

        //lat = 35.7689527777777769
        //lon = 82.1861694444444453
        h[0].slat = lat;
        h[0].slon = -lon;

        bp = 0;

	for(i=0; i<N; i++)  {
		h[i].degrees = e.convert_from_survey_heading_to_degrees(h[i].dir1, h[i].deg, h[i].min, h[i].sec, h[i].dir2);
		lat1 = e.ConvertDegreesToRadians(h[i].slat);					// convert to radians
		lon1 = e.ConvertDegreesToRadians(h[i].slon);					// convert to radians
		faz = e.ConvertDegreesToRadians(h[i].degrees);
		h[i].meters = FEETTOMETERS * h[i].feet;

		e.direct(lat1, lon1, lat2, lon2, faz, baz, h[i].meters);

		lat2deg = e.ConvertRadiansToDegrees(lat2);		// 35.632645826103719
		lon2deg = e.ConvertRadiansToDegrees(lon2);		// -82.536641382416306
		h[i].elat = lat2deg;
		h[i].elon = lon2deg;

		if (i<N-1)  {
			h[i+1].slat = lat2deg;
			h[i+1].slon = lon2deg;
		}
		else  {
			bp = 1;
		}
	}

	// dump out the structure:

	stream = fopen("land.txt", "wt");

	fprintf(stream, "         lat           lon  **** heading ***          feet       degrees        meters\n\n");

	for(i=0; i<N; i++)  {
		fprintf(stream, "%12.6f, %12.6f, %c, %2d, %2d, %2d, %c, %12.6f, %12.6f, %12.6f\n",
			h[i].slat, 
			h[i].slon, 
			h[i].dir1, 
			h[i].deg, 
			h[i].min, 
			h[i].sec, 
			h[i].dir2, 
			h[i].feet, 
			h[i].degrees, 
			h[i].meters
			);
	}

	fprintf(stream, "\n\n");
	// now for inserting into land.kml

	for(i=0; i<N; i++)  {
		fprintf(stream, "\t\t\t\t%.6f, %.6f, 0\n", h[i].slon, h[i].slat);
	}
	fprintf(stream, "\t\t\t\t%.6f, %.6f, 0\n", h[N-1].elon, h[N-1].elat);

	FCLOSE(stream);

	/*
<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://earth.google.com/kml/2.0">
	<Placemark id="khPlacemark552">
		<description><![CDATA[If the <tessellate> tag has a value of 1, the line will contour to the underlying terrain]]></description>
		<name>Tessellated</name>
		<LookAt>
			<longitude>-89.3818320000000030</longitude>
			<latitude>43.0711099999999970</latitude>
			<range>300.0</range>
			<tilt>60.0</tilt>
			<heading>0.0</heading>
		</LookAt>
		<visibility>1</visibility>
		<open>0</open>
		<Style>
			<LineStyle>
				<color>ff0000ff</color>
				<width>4</width>
			</LineStyle>
			<PolyStyle>
				<color>7f00ff00</color>
			</PolyStyle>
		</Style>
		<LineString id="khLineString553">
			<extrude>0</extrude>
			<tessellate>1</tessellate>
			<altitudeMode>clampToGround</altitudeMode>
			<coordinates>
				-89.3818320000000030, 43.0711099999999970, 0
				-89.3818229999999970, 43.0711119999999990, 0
				-89.3818169999999980, 43.0711119999999990, 0
				....
				-89.7006700000000020, 42.9769459999999980, 0
			</coordinates>
		</LineString>
	</Placemark>
</kml>
	*/

#if whichland == 0					// spring cove rd
	stream = fopen("spring_cove.kml", "wt");
#else
#ifdef WIN32
	stream = fopen("busick.kml", "wt");
#else
	stream = fopen("/home/larry/Desktop/busick.kml", "wt");
#endif

#endif

	cptr = "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<kml xmlns=\"http://earth.google.com/kml/2.0\">\n\
	<Placemark id=\"khPlacemark552\">\n\
		<description><![CDATA[If the <tessellate> tag has a value of 1, the line will contour to the underlying terrain]]></description>\n\
		<name>Tessellated</name>";

	fprintf(stream, "%s\n", cptr);

	lat = h[0].slat;
	lon = h[0].slon;

	fprintf(stream, "\
		<LookAt>\n\
			<longitude>%.16f</longitude>\n\
			<latitude>%.16f</latitude>\n\
			<range>1000.0</range>\n\
			<tilt>0.0</tilt>\n\
			<heading>0.0</heading>\n\
		</LookAt>\n", lon, lat);

		cptr = "\
		<visibility>1</visibility>\n\
		<open>0</open>\n\
		<Style>\n\
			<LineStyle>\n\
				<color>ff0000ff</color>\n\
				<width>4</width>\n\
			</LineStyle>\n\
			<PolyStyle>\n\
				<color>7f00ff00</color>\n\
			</PolyStyle>\n\
		</Style>\n\
		<LineString id=\"khLineString553\">\n\
			<extrude>0</extrude>\n\
			<tessellate>1</tessellate>\n\
			<altitudeMode>clampToGround</altitudeMode>\n\
			<coordinates>";

	fprintf(stream, "%s\n", cptr);

	for(i=0; i<N; i++)  {
		//				-82.536339, 35.633257, 0
		//fprintf(stream, "\t\t\t\t%.16f, %.16f, 0\n", h[i].slat, h[i].slon);
		fprintf(stream, "\t\t\t\t%.16f,%.16f,0\n", h[i].slon, h[i].slat);
                bp = i;
	}
	fprintf(stream, "\t\t\t\t%.16f,%.16f,0\n", h[N-1].elon, h[N-1].elat);

	cptr = "\
			</coordinates>\n\
		</LineString>\n\
	</Placemark>\n\
</kml>\n";
	fprintf(stream, "%s\n", cptr);

	FCLOSE(stream);

	bp = 1;
	return;
}

