
#include <QDir>
#include <QDateTime>

#ifdef WIN32
	#include <shlobj.h>
#endif

#include <utils.h>
#include "consolereader.h"

#include "mainclass.h"

/***********************************************************************

	http://treyweaver.blogspot.com/2013/02/qt-console-application-template-tutorial.html

	/tools/options/environment/system/terminal:
		/usr/bin/xterm -geometry 150x48+100+100 -fa 'Monospace' -fs 11 -fg yellow -bg blue -title heehaw -e

***********************************************************************/

MainClass::MainClass(int _debug_level, QObject *_parent) : QObject(_parent) {
	parent = _parent;
	debug_level = _debug_level;
	init();

	//qDebug("\n\nMainClass constructor finished\n");
	if (debug_level > 0) {
		lock_logging();
		logg("MainClass constructed\n");
		unlock_logging();
	}

}

/***********************************************************************
	call this routine to quit the application
***********************************************************************/

void MainClass::quit() {
	emit finished();                    // signal CoreApplication to quit
}


/***********************************************************************
	shortly after quit is called the CoreApplication will signal this routine
	this is a good place to delete any objects that were created in the
	constructor and/or to stop any threads
***********************************************************************/

void MainClass::aboutToQuitApp() {
	// stop threads

	if (timer) {
		timer->stop();
	}
	QThread::sleep(1);          // wait for threads to stop


	// delete any objects

	DEL(ant);
	DEL(timer);
	DEL(tmr);
	FCLOSE(logstream);

	//DEL(sdirs);

}

/***********************************************************************

***********************************************************************/

void MainClass::OnConsoleKeyPressed(char c) {
	int sn = 60697;			// hr
	int type = 120;				// hr
	//int sn = 44014;        // speed-cadence
	//int type = 121;            // speed-cadence

	switch (c) {
		case 0: {
			break;
		}

		case 's':
		case 'S':  {
			//ant->scan_for_devices();
			break;
		}

		case 'a':
		case 'A':  {
			int status;
			Q_UNUSED(status);
			status = ant->assign_sensor(sn, type);
			break;
		}

		case 'u':
		case 'U': {
			ant->unassign_sensor(sn);
			break;
		}

		case 0x1b:  {
			quit();
			break;
		}
		default: {
			bp = 3;
			break;
		}
	}                       // switch(c)
	return;
}


/***********************************************************************
	10ms after the application starts this method will run
	all QT messaging is running at this point so threads, signals and slots will all work as expected.
***********************************************************************/


void MainClass::run() {
	//printf("MainClass.Run is executing\n");
	// timeout_slot() is the main loop
}

/**************************************************************************

**************************************************************************/

void MainClass::timeout_slot() {

	//lock_logging();
	//logg("MainClass tick\n");
	//unlock_logging();
	//tmr->update();                         // 100 ms

	if (!ant) {
		return;
	}

	//bool have_sensors = false;
	scan_ticks++;

	//--------------------------------------------------------
	// run background scanner every 4 seconds
	//--------------------------------------------------------

	//if (scan_ticks >= 40) {
	if (scan_ticks >= 25) {
		scan_ticks = 0;

		int status = -1;
		if (!ant->get_scanning_for_devices()) {
			status = ant->scan_for_devices();                              // can take > 3 seconds the first time
			if (status)  {
				qDebug("scan_for_devices returned %d", status);
				return;
			}
		}

		int n = ant->get_n_devices();

		if (n == 0) {
			qDebug("no sticks 1");
			return;
		}


		static int starts = 0;
		if (!ant->is_listening()) {
			starts++;
			if (starts == 2) {
				bp = 3;
			}
			status = ant->start_listening();
		}

		if (status) {
			qDebug("start_listening returned %d", status);
			return;
		}

		sticks = ant->get_sticks();
		if (!sticks)  {
			qDebug("no sticks 3");
			return;
		}
	}						// if (scan_ticks >= 25) {

	if (sticks==NULL)  {
		return;
	}

	//--------------------------------------------------------
	// display section
	//--------------------------------------------------------

	display_ticks++;

//#define DISP 0			// works ant->dump()
//#define DISP 1			// works	ant->get_sensors_string()	ant->get_sensors_ex()
//#define DISP 2			// works (doesn't crash)	ant->get_sensors2();
#define DISP 3				// ant->get_sensor_structs()


	#if DISP==2
		//QList<SENSORSTRUCT> sensorlist;
		//std::vector<SENSORSTRUCT> sensorstructs;
		B sensorstructs;
	#endif

	if (display_ticks == 10) {
		//tmr->update();                         // 1000 ms
		display_ticks = 0;

#if DISP==0
		ant->dump();
#elif DISP==1
		//const char *sensors = ant->get_sensors_string();
		const char *sensors = ant->get_sensors_ex();
		if (!sensors)  {
			qDebug("no sensors");
			return;
		}
		qDebug("%s", sensors);
#elif DISP==2
		int i, n;
		int hr;
#ifdef _DEBUG
//		int ms;
//		float hr2;
		//float hr3;
//		static int lastms=0;
		//static int lastdms = 0;
//		static bool first = true;
//		int dms = 0;
//		float hrv;
#endif

		sensorstructs = ant->get_sensor_structs();
		//n = sensorstructs.size();
		n = sensorstructs.n;


		if (n==0)  {
			qDebug("no sensors");
			return;
		}

		for(i=0; i<n; i++)  {
			//qDebug("%d %d", sensorlist[i].sn, sensorlist[i].type)	;
			//SENSORSTRUCT ss = sensorstructs[i];
			A ss = sensorstructs.sensors[i];

			if (ss.type==HR_DEV_TYPE)  {								// 120
				hr = ant->get_hr(ss.sn);
#ifdef _DEBUG
//				int denom = ss.ucEventCount - ss.prevcnt;
//				if (denom != 0)  {
//					hr3 = (float)(ss.usTime1024 - ss.usPreviousTime1024) / (float)denom;
//				}
//				else  {
//					hr3 = 0.0f;
//				}

//				//ms = (unsigned short) (ss.usR_RInterval1024*1000.0f/1024.0f);				// period converted to ms
//				ms = qRound((float)ss.usR_RInterval1024*1000.0f/1024.0f);							// period converted to ms

//				// despike
//				double ratio;

//				if (first)  {
//					first = false;
//					ratio = 1.0;
//				}
//				else  {
//					if (lastms==0)  {
//						ratio = 1.0;
//					}
//					else  {
//						ratio = qFabs((double)ms/(double)lastms);
//						if (ratio > 1.4)  {
//							bp = 1;
//							//ms = lastms;
//						}
//						else if (ratio < .6)  {
//							//ms = lastms;
//							bp = 2;
//						}
//					}
//				}



//				dms = ms - lastms;
//				lastms = ms;
//				hrv = smav.compute((float)dms);

//				if (ms != 0)  {
//					hr2 = 60.0f / (ms/1000.0f);
//				}
//				else  {
//					hr2 = 0.0f;
//				}

//				qDebug("hr1 = %d   ms = %4d   hr2 = %.1f   dms = %4d   hrv = %6.1f  ratio = %.3lf   chan = %d",
//						 hr,
//						 ms,
//						 //ss.usR_RInterval1024,
//						 hr2,
//						 dms,
//						 hrv,
//						 ratio,
//						 ss.assigned_channel
//				);

				//qDebug("hr = %d   chan = %d", hr, ss.assigned_channel);
				qDebug("hr = %d", hr);

#if 0
				rr = 1151
				ms = rr*1000/1024				// 1124
				period = ms*1000				// 1.124 seconds
				hr = 60/period					// 53
#endif

#else
				qDebug("%d", hr);
#endif
			}
		}

		bp = 1;

		//float hr = ant->get_hr();

//		float cad=0, rpm=0, speed=0;
//		unsigned long cm=0;
//		cad = ant->get_ant_cadence(44014);
//		rpm = ant->get_ant_wheel_rpm(44014);
//		speed = ant->get_ant_meters_per_hour(44014);
//		cm = ant->get_ant_cm(44014);
//		qDebug("%.2f   %.2f   %.2f   %ld", cad, rpm, speed, cm);

#elif DISP==3
		int i, n, hr;

		const SENSORS *sensors;
		sensors = ant->get_sensors();

		if (sensors==NULL)  {
			qDebug("no channels[0]");
		}

		n = sensors->n;

		if (n==0)  {
			qDebug("no sensors");
			return;
		}

		for(i=0; i<n; i++)  {
			SENSOR s = sensors->sensors[i];

			if (s.type==HR_DEV_TYPE)  {								// 120
				hr = ant->get_hr(s.sn);
				qDebug("hr = %d", hr);
			}
		}
#endif

	}



	return;
}                       // timeout()

/***********************************************************************

***********************************************************************/

void MainClass::init(void) {
	app = QCoreApplication::instance();          // get the instance of the main application
	start_time = QDateTime::currentMSecsSinceEpoch();

	if (debug_level > 0) {
		int status;
		RACERMATE::SDIRS *sdirs = new RACERMATE::SDIRS(debug_level, "ant");
		status = sdirs->get_rc();
		switch (status) {
			case 0: {
				break;
			}
			case -1: {
				throw("sdirs error 1");       // creator catches this
			}
			case -2: {
				throw("sdirs error 2");
			}
			case -3: {
				throw("sdirs error 3");
			}
			default:
				throw("sdirs error 101");
		}

		QString s = sdirs->get_debug_dir() + "main.log";
		DEL(sdirs);

		strncpy(logname, s.toUtf8().constData(), sizeof(logname) - 1);
		logstream = fopen(logname, "wt");
		QDateTime dt = QDateTime::currentDateTime();
		lock_logging();
		logg("%s starting at %s, debug_level = %d\n\n",
			  logname,
			  s.toUtf8().constData(),
			  debug_level);
		unlock_logging();

	}

	if (debug_level > 0) {
		lock_logging();
		logg("MainClass creating ANT\n");
		unlock_logging();
	}

	ant = new RACERMATE::ANT(debug_level, this);          // this has a 1 second timer running that does nothing at the moment

	if (debug_level > 0) {
		lock_logging();
		logg("MainClass created ANT\n");
		unlock_logging();
	}

	// open ini file:

	char curdir[256];
	strcpy(curdir, QDir::currentPath().toStdString().c_str());

	char path[256];
	//sprintf(path, "%s/x.log", QDir::currentPath().toStdString().c_str());
	sprintf(path, "%s/main.ini", curdir);

	printf("ini file = %s\n", path);

	settings = new QSettings(path, QSettings::IniFormat);       // in debug mode this is in the .../...Debug directory

	if (settings->allKeys().size() != 0) {
		QString s;
		QString s2;
		settings->beginGroup("options");
		//serial_device = settings->value("serialdevice").toString();			// case sensitive
		const QStringList childKeys = settings->childKeys();
		bp = 0;

		foreach(const QString &childKey, childKeys) {
			s = childKey;
			s2 = settings->value(childKey).toString();
			printf("s = %s\n", s2.toStdString().c_str());
			bp++;
		}
		settings->endGroup();
	}

#ifdef _DEBUG
//	int status;
//	status = ant->scan_for_devices();
//	if (status)  {
//		DEL(ant);
//		qFatal("ant->scan_for_devices returned %d", status);
//	}

//	QThread::msleep(100);

//	status = ant->scan_for_devices();
//	if (status)  {
//		DEL(ant);
//		qFatal("ant->scan_for_devices returned %d", status);
//	}

//	ant->start_listening();
	// xyzzy
//	while(1)  {				// ok with this infinite loop
//		QThread::msleep(100);
//	}

#endif


	ConsoleReader *consoleReader = new ConsoleReader();
	connect(consoleReader, SIGNAL(KeyPressed(char)), this, SLOT(OnConsoleKeyPressed(char)));
	//consoleReader->
	//QThread::
	if (debug_level > 0) {
		lock_logging();
		logg("MainClass starting console\n");
		unlock_logging();
	}

	consoleReader->start();

// xyzzy

//	while(1)  {				// not ok with this infinite loop
//		QThread::msleep(100);
//	}

	tmr = new RACERMATE::Tmr("main");

	timer = new QTimer(this);
	timer->setInterval(100);
	connect(timer, SIGNAL(timeout()), this, SLOT(timeout_slot()));               // does nothing for now, the program sleeps until a key is pressed

	if (debug_level > 0) {
		lock_logging();
		logg("MainClass starting QTimer\n");
		unlock_logging();
	}
	timer->start();
	if (debug_level > 0) {
		lock_logging();
		logg("MainClass started QTimer\n");
		unlock_logging();
	}

	return;
}                       // init()

/********************************************************************************************************

********************************************************************************************************/

void MainClass::logg(const char *format, ...) {

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


	#ifdef _DEBUG
	qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;
	//s = rstrip(QString::asprintf("%7lld %s", now, logstr));
	s = QString::asprintf("%7lld %s", now, logstr);
	//qDebug("%s", qPrintable(rstrip(s)));
	#endif

	fprintf(logstream, "%s", qPrintable(s));
	fflush(logstream);

	return;
}                                      // logg

/*******************************************************************************************************

*******************************************************************************************************/

void MainClass::lock_logging(void) {
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

void MainClass::unlock_logging(void) {
	logmutex.unlock();
	return;
}
