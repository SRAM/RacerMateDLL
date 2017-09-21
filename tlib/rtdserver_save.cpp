
#define WIN32_LEAN_AND_MEAN


#include <WinSock2.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <commctrl.h>
//#include <winsock2.h>
#include <objbase.h>

#include <glib_defines.h>
#include <utils.h>
#include <averagetimer.h>
#include <rtdserver.h>

//int RTDServer::instances = 0;

/**********************************************************************

**********************************************************************/

RTDServer::RTDServer(void)  {
	int status;

	// should never get here

	throw("rtdserver created");

	server_port = 9072;

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

RTDServer::RTDServer(int _port)  {
	int status;

	server_port = _port;

	status = init();
	if (status != 0)  {
		throw 100;
	}
}


/**********************************************************************
	destructor
**********************************************************************/

RTDServer::~RTDServer()  {

#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 2)  {
		bp = 1;
	}
#endif

	destroy();
	bp = 0;
}

/**********************************************************************

**********************************************************************/

int RTDServer::init(void)  {
	int status;
	char broadcast_address[32];
	unsigned char bcaddr[4] = {   0,   0,   0,  0 };
	unsigned char mask[4] =   { 255, 255, 255,  0 };
	int i;
	char str[256];

	memset(rxq, 0, sizeof(rxq));
	rxdiff = 0;
	rxinptr = 0;
	rxoutptr = 0;

	/*
	memset(txq, 0, sizeof(txq));
	txdiff = 0;
	txinptr = 0;
	txoutptr = 0;
	*/

#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 2)  {
		bp = 1;
	}
#endif

	//unsigned int sendStringLen;   		// length of string to broadcast
	//char *sendString;                 	// string to broadcast
	//char myip[64];

	broadcast_port=9073;     	// server port

	broadcastIP=NULL;                	// ip broadcast address
	memset(myaddr, 0, sizeof(myaddr));
	memset(broadcast_message, 0, sizeof(broadcast_message));

	sprintf(broadcast_message, "Racermate", server_port);
	sendString = broadcast_message;

	bcast_count = 0L;
	ip_discover = false;
	errors = 0;
	memset(&client, 0, sizeof(client));
	client.n = INVALID_SOCKET;

	contin = false;
	//initialized = false;
	//logstream = NULL;
	logstream = fopen("server.log", "wt");

	thrd = NULL;
	//xclient = INVALID_SOCKET;										// client socket
	last_ip_broadcast_time = 0L;
	maxsocket = -1;
	//server_port = 9072;
	server_socket = INVALID_SOCKET;										// server socket
	broadcast_socket = INVALID_SOCKET;

	bp = 0;
	thread_running = false;
	//instances++;

#ifdef WIN32
	WSADATA wsaData;
	//struct sockaddr_in server_address;			// AF_INET
	INITCOMMONCONTROLSEX iccex;

	if( FAILED(CoInitialize(NULL)) )  {
		MessageBox( NULL, "CoInitialize", "Error", MB_OK | MB_ICONINFORMATION );
		return 1;
	}

	memset(&iccex, 0, sizeof(iccex));
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);
	if ( (status=WSAStartup(0x0202, &wsaData)) != 0 )  {
		//logg(true, "WSAStartup error, status = %d\n", status);
		return 1;
	}
	if (wsaData.wVersion != 0x0202)  {		// wrong WinSock version!
		WSACleanup ();						// unload ws2_32.dll
		//logg(true, "wrong winsock version\n");
		return 1;
	}
	//logg(true, "WSAStartup status = %d\n", status);
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
	logg(true, "myip = %s\n", myip);

	if (ip_discover)  {
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

		logg(true, "broadcast address = %s\n", broadcast_address);

		broadcastIP = broadcast_address;	

		sprintf(broadcast_message, "RacerMate:%d", server_port);
		sendString = broadcast_message;

		status =  create_broadcast_socket();
		if (status != 0)  {
			logg(true, "create broadcast socket failed");
			return 1;
		}
	}											// if (ip_discover)  {



	status = create_server_socket();												// creates the server socket and starts listening.
	if (status != 0)  {
		return 1;
	}


	contin = true;
	thrd = new tthread::thread(&mythread, this);
	//thrd = (HANDLE) _beginthread(RTDServer::thread, 20000, this);

	// wait 300 ms for a potential client to connect. The client should be trying to connect every 100 ms.

	Sleep(300);

	return 0;
}															// init()


/**********************************************************************

**********************************************************************/

void RTDServer::destroy(void)  {
	int status;

	status = close();

	return;
}															// destroy()

/**********************************************************************

**********************************************************************/

void RTDServer::tx(const char *str, int _flush_flag)  {
	int status, n;

	if (client.n == INVALID_SOCKET)  {
		return;
	}

	n = strlen(str);

#ifdef WIN32
	status = send(client.n, str, n, 0);
#else
	status = send(clients[i].n, (const void *)outbuf, n, 0);
#endif
	switch(status)  {
		case -1:
			logg(true, "\nsend error, len = %d:\noutbuf: %s", n, str);
			perror("send error");
			break;

		default:
			if (status==n)  {
				client.outpackets++;
				client.bytes_out += (unsigned long)strlen((const char *)str);
			}
			else  {
				if (errors<3)  {
					logg("sent: %s\n", str);
					errors++;
				}
			}
			break;
	}

	return;
}													// void tx()

/**********************************************************************

**********************************************************************/

int RTDServer::rx(char *buf, int buflen)  {
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

	return i;

}

/**********************************************************************

**********************************************************************/

int RTDServer::expect(const char *str, DWORD timeout)  {
	DWORD start;

	start = timeGetTime();

	while(1)  {
		if ((timeGetTime() - start) >= timeout)  {
			break;
		}

		if (!strcmp(str, (const char *)client.rxbuf))  {
			return 0;
		}
	}

	bp = 5;
	return 1;
}

/**********************************************************************

**********************************************************************/

void RTDServer::flush(void)  {

	return;
}

#ifdef _DEBUG
/**********************************************************************

**********************************************************************/

void RTDServer::flush_rawstream(void)  {
	return;
}
#endif

/**********************************************************************

**********************************************************************/

void RTDServer::txx(unsigned char *b, int n)  {
	return;
}


/**********************************************************************

**********************************************************************/


int RTDServer::close(void) {
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
	unsigned long dt = timeGetTime() - start;
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
		closesocket(broadcast_socket);
		broadcast_socket = INVALID_SOCKET;
	}

	if (server_socket != INVALID_SOCKET)  {
		closesocket(server_socket);
		server_socket = INVALID_SOCKET;
	}

	FCLOSE(logstream);
	return rc;

}									// close()

/***********************************************************************************

 ***********************************************************************************/

int RTDServer::create_server_socket(void)  {
	int len_inet;
	struct sockaddr_in server_address;			// AF_INET
	int n;
	int status;

	len_inet = sizeof(server_address);
	memset((void *)&server_address, 0, sizeof(server_address) );
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);										// 9072
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_socket = socket(PF_INET, SOCK_STREAM, 0);

	if (server_socket==-1)  {
		//bail("socket(2)");
		logg(true, "socket error\n");
		return 1;
	}

	// so that we can re-bind to it without TIME_WAIT problems:

	n = 1;

	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&n,	sizeof(int))<0)  {
		//perror("SETSOCKOPT");
		//exit(EXIT_FAILURE);
		logg(true, "setsockopt error\n");
		return 2;
	} 

	status = bind(server_socket, (struct sockaddr *)&server_address, len_inet);
	/*
	if (status==-1)  {
		// You use the same ip/port or you too quickly run the same server and
		// previous used ip/port is in the TIME_WAIT state.
		//bail("bind(2)");
		logg(true, "bind error\n");
		return 3;
	}
	*/

	if (status!=0)  {
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
		logg(true, "bind error\n");
		return 3;
	}

	status = listen(server_socket, BACKLOG);
	if (status==-1)  {
		//bail("listen(2)");
		logg(true, "listen error\n");
		return 4;
	}

	/*
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	FD_SET(server_socket, &readfds);
	FD_SET(server_socket, &writefds);
	FD_SET(server_socket, &exceptfds);

	maxsocket = server_socket;
	*/

	maxsocket = server_socket;

	return 0;
}														// int create_server_socket()


/**********************************************************************

 ***********************************************************************/

void RTDServer::logg(bool _print, const char *format, ...)  {
	va_list ap;													// Argument pointer
	char s[1024];												// Output string
	int len;



	len = (int)strlen(format);
	if (len>1023)  {
		if (_print) printf("\n(string too long in logg())\n");
		if (logstream)  {
			fprintf(logstream, "\n(string too long in logg())\n");
			//fflush(logstream);
		}
		return;
	}

	va_start(ap, format);
	vsprintf(s, format, ap);
	va_end(ap);

	if (_print) printf("%s", s);


//	if (!verbose)  {
//		return;
//	}

#ifdef WIN32
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

	syslog(LOG_INFO, "%s", s);					// syslog
#endif


	return;
}													// logg()

/**********************************************************************
	the server socket is already created and we are waiting for a client
	to connect.
**********************************************************************/

void RTDServer::mythread(void *tmp)  {
	struct timeval timeout;						// timeout value
	char str[256];
	unsigned long lastnow;
	int csock = INVALID_SOCKET;										// client socket
	int bp = 0;
	AverageTimer at("server_thread");


#ifdef _DEBUG
	static int calls = 0;
	calls++;
#endif

	RTDServer *server = static_cast<RTDServer *>(tmp);

	server->thread_running = true;

	int cnt = 0;
	lastnow = 0;


	while(server->contin)  {
		cnt++;

		FD_ZERO(&server->readfds);
		FD_ZERO(&server->writefds);
		FD_ZERO(&server->exceptfds);

		FD_SET(server->server_socket, &server->readfds);
		FD_SET(server->server_socket, &server->writefds);
		FD_SET(server->server_socket, &server->exceptfds);

		if (server->client.n != INVALID_SOCKET)  {
			FD_SET(server->client.n, &server->readfds);
			//FD_SET(server->client.n, &server->writefds);
			FD_SET(server->client.n, &server->exceptfds);
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;													// sample timeout of .1 seconds

		int n = select(server->maxsocket+1, &server->readfds, &server->writefds, &server->exceptfds, &timeout);

		if (n>0)  {
			// gets here if client connects or communicates
			bp = 3;
		}
		else if (n<-1)  {
			server->logg(true, "select error 1\n");
			perror("select 1");
			//break;
			goto finis;
		}
		else if (n==-1)  {
			server->logg(true, "select error 2\n");
			perror("select 2");
			//break;
			goto finis;
		}
		else if (n==0)  {					// timeout
			// fall throug to same code for ip wakeups
			// no timeout will occur if the clients are sending us stuff
			// because the timeout keeps getting reset at a high rate.
			//logg(true, "timeout\n");

			if ((timeGetTime() - lastnow) >= 1000)  {
				server->logg(true, "tick\n");
				lastnow = timeGetTime();
			}

		}

		#ifdef _DEBUG
			//at.update();						// 100.3 ms for linux, 109 ms for windows
		#endif

		DWORD now = timeGetTime();

		if (server->ip_discover)  {
			if ((now - server->last_ip_broadcast_time) >= 3000)  {
				server->last_ip_broadcast_time = now;
				server->bcast_count++;
				if (sendto(server->broadcast_socket, server->sendString, server->sendStringLen, 0, (struct sockaddr *) &server->broadcastAddr, sizeof(server->broadcastAddr)) != server->sendStringLen)  {
					server->logg(true, "sendto() sent a different number of bytes than expected");
					goto finis;
				}
				//logg(true, "%s, %d, %d, %s\n", broadcast_message, broadcast_socket, server_socket, sp?sp->getname():"null");
				server->logg(true, "%s\n", server->broadcast_message);
			}
		}
		else  {
			if ((now - server->last_ip_broadcast_time) >= 1000)  {
				server->last_ip_broadcast_time = now;
				//logg(true, "%d, %d, %d, %s, ct=%s\n", broadcast_socket, server_socket, maxsocket, sp?sp->getname():"null", ct_connected?"true":"false");
			}
		}

		/*
		if (sp)  {
			if (!sp->is_open())  {					// in case they unplugged the usb-serial adapter
				logg(true, "closing %s\n", sp->getname());
				sp->clse(0);
				DEL(sp);
				ct_connected = false;
			}
			#ifdef _DEBUG
			else  {
				bp = 0;
			}
			#endif
		}
		#ifdef _DEBUG
		else  {
			bp = 2;
		}
		#endif
		*/

		/*
		if (ct_connected)  {

#ifdef LARRY
			unsigned long ul = lastCommTime;
			unsigned long ul2 = timeGetTime();
			unsigned long diff = ul2 - ul;
			if (diff > 1500)  {
				bp = 2;
			}
#endif
			status = read_computrainer();		// always returns 0
			assert(status==0);

#ifdef SLUG
			if (baud==B19200)  {
				status = slug.updateHardware();
			}
			else  {
				updateHardware();
			}
#else
			updateHardware();
#endif
		}
		else  {
			if ((now-last_ct_check_time)>2000)  {
				last_ct_check_time = now;
				scan();
			}				// if ((now-last_ct_check_time)>2000)  {
		}					// if (ct_connected)  {
		*/

		now = timeGetTime();

		/*
		if ((now-last_broadcast_time)>250)  {
			last_broadcast_time = now;

			if (clients.size() > 0)  {
				status = broadcast();				// always returns 0
				//status = broadcast2();				// always returns 0
				assert(status==0);
			}
		}
		*/

		if (n==0)  {			// if timeout occurred, then there is no data to read
			continue;
		}

		//------------------------------------	
		// check server socket
		//------------------------------------	

		if (FD_ISSET(server->server_socket, &server->readfds))  {
			// a new client is connecting
			/*
			if (ip_discover)  {
				closesocket(broadcast_socket);
				bcast_count = 0L;
			}
			*/

			/*
			if (broadcast_socket != -1)  {
				close(broadcast_socket);
				broadcast_socket = -1;
			}
			*/

			int len_inet = sizeof(server->adr_clnt);
#ifdef WIN32
			csock = accept( server->server_socket, (struct sockaddr *)&server->adr_clnt, (int FAR *)&len_inet);
#else
			client = accept( server_socket, (struct sockaddr *)&adr_clnt, (socklen_t*)&len_inet);
#endif
			if (csock==-1)  {
				server->logg(true, "error: accept(2)\n");
				//bail("accept(2)");
			}

			server->maxsocket = MAX(server->maxsocket, csock);

			memset(&server->client, 0, sizeof(CLIENT));
			
			server->client.n = csock;
			server->client.ipaddr = server->adr_clnt.sin_addr.s_addr;

#ifndef WIN32
			client.rx = fdopen(client, "rb");
			if (client.rx==NULL)  {
				logg(true,"tclient.rx = NULL\n");
				cleanup();
				return 1;
			}

			client.tx = fdopen(dup(client), "wb");
			if (client.tx==NULL)  {
				logg(true,"tclient.tx = NULL\n");
				cleanup();
				return 1;
			}
#endif
			char *cptr = inet_ntoa(server->adr_clnt.sin_addr);
			strncpy(server->client.ipstr, cptr, sizeof(server->client.ipstr)-1);
			server->logg(true, "client at %s connected\n", server->client.ipstr);
			//clients.push_back(tclient);
			
			//i = clients.size()-1;

#ifndef WIN32
			setbuffer(client.rx, (char *)inbuf, sizeof(inbuf));
			setbuffer(client.tx, (char *)&client.txbuf, sizeof(client.txbuf));
#endif
			server->maxsocket = MAX(server->maxsocket, csock);
			//reset();

			timestamp(str);

			server->logg(true, "\n*******************************************************\n");
			server->logg(true, "\n%s: connected to client on socket %d, maxsocket = %d, ipaddr = %s\n", str, server->client.n, server->maxsocket, server->client.ipstr );
		}

		if (FD_ISSET(server->server_socket, &server->writefds))  {
			server->logg(true, "server write\n");
		}
		if (FD_ISSET(server->server_socket, &server->exceptfds))  {
			server->logg(true,"server except\n");
		}

		//------------------------------------	
		// check client socket
		//------------------------------------	

		if (FD_ISSET(server->client.n, &server->readfds))  {

			memset(server->client.rxbuf, 0, sizeof(server->client.rxbuf));
			n = recv(server->client.n, (char *)server->client.rxbuf, sizeof(server->client.rxbuf)-1, 0);   // get the message

			if (n>0)  {
				server->client.bytes_in += n;
			}			// if (n > 0)
			else if (n==0)  {			// the client is closing the socket
				int ii = server->client.n;
				//logg(true, "client at %s disconnected\n", clients[i].ipstr);
				closesocket(server->client.n);
				server->client.n = INVALID_SOCKET;

#ifndef WIN32
				fclose(client.rx);
				fclose(client.tx);
#endif

				/*
				if (ip_discover)  {
					status =  create_broadcast_socket();
					if (status != 0)  {
						logg(true, "create broadcast socket failed");
						goto finis;
						//return 1;
					}
				}
				*/

				timestamp(str);
				server->logg(true, "%s: client on socket %d closing, maxsocket = %d\n", str, ii, server->maxsocket);
				break;
			}
			else if (n == SOCKET_ERROR )  {
				bp = 1;
			}
		}						// if (FD_ISSET(server->client.n, &server->readfds))  {

		if (FD_ISSET(server->client.n, &server->writefds))  {
			server->logg(true, "client write\n");
		}
		if (FD_ISSET(server->client.n, &server->exceptfds))  {
			server->logg(true, "client except\n");
		}

//#endif
	}												// while(server->contin)


finis:
#ifdef _DEBUG
	//static int calls = 0;
	calls++;
	if (calls == 2)  {
		int bp = 1;
	}
#endif

	server->thread_running = false;
	printf("threadx\n");

	return;
}										// thread()

/****************************************************************************
	http://stackoverflow.com/questions/122208/get-the-ip-address-of-local-computer
	http://tangentsoft.net/wskfaq/examples/ipaddr.html
****************************************************************************/

int RTDServer::getmyip(void)  {
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
		printf("%d   %s", cnt, myip);

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiBroadcastAddress);
		printf("    %s", inet_ntoa(pAddress->sin_addr));

		pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
		printf("    %s    ", inet_ntoa(pAddress->sin_addr));

		printf("    ");

		nFlags = InterfaceList[i].iiFlags;
		if (nFlags & IFF_UP)   {
			printf("up");
		}
      else   {
			printf("down");
		}

		if (nFlags & IFF_POINTTOPOINT)   {
			printf(", point-to-point");
		}
		if (nFlags & IFF_LOOPBACK)  {
			printf(", loopback");
		}
		if (nFlags & IFF_BROADCAST)  {
			printf(", broadcast ");
		}
      if (nFlags & IFF_MULTICAST)  {
			printf(", multicast ");
		}
		printf("\n");

	}

	int n = ifaces.size();

	if (n <= 1)  {
		return 0;
	}

	// more than one interface, need to choose one?

	logg(true, "found more than one interface, using %s\n", myip);

	return 0;
}														// int RTDServer::getmyip(void)  {


/***********************************************************************************

***********************************************************************************/

int RTDServer::create_broadcast_socket(void)  {

	logg(true, "creating broadcast socket\n");

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

	memset(&broadcastAddr, 0, sizeof(broadcastAddr));   // Zero out structure
	broadcastAddr.sin_family = AF_INET;                 // Internet address family
	broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);// Broadcast IP address
	broadcastAddr.sin_port = htons(broadcast_port);         // Broadcast port

	sendStringLen = strlen(sendString);  // Find length of sendString

	return 0;
}													// int RTDServer::create_broadcast_socket(void) 

