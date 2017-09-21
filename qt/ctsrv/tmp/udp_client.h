

#ifndef _X_UDPCLIENT2_H_X_
#define _X_UDPCLIENT2_H_X_

#include <stdio.h>
#include <QtCore>
#include <QUdpSocket>

//#include <comdefs.h>

#include <qt_ss.h>

#include <rider2.h>
#include <coursefile.h>

#include <qt_data.h>

#include <metadata.h>
#include <rmdefs.h>
#include <rmconst.h>

#include <tmr.h>

#ifdef TESTING
	#include <testdata.h>
#endif



namespace MYQT  {


	struct NHBDATA {
			unsigned short version;

			unsigned short devnum;
			unsigned short pktnum;
			unsigned char keys;
			unsigned char keypress;
			float f_mph;
			unsigned short watts;
			unsigned char hrflags;
			unsigned char hrate;
			unsigned char rpmflags;
			unsigned char rpm;
			unsigned char minhr;
			unsigned char maxhr;
			float f_rdrag;
			int rd_measured;
			unsigned short slip;
			int stat;                  // 32 bits of status3 .. 0

			//spinscan stuff
			unsigned short prevturns;  // crank turn count, storage for increase
			unsigned short prevtime;   // time of last turn, 10ms steps
			int turns;                 // increase in turn count
			int deltat;                // time difference, 10 ms steps
			float f_peak;              // load at peak bar==200, lbs * 256
			unsigned char bars[24];    // bars, 0 - 200
	};

	class UDPClient : public QObject  {
			Q_OBJECT

		private:
#ifndef RXQLEN
		const static int RXQLEN = 2048;
		//#define RXQLEN 2048
#endif

		public:

			#ifndef NBARS
			static const int NBARS = 24;
			#endif

			enum LoggingType {
				NO_LOGGING,
				RAW_PERF_LOGGING,
				THREE_D_PERF_LOGGING,
				RM1_PERF_LOGGING
			};

			typedef struct {
					unsigned long time;
					unsigned char buf[6];
			} RAW_COMPUTRAINER_LOGPACKET;


		public:
			explicit UDPClient(int _id, QObject *parent = 0);
			~UDPClient();
			void process(void);

			FILE *rxstream;
			FILE *txstream;
			unsigned long outpackets;
			unsigned long bytes_out;
			unsigned long ipaddr;
			int id;                          // 0 - 15
			int comport;                     // 201-216
			char ipstr[128];
			MYQT::qt_SS *ss;                          // todo: make private
			void readDatagram(QUdpSocket *);

			// rm1 compatibility+++

			// move definitions to common area +++
			unsigned char lastctl;
			METADATA meta;
			//DEVICE d;
			// move definitions to common area ---

			float get_watts_factor(void);
			int end_cal(void);
			float *get_average_bars(void);
			float *get_bars(void);
			float get_ftp(void);
			int get_handlebar_buttons(void);
			bool is_connected(void) { return connected;  }
			bool is_paused(void);
			int reset_stats(void);
			int reset(void);
			int rx(unsigned char *_buf, int _buflen);
			int set_ergo_mode(int _fw, unsigned short _rrc, float _manwatts);
			void set_export(const char *);
			void set_file_mode(bool _mode);
			void set_ftp(float _ftp);
			int set_hr_bounds(int _minhr, int _maxhr, bool _beepEnabled);
			void set_paused(bool _paused);
			int set_slope(float _bike_kgs, float _person_kgs, int _drag_factor, float _grade);
			int start_cal(void);

			// rm1 compatibility---



		private:
			// rm1 compatibility+++
			enum {
				STAT_MPH,
				STAT_HR,
				STAT_WATTS,
				STAT_RPM,
				NSTATS
			};

			//#define UDPRXQLEN 2048
			//STAT stats[NSTATS];


			float f_manwatts;
			unsigned short tdrag;
			char xprtname[256];
			FILE *export_stream;
			//Formula *formula;
			bool filemode;
			bool BeepEnabled;
			float f_grade;
			float f_weight;
			float f_aerodrag;

			// rm1 compatibility---

#if 1															// new control packets
			int SETVAL(int min, int max, int num);

			struct NHBCTL {
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
			struct NHBCTL nhbc;



			//struct	PCDATA {
			typedef struct  {
					int	hbdevnum;
					int	control;
					float	f_grade;
					float	f_wind;
					float	f_weight;
					int	minhr;
					int	maxhr;
					int	hrenable;
					float	f_aerodrag;
					float	f_tdrag;
					float	f_manwatts;
			} PCDATA;

			//struct PCDATA pcd = {
			PCDATA pcd;
			/*
		12345,
		0,
		0.0,
		0.0,
		180.0,
		40,
		190,
		0,
		8.0,
		180.0*0.006,
		345
	};
	 */

			int send_ctlmsg(int msgtype);
			int xseqnum;					// xmit pkt sequence #
			int gen_devnum( unsigned char * buf, int ix);
			int gen_hbctl( unsigned char * buf, int ix);
			unsigned short pcdevnum;

#endif


			struct FORMAT1 {
					unsigned char byte_count;        // includes checksum
					unsigned short devnum;
					unsigned short packet_count;
					unsigned char sectype;           // 0 means no more sections in this packet
					unsigned char secsize;           // in bytes, 0?
					unsigned char csum;
			};

			FORMAT1 x;

			/*
	enum {
		SECT_DEVNUM = 1,
		SECT_FAST,
		SECT_SLOW,
		SECT_SS,
		SECT_EMPTY,
		SECT_RANDOM=64,
		SECT_HERE=255
	};
	*/


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


			struct NHBDATA nhbd;

			MYQT::Tmr *at;

			int get_devnum(int len);
			int decode_ss(int ix);
			void  decode_slow(int ix, struct NHBDATA* d);
			void  decode_fast(int ix, struct NHBDATA* d);

			QUdpSocket *server_socket;
			QHostAddress peeraddr;
			quint16 peerport;

#define XORVAL 0xff
#define RPM_VALID 0x08

			int msgcnt;
			int msgversion;
			int hbdevnum;
			int hbversion;


			bool connected;
			bool finished;
			bool finishEdge;
			bool hrbeep_enabled;
			bool hrvalid;
			bool paused;
			bool registered;
			bool slipflag;
			bool started;
			QT_DATA data;
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
			QAbstractSocket::SocketState socket_state;
			qint64 lastbeeptime;
			qint64 lastCommTime;
			qint64 lastWriteTime;
			//QTimer *timer;
			RAW_COMPUTRAINER_LOGPACKET lp;
			Rider2 rider;
			std::vector<float> calories;
			std::vector<float> winds;
			unsigned char accum_keys;
			unsigned char control_byte;
			unsigned char HB_ERGO_PAUSE;
			unsigned char HB_ERGO_RUN;
			unsigned char idle_packet[7];
			unsigned char is_signed;
			unsigned char keydown;
			unsigned char keys;                             // the 6 hb keys + the polar heartrate bit
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
			unsigned short maxhr;
			unsigned short minhr;
			unsigned short rfdrag;
			unsigned short rfmeas;
			unsigned short rpmflags;
			unsigned short slip;
			unsigned short version;
			int datagrams;

			void beep(void);
			int init(void);
			//int updateHardware(bool = false);
			int updateHardware(void);

#ifdef _DEBUG
			void hexdump(unsigned char *buf, int len, QString pre = "", QString post = "");
#endif
			struct hbdata {
					float f_mph;
					int watts;
					int hrate;
					int hrflags;
					int rpm;
					int rpmflags;
					int minhr;
					int maxhr;
					float f_rdrag;       // removed float rdrag
					int rd_measured;
					int slip;
					int stat;
					int version;
					int keys;
					int ss_sync;
					int devnum;
			};
			struct ssdata {
					unsigned short turns;         // crank turn count
					unsigned short time;          // time of last turn, 10ms steps
					unsigned short peak;          // load at peak bar lbs * 256
					unsigned char bars[24];       // bars, 0 - 255
			};
			struct hbdata hbd;
			struct ssdata hbss;
			//struct SS::SSD ssd;
			qt_SS::SSD ssd;
#define MSGLEN 256
			unsigned char datagram[MSGLEN];    // extracted complete udp packet
			int mstate;                   // pkt finder state machine
			int minptr;                   //
			int bad_msg;                  // bad pkt counter
			int csum;
			int msglen;
			int datalen;

			int get_ubermsg(void);

		private slots:
			//void timer_slot();
			void gradechanged_slot(int _igradex10);
			void windchanged_slot(float);

		signals:
			void connected_to_trainer_signal(int, bool);             // works
			void data_signal(int, QT_DATA *);
			int rescale_signal(int, int);
			void ss_signal(int, qt_SS::SSD *);									// emit this every 100 ms
#ifdef TESTING
			void testing_signal(int, TESTDATA *);
#endif

	};                            // class CLIENT
}											// namespace


#endif                        // #ifndef _X_CLIENT_H_X_


