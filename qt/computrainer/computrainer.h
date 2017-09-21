#ifndef COMPUTRAINER_H
#define COMPUTRAINER_H

#include <vector>

#include <QObject>

#define DOUDP

//#ifdef DOUDP
	#include <QUdpSocket>
//#else
	//#include <QTcpSocket>
//#endif

#include <QAbstractSocket>
#include <QTimer>

#include <comdefs.h>

#include <tmr.h>
#include <rider2.h>
#include <coursefile.h>         // for PerfPoint

#include <qt_ss.h>
#include <qt_data.h>

namespace RACERMATE  {

const int RXQLEN = 2048;
const int RXBUFLEN = 1024;

/**********************************************************************************************

**********************************************************************************************/

class Computrainer : public QObject  {
	Q_OBJECT

public:
	static const int NBARS = 24;

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

#define MODE_WIND               0x2c
#define MODE_WIND_PAUSE         0x28
#define MODE_RFTEST             0x1c
#define MODE_ERGO               0x14
#define MODE_ERGO_PAUSE         0x10

	explicit Computrainer(const char *_ipstr, int _tcp_port, Rider2 &, bool _doudp, QObject *parent = 0);
	explicit Computrainer(int _udp_port, Rider2 &, QObject *parent = 0);
	~Computrainer();

	// setters:
	void setStarted(bool value);
	void setFinished(bool value);
	void setPaused(bool value);
	qt_SS::BARINFO *get_barinfo(int i);
	void start(void);
	void stop(void);
	void setRider(const Rider2& _rider);

private:
	bool doudp;

#ifdef _DEBUG
	qt_SS::SSD *ssdata;
	void display(void);
	char displaystring[256];
#endif
	QT_DATA data;
	//void doConnect(void);
	void init_vars(void);
	int read_data(void);
	int updateHardware(bool _force = false);
	int decode(void);
	void beep(void);
	void reset(void);
	float unpack(unsigned short p);


	float bars[NBARS];
	unsigned char ssraw[NBARS];
	qt_SS *ss;
	double aerodrag;
	float tiredrag;
	qint64 lastdisplaytime;

	QTimer *timer;
	qint64 lastWriteTime;
	unsigned char tx_select;
	bool registered;
	float last_export_grade;
	unsigned char is_signed;
	float manwts;
	unsigned char txbuf[6];
	unsigned char control_byte;
	unsigned char pkt_mask[6];
	unsigned long bytes_out;

	QUdpSocket *udpsocket;
	QHostAddress server_host_addr;
//	QTcpSocket *tcpsocket;
	int port;
	char ipstr[128];
	QAbstractSocket::SocketState socket_state;
	qint64 gstart;                   // class-wide start time for debugging
	qint64 lastCommTime;
	int rxinptr;
	int rxoutptr;
	unsigned char rxq[RXQLEN];
	unsigned char rxbuf[RXBUFLEN];
	int tick;
	bool connected_to_trainer;
	qint64 lastlinkuptime;
	Tmr *at;

	unsigned char rawpacket[6];

	std::vector<float> winds;
	std::vector<float> calories;
	Rider2 rider;
	unsigned char packet[16 + 1];
	int packetIndex;
	bool finishEdge;
	bool started;
	bool finished;
	bool paused;
	LoggingType logging_type;
	FILE *outstream;
	RAW_COMPUTRAINER_LOGPACKET lp;
	unsigned long recordsOut;
	PerfPoint pp;
	bool hrbeep_enabled;
	float igradex10;
	float grade;                          // sent float grade, controlled by course or manually


	unsigned char accum_keys;
	float accum_watts;
	float accum_rpm;
	float accum_hr;
	float accum_kph;
	int accum_tdc;

	float wind;
	float draft_wind;
	int ilbs;                   // rd.kgs converted to in lbs for transmitter
	unsigned long ptime;
	qint64 lastbeeptime;

#define XORVAL 0xff
#define RPM_VALID 0x08


	unsigned char HB_ERGO_RUN;
	unsigned char HB_ERGO_PAUSE;

	unsigned char newmask[6];
	int bp;
	unsigned char keys;                    // the 6 hb keys + the polar heartrate bit

	int accum_watts_count;                 // used for ergvid
	int accum_rpm_count;                   // used for ergvid
	int accum_hr_count;                    // used for ergvid
	int accum_kph_count;                   // used for ergvid

	int parity_errors;                     // keys byte parity bit
	unsigned char lastkeys;
	unsigned char keydown;
	unsigned char keyup;
	int sscount;
	double mps;
	float rawspeed;
	unsigned short rpmflags;
	bool hrvalid;
	float pedalrpm;
	unsigned char rpmValid;
	unsigned short minhr;
	unsigned short maxhr;
	unsigned short rfdrag;
	unsigned short rfmeas;
	unsigned short hbstatus;
	unsigned short version;
	unsigned short slip;
	bool slipflag;
	float raw_rpm;
	DWORD packets;

signals:
	void connected_to_server_signal(bool);
	void connected_to_trainer_signal(int, bool);
	void spinscan_data_signal(int, qt_SS::SSD *);
	void data_signal(int, QT_DATA *);
	int rescale_signal(int, int);


private slots:
	void connected_slot();
	void disconnected_slot();
	void bytesWritten_slot(qint64 bytes);
	void readyRead_slot();
	void state_change_slot(QAbstractSocket::SocketState _socket_state );
	void timer_slot();
	void error_slot(QAbstractSocket::SocketError socketEerror);

public slots:
	void gradechanged_slot(int);
	void windchanged_slot(float);


};

}


#endif // COMPUTRAINER_H
