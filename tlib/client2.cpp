
#ifdef WIN32
	#pragma warning(disable:4996)				// "str" unreferenced
#endif

#include <memory.h>

#ifdef WIN32
	#include <winsock2.h>					// must include before windows.h???!!!!!!
	//#include <ws2tcpip.h>
#else
	//#include <minmax.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#include <linux_utils.h>
#endif


#include <client2.h>

/**********************************************************************
	copy constructor
**********************************************************************/

CLIENT2::CLIENT2(const CLIENT2 &_copy)  {
	idle_packet[0] = 0;
	idle_packet[1] = 0;
	idle_packet[2] = 0;
	idle_packet[3] = 0;
	idle_packet[4] = 0;
	idle_packet[5] = 0;
	idle_packet[6] = 0x80;
	socket = _copy.socket;

	ipaddr = _copy.ipaddr;
	id = _copy.id;
	comport = _copy.comport;
	socket = _copy.socket;
	strncpy(ipstr, _copy.ipstr, sizeof(ipstr)-1);
	bp = 0;
}

/**********************************************************************

**********************************************************************/

CLIENT2::CLIENT2(void)  {
	id = 0;
	init();
	return;
}

/**********************************************************************

**********************************************************************/

CLIENT2::CLIENT2(int _id)  {
	id = _id;
	init();
	return;
}

/**********************************************************************

**********************************************************************/

CLIENT2::~CLIENT2(void)  {

	return;
}

/**********************************************************************

**********************************************************************/

int CLIENT2::init(void)  {
	bp = 0;
	lastidletime = 0L;
	//memset(idle_packet, 0, sizeof(idle_packet));
	idle_packet[0] = 0;
	idle_packet[1] = 0;
	idle_packet[2] = 0;
	idle_packet[3] = 0;
	idle_packet[4] = 0;
	idle_packet[5] = 0;
	idle_packet[6] = 0x80;

	socket = INVALID_SOCKET;
	outpackets = 0L;
	bytes_out = 0L;
	txinptr = 0;
	txoutptr = 0;
	memset(rxq, 0, sizeof(rxq));
	memset(txq, 0, sizeof(txq));
	memset(rxbuf, 0, sizeof(rxbuf));
	memset(txbuf, 0, sizeof(txbuf));
	id = 0;							// 0 - 15broadcast_message
	comport = 0;					// 201-216
	rxinptr = 0;
	rxoutptr = 0;
	inpackets = 0L;
	incomplete = 0L;
	bytes_in = 0L;
	ipaddr = 0L;
	serialport = 0;													// associated serial port for this client

	#ifndef WIN32
		rx = NULL;
		tx = NULL;
	#endif
	
	memset(ipstr, 0, sizeof(ipstr));

	return 0;
}									// CLIENT::init()

/**********************************************************************
	puts stuff in the transmit queue
**********************************************************************/

int CLIENT2::flush(void)  {
	int i = 0;

	while (txoutptr  != txinptr)  {
		txbuf[i] = txq[txoutptr];
		txoutptr = (txoutptr + 1) % sizeof(txq);
		i++;
		if (i==TXBUFLEN)  {
			return 1;										// buffer overflow
		}
	}

#ifdef WIN32
	send(socket, (const char *)txbuf, i, 0);
#endif

	return 0;
}

/**********************************************************************

**********************************************************************/

int CLIENT2::receive(void)  {
	int tot = 0;;

//#ifndef WIN32
#if 0
	//todo
	int i, n;
	int flags = 0;
	n = recv(socket, rxbuf, sizeof(rxbuf)-1, flags)
#else
	int i, n;
	while(1)  {
		n = recv(socket, (char *)rxbuf, sizeof(rxbuf)-1, 0);   // get the message

		if (n==0)  {
			break;
		}
		else if (n<0)  {
			break;
		}

		tot += n;
		bytes_in += tot;

		if (n>RXBUFLEN)  {
			return -1;
		}

		// put it in the rxq

		for(i=0; i<n; i++)  {
			rxq[rxinptr] = rxbuf[i];
			rxinptr = (rxinptr+1) % RXQLEN;
			//i++;
			//if (i==RXBUFLEN)  {
			//	return -1;								// error;
			//}
		}
		bp = 1;
	}
#endif

	return tot;
}												// receive()

/**********************************************************************

**********************************************************************/

/*
void CLIENT2::tx(const unsigned char _c)  {
	//int n;
	//n = recv(socket, (char *)rxbuf, sizeof(rxbuf)-1, 0);   // get the message
	//return n;
	bp = 0;

	bp = 1;
	return;
}
*/

/**********************************************************************

**********************************************************************/

int CLIENT2::idle(void)  {
	DWORD now;

	now = timeGetTime();

	if ((now-lastidletime) >= 500)  {
		lastidletime = now;
		mysend(idle_packet, 7, true);
		return 1;
	}


	return 0;

}

/**********************************************************************
	just puts stuff in the queue
**********************************************************************/

int CLIENT2::mysend(const unsigned char *_buf, int _len, bool _flush)  {

	/*
	int i;

	for(i=0; i<_len; i++)  {
		txq[txinptr] = _str[i];
		txinptr = (txinptr+1) % TXQLEN;
	}
	*/

	int status;
	int flags = 0;

#ifndef WIN32
	flags |= MSG_DONTWAIT;
	status = send(socket, _buf, _len, flags);
#else
	status = send(socket, (char *)_buf, _len, flags);
#endif

	if (status==-1)  {
		const char *cptr = strerror(errno);
		bp = 4;
		return 1;
	}

	return 0;
}

