
#include <utils.h>		// for rstrip()

#include "ant.h"

#include "antdefs.h"

#ifdef DOWORKER
	#include "anterrs.h"
#endif

#include "antworker.h"



#include <sdirs.h>

namespace RACERMATE  {


/**************************************************************************

**************************************************************************/

antWorker::antWorker(qint64 _start_time, int _debug_level, QObject* _parent ) : QObject(_parent) {
	int status;

	start_time = _start_time;
	debug_level = _debug_level;

#ifdef DOWORKER
	#ifdef _DEBUG
	dbg = true;
	#endif

	status = libusb_init(&ctx);
	if (status != 0) {
		if (debug_level > 0)  {
			logg("ANT libusb_init error = %d\n", status);
		}
		return;
	}
	verstruc = libusb_get_version();
	version = ((verstruc->major << 24) & 0xff000000) | ((verstruc->minor << 16) & 0x00ff0000) | ((verstruc->micro << 8) & 0x0000ff00) | ((verstruc->nano) & 0x000000ff);
#else
	ant = (ANT*)_parent;
#endif


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

#ifndef DOWORKER
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
#endif


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

#ifdef DOWORKER
	foreach(int key, devices.keys() ) {
		delete devices.value(key);
	}
	devices.clear();

	if (usb_device_list) {
		libusb_free_device_list(usb_device_list, 1);
		usb_device_list = NULL;
	}

	if (ctx) {
		libusb_exit(ctx);
		ctx = NULL;
	}
#endif

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

//#ifdef DOWORKER
#if 0
	if (usb_device_list) {
		libusb_free_device_list(usb_device_list, 1);
		usb_device_list = NULL;
	}

	if (ctx) {
		libusb_exit(ctx);
		ctx = NULL;
	}
#endif

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


#ifdef DOWORKER

/**********************************************************************************************************
	scans for ant sticks
	takes about 165 ms (mostly for libusb_get_device_list())
		also takes about 1.7 seconds for each stick that needs to be started (should be a one-time thing)
	takes 3.1 seconds if 2 sticks are initialized.
	takes 31 ms after sticks are found and initialized once

**********************************************************************************************************/

int antWorker::scan_for_devices(void) {
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
				logg("antWorker::creating device %d\n", key);
			}

			if(dbg)  {
//				qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;
//				QString s = QString::asprintf("%7lld", now);
//				qDebug("   %s ANT:: creating device %d", qPrintable(s), key);
				qDebug("   antWorker:: creating device %d", key);
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
		}									// if (!devices.contains(key)) {
//#endif
	}                             // devices, for(k)

	if (debug_level > 1)  {
		logg("sfd 16, _nsticks = %d, nsticks = %d\n", _nsticks, nsticks);
	}

	if (_nsticks != nsticks)  {
		if (_nsticks < nsticks)  {
			bp = 0;
	
			if (debug_level > 1)  {
				logg("sfd 17\n");
			}

			foreach(int key, devices.keys() ) {
				if (debug_level > 1)  {
					logg("sfd 18\n");
				}
				if (!devices.value(key)->get_exists())  {
					if (debug_level > 1)  {
						logg("sfd 19\n");
					}

					delete devices.value(key);
					if (debug_level > 1) logg("sfd 20\n");
					devices.remove(key);								// devices shrink here
					if (debug_level > 1) logg("sfd 21\n");
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
		else  {
			if (debug_level > 1) logg("sfdx\n");
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
	called from main app or dll

	sets up channel 0 of the first ant stick (device) to do background scan
	see pdf file, page 7
	Figure 5-2. Configuring Background Scanning Channel

	returns
		0 if OK
		1 if no devices
**********************************************************************************************************/

int antWorker::start_listening()  {

	if (debug_level > 1)  {
		logg("start_listening\n");
	}

	int rc = 0;

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

const char * antWorker::get_sticks(void) {
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

/**************************************************************************

**************************************************************************/

const SENSORS* antWorker::get_sensors(void)  {

	if (devices.size()==0)  {
		return NULL;
	}
	return devices.first()->get_sensors();
}

/**********************************************************************************************************

**********************************************************************************************************/

int antWorker::get_hr(unsigned short _sn)  {
	int hr=0;

	ANTDEV *ad;
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

#endif							// #ifdef DOWORKER

}           // namespace RACERMATE


