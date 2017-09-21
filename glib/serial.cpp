
#ifndef WIN32
	#include <linux_serial.cpp>
#else
////////////////////////////// WINDOWS VERION //////////////////////////////////

#define STRICT
#define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4996)					// for vista strncpy_s
#pragma warning(disable:4290)				// throw() ignored


/**********************************************************************
 start of serial.cpp
**********************************************************************/

#pragma hdrstop
#include <windows.h>

#ifdef _DEBUG
	#include <windowsx.h>
	#include <crtdbg.h>
#endif

#include <stdio.h>

#include <process.h>

#include <mmsystem.h>

#include <glib_defines.h>
#include <serial.h>
#include <err.h>
#include <conio.h>
//#include <minmax.h>

//OVERLAPPED gOverLapped= {0};

#ifdef _DEBUG
	//extern bool oktobreak;
#endif

/**********************************************************************
	Constructor 1
	this constructor doesn't ope the port (for old pc1 compatibility)
**********************************************************************/

Serial::Serial(void)  {

	constructor = 1;
	bluetooth_delay = true;
	tx_thread_running = false;
	memset(&gOverLapped, 0, sizeof(OVERLAPPED));

	logg = NULL;
	strcpy(parity, "N");
	databits = 8;
	stopbits = 1;
	shutDownDelay = 1000;
	//rx_thread_running = false;

#ifdef _DEBUG
	sprintf(str, "Com constructor 1\n" );
	OutputDebugString(str);
	st1 = NULL;
#endif

#ifdef _DEBUG
	rawstream = NULL;
	memset(rawstr, 0, 16);
	//#pragma message ("********************************  LOGGING RAW SERIAL DATA!!!!!!!!!")
	//rawstream = fopen(RAWNAME, "wb");
#endif

	txasleep = FALSE;
	txdiff = 0;
	rxdiff = 0;
	maxtxdiff = 0;
	maxrxdiff = 0;
	szPort[0] = 0;
	comopen = FALSE;
	bytes_in = 0;
	//bytes_out = 0;

	hCommPort = INVALID_HANDLE_VALUE;
	displayErrors = TRUE;

	//Open(pszPort, pszBaudRate, 0, displayErrors);

}


/**********************************************************************
	Constructor 2
**********************************************************************/

Serial::Serial(const char *pszPort,	const char *pszBaudRate, const char *_parity, int _databits, int _stopbits, BOOL _displayErrors, Logger *_logg) throw(int) {
	int status;
#ifdef _DEBUG
	HANDLE procHeap;
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif

	bluetooth_delay = true;
	tx_thread_running = false;
	//logg = NULL;
	memset(&gOverLapped, 0, sizeof(OVERLAPPED));
	logg = _logg;
	if (logg) logg->write(10,0,1,"\n%s\n", pszPort);
	strncpy(parity, _parity, sizeof(parity)-1);
	databits = _databits;
	stopbits = _stopbits;
	shutDownDelay = 1000;
	constructor = 2;

	//rx_thread_running = false;

#ifdef _DEBUG
	rawstream = NULL;
	memset(rawstr, 0, 16);
	//#pragma message ("********************************  LOGGING RAW SERIAL DATA!!!!!!!!!")
	//rawstream = fopen(RAWNAME, "wb");
#endif

#ifdef _DEBUG
	comnum = atoi(&pszPort[3]);
	sprintf(str, "Com%d constructor 2\n", comnum);
	OutputDebugString(str);
	if (!strcmp(pszPort, "COM11"))  {
		int bp = 1;
	}
	st1 = NULL;
#endif

	bytes_in = 0;
	//bytes_out = 0;
	txasleep = FALSE;
	txdiff = 0;
	rxdiff = 0;
	maxtxdiff = 0;
	maxrxdiff = 0;
	szPort[0] = 0;
	comopen = FALSE;


	hCommPort = INVALID_HANDLE_VALUE;

	//displayErrors = TRUE;
	displayErrors = _displayErrors;

	if (logg) logg->write(10,0,1,"   ok1\n");
	status = Open(pszPort, pszBaudRate, displayErrors);
	if (logg) logg->write(10,0,1,"   ok2, status = %d\n", status);

#ifdef _DEBUG
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif

	/*
ok1
ok2
ok3

COM1
   ok1
   CreateFile error 2
   ok2

	*/


	if (status)  {
		destroy();
		throw(status);
	}

	fThreadDone = FALSE;
	fWaitingOnRead = FALSE;
	fWaitingOnStat = FALSE;
	writefile_error_count = 0L;
	hresult = 0;
	id = 0;

#ifdef _DEBUG
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
	int bp = 0;
#endif


}

/**********************************************************************
	Constructor 3
**********************************************************************/

Serial::Serial(bool _bluetooth_delay, const char *pszPort,	const char *pszBaudRate, BOOL _displayErrors, Logger *_logg) throw(int) {
	int status;

	bluetooth_delay = _bluetooth_delay;
	tx_thread_running = false;
	logg = _logg;
	//rx_thread_running = false;
	memset(&gOverLapped, 0, sizeof(OVERLAPPED));
	strncpy(portname, pszPort, sizeof(portname)-1);

	constructor = 3;

	strcpy(parity, "N");
	databits = 8;
	stopbits = 1;
	shutDownDelay = 1000;

#ifdef _DEBUG
	comnum = atoi(&pszPort[3]);
	sprintf(str, "Com%d constructor 3\n", comnum);
	OutputDebugString(str);
	st1 = NULL;
	rawstream = NULL;
	memset(rawstr, 0, 16);

#ifdef LOGRAW
	#pragma message ("********************************  CONSTRUCTOR 3  LOGGING RAW SERIAL DATA!!!!!!!!!")
	if (strcmp(pszBaudRate, "2400")==0)  {
		//char rawstr[16];
		sprintf(rawstr, "3%d.dat", comnum);
		rawstream = fopen(rawstr, "wb");
	}
#endif

#endif



	bytes_in = 0;
	//bytes_out = 0;
	txasleep = FALSE;
	txdiff = 0;
	rxdiff = 0;
	maxtxdiff = 0;
	maxrxdiff = 0;
	szPort[0] = 0;
	comopen = FALSE;

	displayErrors = _displayErrors;
	hCommPort = INVALID_HANDLE_VALUE;

	status = Open(pszPort, pszBaudRate, displayErrors);						// has a 500 ms delay for bluetooth serial ports
	if (status)  {
		shutDownDelay = 0;			// no need to delay if port doesn't exist
		destroy();
		throw(status);
	}

}										// constructor 3


/**********************************************************************
	Constructor 4
	'_dummy' is to just get a unique constructor
**********************************************************************/


Serial::Serial(const char *pszPort,	const char *pszBaudRate, Logger *_logg) throw(int) {
//Serial::Serial(char *pszPort,	char *pszBaudRate, Logger *_logg, bool _displayErrors, char *_dummy) throw(int) {
	int status;

#ifdef _DEBUG
	HANDLE procHeap;
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif

	bluetooth_delay = true;
	tx_thread_running = false;
	logg = _logg;
	//displayErrors = _displayErrors;
	//rx_thread_running = false;
	memset(&gOverLapped, 0, sizeof(OVERLAPPED));

	constructor = 4;

#ifdef _DEBUG
	st1 = new fastTimer("st1");
#endif

	strcpy(parity, "N");
	databits = 8;
	stopbits = 1;
	shutDownDelay = 1000;

#ifdef _DEBUG
	comnum = atoi(&pszPort[3]);
	sprintf(str, "Com%d constructor 4\n", comnum);
	OutputDebugString(str);
	rawstream = NULL;
	memset(rawstr, 0, 16);

#ifdef LOGRAW
	#pragma message ("********************************  CONSTRUCTOR 4  LOGGING RAW SERIAL DATA!!!!!!!!!")
	if (strcmp(pszBaudRate, "2400")==0)  {
		//char rawstr[16];
		sprintf(rawstr, "4%d.dat", comnum);
		rawstream = fopen(rawstr, "wb");
	}
#endif

#endif


	bytes_in = 0;
	//bytes_out = 0;
	txasleep = FALSE;
	txdiff = 0;
	rxdiff = 0;
	maxtxdiff = 0;
	maxrxdiff = 0;
	szPort[0] = 0;
	comopen = FALSE;
	hCommPort = INVALID_HANDLE_VALUE;



#ifdef _DEBUG
//	st1->start();
#endif

	displayErrors = FALSE;
#ifdef _DEBUG
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif
	status = Open(pszPort, pszBaudRate, displayErrors);				// this takes 1.84 seconds !!!!!!!!!!!!
	if (status)  {
		destroy();
		throw(status);
	}

#ifdef _DEBUG
//	st1->stop();
//	double d = st1->getSeconds();			// 1.500 seconds to here
#endif

#ifdef _DEBUG
	_ASSERTE(_CrtCheckMemory());
	procHeap = GetProcessHeap();
	HeapValidate(procHeap, 0, 0);
#endif

}							// constructor 4

/**********************************************************************
	~Serial() - Destructor
**********************************************************************/

Serial::~Serial()  {

	destroy();
}

/*********************************************************************
	eg, Open("COM2","2400",1,1);

  returns:

	0 if ok
	2 = "the system cannot find the specified file"
	5 = "access is denied"
	99 = buildcommDCB error
	100 = setcommstate error
	101 = createEvent error
	102 = createEvent error 2

#define SERIAL_ERROR_DOES_NOT_EXIST 1
#define SERIAL_ERROR_ACCESS_DENIED	2
#define SERIAL_ERROR_BUILDCOMMDCB	3
#define SERIAL_ERROR_SETCOMMSTATE	4
#define SERIAL_ERROR_CREATEEVENT1	5
#define SERIAL_ERROR_CREATEEVENT2	6

*********************************************************************/

int Serial::Open(								// tlm20020130, changed from BOOL
	const char *pszPort,
	const char *pszBaudRate,
	BOOL bDisplayErrors)   {

	char szPortInit[32];
	int istatus;
	DCB dcb;
	BOOL bstatus;

	comnum = atoi(&pszPort[3]);

	if (comnum >= 10)  {
		strcpy(portstr, "\\\\.\\");
		strcat(portstr, pszPort);
	}
	else  {
		strncpy(portstr, pszPort, sizeof(portstr)-1);
	}

	txdiff = 0;
	rxdiff = 0;
	txinptr = 0;
	txoutptr = 0;
	maxin = 0;				// max number of bytes read by readfile
	rxinptr = 0;
	rxoutptr = 0;

	#ifdef _DEBUG
//	if (logg) logg->write(5,1,0,"\nOpen(%s,%s,, %d)\n",portstr,pszBaudRate, (int)bDisplayErrors);
	#endif

	strcpy(szPort, portstr);
	//strcpy(szPortInit, pszBaudRate);
	//strcat(szPortInit, ",N,8,1");
	sprintf(szPortInit, "%s,%s,%d,%d", pszBaudRate, parity, databits, stopbits);

	COMMTIMEOUTS CommTimeOuts = {0};

	memset(&gOverLapped, 0, sizeof(gOverLapped));

	gOverLapped.Offset = 0;
	gOverLapped.OffsetHigh = 0;
	gOverLapped.hEvent = NULL;

	/*
		bluetooth serial adapter note:
		this call doesn't fail at 2400 baud, but does at 38400 baud. But if I break hear and then continue
		there is no error at 38400 baud. So, I'm trying a short delay here.
	*/

	if (bluetooth_delay)  {
		Sleep(500);			// this seems to work for the iogear gbs301
	}


	hCommPort = CreateFile(
						szPort,
						GENERIC_READ | GENERIC_WRITE,
						0,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
						NULL
					);											// .42 seconds


	if(hCommPort == INVALID_HANDLE_VALUE)  {
		Err *err = new Err();													// "element not found"
		DEL(err);

		// 5 = "access is denied"
		// 2 = "the system cannot find the specified file"
		// 1168 = "element not found"  (bluetooth adapter only)

		istatus = GetLastError();												// 1168

		if (logg) logg->write(10,0,1,"   CreateFile error %d\n",istatus);
		if(bDisplayErrors)  {
			DisplayOpenError(istatus);
		}
		if (istatus==5)  {
			return SERIAL_ERROR_ACCESS_DENIED;
		}
		else if (istatus==2)  {
			return SERIAL_ERROR_DOES_NOT_EXIST;
		}
		else if (istatus==1168)  {
			return SERIAL_ERROR_OTHER;
		}
		else if (istatus==121)  {										// the semaphore timeout period has expired
			return SERIAL_ERROR_OTHER;
		}
		else  {
			return SERIAL_ERROR_OTHER;
		}

	}
//xxx
	// setup device buffers
//#ifdef _DEBUG
//	if (st1)  {
//		st1->start();
//	}
//#endif

	bstatus = SetupComm(
					hCommPort,
					1024,						// size of INPUT queue
					1024);					// size of OUTPUT queue

//#ifdef _DEBUG
//	if (st1)  {
//		st1->stop();
//		//	double d = st1->getSeconds();			// 1.500 seconds to here
//	}
//#endif

	if (bstatus==FALSE)  {
		if (logg) logg->write(10,0,1,"setupcomm error\n");
	}

	// SET UP FOR OVERLAPPED NON-BLOCKING I/O

	memset(&CommTimeOuts, 0, sizeof(CommTimeOuts));

//	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;

	CommTimeOuts.ReadTotalTimeoutMultiplier = 5;			// 0
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;		// 0

	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 2000;		// 5000

	bstatus = SetCommTimeouts(hCommPort,&CommTimeOuts);
	if (bstatus==FALSE)  {
		if (logg) logg->write(10,0,1,"setcommtimeouts error\n");
	}

	memset( &dcb, 0, sizeof(DCB));

	dcb.DCBlength = sizeof(DCB);           // sizeof(DCB) 

	//-----------------------------------------------------------
	// setup the following with BuildCommDCB:
	//
	//	DWORD BaudRate;            // current baud rate 
	//	DWORD fParity: 1;          // enable parity checking 
	//	BYTE ByteSize;             // number of bits/byte, 4-8 
	//	BYTE Parity;               // 0-4=no,odd,even,mark,space 
	//	BYTE StopBits;             // 0,1,2 = 1, 1.5, 2
	//
	//-----------------------------------------------------------

 	istatus = BuildCommDCB(szPortInit,&dcb);

	if (istatus == 0)  {				// tlm20011009

		CloseHandle(hCommPort);
		hCommPort = INVALID_HANDLE_VALUE;

		if(bDisplayErrors)  {
			DisplayOpenError(istatus);
		}
		if (logg) logg->write(10,0,1,"   buildcommdcb error = %d\n",istatus);
		return SERIAL_ERROR_BUILDCOMMDCB;
	}


// set desired comm paramaters

	dcb.fBinary = TRUE;							// binary mode, no EOF check 	
	dcb.fNull = 0;          					// enable null stripping 


//	bXonXoff = FALSE;

//	if(bXonXoff == TRUE)
//		dcb.fOutX = TRUE;   						// turns on XON/XOFF for transmitted data
//	else
	dcb.fOutX = FALSE;


	dcb.fInX = FALSE;             			// XON/XOFF in flow control 
	dcb.fOutxDsrFlow = FALSE;     			// DSR output flow control 
	dcb.XonChar = XON;            			// Tx and Rx XON character 
	dcb.XoffChar = XOFF;          			// Tx and Rx XOFF character 
	dcb.fOutxCtsFlow = FALSE;     			// CTS output flow control 
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// RTS flow control 
	dcb.fDtrControl = DTR_CONTROL_ENABLE;	// DTR flow control type 

	/************

	these have not been set:
	-------------------------

	DWORD fDsrSensitivity:1;   // DSR sensitivity 
	DWORD fTXContinueOnXoff:1; // XOFF continues Tx 
	DWORD fErrorChar: 1;       // enable error replacement 
	DWORD fAbortOnError:1;     // abort reads/writes on error 
	DWORD fDummy2:17;          // reserved 
	WORD wReserved;            // not currently used 
	WORD XonLim;               // transmit XON threshold 
	WORD XoffLim;              // transmit XOFF threshold 
	char ErrorChar;            // error replacement character 
	char EofChar;              // end of input character 
	char EvtChar;              // received event character 
	WORD wReserved1;           // reserved; do not use 

	************/

	istatus = SetCommState(hCommPort, &dcb);

	if (istatus == 0) {			// tlm20011009

		CloseHandle(hCommPort);
		hCommPort = INVALID_HANDLE_VALUE;
		if(bDisplayErrors)  {
			DisplayOpenError(istatus);
		}

		if (logg) logg->write(10,0,1,"   setcommstate error = %d\n",istatus);
		//return FALSE;
		return SERIAL_ERROR_SETCOMMSTATE;
	}

	FlushBuffers();

	comopen = TRUE;
	//delay(100);
	Sleep(100);
	if (logg) logg->write(10,0,1,"Open(): returning TRUE\n");


	txcontin = TRUE;

	txControlEvent = CreateEvent(
								NULL,						// no security attributes
								TRUE,						// create manual reset event
								FALSE,					// initial state is not signaled
								NULL);					// no event name

	if (txControlEvent == NULL)  {
		if (logg) logg->write(10,0,1,"error: couldn't create txControlEvent\n");
		//return FALSE;
		return SERIAL_ERROR_CREATEEVENT1;
	}
	else  {
		#ifdef _DEBUG
		if (logg) logg->write(5,0,0,"created txControlEvent\n");
		#endif
	}



	rxThreadExitEvent = CreateEvent(
								NULL,						// no security attributes
								TRUE,						// create manual reset event
								FALSE,					// initial state is not signaled
								NULL);					// no event name

	if (rxThreadExitEvent == NULL)  {
		if (logg) logg->write(10,0,1,"error: couldn't create rxExitlEvent\n");
		return SERIAL_ERROR_CREATEEVENT2;
	}
	else  {
		#ifdef _DEBUG
		if (logg) logg->write(5,0,0,"created rxThreadExitEvent\n");
		#endif
	}

#ifdef _DEBUG
	txthrd = (HANDLE) _beginthread(Serial::txThread, 20000, this);
	rxthrd = (HANDLE) _beginthread(Serial::rxThread, 32768, this);
#else
	txthrd = (HANDLE) _beginthread(Serial::txThread, 20000, this);
	rxthrd = (HANDLE) _beginthread(Serial::rxThread, 32768, this);
#endif

	//Sleep(1000);
	Sleep(10);
	return 0;
}




/**********************************************************************

**********************************************************************/


void Serial::Close(int code) {

	if (logg) logg->write(10,0,1,"   starting close()\n");
#ifdef _DEBUG
	OutputDebugString("close 1\n");
#endif

	if (code==0)  {
		if (logg) logg->write(10,0,1,"\nserial port destructor closing serial port\n");
	}
	else  {
		if (logg) logg->write(10,0,1,"%ld: closing serial port, code = %d\n",timeGetTime(), code);
	}

	if (logg) logg->write(10,0,1,"serial port max in = %d\n", maxin);


#ifdef _DEBUG
	OutputDebugString("close 2\n");
#endif
	if (logg) logg->write(10,0,1,"close 2\n");

	if(hCommPort != INVALID_HANDLE_VALUE)  {
		#ifdef _DEBUG
			OutputDebugString("close 3 (valid handle)\n");
		#endif
//xxx

		BOOL b;
		DWORD flags = 0L;

		b = GetHandleInformation(txControlEvent, &flags);

		if (b==FALSE)  {
			Err *err = new Err();
			if (logg) logg->write(10,0,1,"GetHandleInformation() failed: %s\n", err->getString());
			DEL(err);
		}
		else  {
			if (logg) logg->write(10,0,1,"GetHandleInformation() OK, flags = %08lx\n", flags);
		}

		if (logg) logg->write(10,0,1,"setting txControlEvent, shutdowndelay = %ld\n", shutDownDelay);
		txcontin = FALSE;
		SetEvent(txControlEvent);		// wake up thread

		if (logg) logg->write(10,0,1,"back from setevent()\n");

		int count = 0;

		while(tx_thread_running)  {
			count++;
			/*
			if (count1000000)  {
				if (logg) logg->write(10,0,1,"count = 1,000,000\n");
				break;
			}
			*/

			b = GetHandleInformation(txControlEvent, &flags);
			if (b==FALSE)  {
				Err *err = new Err();
				if (logg) logg->write(10,0,1,"GetHandleInformation() failed: %s\n", err->getString());
				DEL(err);
				break;
			}
		}

		if (logg) logg->write(10,0,1,"setting rxThreadExitEvent\n");
		SetEvent(rxThreadExitEvent);

		//while(rx_thread_running)  {
		//}
		if (logg) logg->write(10,0,1,"back from SetEvent(rxThreadExitEvent), shutdowndelay = %ld\n", shutDownDelay);
		
		count = 0;
		while(rxThreadExitEvent)  {
			count++;
			if (count==10)  {
				if (logg) logg->write(10,0,1,"count = 10\n");
			}
		}

		if (logg) logg->write(10,0,1,"count = %d\n", count);

		//delay(1000);						// give threads a little time to react
		Sleep(shutDownDelay);
		#ifdef _DEBUG
			OutputDebugString("close 4\n");
		#endif

		if (logg) logg->write(10,0,1,"   closing handle\n");

		b = CloseHandle(hCommPort);
		if (!b)  {
			//int bp = 1;
			#ifdef _DEBUG
				OutputDebugString("close 5 (****** ERROR ******)\n");
			#endif
			if (logg) logg->write(10,0,1,"close 5 (****** ERROR ******)\n");
		}
		hCommPort = INVALID_HANDLE_VALUE;
	}

	comopen = FALSE;

#ifdef _DEBUG
	OutputDebugString("close 6\n");
#endif
	if (logg) logg->write(10,0,1,"   finished close()\n");

	return;

}

/**********************************************************************

**********************************************************************/

void Serial::DisplayOpenError(int iStatus)  {
	char szError[128];

	int iRtnVal = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, iStatus,
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				szError, sizeof(szError), NULL);

	if(iRtnVal == 0)
		strcpy(szError,"Unknown error.");

	MessageBox(GetActiveWindow(),szError,"Serial Port Error", MB_OK | MB_ICONEXCLAMATION);


}

/**********************************************************************

**********************************************************************/

unsigned int Serial::GetCommError() {
	unsigned int uiErrorFlags;

	COMSTAT ComStat;

	::ClearCommError(hCommPort, (DWORD*)&uiErrorFlags, &ComStat);
	return uiErrorFlags;
}

/**********************************************************************

**********************************************************************/

void Serial::GetCommErrorStr(
	unsigned int uiCommStatus,
	char *pszErrStr,
	int iMaxStr) {


	*pszErrStr = 0;

	if((uiCommStatus & CE_BREAK   ) != 0)
		strncpy(pszErrStr, "Break", iMaxStr);
	if((uiCommStatus & CE_DNS     ) != 0)
		strncpy(pszErrStr, "Device Not Selected", iMaxStr);
	if((uiCommStatus & CE_FRAME   ) != 0)
		strncpy(pszErrStr, "Frame Error", iMaxStr);
	if((uiCommStatus & CE_IOE     ) != 0)
		strncpy(pszErrStr, "I/O Error", iMaxStr);
	if((uiCommStatus & CE_MODE    ) != 0)
		strncpy(pszErrStr, "Mode Error", iMaxStr);
	if((uiCommStatus & CE_OOP     ) != 0)
		strncpy(pszErrStr, "OOP Error", iMaxStr);
	if((uiCommStatus & CE_OVERRUN ) != 0)
		strncpy(pszErrStr, "Data Overrun", iMaxStr);
	if((uiCommStatus & CE_PTO     ) != 0)
		strncpy(pszErrStr, "Parallel Timeout", iMaxStr);
	if((uiCommStatus & CE_RXOVER  ) != 0)
		strncpy(pszErrStr, "RX Overrun", iMaxStr);
	if((uiCommStatus & CE_RXPARITY) != 0)
		strncpy(pszErrStr, "RX Parity Error", iMaxStr);
	if((uiCommStatus & CE_TXFULL  ) != 0)
		strncpy(pszErrStr, "TX Full", iMaxStr);

}

/**********************************************************************

**********************************************************************/

unsigned int Serial::OutQueueLength() {
	COMSTAT sStatus;

	DWORD dwErrorFlags;
	ClearCommError(hCommPort, &dwErrorFlags, &sStatus);
	return sStatus.cbOutQue;
}


/**********************************************************************

	use the new threaded rx()
eg,
	serialport.Receive(buf,len,&recvd);

**********************************************************************/

/********************************
unsigned int Serial::Receive(
	unsigned char *puszBuffer,
	unsigned int uiLength,
	int *piBytesRcvd)  {

	int iCommBytesIn = 0;
	DWORD dwError;

// GET ANY CHARACTERS WHICH HAVE COME IN

	BOOL bResult;

	//if (hCommPort== INVALID_HANDLE_VALUE)  {
		//	logg->write(0,0,0,"hCommPort is invalid\n");
	//}

	bResult = ReadFile(
					hCommPort,
			   puszBuffer,
			   uiLength,						// number of bytes to read
					(DWORD*)&iCommBytesIn,		// number of bytes read
			   &gOverLapped					// address of overlapped structure for data
   			);

	if(!bResult)  {
   	dwError = GetLastError();
		static int first=10;

		if (first>0)  {
			first--;
		}

		if (first==0)  {
			first = -1;
			logg->write(10,0,0,"serial Receive() error = %lu, turned off further error printing\n",dwError);
			logg->write(10,0,0,"   len = %u, \n",uiLength);
			logg->write(10,0,0,"   bytes read = %d, \n",iCommBytesIn);
		}

   	switch(dwError)  {
	  	case ERROR_HANDLE_EOF:
		 	break;
		 case ERROR_IO_PENDING:				// asyn i/o is still in progress
		 	// do something else for a while
			//bResult = GetOverlappedResult(
			//				hCommPort,
			//			&gOverLapped,
			//			&nBytesRead,
			//			FALSE);
			//	if (!bResult)  {
			//	switch(dwError = GetLastError())  {
			// 	case ERROR_HANDLE_EOF:
			//	  	break;
			// }
			//}
		 	break;
	  }

		//	uiError = GetCommError();				// note: CE_RXOVER is 0x0001
		*piBytesRcvd = 0;
	  return 0;
   }
   else  {
		*piBytesRcvd = iCommBytesIn;
		return 0;
   }

}

**************************************************/




/**********************************************************************

**********************************************************************/

void Serial::Clear() {

// Try to clear the communications

	EscapeCommFunction(hCommPort, SETXON);
	FlushBuffers();
	GetCommError();
}

/**********************************************************************

**********************************************************************/

void Serial::GetPort(char *pszPort, int iMaxLen) {
	strncpy(pszPort, szPort, iMaxLen);
}

/**********************************************************************

**********************************************************************/

BOOL Serial::IsOpen() {

	return (hCommPort != INVALID_HANDLE_VALUE);
}

/**********************************************************************

**********************************************************************/

BOOL Serial::IsWaitingForXon() {
	COMSTAT Stat;

	DWORD dwErrorFlags;

	::ClearCommError(hCommPort, &dwErrorFlags, &Stat);
	return Stat.fXoffHold;
}







/**********************************************************************

**********************************************************************/

void Serial::flush(void)  {
	FlushBuffers();
	return;
}

/***********************************************************************
//=====================================================================
// serial2.h functions:
//=====================================================================

void serial_setup(void)  {
	BOOL bContinue = TRUE;
	char chChar;
	int nParity = NOPARITY;
	int nDataBits = EIGHTDATABITS;
	int nStopBits = ONESTOPBIT;


	CSerialPort COMPort("COM1");

	COMPort.SetParityDataStop(nParity, nDataBits, nStopBits);

	if(!COMPort.IsValid() || !COMPort.StartCommThread(CommReader))  {
		printf("Error %ld initializing COM port!", COMPort.GetLastError());
	}
	else  {
		do  {
			COMPort.WriteCommBlock(&chChar, 1);
		} while(bContinue);
	}

	return;
}
************************************/

/**********************************************************************
	This is the thread function that handles incoming data.  Unless
	otherwise specified, CSerialPort::StartCommThread() passes a pointer
	the the port object itself.
**********************************************************************/

/********************************
#define MAX_SIZE 1024

DWORD WINAPI CommReader(void *pvData)  {
	CSerialPort *COMPort = (CSerialPort *)pvData;
	DWORD dwEventMask;
	char  byBuffer[MAX_SIZE];
	int nSize;

	//----------------------------------------------------------------
	// Tell the port to notify us when a read event occurs
	// (EV_RXCHAR is enabled by default)
	//----------------------------------------------------------------

	if(!COMPort->WaitCommEvent())
		return 1L;      // Error

	//---------------------------------------------------------------
	// Wait for an empty Comm event that signals the COM port object
	// is shutting down
	//---------------------------------------------------------------

	do {

		//--------------------------------------------------------
		// Since this is a separate thread, we can pass TRUE so
		// that the function doesn't return until an event occurs
		//--------------------------------------------------------

		dwEventMask = COMPort->CheckForCommEvent(TRUE);

		if(dwEventMask & EV_RXCHAR)  {

			// Read as much as possible

			nSize = COMPort->ReadCommBlock(byBuffer, MAX_SIZE);

			if(nSize)  {
				//WriteConsole(hStdOut, byBuffer, nSize, &dwBytesWritten, NULL);
			}

			COMPort->WaitCommEvent();		// Set it up to wait for the next event
		}

	} while(dwEventMask);

	return 0L;
}

*************************/

/**********************************************************************

**********************************************************************/

void Serial::flush_tx(void) {
	if(hCommPort != NULL)  {
		PurgeComm(hCommPort, PURGE_TXABORT | PURGE_TXCLEAR);
	}

	memset(txq, 0, sizeof(txq));
	txoutptr = txinptr;

	return;
}


/**********************************************************************

**********************************************************************/

void Serial::FlushBuffers() {
	if(hCommPort != NULL)  {
		PurgeComm(hCommPort, PURGE_TXABORT | PURGE_TXCLEAR);
		PurgeComm(hCommPort, PURGE_RXABORT | PURGE_RXCLEAR);
	}

	memset(txq, 0, sizeof(txq));
	memset(rxq, 0, sizeof(rxq));
	txoutptr = txinptr;
	rxoutptr = rxinptr;

}


/**********************************************************************

**********************************************************************/

void Serial::flushrx(DWORD ms) {
	int total;

	if (ms > 0)  {
		DWORD then, now;
		int n;
		char buf[32];

		then = timeGetTime();
		total = 0;

		while(1)  {
			n = rx((char *)buf, sizeof(buf));
			if (n==0)  {
				now = timeGetTime();
				if ((now - then) > ms) {
					break;								// no comms for 1 sec
				}
				continue;
			}
			else  {
				total += n;
				then = timeGetTime();
			}
		}
	}

	if(hCommPort != NULL)  {
		PurgeComm(hCommPort, PURGE_RXABORT | PURGE_RXCLEAR);
	}
	rxoutptr = rxinptr;
}


/**********************************************************************

**********************************************************************/


void Serial::flushtx(void) {

	if(hCommPort != NULL)  {
		PurgeComm(hCommPort, PURGE_TXABORT | PURGE_TXCLEAR);
	}

	txoutptr = txinptr;				// catch up the queue
}

/**********************************************************************
	forces the transmit buffer to be sent
**********************************************************************/

void Serial::forcetx(void) {
	BOOL b;
	if (hCommPort)  {
		b = FlushFileBuffers(hCommPort);
	}
	return;
}

/***********************************************************************


***********************************************************************/

BOOL Serial::tx(unsigned char c)  {

//	unsigned short behind;
//	behind = (unsigned short) ((txinptr - txoutptr) & (TXQLEN-1));
//	if (behind > TXQLEN-2)  {
//		return FALSE;								// queue is too full
//	}

	txq[txinptr] = c;
	txinptr = (unsigned short) ((txinptr + 1) % TXQLEN);
	return TRUE;
}


/***********************************************************************


***********************************************************************/

void Serial::txx(unsigned char *b, int n)  {
	int i;

	for(i=0;i<n;i++)  {
		txq[txinptr] = b[i];
		txinptr++;
		if (txinptr==TXQLEN)  {
			txinptr = 0;
		}
	}

	SetEvent(txControlEvent);				// wake up transmitter thread

	return;
}


/**********************************************************************


**********************************************************************/

BOOL Serial::txi(unsigned char c)  {
	BOOL bstatus;


	while(!txasleep) {}							// wait for transmitter to sleep

	txoutptr = txinptr;							// flush the queue
	txq[txinptr] = c;								// insert the character
	txinptr = (unsigned short) ((txinptr+1) % TXQLEN);

	bstatus = SetEvent(txControlEvent);		// wake up transmitter thread
	if (!bstatus)  {
		if (logg) logg->write(10,0,1,"txi error = %ld\n", GetLastError());
		return FALSE;
	}

	while(txinptr!=txoutptr)  {
	}

	return TRUE;

}


/**********************************************************************
	watches for the txqxxptrs not equal to trigger transmitting as a
	separate thread.
**********************************************************************/


void Serial::txThread(void *tmp)  {
	OVERLAPPED olWrite = {0};
	BOOL bstat;
	DWORD dwRes;
	DWORD towrite = 1;
	DWORD written;
	DWORD err;

#ifdef _DEBUG
	static int calls = 0;
	calls++;
#endif

	//logg->write(10,0,0,"\nstarting transmitter thread\n");


	Serial *port = static_cast<Serial *>(tmp);

	//port->StackOverflow(0);

	port->bytes_sent=0;

	port->writeEvent = CreateEvent(				// done in thread
								NULL,						// no security attributes
								TRUE,						// create manual reset event
								FALSE,					// initial state is not signaled
								NULL);					// no event name

	if (port->writeEvent == NULL)  {
		if (port->logg) port->logg->write(10,0,1,"error: couldn't create writeEvent\n");
		_endthread();
		return;
	}

	olWrite.hEvent = port->writeEvent;

	port->txpending = port->txasleep = FALSE;

	port->tx_thread_running = true;


	while(port->txcontin)  {

		if (port->txinptr == port->txoutptr)  {

			port->txasleep = TRUE;
			dwRes = WaitForSingleObject(port->txControlEvent, INFINITE);
			port->txasleep = FALSE;

//xxx
			if (!port->txcontin)  {
				if (port->logg) port->logg->write(10,0,1,"\ntxthread awakened by Close(), exiting\n");
				goto txfin;
			}


			switch(dwRes)  {
				case WAIT_OBJECT_0:						// write event set
					bstat = ResetEvent(port->txControlEvent);

					if (!bstat)  {
						err = GetLastError();
						if (port->logg) port->logg->write(10,0,1,"ResetEvent error(2) %ld\n", err);
					}
					break;

				case WAIT_TIMEOUT:						// wait timed out
					if (port->logg) port->logg->write(10,0,1,"WAIT_TIMEOUT 2\n");
					break;

				case WAIT_ABANDONED:
					if (port->logg) port->logg->write(10,0,1,"wait_abandoned 2\n");
					break;

				default:
					if (port->logg) port->logg->write(10,0,1,"unknown error at %d in %s\n", __LINE__, __FILE__);
					break;
			}

		}

		else  {

			// boundschecker error here: "overlapped io operation is in progress"

			bstat = WriteFile(								// codeguard failure
						port->hCommPort,
						&port->txq[port->txoutptr],
						towrite,									// DWORD
						&written,								// LPDWORD
						&olWrite);

			if (bstat)  {										// success
/************
if (port->comnum==1)  {
	if (port->txq[port->txoutptr] & 0x80)  {
		logg->write(1,0,0,"\n%ld: %02x\n", timeGetTime(), port->txq[port->txoutptr]);
	}
	else  {
		logg->write(1,0,0,"%ld: %02x\n", timeGetTime(), port->txq[port->txoutptr]);
	}
}
************/
				port->txoutptr = (unsigned short) ((port->txoutptr+1) % TXQLEN);
				port->bytes_sent++;
				//port->txdiff = (port->txinptr - port->txoutptr)  &  (TXQLEN - 1);
				//port->maxtxdiff = MAX(port->maxtxdiff, port->txdiff);

			}
			else  {												// error
				err = GetLastError();

				if (err == ERROR_IO_PENDING)  {			// write is delayed
					port->txpending = TRUE;
					dwRes = WaitForSingleObject(port->writeEvent, INFINITE);
					port->txpending = FALSE;

					switch(dwRes)  {
						case WAIT_OBJECT_0:						// write event set
							bstat = GetOverlappedResult(
										port->hCommPort,
										&olWrite,
										&written,
										FALSE);

							if (!bstat)  {
								err = GetLastError();
								if (err==996)  {
									if (port->logg) port->logg->write(10,0,1,"GetOverlappedResult: ERROR_IO_INCOMPLETE\n");
								}
								else  {
									if (port->logg) port->logg->write(10,0,1,"GetOverlappedResult: %ld\n", err);
								}
							}
							else  {
/********
if (port->comnum==1)  {
	if (port->txq[port->txoutptr] & 0x80)  {
		logg->write(1,0,0,"\n%ld: %02x\n", timeGetTime(), port->txq[port->txoutptr]);
	}
	else  {
		logg->write(1,0,0,"%ld: %02x\n", timeGetTime(), port->txq[port->txoutptr]);
	}
}
*********/
								port->bytes_sent++;
								port->txoutptr = (unsigned short) ((port->txoutptr+1) % TXQLEN);
								//port->txdiff = (port->txinptr - port->txoutptr)  &  (TXQLEN - 1);
								//port->maxtxdiff = MAX(port->maxtxdiff, port->txdiff);
							}
							break;

						case WAIT_TIMEOUT:					// wait timed out
							if (port->logg) port->logg->write(10,0,1,"WAIT_TIMEOUT\n");
							break;

						case WAIT_ABANDONED:
							if (port->logg) port->logg->write(10,0,1,"wait_abandoned\n");
							break;

						default:
							if (port->logg) port->logg->write(10,0,1,"unknown error at %d in %s\n", __LINE__, __FILE__);
							break;
					}								// switch (dwRes)

					bstat = ResetEvent(port->writeEvent);
					if (!bstat)  {
						err = GetLastError();
						if (port->logg) port->logg->write(10,0,1,"ResetEvent error %ld\n", err);
					}
				}									// if (io_pending)

				else  {
					port->writefile_error_count++;

					if (port->writefile_error_count<10)  {
						if (port->logg) port->logg->write(10,0,1,"tx WriteFile error %ld\n", err);		// writefile failed, but it isn't delayed
					}
					else if (port->writefile_error_count==10)  {
						if (port->logg) port->logg->write(10,0,1,"turned off further WriteFile error logs\n");
					}
				}
			}										// if (!bstat)
		}											// if (port->txinptr != port->txoutptr)

	}												// while(1)


txfin:
	if (port->logg) port->logg->write(10,0,1,"txfin 1\n");

	#ifdef _DEBUG
	//logg->write(10,1,0,"sent %ld bytes\n", bytes_sent);
	//logg->write(10,0,1,"ending serial port txThread\n");
	#endif
 
	CloseHandle(port->writeEvent);
	if (port->logg) port->logg->write(10,0,1,"txfin 2\n");
	CloseHandle(port->txControlEvent);
	port->txControlEvent = NULL;

//xxx
	if (port->logg) port->logg->write(10,0,1,"txfin 3\n");

#ifdef _DEBUG
	if (calls==2)  {
		int bp = 1;
	}
#endif

	port->tx_thread_running = false;

#ifdef _DEBUG
	static int exits=0;
	exits++;
	if (exits==2)  {
		HANDLE procHeap;
		_ASSERTE(_CrtCheckMemory());
		procHeap = GetProcessHeap();
		HeapValidate(procHeap, 0, 0);
	}
#endif

	_endthread();

	if (port->logg) port->logg->write(10,0,1,"txfin x\n");

	return;
}										// txthread()

/**********************************************************************

**********************************************************************/

int Serial::rx(char *buf, int buflen)  {
	int i;

	i = 0;

	while(rxoutptr != rxinptr)  {
		if (i==buflen)  {
			//logg->write(10,0,0, "error in %s at %d: i = %d, c = %02x\n",
			// __FILE__, __LINE__, i, rxq[rxoutptr]);
			break;
		}

		buf[i] = rxq[rxoutptr];
		rxoutptr = (rxoutptr + 1) % RXQLEN;
		i++;

	}

//	if (i>maxin)  {
//		maxin = i;
//	}

	return i;

}


/**********************************************************************
	receiver thread.

**********************************************************************/


//#define RBUFLEN 512


void Serial::rxThread(void *tmp)  {
	BOOL bstat;
	OVERLAPPED	ol = {0};
	HANDLE   	hArray[2];
	DWORD   		flags;
	DWORD 		n;		          					// bytes actually read
	DWORD   		dwRes;           					// result from WaitForSingleObject
	DWORD error;
	COMMTIMEOUTS CommTimeouts;
	BOOL waiting;


	//rx_thread_running = true;

	Serial *port = static_cast<Serial *>(tmp);

	if (port->logg) port->logg->write(5,0,0,"\nstarting receiver thread\n");

	bstat = GetCommTimeouts(port->hCommPort, &CommTimeouts);
	if (!bstat)  {
		if (port->logg) port->logg->write(10,0,1,"GetCommTimeouts error\n");
	}
	else  {
		if (port->logg) port->logg->write(5,0,0,"ReadIntervalTimeout =			%ld\n", CommTimeouts.ReadIntervalTimeout); 
		if (port->logg) port->logg->write(5,0,0,"ReadTotalTimeoutMultiplier =	%ld\n", CommTimeouts.ReadTotalTimeoutMultiplier); 
		if (port->logg) port->logg->write(5,0,0,"ReadTotalTimeoutConstant =		%ld\n", CommTimeouts.ReadTotalTimeoutConstant); 
		if (port->logg) port->logg->write(5,0,0,"WriteTotalTimeoutMultiplier = %ld\n", CommTimeouts.WriteTotalTimeoutMultiplier); 
		if (port->logg) port->logg->write(5,0,0,"WriteTotalTimeoutConstant =	%ld\n", CommTimeouts.WriteTotalTimeoutConstant); 
 	}


	ol.hEvent = CreateEvent(
							NULL,					// no security attributes
							TRUE,					// create manual reset event
							FALSE,				// initial state is not signaled
							NULL);				// no event name


	if (ol.hEvent == NULL)  {
		if (port->logg) port->logg->write(10,0,1,"error: couldn't create ReadEvent\n");
		_endthread();
		return;
	}
	else  {
		#ifdef _DEBUG
		if (port->logg) port->logg->write(5,1,0,"Read event created\n");
		#endif
	}


	hArray[0] = ol.hEvent;
	hArray[1] = port->rxThreadExitEvent; 

	//port->eventflags = EV_BREAK | EV_RXCHAR;

	port->eventflags = EV_RXCHAR;


	if (!SetCommMask(port->hCommPort, port->eventflags))  {
		if (port->logg) port->logg->write(10,0,1,"SetCommMask error\n");
	}



	while(1)  {						// port->rxcontin)  {

		waiting = FALSE;

		bstat = WaitCommEvent(
					port->hCommPort,
					&flags,						// lpdword, event mask
					&ol);

		if (bstat)  {							// success
			port->process_events(flags);
		}
		else  {									// error
			error = GetLastError();
			int bp;

			if (error == ERROR_IO_PENDING)  {
				waiting = TRUE;
			}
			else  {
				if (port != NULL)  {
					bp = 1;
					if (port->logg)  {
						//tlm20051005+++
						// if logg is null here, sometimes Logger gets called anyway where 'this' is 0xdddddddd, so
						// I commented this log statement out.
						//port->logg->write(10,0,1,"WaitCommEvent error %ld\n", error);
						//tlm20051005---
					}
					else  {
						bp = 2;
					}
				}
				else  {
					bp = 3;
				}
			}
		}



		if (waiting)  {

			dwRes = WaitForMultipleObjects(
						2,
						hArray,
						FALSE,
						INFINITE);

			switch(dwRes)  {
				case WAIT_OBJECT_0:						// comm event set
					bstat = GetOverlappedResult(
								port->hCommPort,
								&ol,
								&n,							// number of bytes transferred
								FALSE);

					if (!bstat)  {
						error = GetLastError();
						if (port->logg) port->logg->write(10,0,1,"error: RX GetOverlappedResult: %ld\n", error);
					}
					else  {									// success
						port->process_events(flags);
					}

					bstat = ResetEvent(ol.hEvent);
					if (!bstat)  {
						error = GetLastError();
						if (port->logg) port->logg->write(10,0,1,"ResetEvent error %ld\n", error);
					}

					break;

				case WAIT_OBJECT_0 + 1:						// exit event
					if (port->logg) port->logg->write(5,1,1,"rxthread awakened by rxThreadExitEvent\n");
					goto rxfin;
					//break;

				case WAIT_TIMEOUT:
					if (port->logg) port->logg->write(10,0,1,"RX WAIT_TIMEOUT\n");
					break;

				case WAIT_ABANDONED:
					if (port->logg) port->logg->write(10,0,1,"rx wait_abandoned\n");
					break;

				case WAIT_FAILED:
					error = GetLastError();
					if (port->logg) port->logg->write(10,0,1, "error: WAIT_FAILED (error: %ld)\n", error);
					break;

				default:
					if (port->logg) port->logg->write(10,0,1,"unknown error at %d in %s\n", __LINE__, __FILE__);
					break;
			}								// switch (dwRes)


		}				// if (waiting)
	}

rxfin:

	CloseHandle(ol.hEvent);

	//#ifdef _DEBUG
	if (port->logg) port->logg->write(5,1,1,"ending serial port rxThread\n");
	//#endif


	//tlm20051005+++
	// when the creator exits, the handle is destroyed?
	if (port==NULL)  {
		//int bp = 1;
		if (port->logg) port->logg->write(5,1,1,"error: port is null\n");
	}
	else  {
		CloseHandle(port->rxThreadExitEvent);
		port->rxThreadExitEvent = 0;
	}
	//tlm20051005---

	//rx_thread_running = false;

	if (port->logg) port->logg->write(5,1,1,"calling _endthread()\n");
	_endthread();
	if (port->logg) port->logg->write(5,1,1,"back from _endthread()\n");								// should never get here

	return;

}

/**********************************************************************

**********************************************************************/


void Serial::process_events(DWORD flags)  {
	BOOL bstat;
	COMSTAT cs;
	DWORD n;
	DWORD dwError;
	BYTE buf[128];			// defaults to 0xcc's
	DWORD toRead;
	DWORD i;
#ifdef _DEBUG
	int bp = 0;
#endif


//	GetCommMask(hCommPort, &flags);				// tlm990602

//	if (flags & EV_BREAK)  {						// a break occurred
//		commBreakTime = timeGetTime();
//		commBreak = TRUE;
		//logg->write(10,0,0,"break\n");
//	}
//	else  {
//		commBreak = FALSE;
//	}

	if (flags & EV_RXCHAR)  {						// a character came in
		//logg->write(10,0,0,"char %lx hex\n", flags);

		ClearCommError(hCommPort, &dwError, &cs);

		//-------------------------------------------------------
		// now read the receiver until there are no more bytes.
		// We know that bytes are ready to read.
		// cs.cbInQue contains the number of unread bytes.
		//-------------------------------------------------------

		toRead = cs.cbInQue;


		if (toRead > maxin)  {
			maxin = toRead;
		}

		if (toRead > 128)  {
			toRead = 128;
		}

		bstat = ReadFile(
					hCommPort,
			   		buf,
			   		toRead,				// number of bytes to read
					&n,					// number of bytes read
			   		&gOverLapped		// address of overlapped structure for data
   				);

		if(!bstat)  {
	   		dwError = GetLastError();
			static int first=10;

			if (first>0)  {
				first--;
			}

			if (first==0)  {
				first = -1;
				if (logg) logg->write(10,0,1,"serial Receive() error = %lu, turned off further error printing\n",dwError);
			}

   			switch(dwError)  {

		  		case ERROR_HANDLE_EOF:
		 			break;

				case ERROR_IO_PENDING:				// asyn i/o is still in progress
			 		break;
			}

			return;
   		}

	   else  {

#ifdef _DEBUG
		   if (rawstream) fwrite(buf, 1, n, rawstream);
#endif

			bytes_in += n;
			//-------------------------------------------------------
			// put the bytes in the buffer into the receiver queue:
			//-------------------------------------------------------

			for(i=0;i<n;i++)  {
#ifdef _DEBUG
				if (buf[i]==0xcc)  {
					bp = i;
				}
#endif

				//logg->write(10,0,0,"%c\n", buf[i]);

				rxq[rxinptr] = buf[i];
				rxinptr = (unsigned short) ((rxinptr+1) % RXQLEN);
			}

			rxdiff = (rxinptr - rxoutptr)  &  (RXQLEN - 1);
			maxrxdiff = MAX(maxrxdiff, rxdiff);

			return;
	   }

	}							// if (RX_CHAR)

	return;

}


/**********************************************************************

**********************************************************************/

void Serial::send(char *str, int flush_flag)  {
	int i=0;

	while(str[i])  {
		tx(str[i++]);
	}

	if (flush_flag)  {
		flush();
	}

	if (txasleep)  {
		SetEvent(txControlEvent);				// wake up transmitter thread
	}

	return;
}


/**********************************************************************
	waits for a result
   returns 0 if it is found, 1 otherwise.
**********************************************************************/

int Serial::expect(char *str, DWORD timeout)  {
	int len,i=0;
	unsigned char c[8];
	DWORD start,end;
	int n;

	len = strlen(str);

	start = timeGetTime();

	while(1)  {
		n = rx((char *)c, 1);

      if (n==1)  {
			if (c[0]==str[i])  {
         	i++;
				if (i==len)  {
					return 0;
				}
			}
			else  {
				i = 0;
			}
		}
		else  {
			end = timeGetTime();
         if ((end-start)>timeout)  {
				return 1;
         }
		}
	}

	return 1;
}

/**********************************************************************
	simulates hyperterm

	works only in a console program

	example usage:

	Serial *port = new Serial("COM1", "2400", FALSE, NULL);
	port->hyperterm();
	DEL(port);

**********************************************************************/

void Serial::hyperterm(void)  {
	char c;
	int bp = 0;
	int n;
	char buf[8];
	FILE *stream=NULL;


	stream = fopen("ht.txt", "wb");

	while(1)  {
		if (kbhit())  {
			c = (char)toupper(getch());
			if (c==0)  {
				c = (char)getch();
			}
			else if (c==0x1b)  {
				break;
			}
		}

		n = rx(buf, 1);


		if (n==1)  {
			fwrite(buf, 1, 1, stream);
			if (buf[0]==0x0a)  {
			}
		}
		else if (n>1)  {
			bp = 3;
		}
		Sleep(1);
	}

	FCLOSE(stream);

	bp = 1;
	return;

}


/**********************************************************************

**********************************************************************/

void Serial::destroy(void)  {
	if (logg) logg->write(10,0,1,"   Com%d destructor 1\n", comnum);

#ifdef _DEBUG
	FCLOSE(rawstream);
	sprintf(str, "Com%d destructor 1\n", comnum);
	OutputDebugString(str);

	sprintf(str, "max rxdiff = %hd\n", maxrxdiff);
	if (logg && logg->stream)  {
		logg->write("%s", str);
	}
	OutputDebugString(str);

	//sprintf(str, "max txdiff = %hd\n", maxtxdiff);
	//logg->write("%s", str);
	//OutputDebugString(str);
	sprintf(str, "bytes_in = %ld\n", bytes_in);
	OutputDebugString(str);
	sprintf(str, "bytes_sent = %ld\n", bytes_sent);
	OutputDebugString(str);

#endif

	if (logg && logg->stream) logg->write(10,0,1,"   Com%d destructor 2\n", comnum);
	Close(0);
	if (logg && logg->stream) logg->write(10,0,1,"   Com%d destructor 3\n", comnum);
	Sleep(shutDownDelay);
	hCommPort = 0;

#ifdef _DEBUG
	sprintf(str, "Com%d destructor 2\n", comnum);
	OutputDebugString(str);
	DEL(st1);
#endif

	return;
}

/***********************************************************************

***********************************************************************/

void Serial::txwake(void)  {
	if (txinptr != txoutptr)  {
		if (txasleep)  {
			SetEvent(txControlEvent);				// wake up transmitter thread
		}
	}
	return;
}

/***********************************************************************

***********************************************************************/

const char * Serial::get_port_name(void)  {

	//strncpy(portname, &portstr[3], 7);
	return portname;
}


#ifdef _DEBUG
/**********************************************************************

**********************************************************************/

void Serial::flush_rawstream(void)  {

	if (rawstream)  {
		fflush(rawstream);
		fclose(rawstream);
		rawstream = fopen(rawstr, "a+b");
	}
	return;
}

/**********************************************************************

**********************************************************************/

void Serial::StackOverflow(int depth)  {

	#if 0
		char blockdata[10000];
		printf("Overflow: %d\n", depth);
		StackOverflow(depth+1);
	#endif

	return;
}

#endif


#if 0

/**********************************************************************

**********************************************************************/

const char *Serial::get_error(int i)  {

	/*
static const char *serialErrorStrings[SERIAL_ERROR_OTHER] = {
	"SERIAL_ERROR_DOES_NOT_EXIST",
	"SERIAL_ERROR_ACCESS_DENIED",
	"SERIAL_ERROR_BUILDCOMMDCB",
	"SERIAL_ERROR_SETCOMMSTATE",
	"SERIAL_ERROR_CREATEEVENT1",
	"SERIAL_ERROR_CREATEEVENT2",
	"SERIAL_ERROR_OTHER"
};

	*/

	if (i==5)  {
		return "SERIAL_ERROR_ACCESS_DENIED";
	}
	else if (i==2)  {
		return "SERIAL_ERROR_DOES_NOT_EXIST";
	}
	else if (i==1168)  {
		return "SERIAL_ERROR 1168";
	}
	else if (i==121)  {										// the semaphore timeout period has expired
		return "SERIAL_ERROR 121";
	}
	//else if (i==SERIAL_ERROR_BUILDCOMMDCB)  {
	//	return "SERIAL_ERROR_BUILDCOMMDCB";
	//}

	return "SERIAL_ERROR_OTHER";
}
#endif

#endif				// #ifdef WIN32


