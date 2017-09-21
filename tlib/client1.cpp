
#define WIN32_LEAN_AND_MEAN


#include <WinSock2.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <objbase.h>
#include <assert.h>


#include <glib_defines.h>
#include <utils.h>
#include <averagetimer.h>
#include <client.h>
#include <fatalerror.h>

/**********************************************************************

**********************************************************************/

/*
Client::Client(void)  {
	int status;

	tcp_port = -1;
	memset(url, 0, sizeof(url));
	//strcpy(url, "127.0.0.1");

	status = init();
	if (status != 0)  {
		throw status;
	}
}								// constructor
*/



/**********************************************************************

**********************************************************************/

Client::Client(const char *_url, int _port)  {
	int status;

	tcp_port = _port;

	memset(url, 0, sizeof(url));
	strncpy(url, _url, sizeof(url)-1);



	status = init();
	if (status != 0)  {
		throw status;
	}
}


/**********************************************************************
	destructor
**********************************************************************/

Client::~Client()  {

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

int Client::init(void)  {
	int status;
	int i;
	char str[256];
	//int len_inet;
	struct hostent *hp;
	char *cptr;

	//clsflg = false;
	memset(gstr, 0, sizeof(gstr));
	memset(rxq, 0, sizeof(rxq));
	rxdiff = 0;
	rxinptr = 0;
	rxoutptr = 0;
	connected_to_server = false;
	connected_to_trainer = false;

#ifdef _DEBUG
	static int calls = 0;
	calls++;
	if (calls == 2)  {
		bp = 1;
	}
#endif

	errors = 0;

	contin = false;
	logstream = fopen("server.log", "wt");

	thrd = NULL;
	maxsocket = -1;
	sock = INVALID_SOCKET;										// server socket

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
		return 1;
	}
	if (wsaData.wVersion != 0x0202)  {		// wrong WinSock version!
		WSACleanup ();						// unload ws2_32.dll
		return 2;
	}
#else
#endif					// #ifdef WIN32

#ifdef _DEBUG
	if (calls == 2)  {
		bp = 1;
	}
#endif

//strcpy(url, "127.0.0.1");
//strcpy(url, "192.168.1.200");

	if (url[0] != 0)  {						//xxx
		hp = gethostbyname(url);			// 68.115.225.206, 50.194.61.75
		if (hp)  {
			cptr = inet_ntoa(*(struct in_addr *)*hp->h_addr_list);
			strncpy(url, cptr, sizeof(url)-1);
		}
	}
#ifdef _DEBUG
	else  {
		bp = 2;
	}
#endif

	bp = 1;

/*
	len_inet = sizeof(sinaddr);
	memset((void *)&sinaddr, 0, sizeof(sinaddr) );
	sinaddr.sin_family = AF_INET;
	sinaddr.sin_port = htons(port);										// 9072
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sinaddr.sin_addr.s_addr = inet_addr (url);					// set server's IP
*/


#if 0
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock==INVALID_SOCKET)  {
		//bail("socket(2)");
		logg(true, "socket error\n");
		return 3;
	}
	/*
	// so that we can re-bind to it without TIME_WAIT problems:
	int n = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&n,	sizeof(int))<0)  {
		//perror("SETSOCKOPT");
		//exit(EXIT_FAILURE);
		logg(true, "setsockopt error\n");
		return 2;
	} 
	*/

	status = connect(sock, (struct sockaddr FAR *) &sinaddr, sizeof(struct sockaddr) );

	if (status==SOCKET_ERROR)  {
		status = WSAGetLastError();
		switch(status)  {
			case WSAECONNREFUSED:											// 10061
				sprintf(gstr, "Connection Refused");
				logg(true, "%s\n", gstr);
				closesocket(sock);
            sock = INVALID_SOCKET;
				//cleanup();
				//throw (fatalError(__FILE__, __LINE__));
				return 4;							// causes a throw 100

			case WSAEADDRNOTAVAIL:											// 10049
				sprintf(gstr, "Cannot assign requested address");
				logg(true, "%s\n", gstr);
				//cleanup();
				//throw (fatalError(__FILE__, __LINE__));
				closesocket(sock);
            sock = INVALID_SOCKET;
				return 5;							// causes a throw 101

			default:
				sprintf(gstr, "error: Could not connect(), status = %d\n", status);
				logg(true, gstr);
				//cleanup();
				//throw (fatalError(__FILE__, __LINE__));
				closesocket(sock);
            sock = INVALID_SOCKET;
				return 6;							// causes a throw 102
		}
	}

	//WSAAsyncSelect(sock, hwnd, msg, (FD_READ | FD_CONNECT | FD_CLOSE | FD_WRITE));

	maxsocket = sock;

	// don't start thread yet, let the creator do that (too much overhead for handshaking)

	bp = 9;

	/*
	status = connect(sock, (struct sockaddr FAR *) &sinaddr, sizeof(struct sockaddr) );

	if (status==SOCKET_ERROR)  {
		status = WSAGetLastError();
		switch(status)  {
			case WSAECONNREFUSED:							// 10061
				closesocket(sock);
				sock = INVALID_SOCKET;
				break;

			case WSAEADDRNOTAVAIL:							// 10049
				closesocket(sock);
				sock = INVALID_SOCKET;
				break;

			case WSAEISCONN:									// 10056
				closesocket(sock);
				sock = INVALID_SOCKET;
				break;

			default:
				closesocket(sock);
				sock = INVALID_SOCKET;
				break;
		}
		return status;
	}
	*/

	//----------------------------------
	// connected
	//----------------------------------
#endif



	//connected_to_server = true;

	/*
	status = send( sock, "RacerMate", (int)strlen("Racermate"), 0 );
	if (status == SOCKET_ERROR) {
		//printf("send failed with error: %d\n", WSAGetLastError());
      closesocket(sock);
      sock = INVALID_SOCKET;
      WSACleanup();
		return status;
	}

	// try to get a response for 1000 ms

	unsigned long now = timeGetTime();
	int n;

	while(1)  {
		memset(str, 0, sizeof(str));
		n = recv(sock, str, sizeof(str)-1, 0);   // get the message

		if (strncmp(str, "LinkUp", sizeof(str)-1) != 0)  {
	      closesocket(sock);
		   sock = INVALID_SOCKET;
			WSACleanup();
			return 101;
		}
		else  {
			break;
		}
		if ((timeGetTime() - now) >= 1000)  {
	      closesocket(sock);
		   sock = INVALID_SOCKET;
			WSACleanup();
			return 102;
		}
		Sleep(30);
	}

	// we've found a computrainer, so start the thread
	*/

	inpackets = 0L;
	outpackets = 0L;
	incomplete = 0L;
	bytes_in = 0L;
	bytes_out = 0L;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	memset(txq, 0, sizeof(txq));
	txinptr = 0;
	txoutptr = 0;
	txdiff = 0;
	memset(rxq, 0, sizeof(rxq));
	rxinptr = 0;
	rxoutptr = 0;
	rxdiff = 0;

	if (tcp_port != -1)  {
		//xxx
		contin = true;
		thrd = new tthread::thread(&mythread, this);

	//Sleep(300);
	//send("RacerMate");

	//Sleep(10000);
	//destroy();
	//return 98;

	/*
// dummy loop
DWORD then = timeGetTime();
DWORD now;
bool flag = true;

while(1)  {
	Sleep(100);
	now = timeGetTime();

	if ((now-then) > 5000)  {
		if (flag)  {
			flag = false;
			snd("RacerMate");
			printf("\nsent RacerMate\n");
			status = expect("LinkUp", 1000L);
			if (status==0)  {
				printf("received LinkUp\n");
			}
			bp = 0;
		}
	}
}
	// dummy loop
	*/

		snd( "RacerMate");
		status = expect("LinkUp", 1000L);
	}																					// if (port != -1)
	else  {
		status = 101;
	}


// test to just let the thread run for testing
	/*
while(1)  {
	Sleep(100);
}
*/

#ifdef _DEBUG
	if (calls == 2)  {
		bp = 1;
	}
#endif

	return status;
}															// init()


/**********************************************************************

**********************************************************************/

void Client::destroy(void)  {
	int status;

	status = close();

	return;
}															// destroy()

/**********************************************************************

**********************************************************************/


int Client::close(void) {
	unsigned long start;
	int rc = 0;

	//clsflg = true;

	contin = false;

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
	if (rc==1)  {
		bp = 7;
	}
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


	if (sock != INVALID_SOCKET)  {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}

	WSACleanup();

	FCLOSE(logstream);
	return rc;

}									// close()

/**********************************************************************
	stuffs a buffer into the txq
**********************************************************************/

int Client::snd( const char *_buf, int _len )  {
	int i;

	if (_len<0)  {
		_len = strlen(_buf);
	}

	for(i=0; i<_len; i++)  {
		tx(_buf[i]);
	}

	return 0;
}												// snd()

/**********************************************************************
	stuffs a byte into the txq
**********************************************************************/

//void Client::tx(const char *str, int _len, int _flush_flag)  {
void Client::tx(unsigned char c)  {
	txq[txinptr] = c;
	txinptr = (unsigned short) ((txinptr + 1) % TXQLEN);

	/*
	int status, n;

	if (sock == INVALID_SOCKET)  {
		return;
	}

	if (_len<0)  {
		n = strlen(str);
	}
	else  {
		n = _len;
	}

	// stuff it into txq

	txinptr = 0;
	*/

	/*
#ifdef WIN32
	status = send(sock, str, n, 0);
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
				outpackets++;
				bytes_out += (unsigned long)strlen((const char *)str);
			}
			else  {
				if (errors<3)  {
					logg("sent: %s\n", str);
					errors++;
				}
			}
			break;
	}
	*/

	return;
}													// void tx()

/**********************************************************************

**********************************************************************/
#define EXPECT_BUF_LEN 256

int Client::expect(const char *str, DWORD timeout)  {
	DWORD start;
	char buf[EXPECT_BUF_LEN];
	int i;
	char c;


	if (strlen(str) > sizeof(buf)-1)  {
		throw (fatalError(__FILE__, __LINE__));
	}

	i = 0;
	memset(buf, 0, sizeof(buf));

	start = timeGetTime();


	while(1)  {
		if ((timeGetTime() - start) >= timeout)  {
			break;
		}

		while (rxoutptr != rxinptr)  {
			buf[i] = rxq[rxoutptr];
			i++;
			if (i==EXPECT_BUF_LEN)  {
				close();
				throw (fatalError(__FILE__, __LINE__));
			}
			rxoutptr = (rxoutptr + 1) % RXQLEN;
		}

		if (!strcmp(str, (const char *)buf))  {
			return 0;
		}
	}

	bp = 5;
	return 1;
}											// expect()

/**********************************************************************

**********************************************************************/

int Client::rx(char *buf, int buflen)  {
	int i;

	i = 0;

	while(rxoutptr != rxinptr)  {
		if (i==buflen)  {
			close();
			throw (fatalError(__FILE__, __LINE__));
			break;
		}

		buf[i] = rxq[rxoutptr];
		rxoutptr = (rxoutptr + 1) % RXQLEN;
		i++;

	}

#ifdef _DEBUG
	if (i != 0)  {
		bp = 3;
	}
#endif

	return i;

}											// int rx()

/**********************************************************************

**********************************************************************/

void Client::flush(DWORD _timeout)  {
	DWORD start,end;

	start = end = timeGetTime();

	while(1)  {
		if (rxoutptr != rxinptr)  {
			rxoutptr = (rxoutptr + 1) % RXQLEN;
		}
	   end = timeGetTime();
		if ((end-start)>=_timeout)  {
     		break;
		}
	}

	return;
}												// flush()


/**********************************************************************

**********************************************************************/

void Client::flush(void)  {

	return;
}

#ifdef _DEBUG
/**********************************************************************

**********************************************************************/

void Client::flush_rawstream(void)  {
	return;
}
#endif

/**********************************************************************
	stuff into txq, the select timer will wake up within 10 ms to send
	it.
**********************************************************************/

void Client::txx(unsigned char *b, int n)  {
	int i;

	for(i=0; i<n; i++)  {
		txq[txinptr] = b[i];
		txinptr = (unsigned short) ((txinptr + 1) % TXQLEN);
	}

	return;
}


/**********************************************************************

 ***********************************************************************/

void Client::logg(bool _print, const char *format, ...)  {
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
	the client socket is already created and we will be receiving and
	sending data to the handlebar server
**********************************************************************/

void Client::mythread(void *tmp)  {
	int i, j;
	int n;
	struct timeval timeout;						// timeout value
	char str[256];
	unsigned long lastnow;
	//int csock = INVALID_SOCKET;										// client socket
	int bp = 0;
	AverageTimer at("client_thread");
	DWORD now;
	int status;
	int len_inet;
	unsigned char txbuf[TXQLEN];
	unsigned char rxbuf[RXQLEN];
	bool flag = false;

#ifdef _DEBUG
	int connects = 0;
#endif


	Client *client = static_cast<Client *>(tmp);

	/*
	struct hostent *hp;
	char host[256];
	hp = gethostbyname(client->url);			// 68.115.225.206
	if (hp)  {
		char *cptr = inet_ntoa(*(struct in_addr *)*hp->h_addr_list);
		strncpy(host, cptr, sizeof(host)-1);
	}
	*/


	len_inet = sizeof(client->sinaddr);
	memset((void *)&client->sinaddr, 0, sizeof(client->sinaddr) );
	client->sinaddr.sin_family = AF_INET;
	client->sinaddr.sin_port = htons(client->tcp_port);										// 9072
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	client->sinaddr.sin_addr.s_addr = inet_addr (client->url);					// set server's IP

	int cnt = 0;
	lastnow = 0;

	client->thread_running = true;


	while(client->contin)  {

		if (client->sock==INVALID_SOCKET)  {
			client->sock = socket(PF_INET, SOCK_STREAM, 0);

			if (client->sock==INVALID_SOCKET)  {
				throw (fatalError(__FILE__, __LINE__));
			}
			client->maxsocket = client->sock;
			client->connected_to_server = false;

			//char buf[8];
			//strcpy(buf, "0");
			int kk = 1;

//			status = setsockopt(client->sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&kk, sizeof(int));
//			status = WSAGetLastError();


			sprintf(str, "\ncreated socket %d\n", client->sock);
			OutputDebugString(str);
		}

		if (!client->connected_to_server)  {
			status = connect(client->sock, (struct sockaddr FAR *) &client->sinaddr, sizeof(struct sockaddr) );

			if (status==SOCKET_ERROR)  {
				status = WSAGetLastError();
				switch(status)  {
					case WSAECONNREFUSED:											// no server listening yet
						bp = 1;
						break;

					case WSAEADDRNOTAVAIL:
						closesocket(client->sock);
						client->sock = INVALID_SOCKET;
						break;

					case WSAENOTSOCK:
						closesocket(client->sock);
						client->sock = INVALID_SOCKET;
						break;

					case WSAEISCONN:
						closesocket(client->sock);
						client->sock = INVALID_SOCKET;
						break;

					case WSAEAFNOSUPPORT:
						closesocket(client->sock);
						client->sock = INVALID_SOCKET;
						break;

					default:
						closesocket(client->sock);
						client->sock = INVALID_SOCKET;
						break;
				}

				Sleep(1000);
				continue;
			}											// if (status==SOCKET_ERROR)  {

			u_long iMode=1;
			status = ioctlsocket(client->sock, FIONBIO, &iMode);

			//WSAAsyncSelect(sock, hwnd, msg, (FD_READ | FD_CONNECT | FD_CLOSE | FD_WRITE));
			//client->maxsocket = client->sock;
			OutputDebugString("Connected to server\n");
			client->connected_to_server = true;
#ifdef _DEBUG
			connects++;
			if (connects==2)  {
				bp = 1;
			}
#endif
		}															// if (client->sock==INVALID_SOCKET)


		// good socket and connected to server!

		cnt++;
		lastnow = timeGetTime();

//		FD_ZERO(&client->readfds);
//		FD_SET(client->sock, &client->readfds);

		//FD_ZERO(&client->writefds);
		//FD_SET(client->sock, &client->writefds);

		FD_ZERO(&client->exceptfds);
		FD_SET(client->sock, &client->exceptfds);

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;													// aim for timeout of 100 ms

		n = select(client->maxsocket+1, &client->readfds, &client->writefds, &client->exceptfds, &timeout);
		flag = false;

		if (n>0)  {
			// gets here if server communicates
			//at.update();							// 0.001625 milliseconds
			bp = 3;
		}
		else if (n<-1)  {
			client->logg(true, "select error 1\n");
			perror("select 1");
			//break;
			goto finis;
		}
		else if (n==-1)  {
			status = WSAGetLastError();
			switch(status)  {
				case WSAENOTSOCK:												// 10038
					bp = client->contin?1:0;
					closesocket(client->sock);
					client->sock = INVALID_SOCKET;
					client->connected_to_server = false;
					flag = true;
					break;
				default:
					bp = 3;
					client->logg(true, "select error 2\n");
					perror("select 2");
					//break;
					goto finis;
					break;
			}

		}
		else if (n==0)  {					// timeout
			/*
			unsigned long dt = timeGetTime() - lastnow;
			if (dt >= 1000)  {
				//client->logg(true, "tick\n");
				lastnow = timeGetTime();
			}
			*/
			at.update();
		}

		if (flag)  {
			continue;
		}

		#ifdef _DEBUG
		//assert(n==0);
		//at.update();						// 100.3 ms for linux, 109 ms for windows
		#endif

		/*
		DWORD now = timeGetTime();
		if ((now - client->last_ip_broadcast_time) >= 1000)  {
			client->last_ip_broadcast_time = now;
			//logg(true, "%d, %d, %d, %s, ct=%s\n", broadcast_socket, sock, maxsocket, sp?sp->getname():"null", ct_connected?"true":"false");
		}
	*/

		//-------------------------------------------------
		// keep transmit queue emptied:
		//-------------------------------------------------

		if (client->txinptr != client->txoutptr)  {
			j = 0;
			for(int i=client->txoutptr; i<client->txinptr; i++)  {
				txbuf[j++] = client->txq[i];
				client->txoutptr = (client->txoutptr + 1) % TXQLEN;
			}
#ifdef WIN32
			status = send(client->sock, (const char *)txbuf, j, 0);
#else
			status = send(clients[i].n, (const void *)outbuf, n, 0);
#endif
			switch(status)  {
				case -1:
					//logg(true, "\nsend error, len = %d:\noutbuf: %s", n, str);
					//perror("send error");
					bp = 1;
					break;

				default:
					if (status==n)  {
						//outpackets++;
						//bytes_out += (unsigned long)strlen((const char *)str);
					}
					else  {
						/*
						if (errors<3)  {
							logg("sent: %s\n", str);
							errors++;
						}
						*/
					}
					break;
			}

		}													// if (client->txinptr != client->txoutptr)


		//-------------------------------------------------
		// poll the server for stuff sent to us
		//-------------------------------------------------
#if 1
		memset(client->rxq, 0, sizeof(client->rxq));
		//n = recv(client->sock, (char *)client->rxq, sizeof(client->rxq)-1, 0);   // blocks
		n = recv(client->sock, (char *)rxbuf, sizeof(rxbuf)-1, 0);

		if (n==-1)  {
			status = WSAGetLastError();
			switch(status)  {
				case WSAETIMEDOUT:
					bp = 1;
					break;
				case WSAEWOULDBLOCK:
					bp = 5;
					break;
				default:
					bp = 2;
					break;
			}
		}
		else if (n == 0)  {
			bp = 0;
		}
		else if (n<-1)  {
			bp = 3;
		}
		else  {
			int i, j;
			j = 0;
			client->bytes_in += n;
			for(i=client->rxinptr; i<n; i++)  {
				client->rxq[i] = rxbuf[j++];
				client->rxinptr = (client->rxinptr + 1) % RXQLEN;
			}
		}

		continue;
#endif



		now = timeGetTime();

		//if (n==0)  {			// if timeout occurred, then there is no data to read
		//	continue;
		//}

		if (FD_ISSET(client->sock, &client->readfds))  {

			//memset(rxbuf, 0, sizeof(rxbuf));
			n = recv(client->sock, (char *)rxbuf, sizeof(rxbuf)-1, 0);

			if (n>0)  {
				int i, j;
				j = 0;
				client->bytes_in += n;
				for(i=client->rxinptr; i<n; i++)  {
					client->rxq[i] = rxbuf[j++];
					client->rxinptr = (client->rxinptr + 1) % RXQLEN;
				}
				bp = 2;
			}			// if (n > 0)
			else if (n==0)  {
				bp = 2;
				break;
			}
			else if (n == SOCKET_ERROR )  {
				// gets here if server closes connection
				closesocket(client->sock);
				timestamp(str);
				client->logg(false, "%s: server on socket %d closing, maxsocket = %d\n", str, client->sock, client->maxsocket);
				client->sock = INVALID_SOCKET;
				#ifndef WIN32
				fclose(client.rx);
				fclose(client.tx);
				#endif
				client->connected_to_server = false;
				OutputDebugString("Server shut down\n");
				OutputDebugString("closed socket\n");
				continue;
			}
		}						// if (FD_ISSET(client->sock, &client->readfds))  {

		if (FD_ISSET(client->sock, &client->writefds))  {
			//client->logg(true, "client write\n");
			bp = 1;
		}
		if (FD_ISSET(client->sock, &client->exceptfds))  {
			client->logg(true, "client except\n");
			bp = 1;
		}

	}												// while(client->contin)


finis:

	closesocket(client->sock);
	client->sock = INVALID_SOCKET;

	client->thread_running = false;

	return;
}										// thread()




