
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostInfo>
#include <QSettings>

//#ifdef WIN32
//#include <shlobj.h>
//#endif

#include "worker.h"
#include "ctsrv.h"

namespace RACERMATE  {

/**************************************************************************

**************************************************************************/

Worker::Worker(qint16 _listen_port, qint16 _bcport, QString _override_ip_string, int _debug_level, QObject* _parent ) : QObject(_parent) {
	listen_port = _listen_port;
	bcport = _bcport;
	debug_level = _debug_level;
	override_ip = _override_ip_string;

#ifdef _DEBUG
	const char *cptr = _override_ip_string.toUtf8().constData();
	Q_UNUSED(cptr);

	cptr = override_ip.toString().toUtf8().constData();
	oktobreak = 0;
	qDebug() << "      Worker(): tid = " << QThread::currentThreadId();
#endif

	seqnum = 0x0000;						// quint16
	cs = 0x00;								// quint8

	memset(devnums, 0, sizeof(devnums));
	bp = 0;
	int status;

	udp_server_socket = 0;
	bc_datagram.clear();

	/*
	unsigned char msg[] = {
		'R','M','P','C',
		0x0d, 0xf2,									// msg length, ~length
		0x0a,                               // bcst sect len
		0xfe,											// SECT_BCST, broadcast section id
		0,0,0,0,                            // msg[8..11] ip address, network order, msb first
		0,0,                                // msg[12..13] port #, net order, msb first
		0,0,                                // msg[14..15] bseqnum, msb first
		0,                                  // next sect offset, 0 is end of sects
		0                                   // msg[17] checksum
	};
	*/

	bc_datagram[0] = 'R';
	bc_datagram[1] = 'M';
	bc_datagram[2] = 'P';
	bc_datagram[3] = 'C';
	bc_datagram[4] = 0x0d;						// msg length, ~length
	bc_datagram[5] = (char)0xf2;				// msg length, ~length
	bc_datagram[6] = 0x0a;						// bcst sect len
	bc_datagram[7] = (char)0xfe;				// SECT_BCST, broadcast section id

	bc_datagram[8] = 0;
	bc_datagram[9] = 0;
	bc_datagram[10] = 0;
	bc_datagram[11] = 0;
	bc_datagram[12] = 0;
	bc_datagram[13] = 0;
	bc_datagram[14] = 0;
	bc_datagram[15] = 0;
	bc_datagram[16] = 0;
	bc_datagram[17] = 0;

	bcast_count = 0L;

	last_ip_broadcast_time = 0L;
#ifndef ALLINTFS
	bcsocket = NULL;
#endif

#ifdef _DEBUG
	//Q_ASSERT(override_ip.isNull());
#endif

	char curdir[256];
	QString s2 = QDir::currentPath();

#ifdef _DEBUG
	qDebug() << s2;
#endif

	//const char *cptr = s2.toUtf8().constData();
	strncpy(curdir, s2.toUtf8().constData(), sizeof(curdir)-1);

	char path[256];
	sprintf(path, "%s/main.ini", curdir);

	printf("ini file = %s\n", path);

	//if (!_override_ip_string.isEmpty())  {
	//	override_ip = _override_ip_string;
	//}
	QList<QNetworkAddressEntry> ipv4entries;
	//QList<QNetworkInterface> ifs;
	//QList<INTF> ifs;


	//qDebug() << "";
	qDebug("\n");

	foreach(const QNetworkInterface iface, QNetworkInterface::allInterfaces()) {
		if (iface.flags().testFlag(QNetworkInterface::IsUp) && (!iface.flags().testFlag(QNetworkInterface::IsLoopBack))) {

#ifdef _DEBUG
			qDebug("interface: %-32s", iface.name().toUtf8().constData());
#endif
			bp = iface.CanBroadcast;         // 4
			bp = iface.CanMulticast;         // 32
			bp = 7;
			foreach(QNetworkAddressEntry entry, iface.addressEntries()) {
				if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
#ifdef _DEBUG
					qDebug("   %-40s", entry.ip().toString().toUtf8().constData());
#endif
					ipv4entries.append(entry);
				}
				else  {
#ifdef _DEBUG
					qDebug("   %-40s   ignoring ipv6 protocols", entry.ip().toString().toUtf8().constData());
					bp = 3;
#endif
				}
			}
		}
		else  {
#ifdef _DEBUG
			bool b = iface.flags().testFlag(QNetworkInterface::IsUp);
			if (!b)  {
				qDebug("interface: %-32s   ignoring because interface is not up", iface.name().toUtf8().constData());
			}
			else  {
				b = iface.flags().testFlag(QNetworkInterface::IsLoopBack);
				if (b)  {
					qDebug("interface: %-32s   ignoring because interface is loopback", iface.name().toUtf8().constData());
				}
				else  {
					qDebug() << "unknown interface error";
					qFatal("worker");
				}
			}
#endif
			bp = 2;
		}
	}					// foreach(const QNetworkInterface iface, QNetworkInterface::allInterfaces()) {

#ifdef _DEBUG
	qDebug("\n");
#endif

	int i, n;

	n = ipv4entries.count();
	if (n==0)  {
		throw(1);						// no network
	}

	i = 0;

#ifdef ALLINTFS
	INTF intf;
	ifs.clear();
#endif

	foreach(const QNetworkAddressEntry entry, ipv4entries) {
		i++;
		qDebug("%s", entry.ip().toString().toUtf8().constData());

#ifdef ALLINTFS
		intf.bcaddr = entry.broadcast();
		intf.netmask = entry.netmask();
		//xxx
		ifs.append(intf);
#else
		bp = 1;

		if (!_override_ip_string.isEmpty())  {
			if (entry.ip().toString()==_override_ip_string)  {
				override_ip = entry.ip();
				netmask = entry.netmask();
				bcaddr = entry.broadcast();
			}
		}
		else  {

			if (n>1)  {
				if (entry.ip().toString().startsWith("192"))  {
					override_ip = entry.ip();
					netmask = entry.netmask();
					bcaddr = entry.broadcast();
				}
				else  {
					if (i==n && override_ip.isNull())  {
						override_ip = entry.ip();
						netmask = entry.netmask();
						bcaddr = entry.broadcast();
					}
				}
			}
			else  {
				override_ip = entry.ip();
				netmask = entry.netmask();
				bcaddr = entry.broadcast();
			}
		}
#endif

	}					// foreach(const QNetworkAddressEntry entry, ipv4entries)

	bp = 0;

#ifdef ALLINTFS
	if (ifs.size()==0)  {
		bp = 1;
		throw(2);						// no bcaddr
	}
#else
	if (bcaddr.isNull())  {
		bp = 1;
		throw(2);						// no bcaddr
	}
#endif

#ifdef _DEBUG
	#ifdef ALLINTFS
		n = ifs.size();
	#endif

	qDebug("\n");
	qDebug("ipv4 count = %d", ipv4entries.count());
	//qDebug("bcaddr = %s", bcaddr.toString().toUtf8().constData());
#endif

	/*
	unsigned char msg[] = {
		'R','M','P','C',
		0x0d, 0xf2,									// msg length, ~length
		0x0a,                               // bcst sect len
		0xfe,											// SECT_BCST, broadcast section id
		0,0,0,0,                            // msg[8..11] ip address, network order, msb first
		0,0,                                // msg[12..13] port #, net order, msb first
		0,0,                                // msg[14..15] bseqnum, msb first
		0,                                  // next sect offset, 0 is end of sects
		0                                   // msg[17] checksum
	};
	*/

	quint32 val;

	val = override_ip.toIPv4Address();

	bc_datagram[8] = (val >> 24) & 0x000000ff;
	bc_datagram[9] =  (val >> 16) & 0x000000ff;
	bc_datagram[10] =  (val >> 8) & 0x000000ff;
	bc_datagram[11] =  val & 0x000000ff;

	bc_datagram[12] = (listen_port >> 8) & 0x00ff;
	bc_datagram[13] = listen_port & 0x00ff;

	bp = 1;

	//xxx

	if (bcport != -1) {
#ifdef ALLINTFS
		//foreach(INTF intf, ifs)  {
		for(int j=0; j<ifs.size(); j++)  {
			ifs[j].bcsocket = new QUdpSocket(this);
			Q_ASSERT(ifs[j].bcsocket);
			qDebug() << "j = " << j;
			bp = j;
		}
#else
		bcsocket = new QUdpSocket(this);
		Q_ASSERT(bcsocket);
#endif
	}

	//#ifdef WIN32
#if 0
	wchar_t gstr[512] = { 0 };

	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, gstr);  // "C:\Program Files"
	program_dir = QString::fromWCharArray(gstr);
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, gstr);       // "C:\\Users\\larry\\Documents"
	personal_dir = QString::fromWCharArray(gstr);
	personal_dir += "\\RacerMate";                              // "C:\\Users\\larry\\Documents\\RacerMate"
	settings_dir = personal_dir + "\\Settings";                 // "C:\\Users\\larry\\Documents\\RacerMate\\Settings"
	debug_dir = personal_dir + "\\Debug";                       // "C:\\Users\\larry\\Documents\\RacerMate\\Debug"

#ifdef _DEBUG
	qDebug() << "program_dir = " << program_dir;
#endif

#else
	program_dir = ".";
	personal_dir = ".";
	settings_dir = ".";
	debug_dir = ".";
#endif

	if (!QDir(personal_dir).exists()) {
		QDir().mkdir(personal_dir);
	}
	if (!QDir(settings_dir).exists()) {
		QDir().mkdir(personal_dir);
	}
	if (!QDir(debug_dir).exists()) {
		QDir().mkdir(debug_dir);
	}

	bp = 1;
	int id = 0;

	if (debug_level > 0) {
		QString s = debug_dir + "/udp" + QString::number(id) + ".log";
		file.setFileName(s);

		if ( file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate) ) {
			if (file.isOpen()) {
				bp = 2;
			}
			if (debug_level > 1 && file.isOpen()) {
				qint64 size = file.size();
				file.seek(size);
				QTextStream stream(&file);

				QDateTime dt = QDateTime::currentDateTime();
				QString s2 = dt.toString("yyyy-MM-dd-hh-mm-ss");
				stream << endl << endl << file.fileName() << " starting at " << s2 << ", debug_level = " << debug_level << endl << endl;
				file.flush();
			}
		}
		else  {
			if (file.isOpen()) {
				bp = 2;
			}
		}
	}           // if (debug_level > 0)

	strcpy(logname, "worker.log");

	if (debug_level > 0) {
		logstream = fopen(logname, "wt");
	}
	else  {
		logstream = 0;
	}

	status = create_udp_server_socket();                                        // creates the Worker socket and starts listening.
	if (status != 0) {
		//return 1;
	}

	at = new Tmr("W");

	bctimer = 0;
	m_timer = 0;

	//xxx


	if (bcport != -1) {
		// create a timer for broadcasting
		bctimer = new QTimer(this);
		bctimer->setInterval(1000);
		connect( bctimer, SIGNAL(timeout()), this, SLOT(bc_timeout())  );
		//bctimer->start(2000);
	}

	m_timer = new QTimer(this);
	m_timer->setInterval(10);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));             // , Qt::DirectConnection, prepareResult()

	return;
}										// constructor()


/**********************************************************************

**********************************************************************/

int Worker::create_udp_server_socket(void) {
	bool b;

	udp_server_socket = new QUdpSocket(this);

	b = udp_server_socket->bind(QHostAddress::AnyIPv4, listen_port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
	if (!b) {
		qFatal("error creating Worker socket 1");
	}


#ifdef _DEBUG
	QAbstractSocket::SocketState s = udp_server_socket->state();

	switch (s) {
		case QAbstractSocket::UnconnectedState: {       // 0	The socket is not connected.
			qDebug() << "      socket state = UnconnectedState";
			break;
		}
		case QAbstractSocket::HostLookupState: {        //	1	The socket is performing a host name lookup.
			qDebug() << "      socket state = HostLookupState";
			break;
		}
		case QAbstractSocket::ConnectingState: {        //	2	The socket has started establishing a connection.
			qDebug() << "socket state = ConnectingState";
			break;
		}
		case QAbstractSocket::ConnectedState: {         //	3	A connection is established.
			qDebug() << "socket state = ConnectedState";
			break;
		}
		case QAbstractSocket::BoundState: {             //	4	The socket is bound to an address and port.
			//qDebug() << "      socket state = BoundState";
			break;
		}
		case QAbstractSocket::ClosingState: {           //	6	The socket is about to close (data may still be waiting to be written).
			qDebug() << "socket state = ClosingState";
			break;
		}
		case QAbstractSocket::ListeningState: {         //	5	For internal use only.
			qDebug() << "socket state = ListeningState";
			break;
		}
		default: {
			qDebug() << "socket state = default";
			break;
		}
	}
#endif

	Q_ASSERT(udp_server_socket->state() == QAbstractSocket::BoundState);

	connect(udp_server_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(udp_server_socket, SIGNAL(hostFound()), this, SLOT(hostFound()));
	start_time = QDateTime::currentMSecsSinceEpoch();
	connect(udp_server_socket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	// these have errors:
	//connect(udp_server_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState socketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState socketState)));
	//connect(udp_server_socket, SIGNAL(error(QAbstractSocket::SocketError socketError)), this, SLOT(error(QAbstractSocket::SocketError socketError)));

	return 0;
}

/**********************************************************************

**********************************************************************/

qt_SS::BARINFO *Worker:: get_barinfo(QString _key, int i) {
	Q_UNUSED(i);
	//qFatal("get_barinfo");
	return clients[_key]->ss->get_barinfo(i);
}                          // get_barinfo(int i)


// slots+++

/**********************************************************************

**********************************************************************/

void Worker::disconnected() {
#ifdef _DEBUG
	qDebug() << "      Worker::disconnected slot() executed";
#endif
	return;
}

/**********************************************************************

**********************************************************************/

void Worker::hostFound() {
	bp = 4;
	return;
}

/**********************************************************************
	udp_server_socket slot
**********************************************************************/

void Worker::processPendingDatagrams(void) {

	qint64 n;
	int i, size;
	int loops = 0;
	QString key;

	while (udp_server_socket->hasPendingDatagrams()) {
		QByteArray a;
		loops++;
		a.resize(udp_server_socket->pendingDatagramSize());
		n = udp_server_socket->readDatagram(a.data(), a.size(), &peeraddr, &peerport);         // peeraddr = 192.168.1.40, peerport = 41631, eg

		if (n <= 0) {
			qFatal("readDatagram error");
		}

		bytes_in += n;

		if (n<10)  {
			qFatal("n<10");
		}

		// devnum is at offset 8 and 9 (unless the packet is fragmented (can it happen that we don't get a devnum?))
		int devnum;
		devnum = a[8] << 8 | a[9];

#ifdef _DEBUG
		//Q_ASSERT(devnum==5678 || devnum==6666);
		if (devnum!=5678 && devnum!=6666)  {
			qFatal("devnum error");							// debugging, testing to see if this ever happens
		}
#endif

		//key = peeraddr.toString() + " " + QString::number(peerport) + " " + QString::number(devnum);
		key = peeraddr.toString() + " " + QString::number(peerport);
#ifdef _DEBUG
		if (!key.contains("192.168.1.40 ")) {
			qFatal("paper");
		}
#endif


		if (!clients.contains(key)) {
			// new client connecting, add it to the table
			size = clients.count();

			if (size == MAXCLIENTS) {
				return;                          // reject too many clients
			}

			//qDebug("key = %s", key.toStdString().c_str());

			qDebug("key = %s", key.toUtf8().constData());

			clients[key] = new UDPClient(key, debug_level, this);
			clients[key]->peeraddr = peeraddr;
			clients[key]->peerport = peerport;
			clients[key]->key = key;
			connect(
						clients[key],
						SIGNAL(connected_to_trainer_signal(QString, bool)),
						this,
						SLOT(connected_to_trainer_slot(QString, bool))
			);

			connect(
						this,
						SIGNAL(connected_to_trainer_signal(QString, bool)),
						Ctsrv::instance(),
						SLOT(connected_to_trainer_slot(QString, bool))
						,Qt::DirectConnection
			);

			connect(clients[key], SIGNAL(data_signal(QString, QT_DATA*)), Ctsrv::instance(), SLOT(data_slot(QString, QT_DATA*)));
			connect(clients[key], SIGNAL(ss_signal(QString, qt_SS::SSD*)), Ctsrv::instance(), SLOT(ss_slot(QString, qt_SS::SSD*)));
			connect(clients[key], SIGNAL(rescale_signal(QString, int)), Ctsrv::instance(), SLOT(rescale_slot(QString, int)));
			connect(clients[key], SIGNAL(ctlmsg_signal(QString, unsigned char *, int)), this, SLOT(ctlmsg_slot(QString, unsigned char *, int)));

		}					// if (!clients.contains(key))
		else  {
			if (clients[key]->nhbd.devnum==0)  {
				bp = 2;
			}
			bp = 1;
		}					// if (!clients.contains(key))

		for (i = 0; i < n; i++) {
			clients[key]->rxq[clients[key]->rxinptr] = a[i];
			clients[key]->rxinptr = (clients[key]->rxinptr + 1) % sizeof(clients[key]->rxq);
		}

	}					// while (udp_server_socket->hasPendingDatagrams()) {

#ifdef _DEBUG
	if (clients.count() == 2) {
		bp = 1;
	}
#endif

	// edge detect when the devnum comes in

	if (clients[key]->nhbd.devnum != 0x0000 && clients[key]->lastdevnum == 0x0000)  {
		clients[key]->lastdevnum = clients[key]->nhbd.devnum;
		qDebug() << "   key = " + key + ", setting lastdevnum = " + QString::number(clients[key]->nhbd.devnum);
	}

	return;

}                             // processPendingDatagrams()



/*********************************************************************************************************************************
	bc_timer slot
*********************************************************************************************************************************/

void Worker::bc_timeout() {
	//at->update();							// 3001 ms

	qint64 n;

	//xxx

#ifdef ALLINTFS
	if (ifs.size()==0)  {
		qFatal("no broadcast socket");

	}
#else
	if (!bcsocket)  {
		qFatal("no broadcast socket");

	}
#endif

#ifdef _DEBUG
	// qDebug() << "bcaddr = " << bcaddr.toString();
	//qDebug() << " bc_datagram = " << bc_datagram;
#endif

	/*
	bc_datagram.append("Racermate ");
	bc_datagram.append(QString::number(listen_port));
	bc_datagram.append(" ");
	bc_datagram.append(override_ip.toString());
	bc_datagram.append("\r\n");
	//todo: add sequence number and checksum and move this to bc_timeout()
	//seqnum++;
	//cs = ...
	*/

	/*
The new broadcast fmt. It decodes with the (almost) same ubermsg decoder.
It makes up a char array and sends it in one go so it stays in one udp pkt.

It has one section SECT_BCST in place of SECT_DEVNUM, so ubermsg needs to
allow it to be the only msg. Show me your decoder and I will fix it.




unsigned long s_node = ntohl(inet_addr("192.168.5.1"));
unsigned short s_port = (uint16_t)strtol("9072",NULL,10);


int        send_broadcast(int fd, unsigned long s_node, unsigned short s_port) {

	static unsigned short        bseqnum = 0;
	unsigned char csm = 0;
	int        i;

	unsigned char msg[] = {
		'R','M','P','C',
		0x0d, 0xf2,                        // msg length, ~length
		0x0a,                                        // bcst sect len
		0xfe,                                        // SECT_BCST, broadcast section id
		0,0,0,0,                                // msg[8..11] ip address, network order, msb first
		0,0,                                        // msg[12..13] port #, net order, msb first
		0,0,                                        // msg[14..15] bseqnum, msb first
		0,                                                // next sect offset, 0 is end of sects
		0                                                // msg[17] checksum
	};

	msg[8] = (s_node >> 24) & 0xff;        // our server ip address
	msg[9] = (s_node >> 16) & 0xff;
	msg[10] = (s_node >> 8) & 0xff;
	msg[11] = s_node & 0xff;

	msg[12] = (s_port >> 8) & 0xff;                // our server port #
	msg[13] = s_port & 0xff;                // our server port #

	msg[14] = (bseqnum >> 8) & 0xff;                // our server port #
	msg[15] = bseqnum & 0xff;                // our server port #

	csm = 0;
	for( i=0; i<17; i++) {
		csm += msg[i];                // checksum from start to sect 0
	}
	msg[17] = -csm  & 0xff;        // should add up to zero

	i = send(fd, msg, 18, 0);

	//salt to taste....
	if (i != 18) {
		fprintf(stderr, "Error sending broadcast: i= %d\n", i);
		return -1;
	}

	return 0;
}

	*/

	//bc_datagram.clear();
	//bc_datagram[8] = (char)0xaa;
	//bc_datagram.insert(8, (char)0xaa);

	seqnum++;

	/*
	unsigned char msg[] = {
		'R','M','P','C',
		0x0d, 0xf2,									// msg length, ~length
		0x0a,                               // bcst sect len
		0xfe,											// SECT_BCST, broadcast section id
		0,0,0,0,                            // msg[8..11] ip address, network order, msb first
		0,0,                                // msg[12..13] port #, net order, msb first
		0,0,                                // msg[14..15] bseqnum, msb first
		0,                                  // next sect offset, 0 is end of sects
		0                                   // msg[17] checksum
	};
	*/

	bc_datagram[14] = (seqnum>>8) & 0x00ff;
	bc_datagram[15] = seqnum & 0x00ff;
	cs = 0;

#ifdef _DEBUG
	///qDebug() << "seqnumber = " + seqnum;
#endif

	for(int i=0; i<16; i++)  {
		cs += bc_datagram[i];
	}

	cs = (-cs) & 0xff;

	bc_datagram[17] = cs;

#ifdef ALLINTFS
	n = ifs.size();

	//foreach(INTF intf, ifs)  {
	for(int i=0; i<n; i++)  {
		if (ifs[i].bcsocket)  {
			n = ifs[i].bcsocket->writeDatagram(bc_datagram, ifs[i].bcaddr, bcport);            // miney2.mselectron.net, 9071
			if (n <= 0) {
				qFatal("broadcast write error");
			}
		}
	}

#else
	n = bcsocket->writeDatagram(bc_datagram, bcaddr, bcport);            // miney2.mselectron.net, 9071

	if (n <= 0) {
		qFatal("broadcast write error");
	}
#endif

	return;
}                          // bc_timeout()


// slots---


/**************************************************************************

**************************************************************************/

void Worker::timeout() {                  // see prepareResult(), trackshun
	//at->update();								// 10.00 ms

	foreach(QString key, clients.keys() )  {
		clients.value(key)->process();
	}
	return;
}                    // timeout()

/**************************************************************************

**************************************************************************/

void Worker::start() {
	if (QThread::currentThread() != thread()) {
		QMetaObject::invokeMethod(this, "start");
		return;
	}

#ifdef _DEBUG
	qDebug() << "      Worker::start(): calling m_timer->start()";
#endif

	if (m_timer) {
		m_timer->start();
	}

	if (bctimer) {
		bctimer->start();
	}

}                       // start() slot

/**************************************************************************
	don't do this in a destructor!!!!!
**************************************************************************/

void Worker::stop() {
	if (QThread::currentThread() != thread()) {
		QMetaObject::invokeMethod(this, "stop");
		return;
	}

	if (m_timer) {
		m_timer->stop();
	}

	if (bctimer) {
		bctimer->stop();
	}

	delete at;

	foreach(QString key, clients.keys() )  {
		delete(clients.value(key));
	}
	clients.clear();

	if (udp_server_socket) {
		udp_server_socket->close();
		delete udp_server_socket;
		udp_server_socket = 0;
	}

#ifdef ALLINTFS
	//foreach(INTF intf, ifs)  {
	for(int i=0; i<ifs.size(); i++)  {
		if (ifs[i].bcsocket != NULL) {
			ifs[i].bcsocket->close();
			delete ifs[i].bcsocket;
			ifs[i].bcsocket = 0;
		}
	}
	ifs.clear();
#else
	if (bcsocket != NULL) {
		bcsocket->close();
		delete bcsocket;
		bcsocket = 0;
	}
#endif

	if (logstream) {
		fclose(logstream);
	}

	emit finished();
}           // stop() slot




/****************************************************************************

****************************************************************************/
void Worker::ctlmsg_slot(QString _key, unsigned char *_buf, int _n) {
	int status = -1;

	// should this just go into a message queue? does this block in case another client emits this signal while this is
	// still running?

	if (udp_server_socket)  {
		if (peerport != 0) {
			if (clients.contains(_key))  {
				status = udp_server_socket->writeDatagram((const char*)_buf, _n, clients[_key]->peeraddr, (quint16)clients[_key]->peerport);
			}
		}				// if (peerport != 0) {
		Q_ASSERT(status > 0);
	}					// if (udp_server_socket)  {
	return;
}                 // ctlmsg_slot

/**********************************************************************

**********************************************************************/

void Worker::connected_to_trainer_slot(QString _key, bool _b) {

	if (_b == false) {
		bp = 3;
		// remove this trainer from clients
		if (!clients.contains(_key)) {
			qFatal("wtf?");
		}
		delete clients[_key];
		clients[_key] = 0;
		clients.remove(_key);
#ifdef _DEBUG
		qDebug() << "worker emitting connected to trainer (false) signal to ctsrv, key = " << _key;
#endif
	}
	else  {
#ifdef _DEBUG
		qDebug() << "worker emitting connected to trainer (true) signal to ctsrv, key = " << _key;
#endif
	}


	emit connected_to_trainer_signal(_key, _b);					// pass the signal up to ctsrv
	return;
}

/**********************************************************************

**********************************************************************/

void Worker::set_devnums()  {
	//int devnum;
	char str[32];

	foreach(QString key, clients.keys() )  {
		//clients.value(key)->process();
		sprintf(str, "UDP-%d ", clients.value(key)->nhbd.devnum);
		strcat(devnums, str);
	}
	int len = strlen(devnums);
	if (devnums[len-1]==' ')  {
		devnums[len-1] = 0;
	}
	bp = 0;

	return;
}

/**********************************************************************

**********************************************************************/

bool Worker::have(const char *_devid)  {

	if (clients.contains(_devid))  {
		return true;
	}
	return false;
}                                // int Worker::create_udp_server_socket(void)  {


}           // namespace RACERMATE
