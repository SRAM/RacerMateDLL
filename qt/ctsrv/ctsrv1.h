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

/***************************************************************************************
 1 = QTimer																works with QMainWindow. visual studio error:
																			QObject::startTimer: Timers can only be used with threads started with QThread

 2 = QBasicTimer														works with QMainWindow. visual studio error:
																			QObject::startTimer: Timers can only be used with threads started with QThread

 3 = QObject::startTimer()											works with QMainWindow. visual studio error:
																			QObject::startTimer: Timers can only be used with threads started with QThread

 4 = QTimer with worker_thread.start()							doesn't work with QMainWindow, essentially the same as method 6 (DON'T TEST)

 5 = QTimer, subclassing QThread instead of QObject		works with QMainWindow
																			vs error: Timers can only be used with threads started with QThread
																			same as #1 exctept for this subclassing

 6 = QTimer started in a thread									works with QMainWindow, visual studio error:
																			QObject: Cannot create children for a parent that is in a different thread.
																			in QMainWindow, this line just starts the parent timer and keeps going
																			in visual studio no timer is started

 7 = QTimer started in a thread, modification of #6		same problem as #6
		but with signal/slot connection and defining a
		QThread-derived class

 8 = using moveToThread()											error: QObject::moveToThread: Cannot move objects with a parent

 9 = playing with QCoreApplication

***************************************************************************************/

#define TIMER_METH 6
#define SUBMETH6 1

#if TIMER_METH==7
	#include "mythread.h"
#endif

#include "udp_client.h"


namespace MYQT  {

/**************************************************************************************


************************************************************************************/

#if TIMER_METH==5
	class Ctsrv : public QThread {
		Q_OBJECT
		public:
			explicit Ctsrv(QObject *_parent = 0);
			explicit Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover = false, QObject *_parent = 0);
/*
#elif TIMER_METH==6
	class Ctsrv : public QThread {
		Q_OBJECT
		public:
			explicit Ctsrv(QObject *_parent = 0);
			explicit Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover = false, QObject *_parent = 0);
*/

#elif TIMER_METH==8
	class Ctsrv : public QObject {
		Q_OBJECT
		public:
			explicit Ctsrv(void);
			explicit Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover = false);
#else
	class Ctsrv : public QObject {
		Q_OBJECT
		public:
			explicit Ctsrv(QObject *_parent = 0);
			explicit Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover = false, QObject *_parent = 0);
#endif


	~Ctsrv(void);

	// setters:
	void setStarted(bool value);
	void setFinished(bool value);
	void setPaused(bool value);
	qt_SS::BARINFO *get_barinfo(int i);
	void start(void);
	void stop(void);

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

signals:
	void connected_to_trainer_signal(int _id, bool _b);
	void data_signal(int, MYQT::QT_DATA *);
	void ss_signal(int, MYQT::qt_SS::SSD *);
	void rescale_signal(int, int);
	//void datasig(int, NHBDATA *);				// sends data to the final app
	void gradechanged_signal(int _igradex10);
	void windchanged_signal(float);
#ifdef TESTING
	void testing_signal(int, MYQT::TESTDATA *);
#endif

private slots:
	#if TIMER_METH==1
		void timer_slot();
	#elif TIMER_METH==2
	#elif TIMER_METH==3
	#elif TIMER_METH==4
		void timer_slot();
	#elif TIMER_METH==5
		void timer_slot();
	#elif TIMER_METH==6
		void timer_slot();
		void about_to_quit();
		void onStarted();
	#elif TIMER_METH==7
	#elif TIMER_METH==8
		void timer_slot();
		void onStarted();
	#endif


	#ifdef TESTING
		//void testing_slot(int, MYQT::TESTDATA *);
		void testing_slot(int, TESTDATA *);
	#endif
	void processPendingDatagrams();
	// from the broadcast socket
	void bc_readyRead_slot();
	void bc_timer_slot();
	// from the app:
	void gradechanged_slot(int _igradex10);
	void windchanged_slot(float);
	// from the clients:
	void connected_to_trainer_slot(int _id, bool _b);
	void data_slot(int, QT_DATA *);
	void ss_slot(int, qt_SS::SSD *);
	void rescale_slot(int _id, int _maxforce);
	//void newConnection();
	void disconnected();
	void error(QAbstractSocket::SocketError socketError);
	void hostFound();
	void stateChanged(QAbstractSocket::SocketState socketState);


private:
	bool in_qt_creator;

	TrainerData td;
	int msgcnt;
	QObject *parent;

#if TIMER_METH==1
	QTimer *timer;
#elif TIMER_METH==2
	QBasicTimer *timer;
	void timerEvent(QTimerEvent* event);
#elif TIMER_METH==3
	QBasicTimer *timer;
	void timerEvent(QTimerEvent* event);
#elif TIMER_METH==4
	QTimer *timer;
	QThread worker_thread;
	void run();
	QEventLoop loop;
#elif TIMER_METH==5
	QTimer *timer;
#elif TIMER_METH==6
	QTimer *timer;
	static int argc;
	static char * argv[];
	static QCoreApplication * app;
	static QThread * thread;
	void run();
#elif TIMER_METH==7
	myThread *thread;
#elif TIMER_METH==8
	QThread * thread;											// cannot move objects with parent
	QTimer *timer;
#elif TIMER_METH==9
	//QCoreApplication a(argc, argv);
	QCoreApplication *app;
#endif

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

	//#define BACKLOG 10

	static int instances;

	Tmr *at;
	bool log_to_console;

	unsigned long last_ip_broadcast_time;                    // ethernet

	int bp;
	int init(void);
	int myclose(void);
	//static void mythread(void *);
	//bool thread_running;
	//bool contin;
	void destroy(void);
	qint16 listen_port;
	//fd_set readfds;                           // read set
	//fd_set writefds;                          // write set
	//fd_set exceptfds;                         // exception set
	void logg(bool _print, const char *format, ...);
	FILE *logstream;
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

	typedef struct  {
		char name[32];
		char addr[32];
	} IFACE;

	std::vector<IFACE> ifaces;


public:

	int getPortNumber(void) {
		return 201;
	}
	void flush(void);
	int get_client_socket(int _ix);

};

}


#endif // CTSRV_H

