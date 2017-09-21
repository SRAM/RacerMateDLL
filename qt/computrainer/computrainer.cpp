#include <QDateTime>


//#include <qt_defines.h>
//#include <qt_tdefs.h>

#include <rmconst.h>

#include "computrainer.h"

namespace RACERMATE  {

/**********************************************************************************************

**********************************************************************************************/

Computrainer::Computrainer(const char *_ipstr, int _tcp_port, Rider2 &_rider, bool _doudp, QObject *parent) : QObject(parent) {


	init_vars();

	rider = _rider;
	ilbs = qRound(KGSTOLBS * rider.getkgs());
	strncpy(ipstr, _ipstr, sizeof(ipstr) - 1);
	port = _tcp_port;
	doudp = _doudp;

	connect(parent, SIGNAL(gradechanged_signal(int)), this, SLOT(gradechanged_slot(int)));
	connect(parent, SIGNAL(windchanged_signal(float)), this, SLOT(windchanged_slot(float)));

}

/**********************************************************************************************

**********************************************************************************************/

Computrainer::Computrainer(int _udp_port, Rider2 &_rider, QObject *parent)  {

	init_vars();

	rider = _rider;
	ilbs = qRound(KGSTOLBS * rider.getkgs());
	memset(ipstr, 0, sizeof(ipstr));
	//strncpy(ipstr, _ipstr, sizeof(ipstr) - 1);
	port = _udp_port;
	doudp = true;

	connect(parent, SIGNAL(gradechanged_signal(int)), this, SLOT(gradechanged_slot(int)));
	connect(parent, SIGNAL(windchanged_signal(float)), this, SLOT(windchanged_slot(float)));

}

/**********************************************************************

**********************************************************************/

void Computrainer::start(void) {

	timer = new QTimer(this);
	connect(    timer,               SIGNAL(timeout()),         this, SLOT(timer_slot()));
	timer->start(10);

	return;
}

/**********************************************************************

**********************************************************************/

void Computrainer::stop(void) {

	disconnect(    timer,               SIGNAL(timeout()),         this, SLOT(timer_slot()));
	delete timer;
	timer = 0;

	if (udpsocket) {
		udpsocket->close();
		delete udpsocket;
		udpsocket = 0;
	}
	else  {
		//tcpsocket->close();
		//DEL(tcpsocket);
	}

	if (ss) {
		ss->reset();
	}

	data.reset();

	return;
}


/**********************************************************************

**********************************************************************/

Computrainer::~Computrainer() {
	delete at;
	at = 0;
	delete ss;
	ss = 0;

	/*
		if (socket_state == QAbstractSocket::ConnectedState)  {
		  socket->close();
		}
	 */

}


/**********************************************************************

**********************************************************************/

void Computrainer::init_vars(void) {
	int i, n;

	doudp = false;

	ss = NULL;
	//ss = new qt_SS(NBARS);
	ss = new qt_SS();

#ifdef _DEBUG
	memset(displaystring, 0, sizeof(displaystring));
#endif

	lastdisplaytime = QDateTime::currentMSecsSinceEpoch();
	packetIndex = 0;
	aerodrag = 8.0;
	tiredrag = .006f;
	draft_wind = 0.0f;

	lastlinkuptime = 0;
	tick = 0;
	port = 0;
	memset(ipstr, 0, sizeof(ipstr));
	bp = 0;
	socket_state = QAbstractSocket::UnconnectedState;
	gstart = 0;
	rxinptr = rxoutptr = 0;
	memset(rxq, 0, sizeof(rxq));
	memset(rxbuf, 0, sizeof(rxbuf));
	connected_to_trainer = false;

	timer = NULL;
	udpsocket = NULL;

	at = new Tmr("Computrainer");
	lastCommTime = 0;

	n = NBARS * sizeof(float);
	for (i = 0; i < NBARS; i++) {
		bars[i] = 0.0f;
	}
	bytes_out = 0L;
	calories.clear();
	data.reset();

	finishEdge = false;
	finished = false;
	igradex10 = 0;
	grade = (float)igradex10 / 10.0f;

	hrbeep_enabled = false;
	is_signed = 0x00;
	lastCommTime = 0;
	lastWriteTime = 0;
	last_export_grade = 0.0f;
	lastbeeptime = 0;
	lastlinkuptime = 0;
	logging_type = NO_LOGGING;
	lp.time = 0;
	memset(lp.buf, 0, sizeof(lp.buf));
	manwts = 0.0f;
	outstream = NULL;
	parity_errors = 0;
	paused = false;
	memset(pkt_mask, 0, sizeof(pkt_mask));

	pkt_mask[0] = 0x60;
	pkt_mask[1] = 0x50;
	pkt_mask[2] = 0x48;
	pkt_mask[3] = 0x44;
	pkt_mask[4] = 0x42;
	pkt_mask[5] = 0x41;


	pp.clear();
	ptime = 0;
	recordsOut = 0;
	registered = true;
	tx_select = 0;

	n = sizeof(txbuf);
	memset(txbuf, 0, n);
	wind = 0.0f;
	winds.clear();

	// stuff from decoder+++

	memset(ssraw, 0, sizeof(ssraw));

	accum_hr = 0.0f;
	accum_hr_count = 0;
	accum_keys = 0;
	accum_kph = 0.0f;
	accum_kph_count = 0;
	accum_rpm = 0.0f;
	accum_rpm_count = 0;
	accum_tdc = 0;
	accum_watts = 0.0f;
	accum_watts_count = 0;
	bp = 0;
	HB_ERGO_PAUSE = 0x10;
	HB_ERGO_RUN = 0x14;
	hrvalid = 0;
	keydown = 0;
	keys = 0;
	keyup = 0;
	lastkeys = 0;
	maxhr = 0;
	minhr = 0;
	mps = 0;

	newmask[0] = KEY1;
	newmask[1] = KEY4;
	newmask[2] = KEY5;
	newmask[3] = KEY3;
	newmask[4] = KEY2;
	newmask[5] = KEY6;

	parity_errors = 0;
	packets = 0;
	pedalrpm = 0;
	raw_rpm = 0;
	rawspeed = 0;
	rfdrag = 0;
	rfmeas = 0;
	rpmflags = 0;
	rpmValid = 0;
	slip = 0;
	slipflag = 0;
	sscount = 25;
	version = 0;
	// stuff from decoder---

	control_byte = MODE_WIND;

	return;
}                                   // init_vars()

// slots+++

/**********************************************************************
	slot
**********************************************************************/

void Computrainer::connected_slot() {
	qDebug() << "slot connected()";
	emit connected_to_server_signal(true);
}

/**********************************************************************
	slot
**********************************************************************/

void Computrainer::disconnected_slot() {
	qDebug() << "slot disconnected()";

	udpsocket->close();
	udpsocket = NULL;
	connected_to_trainer = false;
	emit connected_to_server_signal(false);
	return;
}

/**********************************************************************
	slot
**********************************************************************/

void Computrainer::bytesWritten_slot(qint64 bytes) {
	Q_UNUSED(bytes);
	if (bytes != 9) {
		bp = 1;
	}
	return;
}

/**********************************************************************
	slot
**********************************************************************/

void Computrainer::readyRead_slot() {
	QByteArray a;
	int i, n;

	if (doudp) {
		while (udpsocket->bytesAvailable()) {
			a = udpsocket->readAll();
			n = a.length();
			for (i = 0; i < n; i++) {
				rxq[rxinptr] = a[i];
				rxinptr = (rxinptr + 1) % sizeof(rxq);
			}
		}
	}
	else  {
		/*
		while (tcpsocket->bytesAvailable()) {
			a = tcpsocket->readAll();
			n = a.length();
			for (i = 0; i < n; i++) {
				rxq[rxinptr] = a[i];
				rxinptr = (rxinptr + 1) % sizeof(rxq);
			}
		}
		*/
	}

	return;
}

/**********************************************************************
	slot
**********************************************************************/

void Computrainer::error_slot(QAbstractSocket::SocketError socketEerror) {
	int i = (int)socketEerror;

	qDebug() << "err = " << i;

	switch (i) {
		case 1:
		bp = 1;
		break;
		default:
		bp = 3;
		break;
	}

	bp = 2;

	return;
}

/**********************************************************************
	slot
**********************************************************************/

void Computrainer::state_change_slot(QAbstractSocket::SocketState _socket_state ) {
	socket_state = _socket_state;

	qDebug() << "slot state_change: " << _socket_state;

	switch (socket_state) {
		case QAbstractSocket::UnconnectedState:            // 0
		bp = 1;
		break;
		case QAbstractSocket::ConnectingState:             // 2
		bp = 1;
		break;
		case QAbstractSocket::ConnectedState:              // 3
		bp = 1;
		break;
		case QAbstractSocket::BoundState:                  // 4
		bp = 1;
		break;
		case QAbstractSocket::ClosingState:                // 6
		bp = 1;
		break;
		default:
		bp = 2;
		break;
	}

	return;
}
// slots---

/***********************************************************************************************************************
	 10 ms timer gets us here.
	 polls serial receiver and transmits serial data
***********************************************************************************************************************/

void Computrainer::timer_slot() {
	int i;
	int status;

	//at->update();                 // 10.000 ms

	if (doudp) {
		if (udpsocket == NULL) {
			udpsocket = new QUdpSocket(this);
			Q_ASSERT(udpsocket);
			udpsocket->bind();      // (server_host_addr, tcp_port);
			udpsocket->connectToHost(ipstr, port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
			connect(udpsocket, SIGNAL(connected()), this, SLOT(connected_slot()));
			connect(udpsocket, SIGNAL(disconnected()), this, SLOT(disconnected_slot()));
			connect(udpsocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(state_change_slot(QAbstractSocket::SocketState)));
			connect(udpsocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten_slot(qint64)));        // inherited from QIODevice
			connect(udpsocket, SIGNAL(readyRead()), this, SLOT(readyRead_slot()));                          // inherited from QIODevice
			connect(udpsocket, SIGNAL(error(QAbstractSocket::SocketError socketError)), this, SLOT(error_slot(QAbstractSocket::SocketError socketError)));
		}
	}
	else  {
		/*
		if (tcpsocket == NULL) {
			tcpsocket = new QTcpSocket(this);
			Q_ASSERT(tcpsocket);
			tcpsocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);   // TCP_NODELAY
			tcpsocket->connectToHost(ipstr, port);                            // this is not blocking call
			connect(tcpsocket, SIGNAL(connected()), this, SLOT(connected_slot()));
			connect(tcpsocket, SIGNAL(disconnected()), this, SLOT(disconnected_slot()));
			connect(tcpsocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(state_change_slot(QAbstractSocket::SocketState)));
			connect(tcpsocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten_slot(qint64)));  // inherited from QIODevice
			connect(tcpsocket, SIGNAL(readyRead()), this, SLOT(readyRead_slot()));                    // inherited from QIODevice
			connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError socketError)), this, SLOT(error_slot(QAbstractSocket::SocketError)));
		}
		*/
	}

	if (!connected_to_trainer) {
		if (doudp) {
			if (socket_state == QAbstractSocket::ConnectedState) {
				if ((QDateTime::currentMSecsSinceEpoch() - lastlinkuptime) > 3000) {

					qint64 n = udpsocket->write("RacerMate", 9);

					qDebug() << "sent RacerMate, n = " << n;

					switch (n) {
						case 9:
						bp = 0;
						break;
						case -1:
						bp = 2;
						break;
						default:
						bp = 3;
						break;
					}
					lastlinkuptime = QDateTime::currentMSecsSinceEpoch();
				}
			}
		}
		else  {
			/*
			if (socket_state == QAbstractSocket::ConnectedState) {
				if ((QDateTime::currentMSecsSinceEpoch() - lastlinkuptime) > 3000) {
					qint64 n = tcpsocket->write("RacerMate", 9);
					switch (n) {
						case 9:
						bp = 0;
						break;
						case -1:
						bp = 2;
						break;
						default:
						bp = 3;
						break;
					}
					lastlinkuptime = QDateTime::currentMSecsSinceEpoch();
				}
			}
			*/
		}

		//---------------------------------
		// consume rxq:
		//---------------------------------

		i = 0;

		while (rxoutptr != rxinptr) {
			rxbuf[i] = rxq[rxoutptr];
			rxoutptr = (rxoutptr + 1) % RXQLEN;
			i++;
			if (i == RXBUFLEN - 1) {
				break;
			}
		}

		rxbuf[i] = 0;
		if (!strncmp("LinkUp", (const char*)rxbuf, 6)) {
			connected_to_trainer = true;
			lastCommTime = QDateTime::currentMSecsSinceEpoch();
			Q_ASSERT(connected_to_trainer);
			emit connected_to_trainer_signal(-1, true);
		}

		return;        // no more to do until we're connected to a trainer

	}                 // if (!connected_to_trainer)  {

	Q_ASSERT(socket_state == QAbstractSocket::ConnectedState);



	status = read_data();     // get the next packet, decode it, etc
	if (status == 0) {
		// got new data
		bp = status;
	}

	// send data to trainer

	if (socket_state != QAbstractSocket::ConnectedState) {
		qFatal("oops");
	}

	qint64 now;
	now = QDateTime::currentMSecsSinceEpoch();

#if 0
	if ( (now - lastWriteTime) >= 150) {
		updateHardware();
		lastWriteTime = now;
		socket->flush();
	}
#else
	if ( (now - lastWriteTime) >= 290) {
		updateHardware();
		updateHardware();
		updateHardware();
		lastWriteTime = now;
		if (doudp) {
			udpsocket->flush();
		}
		else  {
//			tcpsocket->flush();
		}
	}
#endif

	Q_ASSERT(connected_to_trainer);
	Q_ASSERT(socket_state == QAbstractSocket::ConnectedState);

	now = QDateTime::currentMSecsSinceEpoch();

	tick++;                 // 0-99 10ms ticks == 1/sec.
	if (tick >= 10) {
		//at->update();				// 100 ms
		tick = 0;
		if (ss) {
			ss->do_ten_hz_filter();

#ifdef _DEBUG
			if (data.mph > 10.0f) {
				bp = 1;
			}
#endif



			data.lata = ss->getlata();
			data.rata = ss->getrata();
			data.ss = ss->getss();
			data.lss = ss->getlss();
			data.rss = ss->getrss();
			data.lwatts = ss->getlwatts();
			data.rwatts = ss->getrwatts();
			data.avgss = ss->getavgss();
			data.avglss = ss->getavglss();
			data.avgrss = ss->getavgrss();

			emit(spinscan_data_signal(-1, ss->getdata()));

#ifdef _DEBUG
			if (data.hr > 10.0f) {
				bp = 3;
			}
#endif
			emit(data_signal(-1, &data));

			if (ss->getRescale()) {
				emit(rescale_signal(-1, qRound(ss->get_max_force())));
			}
		}
	}

	if ((now - lastdisplaytime) >= 500) {
		//display();
		lastdisplaytime = now;
	}

	return;
}                          // timer_slot()

/******************************************************************************
	 returns 0 if there is new VALID (or WAIT) data
******************************************************************************/

int Computrainer::read_data(void) {
	int n;
	int i;
	bool flag = false;
	unsigned long now;
	int rc = 0;
	unsigned char cc;
	int cnt;
	int j;
	unsigned char work;     //, pkt[6];

	i = 0;

	while (rxoutptr != rxinptr) {
		rxbuf[i] = rxq[rxoutptr];
		rxoutptr = (rxoutptr + 1) % RXQLEN;
		i++;
		if (i == RXBUFLEN - 1) {
			break;
		}
	}
	//rxbuf[i] = 0;
	n = i;

	now = QDateTime::currentMSecsSinceEpoch();

	if (n == 0) {     // no data
		if (connected_to_trainer) {
			if ((now - lastCommTime) > 30000) {
				connected_to_trainer = false;
			}
		}
		return 1;
	}

	bp = 2;

	for (i = 0; i < n; i++) {
		packet[packetIndex] = rxbuf[i];
		packetIndex++;
		if (packetIndex > 15) {    // overwrite last char after 16 without
			packetIndex = 15;       // b7 set
		}

		if (rxbuf[i] & 0x80) {
			if (packetIndex == 7) {

				// check the sync byte parity bit

				cc = packet[6];
				cnt = 0;

				for (j = 0; j < 8; j++) {
					if (cc & 0x01) {
						cnt++;
					}
					cc >>= 1;
				}
				if ((cnt % 2) == 0) {
					parity_errors++;
				}
				else  {
					flag = true;

					// convert the 7 realtime bytes to 6 packed bytes for the decoder

					work = (unsigned char)(packet[6] & 0x3f);
					for (j = 0; j < 6; j++) {
						rawpacket[j] = (unsigned char)((packet[j] << 1) | ((work >> (5 - j)) & 0x01));
					}


					//decoder->decode(pkt);

					decode();               // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DECODE <<<<<


					//                    if ( (rpmflags & RPM_VALID) != RPM_VALID )  {
#ifdef DO_ANT
					if (asi) {
						if (cad.decoding_data) {
							set_cadence(cad.get_val());
						}
						else if (sc.decoding_data)
						{
							set_speed_cadence( sc.get_wheel_rpm(), sc.get_speed(), sc.get_cadence());
						}
						else  {
							set_cadence(-1.0f);
						}
					}
#endif
					//                    }

					//                    if ( (hrflags&0x0c)==0x0c)  {
					// hr sensor unplugged, so use the ANT heartrate sensor
					//if (ant_stick_initialized)  {
#ifdef DO_ANT
					if (asi) {
						//set_hr(hr.get_hr());
						set_hr(hr.get_val());
					}
#endif
					//                    }

					if (finishEdge) {
						bp = 0;
					}
					else  {
						if (started) {
							if (!finished) {
								if (!paused) {
									if (logging_type == RM1_PERF_LOGGING) {
#ifdef DO_RMP
										if (rmp) {
											rmpdata.Distance = (float)(TOKM * miles);                   // float, distance in KM
											rmpdata.Watts = (UINT16)ROUND(watts);                       // UINT16
											rmpdata.Watts_Load = load;                                  // float
											rmpdata.Speed = (float)(TOKPH * mph);                       // float
											rmpdata.Cadence = (BYTE)ROUND(rpm);                         // BYTE
											rmpdata.Grade = (INT16)ROUND(sfgrade);                      // INT16
											rmpdata.Wind = (float)(TOKPH * wind);                       // float
											rmpdata.HeartRate = (BYTE)ROUND(hr);                        // BYTE
											rmpdata.Calories = (UINT16)ROUND(calories);                 // UINT16
											rmpdata.PulsePower = (BYTE)ROUND(pp);                       // BYTE
											rmpdata.DragFactor = (BYTE)ROUND(100.0f * rd.watts_factor); // BYTE
											rmpdata.RawCalibrationValue = (INT16)rfmeas;                // INT16
											//rmpdata.FrontGear = (INT16)0;								// INT16
											//rmpdata.RearGear = (INT16)0;								// INT16
											//rmpdata.GearInches = (INT16)0;							// INT16
											rmpdata.TSS = formula->tss;                  // float
											rmpdata.IF = formula->IF;                    // float
											rmpdata.NP = formula->np;                    // float
											rmpdata.SS = (BYTE)ROUND(ss);                // BYTE
											rmpdata.SSLeftATA = (BYTE)lata;              // BYTE
											rmpdata.SSRightATA = (BYTE)rata;             // BYTE
											rmpdata.SSRight = (BYTE)ROUND(rss);          // BYTE
											rmpdata.SSLeft = (BYTE)ROUND(lss);           // BYTE
											rmpdata.SSLeftSplit = (BYTE)ROUND(lwatts);   // BYTE
											rmpdata.SSRightSplit = (BYTE)ROUND(rwatts);  // BYTE
											for (int jj = 0; jj < 24; jj++) {
												rmpdata.bars[jj] = (BYTE)bars[jj];        // BYTE
											}
											rmpdata.m_flags1 = 0;                        // UINT16
											rmpdata.m_flags2 = 0;                        // UINT16

											status = rmp->write_record(&rmpdata);
											if (status) {
												bp = 1;
											}
										}
										else  {
											throw(fatalError(__FILE__, __LINE__, "rmp == null"));
										}
#endif                        // #ifdef DO_RMP
									}
									else  {
										if (outstream) {
											switch (logging_type) {
												case RAW_PERF_LOGGING:
												// 1.0 megs per hour, lp is 10 bytes
												lp.time = ptime;
												memcpy(lp.buf, rawpacket, 6);
												fwrite(&lp, sizeof(lp), 1, outstream);
												recordsOut++;
												break;

												case THREE_D_PERF_LOGGING:
												// about 5 megs per hour, pp is 48 bytes, counting winds and calories
												pp.hr = (unsigned char)qRound(data.hr);
												pp.rpm = (unsigned char)qRound(data.rpm);
												pp.watts = (unsigned short)qRound(data.watts);
												pp.kph = (float)(MPHTOKPH * data.mph);
												pp.ms = ptime;
												pp.pp_s_grade_t_100 = (short)qRound(grade * 100.0f);
												pp.left_ata = (unsigned char)data.lata;
												pp.right_ata = (unsigned char)data.rata;
												pp.kilometers = (float)(MILESTOKM * data.miles);
												pp.leftss = (unsigned char)(qRound(data.lss));
												pp.rightss = (unsigned char)(qRound(data.rss));
												pp.leftsplit = (unsigned char)(qRound(data.lwatts));
												pp.rightsplit = (unsigned char)(qRound(data.rwatts));
												for (int ii = 0; ii < SPIN_SEGS; ii++) {
													pp.spin[ii] = (unsigned char)(qRound(bars[ii]));
												}

												fwrite(&pp, sizeof(PerfPoint), 1, outstream);
												winds.push_back(wind);
												calories.push_back(data.cals);
												recordsOut++;
												break;

												case RM1_PERF_LOGGING:
												bp = 1;
												break;

												default:
												break;
											}
										}  // if (outstream)
									}     // if (logging_type...
								}        // if (!paused)
							}           // if (!finished)
						}              // if (started)
					}
				}                    // sync byte parity check
			}                       // if (packetIndex==7)  {

			packetIndex = 0;
		}           // if (rxbuf[i] & 0x80)  {
	}              // for(i...

	if (hrbeep_enabled) {
		//---------------------------------------
		// check the heartrate limits & beep
		//---------------------------------------

		/*
				BIT 0
					 1 = blink heart
					 0 = solid heart, display 0

				BIT 1
					 1 = show "E" on display
					 0 = don't

				BIT 2
					 1 = below limit, show "LO" and beep slow
					 0 = don't

				BIT 3
					 1 = above "HI" limit, show "HI" and beep fast
					 0 = don't

				if bits 2&3 set, no sensor is installed
		 */

		unsigned char flags = (unsigned char)(data.hrflags & 0x0f);

		if ( (flags & 0x0c) != 0x0c) {               // installed?
			if ( (flags & 0x02) != 0x02) {            // no errors?
				if (data.hr >= rider.upper_hr) {       // user is the same as user
					if ( (now - lastbeeptime) > 400) {  // 2.5 hz
						lastbeeptime = now;
						beep();
					}
				}
				else if (data.hr <= rider.lower_hr)       // user is the same as user
				{
					if (data.hr >= 10) {                   // don't want 0 hr to beep!
						if ( (now - lastbeeptime) > 1000) { // 1 hz
							lastbeeptime = now;
							beep();
						}
					}
				}
			}
		}
	}        // if (hrbeep_enabled)

	connected_to_trainer = true;
	lastCommTime = now;


	if (flag) {
		rc = 0;
	}
	else  {
		rc = 1;              // no data
	}

	//gndx:

	return rc;
}                    // get_next_packet()

/**********************************************************************************************

**********************************************************************************************/

void Computrainer::beep(void) {
	//QApplication :: beep();
	return;
}
void Computrainer::setPaused(bool value) {
	paused = value;
}

void Computrainer::setFinished(bool value) {
	finished = value;
}

void Computrainer::setStarted(bool value) {
	started = value;
}


/**********************************************************************************************

**********************************************************************************************/

/*
	int Computrainer::decode(unsigned char pkt[6])  {
	 Q_UNUSED(pkt);
	 return 0;
	}
 */
/***********************************************************************

	 data comes in at 2400 baud about every 50 ms:

	 12937191: 00 00 00 80 c9 ed
	 12937241: 00 00 00 80 10 00
	 12937291: 00 00 00 80 20 00
	 12937342: 00 00 00 80 3c 00
	 12937392: 00 00 00 80 40 00
	 12937442: 00 00 00 80 50 00
	 12937492: 00 00 00 80 68 00
	 12937542: 00 00 00 80 10 00
	 12937592: 00 00 00 80 70 00
	 12937642: 00 00 00 80 80 c7
	 12937692: 00 00 00 80 92 00
	 12937742: 00 00 00 80 a0 00
	 12937792: 00 00 00 80 ba 01
	 12937842: 00 00 00 80 c9 ed
	 12937892: 00 00 00 80 10 00
	 12937942: 00 00 00 80 20 00


										  RECEIVED PACKER FORMAT:

	BYTE	FUNCTION
	----	--------

	0		spinscan
	1		spinscan
	2		spinscan

	3		key presses
				bit 0		reset key			KEY1
				bit 1		f1 key				KEY4
				bit 2		f3 key				KEY5
				bit 3		plus key				KEY3
				bit 4		f2 key				KEY2
				bit 5		- key					KEY6
			----------------------
				bit 6		spinscan sync
				bit 7		parity

	4		high 4 bits	= id
				0x00				UNUSED?
				0x10				speed
			0x20				power
			0x30				low 8 bits = heart rate
									 high 4 bits = heart rate flags
				0x40				UNUSED?
			0x50				UNUSED?
				0x60				low 8 bits = rpm
									 high 4 bits = rpm flags
				0x70				low 8 bits = min heart rate
									 high 4 bits unused?
				0x80				low 8 bits = max heart rate
									 high 4 bits unused?
				0x90				rf drag
		  0xa0				UNUSED?
			0xb0				status
				0xc0				version
		  0xd0				UNUSED?
		  0xe0				UNUSED?
		  0xf0				UNUSED?


		  low 4 bits	= high 4 bits of data

	5		lower 8 bits of data



												data formats:

	unsigned int	speed					mph*50
	unsigned int	power					power in watts
	unsigned int	heartrate			0 - 255

	unsigned int	hrate_flags						BIT 0
														  1 = blink heart
														  0 = solid heart, display 0

																		  BIT 1
														  1 = show "E" on display
														  0 = don't

																		  BIT 2
														  1 = below limit, show "LO" and beep slow
														  0 = don't

																		  BIT 3
														  1 = above "HI" limit, show "HI" and beep fast
														  0 = don't

														  if bits 2&3 set, no sensor is installed




	unsigned int	load					unused
	unsigned int	spinscan				unused
	unsigned int	rpm					0 - 255

	unsigned int	rpm_flags						BIT 3
														  1 = rpm sensor plugged in
														  0 = rpm sensor not plugged in


	unsigned int	min_hrate			40 - 255, 0 = unset, no alarm
	unsigned int	max_hrate			41 - 255, 0 = unset, no alarm
	unsigned int	rf_drag				pounds*256
	unsigned int	rf_measured			flag
	unsigned int	io						unused
	unsigned int	status				flags
	unsigned int	version				??


	SPINSCAN FORMAT:

	 displayed timer
	 rpm
	 watts
	 mph
	 grade
	 heartrate
	 distance

	 blue.ssraw[24]

	entry:
	 hb_buf[6] has latest raw data (already packed from the 7 byte rs232 packet).


***********************************************************************/

int Computrainer::decode(void) {
	int i;
	unsigned short id, val;
	unsigned short vlow, vhi;
	int j;
	int rc = 0;

	keys = (unsigned char)(rawpacket[3] & 0x3f);             // save the current key presses
	accum_keys |= (unsigned char)(rawpacket[3] & 0x3f);      // save the current key presses along with all key presses since accum_keys was last cleared

	/*
		  remap the keys for velotron-charts compatibility:

		#define KEY1		0x02
		#define KEY2		0x04
		#define KEY3		0x08
		#define KEY4		0x10
		#define KEY5		0x20
		#define KEY6		0x40

				bit 0		reset key			KEY1
				bit 1		f1 key				KEY4
				bit 2		f3 key				KEY5
				bit 3		plus key				KEY3
				bit 4		f2 key				KEY2
				bit 5		- key					KEY6

		newmask[0] = KEY1 = 0x02
		newmask[1] = KEY4 = 0x10
		newmask[2] = KEY5 = 0x20
		newmask[3] = KEY3 = 0x08
		newmask[4] = KEY2 = 0x04
		newmask[5] = KEY6 = 0x40
	 */

	unsigned char cc;
	int cnt;

	cc = rawpacket[3];
	cnt = 0;

	for (j = 0; j < 8; j++) {
		if (cc & 0x01) {
			cnt++;
		}
		cc >>= 1;
	}

	if ((cnt % 2) == 0) {
		parity_errors++;
		keys = 0;
		rc = 1;
	}
	else  {
		// odd parity
		if (keys != lastkeys) {
			unsigned char mask = 0x01;
			for (i = 0; i < 6; i++) {
				if (mask & keys) {
					if ((mask & lastkeys) == 0) {
						keydown |= newmask[i];
					}
				}
				else  {
					if (mask & lastkeys) {
						keyup |= newmask[i];
					}
				}
				mask <<= 1;
			}

			lastkeys = keys;
			keys = keys;
		}
	}

	id = (unsigned short)(rawpacket[4] & 0x00f0);
	vlow = (unsigned short)(rawpacket[5] & 0x00ff);    // bc error
	vhi = (unsigned short)(rawpacket[4] & 0x0f);
	val = (unsigned short)((vhi << 8) | vlow);

	//----------------------------------------------
	// HANDLE THE SPINSCAN:
	// always update the output arrays for record()
	//----------------------------------------------


	if (rawpacket[3] & 0x40) { // spinscan first bar sync flag
		accum_tdc = 1;          // tlm20110822
		sscount = 0;
	}

	if (sscount < NBARS && rawpacket[0]) {
		ssraw[sscount++] = rawpacket[0] ^ XORVAL; // fix norm error in hb
		ssraw[sscount++] = rawpacket[1] ^ XORVAL;
		ssraw[sscount++] = rawpacket[2] ^ XORVAL;
		//at->update();			// 247 ms
	}


	if (sscount == NBARS) {       // got 8 blocks of 3 once every 2.00 seconds
		for (j = 0; j < NBARS; j++) {
			//ssbuf[j] = unpack((unsigned short)ssraw[j]);
			//ss->thrust[j].raw = unpack((unsigned short)ssraw[j]);
			if (ss) {
#ifdef _DEBUG
				float f;
				f = unpack((unsigned short)ssraw[j]);
				if (f > 1000.0f) {
					bp = j;
				}
#endif

				ss->setraw(j, unpack((unsigned short)ssraw[j]));
			}
		}
		sscount++;           // to 25 for idle state
		//at->update();			// 481.1 ms, min/max = 329 / 670 ms
	}

	//-------------------------------
	// HANDLE THE NON-SPINSCAN DATA:
	//-------------------------------

	switch (id) {
		case 0x10:                                   // mph

		data.mph = val / 50.0f;

		if (data.mph > 99.0) {
			data.mph = 99.0;
		}
		mps = (MPHTOMPS * data.mph);        // meters per second
		rawspeed = val / 8000.0f;

		accum_kph += (float)(MPHTOKPH * data.mph);
		accum_kph_count++;

		break;

		case 0x20:                       // watts
		data.watts = val;
		if (data.watts > 3000.0f) {
			data.watts = 0.0f;
		}

		/*
				if (kgs > 0.0f)  {
					 wpkg = watts / kgs;
				}
		 */

		accum_watts += data.watts;
		accum_watts_count++;
		break;

		case 0x30:                          // heartrate
		/*
					 BIT 0
						  1 = blink heart
						  0 = solid heart, display 0

					 BIT 1
						  1 = show "E" on display
						  0 = don't

					 BIT 2
						  1 = below limit, show "LO" and beep slow
						  0 = don't

					 BIT 3
						  1 = above "HI" limit, show "HI" and beep fast
						  0 = don't

					 if bits 2&3 set, no sensor is installed

		 */
		data.hrflags = vhi;
		hrvalid = false;

		if ( (data.hrflags & 0x02) == 0x02) {           // error flag set
		}
		else if ( (data.hrflags & 0x0c) == 0x0c)        // no heart rate sensor installed
		{
		}
		else  {
			hrvalid = true;
		}
		if (hrvalid) {
			data.hr = vlow;
			accum_hr += data.hr;
			accum_hr_count++;
		}
		else  {
			data.hr = -1.0f;
			accum_hr = -1.0f;
			accum_hr_count = 1;
		}

		break;

		case 0x60:                       // rpm
		// note: 3d software will generate an rpm based on speed if there is no rpm signal
		rpmflags = vhi;
		if (vhi & RPM_VALID) {
			pedalrpm = vlow;
			data.rpm = vlow;
			rpmValid = true;
			accum_rpm += data.rpm;
			accum_rpm_count++;
			//rpm_invalid = 0;
		}
		else  {
			// no rpm sensor plugged in
			data.rpm = -1.0f;
			pedalrpm = 0;
			rpmValid = false;
			accum_rpm = -1.0f;
			accum_rpm_count = 1;

			//rpm_invalid++;

			//if (rpm_invalid>3)  {
			//	rpm_invalid = 4;
			//}
		}
		break;

		case 0x70:
		minhr = vlow;                    // is this EVER non-zero?
		break;

		case 0x80:
		maxhr = vlow;                    // is this EVER not 199?
		break;

		case 0x90:
		rfdrag = (unsigned short)(val & 0x07ff);
		rfmeas = (unsigned short)vhi;       // (vhi & 0x08);

		break;

		case 0xb0:
		// 11		cadence pickup
		// 10		heart rate sensor
		//  9		pro or + version, 1 = pro
		//  8		1 if drag factor is operating
		//  7		0
		//  6		0
		//  5		0
		//  4		0
		//  3		0
		//  2		0
		//  1		0
		//  0		1


		hbstatus = val;
		break;

		case 0xc0:
		version = val;                      // 2541, 4095, should be 4543

		/*
				switch(version)  {
					 default:
					 case 2541:								// pre drag factor rom
						  an_do_drag_factor = false;
						  break;
					 case 4095:								// first drag factor rom
						  can_do_drag_factor = true;
						  break;
					 case 3611:
						  can_do_drag_factor = true;
						  break;

				}
		 */

		val = (val % 100);
		if (val <= 34) {
			HB_ERGO_PAUSE = HB_ERGO_RUN;
		}
		break;

		case 0xa0:
		/*
				There are 4.576 holes per inch of travel, so with the gear range you have programmed into
				the velotron 105 to 440 inches/crank turn, the value will be from 480 to 2013.
		 */
		slip = val;          // unsigned short
		slipflag = true;
		break;

		case 0x50:
		raw_rpm = val;
		break;

		// 'id' is unused:

		case 0x00:
		case 0x40:
		case 0xd0:
		case 0xe0:
		case 0xf0:
		break;

		default:
		bp = 1;
		break;
	}


	return rc;
}              // decode()



/**************************************************************************
	Norm: this applies to the xx.33-xx.35 handlebar versions, the only ones we
	ever shipped. Vers xx.39 with encryption is the only one I found that used
	the aerodynamic drag stuff.

	TRANSMITTED PACKET FORMAT:

	 txbuf[0] ... [2] are unused in the handlebar for now

	 txbuf[3] = x x x x x x x x = control flags
 | | | | | | | |
 | | | | | | |  --  unused, set to 0
 | | | | | |  ----  unused, set to 0
 | | | | |  ------  0x04 = make load, 0 = paused, unload user
 | | | |  --------	0x08 = mode control 0
 | | |  ----------  0x10 = mode control 1
 | |  ------------  0x20 = enable spinscan data, 0 = none sent
 |  --------------  transmit rate low bit (see below)
					----------------  transmit rate high bit (see below)

		  transmit rate bits:

				00 = 33.0 hz
				01 = 25 hz
				10 = 20 hz
				11 = 20 hz

		  mode control bits:
				0x00 = old pts load level mode, load is set by id 0x70 data below
				0x08 = wind load mode, set by wind, weight, grade
				0x10 = ergometer mode, at tests, constant watts load set by 0x80 data
				0x18 = rolling friction calibrate mode

	 control flag values: no data sent to hb sets standalone mode

	Norm: mode control bits == 00 still select the "pts" load mode, same as
		  standalone. The pts setting, id 0x70 ranges from 0 to 59.

		  I left the aero and tire drag handicap outputs enabled, so this will
		work if we change the hb code to use them. drag_tire would be used
		for the standalone load level but we should never set this mode anyway.



	//#define	HB_STANDALONE	0x00 dont use this mode.



 |||||||||#define	HB_WIND_RUN		0x2c
 |||||||||#define	HB_WIND_PAUSE	0x28
 |||||||||#define	HB_RFTEST		0x1c
 |||||||||#define	HB_ERGO_RUN		0x14
 |||||||||#define	HB_ERGO_PAUSE	0x10


	 txbuf[4] = x x x x x x x x
 | | | | | | | |
 | | | | | | |  ------ bit  8 of data
 | | | | | |  -------- bit  9 of data
 | | | | |  ---------- bit 10 of data
 | | | |  ------------ bit 11 of data
 | | | |
 | | |  -------------- data id bit 0
 | |  ---------------- data id bit 1
 |  ------------------ data id bit 2
					-------------------- data id bit 3

	 txbuf[5] = bits 0 - 7 of data


	ID INFORMATION:
	---------------

	0x00		unused, ignored

	0x10		grade, %*10, signed
	0x20		wind, mph, signed
	0x30		weight, lbs, unsigned
	0x40		min hr, 0 = no alarms, else 40-254
	0x50		max hr, 41-255, should be > min hr
	0x60		"target hr", ignored; in v39 it's drag_aerodynamic
	0x70		"pts/standalone" load, ignored if mode==00; in v39 is drag_tire
	0x80		power level, in watts for ergometer mode

	0x90		ignored...
	0xa0
	0xb0
	0xc0
	0xd0
	0xe0
	0xf0		request data, currently ignored, hb sends all it has

	note: this maintains is own 10hz timer

	idle packet:

 **********************************************************************/

int Computrainer::updateHardware(bool _force) {          // dataSource:: pure virtual
	Q_UNUSED(_force);
	unsigned char work;

	int i = -1;

	if (doudp) {
		if (udpsocket == NULL) {
			return 0;
		}
	}
	else  {
		/*
		if (tcpsocket == NULL) {
			return 0;
		}
		*/
	}

	/*
		qint64 now;

		now = QDateTime::currentMSecsSinceEpoch();
		if ( (now-lastWriteTime) < 100)  {
		  return 0;
		}
		lastWriteTime = now;
	 */

	tx_select = (unsigned char)((tx_select & 0x70) + 0x10);     // 0x10 to 0x80


	switch (tx_select) {

		case 0x10:                          // grade	(-15.0 to +15.0)
		if (registered) {
			i = (int)qRound(grade * 10);     //decoder->grade;

			if (grade != last_export_grade) {
				last_export_grade = grade;
				/*
						  if (export_stream)  {
								fprintf(export_stream, "%10ld %.3f\n", QDateTime::currentMSecsSinceEpoch(), decoder->grade);
								fflush(export_stream);
						  }
				 */
			}
		}
		else  {
			i = 0;
		}
		is_signed = 1;
		break;

		case 0x20:  {              // wind
			is_signed = 1;
			i = (int)qRound(wind + draft_wind);
			break;
		}

		case 0x30:
		is_signed = 0;
		i = ilbs;
		break;

		case 0x40:
		is_signed = 0;
		i = (int)(.5 + rider.lower_hr);
		i |= 0x0800;                     // set bit 11 for hb to understand
		break;

		case 0x50:
		is_signed = 0;
		i = (int)(.5 + rider.upper_hr);
		i |= 0x0800;                     // set bit 11 for hb to understand
		break;

		case 0x60:  {
			is_signed = 0;
			i = (int)(.5 + 256.0 * aerodrag);    // 8.0 is default, 4095 is max (15.99 lbs)
			break;
		}
		case 0x70:  {
			is_signed = 0;
			double d;
			d = tiredrag * (KGSTOLBS * rider.getkgs());
			i = (int)(.5 + 256 * d);
			break;
		}

		case 0x80:
		is_signed = 0;
		i = (int)(.5 + manwts);
		break;

		default:
		return 0;
	}

	if (is_signed) {                          // signed?
		if (i > 2047) {
			i = 2047;
		}
		if (i < -2048) {
			i = -2048;
		}
	}
	else  {
		if (i < 0) {
			i = 0;
		}
		if (i > 4095) {
			i = 4095;
		}
	}

	/*
				enum COMPUTRAINER_MODE  {
					 HB_WIND_RUN = 0x2c,
					 HB_WIND_PAUSE = 0x28,
					 HB_RFTEST = 0x1c,
					 HB_ERGO_RUN = 0x14,
					 HB_ERGO_PAUSE = 0x10
				};
	 */

	txbuf[4] = (unsigned char)(tx_select | (((unsigned char)(i >> 8)) & 0x0f));
	txbuf[5] = (unsigned char)(i & 0xff);
	txbuf[3] = control_byte;

	txbuf[1] = 0;
	txbuf[2] = 0;


	work = txbuf[1];
	for (i = 2; i < 6; i++) {
		work = (unsigned char)((work + txbuf[i]) & 0xff);
	}
	txbuf[0] = (unsigned char)(work ^ 0xff);


	work = 0x80;                        // header byte


	unsigned char buf[7];


	for (i = 0; i < 6; i++) {
		buf[i] = (char)(txbuf[i] >> 1);
		if (txbuf[i] & 0x01) {
			work ^= pkt_mask[i];
		}
	}
	buf[i] = work;

	bytes_out += 7;

	//at->update();                         // 100 ms
	if (doudp) {
		//QByteArray data;
		//data.append("RacerMate");
		//data = QByteArray(reinterpret_cast<char*>(buf), 7);
		//qint64 n = udpsocket->writeDatagram(data, server_host_addr, tcp_port);
		//udpsocket->flush();
		//qint64 n =
		udpsocket->write((const char*)buf, 7);
	}
	else  {
		//qint64 n =
//		tcpsocket->write((const char*)buf, 7);
		//tcpsocket->flush();
	}

	//socket->flush();

	return 0;

}                                            // updateHardware()

/**********************************************************************
	 unpacks the raw spinscan numbers
**********************************************************************/

float Computrainer::unpack(unsigned short p) {
	unsigned short n;
	float f;

	n = (short)(p & 0x07);           // n = exponent

	if (n) {
		f = (float)(( (p & 0xf8) | 0x100) << (n - 1));
		return f / 512.0f;
	}
	else  {
		f = (float)(p & 0xf8);
		return f / 512.0f;
	}
}

/**********************************************************************

**********************************************************************/

void Computrainer::gradechanged_slot(int _i) {
	igradex10 = _i;
	grade = (float)igradex10 / 10.0f;
#ifdef _DEBUG
	qDebug() << "grade = " << grade;
#endif

	return;
}

/**********************************************************************

**********************************************************************/

void Computrainer::windchanged_slot(float _f) {
	wind = _f;

#ifdef _DEBUG
	//qDebug() << "wind = " << setprecision(1) << wind;
	qDebug() << qSetRealNumberPrecision(1) << "wind = " << wind;
	//qDebug() << "wind = " << QString::num(wind);
#endif

	return;
}

#ifdef _DEBUG

/**********************************************************************

**********************************************************************/

void Computrainer::display(void) {
	int i;
	static int cnt = 0;
	char str[16];

	if (ss) {
		//sprintf(displaystring, "%d  %4.1f mph  %4.1f watts  ss = %.2f", cnt++, mph, watts, ss->getTotalss());
		ssdata = ss->getdata();
		//memset(dispstr, 0, sizeof(dispstr));

		sprintf(displaystring, "%5d ", cnt++);

		for (i = 0; i < NBARS - 1; i++) {
#ifdef TEMP222
			sprintf(str, "%4.1f ", ssdata.val[i]);
#else
			sprintf(str, "%4.1f ", ssdata->val[i]);
#endif
			strcat(displaystring, str);
		}
#ifdef TEMP222
		sprintf(str, "%4.1f ", ssdata.val[i]);
#else
		sprintf(str, "%4.1f", ssdata->val[i]);
#endif
		strcat(displaystring, str);

		int n = strlen(displaystring);         // 125
		if (n > 255) {
			qDebug() << "\noops\n" << displaystring;
			qFatal("zzzxx");
		}


	}
	else  {
		sprintf(displaystring, "%d  %4.1f mph  %4.1f watts", cnt++, data.mph, data.watts);
	}

	qDebug() << displaystring;

	return;
}                          // display()

#endif

/**********************************************************************

**********************************************************************/

qt_SS::BARINFO *Computrainer:: get_barinfo(int i) {

	return ss->get_barinfo(i);

}                          // get_barinfo(int i)


/**********************************************************************

**********************************************************************/

void Computrainer::setRider(const Rider2& _rider) {
	if (started) {
		return;
	}

	rider = _rider;

	return;
}
}

