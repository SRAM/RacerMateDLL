#ifndef CTSRV_H
#define CTSRV_H

#include <QHostAddress>
#include <QUdpSocket>

#include <tmr.h>

#include <rmdefs.h>

#include <testdata.h>
//#include <device.h>
#include <trainerdata.h>
#include <ssdata.h>

//#define TEST_WITH_NO_APP
#define NEWAGE


#include <qt_data.h>
//#ifndef NEWAGE
//	#include "udp_client.h"
//#else
	#include <qt_ss.h>
//#endif

#include "worker.h"


/***************************************************************************************

***************************************************************************************/


namespace MYQT  {

/**************************************************************************************


************************************************************************************/

#ifdef NEWAGE
	class Q_DECL_EXPORT Ctsrv : public QObject {
#else
	class Q_DECL_EXPORT Ctsrv : public QThread {
#endif

#ifdef QT_CORE_LIB
		Q_OBJECT
#endif

		private:
			Worker* m_worker;
			QThread* m_workerThread;
			static QCoreApplication *m_application;
			QObject *parent;
			static Ctsrv* m_instance;

			//static int argc;
			//static char * argv[];

#ifndef NEWAGE
			//static int argc;
			//static char * argv[];
			//static QCoreApplication * app;
			//static QThread * thread;
#endif

		public:
#ifndef NEWAGE
		explicit Ctsrv(QObject *_parent = 0);
#endif
		explicit Ctsrv(int _listen_port, int _broadcast_port=-1, QObject *_parent = 0);
		~Ctsrv(void);

		static Ctsrv* instance();
		void doSomeOtherStuff(int value);

		void stop(void);
		qt_SS::BARINFO *get_barinfo(int i);
		void setStarted(bool value);
		void setFinished(bool value);
		void setPaused(bool value);

#ifndef NEWAGE
	// setters:
	//void start(void);

	// rm1 compatibility+++

	// move defs to common area+++
//#define UDP_SERVER_SERIAL_PORT_BASE 221						// 221 - 236
	struct TrainerData GetTrainerData(int _ix, int _fw);
//	DEVICE get_device(int);
	//SSDATA get_ss_data(int _ix);
	// move to common area---

	float get_watts_factor(int _ix);
	bool client_is_running(int _ix);
	int end_cal(int _ix);
	int expect(int _ix, const char *_str, DWORD _timeout);
	float *get_average_bars(int _ix);
	float *get_bars(int _ix);
	float get_ftp(int _ix);
	int get_handlebar_buttons(int _ix);
	bool is_client_connected(int _comport);
	bool is_paused(int _ix);
	int reset_averages(int _ix);
	int reset_client(int _ix);
	int rx(int _ix, unsigned char *buf, int buflen);
	int send(int _ix, const unsigned char *_str, int _len, bool _flush=false);
	int set_ergo_mode(int _ix, int _fw, int _rrc, float _manwatts);
	void set_export(int _ix, const char *_dbgfname);
	void set_file_mode(int _ix, bool _mode);
	int set_ftp(int _ix, float _ftp);
	int set_hr_bounds(int _ix, int _minhr, int _maxhr, bool _beepEnabled);
	int set_paused(int _ix, bool _paused);
	int set_slope(int _ix, float _bike_kgs, float _person_kgs, int _drag_factor, float _grade);
	int start_cal(int _ix);
	void txx(unsigned char *b, int n);

	// rm1 compatibility---
#endif

signals:
	void ss_signal(int, MYQT::qt_SS::SSD *);
	void data_signal(int, MYQT::QT_DATA *);
	void rescale_signal(int, int);

#ifndef NEWAGE
	void gradechanged_signal(int _igradex10);
	void windchanged_signal(float);
	#ifdef TESTING
		void testing_signal(int, MYQT::TESTDATA *);
	#endif

	void connected_to_trainer_signal(int _id, bool _b);
	//void datasig(int, NHBDATA *);				// sends data to the final app
#endif


private slots:
#ifdef NEWAGE
	//void timer_slot();
	void handleResults(const QString &s);

	// signals from UDPClient
	void data_slot(int _ix, MYQT::QT_DATA *_data);
	void ss_slot(int _ix, MYQT::qt_SS::SSD *_ssd);
	void rescale_slot(int _ix, int _maxforce);
	//void connected_to_trainer_signal(int, bool);

#else
	void onStarted();
	// from the broadcast socket
	void bc_readyRead_slot();
	void bc_timer_slot();
	void disconnected();
	void hostFound();

	// from the app:
	void gradechanged_slot(int _igradex10);
	void windchanged_slot(float);
	// from the clients:
	void connected_to_trainer_slot(int _id, bool _b);
	//void newConnection();
	void error(QAbstractSocket::SocketError socketError);
	void stateChanged(QAbstractSocket::SocketState socketState);

	#ifdef TESTING
		//void testing_slot(int, MYQT::TESTDATA *);
		void testing_slot(int, TESTDATA *);
	#endif
#endif




private:

#ifndef NEWAGE
	typedef struct  {
		char name[32];
		char addr[32];
	} IFACE;

	std::vector<IFACE> ifaces;

	bool in_qt_creator;
	void run();
	bool contin;
	bool running;
	//int count;
	int msgcnt;
	QTimer *bctimer;                                  // broadcast timer
	bool started;
	bool finished;
	bool paused;
	int curid;
	//#define MAXCLIENTS2 16

	#ifndef MAXCLIENTS
		static const int MAXCLIENTS=1;
	#endif

	UDPClient *udpclients[MAXCLIENTS];
	unsigned long last_ip_broadcast_time;                    // ethernet
	int errors;
	bool ip_discover;
	unsigned bcast_count;
	QByteArray bc_datagram;

	QHostAddress myip;
	QHostAddress bcaddr;

	QUdpSocket *bcsocket;
	QUdpSocket *udp_server_socket;

	int create_udp_server_socket(void);
	unsigned short bcport;
	TrainerData td;
	//QTimer *timer;
	qint16 listen_port;
#endif

	static int instances;

	Tmr *at;
	bool log_to_console;


	int bp;
	int init(void);

	void logg(bool _print, const char *format, ...);
	FILE *logstream;


public:

	int getPortNumber(void) {
		return 201;
	}

#ifndef NEWAGE
	void flush(void);
	int get_client_socket(int _ix);
#endif

};

}


#endif // CTSRV_H

