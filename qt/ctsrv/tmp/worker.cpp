
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostInfo>

#include "worker.h"

namespace MYQT  {


/**************************************************************************

**************************************************************************/

Worker::Worker(Ctsrv *_ctsrv, qint16 _listen_port, qint16 _broadcast_port)  {
	ctsrv = _ctsrv;
	listen_port = _listen_port;
	broadcast_port = _broadcast_port;

	bp = 0;

	int status;

	udp_server_socket = 0;
	bc_datagram.clear();
	bcast_count = 0L;
	logstream = fopen("worker.log", "wt");
	last_ip_broadcast_time = 0L;
	bcsocket = NULL;

	//QStringList items;
	 //int done = 0;
	 bcaddr = "miney2.mselectron.net";

	bc_datagram.append("Racermate ");
	bc_datagram.append(QString::number(listen_port));
	bc_datagram.append(" ");
	bc_datagram.append(myip.toString());
	bc_datagram.append("\r\n");

	if (broadcast_port != -1) {
		bcsocket = new QUdpSocket(this);
		//#ifdef _DEBUG
		#if 1
		QHostInfo info = QHostInfo::fromName("miney2.mselectron.net");    // QHostInfo::localHostName()  );
		QList<QHostAddress> ip = info.addresses();
		bcaddr = ip[0];
		qDebug() << "bcaddr = " << bcaddr;
		#endif
	}                                // if (ip_discover)  {

	status = create_udp_server_socket();                                     // creates the Worker socket and starts listening.
	if (status != 0) {
		//return 1;
	}

	at = new Tmr("W");

	bctimer = 0;
	timer = 0;

	if (broadcast_port != -1) {
		// create a timer for broadcasting
		bctimer = new QTimer(this);
		connect( bctimer, SIGNAL(timeout()), this, SLOT(bc_timer_slot())  );
		bctimer->start(2000);
	}

	int i;

	for (i = 0; i < MAXCLIENTS; i++) {
		udpclients[i] = new UDPClient(i, this);

		//connect(udpclients[i], SIGNAL(data_signal(int, QT_DATA*)), this, SLOT(data_slot(int, QT_DATA*)));
		connect(udpclients[i], SIGNAL(data_signal(int, QT_DATA*)), this, SLOT(ctsrv->data_slot(int, QT_DATA*)));

		//connect(udpclients[i], SIGNAL(ss_signal(int, qt_SS::SSD*)), this, SLOT(ss_slot(int, qt_SS::SSD*)));
		connect(udpclients[i], SIGNAL(ss_signal(int, qt_SS::SSD*)), this, SLOT(ctsrv->ss_slot(int, qt_SS::SSD*)));

		//connect(udpclients[i], SIGNAL(rescale_signal(int, int)), this, SLOT(rescale_slot(int, int)));
		connect(udpclients[i], SIGNAL(rescale_signal(int, int)), this, SLOT(ctsrv->rescale_slot(int, int)));

#ifdef TESTING
		connect(udpclients[i], SIGNAL(testing_signal(int, TESTDATA*)), this, SLOT(testing_slot(int, TESTDATA*)));
#endif

		connect(this, SIGNAL(gradechanged_signal(int)), udpclients[i], SLOT(gradechanged_slot(int)));
		connect(this, SIGNAL(windchanged_signal(float)), udpclients[i], SLOT(windchanged_slot(float)));

	}

	return;
}										// constructor

/**************************************************************************

**************************************************************************/

Worker::~Worker(void)  {
	int i;

	delete at;
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

	fclose(logstream);

	return;
}


/**********************************************************************
	QSocketDevice
**********************************************************************/

int Worker::create_udp_server_socket(void)  {
	bool b;

#ifdef _DEBUG
	qDebug() << "Worker::create_udp_server_socket(): thread id = " << QThread::currentThreadId();
#endif

	udp_server_socket = new QUdpSocket(this);
	b = udp_server_socket->bind(QHostAddress::AnyIPv4, listen_port);
	if (!b) {
		qFatal("error creating Worker socket 1");
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
}										// int Worker::create_udp_server_socket(void)  {

// slots+++

/**********************************************************************

**********************************************************************/

void Worker::disconnected() {
	qDebug() << "udp Worker::disconnected slot";
	return;
}

/**********************************************************************

**********************************************************************/

void Worker::hostFound() {
	bp = 4;
	return;
}

/**********************************************************************

**********************************************************************/

void Worker::processPendingDatagrams(void) {
	udpclients[0]->readDatagram(udp_server_socket);
}                          // processPendingDatagrams()

/**************************************************************************
		Thread started signal, started in Ctsrv
**************************************************************************/

void Worker::started()  {
	qDebug() << "      worker onStarted()";
	timer = new QTimer(this);
	timer->setInterval(10);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));				// , Qt::DirectConnection

	timer->start();
	qDebug() << "      worker started timer";
}

/**************************************************************************

**************************************************************************/

void Worker::timeout()  {
	emit resultReady("tick");
}


#if 0

/**********************************************************************
	client slot
**********************************************************************/

void Worker::ss_slot(int _id, qt_SS::SSD *_ssd) {
	emit ss_signal(_id, _ssd);

	return;
}

/**********************************************************************
	client slot
**********************************************************************/

void Worker::data_slot(int _id, QT_DATA *_data) {
	emit data_signal(_id, _data);
	return;
}


/**********************************************************************
	client slot
**********************************************************************/

void Worker::rescale_slot(int _id, int _maxforce) {
	emit rescale_signal(_id, _maxforce);

	return;
}

#endif

/**********************************************************************
	app slot
**********************************************************************/

void Worker::gradechanged_slot(int _igradex10) {
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

void Worker::windchanged_slot(float _f) {
	Q_UNUSED(_f);

#ifdef _DEBUG
	float wind;
	wind = _f;
	qDebug() << qSetRealNumberPrecision(1) << "wind = " << wind;
#endif
	emit(windchanged_signal(_f));

	return;
}

// slots---

}			// namespace
