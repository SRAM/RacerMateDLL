
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

#include "tdefs.h"

#if 0
//#include "device.h"
#include "client2.h"
#include "udp_client.h"

/**************************************************************************************

************************************************************************************/

class Server  {

	private:
		#define MAXCLIENTS 8
		#define MAXUDPCLIENTS 1

		#define BACKLOG 10

		TrainerData td;

		/////////////////////////////
		int msgcnt;
		//QTimer *bctimer;                                  // broadcast timer
		bool started;
		bool finished;
		bool paused;
		int curid;

		/////////////////////////////


		void pack(void);
		static int instances;

		AverageTimer *at;
		bool log_to_console;
		int myaddr[4];
		std::vector<CLIENT2> tcpclients;

		UDPClient *udpclients[MAXUDPCLIENTS];


		unsigned long last_ip_broadcast_time;					// ethernet
		struct sockaddr_in adr_clnt;			// AF_INET

#ifdef _DEBUG
		int bp;
#endif

		int tcp_init(void);
		int udp_init(void);
		tthread::thread *thrd;
		tthread::mutex mut;

		int myclose(void);
		static void tcpthread(void *);
		static void udpthread(void *);

		bool thread_running;
		bool contin;
		void destroy(void);
		int create_tcp_listen_socket(void);
		int create_udp_socket(void);
		int maxsocket;
		int listen_port;

		int tcp_listen_socket;
		int udp_socket;

		int broadcast_socket;
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
		int getPortNumber(void);
		void flush(void);
		int get_client_socket(int _ix);
		
		int send(int _ix, const unsigned char *_str, int _len, bool _flush=false);			// puts stuff in the txq

		int expect(int _ix, const char *str, DWORD timeout);
		int rx(int _ix, unsigned char *buf, int buflen);
		int reset_client(int _ix);
		int set_hr_bounds(int _ix, int _LowBound, int _HighBound, bool _BeepEnabled);
		int set_ftp(int _ix, float _ftp);
		int reset_averages(int _ix);
		int set_slope(int _ix, float bike_kgs, float person_kgs, int drag_factor, float grade);			// float, int, float
		bool is_paused(int _ix);
		int set_paused(int _ix, bool _paused);
		float get_ftp(int _ix);
		void set_export(int _ix, const char *_dbgfname);
		void set_file_mode(int _ix, bool _mode);
		float get_watts_factor(int _ix);
		bool client_is_running(int _ix);
		struct TrainerData GetTrainerData(int _ix, int _fw);
		int get_handlebar_buttons(int _ix);
		float *get_bars(int _ix);
		float *get_average_bars(int _ix);
		SSDATA get_ss_data(int _ix);
		int set_ergo_mode(int _ix, int _fw, int _rrc, float _manwatts);
		int start_cal(int _ix);
		int end_cal(int _ix);

		void flush_rawstream(int _ix);
#ifdef _DEBUG
#endif
		void txx(unsigned char *b, int n);
		//int read_comuptrainer(int);
		DEVICE get_device(int);
};

#endif		// #ifndef _SERVER_H_
#endif
