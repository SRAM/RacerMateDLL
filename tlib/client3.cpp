
#define WIN32_LEAN_AND_MEAN

#ifdef WIN32
	#include <winsock2.h>					// must include before windows.h???!!!!!!
	#include <stdio.h>
	#include <windows.h>
	#include <mmsystem.h>
	#include <commctrl.h>
	#include <objbase.h>
	#include <assert.h>
#else
	//#include <minmax.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
#endif


#include <glib_defines.h>
#include <utils.h>
#include <averagetimer.h>
#include <client.h>
#include <fatalerror.h>

/**********************************************************************

**********************************************************************/

Client::Client(const char *_url, int _port)  {
	int status;

	tcp_port = _port;

	memset(url, 0, sizeof(url));
	strncpy(url, _url, sizeof(url)-1);



	status = init();
	if (status != 0)  {
		//close();
		destroy();
		throw status;					// kills this object
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
	//int i;
	//char str[256];
	//int len_inet;
	struct hostent *hp;
	char *cptr;

#ifdef _DEBUG
	gulps = 0;
#endif

	connect_error = 0;
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
	logstream = fopen("client.log", "wt");
	//logstream = NULL;
	if (logstream)  {
		char cwd[260];
		fprintf(logstream, "created client.log\n");
		GetCurrentDirectory(sizeof(cwd), cwd);
		fprintf(logstream, "cwd = %s\n", cwd);
		fprintf(logstream, "url = %s\n", url);
		fprintf(logstream, "tcp_port = %d\n", tcp_port);
		FCLOSE(logstream);
		logstream = fopen("client.log", "a+t");
	}

	thrd = NULL;
	maxsocket = -1;
	sock = INVALID_SOCKET;										// server socket

	bp = 0;
	thread_running = false;

#ifdef WIN32
	WSADATA wsaData;
	INITCOMMONCONTROLSEX iccex;

	status = CoInitialize(NULL);
	switch (status)  {
		case S_OK:
			break;
		case E_INVALIDARG:
			MessageBox( NULL, "client CoInitialize error, E_INVALIDARG", "Error", MB_OK | MB_ICONINFORMATION );
			return 1;
		case E_OUTOFMEMORY:
			MessageBox( NULL, "client CoInitialize error, E_OUTOFMEMORY", "Error", MB_OK | MB_ICONINFORMATION );
			return 1;
		case E_UNEXPECTED:
			MessageBox( NULL, "client CoInitialize error, E_UNEXPECTED", "Error", MB_OK | MB_ICONINFORMATION );
			return 1;
		case S_FALSE:
			//MessageBox( NULL, "client CoInitialize error, S_FALSE", "Error", MB_OK | MB_ICONINFORMATION );
			//return 1;
			break;

		case RPC_E_CHANGED_MODE:
			// gets here if invoked from RM1
			//MessageBox( NULL, "client CoInitialize error, RPC_E_CHANGED_MODE", "Error", MB_OK | MB_ICONINFORMATION );
			//return 1;
			break;
		default:
			MessageBox( NULL, "client CoInitialize error", "Error", MB_OK | MB_ICONINFORMATION );
			return 1;
	}	
	if (logstream)  {
		fprintf(logstream, "ok1\n");
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

	if (logstream)  {
		fprintf(logstream, "ok2\n");
	}

	if (tcp_port != -1)  {
		contin = true;
		thrd = new tthread::thread(&mythread, this);

		if (logstream)  {
			fprintf(logstream, "init3\n");
		}

		snd( "RacerMate");
		if (logstream)  {
			fprintf(logstream, "init4\n");
		}
		status = expect("LinkUp", 30000L);				// connection refused happens in about 1.5 seconds, so this timeout should be longer than that
																	// timeout on non-existent port is 21 seconds

//while(1)  {										// infinite
//	Sleep(100);
//}

	}																					// if (port != -1)
	else  {
		status = 101;
	}

#ifdef _DEBUG
	if (calls == 2)  {
		bp = 1;
	}
#endif
	if (logstream)  {
		fprintf(logstream, "initx, status = %d\n", status);
		FCLOSE(logstream);
		logstream = fopen("client.log", "a+t");
	}

	return status;
}															// init()


/**********************************************************************

**********************************************************************/

void Client::destroy(void)  {
	//int status;

	close();

	return;
}															// destroy()

/**********************************************************************

**********************************************************************/


int Client::close(void) {
	unsigned long start;
	int rc = 0;

	//clsflg = true;

	if (logstream)  {
		fprintf(logstream, "close1\n");
		FCLOSE(logstream);
		logstream = fopen("client.log", "a+t");
	}

	contin = false;

	start = timeGetTime();

	while(thread_running)  {
		if ((timeGetTime() - start) >= 2000)  {
			rc = 1;
			if (logstream)  {
				fprintf(logstream, "couldn't stop thread\n");
				FCLOSE(logstream);
				logstream = fopen("client.log", "a+t");
			}
			break;
		}
		bp++;
	}
	bp = 1;

#ifdef _DEBUG
	if (rc==1)  {
		bp = 7;
	}
	//unsigned long dt = timeGetTime() - start;
	static int calls = 0;
	calls++;
	if (calls==2)  {
		bp = 3;
	}

	if (rc==0)  {
		if (logstream)  {
			fprintf(logstream, "stopped thread\n");
			FCLOSE(logstream);
			logstream = fopen("client.log", "a+t");
		}
	}
#endif

	if (thrd)  {
		thrd->join();
		DEL(thrd);
	}

	if (logstream)  {
		fprintf(logstream, "close2\n");
		FCLOSE(logstream);
		logstream = fopen("client.log", "a+t");
	}

#ifdef WIN32
	if (sock != INVALID_SOCKET)  {
		closesocket(sock);
		sock = INVALID_SOCKET;
	}

	WSACleanup();
#endif

	if (logstream)  {
		fprintf(logstream, "close3\n");
		FCLOSE(logstream);
		logstream = fopen("client.log", "a+t");
	}

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

void Client::tx(unsigned char c)  {
	txq[txinptr] = c;
	txinptr = (unsigned short) ((txinptr + 1) % TXQLEN);
	return;
}													// void tx()

/**********************************************************************

**********************************************************************/
#define EXPECT_BUF_LEN 256

int Client::expect(const char *str, DWORD timeout)  {
	DWORD start;
	char buf[EXPECT_BUF_LEN];
	int i;
	//char c;

	if (logstream)  {
		fprintf(logstream, "expect 1, str = %s, to = %ld\n", str, timeout);
	}

	if (strlen(str) > sizeof(buf)-1)  {
		throw (fatalError(__FILE__, __LINE__));
	}

	i = 0;
	memset(buf, 0, sizeof(buf));

	start = timeGetTime();


	while(1)  {
		if ((timeGetTime() - start) >= timeout)  {
			if (logstream)  {
				fprintf(logstream, "expect timeout (%ld)\n", timeout);
			}
			break;
		}

		if (connect_error != 0)  {

#ifdef WIN32			
			DWORD dt = timeGetTime() - start;

			switch(connect_error)  {
				case WSAECONNREFUSED:					// server actively refused, on 9072, 1.44 seconds
					if (logstream)  {
						fprintf(logstream, "connect error WSAECONNREFUSED\n");
					}
					bp = 1;
					break;
				case WSAETIMEDOUT:						// port 8898, 21.0 seconds
					if (logstream)  {
						fprintf(logstream, "connect error WSAETIMEDOUT\n");
					}
					bp = 1;
					break;
				case WSAEADDRNOTAVAIL:					// ipaddr doesn't exist or dns failed
					if (logstream)  {
						fprintf(logstream, "connect error WSAEADDRNOTAVAIL, dt = %ld\n", timeGetTime() - start);
					}
					bp = 1;
					break;
				case WSAEAFNOSUPPORT:
					if (logstream)  {
						fprintf(logstream, "connect error WSAEAFNOSUPPORT\n");
					}
					bp = 1;
					break;
				case WSAENOTSOCK:
					if (logstream)  {
						fprintf(logstream, "connect error WSAENOTSOCK\n");
					}
					bp = 1;
					break;
				case WSAEISCONN:
					if (logstream)  {
						fprintf(logstream, "connect error WSAEISCONN\n");
					}
					bp = 1;
					break;
				default:
					if (logstream)  {
						fprintf(logstream, "connect error %d\n", connect_error);
					}
					bp = 0;
					break;
			}
#endif
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
			if (logstream)  {
				fprintf(logstream, "expect x, 0\n");
				FCLOSE(logstream);
				logstream = fopen("client.log", "a+t");
			}
			return 0;
		}
	}

	if (logstream)  {
		fprintf(logstream, "expect x, 1\n");
		FCLOSE(logstream);
		logstream = fopen("client.log", "a+t");
	}

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
	/*
	gulps++;
	if (gulps==2)  {
		if (i > 0)  {
			bp = 0;
		}
	}
	*/
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
#ifndef WIN32

// to do:
void Client::logg(bool _print, const char *format, ...)  {

	return;
}

#else
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

#endif


/**********************************************************************
	the client socket is already created and we will be receiving and
	sending data to the handlebar server
**********************************************************************/

//#define DOCLIENTLOG

void Client::mythread(void *tmp)  {
	//int i, j;
	//int n;
#ifdef WIN32
	struct timeval timeout;						// timeout value
	char str[256];
	int n, status;
	#ifdef _DEBUG
		int connects = 0;
		DWORD dt;
	#endif
	int cnt = 0;
	unsigned long lastnow;
	DWORD now;
	int len_inet;
	int j;
	unsigned char txbuf[TXQLEN];
	unsigned char rxbuf[RXQLEN];
	bool flag = false;
#endif

	//int bp = 0;
	AverageTimer at("client_thread");



	Client *client = static_cast<Client *>(tmp);

	//len_inet = sizeof(client->sinaddr);
	memset((void *)&client->sinaddr, 0, sizeof(client->sinaddr) );
	client->sinaddr.sin_family = AF_INET;
	client->sinaddr.sin_port = htons(client->tcp_port);
	client->sinaddr.sin_addr.s_addr = inet_addr (client->url);

	//lastnow = 0;

	client->connect_error = 0;
	client->thread_running = true;

#ifndef WIN32
	// TODO
	while(client->contin)  {
		break;
	}

#else

	while(client->contin)  {

		if (client->sock==INVALID_SOCKET)  {
			client->sock = socket(PF_INET, SOCK_STREAM, 0);

			if (client->sock==INVALID_SOCKET)  {
				throw (fatalError(__FILE__, __LINE__));
			}

			n = 1;
			status = setsockopt(client->sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&n,	sizeof(int));			// so we'll be sending 7 byte packets instead of batching up a few packets
			if (status==SOCKET_ERROR)  {
				status = WSAGetLastError();
				switch(status)  {
					case 0:											// ??? dummy for compiling
						client->bp = 1;
						break;
					default:
						client->bp = 2;
						break;
				}
			}

			client->maxsocket = client->sock;
			client->connected_to_server = false;
			client->connect_error = 0;

			sprintf(str, "\ncreated socket %d\n", client->sock);
			OutputDebugString(str);
		}

		if (!client->connected_to_server)  {
			DWORD start = timeGetTime();
			status = connect(client->sock, (struct sockaddr FAR *) &client->sinaddr, sizeof(struct sockaddr) );

			if (status==SOCKET_ERROR)  {
				status = WSAGetLastError();
				switch(status)  {
					case WSAECONNREFUSED:											// server actively refused, on 9072
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 1.47 seconds
#endif
						if (client->contin)  {
							closesocket(client->sock);								// or non-existent numeric ip address on port 8898
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;

					case WSAETIMEDOUT:  {											// port 8898
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 21.0 seconds, if server is a black hole, different from port is closed case
#endif
						if (client->contin)  {
							closesocket(client->sock);								// or non-existent numeric ip address on port 8898
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;
					}


					case WSAEADDRNOTAVAIL:											// ipaddr doesn't exist or dns failed
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 21.0 seconds, if server is a black hole, different from port is closed case
#endif
						if (client->contin)  {
							closesocket(client->sock);
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;

					case WSAEAFNOSUPPORT:
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 
#endif
						if (client->contin)  {
							closesocket(client->sock);
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;

					case WSAENOTSOCK:
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 21.0 seconds, if server is a black hole, different from port is closed case
#endif
						if (client->contin)  {
							closesocket(client->sock);
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;

					case WSAEISCONN:
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 21.0 seconds, if server is a black hole, different from port is closed case
#endif
						if (client->contin)  {
							closesocket(client->sock);
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;

					default:
#ifdef _DEBUG
						dt = timeGetTime() - start;								// 21.0 seconds, if server is a black hole, different from port is closed case
#endif
						if (client->contin)  {
							closesocket(client->sock);
							client->sock = INVALID_SOCKET;
						}
						client->connect_error = status;
						break;
				}

				Sleep(1000);
				continue;
			}											// if (status==SOCKET_ERROR)  {

			u_long iMode=1;
			status = ioctlsocket(client->sock, FIONBIO, &iMode);								// set recv() to non-blocking

			OutputDebugString("Connected to server\n");
			client->connected_to_server = true;
#ifdef _DEBUG
			connects++;
			if (connects==2)  {
				client->bp = 1;
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
			client->bp = 3;
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
					client->bp = client->contin?1:0;
					closesocket(client->sock);
					client->sock = INVALID_SOCKET;
					client->connected_to_server = false;
					flag = true;
					break;
				default:
					client->bp = 3;
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

			while(client->txoutptr != client->txinptr)  {
				txbuf[j++] = client->txq[client->txoutptr];
				if (j==TXQLEN)  {
					closesocket(client->sock);
					client->sock = INVALID_SOCKET;
					throw (fatalError(__FILE__, __LINE__, "tx buf overflow"));
				}
				client->txoutptr = (client->txoutptr + 1) % TXQLEN;

			}

#ifdef WIN32
			status = send(client->sock, (const char *)txbuf, j, 0);
#else
			status = send(clients[i].n, (const void *)outbuf, n, 0);
#endif
			switch(status)  {
				case -1:
					client->bp = 1;
					break;

				default:
					if (status==n)  {
					}
					else  {
					}
					break;
			}

		}													// if (client->txinptr != client->txoutptr)

		//-------------------------------------------------
		// poll the server for stuff sent to us
		//-------------------------------------------------

		#ifdef _DEBUG
			memset(client->rxq, 0, sizeof(client->rxq));
		#endif

		n = recv(client->sock, (char *)rxbuf, sizeof(rxbuf)-1, 0);

		if (n==-1)  {
			status = WSAGetLastError();
			switch(status)  {
				case WSAETIMEDOUT:
					client->bp = 1;
					break;
				case WSAEWOULDBLOCK:
					client->bp = 5;
					break;
				default:
					client->bp = 2;
					break;
			}
		}
		else if (n == 0)  {
			client->bp = 0;
		}
		else if (n<-1)  {
			client->bp = 3;
		}
		else  {
			#ifdef _DEBUG
			client->gulps++;
			if (client->gulps==2)  {
			}
			#endif

			client->bytes_in += n;

			#ifdef DOCLIENTLOG
				if (client->logstream)  {
					fprintf(client->logstream, "%10ld  %d / %lu   ", timeGetTime(), n, client->bytes_in );
				}
			#endif

			int i = 0;
			unsigned char c;

			while(1)  {
				c = rxbuf[i++];
				client->rxq[client->rxinptr] = c;
				#ifdef DOCLIENTLOG
					if (client->logstream)  {
						if (i<n) fprintf(client->logstream, "%02x ", c);
						else fprintf(client->logstream, "%02x\n", c);
					}
				#endif
				client->rxinptr = (client->rxinptr + 1) % RXQLEN;

				if (i==n)  {
					break;
				}
				else if (i==sizeof(rxbuf))  {
					closesocket(client->sock);
					client->sock = INVALID_SOCKET;
					throw (fatalError(__FILE__, __LINE__, "rx buf overflow"));
				}
				
			}

		}													// data received

		if (FD_ISSET(client->sock, &client->exceptfds))  {
			client->logg(true, "client except\n");
			client->bp = 1;
		}

	}												// while(client->contin)

finis:
	closesocket(client->sock);
#endif


	client->sock = INVALID_SOCKET;

	client->thread_running = false;

	return;
}										// thread()




