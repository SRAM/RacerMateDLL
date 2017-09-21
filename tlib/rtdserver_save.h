
#ifndef _XRTDSERVER_H_
#define _XRTDSERVER_H_

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>					// must include before windows.h???!!!!!!
#include <ws2tcpip.h>
#include <windows.h>
#include <tinythread.h>
//#include <wsk.h>

#include <tdefs.h>


/**************************************************************************************

************************************************************************************/

class RTDServer  {

	private:
		#define BACKLOG 10
		#define BUFLEN 2048

		#define TXQLEN 1024
		#define RXQLEN 1024

		typedef struct  {
			int n;							// socket number
			unsigned char txbuf[BUFLEN];
			unsigned char rxbuf[BUFLEN];
			unsigned long inpackets;
			unsigned long outpackets;
			unsigned long incomplete;
			unsigned long bytes_in;
			unsigned long bytes_out;
			unsigned long ipaddr;

			#ifndef WIN32
				FILE *rx;
				FILE *tx;
			#endif
	
			char ipstr[128];

		} CLIENT;
		int myaddr[4];

		CLIENT client;
		unsigned long last_ip_broadcast_time;					// ethernet
		//int client;														// client socket
		struct sockaddr_in adr_clnt;			// AF_INET

		int bp;
		int init(void);
		//HANDLE thrd;
		tthread::thread *thrd;

		//HANDLE controlEvent;
		int close(void);
		static void mythread(void *);
		bool thread_running;
		bool contin;
		void destroy(void);
		//static int instances;
		int create_server_socket(void);
		int maxsocket;
		int server_port;
		int server_socket;
		int broadcast_socket;
		fd_set readfds;								// read set
		fd_set writefds;								// write set
		fd_set exceptfds;								// exception set
		void logg(bool _print, const char *format, ...);
		FILE *logstream;
		int errors;
		bool ip_discover;
		unsigned bcast_count;
		char broadcast_message[64];			// "CTNet:192.168.1.20:9072"
		unsigned int sendStringLen;   		// length of string to broadcast
		char *sendString;                 	// string to broadcast
		struct sockaddr_in broadcastAddr; 	// broadcast address
		char myip[64];
		int RTDServer::getmyip(void);
		char *broadcastIP;                	// ip broadcast address
		int create_broadcast_socket(void);
		unsigned short broadcast_port;

		#ifdef WIN32
			std::vector<INTERFACE_INFO> ifaces;
		#else
			typedef struct  {
				char name[32];
				char addr[32];
			} IFACE;

			std::vector<IFACE> ifaces;
		#endif

		unsigned char rxq[RXQLEN];
		unsigned short rxdiff;
		unsigned short rxinptr;
		unsigned short rxoutptr;

		/*
		unsigned char txq[TXQLEN];
		unsigned short txdiff;
		unsigned short txinptr;
		unsigned short txoutptr;
		*/

	public:
		RTDServer(void);
		RTDServer(int _port);
		~RTDServer(void);
		void tx(const char *str, int flush_flag);
		int rx(char *buf, int buflen);
		int expect(const char *str, DWORD timeout);
		//int getPortNumber(void)  { return SERVER_SERIAL_PORT; }
		int getPortNumber(void)  { return 231; }
		void flush(void);
		//bool initialized;
		int get_client_socket(void)  { return client.n; }

#ifdef _DEBUG
		void flush_rawstream(void);
#endif
		void txx(unsigned char *b, int n);
};

#endif		// #ifndef _SERVER_H_
