
#ifndef _WORKER_H_
#define _WORKER_H_

#include <QtCore>

#include <rmdefs.h>

#include <QUdpSocket>


#include "udp_client.h"

namespace RACERMATE  {


class Ctsrv;

class Worker : public QObject  {
	Q_OBJECT

	friend class Ctsrv;

	class INTF  {
		public:
			INTF() {
				bcsocket = NULL;
				return;
			}
			QHostAddress bcaddr;
			QHostAddress netmask;
			QUdpSocket *bcsocket;
	};

	public:
		  Worker(qint16 _listen_port, qint16 _bcport, QString _override_ip_string, int _debug_level, QObject* parent = 0);
		// don't use a destructor!!! use the stop() slot instead

	private:

		  //QList<INTF> ifs;
		  QVector<INTF> ifs;

#ifdef _DEBUG
		int oktobreak;
#endif
		  char devnums[256];
		  void set_devnums(void);
		qint16 listen_port;
		qint16 bcport;
		int debug_level;
		QTimer *m_timer;
		QTimer *bctimer;                                  // broadcast timer

		QString program_dir;
		QString personal_dir;
		QString settings_dir;
		QString debug_dir;
		QFile file;



		QHostAddress peeraddr;
		quint16 peerport;
		qint64 bytes_in;
		qint64 start_time;
		qt_SS::BARINFO *get_barinfo(QString _key, int i);


		Ctsrv *ctsrv;
		int bp;
		QUdpSocket *udp_server_socket;
		unsigned bcast_count;
		QByteArray bc_datagram;
		quint16 seqnum;
		quint8 cs;

		FILE *logstream;
		QHash<QString, UDPClient *> clients;
		unsigned long last_ip_broadcast_time;                    // ethernet
		QHostAddress override_ip;


		Tmr *at;
		char logname[32];

		// fuctions

		int create_udp_server_socket(void);
		//bool have(QString _devid);
		bool have(const char *_devid);

	public slots:
		void start();
		void stop();

	private slots:
		void timeout();

		void disconnected();
		void hostFound();
		void processPendingDatagrams();
		void bc_timeout();
		void connected_to_trainer_slot(QString, bool);

		void ctlmsg_slot(QString, unsigned char *, int);

	signals:
		void finished();
		void resultReady(const QString &result);
		void connected_to_trainer_signal(QString, bool);


		void data_signal(QString, RACERMATE::QT_DATA *);
		void ss_signal(QString, RACERMATE::qt_SS::SSD *);
		void rescale_signal(QString, int);
};

}				// namespace RACERMATE

#endif

