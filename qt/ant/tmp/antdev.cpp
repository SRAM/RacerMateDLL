/*****************************************

*****************************************/

#ifdef WIN32
	#pragma warning(disable:4996)       // strncpy_s
#else
	#include <stdarg.h>
#endif

#include <assert.h>

#include <QtCore>

#include <utils.h>
#include <sdirs.h>

#include "antdev.h"
#include "anterrs.h"

int ANTDEV::instances = 0;
#ifdef _DEBUG
int ANTDEV::calls = 0;
#endif

// message format, page 34
unsigned char ANTDEV::lib_config[6] =               { 0xa4, 0x02, 0x6e, 0x00, 0xe0,                                           0x00 };
unsigned char ANTDEV::reset[5] =                    { 0xa4, 0x01, 0x4a, 0x00,                                                 0x00 };
unsigned char ANTDEV::assign_channel[7] =           { 0xa4, 0x03, 0x42, 0x00, 0x00, 0x00,                                     0x00 };
unsigned char ANTDEV::assign_channel_extended[8] =  { 0xa4, 0x04, 0x42, 0x00, 0x10, 0x00, 0x01,                               0x00 };
unsigned char ANTDEV::unassign_channel[5] =         { 0xa4, 0x01, 0x41, 0x00,                                                 0x00 };
unsigned char ANTDEV::set_extended_messages[6] =    { 0xa4, 0x02, 0x66, 0x00, 0x01,                                           0x00 };
unsigned char ANTDEV::set_channel_id[9] =           { 0xa4, 0x05, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00,                         0x00 };
unsigned char ANTDEV::lp_search_timeout[6] =        { 0xa4, 0x02, 0x63, 0x00, 0xff,                                           0x00 };
unsigned char ANTDEV::search_timeout[6] =           { 0xa4, 0x02, 0x44, 0x00, 0xff,                                           0x00 };
unsigned char ANTDEV::open_channel[5] =             { 0xa4, 0x01, 0x4b, 0x00,                                                 0x00 };
unsigned char ANTDEV::close_channel[5] =            { 0xa4, 0x01, 0x4c, 0x00,                                                 0x00 };
unsigned char ANTDEV::get_caps[6] =                 { 0xa4, 0x02, 0x4d, 0x00, 0x54,                                           0x00 };
unsigned char ANTDEV::get_channel_status[6] =               { 0xa4, 0x02, 0x4d, 0x00, 0x52,                                           0x00 };
unsigned char ANTDEV::channel_period[7] =                   { 0xa4, 0x03, 0x43, 0x00, 0x00, 0x00,                                     0x00 };
unsigned char ANTDEV::set_channel_freq[6] =                 { 0xa4, 0x02, 0x45, 0x00, 0x00,                                           0x00 };
unsigned char ANTDEV::set_network_key[13] =         { 0xa4, 0x09, 0x46, 0x00, 0xb9, 0xa5, 0x21, 0xfb, 0xbd, 0x72, 0xc3, 0x45, 0x00 };

const unsigned char ANTDEV::USER_NETWORK_KEY[8] =           {                         0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45       };


QMap<int, QString> ANTDEV::msg_strings =  {
	{ CHANNEL_EVENT,			"CHANNEL_EVENT"										 },
	{ CHANNEL_ID,				"CHANNEL_ID"											 },
	{ CHANNEL_STATUS,			"CHANNEL_STATUS"										 },
	{ SET_NETWORK_KEY,		"SET_NETWORK_KEY"										 },
	{ MESG_STARTUP_MESG_ID, "MESG_STARTUP_MESG_ID"								 },
	{ CAPABILITIES,			"CAPABILITIES"											 },
	{ SERIAL_NUMBER,			"SERIAL_NUMBER"										 },
	{ VERSION,					"VERSION"												 },
	{ BROADCAST_DATA,			"BROADCAST_DATA"										 },
	{ ACK_DATA,					"ACK_DATA"												 },
	{ BURST_DATA,				"BURST_DATA"											 },
	{ REQ_MESSAGE,				"REQ_MESSAGE"											 },
	{ SYSTEM_RESET,			"SYSTEM_RESET"											 },
	{ ASSIGN_CHANNEL,			"ASSIGN_CHANNEL"										 },
	{ UNASSIGN_CHANNEL,		"UNASSIGN_CHANNEL"									 },
	{ ENABLE_EXT_MSGS,		"ENABLE_EXT_MSGS"										 },
	{ LP_SEARCH_TIMEOUT,		"LP_SEARCH_TIMEOUT"									 },
	{ OPEN_CHANNEL,			"OPEN_CHANNEL"											 },
	{ CLOSE_CHANNEL,			"CLOSE_CHANNEL"										 },
	{ CHANNEL_FREQUENCY,		"CHANNEL_FREQUENCY"									 },
	{ CHANNEL_PERIOD,			"CHANNEL_PERIOD"										 },
	{ SEARCH_TIMEOUT,			"SEARCH_TIMEOUT"										 },
	{ MESG_LIB_CONFIG,		"MESG_LIB_CONFIG"                             }
};


QMap<int, QString> ANTDEV::event_strings =  {
	{ EVENT_RX_SEARCH_TIMEOUT, "EVENT_RX_SEARCH_TIMEOUT"							 },
	{ CHANNEL_TX_POWER,			"CHANNEL_TX_POWER"									 },
	{ MESG_NETWORK_KEY_ID,		"MESG_NETWORK_KEY_ID"								 },
	{ ASSIGN_CHANNEL,				"ASSIGN_CHANNEL"										 },
	{ UNASSIGN_CHANNEL,			"UNASSIGN_CHANNEL"									 },
	{ CHANNEL_TX_POWER,			"CHANNEL_TX_POWER"									 },
	{ CHANNEL_FREQUENCY,			"CHANNEL_FREQUENCY"									 },
	{ CHANNEL_ID,					"CHANNEL_ID"											 },
	{ CHANNEL_PERIOD,				"CHANNEL_PERIOD"										 },
	{ OPEN_CHANNEL,				"OPEN_CHANNEL"											 },
	{ CLOSE_CHANNEL,				"CLOSE_CHANNEL"										 },
	{ LP_SEARCH_TIMEOUT,			"LP_SEARCH_TIMEOUT"									 },
	{ SEARCH_TIMEOUT,				"SEARCH_TIMEOUT"										 },
	{ CAPABILITIES,				"CAPABILITIES"											 },
	{ MESG_LIB_CONFIG,				"MESG_LIB_CONFIG"									 },
	{ ENABLE_EXT_MSGS,			"ENABLE_EXT_MSGS"										 }
};

QMap<int, QString> ANTDEV::startup_strings =  {
	{ RESET_POR,	  "RESET_POR"				  },
	{ RESET_SUSPEND, "RESET_SUSPEND"			  },
	{ RESET_SYNC,	  "RESET_SYNC"				  },
	{ RESET_CMD,	  "RESET_CMD"				  },
	{ RESET_WDT,	  "RESET_WDT"				  },
	{ RESET_RST,	  "RESET_RST"				  }
};

QMap<int, QString> ANTDEV::types =  {
	{ 120,			"Heart Rate"										 },
	{ 121,			"Speed-Cadence"									 }
};





/****************************************************************************
	constructor 1
****************************************************************************/

ANTDEV::ANTDEV(qint64 _start_time, int _debug_level, int _key, QObject* _parent ) : QObject(_parent) {
	start_time = _start_time;
	debug_level = _debug_level;
	key = _key;
	parent = _parent;

	init();

	instances++;
	return;
}						// constructor


/******************************************************************************

******************************************************************************/

ANTDEV::~ANTDEV(void) {


//	DEL(tmr);

	int status;
	//int n;
	//n = channels.size();

	foreach(ANTCHAN chan, chanmap ) {
		if (chan.state==ANTCHAN::ASSIGNED)  {
			status = unassign(chan.number);									// closes channel AND unassigns it
			if (status)  {
			}
		}
	}

	//destructing = true;

	chanmap.clear();

	QThread::msleep(500);

	stop();

	fflush(logstream);
	QThread::msleep(50);


	DEL(tmr);
	FCLOSE(logstream);

	instances--;

}										// destructor

/******************************************************************************

******************************************************************************/

int ANTDEV::init(void) {

#ifdef _DEBUG
	dbg = true;
#endif

	if (dbg)  {
		qDebug("      ANTDEV::init()");
	}

	exists = true;

	RACERMATE::SDIRS *sdirs = new RACERMATE::SDIRS(debug_level, "rmant");

	if (debug_level > 0) {
		QString s = sdirs->get_debug_dir() + "antdev.log";
		logstream = fopen(s.toUtf8().constData(), "wt");
		lock_logging();
		logg("%s opened\n", s.toUtf8().constData());
		unlock_logging();
	}

	DEL(sdirs);

	char str[32];
	sprintf(str, "antdev%d", key);
	tmr = new RACERMATE::Tmr(str);
#ifdef _DEBUG
	tmr->setdbg();
#endif

	memset(&desc, 0, sizeof(desc));

	memcpy(user_network_key, USER_NETWORK_KEY, sizeof(user_network_key));

	clear();

	if (dbg)  {
		qDebug("      ANTDEV::init() x");
	}


	return 0;
}                       // init()

/******************************************************************************

******************************************************************************/

void ANTDEV::clear(void) {
	if (handle) {
		libusb_close(handle);
		handle = NULL;
	}

	txinptr = 0;
	txoutptr = 0;
	rxinptr = 0;
	rxoutptr = 0;

	vid = 0;
	pid = 0;
	memset(mfgr, 0, sizeof(mfgr));
	memset(prod, 0, sizeof(prod));
	memset(sn, 0, sizeof(sn));
	nconfigs = 0;
	bus_number = 0;
	device_address = 0;
	int i, n;
	n = ifs.size();

	for (i = 0; i < n; i++) {
		ifs[i].clear();
	}

	ifs.clear();

	return;
}

/******************************************************************************

******************************************************************************/

void ANTDEV::add_intf(INTF _intf) {
	ifs.push_back(_intf);
	return;
}

/******************************************************************************

******************************************************************************/

void ANTDEV::dump(FILE *_stream) {

	if (_stream == NULL) {
		return;
	}

	fprintf(_stream, "\n");

	fprintf(_stream, "manufacturer = %s\n", mfgr);
	fprintf(_stream, "product = %s\n", prod);
	fprintf(_stream, "vendor = %d\n", vid);
	fprintf(_stream, "product = %d\n", pid);
	fprintf(_stream, "bus number = %d\n", bus_number);
	fprintf(_stream, "device address = %d\n", device_address);
	fprintf(_stream, "sn = %s\n", sn);
	fprintf(_stream, "ix = %d\n", ix);
	fprintf(_stream, "nconfigs = %d\n", nconfigs);
	fprintf(_stream, "ifs.size() = %lu\n", ifs.size());

	return;
}


/**************************************************************************************************
	called from writer thread
**************************************************************************************************/

int ANTDEV::tx(unsigned char *_buf, int _n, unsigned long _timeout) {
	int written = 0, total = 0, status = 0;
	bool flag;
	int i, n, rc = -1;


	if (txinptr == txoutptr) {
		return 0;
	}

	if (handle == NULL) {
		return -1;
	}

	tx_calls++;

	total = 0;
	i = 0;
	n = _n;
	flag = true;

//	int ii = wep;							// 1
//	Q_UNUSED(ii);

	while (flag) {
		status = libusb_bulk_transfer( handle, wep, &_buf[i], n, &written, _timeout );
		switch(status)  {
			case 0:
				break;
			case LIBUSB_ERROR_TIMEOUT:
				break;
			default:
				return -1;
		}

		switch (status) {
			case 0: {
				if (written == _n) {
					flag = false;
					rc = _n;
				}
				else  {
					total += written;
					if (total == _n) {
						flag = false;
						rc = _n;
					}
					else  {
						i += written;
						n -= written;
					}
				}
				break;
			}
			case LIBUSB_ERROR_TIMEOUT: {        // if the transfer timed out
				break;
			}
			case LIBUSB_ERROR_PIPE: {           // if the endpoint halted
				rc = -1;
				flag = false;
				break;
			}
			case LIBUSB_ERROR_OVERFLOW: {       // if the device offered more data, see Packets and overflows
				rc = -2;
				flag = false;
				break;
			}
			case LIBUSB_ERROR_NO_DEVICE: {      // if the device has been disconnected
				rc = -3;
				flag = false;
				break;
			}
			case LIBUSB_ERROR_NOT_FOUND: {      // if the device has been disconnected
				rc = -4;
				flag = false;
				break;
			}
			default: {                          // LIBUSB_ERROR code on other error
				rc = -5;
				flag = false;
				break;
			}
		}                 // switch()
	}                    // while(flag)

	return rc;
}                       // tx()


/**************************************************************************************************
	called from reader about 3 times per second
	simply puts stuff in the rxq
**************************************************************************************************/

int ANTDEV::rx(void) {
	int bytes_read, status;
	bool flag;
	int i, rc = -1;
	unsigned char buf[64];
	unsigned short len;

	if (!handle) {
		return 0;
	}

	unsigned int timeout = 100;

	i = rep;
	rep = 0x81;

	i = 0;
	flag = true;

	int reads = 0;
	int timeouts = 0;

	rx_calls++;


	while (flag) {
		reads++;
		#ifdef _DEBUG
		memset(buf, 0, sizeof(buf));
		#endif

		status = libusb_bulk_transfer(
			handle,                       // struct libusb_device_handle *dev_handle,
			rep,                          // unsigned char endpoint, 0x81
			buf,                          // unsigned char *data,
			(int)sizeof(buf),             // int length,
			&bytes_read,                  // int *transferred,
			timeout                       // unsigned int timeout
			);


#ifdef _DEBUG
		char *cptr;
		if (status == -1) {
			cptr = strerror(errno);       // Device or resource busy
			Q_UNUSED(cptr);
		}

#endif

		switch (status) {
			case 0: {
				if (bytes_read == 0) {
					rc = 0;
					flag = false;
					break;
				}

				int j;

				for (i = 0; i < bytes_read; i++) {
					j = (rxinptr + i) % RXBUFLEN;
					rxq[j] = buf[i];
				}

				rxinptr = (rxinptr + bytes_read) % RXBUFLEN;
				len = rxinptr - rxoutptr;

				// check for overflows here? (to do)

				len &= (RXBUFLEN - 1);

				if (bytes_read <= RXBUFLEN) {
					rc = bytes_read;
					flag = false;
				}
				else  {
				}

				break;
			}

			case LIBUSB_ERROR_TIMEOUT: {        // if the transfer timed out
				timeouts++;
				if (timeouts > 0) {
					rc = LIBUSB_ERROR_TIMEOUT;
					flag = false;
				}
				break;
			}
			case LIBUSB_ERROR_PIPE: {           // if the endpoint halted -9
				rc = LIBUSB_ERROR_PIPE;
				flag = false;
				break;
			}
			case LIBUSB_ERROR_OVERFLOW: {       // if the device offered more data, see Packets and overflows
				rc = LIBUSB_ERROR_OVERFLOW;
				flag = false;
				break;
			}
			case LIBUSB_ERROR_NO_DEVICE: {      // if the device has been disconnected
				rc = LIBUSB_ERROR_NO_DEVICE;
				flag = false;
				break;
			}

			case LIBUSB_ERROR_IO: {
				flag = false;
				rc = LIBUSB_ERROR_IO;
				break;
			}

			case LIBUSB_ERROR_NOT_FOUND:  {				// -5
				rc = LIBUSB_ERROR_NOT_FOUND;
				flag = false;
				break;
			}

			default: {                          // LIBUSB_ERROR code on other error
				rc = -999;
				flag = false;
				break;
			}
		}                 // switch()
	}                    // while(flag)


	return rc;
}                       // rx()

/**************************************************************************************************

**************************************************************************************************/

void ANTDEV::process(void) {
	unsigned char msg;

	msg = rxMessage[OFFS_MSG];                               // OFFS_MSG = 2
	int len = rxMessage[1];
	len += 2;

#ifdef _DEBUG
	char msgstr[32];
	char eventstr[32];
	char startupstr[32];

	msgstr[0] = 0;
	eventstr[0] = 0;
	startupstr[0] = 0;

	process_count++;

	if (msg_strings.contains(msg)) {
		strcpy(msgstr, msg_strings[msg].toUtf8().constData());
	}
	else  {
		strcpy(msgstr, "------------");
	}
#endif            // _DEBUG

	switch (msg) {
		case 0xae:  {                       // serial error
			unsigned char reason;
			Q_UNUSED(reason);
			reason = rxMessage[3];                       // 2 = the checksum of the ANT message was incorrect
			break;
		}

		case CHANNEL_EVENT: {                        // 0x40

			unsigned char chan;

			chan = rxMessage[OFFS_CHAN];                 // offs_chan = 3
			event = rxMessage[OFFS_EVENT_ID];            // offs_event_id = 4, pdf, pg 48, e id
			response = rxMessage[OFFS_EVENT_CODE];       // offs_event_code = 5, pdf, pg 48, e code, 0x28

#ifdef _DEBUG
			if (chan==1)  {
				bp = 3;
			}

			if (event_strings.contains(event)) {
				strcpy(eventstr, event_strings[event].toUtf8().constData());
			}
			else  {
				strcpy(eventstr, "------------");
			}
#endif


			switch (event) {

				case EVENT_RX_SEARCH_TIMEOUT:  {          // 0x01
					// see page 116
					// A receive channel has timed out on searching. The search is terminated, and the channel
					// has been automatically closed.
					// In order to restart the search the Open Channel message must be sent again.

					switch(response)  {

						case EVENT_RX_SEARCH_TIMEOUT:					// 0x01
							//	A receive channel has timed out on searching. The search is
							// terminated, and the channel has been automatically closed.
							// In order to restart the search the Open Channel message
							// must be sent again.
#ifdef _DEBUG
								//qDebug("response = EVENT_RX_SEARCH_TIMEOUT on channel %d", chan);
#endif
							//bp = 1;
							break;
						case  EVENT_RX_FAIL:								// 0x02
#ifdef _DEBUG
								//qDebug("response = EVENT_RX_FAIL on channel %d", chan);
#endif
							//bp = 1;
							break;
						case  EVENT_CHANNEL_CLOSED:					// 0x07
							// The channel has been successfully closed. When the Host
							// sends a message to close a channel, it first receives a
							// RESPONSE_NO_ERROR to indicate that the message was
							// successfully received by ANT; however,
							// EVENT_CHANNEL_CLOSED is the actual indication of the
							// closure of the channel. As such, the Host m ust use this event
							// message rather than the RESPONSE_NO_ERROR message to
							// let a channel state machine continue

							if (chan==0)  {
#ifdef _DEBUG
								//qDebug("response = EVENT_CHANNEL_CLOSED (7) on channel %d", chan);
#endif
								listening = false;
								chanmap[chan].open = false;
								chanmap[chan].sensors.clear();
								//int status = start_lisening();

							}
							break;
						case EVENT_RX_FAIL_GO_TO_SEARCH:				// 0x08
							break;

						case EVENT_CHANNEL_COLLISION:					// 0x09
							// see antdefines.h for defines
							// Two channels have drifted into each other and overlapped in
							// time on the device causing one channel to be blocked.
							break;
						case EVENT_SERIAL_QUE_OVERFLOW:				// 0x34
							//bp = 2;
							break;
						case EVENT_QUE_OVERFLOW:						// 0x35
							break;


						default:
#ifdef _DEBUG
							qFatal("oopsy doopsy");
#endif
							chanmap[chan].open = false;
							response = 0xff;																	// unsolicited
							event = 0;
							break;
					}						// switch(response)

					break;
				}											// case EVENT_RX_SEARCH_TIMEOUT


				case UNASSIGN_CHANNEL: {                                  // 0x41
					break;
				}
				case ASSIGN_CHANNEL: {                                  // 0x42
					break;
				}

				case CHANNEL_PERIOD: {                                  // 0x43
					break;
				}
				case SEARCH_TIMEOUT:  {                                 // 0x44
					break;
				}
				case CHANNEL_FREQUENCY: {                               // 0x45
					msgdone = true;
					break;
				}
				case MESG_NETWORK_KEY_ID: {                                 // 0x46
					break;
				}
				case OPEN_CHANNEL: {                                    // 0x4b
#ifdef _DEBUG
					if (chan==1)  {
						bp = 5;									// ok
					}
#endif
					chanmap[chan].open = true;
					break;
				}
				case CLOSE_CHANNEL: {                                    // 0x4c
					chanmap[chan].open = false;
					break;
				}
				case CHANNEL_ID: {                                      // 0x51
					break;
				}
				case CHANNEL_TX_POWER: {                                // 0x60
					break;
				}
				case LP_SEARCH_TIMEOUT:  {                              // 0x63
					break;
				}
				case ENABLE_EXT_MSGS:  {              // 0x66
					if (response == INVALID_MESSAGE) {
						extended = false;
					}
					else if (response != RESPONSE_NO_ERROR)
					{
					}
					else  {
						extended = true;
					}
					break;
				}

				default:  {
#ifdef _DEBUG
					bp = 9;
#endif
					break;
				}
			}              // switch (_ev) {

			break;
		}                             // case CHANNEL_EVENT: {    // 0x40


		case MESG_STARTUP_MESG_ID: {    // 0X6F

			#ifdef _DEBUG
			UCHAR ucReason = rxMessage[OFFS_DATA];
			if (startup_strings.contains(ucReason)) {
				strcpy(startupstr, startup_strings[ucReason].toUtf8().constData());
			}
			else  {
				strcpy(startupstr, "------------");
			}
			#endif

			msgdone = true;
			break;
		}

		case CHANNEL_ID: {        // 0X51, set channel id
			break;
		}

		case CHANNEL_STATUS: {    // 0X52
			int chan = rxMessage[3];

			chanmap[chan].state = (ANTCHAN::CHANSTATE)(rxMessage[4] & 0x03);           // get bits 0 and 1 only

			bool AP1 = false;

			if (!AP1) {
				chanmap[chan].netnum = rxMessage[4] & 0x0c;                // get bits 2 and 3 only
				chanmap[chan].returned_type = rxMessage[4] & 0xf0;
			}
			msgdone = true;
			break;
		}


		case SET_NETWORK_KEY: {       // 0X46
			break;
		}


		case CAPABILITIES: {    // 0x54												// per channel
			char str[1024];
			unsigned char *buf = &rxMessage[3];

			standard_options = buf[2];               // 0x00
			advanced_options = buf[3];                      // 0xba
			advanced2_options = buf[4];                     // 0x36
			// note: buf[5] is reserved (unused)

			strcpy(str, "   Standard Options:\n");

			if ( standard_options & CAPABILITIES_NO_RX_CHANNELS ) {
				strcat(str, "		CAPABILITIES_NO_RX_CHANNELS\n");
			}

			if ( standard_options & CAPABILITIES_NO_TX_CHANNELS ) {
				strcat(str, "		CAPABILITIES_NO_TX_CHANNELS\n");
			}

			if ( standard_options & CAPABILITIES_NO_RX_MESSAGES ) {
				strcat(str, "		CAPABILITIES_NO_RX_MESSAGES\n");
			}

			if ( standard_options & CAPABILITIES_NO_TX_MESSAGES ) {
				strcat(str, "		CAPABILITIES_NO_TX_MESSAGES\n");
			}

			if ( standard_options & CAPABILITIES_NO_ACKD_MESSAGES ) {
				strcat(str, "		CAPABILITIES_NO_ACKD_MESSAGES\n");
			}

			if ( standard_options & CAPABILITIES_NO_BURST_TRANSFER ) {
				strcat(str, "		CAPABILITIES_NO_BURST_TRANSFER\n");
			}

			////////////////////////////////////////////

			strcat(str, "   Advanced Options:\n");

			if ( advanced_options & CAPABILITIES_OVERUN_UNDERRUN ) {
				strcat(str, "		CAPABILITIES_OVERUN_UNDERRUN\n");
			}

			if ( advanced_options & CAPABILITIES_NETWORK_ENABLED ) {
				strcat(str, "		CAPABILITIES_NETWORK_ENABLED\n");                        // this
			}

			if ( advanced_options & CAPABILITIES_AP1_VERSION_2 ) {
				strcat(str, "		CAPABILITIES_AP1_VERSION_2\n");
			}

			if ( advanced_options & CAPABILITIES_SERIAL_NUMBER_ENABLED ) {
				strcat(str, "		CAPABILITIES_SERIAL_NUMBER_ENABLED\n");                  // this
			}

			if ( advanced_options & CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED ) {
				strcat(str, "		CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED\n");           // this
			}

			if ( advanced_options & CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED ) {
				strcat(str, "		CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED\n");            // this
			}

			if ( advanced_options & CAPABILITIES_SCRIPT_ENABLED ) {
				strcat(str, "		CAPABILITIES_SCRIPT_ENABLED\n");
			}

			if ( advanced_options & CAPABILITIES_SEARCH_LIST_ENABLED ) {
				strcat(str, "		CAPABILITIES_SEARCH_LIST_ENABLED\n");                    // this
			}

			////////////////////////////////////////////

			strcat(str, "   Advanced 2 Options:\n");

			if ( advanced2_options & CAPABILITIES_LED_ENABLED ) {
				strcat(str, "		CAPABILITIES_LED_ENABLED\n");
			}

			if ( advanced2_options & CAPABILITIES_EXT_MESSAGE_ENABLED ) {
				strcat(str, "		CAPABILITIES_EXT_MESSAGE_ENABLED\n");                    // this
			}

			if ( advanced2_options & CAPABILITIES_SCAN_MODE_ENABLED ) {
				strcat(str, "		CAPABILITIES_SCAN_MODE_ENABLED\n");                      // this
			}

			if ( advanced2_options & CAPABILITIES_RESERVED ) {
				strcat(str, "		CAPABILITIES_RESERVED\n");
			}

			if ( advanced2_options & CAPABILITIES_PROX_SEARCH_ENABLED ) {
				strcat(str, "		CAPABILITIES_PROX_SEARCH_ENABLED\n");                    // this, proximity search
			}

			if ( advanced2_options & CAPABILITIES_EXT_ASSIGN_ENABLED ) {
				strcat(str, "		CAPABILITIES_EXT_ASSIGN_ENABLED\n");                     // this
			}

			if ( advanced2_options & CAPABILITIES_FS_ANTFS_ENABLED) {
				strcat(str, "		CAPABILITIES_FREE_1\n");
			}

			if ( advanced2_options & CAPABILITIES_FIT1_ENABLED ) {
				strcat(str, "		CAPABILITIES_FIT1_ENABLED\n");
			}


			int len = strlen(str);                    // 385

			if (debug_level > 1)  {
				lock_logging();
				logg("len = %d\n%s", len, str);
				unlock_logging();
			}

			msgdone = true;
			break;
		}                       //	case CAPABILITIES:

		case SERIAL_NUMBER: {   // 0x61
			sernum = (rxMessage[3] << 24) & 0xff000000;
			sernum |= (rxMessage[4] << 16) & 0x00ff0000;
			sernum |= (rxMessage[5] << 8) & 0x0000ff00;
			sernum |= (rxMessage[6] << 24) & 0x000000ff;

			memcpy(&sernum, &rxMessage[3], 4);                                   // reverses the order of the above code
			sprintf(sernum_string, "%04lx", sernum);
			msgdone = true;
			break;
		}

		case VERSION: {    // 0x3e
			int len = rxMessage[1];
			if (len > (int)sizeof(version) - 1) {
				lock_logging();
				logg("version too long\n");
				unlock_logging();
				return;
			}
			memcpy(version, &rxMessage[3], len);                        // "AJK1.04RAF"
			msgdone = true;
			break;
		}

		case BROADCAST_DATA: {									// 0x4e
			if (listening)  {
				int len = rxMessage[1];					// doesn't include sync byte or checksum
				if (len != 14 )  {
					lock_logging();
					logg("bad len\n");
					unlock_logging();
					return;
				}

				int chan = rxMessage[3];

#ifdef _DEBUG
				if (chanmap.size() > 1)  {
					if (chan==0)  {
						bp = 2;															// never gets here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					}
				}

				if (debug_level > 1) {
					if (showrx)  {
						lock_dumping();
						dump(rxMessage, len+4, "rx: ");							// add in sync byte, len, cmd, and checksum
						unlock_dumping();
					}
				}

				if (chan==1)  {
					// a4 0e 4e 01         7a 8d a9 06 36 02 66 0c         80 ee ab 79  xx  xx
					//                                                     |  |  |  |   |   |
					//                                                     |  |  |  |   |   cs
					//                                                     |  |  |  |   transtype
					//                                                     |  |  |  devtype
					//                                                     |  |  devnum, high byte
					//                                                     |  devnum, low byte
					//                                                     flag
//					if (debug_level > 1) {
//						if (chan==0)  {
//							lock_dumping();
//							dump(rxMessage, len+4, "rx: ");							// add in sync byte, len, cmd, and checksum
//							unlock_dumping();
//						}
//					}
					bp = 1;
				}
#endif



				unsigned short devnum = rxMessage[13] | (rxMessage[14]<<8);					// abee		44014				little endian, low byte then high byte

				if (!chanmap[chan].sensors.contains(devnum))  {
					SENSOR sensor;
					sensor.sn = devnum;													// for easier iteration
					sensor.type = rxMessage[15];
					sensor.transtype = rxMessage[16];
					sensor.assigned_channel = chan;									// xxx

#ifdef _DEBUG
					if (chan==1)  {
						bp = 1;
					}
#endif
					chanmap[chan].sensors[devnum] = sensor;												// sensors grow here
#ifdef _DEBUG
					if (chan==1)  {
						bp = 2;
					}
#endif

				}


				switch (chanmap[chan].sensors[devnum].type) {
					case 121: {								// 0x79
						ant_decode_speed_cadence();
						break;
					}
					case 120: {								// 0x78
						chanmap[chan].sensors[devnum].val = decode_hr();
						break;
					}
					default:
						break;
				}

			}							// if (listening)
			else  {
			}
			break;
		}

		case ACK_DATA: {
			break;
		}
		case BURST_DATA: {
			break;
		}

		default: {
			break;
		}
	}                          // switch(msg)

	memset(rxMessage, 0, sizeof(rxMessage));

	return;
}                          // void process()

/**************************************************************************************************

**************************************************************************************************/

void ANTDEV::receiveByte(unsigned char byte) {

	switch (state) {
		case ST_WAIT_FOR_SYNC: {
			if (byte == SYNC_BYTE) {                                // 0xa4
				check_sum = SYNC_BYTE;
				rxMessage[0] = byte;
				state = ST_GET_LENGTH;
			}
			break;
		}

		case ST_GET_LENGTH: {
			if ((byte == 0) || (byte > MAX_LENGTH)) {               // MAX_LENGTH = 9
				state = ST_WAIT_FOR_SYNC;
			}
			else  {
				rxMessage[1] = byte;
				check_sum ^= byte;
				length = byte;
				bytes = 0;
				state = ST_GET_MESSAGE_ID;
			}
			break;
		}

		case ST_GET_MESSAGE_ID: {
			rxMessage[2] = byte;
			check_sum ^= byte;
			state = ST_GET_DATA;
			break;
		}

		case ST_GET_DATA: {
			rxMessage[3 + bytes] = byte;                                   // 0x6f 0x20 = "command reset" response
			check_sum ^= byte;
			if (++bytes == length) {
				state = ST_VALIDATE_PACKET;
			}
			break;
		}

		case ST_VALIDATE_PACKET: {
			if (byte == check_sum) {
				inpackets++;
				rxMessage[3 + bytes] = byte;

				maxbytes = qMax(maxbytes, bytes+3);									// max bytes = 17

				if (maxbytes > 17)  {
					lock_logging();
					logg("maxbytes = %d\n", maxbytes);
					unlock_logging();
					return;
				}
				process();
			}
			else  {
				if (debug_level > 1)  {
					lock_logging();
					logg("rx: %-30s %02x %02x %02x ", "bad checksum:", rxMessage[0], rxMessage[1], rxMessage[2]);
					unlock_logging();
				}
			}

			state = ST_WAIT_FOR_SYNC;
			break;
		}
	}
	return;
}                             // receiveByte()

/********************************************************************************************************

********************************************************************************************************/

bool ANTDEV::wfr(UCHAR _event, qint64 _ms) {
	qint64 curdiff = 0;
	qint64 startms;

	qint64 dms = 0;
	bool have_string = false;
	bool good_response = false;

	if (event_strings.contains(_event)) {
		have_string = true;
	}


#ifdef _DEBUG
	if (event == EVENT_RX_SEARCH_TIMEOUT)  {
		switch(response)  {
			case EVENT_RX_FAIL:
			case EVENT_CHANNEL_CLOSED:
			case EVENT_CHANNEL_COLLISION:
			case EVENT_SERIAL_QUE_OVERFLOW:					// 0x34
				break;
			default:
				qFatal("zzx, response = %d (%x hex)", response, response);
				break;
		}
	}
	else  {
		Q_ASSERT(event == 0);
		Q_ASSERT(response == 0xff);
	}
#endif

	startms = QDateTime::currentMSecsSinceEpoch();

	while (1) {

		if (_event == event) {

			if (response == RESPONSE_NO_ERROR) {
				//dns = et.nsecsElapsed() - startns;
				dms = QDateTime::currentMSecsSinceEpoch() - startms;
				good_response = true;
			}                 // if (response == RESPONSE_NO_ERROR)
			else if (event==1 && response == 7)  {
				dms = QDateTime::currentMSecsSinceEpoch() - startms;
				good_response = true;
			}
			else {
				// correct event, but response was not RESPONSE_NO_ERROR. Keep waiting????? or abort now?
				//break;			// ???
			}                 // if (response == RESPONSE_NO_ERROR)

		}

		if (good_response) {
			break;
		}

		curdiff = QDateTime::currentMSecsSinceEpoch() - startms;


		if (curdiff >= _ms) {
			dms = QDateTime::currentMSecsSinceEpoch() - startms;
			break;                                                   // timed out
		}
		QThread::msleep(5);
	}                          // while(1)


	if (debug_level > 1)  {
		lock_logging();
		if (!good_response) {
			if (have_string) {
				if (_event != EVENT_RX_SEARCH_TIMEOUT)  {
					logg("WFR, event = %s... TIMED OUT after %.lf ms\n", event_strings[_event].toUtf8().constData(), (float)dms);
				}
			}
			else  {
				logg("WFR, unknown event = %d... TIMED OUT after %.2f ms\n", _event, (float)dms);
			}
		}
		else  {
			if (have_string) {
				logg("WFR %s %.2f OK\n", event_strings[_event].toUtf8().constData(), (float)dms);
			}
			else  {
				logg("WFR(%d?), OK, dt = %.2f ms\n", _event, (float)dms);
			}
		}
		unlock_logging();
	}


	response = 0xff;
	event = 0;


	return good_response;
}                 // wfr()



/********************************************************************************************************
	waits up to _ms milliseconds for msgdone to become true
********************************************************************************************************/

bool ANTDEV::wfm(UCHAR _msg, qint64 _ms) {
	Q_UNUSED(_msg);

	msgdone = false;


	bool have_string = false;

	if (msg_strings.contains(_msg)) {
		have_string = true;
	}
	else  {
	}

	qint64 startms = QDateTime::currentMSecsSinceEpoch();
	qint64 nowms, dtms=0;
	bool timed_out = false;

	while (!msgdone) {
		nowms = QDateTime::currentMSecsSinceEpoch();
		dtms = nowms - startms;

		if (dtms >= _ms) {
			timed_out = true;
			break;
		}
		QThread::msleep(1);
	}

	if (debug_level > 1)  {
		lock_logging();

		if (timed_out) {
			if (have_string) {
				logg("WFM: %s TIMED OUT!\n", msg_strings[_msg].toUtf8().constData() );
			}
			else  {
				logg("WFM %d TIMED OUT!\n", _msg);
			}
		}
		else  {
			if (have_string) {
				logg("WFM: %s %d OK\n", msg_strings[_msg].toUtf8().constData(), dtms );
			}
			else  {
				logg("WFM: %d %d OK\n", _msg, dtms);
			}
		}
		unlock_logging();
	}

	return msgdone;

}                 // wfm()

/**************************************************************************************

	this decoder based on


	entry:
		ucWheelCircumference set to calculate speed
		ant_packet contains the payload

	exit:
		cadence calculated
		speed (meters / hour) calculated
		wheel_rpm calculated
		(all floats)

**************************************************************************************/

int ANTDEV::ant_decode_speed_cadence(void) {

	USHORT usEventDiff = 0;
	USHORT usTimeDiff1024 = 0;
	unsigned char *buf = &rxMessage[4];

	//--------------------------------------
	// cadence:
	//--------------------------------------

	usCadTime1024 = buf[0];
	usCadTime1024 += buf[1] << 8;              // crankMeasurmentTime

	usCadEventCount = buf[2];
	usCadEventCount += buf[3] << 8;            // crankRevolutions

	//--------------------------------------
	// speed:
	//--------------------------------------

	usSpdTime1024 = buf[4];
	usSpdTime1024 += buf[5] << 8;              // wheelMeasurementTime

	usSpdEventCount = buf[6];
	usSpdEventCount += buf[7] << 8;            // wheelRevolutions


	if (eState == BSC_INIT) {
		usCadPreviousTime1024 = usCadTime1024;
		usCadPreviousEventCount = usCadEventCount;

		usSpdPreviousTime1024 = usSpdTime1024;
		usSpdPreviousEventCount = usSpdEventCount;

		eState = BSC_ACTIVE;
	}

	//-----------------------------------------------------
	// update cadence calculations on new cadence event
	// this is the large bump on the sensor body
	//-----------------------------------------------------

	if (usCadEventCount != usCadPreviousEventCount) {
		ucNoCadEventCount = 0;
		ant_coasting = false;

		// update cumulative event count

		if (usCadEventCount > usCadPreviousEventCount) {
			usEventDiff = usCadEventCount - usCadPreviousEventCount;
		}
		else  {
			usEventDiff = (USHORT)(0xFFFF - usCadPreviousEventCount + usCadEventCount + 1);
		}
		ulCadAcumEventCount += usEventDiff;

		// update cumulative time (1/1024s)

		if (usCadTime1024 > usCadPreviousTime1024) {
			usTimeDiff1024 = usCadTime1024 - usCadPreviousTime1024;
		}
		else  {
			usTimeDiff1024 = (USHORT)(0xFFFF - usCadPreviousTime1024 + usCadTime1024 + 1);
		}
		ulCadAcumTime1024 += usTimeDiff1024;

		// calculate cadence (rpm)

		if (usTimeDiff1024 > 0) {
			ant_cadence = (float)( ((ULONG)usEventDiff * 0xF000) / (ULONG)usTimeDiff1024 );     // 1 min = 0xF000 = 60 * 1024
		}
	}
	else  {
		ucNoCadEventCount++;

		if (ucNoCadEventCount >= MAX_NO_EVENTS) {
			ant_coasting = true;                         // coasting
			ant_cadence = 0.0f;
		}
	}


	//-----------------------------------------------------
	// Update speed calculations on new speed event
	// this is the head of the giraffe
	//-----------------------------------------------------

	if (usSpdEventCount != usSpdPreviousEventCount) {
		ucNoSpdEventCount = 0;
		ant_stopping = false;
		// Update cumulative event count
		if (usSpdEventCount > usSpdPreviousEventCount) {
			usEventDiff = usSpdEventCount - usSpdPreviousEventCount;
		}
		else  {
			usEventDiff = (USHORT)(0xFFFF - usSpdPreviousEventCount + usSpdEventCount + 1);
		}
		ulSpdAcumEventCount += usEventDiff;

		// Update cumulative time (1/1024s)

		if (usSpdTime1024 > usSpdPreviousTime1024) {
			usTimeDiff1024 = usSpdTime1024 - usSpdPreviousTime1024;
		}
		else  {
			usTimeDiff1024 = (USHORT)(0xFFFF - usSpdPreviousTime1024 + usSpdTime1024 + 1);
		}

		ulSpdAcumTime1024 += usTimeDiff1024;


		ant_wheel_rpm = ((float)usEventDiff / (float)usTimeDiff1024) * 60.0f * 1024.0f;

		if (ant_circumference_cm) {
#ifdef _DEBUG
			//float diameter = (float) (circumference_cm / PI);
#endif
			// Calculate speed (meters/h)
			ant_meters_per_hour = (float)(ant_circumference_cm * 0x9000 * (ULONG)usEventDiff) / (ULONG)usTimeDiff1024;      // 1024 * 36 = 0x9000
		}

		// Calculate distance (cm)

		ant_cm = (ULONG)ant_circumference_cm * ulSpdAcumEventCount;
	}
	else  {
		ucNoSpdEventCount++;
		if (ucNoSpdEventCount >= MAX_NO_EVENTS) {
			ant_stopping = true;
			ant_wheel_rpm = 0.0f;
			ant_meters_per_hour = 0.0f;
		}
	}

	// Update previous values

	usCadPreviousTime1024 = usCadTime1024;
	usCadPreviousEventCount = usCadEventCount;

	usSpdPreviousTime1024 = usSpdTime1024;
	usSpdPreviousEventCount = usSpdEventCount;

	return 0;
}              // decode_speed_cadence()

/**************************************************************************************

**************************************************************************************/

int ANTDEV::decode_hr(void) {
	UCHAR ucPageNum = 0;
	UCHAR ucEventDiff = 0;
	USHORT usTimeDiff1024 = 0;


	unsigned char *buf = &rxMessage[4];

#ifdef _DEBUG
	//tmr->update();								// 250 ms
	hrcalls++;
	if(show_hr_packets)  {
		int len = rxMessage[1];					// doesn't include sync byte or checksum
		lock_dumping();
		dump(rxMessage, len+4, "hr packet: ");							// add in sync byte, len, cmd, and checksum
		unlock_dumping();
	}
#endif


	// monitor page toggle bit

	if (ucStatePage != HRM_EXT_PAGE) {
		if (ucStatePage == HRM_INIT_PAGE) {
			ucStatePage = (buf[0] & HRM_TOGGLE_MASK) >> 7;
			// initialize previous values to correctly get the cumulative values
			ucPreviousEventCount = buf[6];
			usPreviousTime1024 = buf[4];
			usPreviousTime1024 += buf[5] << 8;
		}
		else if (ucStatePage != ((buf[0] & HRM_TOGGLE_MASK) >> 7))  {
			// first page toggle was seen, enable advanced data
			ucStatePage = HRM_EXT_PAGE;
		}
	}

	// remove page toggle bit

	ucPageNum = buf[0] & ~HRM_TOGGLE_MASK;       // page 0 and 4 are the main data pages

	// Handle basic data

	ucEventCount = buf[6];
	usTime1024 = buf[4];
	usTime1024 += buf[5] << 8;
	//if (hrcalls>15)  {
	//if (buf[6] != 0xcc && buf[7] != 0xcc)  {				// wait for valid buf
#ifdef _DEBUG
	//if (have_ant_packet)  {
	instantaneous_hr = buf[7];
	//}
#else
	instantaneous_hr = buf[7];
#endif


	// handle background data, if supported

	if (ucStatePage == HRM_EXT_PAGE) {
		switch (ucPageNum) {
			case HRM_PAGE1: {
				ulElapsedTime2 = buf[1];
				ulElapsedTime2 += buf[2] << 8;
				ulElapsedTime2 += buf[3] << 16;
				break;
			}
			case HRM_PAGE2: {
				ucMfgID = buf[1];
				usSerialNum = buf[2];
				usSerialNum += buf[3] << 8;      // serial number = 3?
				break;
			}
			case HRM_PAGE3: {
				ucHwVersion = buf[1];         // 4
				ucSwVersion = buf[2];         // 4
				ucModelNum = buf[3];          // 5
				break;
			}
			case HRM_PAGE4: {
				usPreviousTime1024 = buf[2];
				usPreviousTime1024 += buf[3] << 8;
				break;
			}
			default: {
				break;
			}
		}
	}


	// only need to do calculations if dealing with a new event

	if (ucEventCount != ucPreviousEventCount) {
		ucNoEventCount = 0;

		// update cumulative event count

		if (ucEventCount > ucPreviousEventCount) {
			ucEventDiff = ucEventCount - ucPreviousEventCount;
		}
		else  {
			ucEventDiff = (UCHAR)0xFF - ucPreviousEventCount + ucEventCount + 1;
		}

		ulAcumEventCount += ucEventDiff;

		// update cumulative time

		if (usTime1024 > usPreviousTime1024) {
			usTimeDiff1024 = usTime1024 - usPreviousTime1024;
		}
		else  {
			usTimeDiff1024 = (USHORT)(0xFFFF - usPreviousTime1024 + usTime1024 + 1);
		}

		ulAcumTime1024 += usTimeDiff1024;


		// calculate R-R Interval

		if (ucEventDiff == 1) {
			usR_RInterval1024 = usTimeDiff1024;
		}

		// calculate heart rate (from timing data), in ant_bpm

		if (usTimeDiff1024 > 0) {
			calculated_hr = (UCHAR)( ((USHORT)ucEventDiff * 0xF000) / usTimeDiff1024 );     // 1 min = 0xF000 = 60 * 1024
		}
	}
	else  {
		ucNoEventCount++;
		if (ucNoEventCount >= MAX_NO_EVENTS) {
			instantaneous_hr = calculated_hr = HRM_INVALID_BPM;   // No new valid HR data has been received
		}
	}

	if (calculated_hr != HRM_INVALID_BPM) {
		int diff = abs(calculated_hr - instantaneous_hr);
		if (diff > 5) {
			// maybe average ant_hr with the calculated hr? What to do?
			// observed calculated_hr's after things were up and running:
			// 26, 27, 28
			// so you'd need a median filter if you use that.
		}
	}

	// update previous time and event count

	if (ucPageNum != HRM_PAGE4) {
		usPreviousTime1024 = usTime1024;  // only if not previously obtained from HRM message
	}

	prevcnt = ucPreviousEventCount;

	ucPreviousEventCount = ucEventCount;

	if (hrcalls > 5)  {
		int diff = abs(calculated_hr-previous_calculated_hr);
		if (diff > 5)  {
			spikes++;
			//calculated_hr = previous_calculated_hr;
		}
	}
	previous_calculated_hr = calculated_hr;

	//return calculated_hr;
	return instantaneous_hr;
}                 // int decode_hr()




/********************************************************************************************************

********************************************************************************************************/

int ANTDEV::stop(void) {


	somethread->exit();
	txtimer->deleteLater();
	rxtimer->deleteLater();

	QThread::msleep(500);         // wait for communication to the ANT sticks to stop (just in case)

	if (handle) {
		libusb_close(handle);
		handle = NULL;
	}

	return 0;
}                                // stop()

/********************************************************************************************************

********************************************************************************************************/

int ANTDEV::set_devtype(int _chan_num, int _devtype) {
	int rc;

	if (_chan_num < 0 || _chan_num > 7) {
		return 2;
	}

	chanmap[_chan_num].devtype = _devtype;

	switch (chanmap[_chan_num].devtype) {
		case 121: {                   // speed/cadence
			rc = 0;
			chanmap[_chan_num].freq = 57;
			chanmap[_chan_num].period = 8086;
			chanmap[_chan_num].number = _chan_num;
			//strcpy(channels[_chan_num].typestr, "Speed Cadence");
			break;
		}

		case 120: {                   // heartrate
			rc = 0;
			chanmap[_chan_num].freq = 57;
			chanmap[_chan_num].period = 8070;
			chanmap[_chan_num].number = _chan_num;
			//strcpy(channels[_chan_num].typestr, "Heart Rate");
			break;
		}

		default: {
			rc = 1;
			break;
		}
	}

	return rc;
}                             // set_devtype()


/********************************************************************************************************

********************************************************************************************************/

int ANTDEV::set_period(int _chan_num, unsigned short _period) {

	if (_chan_num < 0 || _chan_num > 7) {
		return 2;
	}

	chanmap[_chan_num].period = _period;

	return 0;
}                                   // set_period()


/******************************************************************************

******************************************************************************/

void ANTDEV::test(void) {

	return;
}


/********************************************************************************************************

********************************************************************************************************/

int ANTDEV::set_device(libusb_device *_device) {
	int status;
	int rc = 0;
	char str[256];
	libusb_config_descriptor *config = NULL;
	int interface_number = -1;

	device = _device;

	status = libusb_get_device_descriptor(device, &desc);       // desc.idVendor, desc.idProduct
	if (status != 0) {
		rc = status;
		goto finis;
	}

	status = libusb_get_config_descriptor(device, 0, &config);
	if (status != 0) {
		if (config) {
			libusb_free_config_descriptor(config);
			config = NULL;
		}
		rc = status;
		goto finis;
	}

	if (config->bNumInterfaces == 0) {
		libusb_free_config_descriptor(config);
		config = NULL;
		rc = -817;
		goto finis;
	}

	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;


	for(int i=0; i<(int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			if (interface_number==-1)  {
				interface_number = interdesc->bInterfaceNumber;
			}

			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				unsigned char epaddr = epdesc->bEndpointAddress;
				if ((epaddr&0x80)==LIBUSB_ENDPOINT_OUT)  {					// 0
					wep = epaddr & 0x0f;
				}
				else if ((epaddr&0x80)==LIBUSB_ENDPOINT_IN)  {			// 128
					rep = epaddr;
				}
			}
		}
	}

	libusb_free_config_descriptor(config);
	config = NULL;


	status = libusb_open(device, &handle);    // Open a device and obtain a device handle

	switch (status) {
		case 0: {                  // on success
			break;
		}
		case LIBUSB_ERROR_NO_MEM:           // -11	on memory allocation failure
		case LIBUSB_ERROR_ACCESS:           // -3		if the user has insufficient permissions
		case LIBUSB_ERROR_NO_DEVICE: {      // -4		if the device has been disconnected
			rc = status;
			goto finis;
		}
		default:
			rc = status;
			goto finis;
	}

#ifdef WIN32
//	status = libusb_set_configuration(handle, -1);
//	switch(status)  {
//		case 0:
//			break;
//		case LIBUSB_ERROR_INVALID_PARAM:		// -2
//			bp = 1;
//			break;
//		case LIBUSB_ERROR_NOT_FOUND:			// -5
//			bp = 1;
//			//if (auto_claim(transfer, &current_interface, USB_API_WINUSBX) != LIBUSB_SUCCESS)
//			//	return LIBUSB_ERROR_NOT_FOUND;
//			break;
//		default:
//			bp = 2;
//			break;
//	}

	status = libusb_claim_interface (handle, interface_number);
	switch(status)  {
		case 0:
			//bp = 0;
			break;
		case LIBUSB_ERROR_ACCESS:		// -3
			//bp = 1;
			break;
		default:
			//bp = 2;
			break;
	}
#endif

	pid = desc.idProduct;
	vid = desc.idVendor;
	nconfigs = desc.bNumConfigurations;

	status = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, (unsigned char*)str, sizeof(str) - 1);
	Q_ASSERT(status >= 0);

	strncpy(mfgr, str, sizeof(mfgr) - 1);

	status = libusb_get_string_descriptor_ascii(handle, desc.iProduct, (unsigned char*)str, sizeof(str) - 1);
	strncpy(prod, str, sizeof(prod) - 1);

	status = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (unsigned char*)sn, sizeof(sn));
	strncpy(serial_number, str, sizeof(serial_number) - 1);


finis:
	return rc;
}                             // set_device()

/********************************************************************************************************
	called by ANT::scan_for_devices()
********************************************************************************************************/

int ANTDEV::start(void) {
	int status;

#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls==2)  {
		bp = 3;
	}
	Q_ASSERT(listening==false);
#endif

	response = 0xff;

	bool b;

	if (somethread == NULL) {

		somethread = new QThread(this);
		if (txtimer == NULL) {
			txtimer = new QTimer(0);
			txtimer->setInterval(333);
			txtimer->moveToThread(somethread);
			b = (bool)connect(txtimer, SIGNAL(timeout()), this, SLOT(writer(void)), Qt::DirectConnection);
			Q_ASSERT(b);
			b = (bool)QObject::connect(somethread, SIGNAL(started()), txtimer, SLOT(start()));
			Q_ASSERT(b);
		}

		// xyzzy

		if (rxtimer == NULL) {
			rxtimer = new QTimer(0);
			rxtimer->setInterval(350);
			rxtimer->moveToThread(somethread);
			b = (bool)connect(rxtimer, SIGNAL(timeout()), this, SLOT(reader(void)), Qt::DirectConnection);
			Q_ASSERT(b);
			b = (bool)QObject::connect(somethread, SIGNAL(started()), rxtimer, SLOT(start()));
			Q_ASSERT(b);
		}
		somethread->start();
	}

	checksum(reset, sizeof(reset));

	if (debug_level > 1)  {
		lock_logging();
		logg("%s %s\n", mfgr, prod);
		unlock_logging();
	}
	status = send(reset, sizeof(reset));
	if (status) {
		strcpy(error_string, "ANT reset failed 1");
		return 1;
	}
	if (!wfm(MESG_STARTUP_MESG_ID, MESSAGE_TIMEOUT)) {             // waits up to 3.3 seconds for msgdone to become true
		strcpy(error_string, "ANT reset failed 2");
		return 1;
	}


	QThread::msleep(850);               // specs say wait 500ms after reset before sending any more host commands

	checksum(set_network_key, sizeof(set_network_key));
	status = send(set_network_key, sizeof(set_network_key));
	if (status) {
		strcpy(error_string, "SetNetworkKey failed 1");
		return 1;
	}
	if (!wfr(MESG_NETWORK_KEY_ID, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "SetNetworkKey failed 2");
		return 1;
	}

#ifdef _DEBUG
	Q_ASSERT(listening==false);
#endif

	return 0;

}                          // start(void)  {			starts the device only

/**************************************************************************************************

**************************************************************************************************/

void ANTDEV::writer(void) {
	unsigned short len;
	int status;

//	if(destructing) {
//		return;
//	}

	//tmr->update();

	// use mutex here
	if (err != 0) {
		err = 0;
	}

	if (txinptr != txoutptr) {
		len = txinptr - txoutptr;
		len &= (TXBUFLEN - 1);
		status = tx(&txq[txoutptr], len, 500L);
		if (status != len) {
		}
		txoutptr = (txoutptr + len) % TXBUFLEN;
	}
	return;
}                                               // writer()

/**************************************************************************************************
	Thread function: Thread ID
	loads stuff into rxq and advances the rxinptr
**************************************************************************************************/

void ANTDEV::reader(void) {
	int status;

	// use mutex here
	if (err != 0) {
		//emit()
		err = 0;
	}

	//tmr->update();

	status = rx();
	switch (status) {
		case 0: {
			break;
		}

		case LIBUSB_ERROR_TIMEOUT: {                             // timeout
			break;
		}

		case LIBUSB_ERROR_PIPE: {              // if the endpoint halted -9
			break;
		}

		case LIBUSB_ERROR_OVERFLOW: {          // if the device offered more data, see Packets and overflows, -8
			break;
		}

		case LIBUSB_ERROR_NO_DEVICE: {         // if the device has been disconnected, -4
			break;
		}

		case LIBUSB_ERROR_IO: {                // Device or resource busy
			break;
		}
		case LIBUSB_ERROR_OTHER:
			//bp = 1;
			break;

		default:  {
			// normally returns the number of bytes received
			if (status < 0) {
				if (debug_level > 1)  {
					lock_logging();
					logg("ANTDEV::reader() status = %d\n", status);
					unlock_logging();
				}
			}
			// new data in queue

			int i, j, len;

			i = rxinptr - rxoutptr;
			len = i & (RXBUFLEN - 1);

			#ifdef _DEBUG
			if (status != len) {
				bp = 2;
			}
			#endif

			for (i = 0; i < len; i++) {
				j = (rxoutptr + i) % RXBUFLEN;
				receiveByte(rxq[j]);
			}
			rxoutptr = (rxoutptr + len) % RXBUFLEN;

			break;
		}
	}

	return;
}                                               // reader()


/********************************************************************************************************
	see pdf file, page 7
	Figure 5-2. Configuring Background Scanning Channel

********************************************************************************************************/

int ANTDEV::start_listening() {
	int status;

	Q_UNUSED(status);

	unsigned char chan = 0;
	unsigned char type;
	net = 0;

	//---------------------------------------------------------------------
#ifdef _DEBUG
	Q_ASSERT(listening==false);
#endif

	get_caps[3] = chan;                        // 0xa4, 0x02, 0x4d, 0x00, 0x54, 0xbf
	checksum(get_caps, sizeof(get_caps));
	status = send(get_caps, sizeof(get_caps));
	if (status) {
		strcpy(error_string, "getcaps failed 1");
		return 1;
	}

	if (!wfm(CAPABILITIES, MESSAGE_TIMEOUT)) {             // waits up to 3.3 seconds for msgdone to become true
		strcpy(error_string, "getcaps failed 2");
		return 1;
	}


#if 0
	if ( !standard_options & CAPABILITIES_NO_RX_CHANNELS ) {
	if ( !standard_options & CAPABILITIES_NO_TX_CHANNELS ) {
	if ( !standard_options & CAPABILITIES_NO_RX_MESSAGES ) {
	if ( !standard_options & CAPABILITIES_NO_TX_MESSAGES ) {
	if ( !standard_options & CAPABILITIES_NO_ACKD_MESSAGES ) {
	if ( !standard_options & CAPABILITIES_NO_BURST_TRANSFER ) {

		//	these bits enabled:

//		CAPABILITIES_NETWORK_ENABLED
//		CAPABILITIES_SERIAL_NUMBER_ENABLED
//		CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED
//		CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED
//		CAPABILITIES_SEARCH_LIST_ENABLED

	if ( advanced_options & CAPABILITIES_OVERUN_UNDERRUN ) {
	if ( advanced_options & CAPABILITIES_NETWORK_ENABLED ) {								// this
	if ( advanced_options & CAPABILITIES_AP1_VERSION_2 ) {
	if ( advanced_options & CAPABILITIES_SERIAL_NUMBER_ENABLED ) {						// this
	if ( advanced_options & CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED ) {			// this
	if ( advanced_options & CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED ) {				// this
	if ( advanced_options & CAPABILITIES_SCRIPT_ENABLED ) {
	if ( advanced_options & CAPABILITIES_SEARCH_LIST_ENABLED ) {						// this

		//	these bits enabled:

//		CAPABILITIES_EXT_MESSAGE_ENABLED
//		CAPABILITIES_SCAN_MODE_ENABLED
//		CAPABILITIES_PROX_SEARCH_ENABLED
//		CAPABILITIES_EXT_ASSIGN_ENABLED

	if ( advanced2_options & CAPABILITIES_LED_ENABLED ) {
	if ( advanced2_options & CAPABILITIES_EXT_MESSAGE_ENABLED ) {						// this
	if ( advanced2_options & CAPABILITIES_SCAN_MODE_ENABLED ) {							// this
	if ( advanced2_options & CAPABILITIES_RESERVED ) {
	if ( advanced2_options & CAPABILITIES_PROX_SEARCH_ENABLED ) {						// this
	if ( advanced2_options & CAPABILITIES_EXT_ASSIGN_ENABLED ) {						// this
	if ( advanced2_options & CAPABILITIES_FS_ANTFS_ENABLED) {
	if ( advanced2_options & CAPABILITIES_FIT1_ENABLED ) {
#endif


	//---------------------------------------------------------------------

	chan = 0;
	type = 0x40;                                 // 0x00 = receive channel, 0x40 = receive only channel
	net = 0;
	assign_channel_extended[3] = chan;
	assign_channel_extended[4] = type;
	assign_channel_extended[5] = net;
	assign_channel_extended[6] = 0x01;
	checksum(assign_channel_extended, sizeof(assign_channel_extended));
	status = send(assign_channel_extended, sizeof(assign_channel_extended));        // 0x42
	if (status) {
		strcpy(error_string, "assign channel failed 1");
		return 1;
	}
	if (!wfr(ASSIGN_CHANNEL, RESPONSE_TIMEOUT)) {         // MESSAGE_TIMEOUT
		strcpy(error_string, "assign channel failed 2");
		return 1;
	}

chanmap[0].state = ANTCHAN::ASSIGNED;								// channels grow here

	//---------------------------------------------------------------------

	set_channel_id[3] = chan;                     // channel
	set_channel_id[4] = 0;                        // devicd number
	set_channel_id[5] = 0;                        // device number
	set_channel_id[6] = 0;                        // devtype
	set_channel_id[7] = 0;                        // transmission type
	checksum(set_channel_id, sizeof(set_channel_id));
	status = send(set_channel_id, sizeof(set_channel_id));
	if (status) {
		strcpy(error_string, "set channel ID failed 1");
		return 1;
	}
	if (!wfr(CHANNEL_ID, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "set channel ID failed 2");
		return 1;
	}
chanmap[0].devtype = 0;


	//---------------------------------------------------------------------

#if 0

		lib_config[4] = 0x80;									// 0xc0 = device number + signal strength, 0x80 is the same as the old 'set_extended_messages', 0xe0 for everything
		checksum(lib_config, sizeof(lib_config));
		status = send(lib_config, sizeof(lib_config));
		if (status) {
			strcpy(error_string, "lib_config failed 1");
			return 1;
		}
		if (!wfr(MESG_LIB_CONFIG, RESPONSE_TIMEOUT)) {
			strcpy(error_string, "lib_config failed 2");
			return 1;
		}
#else
	checksum(set_extended_messages, sizeof(set_extended_messages));
	status = send(set_extended_messages, sizeof(set_extended_messages));
	if (status) {
		strcpy(error_string, "setting extended messages failed 1");
		return 1;
	}
	if (!wfr(ENABLE_EXT_MSGS, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "setting extended messages failed 2");
		return 1;
	}
#endif

	//---------------------------------------------------------------------

	if ( advanced_options & CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED ) {
		lp_search_timeout[3] = chan;
		lp_search_timeout[4] = qRound(2 * 2.5);            // timeout of 5 seconds
		checksum(lp_search_timeout, sizeof(lp_search_timeout));
		status = send(lp_search_timeout, sizeof(lp_search_timeout));
		if (status) {
			strcpy(error_string, "setting LP search timout 1");
			return 1;
		}
		if (!wfr(LP_SEARCH_TIMEOUT, RESPONSE_TIMEOUT)) {                     // 0x63, took 499 ms!!!
			strcpy(error_string, "setting LP search timout 2");
			return 1;
		}
	}

	//---------------------------------------------------------------------

	search_timeout[3] = chan;
	search_timeout[4] = 255;		// infinite search timeout!
	checksum(search_timeout, sizeof(search_timeout));
	status = send(search_timeout, sizeof(search_timeout));
	if (status) {
		strcpy(error_string, "setting search timout 1");
		return 1;
	}
	if (!wfr(SEARCH_TIMEOUT, RESPONSE_TIMEOUT)) {                     // 0x44
		strcpy(error_string, "setting search timout 2");
		return 1;
	}

	//---------------------------------------------------------------------

	set_channel_freq[3] = chan;                     // channel
	set_channel_freq[4] = 57;                       // frequency
	checksum(set_channel_freq, sizeof(set_channel_freq));
	status = send(set_channel_freq, sizeof(set_channel_freq));
	if (status) {
		strcpy(error_string, "set channel frequency failed 1");
		return 1;
	}
	if (!wfr(CHANNEL_FREQUENCY, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "set channel frequency failed 2");
		return 1;
	}
chanmap[0].freq = 57;

	//---------------------------------------------------------------------

	get_channel_status[3] = chan;
	checksum(get_channel_status, sizeof(get_channel_status));
	status = send(get_channel_status, sizeof(get_channel_status));
	checksum(get_channel_status, sizeof(get_channel_status));
	if (status) {
		strcpy(error_string, "get channel status failed 1");
		return 1;
	}
	if (!wfm(CHANNEL_STATUS, MESSAGE_TIMEOUT)) {             // waits up to 3.3 seconds for msgdone to become true
		strcpy(error_string, "get channel status failed 2");
		return 1;
	}

	//---------------------------------------------------------------------

	status = open_listening_channel();
	if (status)  {
		chanmap[0].reset();
		return 1;
	}

	chanmap[0].open = true;
	chanmap[0].number = 0;

#ifdef _DEBUG
#ifndef WIN32
	lock_logging();
	qDebug("\n\n***********************************************************************************************\n\n");
	unlock_logging();
#endif
#endif

	return 0;
}                                // start_listening()


/********************************************************************************************************

********************************************************************************************************/

void ANTDEV::logg(const char *format, ...) {

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


	qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;

	if (logstr[0]==0x0a)  {
		s = QString::asprintf("\n%7lld %s", now, &logstr[1]);
	}
	else  {
		s = QString::asprintf("%7lld %s", now, logstr);
	}

	fprintf(logstream, "%s", qPrintable(s));
	fflush(logstream);

#ifdef _DEBUG
//	#ifndef WIN32
//		s = rstrip(s);							// qDebug() adds its own lf
//		qDebug("%s", qPrintable(s));
//	#endif
#endif


	return;
}                                      // logg

/********************************************************************************************************

********************************************************************************************************/

void ANTDEV::dump(const unsigned char *_buf, int _len, const char *_leader, const char *_trailer) {
	if (!logstream) {
		return;
	}

	QString s;
	int i;

#ifdef _DEBUG
	if (_trailer) {
		bp = 1;
	}
#endif

	qint64 now = QDateTime::currentMSecsSinceEpoch() - start_time;


	if (_leader) {
		if (_leader[0] == '\n') {
			s = QString::asprintf("\n%7lld %-10s", now, &_leader[1]);
		}
		else  {
			s = QString::asprintf("%7lld %-10s", now, _leader);
		}
	}
	else  {
		s = QString::asprintf("%5lld: ", now);
	}

	for (i = 0; i < _len - 1; i++) {
		s += QString::asprintf("%02x ", _buf[i] & 0xff);
	}


	s += QString::asprintf("%02x", _buf[i] & 0xff);

	if (_trailer) {
		s += QString::asprintf("%s", _trailer);
	}
	else  {
		//s += "\n";
	}

	qDebug("%s", qPrintable(s));

	return;
}                          // dump()

/*******************************************************************************************************

*******************************************************************************************************/

int ANTDEV::send(const unsigned char *a, int len) {
	int i, j;
	int rc = 0;

	outpackets++;

#ifdef _DEBUG
	unsigned char msg;
	msg = a[2];

	if (debug_level > 1) {
		if (showtx)  {
			sprintf(leader, "\ntx: ");
			if (msg_strings.contains((msg))) {
				sprintf(trailer, "     %s", msg_strings[msg].toUtf8().constData());
			}
			else  {
				sprintf(trailer, "     unknown _msg: %02x", msg );
			}
			lock_dumping();
			dump(a, len, leader, trailer);
			unlock_dumping();
		}
	}
#endif


	for (i = 0; i < len; i++) {
		j = (txinptr + i) % TXBUFLEN;
		txq[j] = a[i];
	}
	txinptr = (txinptr + i) % TXBUFLEN;

	return rc;
}                             // send()

/*******************************************************************************************************

*******************************************************************************************************/

void ANTDEV::checksum(unsigned char *_buf, int _len) {
	int i;
	unsigned char cs = 0;

	for (i = 0; i < _len - 1; i++) {
		cs ^= _buf[i];
	}
	//cs ^= 0xff;
	_buf[i] = cs;

	return;
}

/*******************************************************************************************************

*******************************************************************************************************/

void ANTDEV::lock_logging(void) {
	bool b;
	QElapsedTimer et;

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

void ANTDEV::unlock_logging(void) {
	logmutex.unlock();
	return;
}

/*******************************************************************************************************

*******************************************************************************************************/

void ANTDEV::lock_dumping(void) {
	bool b;
	QElapsedTimer et;

#ifdef _DEBUG
	qint64 start;
	start = et.nsecsElapsed();
#endif

	b = dumpmutex.tryLock(5);                       // wait up to 5 ms for a lock
	if (b == false) {
	}

#ifdef _DEBUG
	qint64 nanoseconds;
	nanoseconds = et.nsecsElapsed() - start;
	maxdumpwait = qMax(maxdumpwait, nanoseconds);
#endif
	return;
}

/*******************************************************************************************************

*******************************************************************************************************/

void ANTDEV::unlock_dumping(void) {
	dumpmutex.unlock();
	return;
}


/*******************************************************************************************************

*******************************************************************************************************/

int ANTDEV::open_listening_channel(void)  {
	int status;

	open_channel[3] = 0;
	checksum(open_channel, sizeof(open_channel));
	status = send(open_channel, sizeof(open_channel));
	if (status) {
		strcpy(error_string, "open channel failed 1");
		return 1;
	}
	if (!wfr(OPEN_CHANNEL, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "open channel failed 2");
		return 1;
	}

	listening = true;
	return 0;
}


/*******************************************************************************************************
	38760 121, 33666 120
*******************************************************************************************************/

//const char * ANTDEV::get_sensors(void)  {
QString ANTDEV::get_sensors_string(void)  {

	QString s2;
	int cnt = 0;
	Q_UNUSED(cnt);

	sensors_string.clear();

	int n = chanmap[0].sensors.size();

	if (n==0)  {
		return NULL;
	}


	foreach(SENSOR sensor, chanmap[0].sensors ) {
		cnt++;
		if (cnt==n)  {
			s2 = QString::asprintf("%d %d", sensor.sn, sensor.type);
			sensors_string += s2;
		}
		else  {
			sensors_string = QString::asprintf("%d %d, ", sensor.sn, sensor.type);
		}
	}

	return sensors_string;
	//return sensors_string.toUtf8().constData();
	//return qPrintable(s);
}


/*******************************************************************************************************
	38760 121, 33666 120
*******************************************************************************************************/

const SENSORS* ANTDEV::get_sensors(void)  {
	memset(&sensors, 0, sizeof(sensors));
	if (chanmap.size()==0)  {
		return &sensors;
	}

	sensors.n = chanmap[0].sensors.size();

	int i = -1;

	foreach(SENSOR s, chanmap[0].sensors ) {
		i++;
		sensors.sensors[i].sn = s.sn;
		sensors.sensors[i].type = s.type;
		sensors.sensors[i].assigned_channel = s.assigned_channel;
		i++;
		if (i==MAXSENSORS)  {
			break;
		}
	}
	return &sensors;
}							// get_sensors()



/*******************************************************************************************************

*******************************************************************************************************/

int ANTDEV::close_chan(int _chan)  {
	int status;
	int rc = 0;

	// 0x4C


	close_channel[3] = _chan;
	checksum(close_channel, sizeof(close_channel));
	status = send(close_channel, sizeof(close_channel));
	if (status) {
		strcpy(error_string, "close channel failed 1");
		rc = -1;
		goto finis;
	}


	if (!wfr(EVENT_RX_SEARCH_TIMEOUT, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "wait for search timeout failed 2");
		rc = -1;
		goto finis;
	}

finis:
	return rc;
}				// close_chan



/*******************************************************************************************************

*******************************************************************************************************/

int ANTDEV::get_hr(unsigned short _sn)  {

	foreach(ANTCHAN chan, chanmap ) {
		foreach(SENSOR sensor, chan.sensors )  {
			if (sensor.sn == _sn)  {
				if (sensor.type!=HR_DEV_TYPE)  {
					return 0.0f;
				}
				else  {
					return sensor.val;
				}
			}
		}
	}

	return 0;
}								// get_hr()

/*******************************************************************************************************

*******************************************************************************************************/

int ANTDEV::assign(int _sn, int _devtype)  {
	int status;
	int rc=0;
	//int chan;
	unsigned char freq;
	int type;
	unsigned short period;
	int nextchan=0;
	bool ok;

	if (chanmap.size()==0)  {
		strcpy(error_string, "no listener channel");
		rc = -1;
		goto finis;
	}

	if (_devtype==HR_DEV_TYPE)  {								// heart rate, 120
		freq = 57;
		period = 8070;
	}
	else if (_devtype==SC_DEV_TYPE)  {									// speed cadence, 121
		freq = 57;
		period = 8086;
	}
	else if (_devtype==C_DEV_TYPE)  {								// cadence, 122
		strcpy(error_string, "candence sensors not supported");
		rc = -1;
		goto finis;
	}
	else  {
		sprintf(error_string, "devtype %d not supported", _devtype);
		rc = -1;
		goto finis;
	}


	foreach(ANTCHAN chan, chanmap ) {
		if (chan.state==ANTCHAN::UNASSIGNED)  {
			break;
		}
		nextchan++;
		if (nextchan==8)  {
			break;
		}
	}

	if (nextchan==8)  {
		strcpy(error_string, "No more channels available");
		rc = -1;
		goto finis;
	}

	//xxx

	ok = false;

	foreach(SENSOR sensor, chanmap[0].sensors )  {
		if (sensor.sn == _sn)  {
			chanmap[0].sensors[_sn].assigned_channel = nextchan;
			ok = true;
		}
	}

	if (!ok)  {
		strcpy(error_string, "sensor not in listening channel");
		rc = -1;
		goto finis;
	}

#ifdef _DEBUG
	//qDebug("ANTDEV::assign(%04x, %02x) %d, %d, period = %04x hex ******************************************", _sn, _devtype, _sn, _devtype, 8086);
#endif

	////////////////////////////////////////////////////////////
	// 0x42

	type = 0x00;                                 // 0x00 = receive channel, 0x40 = receive only channel
	assign_channel[3] = nextchan;
	assign_channel[4] = type;
	assign_channel[5] = net;							// public network for entire device
	checksum(assign_channel, sizeof(assign_channel));
#ifdef _DEBUG
	//showtx = true;
#endif

	status = send(assign_channel, sizeof(assign_channel));        // 0x42
	if (status) {
		strcpy(error_string, "assign channel failed 1");
		rc = -1;
		goto finis;
	}
	if (!wfr(ASSIGN_CHANNEL, RESPONSE_TIMEOUT)) {         // MESSAGE_TIMEOUT
		strcpy(error_string, "assign channel failed 2");
		rc = -1;
		goto finis;
	}

	////////////////////////////////////////////////////////////
	// 0x51

	set_channel_id[3] = nextchan;													            // channel
	set_channel_id[4] = (unsigned char)(_sn&0xff);                  // device number
	set_channel_id[5] = (unsigned char)((_sn>>8)&0xff);					            // device number
	//set_channel_id[4] = 0x00;												               // device number
	//set_channel_id[5] = 0x00;					            // device number
	set_channel_id[6] = _devtype;                        // devtype
	set_channel_id[7] = 0;			                       // transmission type
	checksum(set_channel_id, sizeof(set_channel_id));
	status = send(set_channel_id, sizeof(set_channel_id));
	if (status) {
		strcpy(error_string, "set channel ID failed 1");
		rc = -1;
		goto finis;
	}
	if (!wfr(CHANNEL_ID, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "set channel ID failed 2");
		rc = -1;
		goto finis;
	}


	////////////////////////////////////////////////////////////
	// 0x45

	set_channel_freq[3] = nextchan;                     // channel
	set_channel_freq[4] = freq;                       // frequency
	checksum(set_channel_freq, sizeof(set_channel_freq));
	status = send(set_channel_freq, sizeof(set_channel_freq));
	if (status) {
		strcpy(error_string, "set channel frequency failed 1");
		rc = -1;
		goto finis;
	}
	if (!wfr(CHANNEL_FREQUENCY, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "set channel frequency failed 2");
		rc = -1;
		goto finis;
	}

	////////////////////////////////////////////////////////////
	// 0x43

	channel_period[3] = nextchan;                     // channel
	channel_period[4] = period&0xff;                       // period
	channel_period[5] = (period>>8)&0xff;                       // period

	//stMessage.aucData[1] = (UCHAR)(usDeviceNumber_ & 0xFF);
	//stMessage.aucData[2] = (UCHAR)((usDeviceNumber_ >>8) & 0xFF);

	checksum(channel_period, sizeof(channel_period));
	status = send(channel_period, sizeof(channel_period));
	if (status) {
		strcpy(error_string, "set channel period failed 1");
		rc = -1;
		goto finis;
	}
	if (!wfr(CHANNEL_PERIOD, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "set channel period failed 2");
		rc = -1;
		goto finis;
	}

	////////////////////////////////////////////////////////////
	// 0x4B

	open_channel[3] = nextchan;
	checksum(open_channel, sizeof(open_channel));
	status = send(open_channel, sizeof(open_channel));
	if (status) {
		strcpy(error_string, "open channel failed 1");
		rc = -1;
		goto finis;
	}
	if (!wfr(OPEN_CHANNEL, RESPONSE_TIMEOUT)) {
		strcpy(error_string, "open channel failed 2");
		rc = -1;
		goto finis;
	}




	chanmap[nextchan].state = ANTCHAN::ASSIGNED;					// channels grow here
	chanmap[nextchan].devtype = _devtype;
	chanmap[nextchan].freq = freq;
	chanmap[nextchan].period = period;
	chanmap[nextchan].number = nextchan;

	//sz = channels[nextchan].sensors.size();

finis:
#ifdef _DEBUG
	//qDebug("ANTDEV::assign() x ******************************************\n");
	//showtx = false;
	//dump_channels("assign exit");
#endif
	return rc;
}									// assign()

/*******************************************************************************************************

*******************************************************************************************************/

int ANTDEV::unassign(int _chan, int _sn)  {
	int rc=0;
	int status;

#ifdef _DEBUG
	static int calls = 0;
	calls++;
#endif

	//xxx

	if (!chanmap.contains((_chan)))  {
		strcpy(error_string, "unassign channel failed 1");
		rc = -1;
		goto finis;
	}

#ifdef _DEBUG
//	qDebug("ANTDEV::unassign() channel %d\n", _chan);
//	showtx = true;
//	showrx = true;
//	oktobreak = true;
//	dump_channels("unassign entry");
#endif

	status = close_chan(_chan);
	if (status)  {
		goto finis;
	}


	// 0x41

	unassign_channel[3] = _chan;
	checksum(unassign_channel, sizeof(unassign_channel));

#ifdef _DEBUG
//	qDebug("ANTDEV::unassign() channel %d\n", _chan);
//	showtx = true;
//	showrx = true;
//	oktobreak = true;
#endif

	status = send(unassign_channel, sizeof(unassign_channel));        // 0x42
	if (status) {
		strcpy(error_string, "unassign channel failed 2");
		rc = -1;
		goto finis;
	}
	if (!wfr(UNASSIGN_CHANNEL, RESPONSE_TIMEOUT)) {         // MESSAGE_TIMEOUT
		strcpy(error_string, "unassign channel failed 3");
		rc = -1;
		goto finis;
	}

	chanmap.remove(_chan);

	if (chanmap[0].sensors.contains(_sn))  {
		chanmap[0].sensors[_sn].assigned_channel = 0;
	}

finis:
#ifdef _DEBUG
	//showtx = false;
	showrx = false;
	//dump_channels("unassign exit");
#endif

	return rc;
}									// unassign()

#ifdef _DEBUG

/*******************************************************************************************************
	38760 121 1
*******************************************************************************************************/

QString ANTDEV::get_sensors_ex(void)  {
	QString s2;
	int cnt = 0;
	Q_UNUSED(cnt);

	sensors_string.clear();

	int n = chanmap[0].sensors.size();

	if (n==0)  {
		return NULL;
	}


	foreach(SENSOR sensor, chanmap[0].sensors ) {
		cnt++;
		if (cnt==n)  {
			s2 = QString::asprintf("%d %d %d", sensor.sn, sensor.type, sensor.assigned_channel);
			sensors_string += s2;
		}
		else  {
			sensors_string = QString::asprintf("%d %d %d, ", sensor.sn, sensor.type, sensor.assigned_channel);
		}
	}
	return sensors_string;
}				// get_sensors_ex()
#endif

/*******************************************************************************************************
Dynastream Innovations ANT USB-m Stick
	chan type O/C--- state
	0    0   open     1

*******************************************************************************************************/

void ANTDEV::dump_channels(const char *title)  {

	if (chanmap.size()==0)  {
		return;
	}

	static bool first = true;
	static qint64 first_time;

	if (first)  {
		first = false;
		first_time = QDateTime::currentMSecsSinceEpoch();
	}

	qint64 ms = QDateTime::currentMSecsSinceEpoch();

	int secs = (ms-first_time)/1000;

	qDebug("\n%4d %s", secs, title);
	qDebug("   %-4s %-4s %-6s %-5s", "chan", "type", "O/C", "state");


	foreach(ANTCHAN chan, chanmap ) {
		if (!chan.open)  {
			//bp = 1;
		}
		qDebug("   %-4d %-4d %-6s %-5d",
				 chan.number,
				 chan.devtype,
				 chan.open?"open":"closed",
				 chan.state
		);
		foreach(SENSOR sensor, chan.sensors )  {
			qDebug("      %-5d %-16s %d",
				sensor.sn,
				qPrintable(types[sensor.type]),
				sensor.assigned_channel
			);
		}
	}

	return;
}								// dump_channels()
