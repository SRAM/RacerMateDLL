
//#include <winsock.h>
#include <winsock2.h>
#include <fatalerror.h>
#include <logger.h>

#include <glib_defines.h>

class clientSocket  {

	public:
		//#define WINSOCK_VER_11 	0x0101	// Winsock Version required by SockMan
		int bp, error;
		clientSocket::clientSocket(int _port, char *_url, HWND _hwnd, unsigned long _msg);
		virtual clientSocket::~clientSocket();
		void clientSocket::cleanup(void);
		inline bool is_connected(void)  { return connected; }
		inline SOCKET get_socket(void)  { return s; }

	private:
		unsigned long msg;
		HWND hwnd;
		SOCKET s;
		struct sockaddr_in saddr;
		int port;
		char url[256];
		int status;
		char gstr[256];
		Logger *logg;
		WSADATA wsaData;
		LPHOSTENT hostEntry;
		unsigned long addr;
		bool connected;

};


