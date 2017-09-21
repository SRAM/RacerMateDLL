

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <WinSock2.h>
	#include <stdio.h>
	#include <windows.h>
	#include <mmsystem.h>
	#include <commctrl.h>
	//#include <winsock2.h>
	#include <objbase.h>
#else
	#ifndef __MACH__
		#include <stdarg.h>
		#include <ifaddrs.h>
		#include <net/if.h>
		#include <unistd.h>
#include <fcntl.h>
	#endif
	#include <minmax.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
#endif						// #ifdef WIN32

#include <assert.h>

#include <glib_defines.h>
#include <utils.h>
#include <averagetimer.h>
#include <server.h>

int Server::instances = 0;

/**********************************************************************
	use ./client/Client.java on localhost to test
**********************************************************************/

Server::Server(void)  {
	int status;

	throw("bad server constructor");

	listen_port = 9072;

	status = tcp_init();
	if (status != 0)  {
		//initialized = false;
		throw 100;
		//return;
	}

	//initialized = true;

}								// constructor

/**********************************************************************

**********************************************************************/

Server::Server(int _broadcast_port, int _listen_port, bool _ip_discover, bool _udp) {
	int status;

	broadcast_port = _broadcast_port;
	listen_port = _listen_port;
	ip_discover = _ip_discover;
	udp = _udp;

	if (udp) {
		status = udp_init();
		if (status != 0) {
			throw 100;
		}
	}
	else {
		status = tcp_init();
		if (status != 0) {
			throw 100;
		}
	}
}


/**********************************************************************
	destructor
**********************************************************************/

Server::~Server()  {

#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 2)  {
		bp = 1;
	}
#endif

	destroy();
	//bp = 0;
	instances--;

}

/**********************************************************************

**********************************************************************/

int Server::udp_init(void) {
	int status;
	int i;

	for (i = 0; i < MAXUDPCLIENTS; i++) {
		udpclients[i] = NULL;
	}

	//bp = 0;
	//char str[256];


	msgcnt = 0;
	curid = 0;
	//udp_server_socket = 0;
	//tcp_server = 0;

	//for (i = 0; i < MAXCLIENTS; i++) {
	//	clients[i] = new CLIENT2(i);
	//}


#if 0
	for (i = 0; i < MAXCLIENTS; i++) {
		tcpclients[i] = new TCPClient(i, this);
		udpclients[i] = new UDPClient(i, this);

		connect(udpclients[i], SIGNAL(data_signal(int, DATA*)), this, SLOT(data_slot(int, DATA*)));
		connect(udpclients[i], SIGNAL(ss_signal(int, SS::SSD*)), this, SLOT(ss_slot(int, SS::SSD*)));
		connect(udpclients[i], SIGNAL(rescale_signal(int, int)), this, SLOT(rescale_slot(int, int)));
#ifdef TESTING
		connect(udpclients[i], SIGNAL(testing_signal(int, TESTDATA*)), this, SLOT(testing_slot(int, TESTDATA*)));
#endif
		connect(this, SIGNAL(gradechanged_signal(int)), udpclients[i], SLOT(gradechanged_slot(int)));
		connect(this, SIGNAL(windchanged_signal(float)), udpclients[i], SLOT(windchanged_slot(float)));

	}
#endif


	started = false;
	finished = false;
	paused = false;

	log_to_console = true;

	//bc_datagram.clear();

	bcast_count = 0L;
	errors = 0;

	contin = false;
	logstream = NULL;
	//logstream = fopen("server.log", "wt");

	last_ip_broadcast_time = 0L;
	//bcsocket = NULL;

	//bp = 0;
	thread_running = false;


	//QStringList items;
	//int done = 0;

	/*
	bool done = false;
	foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
	if (interface.flags().testFlag(QNetworkInterface::IsUp) && !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

	foreach(QNetworkAddressEntry entry, interface.addressEntries()) {
	if ( interface.hardwareAddress() != "00:00:00:00:00:00" && entry.ip().toString().contains(".")) {
	items << interface.name() + " " + entry.ip().toString() + " " + interface.hardwareAddress();
	myip = entry.ip();

	#ifdef _DEBUG
	//bcaddr = entry.broadcast();
	#else
	bcaddr = entry.broadcast();
	#endif
	done = true;
	//done = 1;
	break;
	// "eth0	192.168.1.20	F0:BF:97:57:D5:9B"
	// "wlan0	192.168.1.21	40:25:C2:26:FE:78"
	}
	}
	if (done) {
	break;
	}
	}
	}
	*/



	//bp = 0;

	/*
	logg(log_to_console, "myip = %s\r\n", myip.toString().toStdString().c_str());

	bc_datagram.append("Racermate ");
	bc_datagram.append(QString::number(listen_port));
	bc_datagram.append(" ");
	bc_datagram.append(myip.toString());
	bc_datagram.append("\r\n");
	*/

	//bool b;
	if (ip_discover) {
		//bcsocket = new QUdpSocket(this);
#if 0
		QList<QHostAddress> ip = info.addresses();
		bcaddr = ip[0];
		qDebug() << "bcaddr = " << bcaddr;
#endif
	}                                // if (ip_discover)  {


	status = 0;


	if (udp) {
		//tcp_server = 0;
		//status = create_udp_server_socket();                                     // creates the server socket and starts listening.
	}
	else {
		//udp_server_socket = 0;
		//status = create_tcp_server();                                        // creates the server socket and starts listening.
	}


	if (status != 0) {
		return 1;
	}



	//at = new Tmr("server");
	//contin = true;

	//qDebug() << "started thread";

	if (ip_discover) {
		// create a timer for broadcasting
		//bctimer = new QTimer(this);
		//connect(bctimer, SIGNAL(timeout()), this, SLOT(bc_timer_slot()));
		//bctimer->start(2000);
	}

	//instances++;
	//qDebug() << "Server::init x";

	//////////////////////////////////////////////

	unsigned char bcaddr[4] = { 0,   0,   0,  0 };
	unsigned char mask[4] = { 255, 255, 255,  0 };
	char str[256];

	log_to_console = true;


#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 2) {
		bp = 1;
	}
#endif

	memset(myaddr, 0, sizeof(myaddr));
	memset(broadcast_message, 0, sizeof(broadcast_message));

	bcast_count = 0L;
	errors = 0;

	tcpclients.clear();

	/*
	for (i = 0; i<(int)clients.size(); i++) {
		memset(&clients[i], 0, sizeof(CLIENT2));
		clients[i].socket = INVALID_SOCKET;
	}
	*/


	contin = false;
	logstream = NULL;
	//logstream = fopen("server.log", "wt");

	thrd = NULL;
	last_ip_broadcast_time = 0L;
	maxsocket = -1;
	tcp_listen_socket = INVALID_SOCKET;										// server socket
	udp_socket = INVALID_SOCKET;
	broadcast_socket = INVALID_SOCKET;
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));

	//bp = 0;
	thread_running = false;

#ifdef WIN32
	WSADATA wsaData;
	INITCOMMONCONTROLSEX iccex;

	if (FAILED(CoInitialize(NULL))) {
		MessageBox(NULL, "CoInitialize", "Error", MB_OK | MB_ICONINFORMATION);
		return 1;
	}

	memset(&iccex, 0, sizeof(iccex));
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);
	if ((status = WSAStartup(0x0202, &wsaData)) != 0) {
		//logg(log_to_console, "WSAStartup error, status = %d\r\n", status);
		return 1;
	}
	if (wsaData.wVersion != 0x0202) {		// wrong WinSock version!
		WSACleanup();						// unload ws2_32.dll
											//logg(log_to_console, "wrong winsock version\r\n");
		return 1;
	}
	//logg(log_to_console, "WSAStartup status = %d\r\n", status);
#else
#endif					// #ifdef WIN32

#ifdef _DEBUG
	if (calls == 2) {
		bp = 1;
	}
#endif

	status = getmyip();							// gets 'myip' (192.168.1.20), (also ifaces contains 192.168.1.21)
	if (status != 0) {
		return 1;
	}
	if (strlen(myip) == 0) {
		strcpy(myip, "unknown");
	}
	logg(log_to_console, "myip = %s\r\n", myip);

	sscanf(myip, "%d.%d.%d.%d", &myaddr[0], &myaddr[1], &myaddr[2], &myaddr[3]);

	memset(broadcast_address, 0, sizeof(broadcast_address));

	for (i = 0; i<4; i++) {
		bcaddr[i] = myaddr[i] | (~mask[i]);
	}

	for (i = 0; i<3; i++) {
		sprintf(str, "%d.", bcaddr[i]);
		strcat(broadcast_address, str);
	}
	sprintf(str, "%d", bcaddr[i]);
	strcat(broadcast_address, str);

#ifdef _DEBUG
	//strcpy(broadcast_address, "miney2.mselectron.net");
	//strcpy(broadcast_address, "50.194.61.75");
#endif

	logg(log_to_console, "broadcast address = %s\r\n", broadcast_address);						// 192.168.1.255

	sprintf(broadcast_message, "RacerMate %s %d", myip, listen_port);

	if (ip_discover) {
		status = create_broadcast_socket();
		if (status != 0) {
			logg(log_to_console, "create broadcast socket failed");
			return 1;
		}
	}											// if (ip_discover)  {



	status = create_udp_socket();												// creates the server socket and starts listening.
	if (status != 0) {
		return 1;
	}

	at = new AverageTimer("server");

	contin = true;
	thrd = new tthread::thread(&udpthread, this);

	// wait 300 ms for a potential client to connect. The client should be trying to connect every 100 ms.

	Sleep(305);

	for (i = 0; i<MAXUDPCLIENTS; i++) {
		udpclients[i] = new UDPClient(i);
		//memset(&udpclients[i], 0, sizeof(UDPClient));
		//udpclients[i]->socket = INVALID_SOCKET;
	}

	instances++;


	//////////////////////////////////////////////

	return 0;

}					// udp_init()


/**********************************************************************

**********************************************************************/

int Server::tcp_init(void)  {
	int status;
	unsigned char bcaddr[4] = {   0,   0,   0,  0 };
	unsigned char mask[4] =   { 255, 255, 255,  0 };
	int i;
	char str[256];

	log_to_console = true;
	for (i = 0; i<MAXCLIENTS; i++) {
		memset(&tcpclients[i], 0, sizeof(CLIENT2));
		tcpclients[i].socket = INVALID_SOCKET;
	}


#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 2)  {
		bp = 1;
	}
#endif

	memset(myaddr, 0, sizeof(myaddr));
	memset(broadcast_message, 0, sizeof(broadcast_message));

	bcast_count = 0L;
	errors = 0;

	for(i=0; i<(int)tcpclients.size(); i++)  {
		memset(&tcpclients[i], 0, sizeof(CLIENT2));
		tcpclients[i].socket = INVALID_SOCKET;
	}

	contin = false;
	logstream = NULL;
	//logstream = fopen("server.log", "wt");

	thrd = NULL;
	last_ip_broadcast_time = 0L;
	maxsocket = -1;
	tcp_listen_socket = INVALID_SOCKET;										// server socket
	udp_socket = INVALID_SOCKET;										// server socket
	broadcast_socket = INVALID_SOCKET;

	//bp = 0;
	thread_running = false;

#ifdef WIN32
	WSADATA wsaData;
	INITCOMMONCONTROLSEX iccex;

	if( FAILED(CoInitialize(NULL)) )  {
		MessageBox( NULL, "CoInitialize", "Error", MB_OK | MB_ICONINFORMATION );
		return 1;
	}

	memset(&iccex, 0, sizeof(iccex));
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);
	if ( (status=WSAStartup(0x0202, &wsaData)) != 0 )  {
		//logg(log_to_console, "WSAStartup error, status = %d\r\n", status);
		return 1;
	}
	if (wsaData.wVersion != 0x0202)  {		// wrong WinSock version!
		WSACleanup ();						// unload ws2_32.dll
		//logg(log_to_console, "wrong winsock version\r\n");
		return 1;
	}
	//logg(log_to_console, "WSAStartup status = %d\r\n", status);
#else
#endif					// #ifdef WIN32

#ifdef _DEBUG
	if (calls == 2)  {
		bp = 1;
	}
#endif

	status = getmyip();							// gets 'myip' (192.168.1.20), (also ifaces contains 192.168.1.21)
	if (status != 0)  {
		return 1;
	}
	if (strlen(myip)==0)  {
		strcpy(myip, "unknown");
	}
	logg(log_to_console, "myip = %s\r\n", myip);

	sscanf(myip, "%d.%d.%d.%d", &myaddr[0], &myaddr[1], &myaddr[2], &myaddr[3]);

	memset(broadcast_address, 0, sizeof(broadcast_address));

	for(i=0; i<4; i++)  {
		bcaddr[i] = myaddr[i] | (~mask[i]);
	}

	for(i=0; i<3; i++)  {
		sprintf(str, "%d.", bcaddr[i]);
		strcat(broadcast_address, str);
	}
	sprintf(str, "%d", bcaddr[i]);
	strcat(broadcast_address, str);

	logg(log_to_console, "broadcast address = %s\r\n", broadcast_address);						// 192.168.1.255

	sprintf(broadcast_message, "RacerMate %s %d", myip, listen_port);

	if (ip_discover)  {
		status =  create_broadcast_socket();
		if (status != 0)  {
			logg(log_to_console, "create broadcast socket failed");
			return 1;
		}
	}											// if (ip_discover)  {



	status = create_tcp_listen_socket();												// creates the server socket and starts listening.
	if (status != 0)  {
		return 1;
	}

	at = new AverageTimer("server");

	contin = true;
	thrd = new tthread::thread(&tcpthread, this);

	// wait 300 ms for a potential client to connect. The client should be trying to connect every 100 ms.

	Sleep(300);
	instances++;

	return 0;
}															// tcp_init()


/**********************************************************************

**********************************************************************/

void Server::destroy(void)  {
	//int status;

	myclose();

	return;
}															// destroy()

/**********************************************************************
	server rx
**********************************************************************/

int Server::rx(int _ix, unsigned char *buf, int buflen)  {

	int ix;

	if (udpclients) {
		ix = _ix - UDP_SERVER_SERIAL_PORT_BASE;
		if (udpclients[ix]) {
			//xxxxxxxxxxx
			int status;
			status = udpclients[ix]->rx(buf, buflen);

			//bp = 1;
		}
	}


	if (_ix >= (int)tcpclients.size())  {
		return 0;
	}

	return 0;
}

/**********************************************************************

**********************************************************************/

int Server::expect(int _ix, const char *_str, DWORD _timeout)   {
	int len,i=0;
	unsigned char c[8];
	DWORD start,end;
	int n;

	len = strlen(_str);

	start = timeGetTime();

	while(1)  {
		n = rx(_ix, c, 1);

      if (n==1)  {
			if (c[0]==_str[i])  {
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
         if ((end-start)>_timeout)  {
				return 1;
         }
		}
	}

	return 1;
}

/**********************************************************************

**********************************************************************/

void Server::flush(void)  {

	return;
}


#ifdef _DEBUG
/**********************************************************************

**********************************************************************/

void Server::flush_rawstream(int _ix)  {
	return;
}


#endif

/**********************************************************************

**********************************************************************/


void Server::txx(unsigned char *b, int n)  {
	return;
}


/**********************************************************************

**********************************************************************/


int Server::myclose(void) {
	unsigned long start;
	int rc = 0;

	contin = FALSE;

	start = timeGetTime();


	while(thread_running)  {
		if ((timeGetTime() - start) >= 2000)  {
			rc = 1;
			break;
		}
		//bp++;
	}
	//bp = 1;

#ifdef _DEBUG
	//unsigned long dt = timeGetTime() - start;
	static int calls = 0;
	calls++;
	if (calls==2)  {
		bp = 3;
	}
#endif

	if (thrd)  {
		thrd->join();
		DEL(thrd);
	}


	if (broadcast_socket != INVALID_SOCKET)  {
		#ifdef WIN32
		closesocket(broadcast_socket);
		#else
		close(broadcast_socket);
		#endif
		broadcast_socket = INVALID_SOCKET;
	}

	if (tcp_listen_socket != INVALID_SOCKET)  {
		#ifdef WIN32
		closesocket(tcp_listen_socket);
		#else
		close(listen_socket);
		#endif
		tcp_listen_socket = INVALID_SOCKET;
	}

	if (udp_socket != INVALID_SOCKET) {
#ifdef WIN32
		closesocket(udp_socket);
#else
		close(udp_socket);
#endif
		udp_socket = INVALID_SOCKET;
	}

	DEL(at);

	/*
	int i;
	for (i = 0; i < clients.size(); i++) {
		clients[i]
	}
	*/
	int i;
	for (i = 0; i < MAXUDPCLIENTS; i++) {
		DEL(udpclients[i]);
	}

	FCLOSE(logstream);
	return rc;

}									// close()

/***********************************************************************************

***********************************************************************************/

int Server::create_udp_socket(void) {
	//int len_inet;
	struct sockaddr_in server_address;			// AF_INET
	int n;
	int status;

	//len_inet = sizeof(server_address);
	memset((void *)&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(listen_port);										// 9072
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	//listen_socket = socket(PF_INET, SOCK_STREAM, 0);
	udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (udp_socket == -1) {
		//bail("socket(2)");
		logg(log_to_console, "socket error\r\n");
		return 1;
	}

	// so that we can re-bind to it without TIME_WAIT problems:

	n = 1;

	if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&n, sizeof(int))<0) {
		//perror("SETSOCKOPT");
		//exit(EXIT_FAILURE);
		logg(log_to_console, "setsockopt error\r\n");
		closesocket(udp_socket);
		WSACleanup();
		return 2;
	}

	status = bind(udp_socket, (struct sockaddr *)&server_address, sizeof(server_address));
	/*
	if (status==-1)  {
	// You use the same ip/port or you too quickly run the same server and
	// previous used ip/port is in the TIME_WAIT state.
	//bail("bind(2)");
	logg(log_to_console, "bind error\r\n");
	return 3;
	}
	*/

	if (status != 0) {
#ifdef WIN32
		status = WSAGetLastError();

		switch (status) {
		case 0:
			//bp = 0;
			break;
		case WSANOTINITIALISED:
			//bp = 2;
			break;
		case WSAENETDOWN:
			//bp = 2;
			break;
		case WSAEACCES:
			//bp = 2;
			break;
		case WSAEADDRINUSE:
			//bp = 2;															// gets here for TIME_WAIT problem
			break;
		case WSAEADDRNOTAVAIL:
			//bp = 2;
			break;
		case WSAEFAULT:
			//bp = 2;
			break;
		case WSAEINPROGRESS:
			//bp = 2;
			break;
		case WSAEINVAL:
			//bp = 2;
			break;
		case WSAENOBUFS:
			//bp = 2;
			break;
		case WSAENOTSOCK:
			//bp = 2;
			break;
		default:
			//bp = 1;
			break;
		}
#endif		// #ifdef WIN32
		logg(log_to_console, "bind error\r\n");
		closesocket(udp_socket);
		WSACleanup();
		return 3;
	}

	/*
	getsockname(udp_socket, (SOCKADDR *)&server_address, (int *)sizeof(server_address));
	char str[256];

	sprintf(str, "Server: Receiving IP(s) used: %s", inet_ntoa(server_address.sin_addr));
	OutputDebugString(str);

	sprintf(str, "Server: Receiving port used: %d", htons(server_address.sin_port));
	OutputDebugString(str);
	*/

	//bp = 2;

	/*
	status = listen(listen_socket, BACKLOG);

	if (status != 0) {
		status = WSAGetLastError();

		switch (status) {
			case WSANOTINITIALISED:
				bp = 0;
				break;
			case WSAENETDOWN:
				bp = 0;
				break;
			case WSAEADDRINUSE:
				bp = 0;
				break;
			case WSAEINPROGRESS:
				bp = 0;
				break;
			case WSAEINVAL:
				bp = 0;
				break;
			case WSAEISCONN:
				bp = 0;
				break;
			case WSAEMFILE:
				bp = 0;
				break;
			case WSAENOBUFS:
				bp = 0;
				break;
			case WSAENOTSOCK:
				bp = 0;
				break;
			case WSAEOPNOTSUPP:					// The referenced socket is not of a type that supports the listen operation
				bp = 0;
				break;
			default:
				bp = 1;
				break;
		}
		//bail("listen(2)");
		logg(log_to_console, "listen error\r\n");
		return 4;
	}
	*/


	/*
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	FD_SET(listen_socket, &readfds);
	FD_SET(listen_socket, &writefds);
	FD_SET(listen_socket, &exceptfds);

	maxsocket = listen_socket;
	*/

	maxsocket = udp_socket;

	return 0;
}


/***********************************************************************************

***********************************************************************************/

int Server::create_tcp_listen_socket(void)  {
	int len_inet;
	struct sockaddr_in server_address;			// AF_INET
	int n;
	int status;

	len_inet = sizeof(server_address);
	memset((void *)&server_address, 0, sizeof(server_address) );
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(listen_port);										// 9072
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	tcp_listen_socket = socket(PF_INET, SOCK_STREAM, 0);

	if (tcp_listen_socket==-1)  {
		//bail("socket(2)");
		logg(log_to_console, "socket error\r\n");
		return 1;
	}

	// so that we can re-bind to it without TIME_WAIT problems:

	n = 1;

	if(setsockopt(tcp_listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&n,	sizeof(int))<0)  {
		//perror("SETSOCKOPT");
		//exit(EXIT_FAILURE);
		logg(log_to_console, "setsockopt error\r\n");
		return 2;
	} 

	status = bind(tcp_listen_socket, (struct sockaddr *)&server_address, len_inet);
	/*
	if (status==-1)  {
		// You use the same ip/port or you too quickly run the same server and
		// previous used ip/port is in the TIME_WAIT state.
		//bail("bind(2)");
		logg(log_to_console, "bind error\r\n");
		return 3;
	}
	*/

	if (status!=0)  {
#ifdef WIN32
		status = WSAGetLastError();

		switch (status)  {
			case 0:
				//bp = 0;
				break;
			case WSANOTINITIALISED:
				//bp = 2;
				break;
			case WSAENETDOWN:
				//bp = 2;
				break;
			case WSAEACCES:
				//bp = 2;
				break;
			case WSAEADDRINUSE:
				//bp = 2;															// gets here for TIME_WAIT problem
				break;
			case WSAEADDRNOTAVAIL:
				//bp = 2;
				break;
			case WSAEFAULT:
				//bp = 2;
				break;
			case WSAEINPROGRESS:
				//bp = 2;
				break;
			case WSAEINVAL:
				//bp = 2;
				break;
			case WSAENOBUFS:
				//bp = 2;
				break;
			case WSAENOTSOCK:
				//bp = 2;
				break;
			default:
				//bp = 1;
				break;
		}
#endif		// #ifdef WIN32
		logg(log_to_console, "bind error\r\n");
		return 3;
	}

	status = listen(tcp_listen_socket, BACKLOG);
	if (status==-1)  {
		//bail("listen(2)");
		logg(log_to_console, "listen error\r\n");
		return 4;
	}

	/*
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	FD_SET(listen_socket, &readfds);
	FD_SET(listen_socket, &writefds);
	FD_SET(listen_socket, &exceptfds);

	maxsocket = listen_socket;
	*/

	maxsocket = tcp_listen_socket;

	return 0;
}														// int create_listen_socket()

//#if defined(WIN32) | if defined(__MACH__)
#if 1

/**********************************************************************

 ***********************************************************************/

void Server::logg(bool _print, const char *format, ...)  {
	va_list ap;													// Argument pointer
	char s[1024];												// Output string
	int len;



	len = (int)strlen(format);
	if (len>1023)  {
		if (_print) printf("\r\n(string too long in logg())\r\n");
		if (logstream)  {
			fprintf(logstream, "\r\n(string too long in logg())\r\n");
			//fflush(logstream);
		}
		return;
	}

//#if defined(WIN32) || defined(__MACH__)
#if 1
	va_start(ap, format);
	vsprintf(s, format, ap);
	va_end(ap);

	if (_print) {
		printf("%s", s);
	}


//	if (!verbose)  {
//		return;
//	}

//#ifndef __MACH__
	if (!logstream)  {
		return;
	}

	int status;

	status = fprintf(logstream, "%s", s);
	if (status < 0)  {
		throw("bad fprintf");
	}
	// in windows, this leaves an 'empty' log file!
	status = fflush(logstream);
	if (status != 0)  {
		throw("bad fflush");
	}
#else
	/*
		setlogmask (LOG_UPTO (LOG_NOTICE));
		openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
		syslog (LOG_NOTICE, "Program started by User %d", getuid ());
		syslog (LOG_INFO, "A tree falls in a forest");
		closelog ();
	 */

//	syslog(LOG_INFO, "%s", s);					// syslog
#endif


	return;
}													// logg()

#else

#endif							// #ifdef WIN32


#ifdef WIN32
/****************************************************************************
	http://stackoverflow.com/questions/122208/get-the-ip-address-of-local-computer
	http://tangentsoft.net/wskfaq/examples/ipaddr.html
****************************************************************************/

int Server::getmyip(void)  {
	unsigned long nBytesReturned;
	int nNumInterfaces;
	u_long nFlags;
	sockaddr_in *pAddress;
	int i;
	int cnt=0;
	INTERFACE_INFO InterfaceList[20];


    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sd == SOCKET_ERROR)  {
		 return 1;
    }

	if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR)  {
		//cerr << "Failed calling WSAIoctl: error " << WSAGetLastError() << endl;
		return 1;
	}

	nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);


	for (i=0; i<nNumInterfaces; ++i) {
		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_LOOPBACK)  {
			continue;												// we don't want this one
		}
		if (!(nFlags & IFF_UP))   {
			continue;
		}

		ifaces.push_back(InterfaceList[i]);

		cnt++;

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
		strncpy(myip, inet_ntoa(pAddress->sin_addr), sizeof(myip)-1);

		//logg(log_to_console, "found more than one interface, using %s\r\n", myip);
		logg(log_to_console, "%d   %s", cnt, myip);

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
		logg(log_to_console, "    %s", inet_ntoa(pAddress->sin_addr));

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
		logg(log_to_console, "    %s    ", inet_ntoa(pAddress->sin_addr));

		logg(log_to_console, "    ");

		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_UP)   {
			logg(log_to_console, "up");
		}
      else   {
			logg(log_to_console, "down");
		}

		if (nFlags & IFF_POINTTOPOINT)   {
			logg(log_to_console, ", point-to-point");
		}
		if (nFlags & IFF_LOOPBACK)  {
			logg(log_to_console, ", loopback");
		}
		if (nFlags & IFF_BROADCAST)  {
			logg(log_to_console, ", broadcast ");
		}
      if (nFlags & IFF_MULTICAST)  {
			logg(log_to_console, ", multicast ");
		}
		logg(log_to_console, "\r\n");

	}

	int n = ifaces.size();

	if (n <= 1)  {
		return 0;
	}

	// more than one interface, need to choose one?

	logg(log_to_console, "found more than one interface, using %s\r\n", myip);

	return 0;
}														// int Server::getmyip(void)  {

#else

int Server::getmyip(void)  {
	//strcpy(myip, "192.168.1.150");

	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];
	int ix;


	if(getifaddrs(&myaddrs) != 0) {
		perror("getifaddrs");
		exit(1);
	}

	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)  {
			continue;
		}

		if (!(ifa->ifa_flags & IFF_UP))  {
			continue;
		}

		switch (ifa->ifa_addr->sa_family) {
			case AF_INET: {
								  struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
								  in_addr = &s4->sin_addr;
								  break;
							  }

			case AF_INET6: {
									struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
									in_addr = &s6->sin6_addr;
									break;
								}

			default:
								continue;
		}

		if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf))) {
			printf("%s: inet_ntop failed!\r\n", ifa->ifa_name);
			continue;
		}

		/*
		lo: 127.0.0.1
		eth0: 192.168.1.20
		wlan0: 192.168.1.21
		lo: ::1
		eth0: fe80::f2bf:97ff:fe57:d59b
		wlan0: fe80::4225:c2ff:fe26:fe78
		 */

		ix = myindex((const char *)ifa->ifa_name, "lo");
		if (ix != -1)  {
			continue;
		}

		ix = myindex((const char *)buf, "::");
		if (ix != -1)  {
			continue;
		}

		printf("%s: %s\r\n", ifa->ifa_name, buf);

		ix = myindex((const char *)ifa->ifa_name, "eth");
		if (ix == 0)  {
			//break;
		}

		ix = myindex((const char *)ifa->ifa_name, "wlan");
		if (ix == 0)  {
		}

		IFACE intf;
		strncpy(intf.name, ifa->ifa_name, sizeof(intf.name)-1);
		strncpy(intf.addr, buf, sizeof(intf.addr)-1);
		ifaces.push_back(intf);
	}										// for()


	freeifaddrs(myaddrs);

	for(int i=0; i<(int)ifaces.size(); i++)  {
		logg(true, "%d   %s   %s\r\n", i+1, ifaces[i].name, ifaces[i].addr);
	}

	if (ifaces.size() > 0)  {
		strncpy(myip, ifaces[0].addr, sizeof(myip)-1);
	}

	return 0;
}											// void getmyip(void)  {

#endif


/***********************************************************************************

***********************************************************************************/

int Server::create_broadcast_socket(void)  {

	logg(log_to_console, "creating broadcast socket\r\n");

	int broadcastPermission;          	// socket opt to set permission to broadcast

	// Create socket for sending/receiving datagrams

	if ((broadcast_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)  {
		return 1;
	}

	// set socket to allow broadcast

	broadcastPermission = 1;

#ifdef WIN32
	if (setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, (const char *) &broadcastPermission, sizeof(broadcastPermission)) < 0)  {
#else
	if (setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)  {
#endif

		printf("setsockopt() failed");
		//cleanup();
		//exit(1);
		return 1;
	}

	// construct local address structure

	memset(&broadcastAddr, 0, sizeof(broadcastAddr));						// Zero out structure
	broadcastAddr.sin_family = AF_INET;											// Internet address family
	broadcastAddr.sin_addr.s_addr = inet_addr(broadcast_address);		// Broadcast IP address

	if (broadcastAddr.sin_addr.s_addr == INADDR_NONE) {
		//bp = 3;
	}

	broadcastAddr.sin_port = htons(broadcast_port);							// Broadcast port

	sendStringLen = strlen(broadcast_message);  // Find length of sendString

	return 0;
}													// int Server::create_broadcast_socket(void) 

/**********************************************************************

**********************************************************************/

bool Server::is_client_connected(int _comport)  {
	int n, ix;

	//std::vector<CLIENT2> tcpclients;
	//UDPClient *udpclients[MAXUDPCLIENTS];

	if (tcpclients.size() > 0)  {
		ix = _comport - SERVER_SERIAL_PORT_BASE;
		n = tcpclients.size();
		if (ix==n-1)  {
			if (tcpclients[ix].socket != INVALID_SOCKET)  {
				return true;
			}
		}
	}

	if (udpclients)  {
		ix = _comport - UDP_SERVER_SERIAL_PORT_BASE + 1;
		if (udpclients[ix]) {
			if (udpclients[ix]->is_connected()) {
				return true;
			}
		}
	}
	return false;
}


/*************************************************************************
	called only frrom thread
*************************************************************************/

void Server::pack(void)  {
	int i, n;
	std::vector<CLIENT2> tc;

	n = tcpclients.size();

	for(i=0;i<n;i++)  {
		if (tcpclients[i].socket!=INVALID_SOCKET)  {
			tc.push_back(tcpclients[i]);
		}
	}

	tcpclients.clear();
	n = tc.size();
	maxsocket = tcp_listen_socket;

	for(i=0;i<n;i++)  {
		tcpclients.push_back(tc[i]);
		maxsocket = MAX(maxsocket, tc[i].socket);
	}

	if (tcpclients.size()==0)  {
		logg(log_to_console, "all clients closed, maxsocket is now %d\r\n", maxsocket);
	}

	return;
}												// pack()

/**********************************************************************
	the server socket is already created and we are waiting for a client
	to connect.
**********************************************************************/

void Server::tcpthread(void *tmp)  {
	struct timeval timeout;						// timeout value
	int csock = INVALID_SOCKET;										// client socket
	AverageTimer at("server_thread");
	int k;
	fd_set readfds;								// read set
	fd_set writefds;								// write set
	fd_set exceptfds;								// exception set

	Server *server = static_cast<Server *>(tmp);
	server->thread_running = true;
	int cnt = 0;
	server->logg(true, "\r\nlistening for connections on port %d\r\n\r\n", server->listen_port);


	while(server->contin)  {
		cnt++;

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

		FD_SET(server->tcp_listen_socket, &readfds);
		FD_SET(server->tcp_listen_socket, &writefds);
		FD_SET(server->tcp_listen_socket, &exceptfds);

		for(k=0; k < (int)server->tcpclients.size(); k++)  {
			if (server->tcpclients[k].socket != INVALID_SOCKET)  {
				//FD_ZERO(&server->clients[k].readfds);
				//FD_ZERO(&server->clients[k].writefds);
				//FD_ZERO(&server->clients[k].exceptfds);

				FD_SET(server->tcpclients[k].socket, &readfds);
				//FD_SET(server->clients[k].n, &server->writefds);
				FD_SET(server->tcpclients[k].socket, &exceptfds);
			}
			else  {
				//server->bp = 1;
			}
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;													// sample timeout of 100 ms

		int n = select(server->maxsocket+1, &readfds, &writefds, &exceptfds, &timeout);

		if (n>0)  {
			// gets here if client connects or communicates
			//server->logg(true, "client at %d connected\r\n", 9);
			//bp = 3;
		}
		else if (n<-1)  {
			server->logg(server->log_to_console, "select error 1\r\n");
			perror("select 1");
			//break;
			goto finis;
		}
		else if (n==-1)  {
			server->logg(server->log_to_console, "select error 2\r\n");
			perror("select 2");
			//break;
			goto finis;
		}
		else if (n==0)  {					// select timeout

			for(int k=0; k<(int)server->tcpclients.size(); k++)  {
				server->tcpclients[k].flush();

			}

			// fall throug to same code for ip wakeups
			// no timeout will occur if the clients are sending us stuff
			// because the timeout keeps getting reset at a high rate.
			//logg(log_to_console, "timeout\r\n");

			//server->at->update();					// 100.000 ms

			/*
			if ((timeGetTime() - lastnow) >= 1000)  {
				server->logg(server->log_to_console, "tick\r\n");
				lastnow = timeGetTime();
			}
			*/
		}									// if (n==...

		#ifdef _DEBUG
		//at.update();						// 100.3 ms for linux, 109 ms for windows
		#endif

		DWORD now = timeGetTime();

		if (server->ip_discover)  {
			if ((now - server->last_ip_broadcast_time) >= 3000)  {
				//server->at->update();					// 3000.000 ms
				server->last_ip_broadcast_time = now;
				server->bcast_count++;
				int m, n2;
				m = server->sendStringLen;

				n2 = sendto(
							server->broadcast_socket,
							server->broadcast_message,
							server->sendStringLen,
							0,
							(struct sockaddr *) &server->broadcastAddr,
							sizeof(server->broadcastAddr)
					  );

				if (m != n2)  {
					server->logg(server->log_to_console, "sendto() sent a different number of bytes than expected");
					goto finis;
				}

				//server->logg(server->log_to_console, "%s, %d, %d, %s\r\n", server->broadcast_message, server->broadcast_socket, server->listen_socket, server->sp?server->sp->getname():"null");
				server->logg(server->log_to_console, "%d %s\r\n", server->bcast_count, server->broadcast_message);		// broadcasts "RacerMate:9072"
			}
			/*
			if ((now - server->last_ip_broadcast_time) >= 1000)  {
				server->last_ip_broadcast_time = now;
				//server->logg(server->log_to_console, "%d, %d, %d, %s, ct=%s\r\n", server->broadcast_socket, server->listen_socket, server->maxsocket, sp?sp->getname():"null", ct_connected?"true":"false");
			}
			*/
		}						// if (server->ip_discover)

		//if (n==0)  {			// if timeout occurred, then there is nothing happening
		//	server->at->update();					// 100.000 ms
		//	continue;
		//}

		//------------------------------------	
		// check server socket
		//------------------------------------	

		if (FD_ISSET(server->tcp_listen_socket, &readfds))  {
			// a new client is connecting

			int len_inet = sizeof(server->adr_clnt);
			#ifdef WIN32
				csock = accept( server->tcp_listen_socket, (struct sockaddr *)&server->adr_clnt, (int FAR *)&len_inet);
			#else
				csock = accept( server->listen_socket, (struct sockaddr *)&server->adr_clnt, (socklen_t*)&len_inet);
			#endif

			if (csock==-1)  {
				server->logg(server->log_to_console, "error: accept(2)\r\n");
				//bail("accept(2)");
			}
			server->maxsocket = MAX(server->maxsocket, csock);

			// set client socket to non blocking:
#ifdef WIN32
#else
			int x;
			x = fcntl(csock ,F_GETFL, 0);
			fcntl(csock, F_SETFL, x | O_NONBLOCK);
			x = fcntl(csock ,F_GETFL, 0);
#endif

			CLIENT2 tclient;
			memset(&tclient, 0, sizeof(CLIENT2));
			
			tclient.socket = csock;
			tclient.ipaddr = server->adr_clnt.sin_addr.s_addr;
			tclient.id = server->tcpclients.size();
			tclient.comport = tclient.id + 201;

			char *cptr = inet_ntoa(server->adr_clnt.sin_addr);
			strncpy(tclient.ipstr, cptr, sizeof(tclient.ipstr)-1);

			server->logg(server->log_to_console, "client %d at %s connected, s = %d, ms = %d, ls = %d\r\n",
				tclient.id,
				tclient.ipstr,
				tclient.socket,
				server->maxsocket,
				server->tcp_listen_socket);

			int ix = server->tcpclients.size();
			server->tcpclients.push_back(tclient);
			assert(csock == server->tcpclients[ix].socket);
			//server->bp = 2;
		}											// if (FD_ISSET(server->listen_socket, &server->readfds))  {

		if (FD_ISSET(server->tcp_listen_socket, &writefds))  {
			server->logg(server->log_to_console, "server write\r\n");
		}
		if (FD_ISSET(server->tcp_listen_socket, &exceptfds))  {
			server->logg(server->log_to_console,"server except\r\n");
		}

		//------------------------------------	
		// check client sockets
		//------------------------------------	

		bool atleastonesocketclosed = false;

		for(int k=0; k<(int)server->tcpclients.size(); k++)  {

			if (FD_ISSET(server->tcpclients[k].socket, &readfds))  {
				n = server->tcpclients[k].receive();
				if (n==0)  {			// the client is actively closing the socket
					server->logg(server->log_to_console, "client %d closing, s = %d closing, ms = %d, ls = %d\r\n",
						server->tcpclients[k].id,
						server->tcpclients[k].socket,
						server->maxsocket,
						server->tcp_listen_socket
					);
					#ifdef WIN32
						closesocket(server->tcpclients[k].socket);
					#else
						int status = close(server->clients[k].socket);
					#endif
					server->tcpclients[k].socket = INVALID_SOCKET;
					atleastonesocketclosed = true;
				}
				else if (n == SOCKET_ERROR )  {
					server->logg(server->log_to_console, "client %d on socket %d closing, maxsocket = %d\r\n",
						server->tcpclients[k].id,
						server->tcpclients[k].socket,
						server->maxsocket
					);
					#ifdef WIN32
						closesocket(server->tcpclients[k].socket);
					#else
						close(server->clients[k].socket);
					#endif
					server->tcpclients[k].socket = INVALID_SOCKET;
					atleastonesocketclosed = true;
				}
				else  {
					//server->bp = 1;
				}

			}						// if (FD_ISSET(server->clients[k].n, &server->readfds))  {
			else  {
				server->tcpclients[k].idle();
			}


			if ( (server->tcpclients[k].socket != INVALID_SOCKET) && FD_ISSET(server->tcpclients[k].socket, &writefds))  {
				server->logg(server->log_to_console, "client write\r\n");
			}
			if ( (server->tcpclients[k].socket != INVALID_SOCKET) && FD_ISSET(server->tcpclients[k].socket, &exceptfds))  {
				server->logg(server->log_to_console, "client except\r\n");
			}

			if (atleastonesocketclosed)  {									// if any client closed
				server->pack();
			}

		}											// for(k)
	}												// while(server->contin)


finis:
	server->thread_running = false;
	printf("threadx\r\n");
	return;
}										// tcpthread()

/**********************************************************************

**********************************************************************/

int Server::send(int _ix, const unsigned char *_str, int _len, bool _flush)  {
	//int i=0;
	int ix;
	int n;
	//unsigned char c;


	ix = _ix + 1 - SERVER_SERIAL_PORT_BASE;
	n = (int)tcpclients.size();

	if (ix >= n)  {
		return 0;
	}

	tcpclients[ix].mysend(_str, _len, _flush);


	return 0;
}											// enqueue()

/**********************************************************************************************************************************

**********************************************************************************************************************************/

void Server::udpthread(void *tmp) {
	int bp = 0;
	int k;
#define BUFLEN 256
	char buf[BUFLEN];
	struct timeval timeout;						// timeout value
	SOCKADDR_IN        SenderAddr;
	int                SenderAddrSize = sizeof(SenderAddr);
	fd_set readfds;								// read set
	fd_set writefds;								// write set
	fd_set exceptfds;								// exception set
	int nfdsets;
	FD_ZERO(&writefds);
	FD_ZERO(&readfds);
	FD_ZERO(&exceptfds);


	AverageTimer at("Server::udpthread");

	Server *server = static_cast<Server *>(tmp);
	server->logg(true, "\r\nlistening for connections on port %d\r\n\r\n", server->listen_port);


	server->thread_running = true;

	while (server->contin) {
		FD_ZERO(&readfds);
		//FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

		FD_SET(server->udp_socket, &readfds);
		//FD_SET(server->udp_socket, &writefds);
		FD_SET(server->udp_socket, &exceptfds);

		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;													// sample timeout of 10 ms

		//nfdsets = select(server->maxsocket + 1, &readfds, &writefds, &exceptfds, &timeout);			// parameter 1 ignored
		//nfdsets = select(0, &readfds, &writefds, &exceptfds, &timeout);									// parameter 1 ignored
//nfdsets = select(0, &readfds, &writefds, &exceptfds, &timeout);									// parameter 1 ignored
nfdsets = select(0, &readfds, &writefds, &exceptfds, NULL);									// no timeouts
		//at.update();				// .0017 ms with writefds set????, 10 ms if writefds not set

		if (nfdsets<=-1) {
			server->logg(server->log_to_console, "select error %d\n", nfdsets);
			perror("select error");
			goto finis;
		}

		if (nfdsets>0) {
			// gets here if any of the fds fire
			//at.update();				// .0017 ms

			if (FD_ISSET(server->udp_socket, &readfds)) {
				#ifdef _DEBUG
					bp = 1;
					//at.update();				// 3000 ms
				#endif

				//server->udpclients[0]->readDatagram(udp_server_socket);

				int recv_len = recvfrom(
						server->udp_socket,
						buf,
						BUFLEN,
						0,
						(SOCKADDR *)&SenderAddr,
						&SenderAddrSize
					);
	
				getpeername(server->udp_socket, (SOCKADDR *)&SenderAddr, &SenderAddrSize);
				char *peeraddr = inet_ntoa(SenderAddr.sin_addr);		// "50.194.61.75"
				int fromport = htons(SenderAddr.sin_port);						// 9072
				//dump((unsigned char *)buf, recv_len);
				if (server->udpclients[0]) {
					server->udpclients[0]->readDatagram(server->udp_socket, &SenderAddr, (unsigned char *)buf, recv_len);				// stuff this client's rxq
				}

			}											// if (FD_ISSET(server->udp_socket, &server->readfds))  {

			if (FD_ISSET(server->udp_socket, &writefds))  {
				bp = 1;
			}

			if (FD_ISSET(server->udp_socket, &exceptfds)) {
				bp = 2;
			}
		}												// if nfds > 0
		else  {					// must be a select timeout
			//at.update();											// 10.0 ms
		}

		//at.update();				// 3000 ms


		if (server->ip_discover) {
			DWORD now = timeGetTime();
			if ((now - server->last_ip_broadcast_time) >= 2000) {
				//server->at->update();					// 3000.000 ms
				server->last_ip_broadcast_time = now;
				server->bcast_count++;
				int m, n2;
				m = server->sendStringLen;

				n2 = sendto(
					server->broadcast_socket,
					server->broadcast_message,
					server->sendStringLen,
					0,
					(struct sockaddr *) &server->broadcastAddr,
					sizeof(server->broadcastAddr)
					);

				if (m != n2) {
					server->logg(server->log_to_console, "sendto() sent a different number of bytes than expected");
					goto finis;
				}

				//server->logg(server->log_to_console, "%s, %d, %d, %s\r\n", server->broadcast_message, server->broadcast_socket, server->listen_socket, server->sp?server->sp->getname():"null");
				server->logg(server->log_to_console, "%d %s\n", server->bcast_count, server->broadcast_message);		// broadcasts "RacerMate:9072"
			}
			/*
			if ((now - server->last_ip_broadcast_time) >= 1000)  {
			server->last_ip_broadcast_time = now;
			//server->logg(server->log_to_console, "%d, %d, %d, %s, ct=%s\r\n", server->broadcast_socket, server->listen_socket, server->maxsocket, sp?sp->getname():"null", ct_connected?"true":"false");
			}
			*/
		}						// if (server->ip_discover)

								//if (n==0)  {			// if timeout occurred, then there is nothing happening
								//	server->at->update();					// 100.000 ms
								//	continue;
								//}

								//------------------------------------	
								// check server socket
								//------------------------------------	



		//------------------------------------	
		// check client sockets
		//------------------------------------	

		//bool atleastonesocketclosed = false;

		for (int k = 0; k<MAXUDPCLIENTS; k++) {

#if 0
			if (FD_ISSET(server->udpclients[k].socket, &server->readfds)) {
				n = server->udpclients[k].receive();
				if (n == 0) {			// the client is actively closing the socket
					server->logg(server->log_to_console, "client %d closing, s = %d closing, ms = %d, ls = %d\r\n",
						server->udpclients[k].id,
						server->udpclients[k].socket,
						server->maxsocket,
						server->listen_socket
						);
#ifdef WIN32
					closesocket(server->udpclients[k].socket);
#else
					int status = close(server->udpclients[k].socket);
#endif
					server->udpclients[k].socket = INVALID_SOCKET;
					atleastonesocketclosed = true;
				}
				else if (n == SOCKET_ERROR) {
					server->logg(server->log_to_console, "client %d on socket %d closing, maxsocket = %d\r\n",
						server->udpclients[k].id,
						server->udpclients[k].socket,
						server->maxsocket
						);
#ifdef WIN32
					closesocket(server->udpclients[k].socket);
#else
					close(server->udpclients[k].socket);
#endif
					server->udpclients[k].socket = INVALID_SOCKET;
					atleastonesocketclosed = true;
				}
				else {
					//server->bp = 1;
				}
			}						// if (FD_ISSET(server->udpclients[k].n, &server->readfds))  {
			else {
				server->udpclients[k].idle();
			}
#endif

#if 0
			if ((server->udpclients[k].socket != INVALID_SOCKET) && FD_ISSET(server->udpclients[k].socket, &server->writefds)) {
				//server->logg(server->log_to_console, "client write\r\n");
			}
			if ((server->udpclients[k].socket != INVALID_SOCKET) && FD_ISSET(server->udpclients[k].socket, &server->exceptfds)) {
				///server->logg(server->log_to_console, "client except\r\n");
			}
#endif

			//if (atleastonesocketclosed) {									// if any client closed
			//	server->pack();
			//}

		}											// for(k)

		//Sleep(10);
	}												// while(server->contin)


finis:
	server->thread_running = false;
	printf("threadx\r\n");
	return;
	//at.update();				// .002 ms = 2 microseconds! bad!
}										// udpthread()

/**********************************************************************************************************************************

**********************************************************************************************************************************/

#ifdef WIN32
	//#pragma push
	#pragma warning(disable:4715)
#endif

DEVICE Server::get_device(int _ix) {
	if (udpclients[_ix])  {
		return udpclients[_ix]->get_device();
	}

	assert(false);
}

#ifdef WIN32
	//#pragma warning(enable:4715)
#endif

/**********************************************************************************************************************************

**********************************************************************************************************************************/
int Server::getPortNumber(void) {
	return 221;
}

/**********************************************************************************************************************************

**********************************************************************************************************************************/

int Server::reset_client(int _ix) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		udpclients[ix]->reset();
		return 0;
	}
	return 1;
}

/*********************************************************************************

*********************************************************************************/

//int Server::set_hr_bounds(int _ix)  {
int Server::set_hr_bounds(int _ix, int _minhr, int _maxhr, bool _beepEnabled)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		udpclients[ix]->set_hr_bounds(_minhr, _maxhr, _beepEnabled);
		return 0;
	}
	return 1;
}

/*********************************************************************************

*********************************************************************************/

int Server::set_ftp(int _ix, float _ftp)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->set_ftp(_ftp);
		return 0;
	}
	return 1;
}

/*********************************************************************************

*********************************************************************************/

int Server::reset_averages(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->reset_stats();
		return 0;
	}
	return 1;
}

/*********************************************************************************

*********************************************************************************/

int Server::set_slope(int _ix, float _bike_kgs, float _person_kgs, int _drag_factor, float _grade)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->set_slope(_bike_kgs, _person_kgs, _drag_factor, _grade);
		return 0;
	}
	return 1;
}

/*********************************************************************************

*********************************************************************************/

bool Server::is_paused(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		bool b = false;
		b = udpclients[ix]->is_paused();
		return b;
	}
	return false;
}

/*********************************************************************************

*********************************************************************************/

int Server::set_paused(int _ix, bool _paused)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		udpclients[ix]->set_paused(_paused);
		return 0;
	}

	return 1;
}

/*********************************************************************************

*********************************************************************************/

float Server::get_ftp(int _ix) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		float f;
		f = udpclients[ix]->get_ftp();
		return f;
	}
	return -1.0f;
}

/*********************************************************************************

*********************************************************************************/

void Server::set_export(int _ix, const char * _dbgfname)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		udpclients[ix]->set_export(_dbgfname);
	}
	return;
}

/*********************************************************************************

*********************************************************************************/

void Server::set_file_mode(int _ix, bool _mode)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		udpclients[ix]->set_file_mode(_mode);
	}
}

/*********************************************************************************

*********************************************************************************/

float Server::get_watts_factor(int _ix) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		float f;
		f = udpclients[ix]->get_watts_factor();
		return f;
	}
	return 0.0f;
}

/*********************************************************************************

*********************************************************************************/

bool Server::client_is_running(int _ix)  {

	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		return true;
	}
	return false;
}

/*********************************************************************************

*********************************************************************************/

TrainerData Server::GetTrainerData(int _ix, int _fw)  {

	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		td = udpclients[ix]->GetTrainerData(_fw);
	}
	else  {
		memset(&td, 0, sizeof(td));
	}

	return td;
}

/*********************************************************************************

*********************************************************************************/

int Server::get_handlebar_buttons(int _ix)  {
	int i;
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	if (udpclients[ix]) {
		i = udpclients[ix]->get_handlebar_buttons();
	}
	else {
		i = 0;
	}
	return i;
}

/*********************************************************************************

*********************************************************************************/

float *Server::get_bars(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		return udpclients[ix]->get_bars();
	}

	return NULL;
}

/*********************************************************************************

*********************************************************************************/

float * Server::get_average_bars(int _ix) {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix])  {
		return udpclients[ix]->get_average_bars();
	}

	return nullptr;
}

/*********************************************************************************

*********************************************************************************/

SSDATA Server::get_ss_data(int _ix)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		return udpclients[ix]->get_ss_data();
	}

	SSDATA ssd;
	memset(&ssd, 0, sizeof(ssd));
	return ssd;
}

/*********************************************************************************

*********************************************************************************/

int Server::set_ergo_mode(int _ix, int _fw, int _rrc, float _manwatts)  {
	int ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;
	int status;


	if (udpclients[ix]) {
		status = udpclients[ix]->set_ergo_mode(_fw, _rrc, _manwatts);
	}

	return status;
}

/*********************************************************************************

*********************************************************************************/

int Server::start_cal(int _ix)  {
	int ix, status;

	ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		status = udpclients[ix]->start_cal();
	}

	return status;
}

/*********************************************************************************

*********************************************************************************/

int Server::end_cal(int _ix)  {
	int ix, status;

	ix = _ix - UDP_SERVER_SERIAL_PORT_BASE + 1;

	if (udpclients[ix]) {
		status = udpclients[ix]->end_cal();
	}

	return status;

}

