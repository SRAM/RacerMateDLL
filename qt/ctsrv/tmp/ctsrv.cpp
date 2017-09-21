
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostInfo>
//#include <QAbstractSocket>

#ifdef WIN32
	#include <assert.h>
#endif

//#include <comdefs.h>
//#include <comutils.h>

#include "worker.h"
#include "ctsrv.h"


namespace MYQT  {

	QCoreApplication * Ctsrv::m_application = NULL;
	//int Ctsrv::argc = 1;
	//char * Ctsrv::argv[] = {"Ctsrv", NULL};


#ifndef NEWAGE
//static inline qint32 ArrayToInt(QByteArray source);
//QThread * Ctsrv::thread = NULL;
#endif

int Ctsrv::instances = 0;

/**********************************************************************

**********************************************************************/

#ifndef NEWAGE
Ctsrv::Ctsrv(QObject *_parent) : QThread(_parent) {

	parent = _parent;

	int status;

	throw("bad Ctsrv constructor");

	listen_port = 9072;

	status = init();
	if (status != 0) {
		throw 100;
	}


}                       // constructor
#endif

/**********************************************************************
	in vs
		parent is null
		this (Ctsrv) is a subclass of QObject

	in qt creator
		parent is a MainWindow ( QObject * )
		this (Ctsrv) is a subclass of QObject

**********************************************************************/

Ctsrv* Ctsrv::m_instance = 0;

#ifdef NEWAGE
Ctsrv::Ctsrv(int _listen_port, int _broadcast_port, QObject *_parent) : QObject(_parent) {
#else
Ctsrv::Ctsrv(int _broadcast_port, int _listen_port, bool _ip_discover, QObject *_parent) : QThread(_parent) {
#endif


	parent = _parent;
	qDebug() << this->metaObject()->className() << ": thread id = " << QThread::currentThreadId();
	Q_ASSERT(!m_instance);
	m_instance = this;

	int argc = 1;
	m_application = new QCoreApplication(argc, 0);


#ifndef NEWAGE
	int status;
	bcport = _broadcast_port;
	listen_port = _listen_port;
	ip_discover = _ip_discover;
	//udp = _udp;

	status = init();
	if (status != 0) {
		throw 100;
	}
	connect(this, SIGNAL(started()), this, SLOT(onStarted()), Qt::DirectConnection);
	start();

#else

	//if (QCoreApplication::instance() == NULL)  {
	//	m_application = new QCoreApplication(argc, argv);
	//}

	m_workerThread = new QThread(this);

	//Worker *worker = new Worker(this, _listen_port, _broadcast_port);
	m_worker = new Worker(this, _listen_port, _broadcast_port);
	m_worker->moveToThread(m_workerThread);

	//void data_slot(int _ix, MYQT::QT_DATA *_data);
	//void ss_slot(int _ix, MYQT::qt_SS::SSD *_ssd);
	//void rescale_slot(int _ix, int _maxforce);
	//void connected_to_trainer_signal(int, bool);

	//connect(&thread, SIGNAL(data_signal(int, MYQT::QT_DATA *_data)), this, SLOT(int, MYQT::QT_DATA *_data));

	//connect(&thread, SIGNAL(started()), worker, SLOT(onStarted()));

	connect(m_workerThread, SIGNAL(started()), m_worker, SLOT(started()));
	connect(m_worker, SIGNAL(finished()), m_workerThread, SLOT(quit()));

	//connect(worker, &Worker::resultReady, this, &Ctsrv::handleResults, Qt::DirectConnection);
	qDebug() << "   Ctsrv starting thread";
	m_workerThread->start();

#endif

	qDebug() << "returning from constructor";

}


/**********************************************************************
	destructor
**********************************************************************/

Ctsrv::~Ctsrv() {
#ifdef NEWAGE
	qDebug() << "   Ctsrv destructor";
	m_workerThread->quit();
	m_workerThread->wait();
	delete m_application;
	qDebug() << "   Ctsrv destructor finished";
#else
	int rc = 0;
	int i;

	qint64 start;
	contin = false;

	start = QDateTime::currentMSecsSinceEpoch();
	bp = 0;

	while (running) {
		if ((QDateTime::currentMSecsSinceEpoch() - start) >= 2000) {
			rc = 1;
			break;
		}
		bp++;
		QThread::usleep(100000);
	}
	bp = 1;

	for (i = 0; i < MAXCLIENTS; i++) {
		delete udpclients[i];
		udpclients[i] = 0;
	}


	if (bcsocket != NULL) {
		bcsocket->close();
		delete bcsocket;
	}

	if (udp_server_socket) {
		//udp_server_socket->disconnect();
		udp_server_socket->close();
	}
#endif

//	int i;

	delete at;

	fclose(logstream);
	logstream = NULL;

	bp = 0;
	instances--;

}

#ifdef NEWAGE

/**********************************************************************

**********************************************************************/

void Ctsrv::data_slot(int _ix, MYQT::QT_DATA *_data)  {
	emit(data_signal(_ix, _data));
	return;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::ss_slot(int _ix, MYQT::qt_SS::SSD *_ssd)  {
	emit(ss_signal(_ix, _ssd));
	return;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::rescale_slot(int _ix, int _maxforce)  {
	emit(rescale_signal(_ix, _maxforce));
	return;
}

#else

/**********************************************************************

**********************************************************************/

int Ctsrv::init(void) {
	int status;
	int i;
	//char str[256];

	if (QCoreApplication::instance() == NULL)  {
		in_qt_creator = false;
	}
	else  {
		in_qt_creator = true;
	}

	contin = false;
	running = false;

	msgcnt = 0;
	curid = 0;
	udp_server_socket = 0;

	for (i = 0; i < MAXCLIENTS; i++) {
		udpclients[i] = new UDPClient(i, this);

		connect(udpclients[i], SIGNAL(ss_signal(int, qt_SS::SSD*)), this, SLOT(ss_slot(int, qt_SS::SSD*)));
		connect(udpclients[i], SIGNAL(data_signal(int, QT_DATA*)), this, SLOT(data_slot(int, QT_DATA*)));

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

	/*
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
	timer->start(10);
	*/


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

#endif


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

#ifndef NEWAGE

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
	QSocketDevice
**********************************************************************/

int Ctsrv::create_udp_server_socket(void)  {
	bool b;

#ifdef _DEBUG
	qDebug() << "Ctsrv::create_udp_server_socket(): thread id = " << QThread::currentThreadId();
#endif

	udp_server_socket = new QUdpSocket(this);
	b = udp_server_socket->bind(QHostAddress::AnyIPv4, listen_port);
	if (!b) {
		qFatal("error creating Ctsrv socket 1");
	}

	QAbstractSocket::SocketState s = udp_server_socket->state();

	switch(s)  {
		case QAbstractSocket::UnconnectedState:		// 0	The socket is not connected.
			qDebug() << "socket state = UnconnectedState";
			break;
		case QAbstractSocket::HostLookupState:			//	1	The socket is performing a host name lookup.
			qDebug() << "socket state = HostLookupState";
			break;
		case QAbstractSocket::ConnectingState:			//	2	The socket has started establishing a connection.
			qDebug() << "socket state = ConnectingState";
			break;
		case QAbstractSocket::ConnectedState:			//	3	A connection is established.
			qDebug() << "socket state = ConnectedState";
			break;
		case QAbstractSocket::BoundState:				//	4	The socket is bound to an address and port.
			qDebug() << "socket state = BoundState";
			break;
		case QAbstractSocket::ClosingState:				//	6	The socket is about to close (data may still be waiting to be written).
			qDebug() << "socket state = ClosingState";
			break;
		case QAbstractSocket::ListeningState:			//	5	For internal use only.
			qDebug() << "socket state = ListeningState";
			break;
		default:
			qDebug() << "socket state = default";
			break;
	}

	Q_ASSERT(s == QAbstractSocket::BoundState);

	//connect(udp_server_socket, SIGNAL(connected()), this, SLOT(newConnection()));                   // never gets there
	connect(udp_server_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(udp_server_socket, SIGNAL(hostFound()), this, SLOT(hostFound()));
	connect(udp_server_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	// these have errors:

	//connect(udp_server_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState socketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState socketState)));
	//connect(udp_server_socket, SIGNAL(error(QAbstractSocket::SocketError socketError)), this, SLOT(error(QAbstractSocket::SocketError socketError)));

	return 0;
}										// int Ctsrv::create_udp_server_socket(void)  {



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
	qDebug() << "udp Ctsrv::disconnected slot";
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

void Ctsrv::ss_slot(int _id, qt_SS::SSD *_ssd) {
	emit ss_signal(_id, _ssd);

	return;
}

/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::data_slot(int _id, QT_DATA *_data) {
	emit data_signal(_id, _data);

	return;
}
/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::rescale_slot(int _id, int _maxforce) {
	emit rescale_signal(_id, _maxforce);

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
#ifdef TESTING
/**********************************************************************
	client slot
**********************************************************************/

void Ctsrv::testing_slot(int _id, TESTDATA *_data) {
	emit testing_signal(_id, _data);
	return;
}
#endif




/*********************************************************************************************************************************
	when the Ctsrv is udp
*********************************************************************************************************************************/

void Ctsrv::processPendingDatagrams(void) {
	udpclients[0]->readDatagram(udp_server_socket);
}                          // processPendingDatagrams()

// slots---

/*********************************************************************************************************************************

*********************************************************************************************************************************/

//void Ctsrv::start(void)  {
//	return;
//}


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
#endif					// #ifdef NEWAGE


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


#ifdef NEWAGE

//xxx

/****************************************************************************

****************************************************************************/

void Ctsrv::stop()  {
	return;
}

/**********************************************************************

**********************************************************************/

qt_SS::BARINFO *Ctsrv:: get_barinfo(int i) {
	Q_UNUSED(i);
	//return udpclients[0]->ss->get_barinfo(i);
	return 0;
}                          // get_barinfo(int i)

/**********************************************************************

**********************************************************************/

void Ctsrv::setStarted(bool value) {
	Q_UNUSED(value);
}

/**********************************************************************

**********************************************************************/

void Ctsrv::setFinished(bool value) {
	Q_UNUSED(value);
}
/**********************************************************************

**********************************************************************/

void Ctsrv::setPaused(bool value) {
	Q_UNUSED(value);
}

#else
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

	/*********************************************************
		10 ms timer
	*********************************************************/
/*
	void Ctsrv::timer_slot() {
		int i;

		for(i=0; i<MAXCLIENTS; i++)  {
			udpclients[i]->process();
		}
		return;
	}
*/

#ifdef NEWAGE
#else
	/****************************************************************************

	****************************************************************************/

	//xxx

	void Ctsrv::onStarted()  {
		qDebug() << "onStarted: " << this->metaObject()->className() << ": thread id = " << QThread::currentThreadId();

		/*
			http://stackoverflow.com/questions/10492480/starting-qtimer-in-a-qthread
			http://blog.qt.io/blog/2010/06/17/youre-doing-it-wrong/
		*/

		/*
		//QTimer* timer = new QTimer(0); //parent must be null
		timer = new QTimer(0); //parent must be null
		timer->setInterval(1000);
		timer->moveToThread(thread);
		connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
		timer->start();
		*/

		// this works:
		//int status = create_udp_server_socket();                                     // creates the Ctsrv socket and starts listening.
		//Q_ASSERT(status==0);

		started = true;
		return;
	}							// onStarted()
#endif

	/****************************************************************************

	****************************************************************************/

	void Ctsrv::stop()  {
		contin = false;
		while(running)  {
			QThread::usleep(100000);
		}
		//exit();
		return;
	}

	/****************************************************************************
		if we subclass QThread
	****************************************************************************/

	void Ctsrv::run()  {
		qDebug() << "run(): thread id = " << QThread::currentThreadId();

		int i;
		/*
		QTimer *timer2 = new QTimer(thread);
		connect(timer2, SIGNAL(timeout()), this, SLOT(timer_slot()));
		timer2->start(100000000);
		*/

		contin = true;
		running = true;

		while(contin)  {
			//count++;
			//emit(data_signal(count));

			for(i=0; i<MAXCLIENTS; i++)  {
				udpclients[i]->process();
			}
			QThread::usleep(10000);
		}

		running = false;
		return;
	}
#endif				// #ifndef NEWAGE



	/****************************************************************************

	****************************************************************************/

	/*
	void Ctsrv::handleResults(const QString &s)  {
		qDebug() << "   controller received from worker: " << s;
	}
	*/

}						// namespace
