
#ifndef _XSERVER_H_
#define _XSERVER_H_

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifdef WIN32
	#include <winsock2.h>					// must include before windows.h???!!!!!!
	#include <ws2tcpip.h>
	#include <windows.h>
#endif

#include <tinythread.h>

#include <averagetimer.h>
#include <tdefs.h>
#include <client2.h>


/**************************************************************************************

************************************************************************************/

class Server  {

	private:
		#define MAXCLIENTS 8

		#define BACKLOG 10
		void pack(void);
		static int instances;

		AverageTimer *at;
		bool log_to_console;
		int myaddr[4];
		std::vector<CLIENT2> clients;

		unsigned long last_ip_broadcast_time;					// ethernet
		struct sockaddr_in adr_clnt;			// AF_INET

		int bp;
		int init(void);
		tthread::thread *thrd;
		tthread::mutex mut;

		int close(void);
		static void mythread(void *);
		bool thread_running;
		bool contin;
		void destroy(void);
		int create_listen_socket(void);
		int maxsocket;
		int listen_port;
		int listen_socket;
		int broadcast_socket;
		fd_set readfds;								// read set
		fd_set writefds;								// write set
		fd_set exceptfds;								// exception set
		void logg(bool _print, const char *format, ...);
		FILE *logstream;
		int errors;
		bool ip_discover;
		bool udp;
		unsigned bcast_count;
		char broadcast_message[64];			// "CTNet:192.168.1.20:9072"
		unsigned int sendStringLen;   		// length of string to broadcast
		struct sockaddr_in broadcastAddr; 	// broadcast address
		char myip[64];
		int getmyip(void);
		char broadcast_address[32];
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

	public:
		Server(void);
		Server(int _broadcast_port, int _listen_port, bool _ip_discover=false, bool _udp=false);
		~Server(void);
		bool is_client_connected(int _ix);
		int getPortNumber(void)  { return 201; }
		void flush(void);
		int get_client_socket(int _ix);
		
		int send(int _ix, const unsigned char *_str, int _len, bool _flush=false);			// puts stuff in the txq

		int expect(int _ix, const char *str, DWORD timeout);
		int rx(int _ix, char *buf, int buflen);

		void flush_rawstream(int _ix);
#ifdef _DEBUG
#endif
		void txx(unsigned char *b, int n);
};

#endif		// #ifndef _SERVER_H_
