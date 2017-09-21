
#include <utils.h>		// for rstrip()

#include "antworker.h"


#include "ant.h"

#include <sdirs.h>

namespace RACERMATE  {


/**************************************************************************

**************************************************************************/

antWorker::antWorker(qint64 _start_time, int _debug_level, QObject* _parent ) : QObject(_parent) {
	start_time = _start_time;
	debug_level = _debug_level;
	ant = (ANT*)_parent;

	int status;

	tmr = new Tmr("W");

	if (debug_level > 0) {
		RACERMATE::SDIRS *sdirs = new RACERMATE::SDIRS(debug_level, "rmant");
		status = sdirs->get_rc();
		switch(status)  {
			case 0:
				break;
			case -1:
				throw("sdirs error 1");			// creator catches this
			case -2:
				throw("sdirs error 2");
			case -3:
				throw("sdirs error 3");
			default:
				throw("sdirs error 101");
		}

		QString s = sdirs->get_debug_dir() + "antworker.log";
		DEL(sdirs);

		strncpy(logname, s.toUtf8().constData(), sizeof(logname)-1);
		logstream = fopen(logname, "wt");
		QDateTime dt = QDateTime::currentDateTime();
		lock_logging();
		logg("%s starting at %s, debug_level = %d\n\n", logname, s.toUtf8().constData(), debug_level);
		unlock_logging();
	}           // if (debug_level > 0)

	timer = new QTimer(this);
	timer->setInterval(100);
	ticks = 999;																					// so first timeout will be > 3000 ms

	if (debug_level > 1)  {
		lock_logging();
		logg("connecting\n");
		unlock_logging();
	}

	connect(timer, SIGNAL(timeout()), this, SLOT(timeout_slot()));             // , Qt::DirectConnection, prepareResult()

	lock_logging();
	logg("constructor finished\n");
	unlock_logging();

	return;
}                    // constructor()


/**************************************************************************

**************************************************************************/

void antWorker::timeout_slot() {                  // see prepareResult(), trackshun

//	if (debug_level > 1)  {
//		lock_logging();
//		logg("tick\n");
//		unlock_logging();
//	}

#ifdef _DEBUG
	//tmr->update();									// 100 ms
#endif

//	bool b;

//	//b = mutex.tryLock(5);                        // wait up to 5 ms for a lock
//	b = mutex.lock();
//	if (b == false) {
//		bp = 1;
//	}

//	int timeout = 30;
//	if (ticks==999)  {
//		timeout = 30;
//	}
//	else  {
//		timeout = 30;
//	}
	ticks++;

	if (ticks < 30)  {								// every 3000 ms
		return;
	}

	ticks = 0;
//	if (debug_level > 1)  {
//		lock_logging();
//		logg("tock\n");
//		unlock_logging();
//	}

	if (ant)  {											// passed in to the constructor
//		if (!ant->is_listening())  {
//			int status = ant->start_listening();
//		}

//		bp = 1;
//		int status;
//		Q_UNUSED(status);

//		if (ant->initialized)  {
//			if (!ant->scanning_for_devices)  {
//				bp = 2;
//				status = ant->scan_for_devices();
//				if (ant->devices.size()==0)  {
//					return;
//				}
//			}

//			ANTDEV *device;
//			device = ant->devices.first();
//			if (!device->is_listening())  {
//				status = device->open_listening_channel();
//				bp = 3;
//			}
//		}
	}



	return;
}                    // timeout_slot()


/**************************************************************************

**************************************************************************/

void antWorker::start() {
	lock_logging();
	logg("start()\n");
	unlock_logging();

	if (QThread::currentThread() != thread()) {
		QMetaObject::invokeMethod(this, "start");
		return;
	}


	#ifdef _DEBUG
		//qDebug() << "      antWorker::start(): calling m_timer->start()";
	#endif

	if (timer) {
		lock_logging();
		logg("starting timer\n");
		unlock_logging();

		timer->start();
//		bp = 4;
	}

	if (debug_level > 1)  {
		lock_logging();
		logg("start x\n");
		unlock_logging();
	}
}                       // start() slot

/**************************************************************************
	don't do this in a destructor!!!!!
**************************************************************************/

void antWorker::stop() {
	if (QThread::currentThread() != thread()) {
		QMetaObject::invokeMethod(this, "stop");
		return;
	}

	if (timer) {
		timer->stop();
	}

#ifdef _DEBUG
	if (tmr)  {
		tmr->setdbg();			// causes Tmr to print update results
	}
#endif

	DEL(tmr);
	//DEL(sdirs);

	FCLOSE(logstream);

	emit worker_finished();
}           // stop() slot


/********************************************************************************************************

********************************************************************************************************/

void antWorker::logg(const char *format, ...) {

	if (logstream == NULL) {
		return;
	}

	if (format == NULL) {
		return;
	}

	va_list ap;                                        // Argument pointer
	QString s;

	va_start(ap, format);
	vsprintf(logstr, format, ap);
	va_end(ap);


	//#ifdef _DEBUG
	qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;
	s = QString::asprintf("%7lld %s", now, logstr);
	//qDebug("%s", qPrintable(rstrip(s)));
	//#endif

	fprintf(logstream, "%s", qPrintable(s));
	fflush(logstream);

	return;
}                                      // logg

/*******************************************************************************************************

*******************************************************************************************************/

void antWorker::lock_logging(void) {
	bool b;

#ifdef _DEBUG
	qint64 start;
	start = et.nsecsElapsed();
#endif

	b = logmutex.tryLock(5);                        // wait up to 5 ms for a lock
	if (b == false) {
	}

	#ifdef _DEBUG
	qint64 nanoseconds;
	nanoseconds = et.nsecsElapsed() - start;
	maxlogwait = qMax(maxlogwait, nanoseconds);
	#endif
	return;
}

/*******************************************************************************************************

*******************************************************************************************************/

void antWorker::unlock_logging(void) {
	logmutex.unlock();
	return;
}

}           // namespace RACERMATE


