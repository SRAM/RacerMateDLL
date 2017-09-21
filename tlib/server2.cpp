
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
	#endif
	#include <minmax.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
#endif						// #ifdef WIN32

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

	status = init();
	if (status != 0)  {
		//initialized = false;
		throw 100;
		//return;
	}

	//initialized = true;

}								// constructor

/**********************************************************************

**********************************************************************/

Server::Server(int _broadcast_port, int _listen_port, bool _ip_discover, bool _udp)  {
	int status;

	broadcast_port = _broadcast_port;
	listen_port = _listen_port;
	ip_discover = _ip_discover;
	udp = _udp;

	status = init();
	if (status != 0)  {
		throw 100;
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
	bp = 0;
	instances--;

}

/**********************************************************************

**********************************************************************/

int Server::init(void)  {
	int status;
	unsigned char bcaddr[4] = {   0,   0,   0,  0 };
	unsigned char mask[4] =   { 255, 255, 255,  0 };
	int i;
	char str[256];

	log_to_console = true;


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

	for(i=0; i<(int)clients.size(); i++)  {
		memset(&clients[i], 0, sizeof(CLIENT2));
		clients[i].socket = INVALID_SOCKET;
	}

	contin = false;
	logstream = fopen("server.log", "wt");

	thrd = NULL;
	last_ip_broadcast_time = 0L;
	maxsocket = -1;
	listen_socket = INVALID_SOCKET;										// server socket
	broadcast_socket = INVALID_SOCKET;

	bp = 0;
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

	sprintf(broadcast_message, "RacerMate:%d", listen_port);

	if (ip_discover)  {
		status =  create_broadcast_socket();
		if (status != 0)  {
			logg(log_to_console, "create broadcast socket failed");
			return 1;
		}
	}											// if (ip_discover)  {



	status = create_listen_socket();												// creates the server socket and starts listening.
	if (status != 0)  {
		return 1;
	}

	at = new AverageTimer("server");

	contin = true;
	thrd = new tthread::thread(&mythread, this);

	// wait 300 ms for a potential client to connect. The client should be trying to connect every 100 ms.

	Sleep(300);
	instances++;

	return 0;
}															// init()


/**********************************************************************

**********************************************************************/

void Server::destroy(void)  {
	//int status;

	close();

	return;
}															// destroy()

/**********************************************************************
	server rx
**********************************************************************/

int Server::rx(int _ix, char *buf, int buflen)  {

	if (_ix >= (int)clients.size())  {
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
		n = rx(_ix, (char *)c, 1);

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


int Server::close(void) {
	unsigned long start;
	int rc = 0;

	contin = FALSE;

	start = timeGetTime();


	while(thread_running)  {
		if ((timeGetTime() - start) >= 2000)  {
			rc = 1;
			break;
		}
		bp++;
	}
	bp = 1;

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
#endif
		broadcast_socket = INVALID_SOCKET;
	}

	if (listen_socket != INVALID_SOCKET)  {
#ifdef WIN32
		closesocket(listen_socket);
#endif
		listen_socket = INVALID_SOCKET;
	}

	DEL(at);

	FCLOSE(logstream);
	return rc;

}									// close()

/***********************************************************************************

***********************************************************************************/

int Server::create_listen_socket(void)  {
	int len_inet;
	struct sockaddr_in server_address;			// AF_INET
	int n;
	int status;

	len_inet = sizeof(server_address);
	memset((void *)&server_address, 0, sizeof(server_address) );
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(listen_port);										// 9072
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_socket = socket(PF_INET, SOCK_STREAM, 0);

	if (listen_socket==-1)  {
		//bail("socket(2)");
		logg(log_to_console, "socket error\r\n");
		return 1;
	}

	// so that we can re-bind to it without TIME_WAIT problems:

	n = 1;

	if(setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&n,	sizeof(int))<0)  {
		//perror("SETSOCKOPT");
		//exit(EXIT_FAILURE);
		logg(log_to_console, "setsockopt error\r\n");
		return 2;
	} 

	status = bind(listen_socket, (struct sockaddr *)&server_address, len_inet);
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
				bp = 0;
				break;
			case WSANOTINITIALISED:
				bp = 2;
				break;
			case WSAENETDOWN:
				bp = 2;
				break;
			case WSAEACCES:
				bp = 2;
				break;
			case WSAEADDRINUSE:
				bp = 2;															// gets here for TIME_WAIT problem
				break;
			case WSAEADDRNOTAVAIL:
				bp = 2;
				break;
			case WSAEFAULT:
				bp = 2;
				break;
			case WSAEINPROGRESS:
				bp = 2;
				break;
			case WSAEINVAL:
				bp = 2;
				break;
			case WSAENOBUFS:
				bp = 2;
				break;
			case WSAENOTSOCK:
				bp = 2;
				break;
			default:
				bp = 1;
				break;
		}
#endif		// #ifdef WIN32
		logg(log_to_console, "bind error\r\n");
		return 3;
	}

	status = listen(listen_socket, BACKLOG);
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

	maxsocket = listen_socket;

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

	if (_print) printf("%s", s);


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
	broadcastAddr.sin_port = htons(broadcast_port);							// Broadcast port

	sendStringLen = strlen(broadcast_message);  // Find length of sendString

	return 0;
}													// int Server::create_broadcast_socket(void) 

/**********************************************************************

**********************************************************************/

bool Server::is_client_connected(int _comport)  {
	int n;


	int ix = _comport - SERVER_SERIAL_PORT_BASE;

	if (clients.size() > 0)  {
		n = clients.size();
		if (ix==n-1)  {
			if (clients[ix].socket != INVALID_SOCKET)  {
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

	n = clients.size();

	for(i=0;i<n;i++)  {
		if (clients[i].socket!=INVALID_SOCKET)  {
			tc.push_back(clients[i]);
		}
	}

	clients.clear();
	n = tc.size();
	maxsocket = listen_socket;

	for(i=0;i<n;i++)  {
		clients.push_back(tc[i]);
		maxsocket = MAX(maxsocket, tc[i].socket);
	}

	if (clients.size()==0)  {
		logg(log_to_console, "all clients closed, maxsocket is now %d\r\n", maxsocket);
	}

	return;
}												// pack()

/**********************************************************************
	the server socket is already created and we are waiting for a client
	to connect.
**********************************************************************/

void Server::mythread(void *tmp)  {
	struct timeval timeout;						// timeout value
	int csock = INVALID_SOCKET;										// client socket
	AverageTimer at("server_thread");
	int k;


#ifdef _DEBUG
	static int calls = 0;
	calls++;
#endif

	Server *server = static_cast<Server *>(tmp);

	server->thread_running = true;

	int cnt = 0;

	server->logg(true, "\r\nlistening for connections on port %d\r\n\r\n", server->listen_port);



	while(server->contin)  {
		cnt++;

		FD_ZERO(&server->readfds);
		FD_ZERO(&server->writefds);
		FD_ZERO(&server->exceptfds);

		FD_SET(server->listen_socket, &server->readfds);
		FD_SET(server->listen_socket, &server->writefds);
		FD_SET(server->listen_socket, &server->exceptfds);

		for(k=0; k < (int)server->clients.size(); k++)  {
			if (server->clients[k].socket != INVALID_SOCKET)  {
				FD_SET(server->clients[k].socket, &server->readfds);
				//FD_SET(server->clients[k].n, &server->writefds);
				FD_SET(server->clients[k].socket, &server->exceptfds);
			}
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;													// sample timeout of 100 ms

		int n = select(server->maxsocket+1, &server->readfds, &server->writefds, &server->exceptfds, &timeout);

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

			for(int k=0; k<(int)server->clients.size(); k++)  {
				server->clients[k].flush();

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

		}									// select timeout

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

		if (n==0)  {			// if timeout occurred, then there is nothing happening
			server->at->update();					// 100.000 ms
			continue;
		}

		//------------------------------------	
		// check server socket
		//------------------------------------	

		if (FD_ISSET(server->listen_socket, &server->readfds))  {
			// a new client is connecting

			int len_inet = sizeof(server->adr_clnt);
			#ifdef WIN32
				csock = accept( server->listen_socket, (struct sockaddr *)&server->adr_clnt, (int FAR *)&len_inet);
			#else
				csock = accept( server->listen_socket, (struct sockaddr *)&server->adr_clnt, (socklen_t*)&len_inet);
			#endif

			if (csock==-1)  {
				server->logg(server->log_to_console, "error: accept(2)\r\n");
				//bail("accept(2)");
			}
			server->maxsocket = MAX(server->maxsocket, csock);

			CLIENT2 tclient;
			memset(&tclient, 0, sizeof(CLIENT2));
			
			tclient.socket = csock;
			tclient.ipaddr = server->adr_clnt.sin_addr.s_addr;
			tclient.id = server->clients.size();
			tclient.comport = tclient.id + 201;

			//memset(idle_packet, 0, sizeof(idle_packet));
			//tclient.idle_packet[0] = 0;
			//tclient.idle_packet[1] = 0;
			//tclient.idle_packet[2] = 0;
			//tclient.idle_packet[3] = 0;
			//tclient.idle_packet[4] = 0;
			//tclient.idle_packet[5] = 0;
			//tclient.idle_packet[6] = 0x80;

#ifndef WIN32
			/*
			server->clients[k].rx = fdopen(client, "rb");
			if (server->clients[k].rx==NULL)  {
				logg(log_to_console,"tclients[k].rx = NULL\r\n");
				cleanup();
				return 1;
			}

			server->clients[k].tx = fdopen(dup(client), "wb");
			if (server->clients[k].tx==NULL)  {
				logg(log_to_console,"tclients[k].tx = NULL\r\n");
				cleanup();
				return 1;
			}
			*/
#endif
			char *cptr = inet_ntoa(server->adr_clnt.sin_addr);
			strncpy(tclient.ipstr, cptr, sizeof(tclient.ipstr)-1);

			server->logg(server->log_to_console, "client %d at %s connected, s = %d, ms = %d, ls = %d\r\n",
				tclient.id,
				tclient.ipstr,
				tclient.socket,
				server->maxsocket,
				server->listen_socket);

#ifdef _DEBUG
			if (server->clients.size()==1)  {
				//bp = 1;
			}
#endif
			server->clients.push_back(tclient);

#ifdef _DEBUG
			server->bp = 1;
#endif

#ifndef WIN32
			/*
			setbuffer(clients[k].rx, (char *)inbuf, sizeof(inbuf));
			setbuffer(clients[k].tx, (char *)&clients[k].txbuf, sizeof(clients[k].txbuf));
			*/
#endif
			//reset();
		}											// if (FD_ISSET(server->listen_socket, &server->readfds))  {

		if (FD_ISSET(server->listen_socket, &server->writefds))  {
			server->logg(server->log_to_console, "server write\r\n");
		}
		if (FD_ISSET(server->listen_socket, &server->exceptfds))  {
			server->logg(server->log_to_console,"server except\r\n");
		}

		//------------------------------------	
		// check client sockets
		//------------------------------------	

		//server->nclients = server->client.size();

		bool closed = false;

		for(int k=0; k<(int)server->clients.size(); k++)  {

			if (FD_ISSET(server->clients[k].socket, &server->readfds))  {

				//memset(server->clients[k].rxq, 0, sizeof(server->clients[k].rxq));
				//n = recv(server->clients[k].n, (char *)server->clients[k].rxq, sizeof(server->clients[k].rxq)-1, 0);   // get the message
				n = server->clients[k].receive();

				if (n==0)  {			// the client is actively closing the socket
					//xxx
					server->logg(server->log_to_console, "client %d closing, s = %d closing, ms = %d, ls = %d\r\n",
						server->clients[k].id,
						server->clients[k].socket,
						server->maxsocket,
						server->listen_socket
					);
#ifdef WIN32
					closesocket(server->clients[k].socket);
#endif
					server->clients[k].socket = INVALID_SOCKET;
					closed = true;
					//server->pack();
					//bp = 1;

	#ifndef WIN32
					/*
					fclose(clients[k].rx);
					fclose(clients[k].tx);
					*/
	#endif

					/*
					if (ip_discover)  {
						status =  create_broadcast_socket();
						if (status != 0)  {
							logg(log_to_console, "create broadcast socket failed");
							goto finis;
							//return 1;
						}
					}
					*/

					//break;
				}
				else if (n == SOCKET_ERROR )  {
					//xxx
					server->logg(server->log_to_console, "client %d on socket %d closing, maxsocket = %d\r\n",
						server->clients[k].id,
						server->clients[k].socket,
						server->maxsocket
					);
#ifdef WIN32
					closesocket(server->clients[k].socket);
#endif
					server->clients[k].socket = INVALID_SOCKET;
					closed = true;
					//bp = 1;
				}
				else  {
					//bp = 1;
				}

			}						// if (FD_ISSET(server->clients[k].n, &server->readfds))  {
			else  {
				server->clients[k].idle();
			}


			if ( (server->clients[k].socket != INVALID_SOCKET) && FD_ISSET(server->clients[k].socket, &server->writefds))  {
				server->logg(server->log_to_console, "client write\r\n");
			}
			if ( (server->clients[k].socket != INVALID_SOCKET) && FD_ISSET(server->clients[k].socket, &server->exceptfds))  {
				server->logg(server->log_to_console, "client except\r\n");
			}

			if (closed)  {									// if any client closed
				server->pack();
			}

		}											// for(k)
//#endif
	}												// while(server->contin)


finis:
#ifdef _DEBUG
	//static int calls = 0;
	calls++;
	if (calls == 2)  {
		//int bp = 1;
	}
#endif

	server->thread_running = false;
	printf("threadx\r\n");

	// client

	return;
}										// mythread()

/**********************************************************************

**********************************************************************/

int Server::send(int _ix, const unsigned char *_str, int _len, bool _flush)  {
	//int i=0;
	int ix;
	int n;
	//unsigned char c;


	ix = _ix + 1 - SERVER_SERIAL_PORT_BASE;
	n = (int)clients.size();

	if (ix >= n)  {
		return 0;
	}

	clients[ix].mysend(_str, _len, _flush);


	return 0;
}											// enqueue()

