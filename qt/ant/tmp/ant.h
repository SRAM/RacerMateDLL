#ifndef _ANT_H
#define _ANT_H

#include <vector>

#include <QObject>
#include <QFile>

#include "sl_global.h"

#include <tmr.h>

#include "antdev.h"

#include "antworker.h"

namespace RACERMATE {

/********************************************************************************************

********************************************************************************************/

class Q_DECL_EXPORT ANT : public QObject  {
	Q_OBJECT

friend class antWorker;

	public:

#define ANT_INVALID_SENSOR -1;
#define HR_DEV_TYPE	120						// heart rate
#define SC_DEV_TYPE	121						// speed cadence
#define C_DEV_TYPE	122						// cadence

#define ANT_HR 0
#define ANT_C 1
#define ANT_SC 2

	int stop(void);
	//int associate(38760, "UDP-5555")
	int assign_sensor(int _sn, int _type);
	int unassign_sensor(int _sn);
	const char* get_sticks(void);

	private:
		char sensors_string[256]={0};
		const char *ssptr=NULL;

		char sticks[512];									// ascii list of sticks
		Qt::HANDLE tid=0;
		int nsticks = 0;
		QMutex devices_mutex;
		QMutex sensors_mutex;
		bool scanning_for_devices = false;

		static ANT* m_instance;
		QCoreApplication* m_application = NULL;
		antWorker* worker = NULL;
		QThread* workerThread = NULL;

		#define ANT_STANDARD_DATA_PAYLOAD_SIZE		((UCHAR)8)

		#define SCPERIOD  8086							// must be 8086 or you get a lot of timeouts, although it tries to work
		#define SCDEVTYPE 121							// has to be 121
		#define HRPERIOD 8070
		#define HRDEVTYPE 120

		// http://www.linux-usb.org/usb.ids:

		#define DYNASTREAM_VID		0x0fcf			// 4047

		#define ANT_Development_Board1	0x1003
		#define ANTUSB_Stick					0x1004
		#define ANT_Development_Board2	0x1006
		#define ANTUSB2_Stick				0x1008
		#define ANTUSB_m_Stick				0x1009

		bool dbg = false;

		void dump_antdevs(void);

		RACERMATE::Tmr *tmr=NULL;
		qint64 start_time;
		int debug_level;
		QObject *parent;
		FILE *logstream = NULL;
		char logstr[2048] = {0};
		libusb_context *ctx = NULL;										// a libusb session
		int nants=0;
		const struct libusb_version *verstruc = NULL;
		unsigned long version=0;
		QMap<quint32, ANTDEV *> devices;
		libusb_device **usb_device_list=NULL;
		char gstr[256]={0};
		unsigned long shutdown_delay=0;
		int init(void);
		void logg(const char *format, ...);
		void merge_log_files(void);
		int bp=0;
		bool listening = false;


	public:
		explicit ANT(QObject *parent = 0);
		explicit ANT(int _debug_level, QObject *parent = 0);
		~ANT();
		const SENSORS* get_sensors(void);
		void dump(void);
		float get_ant_cadence(quint32 _devnum);
		float get_ant_wheel_rpm(quint32 _devnum);
		float get_ant_meters_per_hour(quint32 _devnum);
		unsigned long get_ant_cm(quint32 _devnum);
		int get_hr(unsigned short _sn);
		int scan_for_devices(void);
		int start_listening(void);
		const char *get_sensors_string(void);						// returns sensor_string

#ifdef _DEBUG
		const char *get_sensors_ex(void);
#endif
		bool get_scanning_for_devices(void)  { return scanning_for_devices; }
		int get_n_devices(void)  { return devices.size(); }
		bool is_listening(void)  { return listening; }

	signals:
	private slots:

	protected slots:

	public slots:

	private:
		char channel_status_string[4][32] = {
											"UA",									// "UNASSIGNED CHANNEL",
											"A",									// "ASSIGNED CHANNEL",
											"S",									// "SEARCHING CHANNEL",
											"T"									// "TRACKING CHANNEL"
		};

		bool got_caps = false;
		bool got_status = false;
		bool got_id = false;
		int devnum = 0;
		int netnum = 0;
		bool log_raw = false;
		unsigned char ant_key[8] = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};		// antplus key
		USHORT ant_baud = 57600;
		bool asi;
		bool sensors_initialized;
		bool initialized = false;
		unsigned long g_ant_tid;
		char ant_logname[16];
		DWORD ant_start_time = 0L;
		FILE *ant_logstream = NULL;
		bool do_ant_logging = false;
		bool inprocess;
		bool ant_closed;
		char ant_gstring[2048];
		char pstr[256];
		char ant_error_string[256];
		char ant_str[256];
		char gstatus[4][32];					// astrstatus
		USHORT usDevicePID;
		USHORT usDeviceVID;
		UCHAR aucDeviceDescription[256];
		UCHAR aucDeviceSerial[256];
		bool bursting;										//holds whether the bursting phase of the test has started
		bool ant_broadcasting;
		bool ant_done;
		bool ant_rundone;
		bool ant_display;
		UCHAR ant_txbuf[ANT_STANDARD_DATA_PAYLOAD_SIZE];
		bool gant_channel_opened;
		UCHAR ant_caps[5];
		unsigned char ant_ack;
		unsigned char ant_msg;
};									// class ANT

}			// namespace RACERMATE

#endif					// #ifndef _ANT_H_

