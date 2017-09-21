
#include <QNetworkInterface>
#include <QHostInfo>
#include <QSettings>


#include "ctsrvworker.h"
#include "ctsrv.h"

namespace RACERMATE  {

/**************************************************************************

**************************************************************************/

ctsrvWorker::ctsrvWorker(qint16 _listen_port, qint16 _bcport, QString _override_ip_string, int _debug_level, QObject* _parent ) : QObject(_parent) {
	int status;
	int n;
	//INTERFACE intf;
	char curdir[256];
	QString s2 = QDir::currentPath();
	char path[256];
	//QList<QNetworkAddressEntry> ipv4entries;

	// throw(99);			// ctsrv catches this

	listen_port = _listen_port;
	bcport = _bcport;
	debug_level = _debug_level;
	override_ip = _override_ip_string;
	//qDebug() << override_ip;				// ""

	at = new Tmr("W");

	if (debug_level > 0) {
		sdirs = new SDIRS(debug_level, "rmserver");
		status = sdirs->get_rc();
		switch(status)  {
			case 0:
				break;
			case -1:
				throw("sdirs error 1");			// ctsrv catches this
			case -2:
				throw("sdirs error 2");			// ctsrv catches this
			case -3:
				throw("sdirs error 3");			// ctsrv catches this
			default:
				throw("sdirs error 101");		// ctsrv catches this
		}

		QString s = sdirs->get_debug_dir() + "ctsrvworker.log";
		strncpy(logname, s.toUtf8().constData(), sizeof(logname)-1);
		logstream = fopen(logname, "wt");
		QDateTime dt = QDateTime::currentDateTime();
		s = dt.toString("yyyy-MM-dd-hh-mm-ss");
		fprintf(logstream, "%s starting at %s, debug_level = %d\n\n", logname, s.toUtf8().constData(), debug_level);
		fflush(logstream);
	}           // if (debug_level > 0)


	strncpy(curdir, s2.toUtf8().constData(), sizeof(curdir) - 1);
	sprintf(path, "%s/main.ini", curdir);
	//printf("ini file = %s\n", path);

	// gathers ipv4entries (QNetworkAddressEntry) list

	n = scan_interfaces();

	if (n==-1)  {
		throw(1);                  // no network, ctsrv catches
	}


	if (bcport != -1) {
		bcsocket = new QUdpSocket(this);
		Q_ASSERT(bcsocket);
	}


	if (debug_level > 1)  {
		fprintf(logstream, "creating server socket\n");
		fflush(logstream);
	}

	status = create_udp_server_socket();                                        // creates the ctsrvWorker socket and starts listening.
	if (status != 0) {
		if (debug_level > 0)  {
			fprintf(logstream, "status = %d\n", status);
			fflush(logstream);
		}
	}



	if (bcport != -1) {
		bctimer = new QTimer(this);
		bctimer->setInterval(BCTIMEOUT);
		connect( bctimer, SIGNAL(timeout()), this, SLOT(bc_timeout())  );
	}

	m_timer = new QTimer(this);
	m_timer->setInterval(10);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));             // , Qt::DirectConnection, prepareResult()

	return;
}                    // constructor()



/**********************************************************************

**********************************************************************/

int ctsrvWorker::create_udp_server_socket(void) {
	bool b;

	udp_server_socket = new QUdpSocket(this);

	b = udp_server_socket->bind(QHostAddress::AnyIPv4, listen_port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
	if (!b) {
		throw("error creating ctsrvWorker socket 1");			// ctsrv catchs
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

qt_SS::BARINFO *ctsrvWorker:: get_barinfo(QString _key, int i) {
	Q_UNUSED(i);
	return clients[_key]->ss->get_barinfo(i);
}

/**********************************************************************

**********************************************************************/

//void ctsrvWorker::calc_tp(QString _key, unsigned long _ms, float _watts)  {
//}                          // get_barinfo(int i)


// slots+++

/**********************************************************************

**********************************************************************/

void ctsrvWorker::disconnected() {
#ifdef _DEBUG
	//qDebug() << "      ctsrvWorker::disconnected slot() executed";
#endif
	return;
}

/**********************************************************************

**********************************************************************/

void ctsrvWorker::hostFound() {
	bp = 4;
	return;
}

/**********************************************************************
	udp_server_socket slot
**********************************************************************/

void ctsrvWorker::processPendingDatagrams(void) {

	//throw("testing throws");			// throws from slots not caught

	qint64 n;
	int i, size;
	QString key;

	static int calls = 0;
	int loops = 0;
	calls++;

#ifdef _DEBUG
	static QString lastkey;
	static int lastn = -1;
	Q_UNUSED(lastn);
	//static int grows = 0;


	if (calls==2)  {
		bp = 4;
	}
#endif

//	if (debug_level > 1)  {
//		fprintf(logstream, "\nppd, calls = %d\n", calls);
//		fflush(logstream);
//	}



	while (udp_server_socket->hasPendingDatagrams()) {
		loops++;

		QByteArray a;
		a.resize(udp_server_socket->pendingDatagramSize());
		n = udp_server_socket->readDatagram(a.data(), a.size(), &peeraddr, &peerport);         // peeraddr = 192.168.1.40, peerport = 41631, eg


		if (n <= 0) {
			if (debug_level > 1)  {
				fprintf(logstream, "readDatagram error, n = %d, loops = %d\n", (int)n, loops);
				fflush(logstream);
			}
			//throw("readDatagram error");			// throws from slots not caught
		}

		key = peeraddr.toString() + " " + QString::number(peerport);
		bytes_in += n;

		//nca+++
	   //	only add new client key if pkt passes tests
		// otherwise you will fill the client list with every random new addr:port
		// This assumes pkts fragmented in radio or wifi, so one starts with RMCT


		if (!clients.contains(key))  {
			if (debug_level > 1)  {
				fprintf(logstream, "trying to add client, key %s\n", key.toUtf8().constData());
				fflush(logstream);
			}

			if (
				(n >= 6) &&								// at least 6 bytes
				(a[0] == 'R') &&						// starts with RMCT
				(a[1] == 'M') &&
				(a[2] == 'C') &&
				(a[3] == 'T') &&
				(a.at(4) >= 2) &&
				(a.at(4) < 120) &&
				(((a[4] ^ a[5])&0xff) == 0xff)				// len and ~len match
			) {
				if (debug_level > 1)  {
					fprintf(logstream, "ppd, adding client\n");
					fflush(logstream);
				}

				// new client connecting, add it to the table
				size = clients.count();

//				if (debug_level > 1)  {
//					fprintf(logstream, "ppd3, clients.size() = %d\n", size);
//					fflush(logstream);
//				}


				if (size == MAXCLIENTS) {
					if (debug_level > 1)  {
						fprintf(logstream, "ppd, too many clients, clients.size() = %d\n", size);
						fflush(logstream);
					}
					return;                          // reject too many clients
				}

				if (debug_level > 1)  {
					fprintf(logstream, "\ncreating client %s\n", key.toUtf8().constData());
					fflush(logstream);
				}

				clients[key] = new UDPClient(key, debug_level, this);				// clients grows here
				if (debug_level > 1)  {
					fprintf(logstream, "client created\n");
					fflush(logstream);
				}

				clients[key]->peeraddr = peeraddr;
				clients[key]->peerport = peerport;
				clients[key]->key = key;

				if (debug_level > 1)  {
					fprintf(logstream, "connect 1\n");
					fflush(logstream);
				}

				connect(
					clients[key],
					SIGNAL(connected_to_trainer_signal(QString, bool)),
					this,
					SLOT(connected_to_trainer_slot(QString, bool))
					);

				if (debug_level > 1)  {
					fprintf(logstream, "connect 2\n");
					fflush(logstream);
				}

				connect(
					this,
					SIGNAL(connected_to_trainer_signal(QString, bool)),
					Ctsrv::instance(),
					SLOT(connected_to_trainer_slot(QString, bool))
					, Qt::DirectConnection
					);

				if (debug_level > 1)  {
					fprintf(logstream, "connect 3\n");
					fflush(logstream);
				}
				connect(clients[key], SIGNAL(data_signal(QString, QT_DATA*)), Ctsrv::instance(), SLOT(data_slot(QString, QT_DATA*)));
				if (debug_level > 1)  {
					fprintf(logstream, "connect 4\n");
					fflush(logstream);
				}
				connect(clients[key], SIGNAL(ss_signal(QString, qt_SS::SSD*)), Ctsrv::instance(), SLOT(ss_slot(QString, qt_SS::SSD*)));
				if (debug_level > 1)  {
					fprintf(logstream, "connect 5\n");
					fflush(logstream);
				}
				connect(clients[key], SIGNAL(rescale_signal(QString, int)), Ctsrv::instance(), SLOT(rescale_slot(QString, int)));
				if (debug_level > 1)  {
					fprintf(logstream, "connect 6\n");
					fflush(logstream);
				}
				connect(clients[key], SIGNAL(ctlmsg_signal(QString, unsigned char *, int)), this, SLOT(ctlmsg_slot(QString, unsigned char *, int)));

				if (debug_level > 1)  {
					fprintf(logstream, "client %s fully connected\n\n", key.toUtf8().constData());
					fflush(logstream);
				}
			}				// if (norm's test)
			else  {
				//throw ("failed norm test");			// throws from slots not caught
				if (debug_level > 0)  {
					fprintf(logstream, "failed Norm test\n");
					int ii;
					fprintf(logstream, "ppd2, clients.size() = %d, n = %d, loops = %d, key = %s\n", clients.size(), (int)n, loops, key.toUtf8().constData());
					for(ii=0; ii<n; ii++)  {
						fprintf(logstream,  "%02x ", a.at(ii) & 0x00ff);
					}
					fprintf(logstream, "\n");
					fflush(logstream);
				}
				continue;				// keep looking
			}
		}              // if (!clients.contains(key))
		else {
//			if (clients[key]->nhbd.devnum == 0) {
//				bp = 2;
//			}
//			bp = 1;
		}              // if (!clients.contains(key))


		if (!clients.contains(key))  {
			continue;
		}

		for (i = 0; i < n; i++) {
			clients[key]->rxq[clients[key]->rxinptr] = a[i];
			clients[key]->rxinptr = (clients[key]->rxinptr + 1) % sizeof(clients[key]->rxq);
		}
	}              // while (udp_server_socket->hasPendingDatagrams()) {

	if (!clients.contains(key))  {
		if (debug_level > 1)  {
			fprintf(logstream, "ppd7\n");
			fflush(logstream);
		}
		return;
	}

	// edge detect when the devnum comes in

	if (clients[key]->nhbd.devnum != 0x0000 && clients[key]->lastdevnum == 0x0000) {
		clients[key]->lastdevnum = clients[key]->nhbd.devnum;
	}

	return;

}                             // processPendingDatagrams()



/*********************************************************************************************************************************
	bc_timer slot
*********************************************************************************************************************************/

void ctsrvWorker::bc_timeout() {
	//at->update();							// BCTIMEOUT ms

#ifdef _DEBUG
//	int dms;
//at->start();
#endif

	QElapsedTimer et;
	bool b;
	qint64 nanoseconds;
	Q_UNUSED(nanoseconds);
	et.start();
	//mutex.lock();
	b = mutex.tryLock(5);								// wait up to 5 ms for a lock
	if (b==false)  {
		bp = 1;
	}
	nanoseconds = et.nsecsElapsed();					// max was 1433 nanoseconds in my testing

#ifdef _DEBUG
	maxbcwait = qMax(maxbcwait, nanoseconds);
#endif

	//throw("bc throw");			// throws from slots not caught

	qint64 n;
	int i;

	//xxx
	if (bcsocket == 0) {
		if (debug_level > 1)  {
			fprintf(logstream, "no broadcast socket\n");
			fflush(logstream);
		}
		//throw("no broadcast socket");			// throws from slots not caught
	}


	//if (interfaces.size()==0)  {
	if (bcinfos.size() == 0) {
		if (override_broadcast_info.bc.isNull()) {
			if (debug_level > 1)  {
				fprintf(logstream, "no default broadcast addresses\n");
				fflush(logstream);
			}
			//throw("no default broadcast addresses");			// throws from slots not caught
		}

		if (debug_level > 1)  {
			fprintf(logstream, "no broadcast addresses\n");
			fflush(logstream);
		}
		//throw("no broadcast addresses");			// throws from slots not caught
	}

	seqnum++;

	if (!override_broadcast_info.bc.isNull()) {
		override_broadcast_info.dg[14] = (seqnum >> 8) & 0x00ff;
		override_broadcast_info.dg[15] = seqnum & 0x00ff;


		cs = 0;
		for (i = 0; i < 16; i++) {
			cs += override_broadcast_info.dg[i];
		}

		cs = (-cs) & 0xff;

		override_broadcast_info.dg[17] = cs;
		n = bcsocket->writeDatagram(override_broadcast_info.dg, override_broadcast_info.bc, bcport);            // miney2.mselectron.net, 9071
		if (n <= 0) {
			if (debug_level > 1)  {
				fprintf(logstream, "broadcast write error 1\n");
				fflush(logstream);
			}
			//throw("broadcast write error 1");			// throws from slots not caught
		}
		goto finis;
	}


	foreach(QString key, bcinfos.keys() ) {
		bcinfos[key].dg[14] = (seqnum >> 8) & 0x00ff;
		bcinfos[key].dg[15] = seqnum & 0x00ff;


		cs = 0;
		for (i = 0; i < 16; i++) {
			cs += bcinfos[key].dg[i];
		}

		cs = (-cs) & 0xff;

		bcinfos[key].dg[17] = cs;

		n = bcsocket->writeDatagram(bcinfos[key].dg, bcinfos[key].bc, bcport);            // miney2.mselectron.net, 9071
		if (n <= 0) {
			int err = bcsocket->error();
			switch (err) {
				case QAbstractSocket::ConnectionRefusedError: {
					bp = 1;
					break;
				}
				default: {
					bp = 2;
					break;
				}
			}

			if (debug_level > 1)  {
				fprintf(logstream, "broadcast write error 2\n");
				fflush(logstream);
			}
			//qDebug() << bcsocket->errorString();
			//throw("broadcast write error 2");			// throws from slots not caught

		}
	}


//	int sz = interfaces.size();
//	Q_UNUSED(sz);

/*
	for(i=0; i<sz; i++)  {
		if (ifs[i].bcsocket)  {
			n = ifs[i].bcsocket->writeDatagram(bc_datagram, ifs[i].bcaddr, bcport);            // miney2.mselectron.net, 9071
			if (n <= 0) {
				throw("broadcast write error");			// throws from slots not caught
			}
		}
	}
 */

finis:
	mutex.unlock();
#ifdef _DEBUG
//dms = at->stop();				// < 1 ms
#endif
	return;
}                          // bc_timeout()


// slots---


/**************************************************************************

**************************************************************************/

void ctsrvWorker::timeout() {                  // see prepareResult(), trackshun
	//at->update();								// 10.00 ms

	tick++;
	if (tick >= 500)  {							// 5000 ms
		tick = 0;
		//at->update();
		scan_interfaces();
	}

	foreach(QString key, clients.keys() ) {
//		if (debug_level > 1)  {
//			fprintf(logstream, "timeout, n = %d\n", clients.size());
//			fflush(logstream);
//		}

		clients.value(key)->process();
	}
	return;
}                    // timeout()

/**************************************************************************

**************************************************************************/

void ctsrvWorker::start() {
	if (QThread::currentThread() != thread()) {
		QMetaObject::invokeMethod(this, "start");
		return;
	}

#ifdef _DEBUG
	//qDebug() << "      ctsrvWorker::start(): calling m_timer->start()";
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

void ctsrvWorker::stop() {

	if (debug_level > 1)  {
		fprintf(logstream, "stop 1a\n");
		fflush(logstream);
	}


	if (QThread::currentThread() != thread()) {
		if (debug_level > 1)  {
			fprintf(logstream, "stop 1b\n");
			fflush(logstream);
		}

		QMetaObject::invokeMethod(this, "stop");
		return;
	}

	if (debug_level > 1)  {
		fprintf(logstream, "stop 2\n");
		fflush(logstream);
	}

	if (m_timer) {
		m_timer->stop();
	}

	if (bctimer) {
		bctimer->stop();
	}

#ifdef _DEBUG
	if (at)  {
		at->setdbg();			// causes Tmr to print update results
	}
#endif

	DEL(at);

	if (debug_level > 1)  {
		fprintf(logstream, "stop 3\n");
		fflush(logstream);
	}


	foreach(QString key, clients.keys() ) {
		if (debug_level > 1)  {
			fprintf(logstream, "stop 3b\n");
			fflush(logstream);
		}
		delete(clients.value(key));
	}

	if (debug_level > 1)  {
		fprintf(logstream, "stop 3c\n");
		fflush(logstream);
	}

	clients.clear();

	if (debug_level > 1)  {
		fprintf(logstream, "stop 4\n");
		fflush(logstream);
	}


	if (udp_server_socket) {
		udp_server_socket->close();
		delete udp_server_socket;
		udp_server_socket = 0;
	}
	if (debug_level > 1)  {
		fprintf(logstream, "stop 5\n");
		fflush(logstream);
	}


	if (bcsocket) {
		bcsocket->close();
		delete bcsocket;
		bcsocket = 0;
	}

	/*
		for(int i=0; i<ifs.size(); i++)  {
		if (ifs[i].bcsocket != NULL) {
			ifs[i].bcsocket->close();
			delete ifs[i].bcsocket;
			ifs[i].bcsocket = 0;
		}
		}
		ifs.clear();
	 */
	if (debug_level > 1)  {
		fprintf(logstream, "stop 6\n");
		fflush(logstream);
	}


	DEL(sdirs);

#ifdef _DEBUG
	printf("maxbcwait = %lld\n", maxbcwait);					// 1433, 1383 nanoseconds
	printf("maxscanwait = %lld\n", maxscanwait);				// 1603, 4340 nanoseconds
#endif
	if (debug_level > 1)  {
		fprintf(logstream, "stop x\n");
		fflush(logstream);
	}


	FCLOSE(logstream);

	emit finished();
}           // stop() slot

/****************************************************************************

****************************************************************************/

void ctsrvWorker::ctlmsg_slot(QString _key, unsigned char *_buf, int _n) {
	int status = -1;

	// should this just go into a message queue? does this block in case another client emits this signal while this is
	// still running?

	if (udp_server_socket) {
		if (peerport != 0) {
			if (clients.contains(_key)) {
				status = udp_server_socket->writeDatagram((const char*)_buf, _n, clients[_key]->peeraddr, (quint16)clients[_key]->peerport);
			}
		}           // if (peerport != 0) {
		Q_ASSERT(status > 0);
	}              // if (udp_server_socket)  {
	return;
}                 // ctlmsg_slot

/**********************************************************************

**********************************************************************/

void ctsrvWorker::connected_to_trainer_slot(QString _key, bool _b) {

	if (_b == false) {
		bp = 3;
		// remove this trainer from clients
		if (!clients.contains(_key)) {
			if (debug_level > 1)  {
				fprintf(logstream, "cttts???\n");
				fflush(logstream);
			}
			//throw("ctts???");			// throws from slots not caught
		}
		delete clients[_key];
		clients[_key] = 0;
		clients.remove(_key);
#ifdef _DEBUG
		//qDebug() << "worker emitting connected to trainer (false) signal to ctsrv, key = " << _key;
#endif
	}
	else  {
#ifdef _DEBUG
		//qDebug() << "worker emitting connected to trainer (true) signal to ctsrv, key = " << _key;
#endif
	}


	emit connected_to_trainer_signal(_key, _b);              // pass the signal up to ctsrv
	return;
}

/**********************************************************************

**********************************************************************/

void ctsrvWorker::set_devnums() {
	//int devnum;
	char str[32];

	foreach(QString key, clients.keys() ) {
		//clients.value(key)->process();
		sprintf(str, "UDP-%d ", clients.value(key)->nhbd.devnum);
		strcat(devnums, str);
	}
	int len = strlen(devnums);
	if (devnums[len - 1] == ' ') {
		devnums[len - 1] = 0;
	}
	bp = 0;

	return;
}

/**********************************************************************

**********************************************************************/

bool ctsrvWorker::have(const char *_devid) {

	if (clients.contains(_devid)) {
		return true;
	}
	return false;
}                                // int ctsrvWorker::create_udp_server_socket(void)  {


/**********************************************************************

**********************************************************************/

int ctsrvWorker::scan_interfaces(void)  {
	int n;
	quint32 val;
	QHash<QString, INTERFACE> interfaces;
	QElapsedTimer et;
	bool b;
	qint64 nanoseconds;
	Q_UNUSED(nanoseconds);
	int rc = 0;

#ifdef _DEBUG
	//printf("scanning for interfaces\n");
#endif

	et.start();
	b = mutex.tryLock(5);								// wait up to 5 ms for a lock
	if (b==false)  {
		bp = 1;
	}
	nanoseconds = et.nsecsElapsed();					// max was 4340 nanoseconds in my testing

	//interfaces.clear();
#ifdef _DEBUG
	maxscanwait = qMax(maxscanwait, nanoseconds);

	Q_ASSERT(interfaces.size()==0);
#endif

	n = 0;
	bcinfos.clear();



	foreach(const QNetworkInterface iface, QNetworkInterface::allInterfaces()) {
		QString name = iface.name();

		if (iface.flags().testFlag(QNetworkInterface::IsUp) && (!iface.flags().testFlag(QNetworkInterface::IsLoopBack))) {
			foreach(QNetworkAddressEntry entry, iface.addressEntries()) {
				if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
					n++;
					if (!interfaces.contains(name)) {
						interfaces[name].macaddr = iface.hardwareAddress();         // grows here
						interfaces[name].ipv4entries.append(entry);
						interfaces[name].ip = entry.ip();
						interfaces[name].netmask = entry.netmask();
						interfaces[name].bcaddr = entry.broadcast();

						if (!bcinfos.contains(entry.broadcast().toString())) {
							BCINF bcinf;
							bcinf.ip = entry.ip();
							bcinf.bc = entry.broadcast();
							bcinfos[entry.broadcast().toString()] = bcinf;
						}
					}
				}
				else  {
					bp = 1;
				}
			}
		}
		else  {
			bp = 2;
		}
	}              // foreach(const QNetworkInterface iface, QNetworkInterface::allInterfaces()) {

	if (n == 0) {
		//throw(1);                  // no network, ctsrv catches
		rc = 1;
		goto finis;
	}

	rc = n;

	if (!override_ip.isNull()) {
		override_broadcast_info.clear();

		override_broadcast_info.dg[0] = 'R';
		override_broadcast_info.dg[1] = 'M';
		override_broadcast_info.dg[2] = 'P';
		override_broadcast_info.dg[3] = 'C';
		override_broadcast_info.dg[4] = 0x0d;                 // msg length, ~length
		override_broadcast_info.dg[5] = (char)0xf2;           // msg length, ~length
		override_broadcast_info.dg[6] = 0x0a;                 // bcst sect len
		override_broadcast_info.dg[7] = (char)0xfe;           // SECT_BCST, broadcast section id

		override_broadcast_info.dg[8] = 0;
		override_broadcast_info.dg[9] = 0;
		override_broadcast_info.dg[10] = 0;
		override_broadcast_info.dg[11] = 0;
		override_broadcast_info.dg[12] = 0;
		override_broadcast_info.dg[13] = 0;
		override_broadcast_info.dg[14] = 0;
		override_broadcast_info.dg[15] = 0;
		override_broadcast_info.dg[16] = 0;
		override_broadcast_info.dg[17] = 0;

		QNetworkAddressEntry ae;
		ae.setIp(override_ip);
		override_broadcast_info.bc = ae.broadcast();       // doesn't work
		override_broadcast_info.ip = override_ip;

		val = override_ip.toIPv4Address();
		override_broadcast_info.dg[8] = (val >> 24) & 0x000000ff;
		override_broadcast_info.dg[9] =  (val >> 16) & 0x000000ff;
		override_broadcast_info.dg[10] =  (val >> 8) & 0x000000ff;
		override_broadcast_info.dg[11] =  val & 0x000000ff;
		override_broadcast_info.dg[12] = (listen_port >> 8) & 0x00ff;
		override_broadcast_info.dg[13] = listen_port & 0x00ff;
	}
	else  {
		bp = 0;

		foreach(QString key, bcinfos.keys() ) {
			bcinfos[key].clear_dg();

			bcinfos[key].dg[0] = 'R';
			bcinfos[key].dg[1] = 'M';
			bcinfos[key].dg[2] = 'P';
			bcinfos[key].dg[3] = 'C';
			bcinfos[key].dg[4] = 0x0d;                // msg length, ~length
			bcinfos[key].dg[5] = (char)0xf2;          // msg length, ~length
			bcinfos[key].dg[6] = 0x0a;                // bcst sect len
			bcinfos[key].dg[7] = (char)0xfe;          // SECT_BCST, broadcast section id

			val = bcinfos[key].ip.toIPv4Address();
			bcinfos[key].dg[8] = (val >> 24) & 0x000000ff;
			bcinfos[key].dg[9] =  (val >> 16) & 0x000000ff;
			bcinfos[key].dg[10] =  (val >> 8) & 0x000000ff;
			bcinfos[key].dg[11] =  val & 0x000000ff;
			bcinfos[key].dg[12] = (listen_port >> 8) & 0x00ff;
			bcinfos[key].dg[13] = listen_port & 0x00ff;
			bp++;
		}
	}

#ifdef _DEBUG
	//dms = at->stop();            // 1 ms, <1 ms
#endif

finis:
	mutex.unlock();
	return rc;


}							// scan_interfaces()

/**********************************************************************

**********************************************************************/

QString ctsrvWorker::get_appkey(QString _udpkey)  {
	QString s;

	if (clients.contains(_udpkey)) {
		s = clients[_udpkey]->appkey;
	}

	return s;
}

}           // namespace RACERMATE
