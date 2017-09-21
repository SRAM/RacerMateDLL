
#ifndef _ANTWORKER_H_
#define _ANTWORKER_H_

#include <antdefs.h>

#include "antdev.h"

#include <QtCore>
#include <tmr.h>

namespace RACERMATE  {


class ANT;

class antWorker : public QObject  {
	Q_OBJECT

	friend class ANT;

	public:
		 antWorker(qint64 _start_time, int _debug_level, QObject* parent = 0);
		// don't use a destructor!!! use the stop() slot instead

	private:
#ifdef DOWORKER
		libusb_context *ctx = NULL;										// a libusb session
		QMap<quint32, ANTDEV *> devices;
		int scan_for_devices(void);
		bool scanning_for_devices = false;
		libusb_device **usb_device_list=NULL;
		Qt::HANDLE tid=0;
		bool dbg = false;
		int nsticks = 0;
		bool listening = false;
		int start_listening(void);
		const char* get_sticks(void);
		char sticks[512]={0};									// ascii list of sticks
		const SENSORS* get_sensors(void);
		int get_hr(unsigned short _sn);
		const struct libusb_version *verstruc = NULL;
		unsigned long version=0;
#else
		 ANT *ant=NULL;						// our beloved creator
#endif

		void logg(const char *format, ...);
		char logstr[1024] = {0};
		void lock_logging(void);
		void unlock_logging(void);
		QMutex logmutex;
		QElapsedTimer et;
		qint64 maxlogwait = -1;

		qint64 start_time=0;
		//QObject *parent = NULL;
		QTimer *timer = NULL;
		QMutex mutex;

		Tmr *tmr = NULL;
		int ticks = 999;
		int debug_level = 0;
		//RACERMATE::SDIRS *sdirs=NULL;
		FILE *logstream = NULL;
		char logname[256] = {0};
#ifdef _DEBUG
		int bp = 0;
#endif

	public slots:
		void start();
		void stop();

	private slots:
		void timeout_slot();

	signals:
		void worker_finished();
};

}				// namespace RACERMATE

#endif

