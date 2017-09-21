

#ifndef _RMP_H_
#define _RMP_H_

#ifdef WIN32

#else
    #include <limits.h>
#endif


#include <string>

#include <glib_defines.h>
#include <flags.h>
#include <perfdata.h>
#include <public.h>
#include <tdefs.h>


/********************************************************************************************************

 *******************************************************************************************************/

class RMP  {
	friend class RTD;

    public:
        //#define VERSION 1020
		const static int VERSION = 1020;
        //#define COPYRIGHT "(c) 2012, RacerMate, Inc."
		static const char *COPYRIGHT;						// = "(c) 2012, RacerMate, Inc.";
#ifdef _DEBUG
		static long data_start_finalize;
		static long data_start_load;
		static long data_end_finalize;
		static long data_end_load;
#endif
        #ifdef WIN32
            #pragma pack(push, 1)
        #else
            #pragma pack(push, 1)
        #endif

        typedef struct  {
            INT32 tag;                             //   4 bytes "RM1X"
            INT16 version;                         //   2 bytes - version of this format / 1000
            char date[24];                         //  24 bytes date created
            char creator_exe[32];                  //  32 bytes program that created
            char copyright[32];                    //  32 bytes RacerMate copyright
            char comment[32];                      //  32 bytes description of this file
            INT16 compress_type;                   //   2 bytes different compression type
        } RMHeader;											// 128 bytes

        typedef struct  {
                    INT32 tag;
                    char RiderName[80];
                    INT16 Age;
                    INT16 Height;
                    INT16 Weight;
                    BYTE Gender;
                    BYTE HeartRate;
                    BYTE Upper_HeartRate;
                    BYTE Lower_HeartRate;
                    char CourseName[80];
                    INT16 CourseType;
                    double Distance;
                    float RFDrag;
                    float RFMeas;
                    float Watts_Factor;
                    float FTP;									// functional threshold power
                    INT32 PerfCount;	    // Number of performance points.
                    UINT64 TimeMS;
                    INT64 CourseOffset;
            } PerfInfo;

        typedef struct  {
            UINT32 ms;
            UINT16 GroupFlags;

            float Distance;					// group1
            UINT16 Watts;
            float Watts_Load;

            float Speed;					// group2
            BYTE Cadence;

            INT16 Grade;					// group3

            float Wind;						// group4

            BYTE HeartRate;					// group5

            UINT16 Calories;				// group6

            //BYTE PulsePower;				// group7
            //BYTE DragFactor;

            BYTE PulsePower;				// group7x
            BYTE DragFactor;
            INT16 RawCalibrationValue;

            INT16 FrontGear;				// group8
            INT16 RearGear;
            INT16 GearInches;

            float TSS;						// group9
            float IF;
            float NP;

            BYTE SS;						// group10
            BYTE SSLeftATA;
            BYTE SSRightATA;
            BYTE SSRight;
            BYTE SSLeft;
            BYTE SSLeftSplit;
            BYTE SSRightSplit;
            BYTE bars[24];
            UINT16 m_flags1;				// group11
            UINT16 m_flags2;				// group12
		} RMPDATA;

        typedef struct  {               	// these always change
            UINT32 ms;
            UINT16 GroupFlags;
        } GROUP0;

        typedef struct  {                   // group 1 changes often
            float Distance;
            UINT16 Watts;
            float Watts_Load;
        } GROUP1;


        typedef struct  {
            float Speed;
            BYTE Cadence;
        } GROUP2;

        typedef struct  {
            INT16 Grade;
        } GROUP3;

        typedef struct  {
            float Wind;
        } GROUP4;

        typedef struct  {
            BYTE HeartRate;
        } GROUP5;

        typedef struct  {
            UINT16 Calories;
        } GROUP6;

        typedef struct  {
            BYTE PulsePower;
            BYTE DragFactor;
        } GROUP7;

        typedef struct  {
            BYTE PulsePower;
            BYTE DragFactor;
            INT16 RawCalibrationValue;
        } GROUP7X;

        typedef struct  {
            INT16 FrontGear;
            INT16 RearGear;
            INT16 GearInches;
        } GROUP8;

        typedef struct  {
            float TSS;
            float IF;
            float NP;
        } GROUP9;

        typedef struct  {
            BYTE SS;
            BYTE SSLeftATA;
            BYTE SSRightATA;
            BYTE SSRight;
            BYTE SSLeft;
            BYTE SSLeftSplit;
            BYTE SSRightSplit;
            BYTE bars[24];
        } GROUP10;

        typedef struct  {
            UINT16 m_flags1;
        } GROUP11;

        typedef struct  {
            UINT16 m_flags2;
        } GROUP12;


        struct CourseHeader  {
                    INT32 Tag;
                    char Name[80];
                    char FileName[80];
                    char Description[128];
                    double StartAt;
                    double EndAt;
                    float Length;
                    int Laps;
                    int Count;
                    BYTE Type;
                    BYTE Attributes;					//Mirror; Reverse; Looped; Modified; OutAndBack;
                    BYTE XUnits;
                    BYTE YUnits;
                    BYTE OriginalHash[32];
                    BYTE CourseHash[32];
                    BYTE HeaderHash[32];
        };					// CourseHeader


	/*
	typedef struct  {
		INT32 tag;
		char name[80];
		char filename[80];
		char desc[128];
		double StartAt;
		double EndAt;
		float Length;
		int Laps;
		int Count;
		byte Type;
		byte Attributes; //Mirror; Reverse; Looped; Modified; OutAndBack;
		byte XUnits;
		byte YUnits;
		byte OriginalHash[32];
		byte CourseHash[32];
		byte HeaderHash[32];
	} RMP_HEADER;
	*/

	/*
        public struct PerfData
        {
            // 
            // always set for data 
            public UInt64 StatFlags;
            public UInt64 TimeMS;
            public Int32 KeyFrame; // not set for info
            // data that may not be set
            public double TimeAcc;
            public double Distance;
            public Int32 Lap;
            public float LapTime;
            public float Lead;
            public float Grade;
            public float Wind;
            public float Speed;
            public float Speed_Avg;
            public float Speed_Max;
            public float Watts;
            public float Watts_Avg;
            public float Watts_Max;
            public float Watts_Wkg;
            public float Watts_Load;
            public float HeartRate;
            public float HeartRate_Avg;
            public float HeartRate_Max;
            public float Cadence;
            public float Cadence_Avg;
            public float Cadence_Max;
            public float Calories;
            public float PulsePower;
            public float DragFactor;
            public float SS;
            public float SSLeft;
            public float SSRight;
            public float SSLeftSplit;
            public float SSRightSplit;
            public float SSLeftATA;
            public float SSRightATA;
            public float SSLeft_Avg;
            public float SSRight_Avg;
            public float LeftPower;
            public float RightPower;
            public float PercentAT;
            public Int32 FrontGear;
            public Int32 RearGear;
            public Int32 GearInches;
            public float RawSpinScan;
            public float CadenceTiming;
            public float TSS;
            public float IF;
            public float NP;
            public Int32 Bars_Shown; // bool
            public float[] Bars; // 24
            public float[] AverageBars; // 24

            public Int32 CourseScreenX;
            public string RiderName;
            public string Course;
            //
            public float SS_Stats; // = SS | SSLeft | SSRight | SSLeftSplit | SSRightSplit | SSLeftATA | SSRightATA | SSLeft_Avg | SSRight_Avg,
            public float Gear; // = FrontGear | RearGear,
            public float TSS_IF_NP; // = TSS | IF | NP,

            // added for header version - v1.02
            public bool Drafting;
            public bool Disconnected;
            public Int16 RawCalibrationValue;
            //public bool HardwareStatus;
        };
	*/

        // **** WARNING: changing the values of enum variables here will break Performance File format. ****
        // **** Please only add new ones. Do NOT exchange or reuse values. ****
        #define EMETH 2

        #if EMETH==1
                const __int64 	Zero			= 0LL;
                const __int64 	Time			= (1L << 0);		// Total time of the race.
                const __int64 	LapTime			= (1L << 1);		// Current running total of the lap
                const __int64 	Lap				= (1L << 2);
                const __int64 	Distance		= (1L << 3);		// Total Distance run.
                const __int64 	Lead			= (1L << 4);		// Race order has changed
                const __int64 	Grade			= (1L << 5);
                const __int64 	Wind			= (1L << 6);
                const __int64 	Speed			= (1L << 7);
                const __int64 	Speed_Avg		= (1L << 8);
                const __int64 	Speed_Max		= (1L << 9);
                const __int64 	Watts			= (1L << 10);
                const __int64 	Watts_Avg		= (1L << 11);
                const __int64 	Watts_Max		= (1L << 12);
                const __int64 	Watts_Wkg		= (1L << 13);
                const __int64 	Watts_Load		= (1L << 14);
                const __int64 	HeartRate		= (1L << 15);
                const __int64 	HeartRate_Avg	= (1L << 16);
                const __int64 	HeartRate_Max	= (1L << 17);
                const __int64 	Cadence			= (1L << 18);
                const __int64 	Cadence_Avg		= (1L << 19);
                const __int64 	Cadence_Max		= (1L << 20);
                const __int64 	Calories		= (1L << 21);
                const __int64 	PulsePower		= (1L << 22);
                const __int64 	DragFactor		= (1L << 23);
                const __int64 	SS				= (1L << 24);
                const __int64 	SSLeft			= (1L << 25);
                const __int64 	SSRight			= (1L << 26);
                const __int64 	SSLeftSplit		= (1L << 27);
                const __int64 	SSRightSplit	= (1L << 28);
                const __int64 	SSLeftATA		= (1L << 29);
                const __int64 	SSRightATA		= (1L << 30);
                const __int64 	SSLeft_Avg		= (1L << 31);

                const __int64 SSRight_Avg			= 0x0000000100000000;			// (1L << 32),
                const __int64 SS_Avg				= SSLeft_Avg | SSRight_Avg;
                const __int64 SS_Stats				= SS | SSLeft | SSRight | SSLeftSplit | SSRightSplit | SSLeftATA | SSRightATA | SSLeft_Avg | SSRight_Avg;
                const __int64 HardwareStatusFlags	= 0x0000000200000000;			// (1L << 33);		// Realtime not stored.
                const __int64 LapDistance			= 0x0000000400000000;			// (1L << 34);		// Used for the course display.
                const __int64 PercentAT				= 0x0000000800000000;			// (1L << 35);
                const __int64 FrontGear				= 0x0000001000000000;			// (1L << 36);
                const __int64 RearGear				= 0x0000002000000000;			// (1L << 37);
                const __int64 Gearing				= FrontGear | RearGear;
                const __int64 GearInches			= 0x0000004000000000;			// (1L << 38);
                const __int64 VelotronOnly			= FrontGear | RearGear | GearInches;
                const __int64 Drafting				= 0x0000008000000000;			// (1L << 39);
                const __int64 CadenceTiming			= 0x0000010000000000;			// (1L << 40);
                const __int64 TSS					= 0x0000020000000000;			// (1L << 41);
                const __int64 IF					= 0x0000040000000000;			// (1L << 42);
                const __int64 NP					= 0x0000080000000000;			// (1L << 43);
                const __int64 TSS_IF_NP				= TSS | IF | NP;
                const __int64 Bars					= 0x0000100000000000;			// (1L << 44);
                const __int64 Bars_Shown			= 0x0000200000000000;			// (1L << 45);
                const __int64 Bars_Avg				= 0x0000400000000000;			// (1L << 46);
                const __int64 CourseScreenX			= 0x0000800000000000;			// (1L << 47);
                const __int64 RiderName				= 0x0001000000000000;			// (1L << 48);

                const __int64 Course				= 0x0002000000000000;			// (1L << 49);
                const __int64 Order					= 0x0004000000000000;			//  (1L << 50); // Global flag only
                const __int64 HardwareStatus		= 0x0008000000000000;			// (1L << 51); // Something changed in the hardware connected to that unit.
                const __int64 Finished				= 0x0010000000000000;			// (1L << 52); // Rider finished the race
                const __int64 TrackDistance			= 0x0020000000000000;			// (1L << 53);
                const __int64 Disconnected			= 0x0040000000000000;			// (1L << 54); // True if the unit has been disconnected.
                const __int64 Calibration			= 0x0080000000000000;			// (1L << 55); // Need to read this from the trainer.
                const __int64 HeartRateAlarm		= 0x0100000000000000;			// (1L << 56);
                const __int64 Max					= 0x0200000000000000;			// (1L << 57);
                const __int64 Mask					= Max - 1;

                //const __int64 StatFlags = 0LL;
        #elif EMETH==2
                enum CourseType  {
                        CT_Zero = 0,
                        CT_Distance = 0x01,
                        CT_Watts = 0x02,
                        CT_Video = 0x04,
                        CT_ThreeD = 0x08,
                        CT_GPS = 0x10,
                        CT_Performance = 0x20
                };





        #else               // EMETH

        #endif              // EMETH



        struct Node  {						// course node
			UINT32 count;
			double startx;
			double endx;
			double starty;
			double endy;
		};

        #ifdef WIN32
            #pragma pack(pop)
        #else
            #pragma pack(pop)
        #endif

        //------------------------------------------------
	// N O T  P A C K E D
        //------------------------------------------------

	struct GPSData  {                   // not packed
		UINT frame;						// real frame number
		int real;						// 1 if real data, 0 if interpolated data
		int seconds;
		double lat;						// degrees
		double lon;						// degrees
		double unfiltered_elev;			// raw garmin elevation
		double filtered_elev;			// filtered garmin elevation
		double manelev;					// manually entered elevation

		double accum_meters1;		// from the garmin.txt file
		double accum_meters2;		// computed by me

		double section_meters1;		// garmin's version
		double section_meters2;		// computed by me

		double pg;						// percent grade
		double mps1;					// based on garmin distances
		double mph1;					// based on garmin distances
		double mps2;					// based on computed distances
		double mph2;					// based on computed distances
		double faz;						// forward azimuth
		double seconds_offset;
		double x;						// in meters, used to create the csv file
		double y;						// in meters, used to create the csv file
		double z;						// in meters, used to create the csv file same as manelev but in meters
	};

	struct CourseDataRCV  {                           // not packed
		double Length;
		GPSData gd;
	};

        typedef struct  {
		INT32 Tag;
		UINT16 Mode;
		INT16 RawCalibrationValue;
		INT16 DragFactor;
		UINT16 DeviceType;
		UINT16 DeviceVersion;
		UINT16 PowerAeT;
		UINT16 PowerFTP;
		BYTE HrAeT;
		BYTE HrMin;
		BYTE HrMax;
		BYTE HrZone1;
		BYTE HrZone2;
		BYTE HrZone3;
		BYTE HrZone4;
		BYTE HrZone5;
	} PerfInfoExt1;

        enum ERRORS  {
            OK = 0,
            NIL_DATA,                                    // 1
            NO_DATA_YET,                                 // 2

            WRONG_TAG = INT_MIN,			// 0x80000000
            BAD_COUNT,          			// 0x80000001
            BAD_VERSION,
            EOF_NOT_FOUND,
            FILE_EXISTS,
            FILE_OPEN_ERROR,
            BAD_WRITE_COUNT,
			FILE_NOT_OPEN,

            GENERIC_ERROR
         };

#ifdef _DEBUG
        RMP(const char *_logfilename=NULL);
#else
        RMP(void);
#endif
        ~RMP();
		void compute_date(char date[24]);
		int finalize(void);
		//inline void set_drag_factor(BYTE _df)  { drag_factor = _df; }
        //bool get_initialized(void)  { return initialized; }
        //int read(const char *_fname);
        int load(const char *_fname, FILE *_dbgstream=NULL);
		//int init_output(const char *_fname);
        int save(const char *_fname);
        const char *geterrstr(void)  { return errstr; }
        void set_exe(const char *_exe)  { strncpy(exe, _exe, sizeof(exe));  }
        int get_next_record(bool _freerun=true);
        //int write_record(METADATA *_meta);
        int write_record(RMPDATA *_data);
		int reset_input(void);
		int reset_output(void);
		int dump_perf_data(FILE *stream);
		float inline get_kph(void) { return group2.Speed; }
		float inline get_mph(void) { return (float)TOMPH*group2.Speed; }
		float inline get_watts(void) { return group1.Watts; }
		float inline get_rpm(void) { return group2.Cadence; }
		float inline get_hr(void) { return group5.HeartRate; }
		inline TrainerData GetTrainerData(void) { return td; }
		float inline get_km(void)  { return group1.Distance; }
		float inline get_miles(void)  { return (float)TOMILES*group1.Distance; }
		float inline get_grade(void)  { return (float) group3.Grade/100.0f; }
		float inline get_average_kph(void) { return avg_kph; }
		float inline get_max_kph(void) { return max_kph; }
		float inline get_average_mph(void) { return (float)TOMPH*avg_kph; }
		float inline get_max_mph(void) { return (float)TOMPH*max_kph; }
		float inline get_average_rpm(void) { return avg_rpm; }
		float inline get_max_rpm(void) { return max_rpm; }
		float inline get_max_watts(void) { return (float)max_watts; }
		float inline get_average_watts(void) { return avg_watts; }
		float inline get_max_hr(void) { return (float)max_hr; }
		float inline get_average_hr(void) { return avg_hr; }
#ifdef _DEBUG
		void dump(const char *_fname, const char *_mode="wt");
        void dump_header(FILE *stream, RMHeader _rmheader);
        void dump_perfinfo(FILE *stream, PerfInfo pi);
        void dump_pix1(FILE *stream, PerfInfoExt1 pix1);
        void dump_ch(FILE *stream, CourseHeader ch);
#endif
		const char *compute_course_hash(bool _ascii_hex);
		const char *compute_header_hash(bool _ascii_hex);
		const char *get_embedded_course_name(void)  { return ch.Name; }
		static const char *get_filename_course_name(const char *_fname);
		static char vidname_from_file[64];

		private:
			std::vector<Node> nodes;
			char result[128];
			const char *basename;
			long course_header_start;
			long course_data_start;
			int course_section_count;
			static unsigned char hash[48];		// for computing hashes, (binary copy)
			static char asciihexhash[48];		// for computing hashes, (ascii hex copy)
			// group1: distance, watts, load
			//BYTE drag_factor;
			//METADATA lastmeta;
			RMPDATA lastdata;
			float avg_watts;
			UINT16 max_watts;
			float total_watts;
			long group1_count;

			// group2: speed and rpm

			float avg_kph;
			float max_kph;
			float total_kph;
			float avg_rpm;
			BYTE max_rpm;
			float total_rpm;
			long group2_count;


			// group5: heartrate

			float avg_hr;
			BYTE max_hr;
			float total_hr;
			long group5_count;

			TrainerData td;
            UINT32 next_ms;
            long data_start;
            long data_end;
			DWORD now;
			unsigned long records_in;
			unsigned long records_out;
		    long offs;
			static char gstr[256];
#ifdef _DEBUG
			bool oktobreak;
			int resets;
			Logger *logg;
			int writecount;
#endif

            char exe[32];
            DWORD load_dur;						// time that it takes to parse the file in ms
			DWORD perf_dur;						// time duration of the performance file
            float hdrVersion;
            BYTE XUnits;
            BYTE YUnits;
            BYTE m_OriginalHash[32];
            BYTE m_CourseHash[32];
            BYTE m_HeaderHash[32];
            char errstr[256];
            int bp;
            const char *fname;						// set in load() (while reading files)
            RMHeader rmheader;
            PerfInfo info;
            PerfInfoExt1 infoext;
            PerfData *pd;
            CourseHeader ch;
            CourseDataRCV data;
            CourseDataRCV last_rcv_data;
            DWORD load_start_time;
            DWORD data_start_time;
			DWORD start_time;
			bool first;

            //RMP_HEADER hdr;
            FILE *instream;
            FILE *outstream;
			char outfilename[MAX_PATH];

            INT32 tag;
            INT32 get_next_tag(FILE *instream);
            float version;
            //bool initialized;
            int perfcount;
            GROUP0 group0;           	// these always change
            GROUP1 group1;                  // group 1 changes often
            GROUP2 group2;
            GROUP3 group3;
            GROUP4 group4;
            GROUP5 group5;
            GROUP6 group6;
            GROUP7 group7;
            GROUP7X group7x;
            GROUP8 group8;
            GROUP9 group9;
            GROUP10 group10;
            GROUP11 group11;
            GROUP12 group12;

            UINT16 changedflags;
            UINT16 GroupFlags;


};              // class RMP  {

#endif

