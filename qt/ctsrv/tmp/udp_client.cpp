#include <QHostAddress>
#include <memory.h>


//#include <comutils.h>

#include <rmdefs.h>

#include <computrainer.h>

#include "udp_client.h"


namespace MYQT  {

/**********************************************************************
	id starts at 0
**********************************************************************/

UDPClient::UDPClient(int _id, QObject *parent) : QObject(parent) {
	id = _id;
	init();
	return;
}

/**********************************************************************

**********************************************************************/

UDPClient::~UDPClient() {
	qDebug() << "UDPClient destructor";
	delete ss;
	delete at;
	return;
}

/**********************************************************************

**********************************************************************/

int UDPClient::init(void) {
	//int n;
	//n = sizeof(FORMAT1);       // 10
#ifdef TESTING
	testing_tick = 0;
#endif

	xseqnum = 0;						// xmit pkt sequence #
	pcdevnum = 12345;
	/*
	struct	PCDATA {
		int	hbdevnum;
		int	control;
		float	f_grade;
		float	f_wind;
		float	f_weight;
		int	minhr;
		int	maxhr;
		int	hrenable;
		float	f_aerodrag;
		float	f_tdrag;
		float	f_manwatts;
	};
	*/
	pcd.hbdevnum = 12345;
	pcd.control = MODE_WIND;
	pcd.f_grade = 0.0f;
	pcd.f_wind = 0.0f;
	pcd.f_weight = 180.0f;
	pcd.minhr = 40;
	pcd.maxhr = 190;
	pcd.hrenable = 0;
	pcd.f_aerodrag = 8.0f;
	 pcd.f_tdrag = 180.0f*0.006f;
	pcd.f_manwatts = 345;


	char str[32];
	sprintf(str, "udpc%02d", id);
	at = new Tmr(str);

	memset(&x, 0, sizeof(x));

	datagrams = 0;
	mstate = 0;                   // pkt finder state machine
	minptr = 0;                   //
	bad_msg = 0;                  // bad pkt counter

	connected = false;
	server_socket = 0;
	peeraddr = "";
	peerport = 0;

	msgversion = 0;
	hbdevnum = 0;
	hbversion = 0;
	msgcnt = 0;
	rxstream = NULL;
	txstream = NULL;

	outpackets = 0;
	bytes_out = 0;
	ipaddr = 0;
	comport = 0;                  // 201-216
	memset(ipstr, 0, sizeof(ipstr));
	ss = NULL;
	ss = new qt_SS(NBARS);

	memset(idle_packet, 0, sizeof(idle_packet));
	idle_packet[6] = 0x80;
	memset(ipstr, 0, sizeof(ipstr));
	memset(rxq, 0, sizeof(rxq));
	memset(rawpacket, 0, sizeof(rawpacket));

	finished = false;
	finishEdge = false;
	hrbeep_enabled = false;
	hrvalid = false;
	paused = false;
	registered = true;
	slipflag = false;
	started = false;

	data.reset();

	aerodrag = 8.0;
	tiredrag = .006f;

	lastidletime = 0L;
	packets = 0L;
	outstream = NULL;
	accum_hr = 0.0f;
	accum_kph = 0.0f;
	accum_rpm = 0.0f;
	accum_watts = 0.0f;

	draft_wind = 0.0f;

	igradex10 = 0;
	grade = (float)igradex10 / 10.0f;

	last_export_grade = 0.0f;
	manwts = 0.0f;
	pedalrpm = 0.0f;
	raw_rpm = 0.0f;
	rawspeed = 0.0f;
	tiredrag = 0.0f;
	wind = 0.0f;
	accum_hr_count = 0;                 // used for ergvid
	accum_kph_count = 0;                // used for ergvid
	accum_rpm_count = 0;                // used for ergvid
	accum_tdc = 0;
	accum_watts_count = 0;              // used for ergvid
	bp = 0;
	ilbs = 0;                           // rd.kgs converted to in lbs for transmitter
	packetIndex = 0;
	parity_errors = 0;                  // keys byte parity bit
	rxinptr = 0;
	rxoutptr = 0;
	serialport = 0;                                             // associated serial port for this client
	sscount = 0;
	tick = 0;
	txinptr = 0;
	txoutptr = 0;
	logging_type = NO_LOGGING;
	socket_state = QAbstractSocket::ConnectedState;          // assume connectedstate for now
	lastbeeptime = 0;
	lastCommTime = 0;
	lastWriteTime = 0;
//	timer = NULL;
	lp.time = 0;
	memset(lp.buf, 0, sizeof(lp.buf));

	accum_keys = 0;
	control_byte = MODE_WIND;

	HB_ERGO_PAUSE = 0x10;
	HB_ERGO_RUN = 0x14;
	is_signed = 0;
	keydown = 0;
	keys = 0;                                // the 6 hb keys + the polar heartrate bit
	keyup = 0;
	lastkeys = 0;
	newmask[0] = KEY1;
	newmask[1] = KEY4;
	newmask[2] = KEY5;
	newmask[3] = KEY3;
	newmask[4] = KEY2;
	newmask[5] = KEY6;

	memset(pkt_mask, 0, sizeof(pkt_mask));

	pkt_mask[0] = 0x60;
	pkt_mask[1] = 0x50;
	pkt_mask[2] = 0x48;
	pkt_mask[3] = 0x44;
	pkt_mask[4] = 0x42;
	pkt_mask[5] = 0x41;

	rpmValid = 0;
	tx_select = 0;
	bytes_in = 0;
	incomplete = 0;
	inpackets = 0;
	ptime = 0;
	recordsOut = 0;
	hbstatus = 0;
	maxhr = 0;
	minhr = 0;
	rfdrag = 0;
	rfmeas = 0;
	rpmflags = 0;
	slip = 0;
	version = 0;
	mps = 0.0;

#if 0
	timer = new QTimer(this);

	connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));

#ifdef _DEBUG
	#ifdef QT_CORE_LIB
		//qDebug() << "client " << id << "starting";
	#endif
#endif

	timer->start(10);
#endif

	return 0;
}                          // CLIENT::init()

#if 1

/**********************************************************************

**********************************************************************/

int UDPClient::SETVAL(int min, int max, int num)  {
	return (num < min ? min : num > max ? max : num) & 0xffff;
}

/**********************************************************************

**********************************************************************/

int UDPClient::send_ctlmsg(int msgtype) {

//	static int	xseqnum = 0;
	unsigned char obuf[256] = { 'R','M','P','C' };

	 int	i, start;
	 //int secstart;
	int	j = 4;			// obuf index starting after RMPC
	 int	csm;        //, cs;

	//fprintf(logfile,"snd_ctlmsg: type= %d  fd= %d\n", msgtype, hb_fd);

	if( msgtype == 0) {
		return 0;
	}
	xseqnum++;
	xseqnum &= 0xffff;
#ifdef TESTING
	testdata.xseqnum = xseqnum;
#endif

	start = j++;			// save payload start ix for overall length, leave room for it
	j++;						// space for ~start

	j = gen_devnum(obuf, j);

	if(msgtype & 0x02) {
		j = gen_hbctl(obuf, j);
	}

	obuf[j++] = 0;				// put section = 0 for end of data

	obuf[start] = j - start;		// put payload size to overall byte count
	obuf[start+1] = ~obuf[start];		// put complement of payload size to overall byte count

	csm = 0;
	for( i=0; i<j; i++) {
		csm += obuf[i];		// checksum from start to sect 0
	}

	obuf[j++] = -csm  & 0xff;	// should add up to zero

//	i = write(hb_fd, obuf, j);
	//fprintf(logfile,"write: j= %d  i= %d\n",j,i);


	if (server_socket) {
		//qint64 i;
		i = server_socket->writeDatagram((const char*)obuf, j, peeraddr, peerport);
		bytes_out += j;
		//at->update();                         // 100 ms
	}

	if (i != j) {
		return -1;
	}
	return 0;
}										// send_ctlmsg()

/*******************************************************************************
	make the devnum secton
*******************************************************************************/

int UDPClient::gen_devnum( unsigned char * buf, int ix)  {
	int j = ix;

	int secstart = j++;		// uber-header start, save space
	buf[j++] = SECT_DEVNUM;

	buf[j++] = (pcdevnum >> 8) & 0xff;
	buf[j++] = pcdevnum & 0xff;


	buf[j++] = (xseqnum >> 8) & 0xff;
	buf[j++] = xseqnum & 0xff;

	buf[secstart] = j - secstart;	// put uber-header data size

	return j;
}													// gen_devnum()

/**********************************************************************
	make the hb control secton
**********************************************************************/

int UDPClient::gen_hbctl( unsigned char * buf, int ix) {
	int j = ix;

	int secstart = j++;		// uber-header start, reserve space

	buf[j++] = SECT_HBCTL;

	buf[j++] = (nhbc.hbdevnum >> 8) & 0xff;
	buf[j++] = nhbc.hbdevnum & 0xff;

	buf[j++] = nhbc.ctl & 0xff;

	buf[j++] = (nhbc.grade >> 8) & 0xff;
	buf[j++] = nhbc.grade & 0xff;

	buf[j++] = nhbc.wind & 0xff;

	buf[j++] = (nhbc.weight >> 8) & 0xff;
	buf[j++] = nhbc.weight & 0xff;

	buf[j++] = (nhbc.manwatts >> 8) & 0xff;
	buf[j++] = nhbc.manwatts & 0xff;

	buf[j++] = nhbc.minhr & 0xff;

	buf[j++] = nhbc.maxhr & 0xff;

	buf[j++] = (nhbc.adrag >> 8) & 0xff;
	buf[j++] = nhbc.adrag & 0xff;

	buf[j++] = (nhbc.tdrag >> 8) & 0xff;
	buf[j++] = nhbc.tdrag & 0xff;

	buf[secstart] = j - secstart;	// put uber-header data size

	return j;
}											// gen_hbctl()


/**********************************************************************

**********************************************************************/

int UDPClient::updateHardware(void) {
	//int	val = 0;

	nhbc.hbdevnum = pcd.hbdevnum & 0xffff;
	nhbc.ctl = pcd.control & 0xff;
	nhbc.grade = SETVAL(-150, 150, (int)(pcd.f_grade * 10.0f));
	nhbc.wind = SETVAL(-50, 50, (int)pcd.f_wind);
	nhbc.weight = SETVAL(0, 500, (int)pcd.f_weight);
	nhbc.manwatts = SETVAL(0, 2000, (int)pcd.f_manwatts);
	nhbc.minhr = pcd.hrenable ?  SETVAL(0,198, pcd.minhr) : 0;
	nhbc.maxhr = pcd.hrenable ?  SETVAL(0,198, pcd.maxhr) : 0;
	nhbc.adrag = SETVAL(0, 4095, (int)(pcd.f_aerodrag * 256.0f));
	nhbc.tdrag = SETVAL(0, 4095, (int)(pcd.f_tdrag * 256.0f));

	//send_ctlmsg(cc[hbport].fd, 3);
	send_ctlmsg(3);

	return 0;
}									// updateHardware()

#else

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



 |||||||||||||||||||||||#define	HB_WIND_RUN		0x2c
 |||||||||||||||||||||||#define	HB_WIND_PAUSE	0x28
 |||||||||||||||||||||||#define	HB_RFTEST		0x1c
 |||||||||||||||||||||||#define	HB_ERGO_RUN		0x14
 |||||||||||||||||||||||#define	HB_ERGO_PAUSE	0x10


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

int UDPClient::updateHardware(bool _force) {          // dataSource:: pure virtual
	Q_UNUSED(_force);
	unsigned char work;

	int i = -1;

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
		i = (int)(.5 + 256.0 * aerodrag);       // 8.0 is default, 4095 is max (15.99 lbs)
		break;
	}
		case 0x70:  {
		is_signed = 0;
		double d;
		d = tiredrag * (TOPOUNDS * rider.getkgs());
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

	if (server_socket) {
		//qint64 n;

		//n =
		server_socket->writeDatagram((const char*)buf, 7, peeraddr, peerport);
		bytes_out += 7;

		//at->update();                         // 100 ms
	}

	return 0;

}                                            // updateHardware()
#endif



/**********************************************************************************************

**********************************************************************************************/

void UDPClient::beep(void) {
	//QApplication :: beep();
	return;
}


#ifdef _DEBUG
/*********************************************************************************************************************************

*********************************************************************************************************************************/

void UDPClient::hexdump(unsigned char *buf, int len, QString pre, QString post) {
	QString s;
	int i;

	s = pre;

	for (i = 0; i < len - 1; i++) {
		//QByteArray::toHex(datagram[i]);
		s += QString::number(buf[i], 16);
		s += " ";
	}
	s += QString::number(buf[i], 16);
	s += post;

	qDebug() << s;
	return;
}
#endif


/*********************************************************************************************************************************
	when the server is udp (not a slot), called from Server::
*********************************************************************************************************************************/

void UDPClient::readDatagram(QUdpSocket *_socket) {
	QString s;
	qint64 n;
	int i;

#ifdef TESTING
	testdata.dgs++;						// count datagrams received
	testdata.len = 0;
#endif

	//at->update();						// 3000 ms

	bp = 1;
	int loops = 0;

	while (_socket->hasPendingDatagrams()) {
		QByteArray a;
		loops++;
		a.resize(_socket->pendingDatagramSize());
		n = _socket->readDatagram(a.data(), a.size(), &peeraddr, &peerport);

#ifdef TESTING
	testdata.len += n;						// count datagrams received
#endif

		for (i = 0; i < n; i++) {
			rxq[rxinptr] = a[i];
			rxinptr = (rxinptr + 1) % sizeof(rxq);
		}
	}

	server_socket = _socket;
}                          // readDatagram()

/*********************************************************************************************************************************
	10 ms timer, one for each handlebar controller, 16 of these are running
*********************************************************************************************************************************/

#if 0

void UDPClient::timer_slot() {
	int n;

#ifdef _DEBUG
	if (id==0)  {
		//at->update();								// 10.0 ms
	}
#endif

	if (server_socket == 0) {
		return;
	}

	n = get_ubermsg();
	//at->update();												// 10 ms

	if (n > 0) {
		//at->update();										// 166.4 ms
#ifdef _DEBUG
		QString s;
#endif
		//unsigned char type;
		unsigned short devnum;
		//unsigned short sequence_number;
		//unsigned char seclen;
		int ix;

		devnum = get_devnum(n);   // test the sect chain and get devnum

#ifdef TESTING
		testdata.rmcts++;
		testdata.devnum = devnum;
//nca+++
		testdata.seqnum = datagram[4] << 8 | datagram[5];
//nca---
#endif

		if (devnum > 0) {                   // -1 or 0? is badness

			ix = datagram[0];            // skip over devnum section

			while ( ix < n && datagram[ix] != 0) {

				switch (datagram[ix + 1]) {						// section types
					case  SECT_FAST:
						//at->update();								// 250.0 ms
						decode_fast(ix + 2, &nhbd);
						//nhbd.keys, nhbd.f_mph, nhbd.watts, nhbd.rpm
#ifdef _DEBUG
						s += " fast";
#endif
						break;
					case  SECT_SLOW:
						//at->update();								// 543.8 ms
						decode_slow(ix + 2, &nhbd);
						//nhbd.f_rdrag, (float)nhbd.version / 100.0F, nhbd.stat
#ifdef _DEBUG
						s += " slow";
#endif
						break;
					case  SECT_SS:
						//at->update();								// 456.9 ms
						decode_ss(ix+2);
#ifdef _DEBUG
						s += " ss";
#endif
						break;

					case  SECT_HERE:
						//at->update();								// 0 ms, not called if I'm transmitting
						//qDebug() <<"got HERE: decoded RealSoonNow";
						break;
					case  SECT_EMPTY:
						//at->update();									// 1000 ms, not called if I'm transmitting
						//qDebug() << "empty";
						break;

					case  SECT_RANDOM:
					case  SECT_RANDOM + 1:
					case  SECT_RANDOM + 2:
					case  SECT_RANDOM + 3:
					case  SECT_RANDOM + 4:
					case  SECT_RANDOM + 5:
					case  SECT_RANDOM + 6:
					case  SECT_RANDOM + 7:
						//qDebug() << "random: " << datagram[ix+1];
						//at->update();										// 1000 ms, not called if transmitting
						break;
					default:
						//qFatal("oops");
						break;
				}					// switch
#ifdef _DEBUG
				//qDebug() << s;
#endif
				ix += datagram[ix];       // skip to next section
			}						// while()
		}							// if (devnum > 0)
		else  {
			qDebug() <<"bad datagram";
		}
	}										// if (n > 0)
#ifdef _DEBUG
	else  {
		//at->update();							// 10.7 ms
	}
#endif

	qint64 now;
	now = QDateTime::currentMSecsSinceEpoch();

	if ( (now - lastWriteTime) >= 300) {
		updateHardware();
		//updateHardware();
		//updateHardware();
		lastWriteTime = now;
	}

	now = QDateTime::currentMSecsSinceEpoch();

	tick++;                       // 0-99 10ms ticks == 1/sec.

	if (tick >= 10) {
		//at->update();				// 100 ms
		tick = 0;
		if (ss) {
			ss->do_ten_hz_filter();

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

			data.mph = nhbd.f_mph;
			data.watts = nhbd.watts;
			data.rpm = nhbd.rpm;
			//data.miles = nhbd.
			data.hr = nhbd.hrate;
			//data.cals;
			data.hrflags = nhbd.hrflags;

			emit(data_signal(id, &data));
			emit(ss_signal(id, ss->getdata()));
			if (ss->getRescale()) {
				emit(rescale_signal(id, qRound(ss->get_max_force())));
			}
		}
	}				// if (tick >= 10), every 100 ms

#ifdef TESTING
	testing_tick++;
	if (testing_tick >= 25) {
		//at->update();				// 250 ms
		testing_tick = 0;
		//testdata.dummy = testdata.dummy + 1;
		emit(testing_signal(id, &testdata));
	}
#endif

	return;
}                       // timer_slot()
#endif				// #if 0

/*********************************************************************************************************************************
	called every 10 ms from ctsrv
*********************************************************************************************************************************/

void UDPClient::process(void)  {
	int n;

#ifdef _DEBUG
	if (id==0)  {
		//at->update();								// 10.0 ms
	}
	//if (this==0xfeeefeee)  {
	//	static int bp = 1;
	//}
#endif

	if (server_socket == 0) {
		return;
	}

	n = get_ubermsg();
	//at->update();												// 10 ms

	if (n > 0) {
		//at->update();										// 166.4 ms
#ifdef _DEBUG
		QString s;
#endif
		//unsigned char type;
		unsigned short devnum;
		//unsigned short sequence_number;
		//unsigned char seclen;
		int ix;

		devnum = get_devnum(n);   // test the sect chain and get devnum

#ifdef TESTING
		testdata.rmcts++;
		testdata.devnum = devnum;
//nca+++
		testdata.seqnum = datagram[4] << 8 | datagram[5];
//nca---
#endif

		if (devnum > 0) {                   // -1 or 0? is badness

			ix = datagram[0];            // skip over devnum section

			while ( ix < n && datagram[ix] != 0) {

				switch (datagram[ix + 1]) {						// section types
					case  SECT_FAST:
						//at->update();								// 250.0 ms
						decode_fast(ix + 2, &nhbd);
						//nhbd.keys, nhbd.f_mph, nhbd.watts, nhbd.rpm
#ifdef _DEBUG
						s += " fast";
#endif
						break;
					case  SECT_SLOW:
						//at->update();								// 543.8 ms
						decode_slow(ix + 2, &nhbd);
						//nhbd.f_rdrag, (float)nhbd.version / 100.0F, nhbd.stat
#ifdef _DEBUG
						s += " slow";
#endif
						break;
					case  SECT_SS:
						//at->update();								// 456.9 ms
						decode_ss(ix+2);
#ifdef _DEBUG
						s += " ss";
#endif
						break;

					case  SECT_HERE:
						//at->update();								// 0 ms, not called if I'm transmitting
						//qDebug() <<"got HERE: decoded RealSoonNow";
						break;
					case  SECT_EMPTY:
						//at->update();									// 1000 ms, not called if I'm transmitting
						//qDebug() << "empty";
						break;

					case  SECT_RANDOM:
					case  SECT_RANDOM + 1:
					case  SECT_RANDOM + 2:
					case  SECT_RANDOM + 3:
					case  SECT_RANDOM + 4:
					case  SECT_RANDOM + 5:
					case  SECT_RANDOM + 6:
					case  SECT_RANDOM + 7:
						//qDebug() << "random: " << datagram[ix+1];
						//at->update();										// 1000 ms, not called if transmitting
						break;
					default:
						//qFatal("oops");
						break;
				}					// switch
#ifdef _DEBUG
				//qDebug() << s;
#endif
				ix += datagram[ix];       // skip to next section
			}						// while()
		}							// if (devnum > 0)
		else  {
			qDebug() <<"bad datagram";
		}
	}										// if (n > 0)
#ifdef _DEBUG
	else  {
		//at->update();							// 10.7 ms
	}
#endif

	qint64 now;
	now = QDateTime::currentMSecsSinceEpoch();

	if ( (now - lastWriteTime) >= 300) {
		updateHardware();
		//updateHardware();
		//updateHardware();
		lastWriteTime = now;
	}

	now = QDateTime::currentMSecsSinceEpoch();

	tick++;                       // 0-99 10ms ticks == 1/sec.

	if (tick >= 10) {
		//at->update();				// 100 ms
		tick = 0;
		if (ss) {
			ss->do_ten_hz_filter();

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

			data.mph = nhbd.f_mph;
			data.watts = nhbd.watts;
			data.rpm = nhbd.rpm;
			//data.miles = nhbd.
			data.hr = nhbd.hrate;
			//data.cals;
			data.hrflags = nhbd.hrflags;

			emit(data_signal(id, &data));
			emit(ss_signal(id, ss->getdata()));
			if (ss->getRescale()) {
				emit(rescale_signal(id, qRound(ss->get_max_force())));
			}
		}
	}				// if (tick >= 10), every 100 ms

#ifdef TESTING
	testing_tick++;
	if (testing_tick >= 25) {
		//at->update();				// 250 ms
		testing_tick = 0;
		//testdata.dummy = testdata.dummy + 1;
		emit(testing_signal(id, &testdata));
	}
#endif

	return;
}

/*********************************************************************************************************************************
	decode section SECT_FAST, fast hb data.
	enter with ix -> keys
*********************************************************************************************************************************/

void	UDPClient::decode_fast(int ix, struct NHBDATA* d)	{
	//int i;
	int j;
	unsigned short in;

	j = ix;

	d->keys = datagram[j++];				// keys
	d->keypress = datagram[j++];			// new key presses

	in = datagram[j++] << 8;				// int mph*256
	in |= datagram[j++];
	d->f_mph = (float)in / 256.0F;

	in = datagram[j++] << 8;				// watts
	in |= datagram[j++];
	d->watts = in;

	d->hrflags = datagram[j++];
	d->hrate = datagram[j++];

	d->rpmflags = datagram[j++];
	d->rpm = datagram[j++];

}								// decode_fast()

/*********************************************************************************************************************************
	read from rxq[], put RMCT packets into buf[]
	looks for RMCT<len><~len><bytes..><csm>
	returns zero if rxq empty or any errors
	returns the size of msg_buf[] on good packet or zero
	msg_buf starts at the device #
*********************************************************************************************************************************/

int UDPClient::get_ubermsg(void) {
	unsigned char c;
	const unsigned char *header = (const unsigned char*)"RMCT";

	while (rxoutptr != rxinptr) {
		c = rxq[rxoutptr];
		rxoutptr = (rxoutptr + 1) % RXQLEN;

		csum += c;                    // add every byte to checksum

		switch (mstate) {

			case  0:                         // looking for "RMCT"
				csum = c;                        // init checksum to first char
			case  1:
			case  2:
			case  3:
				if (c != header[mstate++]) {
					mstate = 0;                // no match, start again
				}
				break;

			case  4:
				msglen = c;             // byte 4 is uberlength from here upto checksum
				mstate++;

				if (msglen < 2 || msglen >= (int)(sizeof(datagram) - 1)) {
					mstate = 0;                // go back to looking for RMCT
					bad_msg++;
#ifdef TESTING
					testdata.bad_msg = bad_msg;
#endif
				}
				break;

			case  5:										// byte 4 is ~uberlength, must match len
				if ((c ^ msglen) != 0xff) {
					mstate = 0;							// go back to looking for RMCT
					bad_msg++;
#ifdef TESTING
					testdata.bad_msg = bad_msg;
#endif
					break;
				}

				minptr = 0;								// start buffer on first section size byte
				datalen = msglen - 2;				// starting after msglen, ~msglen
				mstate++;
				break;

			case  6:										// reading msglen chars into buf
				datagram[minptr++] = c;
				if (minptr >= datalen) {
					mstate++;							// adv state after last data, next char will be csm
				}
				break;

			case  7:										// csm byte came in
				mstate = 0;
				if (csum & 0xff) {
					bad_msg++;
#ifdef TESTING
					testdata.bad_msg = bad_msg;
#endif
					minptr = 0;							// return 0 on bad
					return 0;
				}
				return minptr;							// return size of datagram

			default:
				mstate = 0;
				break;
		}						// switch

	}     // while()

	return 0;
}									// get_ubermsg

/*********************************************************************************************************************************
	walk datagram checking for the sections, stop at len
	return device# if walk is ok, and first sect is
	SECT_DEVNUM else -1
*********************************************************************************************************************************/

int UDPClient::get_devnum(int len) {
	int i;
	//int j = 0;
	int devnum = -1;

	i = datagram[0];

	for( i = datagram[0]; i <= len; i += datagram[i]) {
		if( datagram[i] == 0)	{
			break;
		}
	}
	if(datagram[1] == SECT_DEVNUM)	{
		devnum = datagram[2] << 8 | datagram[3];
	}
	return devnum;
}


/*********************************************************************************************************************************
	section SECT_SLOW, enter with ix at minhr
*********************************************************************************************************************************/

void	UDPClient::decode_slow(int ix, struct NHBDATA* d)	{
	int in,j;

	j = ix;

	d->minhr	=	datagram[j++];
	d->maxhr	=	datagram[j++];

	in = datagram[j++] << 8;				// rdrag * 256
	in |= datagram[j++];
	d->rd_measured =  (in & 0x8000) ? in &= 0x7fff, 1 : 0;
	d->f_rdrag = (float)in / 256.0F;


	in = datagram[j++] << 8;				// slip count
	in |= datagram[j++];
	d->slip = in;

	in = datagram[j++] << 8;				// version*100
	in |= datagram[j++];
	d->version = in;

	in = datagram[j++] << 24;				// status ms byte
	in |= datagram[j++] << 16;
	in |= datagram[j++] << 8;
	in |= datagram[j++];				// status ls byte
	d->stat = in;

#ifdef TESTING
	testdata.histatus = d->stat;
#endif

}								// decode_slow()

/*********************************************************************************************************************************
	decode spinscan into struct nhbdata
	entry ix -> turn hby
	returns the # of new crank events, 0 is no new ones
	if ss isnt NULL load ss[24] with thrust in lbs
*********************************************************************************************************************************/

int UDPClient::decode_ss(int ix)	{
	unsigned short turns, time, peak;
	float f_peak;
	int i,j;

	j = ix;
	turns = datagram[j++] << 8;
	turns |= datagram[j++];

	time = datagram[j++] << 8;
	time |= datagram[j++];

	peak = datagram[j++] << 8;	//highest bar is 200. peak is lbs*256 at bar==200
	peak |= datagram[j++];
	f_peak = (float)peak /( 200.0F * 256.0F);		// bar[]*f_peak -> lbs thrust
	nhbd.f_peak = f_peak;

	for(i = 0; i < 24; i++) {
		nhbd.bars[i] = datagram[j++];
		ss->setraw(i, nhbd.f_peak*nhbd.bars[i]);
	}

	nhbd.deltat = (time - nhbd.prevtime) & 0xffff;

	if((nhbd.turns = (turns - nhbd.prevturns)) & 0xffff) {
		nhbd.prevturns = turns;
		nhbd.prevtime = time;
	}

	return nhbd.turns;
}													// decode_ss()

/**********************************************************************
	signal from server
**********************************************************************/

void UDPClient::gradechanged_slot(int _igradex10) {
	pcd.f_grade = _igradex10 / 10.0f;
	return;
}

/**********************************************************************
	signal from server
**********************************************************************/

void UDPClient::windchanged_slot(float _f) {
		pcd.f_wind = _f;
	return;
}

// rm1 compatibility+++

/**********************************************************************

**********************************************************************/

float UDPClient::get_watts_factor(void)  {
	return 0.0f;
}

/**********************************************************************

**********************************************************************/

int UDPClient::end_cal(void)  {
	//ctc.ctl = lastctl;
	control_byte = lastctl;
	return 0;
}

/**********************************************************************

**********************************************************************/

float *UDPClient::get_average_bars(void)  {
	return 0;
}

/**********************************************************************

**********************************************************************/

float *UDPClient::get_bars(void)  {
	return meta.bars;
}

/**********************************************************************

**********************************************************************/

float UDPClient::get_ftp(void)  {
	/*
	if (formula) {
		float f = formula->get_ftp();
		return f;
	}
	*/
	return 0.0f;

}

/**********************************************************************

**********************************************************************/

int UDPClient::get_handlebar_buttons(void)  {
	return keys;
}

/**********************************************************************

**********************************************************************/

bool UDPClient::is_paused(void)  {
	return paused;

}

/**********************************************************************

**********************************************************************/

int UDPClient::reset_stats(void)  {
	/*
	for (int i=0; i<NSTATS; i++) {
		stats[i].total = 0.0;
		stats[i].count = 0L;
		stats[i].avg = 0.0;
		stats[i].min = 0.0;
		stats[i].max = 0.0;
	}
	*/
	return 0;

}

/**********************************************************************

**********************************************************************/

int UDPClient::reset(void)  {
	reset_stats();
	return 0;
}

/**********************************************************************

**********************************************************************/

int UDPClient::rx(unsigned char *_buf, int _buflen)  {
	Q_UNUSED(_buf);
	Q_UNUSED(_buflen);
	return 0;
}

/**********************************************************************
	#define MODE_WIND               0x2c
	#define MODE_WIND_PAUSE         0x28
	#define MODE_RFTEST             0x1c
	#define MODE_ERGO               0x14
	#define MODE_ERGO_PAUSE         0x10

**********************************************************************/

int UDPClient::set_ergo_mode(int _fw, unsigned short _rrc, float _manwatts)  {
	Q_UNUSED(_fw);

	control_byte = MODE_ERGO;
	f_manwatts = _manwatts;
	tdrag = (unsigned short)_rrc;
	return 0;

}

/**********************************************************************

**********************************************************************/

void UDPClient::set_export(const char *_dbgfname)  {

	sprintf(xprtname, "%s\\slope_%d.txt", _dbgfname, id+1);
	export_stream = fopen(xprtname, "wt");
	return;

}

/**********************************************************************

**********************************************************************/

void UDPClient::set_file_mode(bool _mode)  {
	filemode = _mode;
	return;

}

/**********************************************************************

**********************************************************************/

void UDPClient::set_ftp(float _ftp)  {
	Q_UNUSED(_ftp);
//	if (formula)  {
//		formula->set_ftp(_ftp);
//	}
	return;

}

/**********************************************************************

**********************************************************************/

int UDPClient::set_hr_bounds(int _minhr, int _maxhr, bool _beepEnabled)  {
	Q_UNUSED(_minhr);
	Q_UNUSED(_maxhr);
	BeepEnabled = _beepEnabled;

//	ctc.minhr = _minhr;
//	ctc.maxhr = _maxhr;

	return 0;

}

/**********************************************************************

**********************************************************************/

void UDPClient::set_paused(bool _paused)  {
	Q_UNUSED(_paused);
	/*
	if (ctc.ctl == CT_MODE_RFTEST) {
		return;
	}

	paused = _paused;

	if (ctc.ctl == CT_MODE_ERGO) {
		ctc.ctl = CT_MODE_ERGO_PAUSE;
	}
	else if (ctc.ctl == CT_MODE_WIND) {
		ctc.ctl = CT_MODE_WIND_PAUSE;
	}
	else if (ctc.ctl == CT_MODE_ERGO_PAUSE) {
		ctc.ctl = CT_MODE_ERGO;
	}
	else if (ctc.ctl == CT_MODE_WIND_PAUSE) {
		ctc.ctl = CT_MODE_WIND;
	}
	else {
		// do nothing for any other modes
	}

	//	if (paused) ctc.ctl &= 0xfb;
	//	else ctc.ctl |= 0x04;
	*/

	return;

}

/**********************************************************************

**********************************************************************/

int UDPClient::set_slope(float _bike_kgs, float _person_kgs, int _drag_factor, float _grade)  {
	Q_UNUSED(_bike_kgs);
	Q_UNUSED(_person_kgs);
	Q_UNUSED(_drag_factor);
	Q_UNUSED(_grade);

//	ctc.ctl = CT_MODE_WIND;
	f_grade = _grade;
	f_weight = (float)(KGSTOLBS*(_bike_kgs + _person_kgs));
	f_aerodrag = 8.0f;			// f(_drag_factor)
	return 0;

}

/**********************************************************************

**********************************************************************/

int UDPClient::start_cal(void)  {
	lastctl = control_byte;
	control_byte = MODE_RFTEST;

	//lastctl = ctc.ctl;
	//ctc.ctl = CT_MODE_RFTEST;
	return lastctl;

}

// rm1 compatibility---

}
