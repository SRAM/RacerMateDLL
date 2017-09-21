
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostInfo>

#ifdef WIN32
	#include <assert.h>
#endif

#include <comdefs.h>
#include <comutils.h>

#include "ctsrv.h"


namespace MYQT  {

static inline qint32 ArrayToInt(QByteArray source);


int Ctsrv::instances = 0;

#if TIMER_METH==6
	int Ctsrv::argc = 1;
	char * Ctsrv::argv[] = {"Ctsrv", NULL};
	QCoreApplication * Ctsrv::app = NULL;
	QThread * Ctsrv::thread = NULL;
#endif

/**********************************************************************

**********************************************************************/

#if TIMER_METH==5
Ctsrv::Ctsrv(QObject *_parent) : QThread(_parent) {
//#elif TIMER_METH==6
//Ctsrv::Ctsrv(QObject *_parent) : QThread(_parent) {
#elif TIMER_METH==8
	Ctsrv::Ctsrv(void)  {
#else
Ctsrv::Ctsrv(QObject *_parent) : QObject(_parent) {
#endif

	parent = _parent;

	int status;

	throw("bad Ctsrv constructor");

	listen_port = 9072;

	status = init();
	if (status != 0) {
		throw 100;
	}


}                       // constructor

/**********************************************************************
	in vs
		parent is null
		this (Ctsrv) is a subclass of QObject

	in qt creator
		parent is a MainWindow ( QObject * )
		this (Ctsrv) is a subclass of QObject

**********************************************************************/

#if TIMER_METH==5
	Ctsrv::Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover, QObject *_parent) : QThread(_parent) {
	//Ctsrv::Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover, QObject *_parent) : QObject(_parent) {
//#elif TIMER_METH==6
//	Ctsrv::Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover, QObject *_parent) : QThread(_parent) {
#elif TIMER_METH==8
	Ctsrv::Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover) {
#else
	Ctsrv::Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover, QObject *_parent) : QObject(_parent) {
#endif


	parent = _parent;

	int status;

	bcport = _broadcast_port;
	listen_port = _listen_port;
	ip_discover = _ip_discover;
	//udp = _udp;


	status = init();
	if (status != 0) {
		throw 100;
	}

#if TIMER_METH==6
	// xxx

#ifdef SUBMETH6==1
	if (QCoreApplication::instance() == NULL)  {
		try  {
			qDebug() << "creating app";
			app = new QCoreApplication(argc, argv);		// WARNING: QApplication was not created in the main() thread.
			timer = new QTimer(app);
			connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
			qDebug() << "starting timer";
			timer->start(10);

			if (thread == NULL)  {
				qDebug() << "creating thread";
				thread = new QThread();
				connect(thread, SIGNAL(started()), this, SLOT(onStarted()), Qt::DirectConnection);
				thread->start();
			}

			//app->moveToThread(thread);			// QCoreApplication::exec: Must be called from the main thread
			//this->moveToThread(thread);			// blocks forever
			//thread->moveToThread(this);			// have to subclass qthread and it blocks

			app->exec();
			qDebug() << "app->exec() finished";
		}
		catch (...) {
			qDebug() << "crash";
		}

	}
#elif SUBMETH6==2
#endif

	qDebug() << "returning from constructor";
	bp = 7;

	/*
	app = new QCoreApplication(argc, argv);
	timer = new QTimer(app);
	connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
	app->exec();
	bp = 2;
	*/

#endif
}


/**********************************************************************
	destructor
**********************************************************************/

Ctsrv::~Ctsrv() {
	destroy();
	bp = 0;
	instances--;

}

/**********************************************************************

**********************************************************************/

int Ctsrv::init(void) {
	int status;
	int i;
	//char str[256];

#if TIMER_METH==8
	thread = 0;
	timer = 0;
#endif

	if (QCoreApplication::instance() == NULL)  {
		in_qt_creator = false;
	}
	else  {
		in_qt_creator = true;
	}

	msgcnt = 0;
	curid = 0;
	udp_server_socket = 0;

	for (i = 0; i < MAXCLIENTS; i++) {
		udpclients[i] = new UDPClient(i, this);

		connect(udpclients[i], SIGNAL(data_signal(int, QT_DATA*)), this, SLOT(data_slot(int, QT_DATA*)));
		connect(udpclients[i], SIGNAL(ss_signal(int, qt_SS::SSD*)), this, SLOT(ss_slot(int, qt_SS::SSD*)));
		connect(udpclients[i], SIGNAL(rescale_signal(int, int)), this, SLOT(rescale_slot(int, int)));
#ifdef TESTING
		connect(udpclients[i], SIGNAL(testing_signal(int, TESTDATA*)), this, SLOT(testing_slot(int, TESTDATA*)));
#endif
		connect(this, SIGNAL(gradechanged_signal(int)), udpclients[i], SLOT(gradechanged_slot(int)));
		connect(this, SIGNAL(windchanged_signal(float)), udpclients[i], SLOT(windchanged_slot(float)));

	}

	started = false;
	finished = false;
	paused = false;

	log_to_console = true;

	bc_datagram.clear();

	bcast_count = 0L;
	errors = 0;

	//contin = false;
	logstream = fopen("Ctsrv.log", "wt");

	last_ip_broadcast_time = 0L;
	bcsocket = NULL;

	bp = 0;
	//thread_running = false;


	QStringList items;
	 //int done = 0;
	 bcaddr = "miney2.mselectron.net";

	 /*
	 bool done = false;
	 foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
		if (interface.flags().testFlag(QNetworkInterface::IsUp) && !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

			foreach(QNetworkAddressEntry entry, interface.addressEntries()) {
				if ( interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".")) {
					items << interface.name() + " " + entry.ip().toString() + " " + interface.hardwareAddress();
					myip = entry.ip();

#ifdef _DEBUG
					bcaddr = "miney2.mselectron.net";
					//bcaddr = entry.broadcast();
#else
					bcaddr = entry.broadcast();
#endif
						  done = true;
						  //done = 1;
					break;
					// "eth0	192.168.1.20	F0:BF:97:57:D5:9B"
					// "wlan0	192.168.1.21	40:25:C2:26:FE:78"
				}
			}
			if (done) {
				break;
			}
		}
	}
	 */



	bp = 0;

	//logg(log_to_console, "myip = %s\r\n", myip.toString().toStdString().c_str() );

	bc_datagram.append("Racermate ");
	bc_datagram.append(QString::number(listen_port));
	bc_datagram.append(" ");
	bc_datagram.append(myip.toString());
	bc_datagram.append("\r\n");

	//bool b;
	if (ip_discover) {
		bcsocket = new QUdpSocket(this);
		//#ifdef _DEBUG
		#if 1
		QHostInfo info = QHostInfo::fromName("miney2.mselectron.net");    // QHostInfo::localHostName()  );
		QList<QHostAddress> ip = info.addresses();
		bcaddr = ip[0];
		qDebug() << "bcaddr = " << bcaddr;
		#endif
	}                                // if (ip_discover)  {

	#if TIMER_METH==1
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));

		timer->start(10);			// Qt error 1
										// QObject::startTimer: Timers can only be used with threads started with QThread

	#elif TIMER_METH==2
		timer = new QBasicTimer();
		timer->start(10,this);
	#elif TIMER_METH==3
		startTimer(10);
	#elif TIMER_METH==4
		//timer = new QTimer(this);
		//connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
		//QCoreApplication::exec();
		//this->moveToThread(&worker_thread);
		//timer->moveToThread(&worker_thread);

		worker_thread.start();

		//connect(&netobject, SIGNAL(done()), &loop, SLOT(quit()));
		//netobject.start_long_lived_process_that_happens_on_other_thread();

		//loop.exec(); // BLOCKING (non-busy) until quit() is called via the signal done()
		//app.processEvents();
		//app.sendPostedEvents();
		//app.exit();
		//timer->start(10);
	#elif TIMER_METH==5
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
		timer->start(10);
	#elif TIMER_METH==6
		/*
			http://stackoverflow.com/questions/2150488/using-a-qt-based-dll-in-a-non-qt-application
			http://stackoverflow.com/questions/10697107/qeventloop-cannot-be-used-without-qapplication
		*/

		/*
		//if (!in_qt_creator)  {
			app = new QCoreApplication(argc, argv);
			app->exec();
		//}

		if (thread == NULL)  {
			qDebug()<<"Ctsrv:: thread id = " << QThread::currentThreadId();

			thread = new QThread();

			qDebug()<<"Ctsrv:: newly created thread id = " << thread->currentThreadId();

			connect(thread, SIGNAL(started()), this, SLOT(onStarted()), Qt::DirectConnection);
			thread->start();
		}
		*/

	#elif TIMER_METH==7
		qDebug()<<"Ctsrv:: thread id = " << QThread::currentThreadId();
		thread = new myThread(this);

#elif TIMER_METH==8
	qDebug()<<"Ctsrv:: thread id = " << QThread::currentThreadId();
	thread = new QThread(this);
	Q_ASSERT(thread);

	//QThread thread;
	//static QCoreApplication * app;

	//QCoreApplication *x = QCoreApplication::instance();

	//QObject *o = this->
	thread->moveToThread(this->thread);												// QObject::moveToThread: Cannot move objects with a parent

	//thread->moveToThread(QApplication::instance()->thread());
	//moveToThread(&thread);															// QObject::moveToThread: Cannot move objects with a parent

	connect(thread, SIGNAL(started()), this, SLOT(onStarted()));
	thread->start();
	#endif


	status = create_udp_server_socket();                                     // creates the Ctsrv socket and starts listening.

	if (status != 0) {
		return 1;
	}



	at = new Tmr("Ctsrv");

	//contin = true;
	//qDebug() << "started thread";

	if (ip_discover) {
		// create a timer for broadcasting
		bctimer = new QTimer(this);
		connect( bctimer, SIGNAL(timeout()), this, SLOT(bc_timer_slot())  );
		bctimer->start(2000);
	}

	instances++;
	qDebug() << "Ctsrv::init x";

	return 0;
}                                            // init(void)


/**********************************************************************

**********************************************************************/

void Ctsrv::destroy(void) {
	myclose();
	return;
}                                            // destroy()

/**********************************************************************

**********************************************************************/

int Ctsrv::myclose(void) {
	int rc = 0;


#if TIMER_METH==6
	qDebug() << "myclose";
	if (app)  {
		//if (timer)  {
			//timer->stop();
			//timer = 0;
		//}
		app->exit();
		delete app;						// this will kill the timer?
		app = nullptr;
		timer = 0;
	}
#endif

	/*
	unsigned long start;
	contin = FALSE;

	start = QDateTime::currentMSecsSinceEpoch();

	while (thread_running) {
		if ((QDateTime::currentMSecsSinceEpoch() - start) >= 2000) {
			rc = 1;
			break;
		}
		bp++;
	}
	bp = 1;
	*/


	if (bcsocket != NULL) {
		bcsocket->close();
		delete bcsocket;
	}

	if (udp_server_socket) {
		//udp_server_socket->disconnect();
		udp_server_socket->close();
	}

//	int i;


	delete at;

	fclose(logstream);
	logstream = NULL;
	return rc;

}                          // myclose()

/**********************************************************************

 ***********************************************************************/

void Ctsrv::logg(bool _print, const char *format, ...) {
	int len;

	len = (int)strlen(format);
	if (len > 1023) {
		if (_print) {
			qDebug() << "string too long in logg()";
		}
		if (logstream) {
			fprintf(logstream, "\r\n(string too long in logg())\r\n");
			//fflush(logstream);
		}
		return;
	}

	/*
	va_list ap;                                     // Argument pointer
	char s[1024];                                   // Output string
	bool qt = false;

#ifdef QT_CORE_LIB
	qt = true;
#endif

	if (qt)  {
		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);

		if (_print) {
			qDebug() << s;
		}
	}
	*/

	return;
}                                      // logg()

/**********************************************************************

**********************************************************************/

qint32 ArrayToInt(QByteArray source) {
	qint32 temp;
	QDataStream data(&source, QIODevice::ReadWrite);

	data >> temp;
	return temp;
}                       // ArrayToInt()


/**********************************************************************

**********************************************************************/

void Ctsrv::setPaused(bool value) {
	paused = value;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::setFinished(bool value) {
	finished = value;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::setStarted(bool value) {
	started = value;
}

/**********************************************************************

**********************************************************************/

int Ctsrv::create_udp_server_socket(void)  {
	bool b;

	udp_server_socket = new QUdpSocket(this);
	b = udp_server_socket->bind(QHostAddress::AnyIPv4, listen_port);
	if (!b) {
		qFatal("error creating Ctsrv socket 1");
	}

	//connect(udp_server_socket, SIGNAL(connected()), this, SLOT(newConnection()));                   // never gets there
	connect(udp_server_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(udp_server_socket, SIGNAL(hostFound()), this, SLOT(hostFound()));
	connect(udp_server_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	// these have errors:

	//connect(udp_server_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState socketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState socketState)));
	//connect(udp_server_socket, SIGNAL(error(QAbstractSocket::SocketError socketError)), this, SLOT(error(QAbstractSocket::SocketError socketError)));

	return 0;
}


/**********************************************************************

**********************************************************************/

qt_SS::BARINFO *Ctsrv:: get_barinfo(int i) {
	return udpclients[0]->ss->get_barinfo(i);
	//return tcpclients[0]->ss->get_barinfo(i);
//	return NULL;
}                          // get_barinfo(int i)


// slots+++

/*********************************************************************************************************************************
	bc_timer slot
*********************************************************************************************************************************/

void Ctsrv::bc_timer_slot() {
	//at->update();							// 3001 ms

	qint64 n;

	n = bcsocket->writeDatagram(bc_datagram, bcaddr, bcport);            // miney2.mselectron.net, 9071

	if (n <= 0) {
		qDebug() << "n = " << n;
		qDebug() << "bc_datagram = " << bc_datagram;
		qDebug() << "bcaddr = " << bcaddr;
		qDebug() << "bcport = " << bcport;
		qFatal("broadcast write error");
	}
	else  {
		qDebug() << "n = " << n;
		qDebug() << "bc_datagram = " << bc_datagram;
		qDebug() << "bcaddr = " << bcaddr;
		qDebug() << "bcport = " << bcport;
	}

	return;
}                          // timerslot()

/**********************************************************************
	broadcast socket (bcsocket) read slot:
**********************************************************************/

void Ctsrv::bc_readyRead_slot() {
	QByteArray buf;
	QHostAddress sender;
	quint16 senderPort;
	//qint64 n;


	buf.resize(bcsocket->pendingDatagramSize());

	//n =
	bcsocket->readDatagram(buf.data(), buf.size(), &sender, &senderPort );

	return;
}                       // bc_readyRead()


/**********************************************************************
	Ctsrv slot, udp Ctsrv only
**********************************************************************/

void Ctsrv::disconnected() {
	qDebug() << "udp Ctsrv disconnecting";
	return;
}

/**********************************************************************
	Ctsrv slot, udp Ctsrv only
**********************************************************************/

void Ctsrv::error(QAbstractSocket::SocketError socketError) {
	Q_UNUSED(socketError);
	bp = 4;
	return;
}

/**********************************************************************
	Ctsrv slot, udp Ctsrv only
**********************************************************************/

void Ctsrv::hostFound() {
	bp = 4;
	return;
}

/**********************************************************************
	Ctsrv slot, udp Ctsrv only
**********************************************************************/

void Ctsrv::stateChanged(QAbstractSocket::SocketState socketState) {
	Q_UNUSED(socketState);

	bp = 4;
	return;
}

/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::connected_to_trainer_slot(int _id, bool _b) {
	emit connected_to_trainer_signal(_id, _b);                     // pass upstream

	return;
}

/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::data_slot(int _id, QT_DATA *_data) {
	emit data_signal(_id, _data);

	return;
}

#ifdef TESTING
/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::testing_slot(int _id, TESTDATA *_data) {
	emit testing_signal(_id, _data);
	return;
}
#endif

/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::rescale_slot(int _id, int _maxforce) {
	emit rescale_signal(_id, _maxforce);

	return;
}

/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::ss_slot(int _id, qt_SS::SSD *_ssd) {
	emit ss_signal(_id, _ssd);

	return;
}


/**********************************************************************
	app slot
**********************************************************************/

void Ctsrv::gradechanged_slot(int _igradex10) {
//Q_UNUSED(_igradex10);

#ifdef _DEBUG
	int igradex10;
	float grade;                          // sent float grade, controlled by course or manually
#endif

#ifdef _DEBUG
	igradex10 = _igradex10;
	grade = (float)igradex10 / 10.0f;
	qDebug() << "grade = " << grade;
#endif

	emit(gradechanged_signal(_igradex10));


	return;
}

/**********************************************************************
	app slot
**********************************************************************/

void Ctsrv::windchanged_slot(float _f) {
	Q_UNUSED(_f);

#ifdef _DEBUG
	float wind;
	wind = _f;
	qDebug() << qSetRealNumberPrecision(1) << "wind = " << wind;
#endif
	emit(windchanged_signal(_f));

	return;
}

/*********************************************************************************************************************************
	when the Ctsrv is udp
*********************************************************************************************************************************/

void Ctsrv::processPendingDatagrams(void) {
	udpclients[0]->readDatagram(udp_server_socket);
}                          // processPendingDatagrams()

// slots---

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void Ctsrv::start(void)  {
	return;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void Ctsrv::stop(void)  {
	return;
}

// rm1 compatibility+++

/*********************************************************************************************************************************

*********************************************************************************************************************************/

struct TrainerData Ctsrv::GetTrainerData(int _ix, int _fw)  {
	Q_UNUSED(_fw);

	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
#ifdef DOITTOTHEMAX
		td = udpclients[ix]->GetTrainerData(_fw);
#endif
	}
	else  {
		memset(&td, 0, sizeof(td));
	}

	return td;

}


#ifdef DOITTOTHEMAX
/*********************************************************************************************************************************

*********************************************************************************************************************************/

DEVICE Ctsrv::get_device(int _ix)  {
	if (udpclients[_ix])  {
		return udpclients[_ix]->get_device();
	}

	assert(false);
}
#endif

/*********************************************************************************************************************************

*********************************************************************************************************************************/

//SSDATA Ctsrv::get_ss_data(int _ix)  {
	//return 0;
//}


/*********************************************************************************************************************************

*********************************************************************************************************************************/

float Ctsrv::get_watts_factor(int _ix)  {
	//Q_UNUSED(_ix);
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		float f;
		f = udpclients[ix]->get_watts_factor();
		return f;
	}
	return 0.0f;

}


/*********************************************************************************************************************************

*********************************************************************************************************************************/

bool Ctsrv::client_is_running(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		return true;
	}
	return false;

	return false;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::end_cal(int _ix)  {
	int ix, status;

	ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		status = udpclients[ix]->end_cal();
	}

	return status;


	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::expect(int _ix, const char *_str, DWORD _timeout)  {
	//Q_UNUSED(_ix);
	//Q_UNUSED(str);
	//Q_UNUSED(timeout);

	int len,i=0;
	unsigned char c[8];
	DWORD start,end;
	int n;

	len = strlen(_str);

	start = getms();

	while(1)  {
		n = rx(_ix, c, 1);

		if (n==1)  {
			if (c[0]==_str[i])  {
				i++;
				if (i==len)  {
					return 0;
				}
			}
			else  {
				i = 0;
			}
		}
		else  {
			end = getms();
			if ((end-start)>_timeout)  {
				return 1;
			}
		}
	}

	return 1;

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

float *Ctsrv::get_average_bars(int _ix)  {
	//Q_UNUSED(_ix);
	//return nullptr;
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		return udpclients[ix]->get_average_bars();
	}

	return nullptr;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

float *Ctsrv::get_bars(int _ix)  {
	//Q_UNUSED(_ix);
	//return nullptr;
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		return udpclients[ix]->get_bars();
	}

	return nullptr;
}


/*********************************************************************************************************************************

*********************************************************************************************************************************/

float Ctsrv::get_ftp(int _ix)  {
	//Q_UNUSED(_ix);
	//return 0.0f;
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		float f;
		f = udpclients[ix]->get_ftp();
		return f;
	}
	return -1.0f;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::get_handlebar_buttons(int _ix)  {
	//Q_UNUSED(_ix);
	//return 0;
	int i;
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		i = udpclients[ix]->get_handlebar_buttons();
	}
	else {
		i = 0;
	}
	return i;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

bool Ctsrv::is_client_connected(int _comport)  {
	//Q_UNUSED(_ix);
	//return 0;
	int ix;

	//std::vector<CLIENT2> tcpclients;
	//UDPClient *udpclients[MAXUDPCLIENTS];

	if (udpclients)  {
		ix = _comport - UDP_SERVER_SERIAL_PORT_BASE + 1;
		if (udpclients[ix]) {
			if (udpclients[ix]->is_connected()) {
				return true;
			}
		}
	}
	return false;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

bool Ctsrv::is_paused(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		bool b = false;
		b = udpclients[ix]->is_paused();
		return b;
	}
	return false;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::reset_averages(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->reset_stats();
		return 0;
	}
	return 1;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::reset_client(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		udpclients[ix]->reset();
		return 0;
	}
	return 1;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::rx(int _ix, unsigned char *buf, int buflen)  {
	//Q_UNUSED(_ix);
	//Q_UNUSED(buf);
	//Q_UNUSED(buflen);
	int ix;

	if (udpclients) {
		ix = _ix - UDP_SERVER_SERIAL_PORT_BASE;
		if (udpclients[ix]) {

			int status;
			status = udpclients[ix]->rx(buf, buflen);

			//bp = 1;
		}
	}

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::send(int _ix, const unsigned char *_str, int _len, bool _flush)  {
	//int i=0;
	//int ix;
	//int n;
	//unsigned char c;
	Q_UNUSED(_ix);
	Q_UNUSED(_str);
	Q_UNUSED(_len);
	Q_UNUSED(_flush);


	/*
	ix = _ix + 1 - SERVER_SERIAL_PORT_BASE;
	n = (int)tcpclients.size();

	if (ix >= n)  {
		return 0;
	}

	tcpclients[ix].mysend(_str, _len, _flush);
	*/

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_ergo_mode(int _ix, int _fw, int _rrc, float _manwatts)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	int status;


	if (udpclients[ix]) {
		status = udpclients[ix]->set_ergo_mode(_fw, _rrc, _manwatts);
	}

	return status;

}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void Ctsrv::set_export(int _ix, const char *_dbgfname)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		udpclients[ix]->set_export(_dbgfname);
	}
	return;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void Ctsrv::set_file_mode(int _ix, bool _mode)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		udpclients[ix]->set_file_mode(_mode);
	}

	return;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_ftp(int _ix, float _ftp)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->set_ftp(_ftp);
		return 0;
	}
	return 1;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_hr_bounds(int _ix, int _minhr, int _maxhr, bool _beepEnabled)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		udpclients[ix]->set_hr_bounds(_minhr, _maxhr, _beepEnabled);
		return 0;
	}
	return 1;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_paused(int _ix, bool _paused)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->set_paused(_paused);
		return 0;
	}

	return 1;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_slope(int _ix, float _bike_kgs, float _person_kgs, int _drag_factor, float _grade)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->set_slope(_bike_kgs, _person_kgs, _drag_factor, _grade);
		return 0;
	}
	return 1;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::start_cal(int _ix)  {
	int ix, status;

	ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		status = udpclients[ix]->start_cal();
	}

	return status;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void Ctsrv::txx(unsigned char *b, int n)  {
	Q_UNUSED(b);
	Q_UNUSED(n);

	return;
}

// rm1 compatibility---


#if TIMER_METH==1

	/*********************************************************************************************************************************
		10 ms timer
	*********************************************************************************************************************************/

	void Ctsrv::timer_slot() {
		int i;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}
#elif TIMER_METH==2

	/*********************************************************************************************************************************
		10 ms timer used by QBasicTimer
	*********************************************************************************************************************************/

	void Ctsrv::timerEvent(QTimerEvent* event)  {
		//Q_UNUSED(event);
		int i;
		int id;

		id = event->timerId();


		bp = 3;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}
#elif TIMER_METH==3

	/*********************************************************************************************************************************
		10 ms timer used by QBasicTimer
	*********************************************************************************************************************************/

	void Ctsrv::timerEvent(QTimerEvent* event)  {
		//Q_UNUSED(event);
		int i;
		int id;

		id = event->timerId();


		bp = 3;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}

#elif TIMER_METH==4

	void Ctsrv::run()  {
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
		timer->start(10);
		return;
	}

	/*********************************************************
		10 ms timer
	*********************************************************/

	void Ctsrv::timer_slot() {
		int i;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}
#elif TIMER_METH==5
	/*********************************************************
		10 ms timer
	*********************************************************/

	void Ctsrv::timer_slot() {
		int i;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}
#elif TIMER_METH==6
	/****************************************************************************
		10 ms timer
	****************************************************************************/

	void Ctsrv::timer_slot() {
		int i;
		//qDebug() << "timer_slot";

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}

	/****************************************************************************

	****************************************************************************/

	void Ctsrv::about_to_quit()  {
		qDebug() << "about_to_quit";
	}

	/****************************************************************************

		http://stackoverflow.com/questions/2150488/using-a-qt-based-dll-in-a-non-qt-application

	****************************************************************************/

	void Ctsrv::onStarted()  {
		//xxx
		qDebug() << "onStarted";
		//app->exec();									// QCoreApplication::exec: Must be called from the main thread
		//qDebug() << "app->exec() finished";

#if 0
		if (QCoreApplication::instance() == NULL)  {
			try  {
				qDebug() << "os1";
				app = new QCoreApplication(argc, argv);		// WARNING: QApplication was not created in the main() thread.
				qDebug() << "os2";
				connect(app, SIGNAL(aboutToQuit()), this, SLOT(about_to_quit()));
				qDebug() << "os3";
				timer = new QTimer(app);
				qDebug() << "os4";
				connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
				qDebug() << "os5";
				timer->start(10);
				qDebug() << "os6";
				app->exec();
				qDebug() << "back from app->exec()";
			}
			catch (...) {
				bp = 6;
				/*
				qFatal("Error <unknown> sending event %s to object %s (%s)",
					typeid(*event).name(), qPrintable(receiver->objectName()),
					typeid(*receiver).name());
				*/
			}
		}					// if (QCoreApplication::instance() == NULL)  {
#endif


		/*
		//QCoreApplication *x = QCoreApplication::instance();
		//Qt::HANDLE h = QThread::currentThreadId();

		qDebug()<<"onStarted thread id = " << QThread::currentThreadId();

		//Ctsrv:: thread id =  0x1d18
		//Ctsrv:: newly created thread id =  0x1d18
		//onStarted thread id =  0x1ec8

		instances++;

		if (!in_qt_creator)  {
			app = new QCoreApplication(argc, argv);
			app->exec();
		}

		if (instances==1)  {

			timer = new QTimer(this);	// QObject: Cannot create children for a parent that is in a different thread.
												// (Parent is MYQT::Ctsrv(0x596c48), parent's thread is QThread(0x57fac8),
												// current thread is QThread(0x5a07b8)
												// in QMainWindow, this line just creates it in the parent thread and keeps going

			connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
			timer->start(10);
		}
		else  {
			qDebug() << "instances = " << instances;
			bp = 1;
		}
		*/
	}												// onStarted()

	/****************************************************************************

	****************************************************************************/

	void Ctsrv::run()  {
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
		timer->start(10);
		return;
	}

#elif TIMER_METH==8

	/****************************************************************************

	****************************************************************************/

	void Ctsrv::onStarted()  {

		qDebug()<<"onStarted thread id = " << QThread::currentThreadId();

		instances++;

		if (instances==1)  {
												// Qt error 2
			timer = new QTimer(this);	// QObject: Cannot create children for a parent that is in a different thread.
												// (Parent is MYQT::Ctsrv(0x596c48), parent's thread is QThread(0x57fac8),
												// current thread is QThread(0x5a07b8)
												// in QMainWindow, this line just creates it in the parent thread and keeps going

			connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
			timer->start(10);
		}
		else  {
			qDebug() << "instances = " << instances;
			bp = 1;
		}
	}												// onStarted()
	/*********************************************************
		10 ms timer
	*********************************************************/

	void Ctsrv::timer_slot() {
		int i;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}
#endif

}						// namespace
