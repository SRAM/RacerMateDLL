
#include "ctsrvworker.h"
#include "ctsrv.h"

#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostInfo>

//#ifdef WIN32
//#include <assert.h>
//#endif

#include <comutils.h>

namespace RACERMATE  {

Ctsrv* Ctsrv::m_instance = 0;

/**********************************************************************

**********************************************************************/

Ctsrv::Ctsrv(int _listen_port, int _broadcast_port, const char *_myip, int _debug_level, QObject *_parent) : QObject(_parent) {
	Q_ASSERT(!m_instance);
	m_instance = this;


//qFatal("testing qFatal");		// rm1 can't see this
//throw("testing...");				// rm1 can see this

	debug_level = _debug_level;
	parent = _parent;


	logstream = 0;
	sdirs = new SDIRS(debug_level, "rmserver");
	if (debug_level > 0) {
		QString s;
		s = sdirs->get_debug_dir() + "server.log";
		strncpy(logname, s.toUtf8().constData(), sizeof(logname)-1);
		logstream = fopen(logname, "wt");
		QDateTime dt = QDateTime::currentDateTime();
		s = dt.toString("yyyy-MM-dd-hh-mm-ss");
		fprintf(logstream, "%s starting at %s, debug_level = %d\n\n", logname, s.toUtf8().constData(), debug_level);
		fflush(logstream);
	}           // if (debug_level > 0)

#ifdef _DEBUG
	//qDebug() << "   Ctsrv(): tid = " << QThread::currentThreadId();
#endif

	worker = NULL;

	int argc = 1;
	//m_application = qApp ? 0 : new QCoreApplication(argc, 0);

	if (qApp) {
		m_application = 0;
		//qDebug() << "   qApp exists, so m_application set to 0";
	}
	else  {
		m_application = new QCoreApplication(argc, 0);
		//qDebug() << "   qApp is 0, so creating a QCoreApplication";
	}

	workerthread = new QThread(this);
	try  {
		worker = new ctsrvWorker(_listen_port, _broadcast_port, _myip, debug_level);
	}
	catch (const char *str) {
		Q_UNUSED(str);
		//printf("\r\nconst char *exception: %s\r\n\r\n", str);
		if (debug_level > 1)  {
			fprintf(logstream, "caught 1 %s\n", str);
			fflush(logstream);
		}
		throw -1;
	}
	catch (int &i)  {
		if (debug_level > 1)  {
			fprintf(logstream, "caught 2 %d\n", i);
			fflush(logstream);
		}
		throw i;
	}
	catch (...)  {
		if (debug_level > 1)  {
			fprintf(logstream, "caught 3\n");
			fflush(logstream);
		}
		throw -2;
	}

	worker->moveToThread(workerthread);

	connect(workerthread, SIGNAL(started()), worker, SLOT(start()));
	connect(worker, SIGNAL(resultReady(QString)), SLOT(handleResults(QString)), Qt::DirectConnection);
	connect(worker, SIGNAL(finished()), workerthread, SLOT(quit()));


	workerthread->start();

	if (debug_level > 1)  {
		fprintf(logstream, "worker started\n");
		fflush(logstream);
	}

	//--------------------------------------------------------
	// now wait until any clients out there get connected:
	//--------------------------------------------------------

	memset(gstr, 0, sizeof(gstr));

/*
	int nclients=0;
	int lastnclients=0;
	//int devnum;
	bool flag=true;
	quint64 now, lastcommtime;

	int dms;
	tmr = new Tmr("server");

	tmr->start();

	lastcommtime = QDateTime::currentMSecsSinceEpoch();

	if (debug_level > 1)  {
		fprintf(logstream, "searching...\n");
		fflush(logstream);
	}

	while (flag) {
		nclients = get_nclients();
		now = QDateTime::currentMSecsSinceEpoch();
		if (nclients != lastnclients)  {
			lastnclients = nclients;
#ifdef _DEBUG
			//qDebug("dt = %d", now - lastcommtime);
#endif
			lastcommtime = now;
		}
		if (nclients > 0)  {
			if ((now - lastcommtime)>5000)  {							// was 4000
				flag = false;
			}
		}
		else  {
			if ((now - lastcommtime)>10000)  {
				flag = false;
			}
		}
		QThread::msleep(10);
	}

	dms = tmr->stop();            // 2.97, 5.0, 3.98, 2.90, 4.90, 4.1, 3.0, 4.94, 2.90, 4.96, 4281 ms

	if (debug_level > 1)  {
		fprintf(logstream, "finished searching after %.2f seconds\n", (float)(dms / 1000.0f));
		fflush(logstream);
	}


	if (debug_level > 1)  {
		fprintf(logstream, "done searching\n");
		fflush(logstream);
	}

	if (logstream)  {
		logg("dms = %d\n", (int)dms);
	}

	m_worker->set_devnums();


#ifdef _DEBUG
	int devnum;
	foreach(QString key, m_worker->clients.keys() ) {
		devnum = m_worker->clients[key]->nhbd.devnum;
		Q_ASSERT(devnum != 0);
	}
#endif
*/

	if (debug_level > 1)  {
		fprintf(logstream, "serverx\n");
		fflush(logstream);
	}

	bp = 0;
}


/**********************************************************************
	 destructor
**********************************************************************/

Ctsrv::~Ctsrv() {
	bool dbg;

#ifdef _DEBUG
	dbg = true;
	//dbg = false;
#else
	dbg = false;
#endif

	if (debug_level > 1)  {
		fprintf(logstream, "close 1\n");
		fflush(logstream);
	}


	QString s;

	if (dbg) {
		qDebug() << "   ~Ctsrv()";
	}

	if (dbg) {
		s = m_application ? "m_application exists" : "m_application is 0";

		if (qApp) {
			qDebug() << "   qApp is NOT 0";
		}
		else  {
			qDebug() << "   qApp is 0";
		}
		qDebug() << "   ~Ctsrv(): connecting m_workerThread finished signal to eventLoop quit() slot, " << s;
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close 2\n");
		fflush(logstream);
	}


	QEventLoop eventLoop;

	if (dbg) {
		qDebug() << "   ~Ctsrv(): calling m_worker->stop()";
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close 3\n");
		fflush(logstream);
	}


	worker->stop();       // <<<<<<<  QEventLoop: Cannot be used without QApplication


	if (dbg) {
		qDebug() << "   ~Ctsrv(): back from m_worker->stop()";
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close 4\n");
		fflush(logstream);
	}



	// Wait worker and it's thread to finish gracefully
	connect(workerthread, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	if (dbg) {
		qDebug() << "   ~Ctsrv(): calling eventLoop.exec(), " << s;
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close 4b\n");
		fflush(logstream);
	}

	eventLoop.exec();								// <<<<<<<<<<< crashes here

	if (dbg) {
		qDebug() << "   ~Ctsrv(): back from eventLoop.exec()";
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close 5\n");
		fflush(logstream);
	}


	delete worker;

	if (dbg)  {
		qDebug() << "   ~Ctsrv(): 1";
	}

	if (m_application) {
		if (dbg) {
			qDebug() << "   ~Ctsrv(): deleting m_application";
		}
		delete m_application;
		if (dbg)  {
			qDebug() << "   ~Ctsrv(): 2";
		}
		m_application = 0;
	}

	m_instance = 0;

	if (dbg)  {
		qDebug() << "   ~Ctsrv(): 3";
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close 6\n");
		fflush(logstream);
	}


	DEL(tmr);

	DEL(sdirs);
	bp = 0;

	if (dbg)  {
		qDebug() << "   ~Ctsrv() tid = " << QThread::currentThreadId();
		qDebug() << "   ~Ctsrv() finished";
	}

	if (debug_level > 1)  {
		fprintf(logstream, "close x\n");
		fflush(logstream);
	}

	FCLOSE(logstream);

}              // ~Ctsrv()


#ifdef DOINT
/**********************************************************************

**********************************************************************/

void Ctsrv::data_slot(int _ix, RACERMATE::QT_DATA *_data) {
	emit(data_signal(_ix, _data));
	return;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::ss_slot(int _ix, RACERMATE::qt_SS::SSD *_ssd) {
	emit(ss_signal(_ix, _ssd));
	return;
}


/**********************************************************************

**********************************************************************/

void Ctsrv::rescale_slot(int _ix, int _maxforce) {
	emit(rescale_signal(_ix, _maxforce));
	return;
}
#else
/**********************************************************************

**********************************************************************/

void Ctsrv::data_slot(QString _key, RACERMATE::QT_DATA *_data) {
	emit data_signal(_key, _data);

	return;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::ss_slot(QString _key, RACERMATE::qt_SS::SSD *_ssd) {
	emit ss_signal(_key, _ssd);

	return;
}


/**********************************************************************

**********************************************************************/

void Ctsrv::rescale_slot(QString _key, int _maxforce) {
	emit rescale_signal(_key, _maxforce);

	return;
}
#endif

/**********************************************************************

 ***********************************************************************/

void Ctsrv::logg(const char *format, ...) {
	if (!logstream)  {
		return;
	}

	int len;

	len = (int)strlen(format);
	if (len > 1023) {
		fprintf(logstream, "\r\n(string too long in logg())\r\n");
		return;
	}

	va_list	ap;													// Argument pointer
	char	s[1024];												// Output string
	//int i;


	if (format == NULL)  {
		fprintf(logstream,"Error: format = NULL!!!!\n");
		fclose(logstream);
		logstream = fopen("server.log", "a+t");
		return;
	}

	va_start(ap, format);
	vsprintf(s, format, ap);
	va_end(ap);
	fprintf(logstream, "%s", s);


	return;
}


/**********************************************************************

**********************************************************************/

qint32 ArrayToInt(QByteArray source) {
	qint32 temp;
	QDataStream data(&source, QIODevice::ReadWrite);

	data >> temp;
	return temp;
}                          // ArrayToInt()



/**********************************************************************

**********************************************************************/

qt_SS::BARINFO *Ctsrv:: get_barinfo(QString _key, int i) {
	if (worker) {
		if (worker->clients.contains(_key)) {
			return worker->clients[_key]->ss->get_barinfo(i);
		}
		else  {
			bp = 1;
		}
	}
	else  {
		bp = 2;
	}
	return 0;
}

#ifdef INTID
/**********************************************************************

**********************************************************************/

void Ctsrv::setStarted(int _ix, bool _value) {
	Q_UNUSED(_ix);
	Q_UNUSED(_value);
	return;
}
#else
/**********************************************************************

**********************************************************************/

void Ctsrv::setStarted(const char * _key, bool _value) {
	Q_UNUSED(_key);
	Q_UNUSED(_value);
	return;
}
#endif


/**********************************************************************

**********************************************************************/

int Ctsrv::setPaused(QString _key, bool _paused) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif


	if (worker) {
		if (worker->clients.contains(_key)) {
			worker->clients[_key]->set_paused(_paused);
		}
		else  {
#ifdef _DEBUG
			qDebug() << "setPaused error";
			//throw("oooops");
			return 1;
#endif
		}
	}
	return 0;
}
/**********************************************************************

**********************************************************************/

void Ctsrv::setFinished(const char * _key, bool _value) {
	Q_UNUSED(_key);
	Q_UNUSED(_value);

#ifdef _DEBUG
	//const char *cptr = _key.toUtf8().constData();
#endif

	/*
	 if (m_worker) {
		  if (m_worker->clients.contains(_key)) {
				m_worker->clients[_key]->...
		  }
		  else  {
	 #ifdef _DEBUG
				qDebug() << "set_file_mode error";
				throw("oooops");
	 #endif
		  }
	 }
	 */

	return;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::doSomeOtherStuff(int value) {
	QMutexLocker locker(&m_accessMutex);                           // Prevent calling following code from different threads simultaneously

	qDebug() << "Doing some other stuff..." << value;
}


/*********************************************************************************************************************************

*********************************************************************************************************************************/

#ifdef INTID
int Ctsrv::reset_client(int _ix) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (ix < 0) {
		ix = _ix;
	}

	foreach(QString key, m_worker->udpclients.keys() ) {
		if (m_worker->udpclients[key]->id == _ix) {
			m_worker->udpclients[key]->reset();
			return 0;
		}
	}

	return 1;
}
#else
//int Ctsrv::reset_client(QString _key) {
int Ctsrv::reset_client(const char * _key) {
	if (worker->clients.contains((_key))) {
		worker->clients[_key]->reset();
		return 0;
	}

	return 1;
}
#endif

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::send(int _ix, const unsigned char *_str, int _len, bool _flush) {
	Q_UNUSED(_ix);
	Q_UNUSED(_str);
	Q_UNUSED(_len);
	Q_UNUSED(_flush);
	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_ergo_mode(QString _key, int _fw, int _rrc, float _manwatts) {
	int status = -1;

	if (worker->clients.contains(_key)) {
		status = worker->clients[_key]->set_ergo_mode(_fw, _rrc, _manwatts);
	}

	return status;

}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void Ctsrv::set_export(QString _key, const char *_dbgfname) {
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_export(_dbgfname);
	}
	return;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_file_mode(QString _key, bool _mode) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_file_mode(_mode);
	}
	else  {
#ifdef _DEBUG
		qDebug() << "set_file_mode error";
		//throw("oooops");
		return 1;
#endif
	}
	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

bool Ctsrv::client_is_running(int _ix) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (ix < 0) {
		ix = _ix;
	}
	throw("client not running");
	return false;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::get_nclients() {
	if (worker) {
		return worker->clients.count();
	}
	return 0;
}


/*********************************************************************************************************************************

*********************************************************************************************************************************/

float *Ctsrv::get_average_bars(QString _key) {

#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		return worker->clients[_key]->get_average_bars();
	}
	else {
	}

	return 0;
}


/*********************************************************************************************************************************

*********************************************************************************************************************************/

float Ctsrv::get_ftp(QString _key) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif
	if (worker->clients.contains(_key)) {
		float f = worker->clients[_key]->get_ftp();
		return f;
	}
	return -1.0f;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::get_handlebar_buttons(QString _key) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif
	if (worker->clients.contains(_key)) {
		return worker->clients[_key]->get_handlebar_buttons();
	}
	else {
	}

	//return 0;
	return 0x40;			// setting the 'no communication' bit
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

bool Ctsrv::is_client_connected(QString _key) {
	if (worker->clients.contains(_key)) {
		if (worker->clients[_key]->is_connected()) {
			return true;
		}
	}
	return false;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

bool Ctsrv::is_paused(QString _key) {
	if (worker->clients.contains(_key)) {
		bool b = false;
		b = worker->clients[_key]->is_paused();
		return b;
	}
	return false;
}

/*********************************************************************************************************************************
	 xxx

#ifdef _DEBUG
	 const char *cptr = _key.toUtf8().constData();
#endif

	 else  {
#ifdef _DEBUG
		  qDebug() << "set_file_mode error";
		  throw("oooops");
#endif
	 }

*********************************************************************************************************************************/

int Ctsrv::reset_averages(QString _key) {

#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		worker->clients[_key]->reset_stats();
	}
	else  {
#ifdef _DEBUG
		qDebug() << "reset_averages error";
		//throw("oooops");
		return 1;
#endif
	}

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_ftp(QString _key, float _ftp) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_ftp(_ftp);
	}
	else  {
#ifdef _DEBUG
		qDebug() << "set_ftp error";
		//throw("oooops");
		return 1;
#endif
	}
	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_hr_bounds(QString _key, int _minhr, int _maxhr, bool _beepEnabled) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_hr_bounds(_minhr, _maxhr, _beepEnabled);
	}

	else  {
#ifdef _DEBUG
		qDebug() << "set_hr_bounds error";
		//throw("oooops");
		return 1;
#endif
	}

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::set_paused(QString _key, bool _paused) {
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_paused(_paused);
	}
	return 1;
}

/**************************************************************

**************************************************************/
int Ctsrv::set_grade(QString _key, float _bike_kgs, float _person_kgs, float _drag_factor, int _igradex10) {            // sets windload mode
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_grade(_bike_kgs, _person_kgs, _drag_factor, _igradex10);
	}
	return 0;
}

/*********************************************************************************************************************************
	 rm1 compatibility
*********************************************************************************************************************************/
int Ctsrv::set_slope(QString _key, float _bike_kgs, float _person_kgs, int _drag_factor, float _grade) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		int igradex10 = qRound(_grade * 10.0f);
		worker->clients[_key]->set_grade(_bike_kgs, _person_kgs, _drag_factor, igradex10);
	}
	else  {
#ifdef _DEBUG
		qDebug() << "set_slope error";
		//throw("oooops");
		return 1;
#endif
	}

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::start_cal(QString _key) {
	// Q_UNUSED(ix);
	int status = -1;

	if (worker->clients.contains(_key)) {
		status = worker->clients[_key]->start_cal();
	}
	return status;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int Ctsrv::end_cal(QString _key) {
	int status = -1;
	Q_UNUSED(status);

	if (worker->clients.contains(_key)) {
		status = worker->clients[_key]->end_cal();
	}
	return 0;
}

/**************************************************************

**************************************************************/

float Ctsrv::get_watts_factor(QString _key) {

#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		float f;
		f = worker->clients[_key]->get_watts_factor();
		return f;
	}
	else  {
#ifdef _DEBUG
		qDebug() << "get_watts_factor error";
		//throw("oooops");
		return -1.0f;
#endif
	}


	return 0.0f;
}

/**************************************************************

**************************************************************/


SSDATA Ctsrv::get_ss_data(QString _key, int _fw) {

#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		ssd = worker->clients[_key]->get_ss_data(_fw);
	}
	else  {
	}
	return ssd;
}



/**************************************************************

**************************************************************/

void Ctsrv::setwind(QString _udpkey, float _wind_kph) {

#ifdef _DEBUG
	const char *cptr = _udpkey.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_udpkey)) {
		worker->clients[_udpkey]->set_wind(_wind_kph);
	}
	else  {
#ifdef _DEBUG
		qDebug() << "setwind error";
		//throw("oooops");
#endif
	}

	return;

}

/**************************************************************

**************************************************************/

void Ctsrv::set_draftwind(QString _key, float _draft_wind_kph) {
#ifdef _DEBUG
	const char *cptr = _key.toUtf8().constData();
	Q_UNUSED(cptr);
#endif

	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_draft_wind((_draft_wind_kph));
	}
	else  {
#ifdef _DEBUG
		//qDebug() << "set_draftwind error";
		throw("set_draftwind error");
#endif
	}
	return;

}


/**************************************************************

**************************************************************/
#ifdef INTID
void Ctsrv::setlbs(int _ix, float _person_lbs, float _bike_lbs) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (ix < 0) {
		ix = _ix;
	}
	foreach(QString key, m_worker->udpclients.keys() ) {
		if (m_worker->udpclients[key]->id == _ix) {
			m_worker->udpclients[key]->set_lbs(_person_lbs, _bike_lbs);
			break;
		}
	}
	return;
}
#else
void Ctsrv::setlbs(QString _key, float _person_lbs, float _bike_lbs) {
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_lbs(_person_lbs, _bike_lbs);
	}
	return;
}
#endif

/**************************************************************

**************************************************************/
#ifdef INTID
void Ctsrv::setkgs(int _ix, float _person_kgs, float _bike_kgs) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (ix < 0) {
		ix = _ix;
	}
	foreach(QString key, m_worker->udpclients.keys() ) {
		if (m_worker->udpclients[key]->id == _ix) {
			m_worker->udpclients[key]->set_kgs(_person_kgs, _bike_kgs);
			break;
		}
	}
	return;
}
#else
void Ctsrv::setkgs(QString _key, float _person_kgs, float _bike_kgs) {
	if (worker->clients.contains(_key)) {
		worker->clients[_key]->set_kgs(_person_kgs, _bike_kgs);
	}
	return;
}
#endif

/**************************************************************
	 returns -1 or 0 if we don't yet have it
**************************************************************/

int Ctsrv::get_cal(QString _udpkey) {
	int cal = -1;

	if (worker->clients.contains(_udpkey)) {
		cal = worker->clients[_udpkey]->get_cal();
	}
	return cal;
}


/**************************************************************
	example key is "192.168.1.40 37076"

**************************************************************/

unsigned short Ctsrv::get_fw(const char*_udpkey)  {
	int fw = 0;

	if (worker->clients.contains(_udpkey)) {
		fw = worker->clients[_udpkey]->get_fw();
	}
	return fw;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::handleResults(const QString& result) {
	QMutexLocker locker(&m_accessMutex);

	qDebug() << "Controller received result from worker:" << result;
}

/**********************************************************************

**********************************************************************/

Ctsrv* Ctsrv::instance() {
	return m_instance;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

QList<QString> Ctsrv::get_keys() {
	keys.clear();

	foreach(QString key, worker->clients.keys() ) {
		keys.append(key);
	}
	return keys;
}

/**********************************************************************

**********************************************************************/

int Ctsrv::get_devnum(QString _key) {

	if (worker) {
		if (worker->clients.contains(_key)) {
			return worker->clients[_key]->nhbd.devnum;
		}
	}
	return -1;
}

/**********************************************************************

**********************************************************************/

const char *Ctsrv::get_devnums()  {

	/*
	 char devnums[256];
	 char str[32];
	 int devnum;

	 foreach(QString key, m_worker->clients.keys() ) {
		  devnum = m_worker->clients[key]->nhbd.devnum;
		  strcat(devnums, )
	 }
	 */

	if (worker)  {
		return worker->devnums;
	}
	return NULL;
}

/**********************************************************************

**********************************************************************/

const char *Ctsrv::get_udpkey_from_devnum(const char *_devnum)  {
	int devnum;
	const char *ccptr;
	//static char str[32];

#ifdef _DEBUG
	if(strcmp(_devnum, "5678")==0) {
		bp = 1;
	}
	int n = worker->clients.size();
	Q_UNUSED(n);
#endif


	foreach(QString key, worker->clients.keys() ) {
#ifdef _DEBUG
		//ccptr = key.toStdString().c_str();
		ccptr = key.toUtf8().constData();
#endif

		devnum = worker->clients[key]->nhbd.devnum;
#ifdef _DEBUG
		if(devnum != 5678) {
			bp = 2;
		}
#endif
		//if (!strcmp(itoa(devnum), _devnum))  {
		if (_devnum==QString::number(devnum))  {
			ccptr = worker->clients[key]->key.toUtf8().constData();
			//strncpy(gstr, m_worker->clients[key]->key.toStdString().c_str(), sizeof(gstr));
			strncpy(gstr, ccptr, sizeof(gstr));
#ifdef _DEBUG
			if(strstr(ccptr, "192.168.1.40 ")==NULL) {
				bp = 3;
			}
#endif
			//return ccptr;
			return gstr;
		}
	}
	return NULL;
}									// get_udpkey_from_devnum()

/**********************************************************************

**********************************************************************/

const char *Ctsrv::get_udpkey_from_appkey(QString _appkey)  {
	Q_UNUSED(_appkey);
//	const char *ccptr;

//	foreach(QString udpkey, worker->clients.keys() ) {
//		//devnum = worker->clients[key]->nhbd.devnum;
//		if (udpkey==_appkey)  {
//			ccptr = worker->clients[key]->key.toUtf8().constData();
//			//strncpy(gstr, m_worker->clients[key]->key.toStdString().c_str(), sizeof(gstr));
//			strncpy(gstr, ccptr, sizeof(gstr));
//			return gstr;
//		}
//	}
	return NULL;
}								// get_udpkey_from_appkey()

/**********************************************************************

**********************************************************************/

#if 1
int Ctsrv::check_udp_keys(void) {

	/*
	 DEVICE *d;
	 const char *const* appkey;				// eg, "UDP-5678"

																	 //d = &devices[_id];

	 int n = devices.size();
	 n = 0;

	 for(auto it = devices.begin(); it != devices.end(); ++it) {
		  appkey = &it->first;
		  d = &it->second;
		  if(!strstr(d->udpkey, "192.168.1.40 ")) {
				throw(fatalError(__FILE__, __LINE__));
		  }
		  n++;
	 }

	 //if (!strstr(d->udpkey, "192.168.1.40 ")) {
	 //	throw(fatalError(__FILE__, __LINE__));
	 //}
	 return n;
	 */
	return 0;
}
#endif

/**************************************************************

**************************************************************/

int Ctsrv::get_status_bits(QString _key) {
	int status=-1;

	if (worker)  {
		if (worker->clients.contains(_key)) {
			status = worker->clients[_key]->get_status_bits();
		}
	}

	return status;
}

/**********************************************************************
	 gets signal from Worker::
	 this slot is connected by worker in worker.cpp
**********************************************************************/

void Ctsrv::connected_to_trainer_slot(QString _udpkey, bool _b) {

#ifdef _DEBUG
	int n;
	const char *cptr = _udpkey.toUtf8();
	Q_UNUSED(cptr);

	static int calls = 0;
	calls++;
	if (calls==3)  {
		bp = 3;
	}

	if (_b == false) {
		if (worker->clients.contains(_udpkey)) {
			bp = 2;
		}
		else  {
			//GetTrainerData(_key.toStdString().c_str());
		}

		n = worker->clients.size();
		if (n==0)  {
			bp = 1;
		}
		//qDebug() << "ctsrv got connected to trainer (false) signal, key = " << _key;
	}
	else  {
		//qDebug() << "ctsrv got connected to trainer (true) signal, key = " << _key;
	}
	//qDebug() << "Ctsrv emitting connected to trainer signal to mainclass, key = " << _key;
#endif

	emit connected_to_trainer_signal(_udpkey, _b);					// pass up to final application if it's a Qt app and connected
	return;
}								// connected_to_trainer_slot()

/****************************************************************************

****************************************************************************/

struct TrainerData Ctsrv::GetTrainerData(const char *_key) {

#ifdef _DEBUG
	int n = worker->clients.size();
	if (n==0)  {
		bp = 4;
	}
#endif

	if (worker->clients.contains(_key)) {
		td = worker->clients[_key]->GetTrainerData();
	}
	else  {
		memset(&td, 0, sizeof(td));
	}
	return td;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

float *Ctsrv::get_bars(QString _key)  {


#ifdef _DEBUG
	//const char *cptr = _key.toUtf8().constData();
	//Q_UNUSED(cptr);

	if (worker->clients.size()==0)  {
		bp = 4;
	}

#endif
	if (worker->clients.contains(_key)) {
		return worker->clients[_key]->get_bars();
	}
	else {
#ifdef _DEBUG
		//qDebug() << "get_bars error";
		//set_draftwind error("oooops");
		return nullptr;
#endif
	}
	return nullptr;
}				// get_bars()

/**********************************************************************

**********************************************************************/

bool Ctsrv::have(const char *_devid)  {
	if(!worker) {
		return false;
	}
	if (worker->have(_devid))  {
		return true;
	}
	return false;
}

/**********************************************************************

**********************************************************************/

const char *Ctsrv::get_udpclient_names(void)  {
	memset(client_names, 0, sizeof(client_names));
	QString s;
	UDPClient *clnt;

#ifdef _DEBUG
	const char *ccptr;
	Q_UNUSED(ccptr);
#endif


	foreach(QString key, worker->clients.keys() ) {
		clnt = worker->clients[key];
		if (clnt->nhbd.devnum != 0)  {
			s += ("UDP-" + QString::number(clnt->nhbd.devnum) + " ");
#ifdef _DEBUG
			ccptr = s.toUtf8().constData();
#endif
		}

		/*
		s += (key + ",");
		#ifdef _DEBUG
			ccptr = key.toUtf8().constData();
			ccptr = s.toUtf8().constData();
		#endif
		*/

	}

	s = s.trimmed();

#ifdef _DEBUG
	ccptr = s.toUtf8().constData();
#endif

	if (s.isEmpty())  {
		return NULL;
	}

	memcpy(client_names, s.toUtf8().constData(), sizeof(client_names)-1);

	return client_names;
}

/**********************************************************************

**********************************************************************/

void Ctsrv::calc_tp(QString _key, float _watts)  {
	if (worker)  {
		if (worker->clients.contains(_key)) {
			worker->clients[_key]->calc_tp(_watts);
		}
	}

	return;
}

/**********************************************************************

**********************************************************************/

QString Ctsrv::get_appkey(QString _udpkey)  {
	QString s;
	if (worker)	 {
		s = worker->get_appkey(_udpkey);
	}
	return s;
}

#ifdef _DEBUG

/**********************************************************************

**********************************************************************/

const UDPClient::COMSTATS *Ctsrv::get_comstats(QString _udpkey)  {
	//static UDPClient::COMSTATS comstats;

	if (worker->clients.contains(_udpkey)) {
		return worker->clients[_udpkey]->get_comstats();
	}
	return NULL;
}
#endif


}           // namespace RACERMATE

