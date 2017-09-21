
#ifndef _XCLIENT_H_
#define _XCLIENT_H_

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifdef WIN32
	#include <winsock2.h>					// must include before windows.h???!!!!!!
	#include <ws2tcpip.h>
	#include <windows.h>
#endif

#include <tinythread.h>

#include <tdefs.h>


/**************************************************************************************

************************************************************************************/

class Client  {

//#define tx(c)  { txq[txinptr] = c; txinptr = (unsigned short) ((txinptr + 1) % TXQLEN); } }

	private:
#ifdef _DEBUG
		int gulps;
#endif
		/*
		bool refused;
		bool af_not_supported;
		bool connect_timed_out;
		bool other_connect_error;
		*/

		//enum  {
		//} CONNECT_ERRORS;

		//struct  {
		//	int refused
		//} CONNECTION_ERRORS;

		int connect_error;

		#define BACKLOG 10
		//#define BUFLEN 2048

		#define TXQLEN 1024
		#define RXQLEN 1024

		/*
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


		CLIENT client;
		*/

		//unsigned char txq[BUFLEN];
		//unsigned char rxq[BUFLEN];
		//int txinptr;
		//int txoutptr;
		//int rxinptr;
		//int rxoutptr;

		unsigned long inpackets;
		unsigned long outpackets;
		unsigned long incomplete;
		unsigned long bytes_in;
		unsigned long bytes_out;
		//unsigned long ipaddr;
		bool connected_to_server;
		bool connected_to_trainer;

		#ifndef WIN32
			FILE *rxfd;
			FILE *txfd;
		#endif
	
//		char ipstr[128];

		struct sockaddr_in adr_clnt;			// AF_INET
		char url[256];
		int tcp_port;

		int bp;
		int init(void);
		tthread::thread *thrd;

		int close(void);
		static void mythread(void *);
		bool thread_running;
		bool contin;
		void destroy(void);
		int maxsocket;
		int sock;
		fd_set readfds;								// read set
		fd_set writefds;								// write set
		fd_set exceptfds;								// exception set
		void logg(bool _print, const char *format, ...);
		FILE *logstream;
		int errors;
		struct sockaddr_in sinaddr;			// AF_INET

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

		unsigned char txq[RXQLEN];
		unsigned short txdiff;
		unsigned short txinptr;
		unsigned short txoutptr;
		char gstr[256];
		//bool clsflg;

	public:
		Client(const char *_url, int _port);
		~Client(void);

		int snd( const char *_buf, int _len=-1 );
		//void tx(const char *str, int _len=-1, int flush_flag=0);
		void tx(unsigned char c);
		int rx(char *buf, int buflen);
		int expect(const char *str, DWORD timeout);
		void flush(DWORD _timeout);

		//int getPortNumber(void)  { return CLIENT_SERIAL_PORT_BASE; }
		void flush(void);
		int get_socket(void)  { return sock; }

#ifdef _DEBUG
		void flush_rawstream(void);
#endif
		void txx(unsigned char *b, int n);
};

#endif		// #ifndef _XCLIENT_H_
