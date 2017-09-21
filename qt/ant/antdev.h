#ifndef _ANTDEV_H_
#define _ANTDEV_H_

#include <stdio.h>
#include <string.h>
#include <vector>

#include <QTimer>
#include <QThread>
#include <QElapsedTimer>
//#include <QList>

#ifdef WIN32
	#pragma warning (push)
	#pragma warning(disable: 4200 )
		#include <libusb.h>
	#pragma warning (pop)
#else
	#include <libusb.h>
#endif

#include <smav.h>
#include <median.h>

#include "antdefs.h"
#include "antchan.h"
//#include "sensor.h"

#include "tmr.h"


/****************************************************************************

****************************************************************************/

class ENDPOINT  {
public:
	void set_type(unsigned char _type) {
		type = _type;
	}
	void set_addr(unsigned char _addr) {
		addr = _addr;
	}

	void clear(void) {
		type = 0;
		addr = 0;
		return;
	}

private:
	unsigned char type;
	unsigned char addr;
};

/****************************************************************************

****************************************************************************/

class ALTSETTING  {
public:
	void clear(void) {
		int i, n;

		n = eps.size();
		for (i = 0; i < n; i++) {
			eps[i].clear();
		}
		eps.clear();
		return;
	}

	void add_ep(ENDPOINT _ep) {
		eps.push_back(_ep);
	}


private:
	std::vector<ENDPOINT> eps;
};

/****************************************************************************

****************************************************************************/

class INTF  {
public:
	void clear(void) {
		int i, n;

		n = alts.size();
		for (i = 0; i < n; i++) {
			alts[i].clear();
		}
		alts.clear();
		return;
	}
	void add_alt(ALTSETTING _as) {
		alts.push_back(_as);
	}
	void set_alt(std::vector<ALTSETTING> _asvec) {
		alts = _asvec;
	}

private:
	std::vector<ALTSETTING> alts;

};


/****************************************************************************

****************************************************************************/

#ifdef DOPARENT
class ANTDEV : public QObject  {
	Q_OBJECT
#else
class ANTDEV  {
#endif

	friend class ANT;

public:
#ifdef DOPARENT
	explicit ANTDEV(qint64 _start_time, int _debug_level, int _key, QObject* _parent = 0);
#else
	ANTDEV(qint64 _start_time, int _debug_level, int _key);
#endif

	~ANTDEV(void);
	bool get_exists(void) { return exists; }
	void set_exists(bool _b)  { exists = _b; }
	int get_hr(unsigned short _sn);
	int set_device(libusb_device *_dev);
	int start(void);
	int start_listening();
	char mfgr[64] = { 0 };
	char prod[64] = { 0 };
	int assign(int _sn, int _devtype);
	int unassign(int _chan, int _sn=-1);
	float get_ant_cadence(void)  { return ant_cadence; }
	QString get_sensors_string(void);
	void dump_channels(const char *title);
	QMap<int, ANTCHAN> chanmap;

#ifdef _DEBUG
	QString get_sensors_ex(void);
#endif
	const SENSORS* get_sensors(void);

private:


	SENSORS sensors;
	QString sensors_string;

	bool is_listening()  { return listening; }
	int open_listening_channel(void);
	void dump(FILE *_stream = NULL);
	void test(void);
	unsigned char instantaneous_hr = 0;
	unsigned char calculated_hr = 0;
	unsigned char previous_calculated_hr = 0;
	int spikes = 0;
	char serial_number[64] = { 0 };
	libusb_device_handle *handle = NULL;
	int set_devtype(int _chan_num, int _devtype);
	int set_period(int _chan_num, unsigned short _period);
	void clear(void);
	int stop(void);
	libusb_device_descriptor desc;
	libusb_device *get_device(void) {
		return device;
	}
	const char *get_mfgr(void) {
		return mfgr;
	}
	const char *get_product(void) {
		return prod;
	}
	const char *get_sn(void) {
		return sn;
	}
	unsigned short get_vid(void) {
		return vid;
	}
	unsigned short get_pid(void) {
		return pid;
	}
	unsigned char get_bus_number(void) {
		return bus_number;
	}
	unsigned char get_device_address(void) {
		return device_address;
	}
	int get_ix(void) {
		return ix;
	}
	int get_nconfigs(void) {
		return nconfigs;
	}
	int get_nifs(void) {
		return ifs.size();
	}
	void add_intf(INTF _intf);             //{ ifs.push_back(_intf); }
	const char *get_error_string(void) {
		return error_string;
	}
	int close_chan(int _chan);
	float get_ant_wheel_rpm(void)  { return ant_wheel_rpm; }
	float get_ant_meters_per_hour(void)  { return ant_meters_per_hour; }
	unsigned long get_ant_cm(void)  { return ant_cm; }

	bool dbg=false;
	static QMap<int, QString> types;

	// speed/cadence
	float ant_cadence = 0.0f;
	float ant_wheel_rpm = 0.0f;
	float ant_meters_per_hour = 0.0f;							// meters per hour
	unsigned long ant_cm = 0L;                 // cumulative distance (cm)

	//-----------------------------------------
	// heartrate
	//-----------------------------------------

	UCHAR ucStatePage = HRM_INIT_PAGE;              // Track if advanced data is supported
	UCHAR ucPreviousEventCount = 0;           // Previous heart beat event count
	USHORT usPreviousTime1024 = 0;            // Time of previous heart beat event (1/1024 seconds)
	UCHAR ucEventCount = 0;                   // Heart beat event count
	USHORT usTime1024 = 0;                    // Time of last heart beat event (1/1024 seconds)
	ULONG ulElapsedTime2 = 0;                 // Cumulative operating time (elapsed time since battery insertion) in 2 second resolution
	sMav hrsmav = sMav(3);
	MEDIAN hrmed = MEDIAN(5);

#ifdef _DEBUG
	bool showtx = false;
	bool showrx = false;
	bool show_hr_packets = false;
#endif

	bool exists = false;
	Qt::HANDLE tid=0;

	void lock_logging(void);
	void unlock_logging(void);
	void lock_dumping(void);
	void unlock_dumping(void);

	QMutex logmutex;
	QMutex dumpmutex;

#ifdef _DEBUG
	qint64 maxwfm = -1;
	qint64 maxwfr = -1;
	qint64 maxlogwait = -1;
	qint64 maxdumpwait = -1;
#endif

	bool msgdone = false;
	unsigned char response = 0xff;
	unsigned char event = 0;
	int err = 0;
	int id = -1;
	int inpackets = 0;
	int outpackets = 0;
	int process_count = 0;

	UCHAR standard_options = 0;
	UCHAR advanced_options = 0;
	UCHAR advanced2_options = 0;
	unsigned long sernum = 0;
	char sernum_string[16] = { 0 };
	char version[32] = { 0 };
	bool extended = false;

#ifdef _DEBUG
	//#define MESSAGE_TIMEOUT		(1000*1000)
	#define MESSAGE_TIMEOUT       (3300)
	#define RESPONSE_TIMEOUT      (3300)
#else
	#define MESSAGE_TIMEOUT       (3300)
	#define RESPONSE_TIMEOUT      (3300)
#endif

	#define HRM_PAGE0       ((UCHAR)0)
	#define HRM_PAGE1       ((UCHAR)1)
	#define HRM_PAGE2       ((UCHAR)2)
	#define HRM_PAGE3       ((UCHAR)3)
	#define HRM_PAGE4       ((UCHAR)4)

	#define HRM_INVALID_BPM    ((UCHAR)0)

	#define OFFS_SYNC          0
	#define OFFS_LEN           1
	#define OFFS_MSG           2
	#define OFFS_DATA          3                    // variable

	// when it's a channel  event:

	#define OFFS_CHAN          3
	#define OFFS_EVENT_ID      4
	#define OFFS_EVENT_CODE    5

	int hrcalls = 0;

	#ifdef _DEBUG
	int sccalls = 0;
	#endif

	enum HRMStatePage {
		HRM_STD0_PAGE = 0,
		HRM_STD1_PAGE = 1,
		HRM_INIT_PAGE = 2,
		HRM_EXT_PAGE = 3
	};

	#define HRM_TOGGLE_MASK    ((UCHAR)0x80)

	char logstr[1024] = { 0 };
	void logg(const char *format, ...);                         // process log, only called from the reader thread
	void dump(const unsigned char *_buf, int _len, const char *_leader = NULL, const char *_trailer = NULL);
	char leader[256] = { 0 };
	char trailer[256] = { 0 };
	char error_string[256] = { 0 };
	unsigned char net = 0;
	const static unsigned char USER_NETWORK_KEY[8];
	static unsigned char reset[5];
	static unsigned char set_network_key[13];
	static unsigned char assign_channel_extended[8];
	static unsigned char assign_channel[7];
	static unsigned char unassign_channel[5];

	static unsigned char set_channel_id[9];
	static unsigned char set_extended_messages[6];
	static unsigned char lp_search_timeout[6];
	static unsigned char search_timeout[6];
	static unsigned char open_channel[5];
	static unsigned char close_channel[5];
	static unsigned char get_caps[6];
	static unsigned char get_channel_status[6];
	static unsigned char channel_period[7];
	static unsigned char set_channel_freq[6];
	static unsigned char lib_config[6];


	void checksum(unsigned char *_buf, int _len);

	unsigned char user_network_key[8];              // = USER_NETWORK_KEY;
	int key;                                        // unique key



	// Background Data

	UCHAR ucMfgID = 0;                        // Manufacturing ID
	UCHAR ucHwVersion = 0;                    // Hardware version
	UCHAR ucSwVersion = 0;                    // Software version
	UCHAR ucModelNum = 0;                     // Model number
	USHORT usSerialNum = 0;                   // Serial number
	UCHAR ucNoEventCount = 0;                 // Successive transmissions with no new HR events
	ULONG ulAcumEventCount = 0;               // Cumulative heart beat event count
	ULONG ulAcumTime1024 = 0;                 // Cumulative time (1/1024 seconds), conversion to s is performed for data display
	USHORT usR_RInterval1024 = 0;             // R-R interval (1/1024 seconds), conversion to ms is performed for data display
	UCHAR prevcnt = 0;

	#define TXBUFLEN 256
	#define RXBUFLEN 256

	enum States {
		ST_WAIT_FOR_SYNC,
		ST_GET_LENGTH,
		ST_GET_MESSAGE_ID,
		ST_GET_DATA,
		ST_VALIDATE_PACKET
	} state = ST_WAIT_FOR_SYNC;

	enum BSCState {
		BSC_INIT = 0,
		BSC_ACTIVE = 1,
	};

	#define MAX_NO_EVENTS   12       // Maximum number of messages with no new events (3s)

	static int instances;            // count of instances

#ifdef _DEBUG
	static int calls;          // count of calls
#endif

	int init(void);
#ifdef _DEBUG
	int bp = 0;
	bool oktobreak = false;
#endif
	unsigned short vid = 0;
	unsigned short pid = 0;
	char sn[32];
	int nconfigs = 0;
	unsigned char bus_number = 0;
	unsigned char device_address = 0;
	int ix = 0;                                  // index into devlist array
	std::vector<INTF> ifs;
	libusb_device *device = NULL;

	double total_tx_seconds = 0;
	unsigned long tx_calls = 0;
	int send(const unsigned char *a, int len);
	int tx(unsigned char *_buf, int _n, unsigned long _timeout);

	QTimer *txtimer = NULL;
	QTimer *rxtimer = NULL;
	QThread* somethread = NULL;

	static QMap<int, QString> msg_strings;
	static QMap<int, QString> event_strings;
	static QMap<int, QString> startup_strings;

	double total_rx_seconds = 0;
	unsigned long rx_calls = 0;
	unsigned long read_calls = 0;
	bool wfr(UCHAR _msg, qint64 _ms);                     // response to channel event
	bool wfm(UCHAR _msg, qint64 _ms);
	void receiveByte(unsigned char byte);
	void process(void);
	int rx(void);

	unsigned char rxMessage[MAX_MESSAGE_SIZE] = { 0 };
	bool listening = false;
	int maxbytes = 0;

	FILE *logstream = NULL;
	int length = 0;
	int bytes = 0;
	int check_sum = 0;
	int ant_decode_speed_cadence(void);

	USHORT usCadTime1024 = 0;           // Time of last bike cadence event (1/1024s)
	USHORT usCadEventCount = 0;         // Bike cadence event count (pedal revolutions)
	USHORT usSpdTime1024 = 0;           // Time of last bike speed event (1/1024s)
	USHORT usSpdEventCount = 0;         // Bike speed event count (wheel revolutions)
	BSCState eState = BSC_INIT;         // state
	USHORT usCadPreviousEventCount = 0; // Bike cadence previous event count
	USHORT usCadPreviousTime1024 = 0;   // Time of previous bike cadence event (1/1024s)
	USHORT usSpdPreviousEventCount = 0; // Bike speed previous event count
	USHORT usSpdPreviousTime1024 = 0;   // Time of previous bike speed event (1/1024s)
	UCHAR ucNoSpdEventCount = 0;        // counter for successive transmissions with no new speed events
	UCHAR ucNoCadEventCount = 0;        // counter for successive transmissions with no new cadence events
	bool ant_coasting = false;          // coasting flag
	bool ant_stopping = false;
	ULONG ulCadAcumEventCount = 0;      // Cumulative bike cadence event count (pedal revolutions)
	ULONG ulCadAcumTime1024 = 0;        // Cumulative time (1/1024 seconds)
	ULONG ulSpdAcumEventCount = 0;      // Cumulative bike speed event count (wheel revolutions)
	ULONG ulSpdAcumTime1024 = 0;        // Cumulative time (1/1024 seconds)
	UCHAR ant_circumference_cm = 70;    // Wheel circumference (cm)
	int decode_hr(void);
	unsigned char txq[TXBUFLEN] = { 0 };
	unsigned short txinptr = 0;
	unsigned short txoutptr = 0;

	unsigned char rxq[RXBUFLEN] = { 0 };
	unsigned short rxinptr = 0;
	unsigned short rxoutptr = 0;
	int wep = -1;
	int rep = -1;

	qint64 start_time = 0;

	//QObject *parent;
	RACERMATE::Tmr *tmr = NULL;
	int debug_level = 0;


private slots:
	void writer(void);
	void reader(void);

};

#endif      // #ifndef _SENTRY_H_

