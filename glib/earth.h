
#ifndef _EARTH_H_
#define _EARTH_H_


#ifndef PI
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164
#endif

#define MAX_VAL 4
//#define MAXLONG 2147483647.
#define MAXLONGTLM 2147483647
#define DBLLONG 4.61168601e18



typedef struct  {
	short deg;
	short min;
	double sec;
} ANGLE;


typedef struct  {
	ANGLE lat;
	ANGLE lon;
} EARTHPOINT;


typedef struct  {
	double lat;
	double lon;
	double elev;
} DP;							// decimal degree point

#define SQUARE(x)       x * x   /* x**2 */

/************************************************************************************************
	aug 12: moved from topo3.0 to .../lib/gen to use also for the video project
************************************************************************************************/

class Earth  {
	private:

		double F0;					// scale factor on central meridian
		double eRatio;				// was 'n'
		double phi0;				// latitude of true origin
		double lambda0;			// lon of true origin
		double eSquared;			// eccentricity squared, was 'e2'
		double N0;					// northing of true origin
		double E0;					// easting of true origin

		double F;
		double TORAD;
		void inver1(double lat1, double lon1, double lat2, double lon2, double *faz, double *baz, double *S);
		double getrad(ANGLE angle, int isign);
		void todms(double _val, ANGLE *angle);
		long idint(double d);
		long idnint(double d);
		double pi;
		double e;		// eccentricity squared

		//------------------------
		double r_major;			// major axis 				
		double r_minor;			// minor axis 				
		double scale_factor;		// scale factor				
		double lon_center;		// Center longitude (projection center) 
		double lat_origin;		// center latitude			
		double e0,e1,e2,e3;		// eccentricity constants		
		double /*e,*/ es, esp;	// eccentricity constants		
		double ml0;					// small value m			
		double false_northing;	// y offset in meters			
		double false_easting;	// x offset in meters	
		double ind;					// spherical flag			
		double adjust_lon(double x);
		void sincos(double val, double *sin_val, double *cos_val);
		double mlfn(double e0, double e1,double e2,double e3, double phi);
		int sign(double x);
		double TWO_PI;

		double e0fn(double x);
		double e1fn(double x);
		double e2fn(double x);
		double e3fn(double x);


		//----------------------------------

	public:
		Earth(void);
		~Earth();
		void land(void);
		void direct(double lat1, double lon1, double &lat2, double &lon2, double faz,double &baz, double s);
		double getDist(EARTHPOINT _a, EARTHPOINT _b, double *faz, double *baz);
		double getDist2(EARTHPOINT _a, EARTHPOINT _b, double *faz, double *baz);
		double Pi(void);
		double ConvertDegreesMinutesSecondsCoordinateToDecimalDegrees( double degrees, double minutes, double seconds );
		double ConvertDegreesToRadians(double degrees);
		double ConvertRadiansToDegrees( const double& radians );
		void ConvertDecimalDegreesToDegreesMinutesSeconds( double decimal_degrees, double& degrees, double& minutes, double& seconds );
		double convert_from_survey_heading_to_degrees(char dir1, int deg, int min, int sec, char dir2);

		double GetSurfaceDistance( DP point_1, DP point_2, double * heading_from_point_1_to_point_2_p, double * heading_from_point_2_to_point_1_p );
		void test(void);
		int tmfor(double lat, double lon, double *x, double *y);
		void ecef(double lat, double lon, double elev, double *x, double *y, double *z);
		int tmforint(
			//double r_maj,				// major axis
			//double r_min,				// minor axis
			double scale_fact,		// scale factor
			double center_lat,		// center latitude		
			double center_lon,		// center longitude		
			double false_east,		// x offset in meters	
			double false_north		// y offset in meters	
		);

		// temporary public
		double major;							// semi-major axis radius
		double minor;							// semi-minor axis radius

		//void testTM(void);
		void project(double lat, double lon, double *northing, double *easting);
		//void project2(double lat, double lon, double *northing, double *easting);
		void initTM(double lat, double lon, double northing, double easting);

};

#endif


