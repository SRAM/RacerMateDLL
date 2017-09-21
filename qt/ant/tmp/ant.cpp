
#include <QtCore>

#ifdef _DEBUG
#include <QDebug>
#endif

#include <QDateTime>

#include <utils.h>			// for rstrip
#include <sdirs.h>

#include "ant.h"
#include "anterrs.h"

namespace RACERMATE {

ANT* ANT::m_instance = 0;

/**********************************************************************************************************
	main site:
		http://libusb.info/

	API documentation:
		http://libusb.sourceforge.net/api-1.0/

	mailing list archives:
		http://marc.info/?l=libusb-devel

	ANT:
		main site:  http://www.thisisant.com/   (log in)
		forum:      http://www.thisisant.com/forum/

 **************************************************************************************************/

ANT::ANT(QObject *_parent) : QObject(_parent) {
	parent = _parent;
	Q_ASSERT(!m_instance);
	m_instance = this;


	debug_level = 0;

	int status;

	status = init();
	if (status != 0) {
		throw 1;
	}

	initialized = true;
	return;
}

/**************************************************************************************************

**************************************************************************************************/

ANT::ANT(int _debug_level, QObject *_parent) : QObject(_parent) {
	parent = _parent;
	Q_ASSERT(!m_instance);
	m_instance = this;

	debug_level = _debug_level;

	int status;

	status = init();
	if (status != 0) {
		throw 1;
	}

	if (debug_level > 1)  {
		logg("ANT x\n\n");
	}

	initialized = true;
}

/**************************************************************************************************
	destructor
**************************************************************************************************/

ANT::~ANT() {

#ifdef _DEBUG
	int n;
	Q_UNUSED(n);
	n = devices.size();
#endif

	//QString s;

#ifdef _DEBUG
	if (debug_level > 1)  {
		//qDebug() << "   ANT:: ~ANT()";
	}
#endif
	if (dbg)  {
		//qDebug() << "   ANT:: ~ANT()";
	}


	foreach(int key, devices.keys() ) {
		delete devices.value(key);
	}
#ifdef _DEBUG
	if (debug_level > 1)  {
		//qDebug() << "   ANT:: ~ANT() 2";
	}
#endif

	devices.clear();

#ifdef _DEBUG
	if (debug_level > 1)  {
		//qDebug() << "   ANT:: ~ANT() creating a QEventLoop";
	}
#endif

	QEventLoop eventLoop;				// "QEventLoop: Cannot be used without QApplication"

#ifdef _DEBUG
	if (debug_level > 1) {
		//qDebug() << "   ANT::  ~ANT(): calling worker->stop()";
	}
#endif

	worker->stop();       // <<<<<<<  QEventLoop: Cannot be used without QApplication


#ifdef _DEBUG
	if (debug_level > 1) {
		//qDebug() << "   ANT::  ~ANT(): back from worker->stop()";
	}
#endif



	// Wait worker and it's thread to finish gracefully
	connect(workerThread, SIGNAL(finished()), &eventLoop, SLOT(quit()));
#ifdef _DEBUG
	if (debug_level > 1) {
		//qDebug() << "   ANT::  ~ANT(): calling eventLoop.exec()";
	}
#endif

	eventLoop.exec();

#ifdef _DEBUG
	if (debug_level > 1) {
		//qDebug() << "   ANT::  ~ANT(): back from eventLoop.exec()";
	}
#endif

	delete worker;


	if (m_application) {
#ifdef _DEBUG
		if (debug_level > 1) {
			//qDebug() << "   ANT::  ~ANT(): deleting m_application";
		}
#endif
		delete m_application;
		m_application = 0;
	}

	m_instance = 0;


#ifdef _DEBUG
	DEL(tmr);
#endif

	if (usb_device_list) {
		libusb_free_device_list(usb_device_list, 1);
		usb_device_list = NULL;
	}

	if (ctx) {
		libusb_exit(ctx);
		ctx = NULL;
	}

	FCLOSE(logstream);
	//DEL(sdirs);
}                    // destructor

/****************************************************************************************

****************************************************************************************/

int ANT::init(void) {
	start_time = QDateTime::currentMSecsSinceEpoch();

#ifdef _DEBUG
	dbg = true;
	static int calls = 0;
	calls++;
	if (calls==2)  {
		bp = 2;
	}
#else
	dbg = false;
#endif

	int status;
	//qint64 now;

	if (dbg)  {
		//qDebug("   ANT::init()");
	}

	if (debug_level > 0)  {
		QString s;
		RACERMATE::SDIRS *sdirs = new RACERMATE::SDIRS(debug_level, "rmant");
		s = sdirs->get_debug_dir() + "ant.log";
		DEL(sdirs);
		logstream = fopen(s.toUtf8().constData(), "wt");
		logg("rmant.log opened\n");
	}

//	cad.push_back(new CAD);
//	sc.push_back(new SC());
//	hr.push_back(new HR());

	shutdown_delay = 50;

#ifdef _DEBUG
	//dbg = true;
	tmr = new RACERMATE::Tmr("ant");
#else
	//	dbg = false;
#endif

	//nants = 0;
	//version = 0L;
	//memset(gstr, 0, sizeof(gstr));


	//critsec = false;

	if (dbg)  {
		//qDebug("   ANT::init() 2");
	}


	status = libusb_init(&ctx);
	if (status != 0) {
		if (debug_level > 0)  {
			logg("ANT libusb_init error = %d\n", status);
		}
		return 1;
	}

	if (debug_level > 0)  {
		logg("ANT1\n");
	}

#ifdef _DEBUG
#ifdef WIN32
	//libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);
	//libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);                 // prints out more warnings!
#else
	//libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);                 // prints out more warnings!
#endif
#else
	//libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);                 // prints out more warnings!
#endif

	verstruc = libusb_get_version();
	version = ((verstruc->major << 24) & 0xff000000) | ((verstruc->minor << 16) & 0x00ff0000) | ((verstruc->micro << 8) & 0x0000ff00) | ((verstruc->nano) & 0x000000ff);
	char str[24] = { 0 };
	sprintf(str, "%d.%d.%d.%d", verstruc->major, verstruc->minor, verstruc->micro, verstruc->nano );
	//sprintf(str, "%d.%d.%d.%d", (verstruc->major << 24) & 0xff000000) | ((verstruc->minor << 16) & 0x00ff0000) | ((verstruc->micro << 8) & 0x0000ff00) | ((verstruc->nano) & 0x000000ff);

	//logg("version = %08lx\n", version);          // version 1.0.20.107
	//logg("version = %s\n", str);                   // version 1.0.20.107
	if (debug_level > 0)  {
		logg("ANT version = %s\n", str);                   // version 1.0.20.107
	}

	if (dbg)  {
		//qDebug("   ANT::init() version = %s", str);
	}

	/*
	workerThread = new QThread(this);
	timer = new QTimer(0);
	timer->setInterval(100);
	timer->moveToThread(workerThread);
	bool b;
	b = (bool)connect(timer, SIGNAL(timeout()), this, SLOT(timeout_slot(void)), Qt::DirectConnection);
	Q_ASSERT(b);
	workerThread->start();
	*/

	//worker = NULL;

	int argc = 1;
	//m_application = qApp ? 0 : new QCoreApplication(argc, 0);

	if (qApp) {
		m_application = 0;
		if (dbg)  {
			qDebug() << "   qApp exists, so m_application set to 0";
		}
	}
	else  {
		m_application = new QCoreApplication(argc, 0);
		if (dbg)  {
			//qDebug() << "   qApp is 0, so creating a QCoreApplication";
		}
	}

	workerThread = new QThread(this);
	try  {
		//tmr->start();
		//worker = new Worker(debug_level, this);		// "cannot move objects with a parent"
		if (debug_level > 1) logg("ANT creating worker\n");
		worker = new antWorker(start_time, debug_level);						// takes 2 ms
		if (debug_level > 1) logg("ANT created worker\n");
		//tmr->stop();
//		double dt = tmr->get_seconds();
//		Q_UNUSED(dt);
		QThread::msleep(20);					// allow worker::timeout_slot to run once before the next line
		worker->ant = this;					// I'm a friend
		bp = 1;
	}
	catch (const char *str) {
		Q_UNUSED(str);
		//printf("\r\nconst char *exception: %s\r\n\r\n", str);
		if (debug_level > 1)  {
			logg("ANT caught 1 %s\n", str);
		}
		throw -1;
	}
	catch (int &i)  {
		if (debug_level > 1)  {
			logg("ANT caught 2 %d\n", i);
		}
		throw i;
	}
	catch (...)  {
		if (debug_level > 1)  {
			logg("ANT caught 3\n");
		}
		throw -2;
	}

	worker->moveToThread(workerThread);		// cannot move objects with a parent

//	connect(workerThread, SIGNAL(started()), worker, SLOT(start()));
//	//connect(worker, SIGNAL(resultReady(QString)), SLOT(handleResults(QString)), Qt::DirectConnection);
//	connect(worker, SIGNAL(finished()), workerThread, SLOT(quit()));

	connect(workerThread, SIGNAL(started()), worker, SLOT(start()));
	//connect(worker, SIGNAL(resultReady(QString)), SLOT(handleResults(QString)), Qt::DirectConnection);
	connect(worker, SIGNAL(worker_finished()), workerThread, SLOT(quit()));

	if (debug_level > 1)  {
		logg("starting worker\n");
	}

	workerThread->start();

	if (debug_level > 1)  {
		logg("ANT worker started\n");
	}

	if (dbg)  {
		//qDebug("   ANT::init() x");
	}


	return 0;
}                    // init()


/**********************************************************************************************************

**********************************************************************************************************/

/*
QString ANT::rstrip(const QString& str) {
	int n = str.size() - 1;

	for (; n >= 0; --n) {
		if (!str.at(n).isSpace()) {
			//if (!str.at(n)==0x0a) {
			return str.left(n + 1);
		}
	}
	return "";
}
*/


/**********************************************************************************************************
	merges rx0.log and tx0.log
**********************************************************************************************************/

void ANT::merge_log_files(void) {


	QFile::copy("rx0.log", "x.x");

#ifdef WIN32
	int status;
	const char *cmd1 = "c:\\cygwin64\\bin\\cat tx0.log >> x.x";
	status = system(cmd1);

	const char *cmd2 = "c:\\cygwin64\\bin\\sort x.x > d.txt";
	status = system(cmd2);
#endif

	QDir dir;
	dir.remove("x.x");
	//	unlink("rx0.log");
	//	unlink("tx0.log");


	return;
}

/**********************************************************************************************************
	scans for ant sticks
	takes about 165 ms (mostly for libusb_get_device_list())
		also takes about 1.7 seconds for each stick that needs to be started (should be a one-time thing)
	takes 3.1 seconds if 2 sticks are initialized.
	takes 31 ms after sticks are found and initialized once

**********************************************************************************************************/

int ANT::scan_for_devices(void) {
//	if (scanning_for_devices)  {
//		return 0;
//	}



	scanning_for_devices = true;
	
//	if(dbg)  {
//		qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;
//		QString s = QString::asprintf("%7lld", now);
//		qDebug("   %s ANT:: scan_for_devices", qPrintable(s));
//	}

	if (debug_level > 1)  {
		logg("scan_for_devices()\n");
	}

	int _nsticks = 0;

#ifdef _DEBUG
	int ms = 0;
	Q_UNUSED(ms);
	static int calls = 0;
	static int grows = 0;
	calls++;
#endif

	//bool b = devices_mutex.tryLock(5);

	//critsec = true;

#ifdef _DEBUG
	if (calls == 1) {
		//tmr->start();
	}
	else if (calls == 3)
	{
		bp = 3;
	}
#endif


	int rc = 0;
	int status = 0;
	ssize_t cnt;
	libusb_device *device;						// opaque pointer!!!!! FSD!
	quint32 key = 0x00000000;
	int k;
	bool is_ant_stick;
	char sn[64];
	libusb_config_descriptor *config = NULL;
	libusb_device_handle *handle = NULL;


	if (ctx == NULL) {
		rc = RMANT_ERROR_NO_CTX;
		goto finis;
	}


	if (usb_device_list) {
		libusb_free_device_list(usb_device_list, 1);
		usb_device_list = NULL;
	}


	//tmr->start();
	cnt = libusb_get_device_list(ctx, &usb_device_list);         // 16 with 2 ant sticks, 15 with 1 ant stick
	//ms = tmr->stop();				// 151 ms

	if (debug_level > 1)  {
		logg("sfd ndevices = %d\n", cnt);
	}

	// todo: handle a stick going away

	if (cnt <= 0) {
		rc = RMANT_ERROR_NO_USB_DEVICES;
		goto finis;
	}

	quint32 bus, addr, port;

	foreach(int key, devices.keys() ) {
		devices.value(key)->set_exists(false);
	}

	if (debug_level > 1)  {
		logg("sfd 1\n");
	}

//	tmr->start();
	for (k = 0; k < cnt; k++) {
		if (debug_level > 1)  {
			logg("sfd k = %d\n", k);
		}
		libusb_device_descriptor desc;
		device = usb_device_list[k];

		//status = libusb_get_device_descriptor(usb_device_list[k], &desc);       // desc.idVendor, desc.idProduct
		status = libusb_get_device_descriptor(device, &desc);       // desc.idVendor, desc.idProduct
		if (status != 0) {
			rc = RMANT_ERROR_GET_DEVICE_DESCRIPTOR;
			goto finis;
		}

		config = NULL;
		if (debug_level > 1)  {
			logg("sfd 2\n");
		}

		//status = libusb_get_config_descriptor(usb_device_list[k], 0, &config);
		status = libusb_get_config_descriptor(device, 0, &config);

		if (debug_level > 1)  {
			logg("sfd 3, status = %d\n", status);
		}

		if (status != 0) {
			if (config) {
				libusb_free_config_descriptor(config);
				config = NULL;
			}
			continue;
		}

		if (config->bNumInterfaces == 0) {
			libusb_free_config_descriptor(config);
			config = NULL;
			continue;
		}

		if (debug_level > 1)  {
			logg("sfd 4\n");
		}

		libusb_free_config_descriptor(config);
		config = NULL;

		is_ant_stick = false;

		switch(desc.idVendor)  {
			case DYNASTREAM_VID:
				is_ant_stick = true;                // true for suunto and garmin
				switch(desc.idProduct)  {
					case ANT_Development_Board1:		// 0x1003
						break;
					case ANTUSB_Stick:					// 0x1004
						bp = 1;
						break;
					case ANT_Development_Board2:		// 0x1006
						bp = 1;
						break;
					case ANTUSB2_Stick:					// 0x1008, my Garmin USB2 stick AND the suunto stick
						bp = 1;
						break;
					case ANTUSB_m_Stick:					// 0x1009, my new Garmin mini stick
						bp = 1;
						break;
					default:
						bp = 2;
						break;
				}
				break;
			default:
				bp = 0;
				break;
		}


		if (!is_ant_stick) {
			continue;
		}
		if (debug_level > 1)  {
			logg("\nsfd found stick ***************************\n");
		}

		_nsticks++;

		key = 0x00000000;

		bus = libusb_get_bus_number(device)&0x000000ff;
		addr = libusb_get_device_address(device)&0x000000ff;
		port = libusb_get_port_number(device)&0x000000ff;
		key = (bus<<24) | (addr<<16) | (port<<8) | (desc.idProduct&0x00ff);			// 0x 03 09		02 00
		//qDebug("********************* key = %08x", key);
#ifdef _DEBUG
		if (calls==2)  {
			bp = 1;
		}
#endif
		if (debug_level > 1)  {
			logg("sfd 5\n");
		}

		if (!devices.contains(key)) {
			if (debug_level > 1)  {
				logg("sfd 6\n");
			}

			status = libusb_open(device, &handle); // Open a device and obtain a device handle

			if (debug_level > 1)  {
				logg("sfd 7, status = %d\n", status);
			}

			if (status==0)  {
				bp = 1;
			}
			else if (status==-3)  {
				// if the user has insufficient permissions,
				// on linux edit /etc/udev/rules.d/myusbs.rules.
				// Had to reboot to get it to take!
				// Also if the device is already open
				rc = status;
				goto finis;
			}
			else  {
				rc = status;
				goto finis;
			}

			if (debug_level > 1)  {
				logg("sfd 8\n");
			}

			if (libusb_kernel_driver_active(handle, 0) == 1) {          //find out if kernel driver is attached
				//qDebug() << "Kernel Driver Active";

				if (libusb_detach_kernel_driver(handle, 0) != 0) {             //detach it
					rc = RMANT_ERROR_DETACHING;
					goto finis;
				}
				else  {
					//qDebug() << "Kernel Driver Detached!";
				}
			}

			//key = desc.idProduct + desc.idVendor;
			if (debug_level > 1)  {
				logg("sfd 9\n");
			}

			status = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (unsigned char*)sn, sizeof(sn));			// returns < 0 on error
			if (status < 0)  {
				if (debug_level > 0)  {
					logg("sfd libusb_get_string_descriptor_ascii returned = %d\n", status);
				}
				if (status == LIBUSB_ERROR_NOT_FOUND) {
					//if (auto_claim(transfer, &current_interface, USB_API_WINUSBX) != LIBUSB_SUCCESS)
					//	return LIBUSB_ERROR_NOT_FOUND;
					bp = 2;
				}
				else  {
					bp = 3;
				}
				libusb_close(handle);
				handle = NULL;
				rc = status;
				goto finis;
			}

			libusb_close(handle);
			handle = NULL;

			tid = QThread::currentThreadId();
#ifdef _DEBUG
			//qDebug() << tid;
			grows++;
			if (grows == 2) {
				bp = 5;
			}

#endif

			if (debug_level > 1)  {
				logg("ANT::creating device %d\n", key);
			}

			if(dbg)  {
//				qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;
//				QString s = QString::asprintf("%7lld", now);
//				qDebug("   %s ANT:: creating device %d", qPrintable(s), key);
				qDebug("   ANT:: creating device %d", key);
			}

			// QObject:: Cannot create children for a parent that is in a different thread.
			// Parent is RACERMATE::ANT

			devices[key] = new ANTDEV(start_time, debug_level, key, this);              // devices grow here

			if(dbg)  {
//				qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;
//				QString s = QString::asprintf("%7lld", now);
//				qDebug("   %s ANT:: created device %d", qPrintable(s), key);
				qDebug("   ANT:: created device %d", key);
			}

			if (debug_level > 1)  {
				logg("sfd 12\n");
			}

			status = devices[key]->set_device(device);
			if (debug_level > 1)  {
				logg("sfd 13, status = %d\n", status);
			}
			if (status != 0) {
				bp = 1;
			}
			//tmr->start();
			if (debug_level > 1)  {
				logg("sfd 14\n");
			}
			devices[key]->start();			// starts the communication timers and does ANTMsg::resetSystem(), ANTMsg::setNetworkKey(net, key)
			//ms = tmr->stop();					// 1.44 seconds
			//QThread::msleep(500);
			if (debug_level > 1)  {
				logg("sfd 15\n");
			}

		}                          // if (!antdevs.contains(key))  {
		else {
			devices[key]->set_exists(true);
		}
	}                             // devices, for(k)

	if (debug_level > 1)  {
		logg("sfd 16, _nsticks = %d, nsticks = %d\n", _nsticks, nsticks);
	}

	if (_nsticks != nsticks)  {
		if (_nsticks < nsticks)  {
			bp = 0;
			foreach(int key, devices.keys() ) {
				if (!devices.value(key)->get_exists())  {
					delete devices.value(key);
					devices.remove(key);								// devices shrink here
				}
				bp++;
			}
		}
		nsticks = _nsticks;
		if (nsticks==0)  {
			listening = false;
		}
	}

#ifdef _DEBUG
//	ms = tmr->stop();				// 14 ms
//	dump_antdevs();
//	cnt = devices.size();
#endif


//	foreach(int key, devices.keys() ) {
//		devices.value(key)->start();				// uses libusb to reset the stick and set the network key
//		QThread::msleep(500);
//	}

	//	if (usb_device_list) {
	//		libusb_free_device_list(usb_device_list, 1);
	//		usb_device_list = NULL;
	//	}

finis:
#ifdef _DEBUG
	Q_ASSERT(config == NULL);
	Q_ASSERT(handle == NULL);
#endif

	if (debug_level > 1)  {
		if (rc != 0)  {
			logg("sfd x, rc = %d", rc);
		}
	}

	//	if (usb_device_list) {
	//		libusb_free_device_list(usb_device_list, 1);
	//		usb_device_list = NULL;
	//	}

#ifdef _DEBUG
	if (calls == 1) {
		ms = tmr->stop();
//		double dt = tmr->get_seconds();                    // 1.97 seconds if we call devices[key]->start()
//		Q_UNUSED(dt);
		bp = 2;
	}
#endif

#ifdef _DEBUG
	if (rc != 0) {
		bp = 7;
	}
#endif

	//critsec = false;
	scanning_for_devices = false;

	return rc;

}										// scan_for_devices()


/**********************************************************************************************************

**********************************************************************************************************/

void ANT::dump_antdevs(void) {
	ANTDEV *ad;

	qDebug("\nant terminal  ANTDEVS:");
	bp = 0;
	foreach(int key, devices.keys() ) {
		ad = devices.value(key);
		qDebug("    ANt::  %s   %s", ad->mfgr, ad->prod);
		bp++;
	}

	return;
}						// dump_antdevs()


/**********************************************************************************************************
	called from main app or dll

	sets up channel 0 of the first ant stick (device) to do background scan
	see pdf file, page 7
	Figure 5-2. Configuring Background Scanning Channel

	returns
		0 if OK
		1 if no devices
**********************************************************************************************************/

int ANT::start_listening()  {

	if (debug_level > 1)  {
		logg("start_listening\n");
	}

	int rc = 0;
	//int chan = 7;
	ANTDEV *device;

	if (devices.size()==0)  {
		rc = RMANT_ERROR_NO_DEVICES;
		goto done;
	}

	device = devices.first();

	if (debug_level > 1)  {
		logg("found device\n");
	}

	rc = device->start_listening();


done:
	if (rc==0)  {
		listening = true;
	}
	else  {
		listening = false;
	}

	if (debug_level > 1)  {
		logg("start_listening done, rc = %d\n", rc);
	}

	return rc;
}                          // start_listening()

/**********************************************************************************************************

**********************************************************************************************************/
/*
int ANT::update()  {

	if (devices.size()==0)  {
		rc = RMANT_ERROR_NO_DEVICES;
		goto done;
	}

	device = devices.first();

	rc = device->start_listening();

	return 0;
}
*/






/********************************************************************************************************

********************************************************************************************************/

int ANT::stop(void) {

	workerThread->exit();
	QThread::msleep(500);         // wait for communication to the ANT sticks to stop (just in case)

//	if (handle) {
//		libusb_close(handle);
//		handle = NULL;
//	}

	return 0;
}                                // stop()

/********************************************************************************************************

********************************************************************************************************/

void ANT::logg(const char *format, ...) {

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

#ifdef _DEBUG
	//#ifndef WIN32
//		s = rstrip(s);							// qDebug() adds its own lf
//		qDebug("%s", qPrintable(s));
	//#endif
#endif


	return;
}                                      // logg

/********************************************************************************************************

********************************************************************************************************/

float ANT::get_ant_cadence(quint32 _devnum)  {
	float f = 0.0f;



	if (!devices.contains(_devnum)) {
		return f;
	}

	f = devices[_devnum]->get_ant_cadence();
	return f;

}

/********************************************************************************************************

********************************************************************************************************/

float ANT::get_ant_wheel_rpm(quint32 _devnum)  {
	Q_UNUSED(_devnum);
	float f = 0.0f;

//	if (!devices.contains(_devnum)) {
//		return f;
//	}
//	f = devices[_key]->get_ant_wheel_rpm();
	return f;
}

/********************************************************************************************************

********************************************************************************************************/

float ANT::get_ant_meters_per_hour(quint32 _devnum)  {
	Q_UNUSED(_devnum);
	float f = 0.0f;

//	if (!devices.contains(_key)) {
//		return f;
//	}
//	f = devices[_key]->get_ant_meters_per_hour();
	return f;
}

/********************************************************************************************************

********************************************************************************************************/

unsigned long ANT::get_ant_cm(quint32 _devnum)  {
	Q_UNUSED(_devnum);
	unsigned long cm = 0;

//	if (!devices.contains(_key))  {
//		return cm;
//	}
//	cm = devices[_key]->get_ant_cm();

	return cm;
}

/**************************************************************************

**************************************************************************/

const char * ANT::get_sensors_string() {
	if (devices.size()==0)  {
		return NULL;
	}
	ANTDEV *device;
	device = devices.first();
	strncpy(sensors_string, device->get_sensors_string().toUtf8().constData(), sizeof(sensors_string));
	if (strlen(sensors_string)==0)  {
		return NULL;
	}
	return sensors_string;
}

/**************************************************************************

**************************************************************************/

const SENSORS* ANT::get_sensors(void)  {
	if (devices.size()==0)  {
		return NULL;
	}
	return devices.first()->get_sensors();
}

#ifdef _DEBUG
/**************************************************************************

**************************************************************************/

const char * ANT::get_sensors_ex() {
	if (devices.size()==0)  {
		return NULL;
	}

	ANTDEV *device;

	device = devices.first();
	strncpy(sensors_string, device->get_sensors_ex().toUtf8().constData(), sizeof(sensors_string)-1);
	if (strlen(sensors_string)==0)  {
		return NULL;
	}
	return sensors_string;
}
#endif

/**********************************************************************************************************

**********************************************************************************************************/

const char * ANT::get_sticks(void) {

	if (devices.size()==0)  {
		return NULL;
	}

	ANTDEV *ad;
	QString s2;

	memset(sticks, 0, sizeof(sticks));

	foreach(int key, devices.keys() ) {
		ad = devices.value(key);
		if (strlen(sticks) > 0)  {
			s2 = QString::asprintf( ", %s   %s", ad->mfgr, ad->prod);
		}
		else  {
			s2 = QString::asprintf( "%s   %s", ad->mfgr, ad->prod);
		}
		strcat(sticks, s2.toUtf8().constData());
		bp++;
	}

	if (strlen(sticks) > 0)  {
#ifdef _DEBUG
		int len = strlen(sticks);
		if (len != 40)  {
			bp = 2;
		}
		if (sticks[0] != 'D')  {
			bp = 3;
		}
#endif

		return sticks;
	}
	else  {
		return NULL;
	}

}						// get_sticks()

/********************************************************************************************************
	_sn = the sensor 'device number'
********************************************************************************************************/

int ANT::assign_sensor(int _sn, int _type)  {
	int rc = -1;
	ANTDEV *ad;

	if (devices.size()==0)  {
		return -1;
	}


	ad = devices.first();

	foreach(SENSOR sensor, ad->chanmap[0].sensors ) {
		if (sensor.sn==_sn)  {

#ifdef _DEBUG
			//qDebug("assigning %d to next channel", _sn);
#endif
			// xxx
			rc = ad->assign(_sn, _type);
			goto finis;
		}
	}

finis:
	bp = 1;

	return rc;
}

/********************************************************************************************************
	unassigns the channel that _sn is on
********************************************************************************************************/

int ANT::unassign_sensor(int _sn)  {
	ANTDEV *ad;
	int rc = 0;
	int assigned_chan = -1;
	//Q_UNUSED(chan);

	// xxx

	// find what stick the _sn is on

	foreach(int key, devices.keys() ) {
		ad = devices.value(key);
		if (ad->chanmap[0].sensors.contains(_sn))  {
			assigned_chan = ad->chanmap[0].sensors[_sn].assigned_channel;
			//rc = devices[assigned_chan]->unassign(assigned_chan);
			rc = ad->unassign(assigned_chan, _sn);
			bp = 2;
			break;
		}
	}


	return rc;
}

/**********************************************************************************************************

**********************************************************************************************************/

void ANT::dump(void)  {

	if (devices.size()==0)  {
		return;
	}

	ANTDEV *ad;
	QString s2;

	memset(sticks, 0, sizeof(sticks));

	foreach(int key, devices.keys() ) {
		ad = devices.value(key);
		s2 = QString::asprintf( "%s %s", ad->mfgr, ad->prod);
		ad->dump_channels(s2.toUtf8().constData());
		bp++;
	}
	return;

}						// dump()

/**********************************************************************************************************

**********************************************************************************************************/

int ANT::get_hr(unsigned short _sn)  {
	ANTDEV *ad;
	int hr=0;

	// find what stick the _sn is on

	foreach(int key, devices.keys() ) {
		ad = devices.value(key);
		if (ad->chanmap[0].sensors.contains(_sn))  {
			hr = ad->get_hr(_sn);
			break;
		}
	}

	return hr;

}

}		// namespace RACERMATE
