
/***************************************************************************************************
	speed_cadence
		times out in 30 seconds from
		times out in 288 seconds
***************************************************************************************************/

#include "aunt.h"

#ifdef DO_ANT

#pragma warning(disable:4996)					// for vista strncpy_s

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <assert.h>


#include <glib_defines.h>
#include <averagetimer.h>

void ant_set_circumference(float _mm);		//  { circumference_cm = ROUND(_mm/10.0f); }
int ant_close(void);

// internal:

int gen_start(ANT *);
int connect(ANT *);

bool ant_WaitAck(UCHAR _msg, unsigned long _ms);
int ant_decode_speed_cadence(void);
int ant_decode_hr(void);
void ant_log(char *str, bool flush=false);
void (*ant_keyfunc)(void);
void ant_print_caps(void);
static DSI_THREAD_RETURN RunMessageThread(void *pvParameter_);		//Starts the Message thread.
void MessageThread(void);											//Listens for a response from the module
void ant_process(ANT_MESSAGE stMessage, USHORT usSize_);			//Decodes the received message
void ant_run(void);

//--------------------------------------
// globals:
//--------------------------------------

char channel_status_string[4][32] = {
									"UA",									// "UNASSIGNED CHANNEL",
									"A",									// "ASSIGNED CHANNEL",
									"S",									// "SEARCHING CHANNEL",
									"T"									// "TRACKING CHANNEL"
};

DWORD start_time;							// the time the message thread starts
bool got_caps = false;
bool got_status = false;
bool got_id = false;
int devnum = 0;
int netnum = 0;
bool log_raw = false;

#ifdef _DEBUG
	int hrcalls;
	int sccalls;
#endif

unsigned char ant_key[8] = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};		// antplus key
USHORT ant_baud = 57600;

AverageTimer gat("gant");
bool asi;
bool sensors_initialized;
unsigned long g_ant_tid;

char ant_logname[16];

CAD cad;
HR hr;
SC sc;

DWORD ant_start_time = 0L;

FILE *ant_logstream = NULL;
bool do_ant_logging = false;
bool inprocess;
bool ant_closed;
char ant_gstring[2048];
char pstr[256];
char ant_error_string[256];
char ant_str[256];
char gstatus[4][32];					// astrstatus
USHORT usDevicePID;
USHORT usDeviceVID;
UCHAR aucDeviceDescription[256];
UCHAR aucDeviceSerial[256];
int ant_bp;
UCHAR MAX_NO_EVENTS = 12;						// Maximum number of messages with no new events (3s)
bool bursting;										//holds whether the bursting phase of the test has started
bool ant_broadcasting;
bool ant_done;
bool ant_rundone;
DSIFramerANT* framer;
DSI_THREAD_ID uiDSIThread;
DSI_CONDITION_VAR condTestDone;
DSI_MUTEX mutexTestDone;
bool ant_display;
UCHAR ant_txbuf[ANT_STANDARD_DATA_PAYLOAD_SIZE];
bool gant_channel_opened;
UCHAR ant_caps[5];
//UCHAR gstatusbyte;						// ucStatusByte;
unsigned char ant_ack;
unsigned char ant_msg;
DSISerialGeneric *pclSerialObject;
unsigned char ant_packet[8];

/**************************************************************************************

**************************************************************************************/

int ant_init(void (*_ant_keyfunc)(void) )  {
	if (asi)  {
		return 0;
	}

	ant_start_time = timeGetTime();

	cad.reset();
	hr.reset();
	sc.reset();

	ant_keyfunc = _ant_keyfunc;

	strcpy(ant_logname, "ant.log");

#ifdef _DEBUG
	do_ant_logging = false;
#else
	do_ant_logging = false;
#endif


	if (do_ant_logging)  {
		ant_logstream = fopen(ant_logname, "wt");
	}

	BOOL B;

#ifdef _DEBUG
	hrcalls = 0;
	sccalls = 0;
#endif

	memset(ant_packet, 0, 8);

	ant_closed = false;

	//ant_channel_opened = false;
	memset(ant_caps, 0, sizeof(ant_caps));

	asi = false;
	sensors_initialized = false;
	inprocess = false;

	pclSerialObject = (DSISerialGeneric*)NULL;

	framer = (DSIFramerANT*)NULL;
	uiDSIThread = (DSI_THREAD_ID)NULL;
	ant_rundone = false;
	ant_done = false;
	ant_display = true;
	ant_broadcasting = false;

	memset(ant_txbuf,0,ANT_STANDARD_DATA_PAYLOAD_SIZE);
	bursting = false;
	memset(&condTestDone, 0, sizeof(condTestDone));

	mutexTestDone = 0;

	memset(ant_gstring, 0, sizeof(ant_gstring));
	memset(ant_error_string, 0, sizeof(ant_error_string));
	memset(pstr, 0, sizeof(pstr));
	memset(ant_str, 0, sizeof(ant_str));
	//ucStatusByte = 0;

	g_ant_tid = GetCurrentThreadId();

	memset(gstatus, 0, sizeof(gstatus));

	// Initialize condition var and mutex
	UCHAR ucCondInit = DSIThread_CondInit(&condTestDone);
	assert(ucCondInit == DSI_THREAD_ENONE);

	UCHAR ucMutexInit = DSIThread_MutexInit(&mutexTestDone);
	assert(ucMutexInit == DSI_THREAD_ENONE);

#if defined(DEBUG_FILE)
	// Enable ant_loging
	DSIDebug::Init();
	DSIDebug::SetDebug(TRUE);
#endif

	// Create Serial object

	if (!pclSerialObject)  {
		pclSerialObject = new DSISerialGeneric();			// memleak
		assert(pclSerialObject);
	}

	#ifdef _DEBUG
	if (do_ant_logging)  {
		if (pclSerialObject)  {
			ant_log("have pclSerialObject\n", true);
		}
		else  {
			ant_log("do not have pclSerialObject\n", true);
		}
	}

	#endif

	//--------------------------------
	// try 57600 baud:
	//--------------------------------

	ant_baud = 57600;

	{
		DEL(framer);
		B = pclSerialObject->Init(ant_baud, devnum);			// <<<<<<<<<<<<<<<< 57600, 0
		assert(B);

		if (do_ant_logging)  {
			sprintf(ant_gstring, "baud = %d, devnum = %d\n", ant_baud, devnum);
			ant_log(ant_gstring);
		}

		framer = new DSIFramerANT(pclSerialObject);
		assert(framer);
		B = framer->Init();
		assert(B);
		pclSerialObject->SetCallback(framer);
		if (do_ant_logging)  {
			sprintf(ant_gstring, "pclSerialObject->Open(), baud = %d\n", ant_baud);
			ant_log(ant_gstring, true);
		}

//#ifdef _DEBUG
#if 0
		{
			int dn = pclSerialObject->GetDeviceNumber();
			if(framer->GetDeviceUSBInfo(dn, aucDeviceDescription, aucDeviceSerial, USB_MAX_STRLEN))  {
				printf("  Product Description: %s\n", aucDeviceDescription);
				printf("  Serial String: %s\n", aucDeviceSerial);
			}
		}
#endif

		B = pclSerialObject->Open();
	}

	// If the Open function failed, most likely the device
	// we are trying to access does not exist, or it is connected
	// to another program

	if(!B)  {
		sprintf(ant_gstring, "Failed to connect to device at USB port %d\n", devnum);
		ant_log(ant_gstring, true);
		DEL(framer);
		DEL(pclSerialObject);
		return 1;
	}

	// Create message thread.
	//uiDSIThread = DSIThread_CreateThread(&RunMessageThread, this);
	uiDSIThread = DSIThread_CreateThread(&RunMessageThread, NULL);
	assert(uiDSIThread);

	//-------------------------------------------------------------

	if (do_ant_logging)  {
		strcpy(ant_gstring, "ResetSystem()\n");
		ant_log(ant_gstring);
	}

	B = framer->ResetSystem();

#ifdef _DEBUG
	Sleep(1000);
#endif

	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "ResetSystem failed.\n");
			//printf("%s", ant_error_string);
			ant_log(ant_gstring, true);
		}
		DEL(framer);
		DEL(pclSerialObject);
		return 2;
	}

	DSIThread_Sleep(1000);

	ant_bp = 0;

	//-------------------------------------------------------------

	if (do_ant_logging)  {
		sprintf(ant_gstring, "SetNetworkKey(%d, <%x, %x, %x, %x, %x, %x, %x, %x >)\n",
					netnum,								// ant_sensors[n].net_num,
					ant_key[0],
					ant_key[1],
					ant_key[2],
					ant_key[3],
					ant_key[4],
					ant_key[5],
					ant_key[6],
					ant_key[7]
				);
		ant_log(ant_gstring, true);
	}

	B = framer->SetNetworkKey(netnum, ant_key);
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetNetworkKey failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 3;
	}
	if (!ant_WaitAck(MESG_NETWORK_KEY_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetNetworkKey failed 2.\n");
			ant_log(ant_error_string, true);
		}
		return 4;
	}

#if defined (ENABLE_EXTENDED_MESSAGES)
	if (do_ant_logging)  {
		strcpy(ant_gstring, "RxExtMesgsEnable(TRUE)\n");
		ant_log(ant_gstring, true);
	}
	B = framer->RxExtMesgsEnable(TRUE);
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "RxExtMesgsEnable failed 1\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_RX_EXT_MESGS_ENABLE_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "RxExtMesgsEnable failed 2\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

#endif

	if (do_ant_logging)  {
		strcpy(ant_gstring, "Requesting capabilities\n");
		ant_log(ant_gstring, true);
	}
	ANT_MESSAGE_ITEM stResponse;
	framer->SendRequest(MESG_CAPABILITIES_ID, 0, &stResponse, 0);					// request capabilites

	// wait for request to be finished

	DWORD start = timeGetTime();
	got_caps = false;

	while(1)  {
		if ((timeGetTime()-start) > 1000L)  {
			break;
		}
		if (got_caps)  {
			break;
		}
	}
	got_caps = false;



	if (do_ant_logging)  {
		strcpy(ant_gstring, "GetDeviceUSBVID(usDeviceVID)\n");
		ant_log(ant_gstring, true);
	}

	if(framer->GetDeviceUSBVID(usDeviceVID))  {
		if (do_ant_logging)  {
			sprintf(ant_gstring, "   VID: 0x%X\n", usDeviceVID);
			ant_log(ant_gstring, true);
		}
	}

	if (do_ant_logging)  {
		strcpy(ant_gstring, "GetDeviceUSBPID(usDevicePID)\n");
		ant_log(ant_gstring, true);
	}
	if(framer->GetDeviceUSBPID(usDevicePID))  {
		if (do_ant_logging)  {
			sprintf(ant_gstring, "   PID: 0x%X\n", usDevicePID);
			ant_log(ant_gstring, true);
		}
	}

	if (do_ant_logging)  {
		strcpy(ant_gstring, "GetDeviceUSBInfo\n");
		ant_log(ant_gstring, true);
	}
	if(framer->GetDeviceUSBInfo(pclSerialObject->GetDeviceNumber(), aucDeviceDescription, aucDeviceSerial, USB_MAX_STRLEN))  {
		if (do_ant_logging)  {
			sprintf(ant_gstring, "   Product Description: %s\n", aucDeviceDescription);
			ant_log(ant_gstring, true);

			sprintf(ant_gstring, "   Serial String: %s\n", aucDeviceSerial);
			ant_log(ant_gstring, true);
		}
	}


	//-------------------------------------------------------------

	if (do_ant_logging)  {
		ant_log("init device done\n\n", true);
	}

	ant_bp = 0;

	//ant_add_sc(sc);
	//ant_add_heartrate(hr);

	//ant_stick_initialized = true;
	asi = true;

	return 0;

}										// ant_init()

/**************************************************************************************

**************************************************************************************/

int ant_start_sc(void)  {
	if (!asi)  {
		return 1;
	}

	ANT *s = &sc;
	gen_start(s);
	return 0;
}


/**************************************************************************************

**************************************************************************************/

int ant_start_heartrate(void)  {
	if (!asi)  {
		return 1;
	}
	ANT *s = &hr;
	gen_start(s);
	return 0;
}


/**************************************************************************************

**************************************************************************************/


int ant_start_cad(void)  {
	if (!asi)  {
		return 1;
	}
	ANT *s = &cad;
	gen_start(s);
	return 0;
}

/**************************************************************************************
	chan_num
	dev_type
	period
	freq
**************************************************************************************/

int gen_start(ANT *s)  {

	if (!framer)  {
		return 1;
	}

#ifdef _DEBUG
	static int calls = 0;
	int bp = 0;
	calls++;
	if (calls > 1)  {
		bp = 3;
	}
#endif

	sprintf(ant_gstring, "\n\n%10ld AssignChannel(chan = %d, %d, %d), (%s to channel %d)\n", timeGetTime(), s->chan_num, 0, 0, s->name, s->chan_num);
	ant_log(ant_gstring, true);

	BOOL B = framer->AssignChannel(s->chan_num,	0, 0);			// <<<<<<<<<<<<<<  MESG_ASSIGN_CHANNEL_ID
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "AssignChanel failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_ASSIGN_CHANNEL_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "AssignChanel failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

	//-------------------------------------------------------------

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld SetChannelId(%d, %d, %d, %d)\n", timeGetTime(), s->chan_num, devnum, s->dev_type, s->trans_type);
		ant_log(ant_gstring, true);
	}

	B = framer->SetChannelID(s->chan_num, devnum, s->dev_type, s->trans_type);			// <<<<<<<<<<<<<<

	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelId failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_CHANNEL_ID_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelId failed 2.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	//nca+++ changed to use 30 sec low priority search, no hi prio search
	//
	unsigned char uc = ROUND(30.0f/2.5f);

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld SetLowPriorityChannelSearchTimeout(%d, %d)\n", timeGetTime(), s->chan_num, uc);
		ant_log(ant_gstring, true);
	}

	B = framer->SetLowPriorityChannelSearchTimeout(s->chan_num, uc, MESSAGE_TIMEOUT);

	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetLowPriorityChannelSearchTimeout failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	//if (!ant_WaitAck(MESG_CHANNEL_SEARCH_TIMEOUT_ID, MESSAGE_TIMEOUT))  {
	if (!ant_WaitAck(MESG_SET_LP_SEARCH_TIMEOUT_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetLowPriorityChannelSearchTimeout failed 2.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	uc = 0;  //ROUND(30.0f/2.5f);

	//nca---

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld SetChannelSearchTimeout(%d, %d)\n", timeGetTime(), s->chan_num, uc);
		ant_log(ant_gstring, true);
	}

	B = framer->SetChannelSearchTimeout(s->chan_num, uc, MESSAGE_TIMEOUT);

	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelSearchTimeout failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_CHANNEL_SEARCH_TIMEOUT_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelSearchTimeout failed 2.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

	//////////////////////////////////////////////////////////////////////////////////////

	//-------------------------------------------------------------

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld SetChannelPeriod(%d, %d)\n", timeGetTime(), s->chan_num, s->period);
		ant_log(ant_gstring, true);
	}
	B = framer->SetChannelPeriod(s->chan_num, s->period);			// <<<<<<<<<<<<<<
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelPeriod failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_CHANNEL_MESG_PERIOD_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelPeriod failed 2.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

	//-------------------------------------------------------------

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld SetChannelRFFrequency(%d, %d)\n", timeGetTime(), s->chan_num, s->freq);
		ant_log(ant_gstring, true);
	}

	B = framer->SetChannelRFFrequency(s->chan_num,	s->freq);			// <<<<<<<<<<<<<<
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelRFFrequency failed 1.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_CHANNEL_RADIO_FREQ_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelRFFrequency failed 2.\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

	//-------------------------------------------------------------

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld SetChannelTransmitPower(%d, %d)\n", timeGetTime(), s->chan_num, 3);
		ant_log(ant_gstring, true);
	}
	B = framer->SetChannelTransmitPower(s->chan_num, 3);			// <<<<<<<<<<<<<<
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelTransmitPower failed 1\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_CHANNEL_RADIO_TX_POWER_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "SetChannelTransmitPower failed 2\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

#ifdef _DEBUG
	if (s->chan_num < 0 || s->chan_num > 2)  {
		bp = 5;
	}
#endif

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld OpenChannel(%d)\n", timeGetTime(), s->chan_num);
		ant_log(ant_gstring, true);
	}
	B = framer->OpenChannel(s->chan_num);			// <<<<<<<<<<<<<<
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "OpenChannel failed 1\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_OPEN_CHANNEL_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "OpenChannel failed 2\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	ant_broadcasting = true;
	
	Sleep(100);

//#if defined (ENABLE_EXTENDED_MESSAGES)
#if 0
	if (do_ant_logging)  {
		strcpy(ant_gstring, "RxExtMesgsEnable(TRUE)\n");
		ant_log(ant_gstring, true);
	}
	B = framer->RxExtMesgsEnable(TRUE);
	if (!B)  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "RxExtMesgsEnable failed 1\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}
	if (!ant_WaitAck(MESG_RX_EXT_MESGS_ENABLE_ID, MESSAGE_TIMEOUT))  {
		if (do_ant_logging)  {
			strcpy(ant_error_string, "RxExtMesgsEnable failed 2\n");
			ant_log(ant_error_string, true);
		}
		return 1;
	}

#endif


#if 1
	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld Requesting status(%d)\n", timeGetTime(), s->chan_num);
		ant_log(ant_gstring, true);
	}
	ANT_MESSAGE_ITEM stResponse;
	got_status = false;
	framer->SendRequest(MESG_CHANNEL_STATUS_ID, s->chan_num, &stResponse, 0);				// Request channel status
	// wait for request to be finished

	DWORD start = timeGetTime();

	while(1)  {
		if ((timeGetTime()-start) > 1000L)  {
			break;
		}
		if (got_status)  {
			break;
		}
	}



	//int status;
	//status = connect(s);

	/*
		STATUS_UNASSIGNED_CHANNEL:		// 0
		STATUS_ASSIGNED_CHANNEL:		// 1
		STATUS_SEARCHING_CHANNEL:		// 2
		STATUS_TRACKING_CHANNEL:		// 3
	*/

	if (do_ant_logging)  {
		sprintf(ant_gstring, "%10ld Requesting channel id(%d)\n", timeGetTime(), s->chan_num);
		ant_log(ant_gstring, true);
	}
	framer->SendRequest(MESG_CHANNEL_ID_ID, s->chan_num, &stResponse, 0);					// Request channel ID
	got_id = false;
	start = timeGetTime();

	while(1)  {
		if ((timeGetTime()-start) > 1000L)  {
			break;
		}
		if (got_id)  {
			break;
		}
	}
	got_id = false;
	s->initialized = true;

	return s->channel_status;



#else	
	int status;
	status = connect(_n);
	return status;
#endif

}													// gen_start()

#if 0

/**************************************************************************************
	TRYS to connect once we are TRACKING channnel
**************************************************************************************/

int connect(ANT *s)  {
	ANT_MESSAGE_ITEM stResponse;

	if (s->channel_status == STATUS_TRACKING_CHANNEL)  {
		return -1;
	}

//#ifdef _DEBUG
#if 0
	char str[128];
	sprintf(str, "%.2f ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ connecting\n", (float)(timeGetTime()-ant_start_time) / 1000.0 );
	OutputDebugString(str);
#endif

	//framer->SendRequest(MESG_CAPABILITIES_ID, ant_sensors[n].chan_num, &stResponse, 0);				// Request capabilites.
	//framer->SendRequest(MESG_CHANNEL_STATUS_ID, ant_sensors[_n].chan_num, &stResponse, 0);				// Request channel status

#ifdef _DEBUG
	//Sleep(1000);
#endif

	/*
		STATUS_UNASSIGNED_CHANNEL:		// 0
		STATUS_ASSIGNED_CHANNEL:		// 1
		STATUS_SEARCHING_CHANNEL:		// 2
		STATUS_TRACKING_CHANNEL:		// 3
	*/

	if (do_ant_logging)  {
		sprintf(ant_gstring, "Requesting channel id(%d)\n", s->chan_num);
		ant_log(ant_gstring, true);
	}
	framer->SendRequest(MESG_CHANNEL_ID_ID, s->chan_num, &stResponse, 0);					// Request channel ID
	DWORD start = timeGetTime();
	got_id = false;

	while(1)  {
		if ((timeGetTime()-start) > 1000L)  {
			break;
		}
		if (got_id)  {
			break;
		}
	}
	got_id = false;

	return s->channel_status;
}													// connect()

#endif

/**************************************************************************************

**************************************************************************************/

int ant_close(void)  {
	//ant_close2();

	if (ant_closed)  {
		return 0;
	}

	ant_closed = true;
	//ant_stick_initialized = false;
	asi = false;
	sensors_initialized = false;

	//Wait for test to be done
	DSIThread_MutexLock(&mutexTestDone);
	ant_done = true;

	UCHAR ucWaitResult = DSIThread_CondTimedWait(&condTestDone, &mutexTestDone, DSI_THREAD_INFINITE);
	assert(ucWaitResult == DSI_THREAD_ENONE);

	DSIThread_MutexUnlock(&mutexTestDone);

	//Destroy mutex and condition var
	DSIThread_MutexDestroy(&mutexTestDone);
	DSIThread_CondDestroy(&condTestDone);

	if(pclSerialObject)  {
		pclSerialObject->Close();
	}

#if defined(DEBUG_FILE)
	DSIDebug::Close();
#endif

	if(framer)  {
		delete framer;
	}

	if(pclSerialObject)  {
		delete pclSerialObject;
	}

	FCLOSE(ant_logstream);

	return 0;
}								// ant_close()

/**************************************************************************************

**************************************************************************************/

void ant_run(void)  {

	ant_rundone = false;

	while(!ant_rundone) {
		char c;
		if (kbhit())  {
			c = getch();
			if (c==0)  {
				c = getch();
			}
		}
		else  {
			Sleep(200);
			continue;
		}


		switch(c)  {
			case 0x1b:	{							// escape
				if (do_ant_logging)  {
					ant_bp = 2;
					/*
					sprintf(ant_gstring, "\n\n****** escape hit ********\n\nCloseChannel(%d)\n", cad.chan_num);
					ant_log(ant_gstring, true);
					*/
				}
				ant_broadcasting = false;
				if (do_ant_logging)  {
					strcpy(ant_gstring, "CloseChannel\n");
					ant_log(ant_gstring);
				}
				//framer->CloseChannel(cad.chan_num, MESSAGE_TIMEOUT);
				//if (cad.channel_status)  {
				if (cad.chan_num != -1)  {
					framer->CloseChannel(cad.chan_num, MESSAGE_TIMEOUT);
				}

				//if (sc.connected)  {
				framer->CloseChannel(sc.chan_num, MESSAGE_TIMEOUT);
				//}
				framer->CloseChannel(hr.chan_num, MESSAGE_TIMEOUT);
				break;
			}

			default:  {
				ant_bp = 4;
				break;
			}
		}
		Sleep(100);			// ANT_Nap(0);

	}

	//this->close();
	//ant_close2();
	ant_close();
	return;
}									// ANT::run()

/**************************************************************************************
	Callback function that is used to create the thread. This is a static
	function.
**************************************************************************************/

DSI_THREAD_RETURN RunMessageThread(void *pvParameter_)  {
	MessageThread();
	return NULL;
}

/**************************************************************************************

**************************************************************************************/

void MessageThread()  {
	ANT_MESSAGE antmsg;
	USHORT usSize;
	ant_done = false;
	DWORD now, last_message_time;
	//DWORD last_status_request_time = 0;
	ANT_MESSAGE_ITEM stResponse;
	BOOL B;


#ifdef _DEBUG
	int bp = 0;
	if (do_ant_logging)  {
		unsigned long _tid = GetCurrentThreadId();
		sprintf(ant_str, "   thread starting, tid = %ld\n",g_ant_tid);
		ant_log(ant_str, true);
	}
#endif

	last_message_time = timeGetTime();
	start_time = timeGetTime();

	hr.last_status_request_time = start_time;
	cad.last_status_request_time = start_time + 333;
	sc.last_status_request_time = start_time + 667;


	while(!ant_done)  {
		if(framer->WaitForMessage(1000))  {
			usSize = framer->GetMessage(&antmsg);

			if(ant_done)  {
				break;
			}

			if(usSize == DSI_FRAMER_ERROR)  {								// 65535
				// Get the message to clear the error
				usSize = framer->GetMessage(&antmsg, MESG_MAX_SIZE_VALUE);
				if (do_ant_logging)  {
					sprintf(ant_gstring, "DSI_FRAMER_ERROR, size = %d\n", usSize);
					ant_log(ant_gstring, true);
				}
				continue;
			}

			else if(usSize != DSI_FRAMER_TIMEDOUT && usSize != 0)  {
				last_message_time = timeGetTime();
				ant_process(antmsg, usSize);
			}
			else if(usSize == DSI_FRAMER_TIMEDOUT)  {
#ifdef _DEBUG
				//at.update();																	// 1002 ms
#endif
				if (sensors_initialized)  {
#ifdef _DEBUG
					ant_bp = 6;
#endif

#if 0
					if (!sc.connected)  {
						#ifdef _DEBUG
						ant_bp = 2;
						#endif


						/*
							STATUS_UNASSIGNED_CHANNEL:		// 0
							STATUS_ASSIGNED_CHANNEL:		// 1
							STATUS_SEARCHING_CHANNEL:		// 2
							STATUS_TRACKING_CHANNEL:		// 3
						*/

						if (do_ant_logging)  {
							sprintf(ant_gstring, "Requesting status(%d)\n", sc.chan_num);
							ant_log(ant_gstring, true);
						}
						ANT_MESSAGE_ITEM stResponse;
						got_status = false;
						framer->SendRequest(MESG_CHANNEL_STATUS_ID, sc.chan_num, &stResponse, 0);				// Request channel status
						// wait for request to be finished

						DWORD start = timeGetTime();

						while(1)  {
							if ((timeGetTime()-start) > 1000L)  {
								break;
							}
							if (got_status)  {
								break;
							}
						}

						ant_bp = 1;
					}												// if (!sc.connected)  {
#endif								// #if 0
					/*
					if (do_ant_logging)  {
						sprintf(ant_gstring, "Requesting channel id(%d)\n", s->chan_num);
						ant_log(ant_gstring, true);
					}
					framer->SendRequest(MESG_CHANNEL_ID_ID, s->chan_num, &stResponse, 0);					// Request channel ID
					got_id = false;
					start = timeGetTime();

					while(1)  {
						if ((timeGetTime()-start) > 1000L)  {
							break;
						}
						if (got_id)  {
							break;
						}
					}
					got_id = false;
					s->initialized = true;

					//ant_start_sc();
					*/

				}									// if (sensors_initialized)  {

			}										// else if(usSize == DSI_FRAMER_TIMEDOUT)  {
#ifdef _DEBUG
			else  {
				ant_bp = 6;
			}
#endif


			if (!sensors_initialized)  {
				continue;
			}

			now = timeGetTime();

			//---------------------------------------
			// get channel status once a second
			//---------------------------------------

			if ((now - hr.last_status_request_time) >= 1000)  {
				framer->SendRequest(MESG_CHANNEL_STATUS_ID, hr.chan_num, &stResponse, 0);				// Request channel status
				hr.last_status_request_time = now;
			}
			if ((now - cad.last_status_request_time) >= 1000)  {
				framer->SendRequest(MESG_CHANNEL_STATUS_ID, cad.chan_num, &stResponse, 0);				// Request channel status
				cad.last_status_request_time = now;
			}
			if ((now - sc.last_status_request_time) >= 1000)  {
				framer->SendRequest(MESG_CHANNEL_STATUS_ID, sc.chan_num, &stResponse, 0);				// Request channel status
				sc.last_status_request_time = now;
			}


			//------------------------------------------------
			// maintain decoding_data flag every 5000 ms
			//------------------------------------------------

			if (cad.decoding_data)  {
				if ((now-cad.get_decode_time()) > 5000L)  {
					cad.decoding_data = false;
					cad.val = 0;
					cad.accum = 0.0f;
					cad.accum_count = 0L;
				}
			}
			if (hr.decoding_data)  {
				if ((now-hr.get_decode_time()) > 5000L)  {
					hr.decoding_data = false;
					hr.val = 0;
					hr.calculated_val = 0;
					hr.accum = 0.0f;
					hr.accum_count = 0L;
				}
			}
			if (sc.decoding_data)  {
				if ((now-sc.get_decode_time()) > 5000L)  {
					sc.decoding_data = false;
					sc.set_cadence(0.0f);
				}
			}

			//--------------------------------------------------------------------------------------------------------------
			// try to reopen channnel every 5000 ms if status is 'channel assigned'
			//--------------------------------------------------------------------------------------------------------------

			if (hr.channel_status == STATUS_ASSIGNED_CHANNEL)  {
				if ((now - hr.last_open_attempt_time) >= 5000L)  {
					hr.last_open_attempt_time = now;

					if (do_ant_logging)  {
						sprintf(ant_gstring, "%10ld Calling OpenChannel(%d)\n", now, hr.chan_num);
						ant_log(ant_gstring, true);
					}

					B = framer->OpenChannel(ANT_HR);
					if (!B)  {
						if (do_ant_logging)  {
							strcpy(ant_error_string, "   OpenChannel failed 1\n");
							ant_log(ant_error_string, true);
						}
					}

				}
			}								// if (hr.channel_status == STATUS_SEARCHING_CHANNEL)  {

			if (cad.channel_status == STATUS_ASSIGNED_CHANNEL)  {
				if ((now - cad.last_open_attempt_time) >= 5000L)  {
					cad.last_open_attempt_time = now;

					if (do_ant_logging)  {
						sprintf(ant_gstring, "%10ld Calling OpenChannel(%d)\n", now, cad.chan_num);
						ant_log(ant_gstring, true);
					}

					B = framer->OpenChannel(ANT_C);
					if (!B)  {
						if (do_ant_logging)  {
							strcpy(ant_error_string, "   OpenChannel failed 1\n");
							ant_log(ant_error_string, true);
						}
					}
				}
			}								// if (cad.channel_status == STATUS_SEARCHING_CHANNEL)  {

			if (sc.channel_status == STATUS_ASSIGNED_CHANNEL)  {
				if ((now - sc.last_open_attempt_time) >= 5000L)  {
					sc.last_open_attempt_time = now;

					if (do_ant_logging)  {
						sprintf(ant_gstring, "%10ld Calling OpenChannel(%d)\n", now, sc.chan_num);
						ant_log(ant_gstring, true);
					}

					B = framer->OpenChannel(ANT_SC);
					if (!B)  {
						if (do_ant_logging)  {
							strcpy(ant_error_string, "   OpenChannel failed 1\n");
							ant_log(ant_error_string, true);
						}
					}
				}
			}								// if (sc.channel_status == STATUS_SEARCHING_CHANNEL)  {

		}									// if(framer->WaitForMessage(1000)) 
		else  {
#ifdef _DEBUG
			bp = 1;
			//at.update();
#endif
		}
	}

	DSIThread_MutexLock(&mutexTestDone);
	UCHAR ucCondResult = DSIThread_CondSignal(&condTestDone);
	assert(ucCondResult == DSI_THREAD_ENONE);
	DSIThread_MutexUnlock(&mutexTestDone);

}								// void ANT::MessageThread()  {

/**************************************************************************************

**************************************************************************************/

void ant_process(ANT_MESSAGE antmsg, USHORT usSize_)  {
	inprocess = true;

	BOOL ant_bprintBuffer = FALSE;
	UCHAR ucDataOffset = 1;   // For most data messages

#ifdef _DEBUG
	//unsigned long _tid = GetCurrentThreadId();										// different thread
	int id = antmsg.ucMessageID;
	int tmp2 = antmsg.aucData[0];

	switch(tmp2)  {
		case 32:
			ant_bp = 32;
			break;
		case 0:
			ant_bp = 0;
			break;
		case 1:
			ant_bp = 1;
			break;
		case 8:
			ant_bp = 8;
			break;
		default:
			ant_bp = 0;
			break;
	}
#endif

#ifdef _DEBUG
	switch (antmsg.ucMessageID)  {
		case MESG_RESPONSE_EVENT_ID:				// 0x40
			ant_bp = 1;
			break;
		case MESG_STARTUP_MESG_ID:					// 0x6f
			ant_bp = 5;
			break;
		case MESG_CAPABILITIES_ID:					// 0x54
			ant_bp = 4;
			break;
		case MESG_CHANNEL_STATUS_ID:				// 0x52
			ant_bp = 3;
			break;
		case MESG_CHANNEL_ID_ID:					// 0x51
			ant_bp = 2;
			break;
		case MESG_BROADCAST_DATA_ID:				// 0x4e
			ant_bp = 7;
			break;

		default:
			ant_bp = 255;
			break;
	}

	if (do_ant_logging)  {
		//sprintf(pstr, "msgid = %02x,    %02x %02x %02x\n", antmsg.ucMessageID, antmsg.aucData[0], antmsg.aucData[1], antmsg.aucData[2] );
		//ant_log(pstr);
	}
#endif

	switch(antmsg.ucMessageID)  {
		//RESPONSE MESG
		case MESG_RESPONSE_EVENT_ID:  {
			//int msg;
			ant_msg = antmsg.aucData[1];
			ant_ack = antmsg.aucData[2];								// used in ant_WaitAck()
			ant_bp = 0;

			//RESPONSE TYPE
			switch(ant_msg)  {
				case MESG_NETWORK_KEY_ID:  {
					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error configuring network key: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					if (do_ant_logging)  {
						strcpy(pstr, "	network key set\n");
						ant_log(pstr);
					}
					break;
				}

				case MESG_ASSIGN_CHANNEL_ID:  {
					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "Error assigning channel: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}

					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld Channel assigned\n", timeGetTime() );
						ant_log(pstr);
					}
					break;
				}

				case MESG_CHANNEL_ID_ID:  {
					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error configuring Channel ID: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld Channel ID set\n", timeGetTime());
						ant_log(pstr);
					}
					break;
				}

				case MESG_CHANNEL_MESG_PERIOD_ID:  {
					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "Error configuring Channel ID: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld Channel period set\n", timeGetTime() );
						ant_log(pstr);
					}
					break;
				}

				case MESG_CHANNEL_RADIO_FREQ_ID:   {
					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error configuring Radio Frequency: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld Radio Frequency set\n", timeGetTime() );
						ant_log(pstr);
					}
					break;
				}

				case MESG_RADIO_TX_POWER_ID:  {
					ant_bp = 1;
					break;
				}

				case MESG_CHANNEL_RADIO_TX_POWER_ID:  {
					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld channel tx power set\n", timeGetTime() );
						ant_log(pstr);
					}
					break;
				}

				case MESG_OPEN_CHANNEL_ID:  {
					int chan = antmsg.aucData[0];

					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error opening channel %d: Code 0%d\n", chan, antmsg.aucData[2]);
							ant_log(pstr);
						}
						ant_broadcasting = false;
						break;
					}

					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld Channel opened\n", timeGetTime());
						ant_log(pstr);
						switch(chan)  {
							case ANT_HR:
								hr.channel_open_time = timeGetTime();
								break;
							case ANT_C:
								cad.channel_open_time = timeGetTime();
								break;
							case ANT_SC:
								sc.channel_open_time = timeGetTime();
								break;
							default:
								ant_bp = 1;
								break;
						}

						//channel_open_
					}
					break;
				}

				case MESG_RX_EXT_MESGS_ENABLE_ID:  {
					if(antmsg.aucData[2] == INVALID_MESSAGE)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Extended messages not supported in this ANT product\n");
							ant_log(pstr);
						}
						break;
					}
					else if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error enabling extended messages: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					if (do_ant_logging)  {
						sprintf(pstr, "	Extended messages enabled\n");
						ant_log(pstr);
					}
					break;
				}

				case MESG_UNASSIGN_CHANNEL_ID:  {
					if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error unassigning channel: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					if (do_ant_logging)  {
						sprintf(pstr, "	%10ld Channel unassigned\n", timeGetTime());
						ant_log(pstr);
					}
					ant_rundone = true;
					break;
				}

				case MESG_CLOSE_CHANNEL_ID:  {
					int chan = antmsg.aucData[0];

					if(antmsg.aucData[2] == CHANNEL_IN_WRONG_STATE)  {
						// We get here if we tried to close the channel after the search timeout (slave)
						if (do_ant_logging)  {
							sprintf(pstr, "	Channel is already closed\n");
							ant_log(pstr);
							//sprintf(pstr, "UnAssignChannel(%d, %d)\n", cad.chan_num, MESSAGE_TIMEOUT);
							//ant_log(pstr);
						}

						switch(chan)  {
							case ANT_HR:
								ant_bp = 0;
								break;
							case ANT_C:
								break;
							case ANT_SC:
								ant_bp = 0;
								break;
							default:
								ant_bp = 1;
								break;
						}

						// not sure if I should do this
						//B = framer->UnAssignChannel(chan, MESSAGE_TIMEOUT);

						break;
					}
					else if(antmsg.aucData[2] != RESPONSE_NO_ERROR)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Error closing channel: Code 0%d\n", antmsg.aucData[2]);
							ant_log(pstr);
						}
						break;
					}
					// If this message was successful, wait for EVENT_CHANNEL_CLOSED to confirm channel is closed
					break;
				}									// case MESG_CLOSE_CHANNEL_ID:

				case MESG_REQUEST_ID:  {
					if(antmsg.aucData[2] == INVALID_MESSAGE)  {
						if (do_ant_logging)  {
							sprintf(pstr, "	Requested message not supported in this ANT product\n");
							ant_log(pstr);
						}
					}
					if (do_ant_logging)  {
						sprintf(pstr, "	MESG_REQUEST_ID\n");
						ant_log(pstr);
					}
					break;
				}

				case MESG_EVENT_ID:  {
					int chan = antmsg.aucData[0];

					switch(antmsg.aucData[2])  {
						case EVENT_CHANNEL_CLOSED:  {
							if (do_ant_logging)  {
								char str[32];
								switch(chan)  {
									case ANT_HR:
										strcpy(str, "   (HR)\n");
										break;
									case ANT_C:
										strcpy(str, "   (Cadence)\n");
										break;
									case ANT_SC:
										strcpy(str, "   (Speed/Cadence)\n");
										break;
									default:
										ant_bp = 1;
										break;
								}

								sprintf(pstr, "	%10ld Channel Closed", timeGetTime());
								strcat(pstr, str);
								ant_log(pstr);


								/*
								// not sure if I should do this:
								sprintf(pstr, "%10ld UnAssignChannel(%d, %d)", timeGetTime(), chan, MESSAGE_TIMEOUT);
								strcat(pstr, str);
								ant_log(pstr);
								B = framer->UnAssignChannel(chan, MESSAGE_TIMEOUT);
								*/
							}

							if (do_ant_logging)  {
#ifdef _DEBUG
								static int calls=0;
								calls++;
								if (calls==2)  {
									ant_bp = 1;
								}
#endif
								sprintf(ant_gstring, "%10ld   OpenChannel(%d)\n", timeGetTime(), chan);
								ant_log(ant_gstring, true);
							}

							/*
							BOOL B;

							B = framer->OpenChannel(chan);			// <<<<<<<<<<<<<<
							if (!B)  {
								if (do_ant_logging)  {
									strcpy(ant_error_string, "   OpenChannel failed 1\n");
									ant_log(ant_error_string, true);
								}
								break;
							}

							if (!ant_WaitAck(MESG_OPEN_CHANNEL_ID, MESSAGE_TIMEOUT))  {
								if (do_ant_logging)  {
									strcpy(ant_error_string, "   OpenChannel failed 2\n");
									ant_log(ant_error_string, true);
								}
							}
							*/

							break;
						}														// case EVENT_CHANNEL_CLOSED:  {

						case EVENT_TX:  {
							int chan = antmsg.aucData[0];
							// This event indicates that a message has just been
							// sent over the air. We take advantage of this event to set
							// up the data for the next message period.
							static UCHAR ucIncrement = 0;      // Increment the first byte of the buffer

							ant_txbuf[0] = ucIncrement++;

							// Broadcast data will be sent over the air on
							// the next message period.
							if(ant_broadcasting)  {
								//framer->SendBroadcastData(cad.chan_num, ant_txbuf);
								framer->SendBroadcastData(chan, ant_txbuf);

								// Echo what the data will be over the air on the next message period.
								if(ant_display && do_ant_logging)  {
									sprintf(pstr, "Tx:(%d): [%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x]\n",
										chan,
										ant_txbuf[0],
										ant_txbuf[1],
										ant_txbuf[2],
										ant_txbuf[3],
										ant_txbuf[4],
										ant_txbuf[5],
										ant_txbuf[6],
										ant_txbuf[7]);
									ant_log(pstr);
								}
								else  {
									if (do_ant_logging)  {
										static int iIndex = 0;
										static char ac[] = {'|','/','-','\\'};
										sprintf(pstr, "Tx: %c\r",ac[iIndex++]); fflush(stdout);
										ant_log(pstr);
										iIndex &= 3;
									}
								}
							}
							break;

						}
						case EVENT_RX_SEARCH_TIMEOUT:   {
							if (do_ant_logging)  {
								double dt;
								DWORD now = timeGetTime();

								switch(chan)  {
									case ANT_HR:
										dt = (now - hr.channel_open_time) / 1000.0;
										break;
									case ANT_C:
										dt = (now - cad.channel_open_time) / 1000.0;
										break;
									case ANT_SC:
										dt = (now - sc.channel_open_time) / 1000.0;
										break;
									default:
										dt = 0.0;
										break;
								}

								sprintf(pstr, "	%10ld Search Timeout after %.2f seconds\n", timeGetTime(), dt);
								ant_log(pstr);
							}
							break;
						}
						case EVENT_TRANSFER_RX_FAILED:  {
							if (do_ant_logging)  {
								sprintf(pstr, "	Burst receive has failed\n");
								ant_log(pstr);
							}
							break;
						}
						case EVENT_TRANSFER_TX_COMPLETED:  {
							if (do_ant_logging)  {
								sprintf(pstr, "Tranfer Completed\n");
								sprintf(pstr, "%s", pstr);
								ant_log(pstr);
							}
							break;
						}
						case EVENT_TRANSFER_TX_FAILED:  {
							if (do_ant_logging)  {
								sprintf(pstr, "Tranfer Failed\n");
								ant_log(pstr);
							}
							break;
						}
						case EVENT_RX_FAIL:  {						// 0x02
							//xxx
							/*
								A receive channel missed a message which it was expecting. This happens when a slave is tracking a master
								and is expecting a message at the set message rate.
							*/
							double dt;
							int chan = antmsg.aucData[0];
							DWORD now = timeGetTime();

							switch(chan)  {
								case ANT_HR:
									dt = (now - hr.last_data_time) / 1000.0;
									break;
								case ANT_C:
									dt = (now - cad.last_data_time) / 1000.0;
									break;
								case ANT_SC:
									dt = (now - sc.last_data_time) / 1000.0;
									break;
								default:
									dt = -1.0;
									break;
							}

							if (do_ant_logging)  {
								if (asi)  {
									sprintf(pstr, "	Rx Fail, channel %d, dt since last_data_time: %.2lf seconds\n", chan, dt);
									ant_log(pstr);
								}
							}
							break;
						}
						case EVENT_RX_FAIL_GO_TO_SEARCH:  {
							int chan = antmsg.aucData[0];
							double dt;
							DWORD now = timeGetTime();

							// will recover if this gets hit
							switch(chan)  {
								case ANT_HR:
									dt = (now - hr.last_data_time) / 1000.0;
									break;
								case ANT_C:
									dt = (now - cad.last_data_time) / 1000.0;
									break;
								case ANT_SC:
									dt = (now - sc.last_data_time) / 1000.0;
									break;
								default:
									dt = -1.0;
									break;
							}

							if (do_ant_logging)  {
								sprintf(pstr, "   EVENT_RX_FAIL_GO_TO_SEARCH  %10ld, dt since last_data_time = %.2f seconds\n", timeGetTime(), dt);
								ant_log(pstr);
							}
							break;
						}

						case EVENT_CHANNEL_COLLISION:  {
							if (do_ant_logging)  {
								if (asi)  {
									sprintf(pstr, "Channel Collision, channel %d\n", chan);
									ant_log(pstr);
								}
							}
							break;
						}
						case EVENT_TRANSFER_TX_START:  {
							if (do_ant_logging)  {
								sprintf(pstr, "Burst Started\n");
								ant_log(pstr);
							}
							break;
						}
						case EVENT_SERIAL_QUE_OVERFLOW:  {
							if (do_ant_logging)  {
								//if (ant_stick_initialized)  {
								if (asi)  {
									sprintf(pstr, "serial queue overflow\n");
									ant_log(pstr);
								}
							}
							break;
						}

						default:  {													// see antdefines.h
							if (do_ant_logging)  {
								sprintf(pstr, "Unhandled channel event: 0x%X\n", antmsg.aucData[2]);
								ant_log(pstr);
							}
							break;
						}
					}						// switch(antmsg.aucData[2])  {
					break;
				}

				default:  {
					if (do_ant_logging)  {
						sprintf(pstr, "Unhandled response 0%d to message 0x%X\n", antmsg.aucData[2], antmsg.aucData[1]);
						ant_log(pstr);
					}
					break;
				}
			}								// switch(msg)  {
			break;
		}									// case MESG_RESPONSE_EVENT_ID

		case MESG_STARTUP_MESG_ID:  {
			if (do_ant_logging)  {
				sprintf(pstr, "	RESET Complete, reason: ");
				ant_log(pstr);
			}

			UCHAR ucReason = antmsg.aucData[0];

			if(ucReason == RESET_POR)  {
				if (do_ant_logging)  {
					sprintf(pstr, "RESET_POR");
					ant_log(pstr);
				}
			}

			if(ucReason & RESET_SUSPEND)  {
				if (do_ant_logging)  {
					sprintf(pstr, "RESET_SUSPEND ");
					ant_log(pstr);
				}
			}

			if(ucReason & RESET_SYNC)  {
				if (do_ant_logging)  {
					sprintf(pstr, "RESET_SYNC ");
					ant_log(pstr);
				}
			}

			if(ucReason & RESET_CMD)  {
				if (do_ant_logging)  {
					sprintf(pstr, "RESET_CMD ");
					ant_log(pstr);
				}
			}

			if(ucReason & RESET_WDT)  {
				if (do_ant_logging)  {
					sprintf(pstr, "RESET_WDT ");
					ant_log(pstr);
				}
			}

			if(ucReason & RESET_RST)  {
				if (do_ant_logging)  {
					sprintf(pstr, "RESET_RST ");
					ant_log(pstr);
				}
			}

			if (do_ant_logging)  {
				sprintf(pstr, "\n");
				ant_log(pstr);
			}

			break;
		}

		case MESG_CAPABILITIES_ID:  {
			if (do_ant_logging)  {
				sprintf(pstr, "	CAPABILITIES:\n");
				ant_log(pstr);

				sprintf(pstr, "		Max ANT Channels: %d\n",antmsg.aucData[0]);
				ant_log(pstr);

				sprintf(pstr, "		Max ANT Networks: %d\n",antmsg.aucData[1]);
				ant_log(pstr);
			}

			UCHAR ucStandardOptions = antmsg.aucData[2];
			UCHAR ucAdvanced = antmsg.aucData[3];
			UCHAR ucAdvanced2 = antmsg.aucData[4];

			if (do_ant_logging)  {
				sprintf(pstr, "	Standard Options:\n");
				ant_log(pstr);
			}

			if( ucStandardOptions & CAPABILITIES_NO_RX_CHANNELS )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NO_RX_CHANNELS\n");
					ant_log(pstr);
				}
			}

			if( ucStandardOptions & CAPABILITIES_NO_TX_CHANNELS )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NO_TX_CHANNELS\n");
					ant_log(pstr);
				}
			}

			if( ucStandardOptions & CAPABILITIES_NO_RX_MESSAGES )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NO_RX_MESSAGES\n");
					ant_log(pstr);
				}
			}

			if( ucStandardOptions & CAPABILITIES_NO_TX_MESSAGES )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NO_TX_MESSAGES\n");
					ant_log(pstr);
				}
			}

			if( ucStandardOptions & CAPABILITIES_NO_ACKD_MESSAGES )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NO_ACKD_MESSAGES\n");
					ant_log(pstr);
				}
			}

			if( ucStandardOptions & CAPABILITIES_NO_BURST_TRANSFER )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NO_BURST_TRANSFER\n");
					ant_log(pstr);
				}
			}

			if (do_ant_logging)  {
				sprintf(pstr, "	Advanced Options:\n");
				ant_log(pstr);
			}

			if( ucAdvanced & CAPABILITIES_OVERUN_UNDERRUN )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_OVERUN_UNDERRUN\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_NETWORK_ENABLED )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_NETWORK_ENABLED\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_AP1_VERSION_2 )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_AP1_VERSION_2\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_SERIAL_NUMBER_ENABLED )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_SERIAL_NUMBER_ENABLED\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_PER_CHANNEL_TX_POWER_ENABLED\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_LOW_PRIORITY_SEARCH_ENABLED\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_SCRIPT_ENABLED )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_SCRIPT_ENABLED\n");
					ant_log(pstr);
				}
			}

			if( ucAdvanced & CAPABILITIES_SEARCH_LIST_ENABLED )  {
				if (do_ant_logging)  {
					sprintf(pstr, "		CAPABILITIES_SEARCH_LIST_ENABLED\n");
					ant_log(pstr);
				}
			}


			if(usSize_ > 4)  {
				if (do_ant_logging)  {
					sprintf(pstr, "	Advanced 2 Options 1:\n");
					ant_log(pstr);
				}

				if( ucAdvanced2 & CAPABILITIES_LED_ENABLED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_LED_ENABLED\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_EXT_MESSAGE_ENABLED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_EXT_MESSAGE_ENABLED\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_SCAN_MODE_ENABLED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_SCAN_MODE_ENABLED\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_RESERVED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_RESERVED\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_PROX_SEARCH_ENABLED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_PROX_SEARCH_ENABLED\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_EXT_ASSIGN_ENABLED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_EXT_ASSIGN_ENABLED\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_FS_ANTFS_ENABLED)  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_FREE_1\n");
						ant_log(pstr);
					}
				}

				if( ucAdvanced2 & CAPABILITIES_FIT1_ENABLED )  {
					if (do_ant_logging)  {
						sprintf(pstr, "		CAPABILITIES_FIT1_ENABLED\n");
						ant_log(pstr);
					}
				}
			}

			got_caps = true;

			break;
		}

		case MESG_CHANNEL_STATUS_ID:  {
			UCHAR chan = antmsg.aucData[0];
			UCHAR channel_status;

			channel_status = antmsg.aucData[1] & STATUS_CHANNEL_STATE_MASK;		// MUST MASK OFF THE RESERVED BITS

			switch(chan)  {
				case ANT_HR:
					hr.channel_status = channel_status;
					break;
				case ANT_C:
					cad.channel_status = channel_status;
					break;
				case ANT_SC:
					sc.channel_status = channel_status;
					break;
				default:
					ant_bp = 0;
					break;
			}


			// 0x52
			// 0x01 0x01 0x07 0xba 0x36 0x00
			/*
				antmsg.aucData[0]					channel number

				antmsg.aucData[1]					channel status
															Bits 4:7
																Channel type (invalid on AP1 devices) Refer to Table 5-1 (page 15)
																	0x00		Bidirectional Slave Channel
																	0x10		Bidirectional Master Channel
																	0x20		Shared Bidirectional Slave Channel
																	0x40		Slave Receive Only Channel
															Bits 2:3
																Network number (invalid on AP1 devices) Refer to section 5.2.5.1 (page 19)

															Bits 0:1
																Channel State:
																	Un-Assigned = 0
																	Assigned = 1
																	Searching = 2
																	Tracking = 3
			*/

			if (do_ant_logging)  {

#ifdef _DEBUG
				UCHAR channel_type, netnum;
				channel_type = antmsg.aucData[1] & 0x0f0;
				channel_type >>= 4;
				netnum = antmsg.aucData[1] & 0x0c;
				channel_type >>= 2;
#endif
				sprintf(pstr, "	%10ld Got Status, channel %d, status = ", timeGetTime(), chan);

				switch(channel_status)  {
					case STATUS_UNASSIGNED_CHANNEL:
						strcat(pstr, "STATUS_UNASSIGNED_CHANNEL\n");
						break;
					case STATUS_SEARCHING_CHANNEL:
						strcat(pstr, "STATUS_SEARCHING_CHANNEL\n");
						break;

					case STATUS_ASSIGNED_CHANNEL:
						strcat(pstr, "STATUS_ASSIGNED_CHANNEL *******\n");					// gets this message as soon as sensor moves, before light flashes
						break;
					case STATUS_TRACKING_CHANNEL:  {
						strcat(pstr, "STATUS_TRACKING_CHANNEL <<<<<<<<<<\n");
						break;
					}
					default:
						ant_bp = 2;
						break;
				}

				ant_log(pstr);
			}											// if (do_ant_logging)  {
	
			got_status = true;

			break;
		}

		case MESG_CHANNEL_ID_ID:  {
			// Channel ID of the device that we just recieved a message from.
			devnum = antmsg.aucData[1] | (antmsg.aucData[2] << 8);

			if (do_ant_logging)  {
				sprintf(pstr, "	%10ld CHANNEL ID: (devnum = %d, dev_type = %d, trans_type = %d)\n\n", timeGetTime(), devnum, antmsg.aucData[3], antmsg.aucData[4]);
				ant_log(pstr, true);
			}
			got_id = true;
			break;
		}

		case MESG_VERSION_ID:  {
			if (do_ant_logging)  {
				sprintf(pstr, "VERSION: %s\n", (char*) &antmsg.aucData[0]);
				ant_log(pstr);
			}
			break;
		}


		case MESG_ACKNOWLEDGED_DATA_ID:			// 0x4f
			#ifdef _DEBUG
				ant_bp = 1;
			#endif
		case MESG_BURST_DATA_ID:				// 0x50
			#ifdef _DEBUG
				ant_bp = 2;
			#endif

		case MESG_BROADCAST_DATA_ID:  {			// 0x4e, 78
			int chan = antmsg.aucData[0];
			//xxx

			switch(chan)  {
				case ANT_HR:
					hr.last_data_time = timeGetTime();
					break;
				case ANT_C:
					cad.last_data_time = timeGetTime();
					break;
				case ANT_SC:
					sc.last_data_time = timeGetTime();
					break;
				default:
					break;
			}

			if (do_ant_logging)  {
				if (asi)  {
					if (log_raw)  {
						DWORD now = timeGetTime();
						double dt;

						dt = (now - start_time) / 1000.0;

						switch(chan)  {
							case ANT_HR:
								sprintf(pstr, "\n   %.2lf seconds MESG_BROADCAST_DATA_ID, usize = %d, chan = %d, state = %s\n", dt, usSize_, chan, channel_status_string[hr.channel_status]);
								break;
							case ANT_C:
								sprintf(pstr, "\n   %.2lf MESG_BROADCAST_DATA_ID, usize = %d, chan = %d, state = %s\n", dt, usSize_, chan, channel_status_string[cad.channel_status]);
								break;
							case ANT_SC:
								sprintf(pstr, "\n   %.2lf MESG_BROADCAST_DATA_ID, usize = %d, chan = %d, state = %s\n", dt, usSize_, chan, channel_status_string[sc.channel_status]);
								break;
							default:
								break;
						}
						//sprintf(pstr, "\nMESG_BROADCAST_DATA_ID, usize = %d, chan = %d\n", usSize_, chan);
						//ant_log(pstr);
					}
				}
			}
#if 1
			//static UCHAR ucOldPage;
			//UCHAR ucPage = pucEventBuffer[BUFFER_INDEX_MESG_DATA];

			//static UCHAR ucState = HRM_INIT_PAGE;	// sets the state of the receiver - INIT, STD_PAGE, EXT_PAGE
			//if (ucState == HRM_INIT_PAGE)  {
			//	ucState = HRM_STD_PAGE;				// change the state to STD_PAGE and allow the checking of old and new pages
			//}
#endif

			//------------------------------------------------------------------------------
			// The flagged and unflagged data messages have the same
			// message ID. Therefore, we need to check the size to
			// verify of a flag is present at the end of a message.
			// To enable flagged messages, must call ANT_RxExtMesgsEnable first.
			//------------------------------------------------------------------------------

			if(usSize_ > MESG_DATA_SIZE)  {					// MESG_DATA_SIZE = 9
				UCHAR ucFlag = antmsg.aucData[9];

				if(ant_display && ucFlag & ANT_EXT_MESG_BITFIELD_DEVICE_ID)  {		// ANT_EXT_MESG_BITFIELD_DEVICE_ID = 0x80
					// Channel ID of the device that we just recieved a message from.
					USHORT usDeviceNumber = antmsg.aucData[10] | (antmsg.aucData[11] << 8);
					UCHAR ucDeviceType =  antmsg.aucData[12];
					UCHAR ucTransmissionType = antmsg.aucData[13];

					if (do_ant_logging)  {
						if (asi)  {
							if (log_raw)  {
								char str[128];
								sprintf(str, "   Extended!, devnum = %d, devtype = %d, transmission type = %d", usDeviceNumber, ucDeviceType, ucTransmissionType);
								strcat(pstr, str);
							}
						}
					}
				}
			}
#ifdef _DEBUG
			else  {
				ant_bp = usSize_;
				strcat(pstr, "   Regular");
				ant_bp = 0;
			}
#endif

			if (asi)  {
				if (do_ant_logging)  {
					if (log_raw)  {
						strcat(pstr, "\n");
						ant_log(pstr);
					}
				}
			}

			// Display recieved message
			ant_bprintBuffer = TRUE;
			ucDataOffset = 1;   // For most data messages

			if(ant_display)  {
				if(antmsg.ucMessageID == MESG_ACKNOWLEDGED_DATA_ID )  {
					if (do_ant_logging)  {
						sprintf(pstr, "   Acked Rx:(%d): ", antmsg.aucData[0]);
						ant_log(pstr);
					}
				}
				else if(antmsg.ucMessageID == MESG_BURST_DATA_ID)  {
					if (do_ant_logging)  {
						//if (ant_stick_initialized)  {
						if (asi)  {
							sprintf(pstr, "   Burst(0x%02x) Rx:(%d): ", ((antmsg.aucData[0] & 0xE0) >> 5), antmsg.aucData[0] & 0x1F );
							ant_log(pstr);
						}
					}
				}
				else if (antmsg.ucMessageID == MESG_BROADCAST_DATA_ID)  {			// 0x4e, 78
					if (do_ant_logging)  {
						//if (ant_stick_initialized)  {
						if (asi)  {
							if (log_raw)  {
								sprintf(pstr, "   Rx:(%d)    ", antmsg.aucData[0]);
								ant_log(pstr);
							}
						}
					}
				}
				else  {
					ant_bp = 8;
				}
			}

			// rx fail
			//if (ant_stick_initialized)  {
			if (asi)  {
				int chan = antmsg.aucData[0];						// 1 for heartrate, 0 for speed/cadence

				memcpy(ant_packet, &antmsg.aucData[ucDataOffset], 8);

				switch(chan)  {
					case ANT_HR:
						hr.decode();
						break;
					case ANT_C:
						cad.decode();				// 80 ff ff ff 00 00 00 00
						break;
					case ANT_SC:
						sc.decode();
						break;
					default:
						ant_bp = 0;
						break;
				}
			}

			break;
		}				// MESG_ACKNOWLEDGED_DATA_ID, MESG_BURST_DATA_ID, MESG_BROADCAST_DATA_ID


		case MESG_EXT_BROADCAST_DATA_ID:
		case MESG_EXT_ACKNOWLEDGED_DATA_ID:
		case MESG_EXT_BURST_DATA_ID:
		{

			//----------------------------------------------------------------------------------
			// The "extended" part of this message is the 4-byte channel
			// id of the device that we recieved this message from. This message
			// is only available on the AT3. The AP2 uses flagged versions of the
			// data messages as shown above.
			//----------------------------------------------------------------------------------

			// Channel ID of the device that we just recieved a message from

			USHORT usDeviceNumber = antmsg.aucData[1] | (antmsg.aucData[2] << 8);
			UCHAR ucDeviceType =  antmsg.aucData[3];
			UCHAR ucTransmissionType = antmsg.aucData[4];

			ant_bprintBuffer = TRUE;
			ucDataOffset = 5;   // For most data messages

			if(ant_display)  {
				// Display the channel id
				if (do_ant_logging)  {
					sprintf(pstr, "Chan ID(%d/%d/%d)\n", usDeviceNumber, ucDeviceType, ucTransmissionType );
					ant_log(pstr);
				}

				if(antmsg.ucMessageID == MESG_EXT_ACKNOWLEDGED_DATA_ID)  {
					if (do_ant_logging)  {
						sprintf(pstr, "   Acked Rx:(%d)\n", antmsg.aucData[0]);
						ant_log(pstr);
					}
				}
				else if(antmsg.ucMessageID == MESG_EXT_BURST_DATA_ID)  {
					if (do_ant_logging)  {
						//if (ant_stick_initialized)  {
						if (asi)  {
							sprintf(pstr, "Burst(0x%02x) Rx:(%d)\n", ((antmsg.aucData[0] & 0xE0) >> 5), antmsg.aucData[0] & 0x1F );
							ant_log(pstr);
						}
					}
				}
				else  {
					if (do_ant_logging)  {
						//if (ant_stick_initialized)  {
						if (asi)  {
							sprintf(pstr, "Rx:(%d)\n", antmsg.aucData[0]);
							ant_log(pstr);
						}
					}
				}
			}

			break;
		}				// MESG_EXT_BROADCAST_DATA_ID, MESG_EXT_ACKNOWLEDGED_DATA_ID, MESG_EXT_BURST_DATA_ID

		default:  {
			ant_bp = 1;
			break;
		}
	}								// switch(antmsg.ucMessageID)  {




	if(ant_bprintBuffer)  {
		if(ant_display)  {

#if 0
			//sprintf(pstr, "%5.1f %5.1f\n", cadence, wheel_rpm);
			sprintf(pstr, "%5.1f %5.1f\n", f1, f2);
#else
			if (do_ant_logging)  {
				//if (ant_stick_initialized)  {
				if (asi)  {
					if (log_raw)  {
						sprintf(pstr, "[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x]\n",
							antmsg.aucData[ucDataOffset + 0],
							antmsg.aucData[ucDataOffset + 1],
							antmsg.aucData[ucDataOffset + 2],
							antmsg.aucData[ucDataOffset + 3],
							antmsg.aucData[ucDataOffset + 4],
							antmsg.aucData[ucDataOffset + 5],
							antmsg.aucData[ucDataOffset + 6],
							antmsg.aucData[ucDataOffset + 7]);
					}
#endif

					ant_log(pstr);
				}
			}

		}
		else  {
			if (do_ant_logging)  {
				static int iIndex = 0;
				static char ac[] = {'|','/','-','\\'};
				sprintf(pstr, "Rx: %c\n",ac[iIndex++]); fflush(stdout);
				ant_log(pstr);
				iIndex &= 3;
			}
		}
	}						// if(ant_bprintBuffer)  {

	inprocess = false;

	return;
}										// ANT::process()

/********************************************************************************************************

********************************************************************************************************/

void ant_log(char *str, bool flush)  {

#ifdef _DEBUG
	//printf("%s", str);
#endif

	if (!ant_logstream)  {
		return;
	}

	fprintf(ant_logstream, "%s", str);

	fflush(ant_logstream);

	//if (flush)  {
		//fclose(ant_logstream);
		//ant_logstream = fopen(ant_logname, "a+t");
	//}
	return;
}							// ant_log
/********************************************************************************************************

********************************************************************************************************/

bool ant_WaitAck(UCHAR _msg, unsigned long _ms)  {

	bool b = false;
	unsigned long start = GetTickCount();

	// 0x4b = MESG_OPEN_CHANNEL_ID
	// 0x46 = MESG_NETWORK_KEY_ID

#ifdef _DEBUG
	if (_msg==MESG_OPEN_CHANNEL_ID || _msg==MESG_NETWORK_KEY_ID)  {						// 0x4b, 0x46
		static int calls = 0;
		calls++;
		if (calls==2)  {
			ant_bp = 1;
		}
	}
#endif

		//do  {
	while(!b)  {
		if(_msg == ant_msg)  {
			if(ant_ack == RESPONSE_NO_ERROR)  {
				if (!inprocess)  {
					b = true;
				}
			}
			else  {
				ant_bp = 2;
				break;
			}
			if ((GetTickCount()-start) >= _ms)  {
				b = true;
			}
		}
	//} while((b==false) && ((GetTickCount()-start) < _ms) && !inprocess );
	//} while((b==false) && ((GetTickCount()-start) < _ms) );
	}


	ant_ack = 0;
	ant_msg = 0;

	return b;
}

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

int SC::decode()  {
	event_diff = 0;
	time_diff = 0;

	if (!initialized)  {
		return 0;
	}

	decoding_data = true;
	//does_exist = true;
	decode_time = timeGetTime();

	// cadence:

	cad_time = ant_packet[0];
	cad_time += ant_packet[1] <<8;

	cad_event_count = ant_packet[2];
	cad_event_count += ant_packet[3] <<8;

	// speed:

	speed_time = ant_packet[4];
	speed_time += ant_packet[5] <<8;

	speed_event_count = ant_packet[6];
	speed_event_count += ant_packet[7] <<8;

	if(cad_state == BSC_INIT)  {
		cad_previous_time = cad_time;
		cad_previous_event_count = cad_event_count;

		speed_previous_time = speed_time;
		speed_previous_event_count = speed_event_count;

		cad_state = BSC_ACTIVE;
	}

	//-----------------------------------------------------
	// update cadence calculations on new cadence event
	// this is the large bump on the sensor body
	//-----------------------------------------------------

	if(cad_event_count != cad_previous_event_count)  {
		ucNoCadEventCount = 0;
		cad_coasting = false;

		// update cumulative event count

		if(cad_event_count > cad_previous_event_count)  {
			event_diff = cad_event_count - cad_previous_event_count;
		}
		else  {
			event_diff = (USHORT) (0xFFFF - cad_previous_event_count + cad_event_count + 1);
		}
		cad_acum_event_count += event_diff;

		// update cumulative time (1/1024s)

		if(cad_time > cad_previous_time)  {
			time_diff = cad_time - cad_previous_time;
		}
		else  {
			time_diff = (USHORT) (0xFFFF - cad_previous_time + cad_time + 1);
		}
		cad_acum_time += time_diff;

		// calculate cadence (rpm)

		if(time_diff > 0)  {
			cadence = (float) ( ((ULONG) event_diff * 0xF000) / (ULONG) time_diff );	// 1 min = 0xF000 = 60 * 1024
#ifdef _DEBUG
//#if 0
			if (channel_status == STATUS_TRACKING_CHANNEL)  {
				if (cadence < .01f)  {
					ant_bp = 3;
				}
			}
#endif
		}
	}
	else  {
		ucNoCadEventCount++;

		if(ucNoCadEventCount >= MAX_NO_EVENTS)  {
			cad_coasting = true;									// coasting
			cadence = 0.0f;
		}
	}

	//-----------------------------------------------------
	// Update speed calculations on new speed event
	// this is the head of the giraffe
	//-----------------------------------------------------

	if(speed_event_count != speed_previous_event_count)  {
		ucNoSpdEventCount = 0;
		cad_stopping = FALSE;
		// Update cumulative event count
		if(speed_event_count > speed_previous_event_count)  {
			event_diff = speed_event_count - speed_previous_event_count;
		}
		else  {
			event_diff = (USHORT) (0xFFFF - speed_previous_event_count + speed_event_count + 1);
		}
		speed_acum_event_count += event_diff;

		// Update cumulative time (1/1024s)

		if(speed_time > speed_previous_time)  {
			time_diff = speed_time - speed_previous_time;
		}
		else  {
			time_diff = (USHORT) (0xFFFF - speed_previous_time + speed_time + 1);
		}

		speed_acum_time += time_diff;


		wheel_rpm = ((float) event_diff / (float) time_diff) * 60.0f * 1024.0f;

		if(circumference_cm)  {
#ifdef _DEBUG
			//float diameter = (float) (circumference_cm / PI);
#endif
			// Calculate speed (meters/h)
			speed = (float) (circumference_cm * 0x9000 * (ULONG) event_diff) / (ULONG) time_diff;	// 1024 * 36 = 0x9000
		}

		// Calculate distance (cm)

		cad_distance = (ULONG) circumference_cm * speed_acum_event_count;
	}
	else  {
		ucNoSpdEventCount++;
		if(ucNoSpdEventCount >= MAX_NO_EVENTS)  {
			cad_stopping = true;
			wheel_rpm = 0.0f;
			speed = 0.0f;
		}
	}

	// Update previous values

	cad_previous_time = cad_time;
	cad_previous_event_count = cad_event_count;

	speed_previous_time = speed_time;
	speed_previous_event_count = speed_event_count;

	return 0;
}					// SC::decode()

/**************************************************************************************
	ant_packet contains the payload
**************************************************************************************/

//int PAGE_SENSOR::decode()  {
int HR::decode()  {
	if (!initialized)  {
		return 0;
	}

	decoding_data = true;

	UCHAR l_page_num = 0;
	UCHAR l_ucEventDiff = 0;
	USHORT l_usTimeDiff1024 = 0;

	/*
	UCHAR ucPageNum = 0;
	UCHAR ucEventDiff = 0;
	USHORT usTimeDiff1024 = 0;
	*/

	decode_time = timeGetTime();
	//does_exist = true;

#ifdef _DEBUG
	hrcalls++;

	bool have_ant_packet;
	if (ant_packet[6] != 0xcc && ant_packet[7] != 0xcc)  {				// wait for valid ant_packet
		//hr = ant_packet[7];
		have_ant_packet = true;
	}
	else  {
		have_ant_packet = false;
	}
#endif

	// heartrate
	// monitor page toggle bit

	if(page != EXT_PAGE)  {
		if(page == INIT_PAGE)  { 
			page = (ant_packet[0] & TOGGLE_MASK) >>7;
			// initialize previous values to correctly get the cumulative values
			previous_event_count = ant_packet[6];
			previous_time_1024 = ant_packet[4];
			previous_time_1024 += ant_packet[5]<<8;
		}
		else if(page != ((ant_packet[0] & TOGGLE_MASK) >>7))  { 
			// first page toggle was seen, enable advanced data
			page = EXT_PAGE;
		}
	}
	
	// remove page toggle bit

	l_page_num = ant_packet[0] & ~TOGGLE_MASK;

	// Handle basic data
	// heartrate

	event_count = ant_packet[6];
	time_1024 = ant_packet[4];
	time_1024 += ant_packet[5]<<8;
	//if (hrcalls>15)  {
	//if (ant_packet[6] != 0xcc && ant_packet[7] != 0xcc)  {				// wait for valid ant_packet
#ifdef _DEBUG
	if (have_ant_packet)  {
		val = ant_packet[7];
	}

//	if (hr > 10)  {
//		ant_bp = 2;
//	}
#else
	val = ant_packet[7];
#endif

	accum += val;
	accum_count++;

	// heartrate
	// handle background data, if supported

	if(page == EXT_PAGE)  {
		switch(l_page_num)  {
			case PAGE1:
				elapsed_time2 = ant_packet[1];
				elapsed_time2 += ant_packet[2] << 8;
				elapsed_time2 += ant_packet[3] << 16;
				break;
			case PAGE2:
				mfgr_id = ant_packet[1];
				sernum = ant_packet[2];
				sernum += ant_packet[3] << 8;		// serial number = 3?
				break;
			case PAGE3:
				hw_version = ant_packet[1];			// 4
				sw_version = ant_packet[2];			// 20
				model = ant_packet[3];					// 7
				break;
			case PAGE4:
				previous_time_1024 = ant_packet[2];
				previous_time_1024 += ant_packet[3] << 8;
				break;
			default:
#ifdef _DEBUG
				ant_bp = 0;
#endif
				break;
		}
	}

	// heartrate


	// only need to do calculations if dealing with a new event

	if(event_count != previous_event_count)  {
		no_event_count = 0;

		// update cumulative event count

		if(event_count > previous_event_count)  {
			l_ucEventDiff = event_count - previous_event_count;
		}
		else  {
			l_ucEventDiff = (UCHAR) 0xFF - previous_event_count + event_count + 1;
		}

		acum_event_count += l_ucEventDiff;

		// update cumulative time

		if(time_1024 > previous_time_1024)  {
			l_usTimeDiff1024 = time_1024 - previous_time_1024;
		}
		else  {
			l_usTimeDiff1024 = (USHORT) (0xFFFF - previous_time_1024 + time_1024 + 1);
		}

		// heartrate
		ulAcumTime1024 += l_usTimeDiff1024;


		// calculate R-R Interval

		if(l_ucEventDiff == 1)  {
			rr_interval_1024 = l_usTimeDiff1024;
		}

		// calculate heart rate (from timing data), in ant_bpm

		if(l_usTimeDiff1024 > 0)  {
			calculated_val = (UCHAR) ( ((USHORT) l_ucEventDiff * 0xF000) / l_usTimeDiff1024 );	// 1 min = 0xF000 = 60 * 1024

#ifdef _DEBUG
			if (val > 30 && val < 100)  {
				ant_bp = 2;
			}
#endif

		}
	}
	// heartrate
	else  {
		no_event_count++;
		if(no_event_count >= MAX_NO_EVENTS)  {
			val = calculated_val = INVALID_PAGE;	// No new valid HR data has been received
		}
	}

	if (calculated_val != INVALID_PAGE)  {
		int diff = (int)abs(calculated_val - val);
		if (diff > 5)  {
			// maybe average hr with the calculated hr? What to do?
			// observed calculated_hr's after things were up and running:
			// 26, 27, 28
			// so you'd need a median filter if you use that.
			ant_bp = 7;
		}
	}

	// update previous time and event count

	if(l_page_num != PAGE4)  {
		previous_time_1024 = time_1024;  // only if not previously obtained from HRM message
	}

	previous_event_count = event_count;
	// heartrate

#ifdef _DEBUG
	if (val > 10)  {
		ant_bp = 2;
	}
#endif

	return 0;
}						// int HR::decode()


/**************************************************************************************

**************************************************************************************/

void ant_set_circumference(float _mm)  {
	//SC *sc = (SC *)&ant_sensors[ANT_SC];
	sc.set_circum((UCHAR)ROUND(_mm / 10.0f));

	//ant_sensors[ANT_SC].set_circum((UCHAR)ROUND(_mm / 10.0f));

	return;
}


/**************************************************************************************

**************************************************************************************/

int CAD::decode(void)  {
	UCHAR l_page_num = 0;
	UCHAR l_ucEventDiff = 0;
	USHORT l_usTimeDiff1024 = 0;

	decode_time = timeGetTime();
	decoding_data = true;

	// monitor page toggle bit
	// CAD

	if(page != EXT_PAGE)  {										// EXT_PAGE = 3
		if(page == INIT_PAGE)  {								// INIT_PAGE = 2
			page = (ant_packet[0] &TOGGLE_MASK) >>7;
			// initialize previous values to correctly get the cumulative values
			previous_time_1024 = ant_packet[4];
			previous_time_1024 += ant_packet[5]<<8;

			previous_event_count = ant_packet[6];
			previous_event_count += ant_packet[7]<<8;

		}
		else if(page != ((ant_packet[0] & TOGGLE_MASK) >>7))  { 
			// first page toggle was seen, enable advanced data
			page = EXT_PAGE;
		}
	}
	

	// Handle basic data

	//event_count = ant_packet[6];
	event_count = ant_packet[6];
	event_count += ant_packet[7]<<8;

	time_1024 = ant_packet[4];
	time_1024 += ant_packet[5]<<8;

	// CAD
	//val = ant_packet[7];
	
	l_page_num = ant_packet[0] & ~TOGGLE_MASK;			// remove page toggle bit

	// handle background data, if supported

	if(page == EXT_PAGE)  {										// EXT_PAGE = 3
	// CAD
	switch(l_page_num)  {
			case 0:
				ant_bp = 2;
				break;
			case PAGE1:												// Cumulative Operating Time
				elapsed_time2 = ant_packet[1];
				elapsed_time2 += ant_packet[2] << 8;
				elapsed_time2 += ant_packet[3] << 16;		// 39019, 39067
				break;
			case PAGE2:
				mfgr_id = ant_packet[1];			// 13
				sernum = ant_packet[2];
				sernum += ant_packet[3] << 8;		// serial number = 26626
				break;
			case PAGE3:
				hw_version = ant_packet[1];			// 255
				sw_version = ant_packet[2];			// 5
				model = ant_packet[3];					// 3
				break;
			case PAGE4:
				// this page is reserved
				//previous_time_1024 = ant_packet[2];
				//previous_time_1024 += ant_packet[3] << 8;
				break;
			default:
				ant_bp = 1;
				break;
		}
	}


	// only need to do calculations if dealing with a new event
	// CAD

	if(event_count != previous_event_count)  {
		no_event_count = 0;

		// update cumulative event count

		if(event_count > previous_event_count)  {
			l_ucEventDiff = event_count - previous_event_count;
		}
		else  {
			l_ucEventDiff = (UCHAR) 0xFF - previous_event_count + event_count + 1;
		}

		acum_event_count += l_ucEventDiff;

		// update cumulative time

		if(time_1024 > previous_time_1024)  {
			l_usTimeDiff1024 = time_1024 - previous_time_1024;
		}
		else  {
			l_usTimeDiff1024 = (USHORT) (0xFFFF - previous_time_1024 + time_1024 + 1);
		}

		ulAcumTime1024 += l_usTimeDiff1024;


		// calculate R-R Interval
		// CAD

		//if(l_ucEventDiff == 1)  {
		//	rr_interval_1024 = l_usTimeDiff1024;
		//}

		// calculate cadence (from timing data)

		if(l_usTimeDiff1024 > 0)  {
			val = (UCHAR) ( ((USHORT) l_ucEventDiff * 0xF000) / l_usTimeDiff1024 );				// 1 min = 0xF000 = 60 * 1024 
		}
	}
	else  {
		no_event_count++;
		if(no_event_count >= MAX_NO_EVENTS)  {
			val = INVALID_PAGE;			// No new valid cadence data has been received
		}
	}

	// update previous time and event count

	if(l_page_num != PAGE4)  {
		previous_time_1024 = time_1024;  // only if not previously obtained from HRM message
	}

	// CAD
	previous_event_count = event_count;

#ifdef _DEBUG
	if (val > 10)  {
		ant_bp = 5;
	}
#endif

	return 0;
}					// CAD::decode()

/**************************************************************************************

**************************************************************************************/

int PAGE_SENSOR::reset(void)  {
	ANT::reset();

	page = PAGE0;

	//val = 0x00;
	//calculated_val = 0x00;

	ulAcumTime1024 = 0L;
	decode_time = 0L;
	previous_event_count = 0;
	previous_time_1024 = 0;
	time_1024 = 0;
	event_count = 0;
	elapsed_time2 = 0L;
	mfgr_id = 0;
	sernum = 0;
	hw_version = 0;
	sw_version = 0;
	model = 0;
	no_event_count = 0;
	acum_event_count = 0;
	rr_interval_1024 = 0;

	return 0;
}													// PAGE_SENSOR::reset()

/**************************************************************************************

**************************************************************************************/

int CAD::init(void)  {
	reset();									// CAD::reset()
	return 0;
}												// CAD::init()


/**************************************************************************************

**************************************************************************************/

int HR::reset(void)  {
	PAGE_SENSOR::reset();
	chan_num = ANT_HR;
	return 0;
}														// HR::reset()

/**************************************************************************************

**************************************************************************************/

int CAD::reset(void)  {
	PAGE_SENSOR::reset();
	chan_num = ANT_C;
	return 0;
}												// CAD::reset()

/**************************************************************************************

**************************************************************************************/

int SC::reset(void)  {

	ANT::reset();

	chan_num = ANT_SC;
	event_diff = 0;
	time_diff = 0;

	cadence = 0.0f;
	circumference_cm = 0;
	wheel_rpm = 0.0f;
	speed = 0.0f;
	speed_event_count = 0;
	speed_previous_event_count = 0;
	speed_time = 0;
	speed_previous_time = 0;
	speed_acum_event_count = 0L;
	speed_acum_time = 0L;
	cad_event_count = 0;
	cad_previous_event_count = 0;
	cad_time = 0;
	cad_previous_time = 0;
	cad_acum_event_count = 0L;
	cad_acum_time = 0L;
	cad_distance = 0L;
	cad_state = BSC_INIT;
	cad_coasting = false;
	cad_stopping = false;
	ucNoSpdEventCount = 0;
	ucNoCadEventCount = 0;
	cadence	= 0.0f;
	wheel_rpm = 0.0f;
	speed = 0.0f;
	circumference_cm = 0;

	return 0;
}											// SC::reset()

/**************************************************************************************

**************************************************************************************/

void SC::set_circum(UCHAR _cm)  {

	circumference_cm = _cm;

	return;
}

/**************************************************************************************

**************************************************************************************/

ANT::ANT(void)  {
	int status;
	status = init();				// ANT::init()
	return;
}										// ANT::ANT()

/**************************************************************************************

**************************************************************************************/

PAGE_SENSOR::PAGE_SENSOR(void) : ANT()  {
#ifdef _DEBUG
	bp = 1;
#endif
	init();							// PAGE_SENSOR::init()
	return;
}										// ANT::ANT()

/**************************************************************************************

**************************************************************************************/

CAD::CAD(void) : PAGE_SENSOR()  {
	#ifdef _DEBUG
		bp = 2;
	#endif
	init();							// CAD::init()
	return;
}										// ANT::ANT()

/**************************************************************************************

**************************************************************************************/

int ANT::init(void)  {

	last_open_attempt_time = 0L;
	last_status_request_time = 0L;
	channel_open_time = 0L;
	last_data_time = 0L;

	dev_type = ANT_INVALID_SENSOR;
	dev_type = 0;				// 151

	initialized = false;
	decoding_data = false;
	//does_exist = false;

	// unsigned char:
	chan_num = 0;				// 0
	freq = 0;						// 57
	trans_type = 0;				// 0
	chan_type = 0;				// CHANNEL_TYPE_SLAVE, Channel type as chosen by user (master or slave), 0 = slave?
	page = 0;
	previous_event_count = 0;
	event_count = 0;
	mfgr_id = 0;					// Manufacturing ID
	hw_version = 0;				// Hardware version
	sw_version = 0;				// Software version
	model = 0;				   // Model number
	no_event_count = 0;		// Successive transmissions with no new cadence events
	channel_status = STATUS_UNASSIGNED_CHANNEL;

	// shorts:
	period = 0;					// 8086
	previous_time_1024 = 0;
	time_1024 = 0;
	sernum = 0;					// Serial number
	rr_interval_1024 = 0;	// R-R interval (1/1024 seconds), conversion to ms is performed for data display

	// longs
	elapsed_time2 = 0L;			// Cumulative operating time (elapsed time since battery insertion) in 2 second resolution	
	acum_event_count = 0L;		// Cumulative heart beat event count
	lastnow = 0L;

	memset(name, 0, sizeof(name));

#ifdef _DEBUG
		bp = 0;
#endif

	//initialized = true;

	return 0;
}														// ANT::init(void)

/**************************************************************************************

**************************************************************************************/

int ANT::reset(void)  {
	dev_type = ANT_INVALID_SENSOR;

	// unsigned char:
	chan_num = 0;				// 0
	freq = 0;						// 57
	trans_type = 0;				// 0
	chan_type = 0;				// CHANNEL_TYPE_SLAVE, Channel type as chosen by user (master or slave), 0 = slave?
	page = 0;
	previous_event_count = 0;
	event_count = 0;
	mfgr_id = 0;					// Manufacturing ID
	hw_version = 0;				// Hardware version
	sw_version = 0;				// Software version
	model = 0;				   // Model number
	no_event_count = 0;		// Successive transmissions with no new cadence events
	channel_status = 0;

	// shorts:
	period = 0;					// 8086
	previous_time_1024 = 0;
	time_1024 = 0;
	sernum = 0;					// Serial number
	rr_interval_1024 = 0;	// R-R interval (1/1024 seconds), conversion to ms is performed for data display

	// longs
	accum_count = 0L;
	elapsed_time2 = 0L;			// Cumulative operating time (elapsed time since battery insertion) in 2 second resolution	
	acum_event_count = 0L;		// Cumulative heart beat event count
	decode_time = 0L;

	memset(name, 0, sizeof(name));

	// floats
	accum = -1.0f;

#ifdef _DEBUG
		bp = 0;
#endif

	return 0;
}														// ANT::reset(void)

/**************************************************************************************

**************************************************************************************/

int PAGE_SENSOR::init(void)  {
	reset();
	return 0;
}

#if 0

/**************************************************************************************

**************************************************************************************/

float PAGE_SENSOR::get_val(void)  {
	if (!decoding_data)  {
		val = 0;
		accum = 0.0f;
		accum_count = 0L;
		return -1.0f;
	}

	if (accum_count==0)  {
		return val;
	}

	float f = accum / accum_count;

	//accum = 0.0f;
	//accum_count = 0L;

#ifdef _DEBUG
	/*
	static bool flag = false;
	if (f > 10.0f)  {
		flag = true;
		ant_bp = 2;
	}
	else  {
		if (flag)  {
			ant_bp = 4;
		}
	}
	*/
#endif

	//return accum_val;
	return f;
}															// HR::get_accum_val()
#endif

/**************************************************************************************

**************************************************************************************/

float CAD::get_val(void)  {

	if (!decoding_data)  {
		return -1.0f;
	}

	if (accum_count==0)  {
		return val;
	}

	float f = accum / accum_count;

	return f;
}															// CAD::get_val()

/**************************************************************************************

**************************************************************************************/

float HR::get_val(void)  {
	if (!decoding_data)  {
		//val = 0;
		//accum = 0.0f;
		//accum_count = 0L;
		return -1.0f;
	}

	if (accum_count==0)  {
		return val;
	}

	float f = accum / accum_count;

	//accum = 0.0f;
	//accum_count = 0L;

#ifdef _DEBUG
	/*
	static bool flag = false;
	if (f > 10.0f)  {
		flag = true;
		ant_bp = 2;
	}
	else  {
		if (flag)  {
			ant_bp = 4;
		}
	}
	*/
#endif

	//return accum_val;
	return f;
}															// HR::get_val()

/**************************************************************************************

**************************************************************************************/
/*
float PAGE_SENSOR::get_val(void)  {
	if (!decoding_data)  {
		return -1.0f;
	}
	return val;
}															// HR::get_val()
*/

/**************************************************************************************

**************************************************************************************/

#if 0
int PAGE_SENSOR::reset(void)  {
	page = 0;
	previous_event_count = 0;
	event_count = 0;
	mfgr_id = 0;							// Manufacturing ID
	hw_version = 0;						// Hardware version
	sw_version = 0;						// Software version
	model = 0;							// Model number
	no_event_count = 0;				// Successive transmissions with no new cadence events

	previous_time_1024 = 0;
	time_1024 = 0;
	sernum = 0;							// Serial number
	rr_interval_1024 = 0;			// R-R interval (1/1024 seconds), conversion to ms is performed for data display

	ulAcumTime1024 = 0L;				// Cumulative time (1/1024 seconds), conversion to s is performed for data display
	decode_time = 0L;
	elapsed_time2 = 0L;					// Cumulative operating time (elapsed time since battery insertion) in 2 second resolution	
	acum_event_count = 0L;				// Cumulative heart beat event count

	does_exist = false;

	return 0;
}													// PAGE_SENSOR::init()

#endif

#endif			// #ifdef DO_ANT
