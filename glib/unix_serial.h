
#ifndef _UNIX_SERIAL_H_
#define _UNIX_SERIAL_H_

#include "logger.h"

enum { N8R, N8R1, E7R, N8CRLF, E7CR, N8CR, E7CRLF };

class Serial  {
	public:
		Serial(void);
		Serial( char *_ident, char *_device, int _baud, int _ttymode);
		Serial(const char *pszPort,		const char *pszBaudRate,	const char *_parity,	int _databits,		int _stopbits,				bool _displayErrors=TRUE,		Logger *_logg=NULL)  { return; }				// constructor 2
		Serial(bool _bluetooth_delay, const char *pszPort,		const char *pszBaudRate,	bool _displayErrors,	Logger *_logg)  { return; }				// constructor 3
		Serial(const char *pszPort,		const char *pszBaudRate,	Logger *_logg);		//, bool _displayErrors=true, char *_dummy=NULL);		// constructor 4

		~Serial();

		unsigned short txdiff;
		unsigned short rxdiff;
		unsigned short rxinptr;
		unsigned short rxoutptr;
		unsigned short txinptr;
		unsigned short txoutptr;

		long rx(void);
		int rx(char* buf, long len);
		//int tx(unsigned char *buf, long len);
		int tx(const unsigned char* buf, long len);
		int txx(const unsigned char* buf, long len);
		void flushrx(unsigned long ms)  { return; }
		int Open(const char * pszPort, const char *BaudRate, bool bDisplayErrors = true);
		void flush_rawstream(void) { return; }
		void flush(void)  { return; }
		int getPortNumber(void)  { return -1; }
		void flush_tx(void)  { return; }
		//void send(const char *str, int flush_flag)  { return; }
		int send(const char *str, int flush_flag);
		//int expect(const char *str, DWORD timeout)  { return 0; }
		int expect(const char *str, DWORD timeout);
		//void setShutDownDelay(unsigned long _ms)  { shutDownDelay = _ms; }
		void setShutDownDelay(unsigned long _ms);
		//bool isopen(void);
		//bool is_open(void)  { return false; }
		bool is_open(void);
		const char *get_port_name(void);
		void log(const char *str, bool flush=false);


	private:
		#ifdef _DEBUG
		int bp;
		#endif

		char str[260];
		FILE *logstream;
		char ident[32];
		char device[32];
		int baud;
		//char baudstr[32];
		int ttymode;
		int fd;
		//FILE *fp;
		unsigned long shutDownDelay;
		void init(void);
		void opn(void);
		int clse(void);
        char rxbuf[256];

};

#endif



