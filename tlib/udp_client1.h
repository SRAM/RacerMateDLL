
#ifndef _X_UDPCLIENT2_H_X_
#define _X_UDPCLIENT2_H_X_

#include <stdio.h>
#include <string>

#include <glib_defines.h>
#include <averagetimer.h>

#include "rider2.h"
//#include "ss.h"
#include "coursefile.h"
#include "data.h"
#include "device.h"
#include "stat.h"
#include "formula.h"

class UDPClient  {
	private:
		enum {
			STAT_MPH,
			STAT_HR,
			STAT_WATTS,
			STAT_RPM,
			NSTATS
		};

		#define UDPRXQLEN 2048
		STAT stats[NSTATS];

		unsigned short version;
		unsigned short devnum;
		unsigned short pktnum;
		unsigned char keys;
		unsigned char keypress;
		float f_mph;
		unsigned short watts;
		unsigned char hrflags;
		unsigned char hrate;
		unsigned char rpm;
		float f_rdrag;
		int rd_measured;
		int stat;                  // 32 bits of status3 .. 0

		//spinscan stuff
		unsigned short prevturns;  // crank turn count, storage for increase
		unsigned short prevtime;   // time of last turn, 10ms steps
		int turns;                 // increase in turn count
		int deltat;                // time difference, 10 ms steps
		float f_peak;              // load at peak bar==200, lbs * 256
		unsigned char bars[24];    // bars, 0 - 200
		float fbars[24];				// bars, 0 - 200
		//void calc_ss(void);
		
	public:

		enum LoggingType {
			NO_LOGGING,
			RAW_PERF_LOGGING,
			THREE_D_PERF_LOGGING,
			RM1_PERF_LOGGING
		};

		/*
		#define MODE_WIND               0x2c
		#define MODE_WIND_PAUSE         0x28
		#define MODE_RFTEST             0x1c
		#define MODE_ERGO               0x14
		#define MODE_ERGO_PAUSE         0x10
		*/

	public:
		UDPClient(int _id);
		~UDPClient();
		unsigned long outpackets;
		unsigned long bytes_out;
		unsigned long ipaddr;
		int id;                          // 0 - 15
		int comport;                     // 201-216
		char ipstr[128];
		SS *ss;                          // todo: make private
		void readDatagram(int _socket, struct sockaddr_in *_peeraddr, unsigned char *_buf, int _buflen);
		bool is_connected(void) { return connected;  }
		int rx(unsigned char *_buf, int _buflen);
		DEVICE get_device(void);
		int reset(void);
		float get_ftp(void);
		void set_ftp(float _ftp);

		#ifdef DO_EXPORT
		void set_export(const char *);
		#endif

		void set_file_mode(bool _mode);
		void set_paused(bool _paused);
		int set_hr_bounds(int _minhr, int _maxhr, bool _beepEnabled);
		int reset_stats(void);
		bool is_paused(void);
		float get_watts_factor(void);
		int set_slope(float _bike_kgs, float _person_kgs, int _drag_factor, float _grade);
		TrainerData GetTrainerData(int _fw);
		int get_handlebar_buttons(void);
		float *get_bars(void);
		float *get_average_bars(void);
		SSDATA get_ss_data(void);										// rm1 compatible ss data
		int set_ergo_mode(int _fw, unsigned short _rrc, float _manwatts);
		int start_cal(void);
		int end_cal(void);

	private:
		int lastctl;
		struct sockaddr_in *peeraddr;
		unsigned long last_connected_time;
		char portname[32];
		bool filemode;
		bool BeepEnabled;
		TrainerData td;
		char xprtname[256];
		FILE *export_stream;
		Formula *formula;

		int SETVAL(int min, int max, int num);


		struct CTCONTROL {
			unsigned short hbdevnum;		// target hb devnum, must match this hb
			unsigned char ctl;				// control byte
			short grade;						// signed grade percent * 10
			char wind;							// signed wind mph
			unsigned short weight;			// unsigned lbs
			unsigned short manwatts;		// unsigned watts
			unsigned short minhr;			// minimum hr limit, 40 to 254, 0 disables it
			unsigned short maxhr;			// max hr limit, min+1 to 255, 0 disables
			unsigned short adrag;			// aero drag, lbs@30mph * 256
			unsigned short tdrag;			// tire drag, lbs * 256
		};
		struct CTCONTROL ctc;


		//int	control;
		float	f_grade;
		float	f_wind;
		float	f_weight;
		int	minhr;
		int	maxhr;
		int	hrenable;
		float	f_aerodrag;
		float	f_tdrag;
		float	f_manwatts;


		int send_ctlmsg(int msgtype);
		int xseqnum;					// xmit pkt sequence #
		int gen_devnum( unsigned char * buf, int ix);
		int gen_hbctl( unsigned char * buf, int ix);
		unsigned short pcdevnum;

		struct FORMAT1 {
			unsigned char byte_count;        // includes checksum
			unsigned short devnum;
			unsigned short packet_count;
			unsigned char sectype;           // 0 means no more sections in this packet
			unsigned char secsize;           // in bytes, 0?
			unsigned char csum;
		};

		FORMAT1 x;

		// used for both directions

		enum  {
			SECT_DEVNUM = 1,
			SECT_FAST,
			SECT_SLOW,
			SECT_SS,
			SECT_IDLE,
			SECT_HBCTL,
			SECT_EMPTY,
			SECT_RANDOM=64,
			SECT_HERE=255
		};

		AverageTimer *at;

		int get_devnum(int len);
		int decode_ss(int ix);
		void  decode_slow(int ix);
		void  decode_fast(int ix);

		int server_socket;
		short peerport;

		#define XORVAL 0xff
		#define RPM_VALID 0x08

		int msgcnt;
		int msgversion;
		int hbdevnum;
		int hbversion;


		bool connected;									// if there is a hb client at least sending I'm here packets, or actual data
		bool finished;
		bool finishEdge;
		bool hrbeep_enabled;
		bool hrvalid;
		bool paused;
		bool registered;
		bool slipflag;
		bool started;
		//DATA data;

		#ifdef TESTING
		TESTDATA testdata;
		#endif

		double aerodrag;
		double mps;
		DWORD lastidletime;
		DWORD packets;
		FILE *outstream;
		float accum_hr;
		float accum_kph;
		float accum_rpm;
		float accum_watts;

		float draft_wind;
		float grade;                             // sent float grade, controlled by course or manually
		float igradex10;
		float last_export_grade;
		float manwts;
		float pedalrpm;
		float raw_rpm;
		float rawspeed;
		float tiredrag;
		float wind;
		int accum_hr_count;              // used for ergvid
		int accum_kph_count;             // used for ergvid
		int accum_rpm_count;             // used for ergvid
		int accum_tdc;
		int accum_watts_count;           // used for ergvid
		int bp;
		int ilbs;                        // rd.kgs converted to in lbs for transmitter
		int packetIndex;
		int parity_errors;               // keys byte parity bit
		int rxinptr;
		int rxoutptr;
		int serialport;                                          // associated serial port for this client
		int sscount;
		int tick;
		#ifdef TESTING
		int testing_tick;
		#endif
		int txinptr;
		int txoutptr;
		LoggingType logging_type;
		PerfPoint pp;
		char str[256];

		unsigned long lastbeeptime;
		unsigned long lastCommTime;
		unsigned long lastWriteTime;
		Rider2 rider;
		std::vector<float> calories;
		std::vector<float> winds;
		unsigned char accum_keys;
		//unsigned char control_byte;
		unsigned char HB_ERGO_PAUSE;
		unsigned char HB_ERGO_RUN;
		unsigned char idle_packet[7];
		unsigned char is_signed;
		unsigned char keydown;
		unsigned char keyup;
		unsigned char lastkeys;

		unsigned char newmask[6];
		unsigned char packet[16 + 1];
		unsigned char pkt_mask[6];
		unsigned char rawpacket[6];

		unsigned char rpmValid;
		unsigned char rxq[RXQLEN];
		unsigned char txbuf[6];                         // used in updateHardware()
		unsigned char tx_select;
		unsigned long bytes_in;
		unsigned long incomplete;
		unsigned long inpackets;
		unsigned long ptime;
		unsigned long recordsOut;
		unsigned short hbstatus;
		unsigned short rfdrag;
		unsigned short rfmeas;
		unsigned short rpmflags;
		unsigned short slip;
		int datagrams;

		void beep(void);
		int init(void);
		int updateHardware(void);

		SSDATA ssdata;											// rm1 compatible structure

		//SS::SSD ssd;

		/*
		struct ssdata {
			unsigned short turns;         // crank turn count
			unsigned short time;          // time of last turn, 10ms steps
			unsigned short peak;          // load at peak bar lbs * 256
			unsigned char bars[24];       // bars, 0 - 255
		};
		*/
		//struct ssdata hbss;


		#define MSGLEN 256
		unsigned char datagram[MSGLEN];    // extracted complete udp packet
		int mstate;                   // pkt finder state machine
		int minptr;                   //
		int bad_msg;                  // bad pkt counter
		int csum;
		int msglen;
		int datalen;

		int get_ubermsg(void);

		METADATA meta;
		DEVICE d;

		unsigned long run_event;
		static void CALLBACK timer_slot(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

};                            // class

#endif                        // #ifndef ...


