
#ifndef _CTSRVWORKER_H_
#define _CTSRVWORKER_H_

#include <QtCore>
#include <QUdpSocket>
#include <QNetworkAddressEntry>

#include <rmdefs.h>
#include <sdirs.h>
#include <tmr.h>

#include "udp_client.h"

namespace RACERMATE  {


class Ctsrv;

class ctsrvWorker : public QObject  {
	Q_OBJECT

	friend class Ctsrv;

	class BCINF  {
		public:
			BCINF()  {
				clear();
				return;
			}
			void clear_dg()  {
				dg.clear();
			}

			void clear()  {
				ip.clear();
				bc.clear();
				clear_dg();
			}

			QHostAddress ip;			// my ip addr
			QHostAddress bc;			// broadcast
			QByteArray dg;				// datagram
	};

	BCINF override_broadcast_info;

	class INTERFACE  {
		public:
			INTERFACE() {
				clear();
				return;
			}
			void clear()  {
				bcaddr.clear();
				netmask.clear();
				name.clear();
				macaddr.clear();
				ipv4entries.clear();
			}

			QString name;
			QString macaddr;
			QList<QNetworkAddressEntry> ipv4entries;
			QHostAddress ip;
			QHostAddress bcaddr;
			QHostAddress netmask;
	};

	public:
		  ctsrvWorker(qint16 _listen_port, qint16 _bcport, QString _override_ip_string, int _debug_level, QObject* parent = 0);
		// don't use a destructor!!! use the stop() slot instead

	private:
		  //QHash<QString, INTERFACE> interfaces;
		  QHash<QString, BCINF> bcinfos;					// key is the ip address of the computer, could have multiple ips for
																	// a single broadcast address.
		  QUdpSocket *bcsocket = NULL;
		  int scan_interfaces(void);
		  QMutex mutex;
			int tick = 0;

#ifdef _DEBUG
		int oktobreak;
		qint64 maxbcwait = 0;
		qint64 maxscanwait = 0;
#endif
		  char devnums[256] = {0};
		  void set_devnums(void);
		qint16 listen_port;
		qint16 bcport = -1;
		int debug_level;
		QTimer *m_timer = NULL;
		QTimer *bctimer = NULL;                                  // broadcast timer

		QHostAddress peeraddr;
		quint16 peerport;
		qint64 bytes_in;
		qint64 start_time;
		qt_SS::BARINFO *get_barinfo(QString _key, int i);
		//void calc_tp(QString _key, unsigned long _ms, float _watts);


		Ctsrv *ctsrv = NULL;
		int bp = 0;
		QUdpSocket *udp_server_socket = 0;
		unsigned bcast_count = 0;
		//QByteArray bc_datagram;
		quint16 seqnum = 0;
		quint8 cs = 0;

		QHash<QString, UDPClient *> clients;
		unsigned long last_ip_broadcast_time = 0;                    // ethernet
		QHostAddress override_ip;


		Tmr *at = NULL;
		int BCTIMEOUT = 1000;

		FILE *logstream = NULL;
		char logname[256] = {0};
		//QFile file;
		SDIRS *sdirs=NULL;

		// fuctions

		int create_udp_server_socket(void);
		//bool have(QString _devid);
		bool have(const char *_devid);
		QString get_appkey(QString _udpkey);

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

