#pragma warning(disable:4996)					// for vista strncpy_s

#include <clientsocket.h>


clientSocket::clientSocket(int _port, char *_url, HWND _hwnd, unsigned long _msg)  {
	s = INVALID_SOCKET;
	port = _port;
	strncpy(url, _url, sizeof(url)-1);
	hwnd = _hwnd;
	msg = _msg;

	connected = false;

	logg = new Logger("client.log");

	logg->write("url = %s\n", url);


	if ( (status=WSAStartup(0x0202, &wsaData)) != 0 )  {
		sprintf(gstr, "WSAStartup error, status = %d\n", status);
		logg->write(gstr);
		cleanup();
		throw (fatalError(__FILE__, __LINE__));
	}

	if (wsaData.wVersion != 0x0202)  {		// wrong WinSock version!
		WSACleanup ();						// unload ws2_32.dll
		throw (fatalError(__FILE__, __LINE__, "wrong winsock version") );
	}



	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)  {
		cleanup();
		throw (fatalError(__FILE__, __LINE__, "Could not create socket"));
	}


	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	//saddr.sin_addr.s_addr = ((LPIN_ADDR)hostEntry->h_addr)->s_addr;
	saddr.sin_addr.s_addr = inet_addr (url);					// set server's IP

//target.sin_family = AF_INET;           // address family Internet
//target.sin_port = htons (5001);        // set server's port number
//target.sin_addr.s_addr = inet_addr ("52.123.72.251");  // set server's IP

//if (connect(s, target, sizeof(target)) == SOCKET_ERROR)
//{ // an error connecting has occurred!
  //WSACleanup ();
  //return;
//}

	status = connect(s, (struct sockaddr FAR *) &saddr, sizeof(struct sockaddr) );

	if (status==SOCKET_ERROR)  {
		status = WSAGetLastError();
		switch(status)  {
			case 10061:
				sprintf(gstr, "Connection Refused");
				logg->write("%s\n", gstr);
				cleanup();
				//throw (fatalError(__FILE__, __LINE__));
				return;

			case 10049:
				sprintf(gstr, "Cannot assign requested address");
				logg->write("%s\n", gstr);
				cleanup();
				throw (fatalError(__FILE__, __LINE__));

			default:
				sprintf(gstr, "error: Could not connect(), status = %d\n", status);
				logg->write(gstr);
				//cleanup();
				//throw (fatalError(__FILE__, __LINE__));
				return;
		}
	}

	WSAAsyncSelect(s, hwnd, msg, (FD_READ | FD_CONNECT | FD_CLOSE | FD_WRITE));

	connected = true;

	logg->write("connection ok\n");

}					// constructor


/*************************************************************************************

*************************************************************************************/

clientSocket::~clientSocket()  {
	cleanup();
}

/*************************************************************************************

*************************************************************************************/

void clientSocket::cleanup(void)  {

	//shutdown(s, SD_SEND);							// s cannot send anymore

	if (s != INVALID_SOCKET)  {
		// you should check to see if any last data has arrived here
		closesocket(s);
		s = INVALID_SOCKET;
	}

	WSACleanup();
	DEL(logg);
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#if 0
	if (url)  {
		addr = inet_addr(url);		// check for a dotted-IP address string
 
		/* If not an address, then try to resolve it as a hostname */

		if ((addr == INADDR_NONE) && (strcmp (url, "255.255.255.255")))  {
			hostEntry = gethostbyname(url);
			if (hostEntry)  {											// SUCCESS
				addr = *((u_long FAR *) (hostEntry->h_addr));
			}
			else  {
				addr = INADDR_ANY;									// FAILURE
			}
		//}
	//}
	//return (addr);




			/*
			char *token;
			bool numeric = true;

			token = strtok(url, ".");

			if (isNumericString(token))  {
				while(token != NULL)  {
					token = strtok(NULL, ".");
					if (!isNumericString(token))  {
						numeric = false;
					}
				}
			}
			else  {
				numeric = false;
			}


			strncpy(url, _url, sizeof(url)-1);

			if (!numeric)  {
				hostEntry = gethostbyname(url);							// Specifying the server by its name. another option is gethostbyaddr() (see below)
			}
			*/

//hostEntry = gethostbyname(url);							// Specifying the server by its name. another option is gethostbyaddr() (see below)
	/*
int i = WSAGetLastError();
switch(i)  {
	case WSANOTINITIALISED:			// A successful WSAStartup call must occur before using this function. 
		bp = 1;
		break;
	case WSAENETDOWN:					// The network subsystem has failed. 
		bp = 1;
		break;
	case WSAHOST_NOT_FOUND:			// Authoritative Answer Host not found. 
		bp = 1;
		break;
	case WSATRY_AGAIN:				// Nonauthoritative Host not found, or server failure. 
		bp = 1;
		break;
	case WSANO_RECOVERY:				// A nonrecoverable error occurred. 
		bp = 1;
		break;
	case WSANO_DATA:					// Valid name, no data record of requested type. 
		bp = 1;
		break;
	case WSAEINPROGRESS:				// A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
		bp = 1;
		break;
	case WSAEFAULT:					// The name parameter is not a valid part of the user address space. 
		bp = 1;
		break;
	case WSAEINTR:						// A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. 
		bp = 1;
		break;
	default:
		break;
}
	*/

	/*
			addr = inet_addr(url);

			if(addr==INADDR_NONE)  {
				hostEntry = gethostbyname(url);
				if (!hostEntry) {
					sprintf(gstr, "gethostbyname error\n");
					logg->write(gstr);
					cleanup();
					throw (fatalError(__FILE__, __LINE__));
				}
				logg->write("resolved %s:  %d.%d.%d.%d\n",
					hostEntry->h_name,
					hostEntry->h_addr_list[0][0] & 0x00ff,
					hostEntry->h_addr_list[0][1] & 0x00ff,
					hostEntry->h_addr_list[0][2] & 0x00ff,
					hostEntry->h_addr_list[0][3] & 0x00ff
				);
			}
			else  {
				hostEntry = gethostbyaddr(url, sizeof(url), AF_INET);
				if (!hostEntry) {
					sprintf(gstr, "gethostbyname error\n");
					logg->write(gstr);
					cleanup();
					throw (fatalError(__FILE__, __LINE__));
				}
			}

*/


	logg->write("resolved %s:  %d.%d.%d.%d\n",
			hostEntry->h_name,
			hostEntry->h_addr_list[0][0] & 0x00ff,
			hostEntry->h_addr_list[0][1] & 0x00ff,
			hostEntry->h_addr_list[0][2] & 0x00ff,
			hostEntry->h_addr_list[0][3] & 0x00ff
		);
	}
}

	sprintf(gstr, "saddr.sin_family = %d\n", saddr.sin_family);
	logg->write(gstr);

	sprintf(gstr, "saddr.sin_port = %d\n", ntohs(saddr.sin_port));
	logg->write(gstr);

	sprintf(gstr, "saddr.sin_addr = %d.%d.%d.%d\n",
		saddr.sin_addr.S_un.S_un_b.s_b1,
		saddr.sin_addr.S_un.S_un_b.s_b2,
		saddr.sin_addr.S_un.S_un_b.s_b3,
		saddr.sin_addr.S_un.S_un_b.s_b4);
	logg->write(gstr);

#endif		// if 0
