#include <memory.h>
#include <assert.h>

#include <utils.h>
#include "udp_client.h"

/**********************************************************************
	id starts at 0
**********************************************************************/

UDPClient::UDPClient(int _id)  {
	id = _id;

	if (id != 0) {
		bp = 1;
	}

	init();
	return;
}

/**********************************************************************

**********************************************************************/

UDPClient::~UDPClient()  {

	DEL(formula);
	FCLOSE(export_stream);

	DEL(ss);

	if (run_event != 0xffffffff) {
		timeKillEvent(run_event);
		run_event = 0xffffffff;
		//Sleep(20);			// ??? to be safe??
	}

	DEL(at);
	FCLOSE(outstream);
	return;
}

/**********************************************************************

**********************************************************************/

int UDPClient::init(void) {
#ifdef TESTING
	testing_tick = 0;
#endif

	lastctl = 0;
	memset(str, 0, sizeof(str));
	memset(fbars, 0, sizeof(fbars));

	formula = NULL;
	memset(xprtname, 0, sizeof(xprtname));
	export_stream = NULL;

	memset(&td, 0, sizeof(td));

	memset(&ctc, 0, sizeof(ctc));
	ctc.minhr = 50;
	ctc.maxhr = 120;
	ctc.ctl = CT_MODE_WIND;

	BeepEnabled = true;

	filemode = false;
	sprintf(portname, "COM%d", id + UDP_SERVER_SERIAL_PORT_BASE);

	last_connected_time = 0L;

	peeraddr = NULL;

	xseqnum = 0;						// xmit pkt sequence #
	pcdevnum = 12345;

	hbdevnum = 12345;
	f_grade = 0.0f;
	f_wind = 0.0f;
	f_weight = 180.0f;
	minhr = 40;
	maxhr = 190;
	hrenable = 0;
	f_aerodrag = 8.0f;
	f_tdrag = 180.0f*0.006f;
	f_manwatts = 345;

	char str[32];
	sprintf(str, "udp_client_%02d", id);
	at = new AverageTimer(str);

	memset(&x, 0, sizeof(x));

	datagrams = 0;
	mstate = 0;                   // pkt finder state machine
	minptr = 0;                   //
	bad_msg = 0;                  // bad pkt counter

	connected = false;
	server_socket = INVALID_SOCKET;
	peerport = 0;

	msgversion = 0;
	hbdevnum = 0;
	hbversion = 0;
	msgcnt = 0;

	outpackets = 0;
	bytes_out = 0;
	ipaddr = 0;
	comport = 0;                  // 201-216
	memset(ipstr, 0, sizeof(ipstr));
	ss = NULL;
	ss = new SS();

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

	//data.reset();

	aerodrag = 8.0;
	tiredrag = .006f;

	lastidletime = 0L;
	packets = 0L;
	sprintf(str, "client%d.log", id);
	outstream = NULL;
#ifdef _DEBUG
	//outstream = fopen(str, "wt");
#endif

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
	lastbeeptime = 0;
	lastCommTime = 0;
	lastWriteTime = 0;
	//lp.time = 0;
	//memset(lp.buf, 0, sizeof(lp.buf));

	accum_keys = 0;

	HB_ERGO_PAUSE = 0x10;
	HB_ERGO_RUN = 0x14;
	is_signed = 0;
	keydown = 0;
	keys = 0;                                // the 6 hb keys + the polar heartrate bit
	keyup = 0;
	lastkeys = 0;

	/*
	newmask[0] = KEY1;
	newmask[1] = KEY4;
	newmask[2] = KEY5;
	newmask[3] = KEY3;
	newmask[4] = KEY2;
	newmask[5] = KEY6;
	*/

	newmask[0] = 0x01;
	newmask[1] = 0x02;
	newmask[2] = 0x08;
	newmask[3] = 0x10;
	newmask[4] = 0x04;
	newmask[5] = 0x20;

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
	run_event = 0xffffffff;

	if (run_event == 0xffffffff) {
		run_event = timeSetEvent(10, 1, timer_slot, (DWORD)this, TIME_PERIODIC);
	}

	memset(&meta, 0, sizeof(meta));
	memset(&ssdata, 0, sizeof(ssdata));

	return 0;
}                          // CLIENT::init()


/**********************************************************************

**********************************************************************/

int UDPClient::SETVAL(int min, int max, int num)  {
	return (num < min ? min : num > max ? max : num) & 0xffff;
}


/*******************************************************************************************
calculates spinscan
*******************************************************************************************/

/*
void UDPClient::calc_ss(void) {
	ssdata.ss = 0.0f;
	ssdata.lss = 0.0f;
	ssdata.rss = 0.0f;
	ssdata.lsplit = 0.0f;
	ssdata.rsplit = 0.0f;

	return;
}
*/


/**********************************************************************

**********************************************************************/

int UDPClient::send_ctlmsg(int msgtype) {

	unsigned char obuf[256] = { 'R','M','P','C' };

    int	i, start;
	int	j = 4;			// obuf index starting after RMPC
    int	csm;        //, cs;

	if( msgtype == 0) {
		return 0;
	}
	xseqnum++;
	xseqnum &= 0xffff;
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

	if (server_socket != INVALID_SOCKET)  {
		int n;
		//at->update();                         // 300 ms

		n = sendto(
			server_socket,
			(const char*)obuf,
			j,
			0,
			(struct sockaddr *) peeraddr,
			sizeof(sockaddr_in)
			);

		if (n != j) {
			bp = 1;
		}

		bytes_out += n;
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

	buf[j++] = (ctc.hbdevnum >> 8) & 0xff;
	buf[j++] = ctc.hbdevnum & 0xff;

	buf[j++] = ctc.ctl;

	buf[j++] = (ctc.grade >> 8) & 0xff;
	buf[j++] = ctc.grade & 0xff;

	buf[j++] = ctc.wind & 0xff;

	buf[j++] = (ctc.weight >> 8) & 0xff;
	buf[j++] = ctc.weight & 0xff;

	buf[j++] = (ctc.manwatts >> 8) & 0xff;
	buf[j++] = ctc.manwatts & 0xff;

	buf[j++] = ctc.minhr & 0xff;

	buf[j++] = ctc.maxhr & 0xff;

	buf[j++] = (ctc.adrag >> 8) & 0xff;
	buf[j++] = ctc.adrag & 0xff;

	buf[j++] = (ctc.tdrag >> 8) & 0xff;
	buf[j++] = ctc.tdrag & 0xff;

	buf[secstart] = j - secstart;	// put uber-header data size

	return j;
}											// gen_hbctl()


/**********************************************************************

**********************************************************************/

int UDPClient::updateHardware(void) {
	ctc.hbdevnum = hbdevnum & 0xffff;
	ctc.grade = SETVAL(-150, 150, (int)(f_grade * 10.0f));
	ctc.wind = SETVAL(-50, 50, (int)f_wind);
	ctc.weight = SETVAL(0, 500, (int)f_weight);
	ctc.manwatts = SETVAL(0, 2000, (int)f_manwatts);
	ctc.minhr = hrenable ? SETVAL(0, 198, minhr) : 0;
	ctc.maxhr = hrenable ? SETVAL(0, 198, maxhr) : 0;
	ctc.adrag = SETVAL(0, 4095, (int)(f_aerodrag * 256.0f));
	ctc.tdrag = SETVAL(0, 4095, (int)(f_tdrag * 256.0f));

	send_ctlmsg(3);

	return 0;
}									// updateHardware()




/**********************************************************************************************

**********************************************************************************************/

void UDPClient::beep(void) {
	//QApplication :: beep();
	return;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

void UDPClient::readDatagram(int _socket, struct sockaddr_in *_peeraddr, unsigned char *_buf, int _n)  {
	int i;

	//at->update();						// 3000 ms

	peeraddr = _peeraddr;
	server_socket = _socket;

	bp = 1;

	assert(id == 0);

	for (i=0; i<_n; i++)  {
		rxq[rxinptr] = _buf[i];
		rxinptr = (rxinptr + 1) % sizeof(rxq);
	}
}                          // readDatagram()



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
10 ms timer, one for each handlebar controller, 16 of these are running
*********************************************************************************************************************************/

void CALLBACK UDPClient::timer_slot(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2) {
	int ix, n, devnum=-1;

	UDPClient *client = (UDPClient *)dwUser;

#ifdef _DEBUG
	if (client->id == 0) {
		//client->at->update();								// 10.0 ms
	}
#endif

	n = client->get_ubermsg();
	//at->update();												// 10 ms

	if (n > 0) {
		//at->update();										// 166.4 ms

		devnum = client->get_devnum(n);   // test the sect chain and get devnum

		if (devnum > 0) {                   // -1 or 0? is badness
			client->connected = true;
			client->last_connected_time = timeGetTime();

			ix = client->datagram[0];            // skip over devnum section

			while (ix < n && client->datagram[ix] != 0) {

				switch (client->datagram[ix + 1]) {						// section types
					case  SECT_FAST:
						//client->at->update();								// 250.0 ms
						client->decode_fast(ix + 2);

						// keys, keypress, hrflags, rpmflags
						client->stats[STAT_HR].update(client->hrate);
						client->stats[STAT_MPH].update(client->f_mph);
						client->stats[STAT_WATTS].update(client->watts);
						client->stats[STAT_RPM].update(client->rpm);

						//s += " fast";
						break;

					case  SECT_SLOW:
						//client->at->update();								// 541.7 ms
						// minhr, maxhr, rd_measured, f_drag, slip, version, stat
						client->decode_slow(ix + 2);

						//f_rdrag, (float)version / 100.0F, stat
						//s += " slow";
						break;
					case  SECT_SS:
						//client->at->update();								// 460 ms
						client->decode_ss(ix + 2);
						//client->calc_ss();
						break;

					case  SECT_HERE:
						client->bp = 1;
						//client->at->update();								// 3000 ms, not called if I'm transmitting
						break;

					case  SECT_EMPTY:
						//at->update();									// 1000 ms, not called if I'm transmitting
						client->bp = 1;
						break;

					case  SECT_RANDOM:
					case  SECT_RANDOM + 1:
					case  SECT_RANDOM + 2:
					case  SECT_RANDOM + 3:
					case  SECT_RANDOM + 4:
					case  SECT_RANDOM + 5:
					case  SECT_RANDOM + 6:
					case  SECT_RANDOM + 7:
						//at->update();										// 1000 ms, not called if transmitting
						client->bp = 1;
						break;
					default:
						//qFatal("oops");
						client->bp = 1;
						break;
				}					// switch
				ix += client->datagram[ix];       // skip to next section
			}						// while()
		}							// if (devnum > 0)
		else {
			OutputDebugString("bad datagram");
		}
	}										// if (n > 0)
#ifdef _DEBUG
	else {
		client->bp = 1;
		//client->at->update();							// 10.7 ms
	}
#endif

	if (devnum <= 0 || n <= 0)  {					// no data or bad devnum
		unsigned long dt = timeGetTime() - client->last_connected_time;
		if (dt > 3000)  {
			client->connected = false;
		}
	}

	unsigned long now;
	now = timeGetTime();

	if ((now - client->lastWriteTime) >= 300) {
		//client->at->update();							//  300.0 ms
		client->updateHardware();
		//updateHardware();
		//updateHardware();
		client->lastWriteTime = now;
	}

	now = timeGetTime();

	client->tick++;                       // 0-99 10ms ticks == 1/sec.

	if (client->tick >= 10) {
		//client->at->update();				// 100 ms
		client->tick = 0;
		if (client->ss) {

			client->ss->do_ten_hz_filter(&client->meta, client->started, client->finished, client->paused);

			client->ssdata.lss = client->meta.lss;
			client->ssdata.rss = client->meta.rss;
			client->ssdata.ss = client->meta.ss;
			client->ssdata.lsplit = client->meta.lwatts;
			client->ssdata.rsplit = client->meta.rwatts;

			/*
			client->data.lata = (float)client->meta.lata;
			client->data.rata = (float)client->meta.rata;
			client->data.ss = client->meta.ss;
			client->data.lss = client->meta.lss;
			client->data.rss = client->meta.rss;
			client->data.lwatts = client->meta.lwatts;
			client->data.rwatts = client->meta.rwatts;
			client->data.avgss = client->meta.average_ss;
			client->data.avglss = client->meta.average_lss;
			client->data.avgrss = client->meta.average_rss;

			client->data.mph = client->f_mph;
			client->data.watts = client->watts;
			client->data.rpm = client->rpm;
			client->data.hr = client->hrate;
			client->data.hrflags = client->hrflags;
			*/

		}
	}				// if (tick >= 10), every 100 ms


	return;
}                       // timer_slot()

/*********************************************************************************************************************************

*********************************************************************************************************************************/

int UDPClient::rx(unsigned char *_buf, int _buflen) {

	return 0;
}

/*********************************************************************************************************************************

*********************************************************************************************************************************/

DEVICE UDPClient::get_device(void) {
	d.ix = id;
	d.commtype = TRAINER_IS_CLIENT;
	strncpy(d.portname, portname, sizeof(d.portname)-1);
	d.what = DEVICE_COMPUTRAINER;
	strcpy(d.nv.ver, "4095");
	strcpy(d.nv.cal, "2.00 U");
	d.when = timeGetTime();

	return d;
}

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::reset(void)  {
	reset_stats();

	return 0;
}										// reset()

/*******************************************************************************************

*******************************************************************************************/

float UDPClient::get_ftp(void)  {
	if (formula) {
		float f = formula->get_ftp();
		return f;
	}
	return 0.0f;
}

/*******************************************************************************************

*******************************************************************************************/

void UDPClient::set_ftp(float _ftp)  {
	if (formula)  {
		formula->set_ftp(_ftp);
	}
	return;
}

/*******************************************************************************************

*******************************************************************************************/

void UDPClient::set_export(const char *_dbgfname)  {
	sprintf(xprtname, "%s\\slope_%d.txt", _dbgfname, id+1);
	export_stream = fopen(xprtname, "wt");
	return;
}

/*******************************************************************************************

*******************************************************************************************/

void UDPClient::set_file_mode(bool _mode)  {
	filemode = _mode;
	return;
}


/*******************************************************************************************

*******************************************************************************************/

int UDPClient::set_hr_bounds(int _minhr, int _maxhr, bool _beepEnabled)  {
	BeepEnabled = _beepEnabled;

	ctc.minhr = _minhr;
	ctc.maxhr = _maxhr;

	return 0;
}

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::reset_stats(void)  {
	for (int i=0; i<NSTATS; i++) {
		stats[i].total = 0.0;
		stats[i].count = 0L;
		stats[i].avg = 0.0;
		stats[i].min = 0.0;
		stats[i].max = 0.0;
	}

	return 0;
}

/*******************************************************************************************

*******************************************************************************************/

bool UDPClient::is_paused(void)  {
	return paused;
}

/*******************************************************************************************

*******************************************************************************************/

float UDPClient::get_watts_factor(void)  {						// tdx
	//pcd.f_aerodrag = 8.0f;			// f(_drag_factor)
	//ctc.adrag = xx;

	return 0.0f;
}


/*******************************************************************************************

*******************************************************************************************/

TrainerData UDPClient::GetTrainerData(int _fw)  {
	// keys, keypress, hrflags, rpmflags
	// minhr, maxhr, rd_measured, f_drag, slip, version, stat
	// f_peak, bars[], deltat, turns, prevturns, prevtime

	td.cadence = rpm;
	td.HR = hrate;
	td.kph = (float) (MPHTOKPH*f_mph);
	td.Power = watts;

	return td;
}

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::get_handlebar_buttons(void)  {
	return keys;
}

/*******************************************************************************************

*******************************************************************************************/

float * UDPClient::get_bars(void)  {
	//return (float *)fbars;
	return meta.bars;

	//return nullptr;
}

/*******************************************************************************************

*******************************************************************************************/

float * UDPClient::get_average_bars(void)  {
	return nullptr;
	//return meta.
}

/*******************************************************************************************

*******************************************************************************************/

SSDATA UDPClient::get_ss_data(void)  {
	return ssdata;
	//return nullptr;
}

/*********************************************************************************************************************************
decode section SECT_FAST, fast hb data.
enter with ix -> keys
*********************************************************************************************************************************/

void	UDPClient::decode_fast(int ix) {
	int j;
	unsigned short in;

	j = ix;

	keys = datagram[j++];				// keys

	/*

				raw			app

	reset		0x01			0x01
	f1			0x02			0x02
	f3			0x04			0x08
	+			0x08			0x10
	f2			0x10			0x04
	-			0x20			0x20

	define newmask as the app-level bits
	*/

	int i;
	unsigned char uc = 0;
	unsigned char mask = 0x01;

	for (i=0; i<6; i++)  {
		if (mask & keys)  {
			uc |= newmask[i];
		}
		mask <<= 1;
	}
	keys = uc;

	keypress = datagram[j++];			// new key presses

	in = datagram[j++] << 8;				// int mph*256
	in |= datagram[j++];
	f_mph = (float)in / 256.0F;

	in = datagram[j++] << 8;				// watts
	in |= datagram[j++];
	watts = in;

	hrflags = datagram[j++];
	hrate = datagram[j++];

	rpmflags = datagram[j++];
	rpm = datagram[j++];

	if (outstream) {
		//sprintf(str, "fast: keypress = %02x, keys = %02x, hrflags = %02x, rpmflags = %04x\n", keypress, keys, hrflags, rpmflags);
		//fprintf(outstream, str);
		//fflush(outstream);
		//printf("%s", str);
	}

	return;
}								// decode_fast()

/*********************************************************************************************************************************
section SECT_SLOW, enter with ix at minhr
*********************************************************************************************************************************/

void	UDPClient::decode_slow(int ix) {
	int in, j;

	j = ix;

	minhr = datagram[j++];
	maxhr = datagram[j++];

	in = datagram[j++] << 8;				// rdrag * 256
	in |= datagram[j++];
	rd_measured = (in & 0x8000) ? in &= 0x7fff, 1 : 0;				// 1 = calibrated, 0 = not calibrated
	f_rdrag = (float)in / 256.0F;											// eg, 2.36f


	in = datagram[j++] << 8;				// slip count
	in |= datagram[j++];
	slip = in;

	in = datagram[j++] << 8;				// version*100
	in |= datagram[j++];
	version = in;

	in = datagram[j++] << 24;				// status ms byte
	in |= datagram[j++] << 16;
	in |= datagram[j++] << 8;
	in |= datagram[j++];				// status ls byte
	stat = in;

#ifdef TESTING
	testdata.histatus = d->stat;
#endif

	if (outstream) {
		//sprintf(str, "slow: ver = %d, slip = %d, rdm = %d, fdrag = %.3f, minhr = %d, maxhr = %d, stat = %04x\n", version, slip, rd_measured, f_rdrag, minhr, maxhr, stat);
		//fprintf(outstream, "%s", str);
		//fflush(outstream);
		//printf("%s", str);
	}

	return;
}								// decode_slow()

/*********************************************************************************************************************************
	decode spinscan into struct nhbdata
	entry ix -> turn hby
	returns the # of new crank events, 0 is no new ones
	if ss isnt NULL load ss[24] with thrust in lbs
*********************************************************************************************************************************/

int UDPClient::decode_ss(int ix) {
	unsigned short turns, time, peak;
	float f_peak;
	int i, j;

	j = ix;
	turns = datagram[j++] << 8;
	turns |= datagram[j++];

	time = datagram[j++] << 8;
	time |= datagram[j++];

	peak = datagram[j++] << 8;	//highest bar is 200. peak is lbs*256 at bar==200
	peak |= datagram[j++];
	f_peak = (float)peak / (200.0F * 256.0F);		// bar[]*f_peak -> lbs thrust
	//f_peak = f_peak;


	for (i = 0; i < 24; i++) {
		bars[i] = datagram[j++];
		//meta.bars[i] = f_peak*bars[i];
		fbars[i] = f_peak*bars[i];
	}

	ss->setraw(fbars);

	deltat = (time - prevtime) & 0xffff;

	if ((turns = (turns - prevturns)) & 0xffff) {
		prevturns = turns;
		prevtime = time;
	}


	if (outstream) {
		char str2[16];

		sprintf(str, "bars: ");
		for (i = 0; i<12; i++) {
			//sprintf(str2, "%3d ", bars[i]);
			sprintf(str2, "%5.2f ", meta.bars[i]);
			strcat(str, str2);
		}

		strcat(str, "    ");

		for (i=12; i<23; i++) {
			//sprintf(str2, "%3d ", bars[i]);
			sprintf(str2, "%5.2f ", meta.bars[i]);
			strcat(str, str2);
		}
		//sprintf(str2, "%3d", bars[i]);
		sprintf(str2, "%5.2f", meta.bars[i]);
		strcat(str, str2);
		strcat(str, "\n");

		fprintf(outstream, "%s", str);
		fflush(outstream);
		//printf("%s", str);
	}					// if (outstream) {

	return turns;
}													// decode_ss()

/*
	#define CT_MODE_WIND               0x2c
	#define CT_MODE_WIND_PAUSE         0x28
	#define CT_MODE_RFTEST             0x1c
	#define CT_MODE_ERGO               0x14
	#define CT_MODE_ERGO_PAUSE         0x10
*/

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::start_cal(void) {
	lastctl = ctc.ctl;
	ctc.ctl = CT_MODE_RFTEST;
	//paused = false;
	return lastctl;
}

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::end_cal(void)  {
	ctc.ctl = lastctl;
	return 0;
}

/*******************************************************************************************

*******************************************************************************************/

void UDPClient::set_paused(bool _paused) {

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

	return;
}

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::set_ergo_mode(int _fw, unsigned short _rrc, float _manwatts) {
	ctc.ctl = CT_MODE_ERGO;
	f_manwatts = _manwatts;
	ctc.tdrag = (unsigned short)_rrc;
	return 0;
}

/*******************************************************************************************

*******************************************************************************************/

int UDPClient::set_slope(float _bike_kgs, float _person_kgs, int _drag_factor, float _grade) {
	ctc.ctl = CT_MODE_WIND;
	f_grade = _grade;
	f_weight = (float)(KGSTOLBS*(_bike_kgs + _person_kgs));
	f_aerodrag = 8.0f;			// f(_drag_factor)
	return 0;
}

