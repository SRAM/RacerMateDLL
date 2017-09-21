#ifndef _X_CLIENT2_H_X_
#define _X_CLIENT2_H_X_

#ifndef WIN32
	#include <stdio.h>
#endif

#include <glib_defines.h>


class CLIENT2  {
	//friend class Server;
	//private:
	//	const int TXQLEN = 1024;
	private:
		#define TXQLEN 1024
		#define RXQLEN 1024
		#define RXBUFLEN 1024
		#define TXBUFLEN 1024

	public:
		int socket;							// socket number
		unsigned long outpackets;
		unsigned long bytes_out;
		unsigned long ipaddr;
		int id;							// 0 - 15
		int comport;					// 201-216
		char ipstr[128];

	private:
		DWORD lastidletime;
		unsigned char idle_packet[7];
		unsigned long bytes_in;
		int bp;
		int rxinptr;
		int rxoutptr;
		int txinptr;
		int txoutptr;
		unsigned char txq[TXQLEN];
		unsigned char rxq[RXQLEN];
		unsigned char rxbuf[RXBUFLEN];
		unsigned char txbuf[TXBUFLEN];

		unsigned long inpackets;
		unsigned long incomplete;
		int serialport;													// associated serial port for this client

		int init(void);

	public:
		CLIENT2(const CLIENT2 &_copy);
		CLIENT2(void);
		CLIENT2(int _id);
		~CLIENT2(void);
		int flush(void);
		int mysend(const unsigned char *_str, int _len, bool _flush);
		int idle(void);

		#ifndef WIN32
			FILE *rx;
			FILE *tx;
		#endif


		int receive(void);
		//void tx(const unsigned char _c);

};										// class CLIENT2

#endif								// #ifndef _X_CLIENT_H_X_


