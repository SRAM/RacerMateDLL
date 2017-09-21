
#ifndef _WORKER_H_
#define _WORKER_H_

#include <QtCore>
#include <QUdpSocket>

#include "udp_client.h"
namespace MYQT  {

class Ctsrv;

class Worker : public QObject  {
		Q_OBJECT
	//friend class Ctsrv;

	public:
		Worker(Ctsrv *_ctsrv, qint16 _listen_port, qint16 _broadcast_port);
		~Worker();

	private:
#ifndef MAXCLIENTS
		static const int MAXCLIENTS=1;
#endif
		Ctsrv *ctsrv;
		int bp;
		qint16 listen_port;
		qint16 broadcast_port;
		QTimer *timer;
		QTimer *bctimer;                                  // broadcast timer
		QUdpSocket *bcsocket;
		QUdpSocket *udp_server_socket;
		unsigned bcast_count;
		QByteArray bc_datagram;
		FILE *logstream;
		UDPClient *udpclients[MAXCLIENTS];
		unsigned long last_ip_broadcast_time;                    // ethernet
		QHostAddress myip;
		QHostAddress bcaddr;
		Tmr *at;

		// fuctions

		int create_udp_server_socket(void);

	private slots:
		void started();
		void finished();

		void disconnected();
		void hostFound();
		void processPendingDatagrams();
		void timeout();
		//void data_slot(int, QT_DATA *);
		//void ss_slot(int, qt_SS::SSD *);
		//void rescale_slot(int _id, int _maxforce);
		void gradechanged_slot(int _igradex10);
		void windchanged_slot(float);


	signals:
		void resultReady(const QString &result);
		void ss_signal(int, MYQT::qt_SS::SSD *);
		void data_signal(int, MYQT::QT_DATA *);
		void rescale_signal(int, int);
		void gradechanged_signal(int _igradex10);
		void windchanged_signal(float);

};
}

#endif

