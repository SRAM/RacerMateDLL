
#ifndef _RTD_H_
#define _RTD_H_

#ifdef WIN32
	#include <windows.h>
#endif

#include <string.h>
#include <vector>

#include <tdefs.h>
#include <serial.h>

#ifdef DO_RTD_SERVER
	#include <rtdserver.h>
#else
	#include <server.h>
#endif

#include <client.h>
#include <datasource.h>

#include <bike.h>

#include <coursefile.h>

//#define DO_RMP

#ifdef DO_RMP
	#include <rmp.h>
#endif


class RTD : public dataSource  {
	private:
		void create_perf_file(void);
		int add_course(void);
		unsigned long make_section_code(const char *str);
		//RMP::RMHeader rmheader;

	public:
		//void finalize_3dp(void);
		inline unsigned long get_packet_error1(void)  { return packet_error1; }
		inline unsigned long get_packet_error2(void)  { return packet_error2; }
		virtual const char *get_perf_file_name(void);
		void set_circumference(float _circumference);
		//void set_cadence_sensor_params(float _circumference);
		//void set_use_hr_sensor(bool _b);


	protected:
		char url[256];
		int tcp_port;
		int ix;

		//ANT scant;
		//ANT hrant;
		//ANT cant;												// cadence ant sensor

#ifdef DO_RMP
		RMP *rmp;													// for writing .rmp files
		RMP::RMPDATA rmpdata;
#endif

		std::vector<SEC> secs;
		bool caller_created_port;
		//bool demo;
		//unsigned long bad_parity_count;
		//unsigned long packet_error1;
		//unsigned long packet_error2;

		PERF_FILE_HEADER perf_file_header;
		void write_encrypted_section(unsigned char *buf);
		RIDER_INFO rinf;


		PerformanceInfo pinf;
		PerfPoint pp;
		long perflenoffset;
		long perfoffset;
		long pinfoffset;
		void write_long_section(const char *str, long i);
		void write_float_section(const char *str, float f);
		void write_floats_section(const char *str, std::vector <float> &vec);
		void write_string_section(const char *secstr, const char *str);
		std::vector <float> winds;
		std::vector <float> calories;
		GEARINFO tmpgi;
		std::vector<GEARINFO> gi;
		void write_gear_section(void);

		virtual void makelogpacket(void) { return; }

		bool show_errors;
		bool finalized;
		void destruct(void);
		//char dataFileName[256];
		void resetOutputFile(void);
		long dataStart;
		virtual void reset(void);
		bool saved;
		virtual char * save(bool _showDialog=true)  { return NULL; }
		DWORD lastCommTime;
		int CLEN;
		unsigned char *workbuf;
		int packetIndex;
		DWORD lastHWTime;
		FILE *outstream;						// our stream to save real time data
		//void close_outstream(void);
		char outfilename[256];					// the name of our file, eg, "1.tmp"
		int comportnum;
		CRF *sout;								// encryptor for saving data files


	public:

#ifndef NEWUSER
		RTD(
			  //const char *_comportstr, 
			  //const char *_baudstr, 
				int _port,
				int _baud,
			  User *_user, 
			  Course *_course, 
			  Bike *_bike,
			  const char *_dataFileName=NULL,
			  bool _show_errors = false
		  	);
		
		RTD(
				Serial *_port, 
				User *_user, 
				Course *_course, 
				Bike *_bike,
				bool _show_errors = false
		  	);
#else
		RTD(
             const char *_port,
              int _baud,
			  RIDER_DATA &_rd,
			  Course *_course,
			  Bike *_bike,
			  //const char *_dataFileName,						// = NULL
			  bool _show_errors,								// = false
			  LoggingType _logging_type,
			  const char *_url,
			  int _tcp_port
		  	);

		RTD(
				Serial *_port,
        		RIDER_DATA &_rd,
				Course *_course,
				bool _show_errors,						// = false
				Bike *_bike,
				//bool _demo
				LoggingType _logging_type
		  	);

		RTD(
				int _ix,
#ifdef DO_RTD_SERVER
				RTDServer *_server,
#else
				Server *_server,
#endif
        		RIDER_DATA &_rd,
				Course *_course,
				bool _show_errors,						// = false
				Bike *_bike,
				//bool _demo
				LoggingType _logging_type
		  	);

	#if 0
		RTD(
				Client *_client,
        		RIDER_DATA &_rd,
				Course *_course,
				bool _show_errors,						// = false
				Bike *_bike,
				//bool _demo
				LoggingType _logging_type
		  	);
	#endif		// #if 0
#endif


		//RTD(int _portnum);

		virtual ~RTD(void);
		Serial *port;							// should call this pport, port_copy or something else
#ifdef DO_RTD_SERVER
		RTDServer *server;
#else
		//Server *server;
#endif

		//Client *client;
		//const char * get_outfilename(void)  { return outfilename; }

		inline virtual bool is_realtime(void)  { return true; }
		unsigned long recordsOut;
		virtual void setShutDownDelay(unsigned long _ms);

		//char *getDataFileName(void)  {
		//	return dataFileName;
		//}
		//void setDataFileName(char *_fname)  {
		//	strncpy(dataFileName, _fname, sizeof(dataFileName)-1);
		//	return;
		//}
		virtual void pause(void) {
			return;
		}
};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _RTD_H_

//lint -restore

