
#ifndef _LINUX_SERIAL_H_
#define _LINUX_SERIAL_H_

#include <stdio.h>
#include <termios.h>
//#include "/usr/include/linux/serial.h"

#include <iobase.h>
#include <logger.h>

#define WXSERIALPORT_NAME_LEN 32

enum  {
	SERIAL_ERROR_DOES_NOT_EXIST=1,
	SERIAL_ERROR_ACCESS_DENIED,
	SERIAL_ERROR_BUILDCOMMDCB,
	SERIAL_ERROR_SETCOMMSTATE,
	SERIAL_ERROR_CREATEEVENT1,
	SERIAL_ERROR_CREATEEVENT2,
	SERIAL_ERROR_OTHER
};

enum wxBaud {
	wxBAUD_150=150,
	wxBAUD_300=300,       
	wxBAUD_600=600,
	wxBAUD_1200=1200,
	wxBAUD_2400=2400,
	wxBAUD_4800=4800,
	wxBAUD_9600=9600,
	wxBAUD_19200=19200,
	wxBAUD_38400=38400,
	wxBAUD_57600=57600,
	wxBAUD_115200=115200,
	wxBAUD_230400=230400,
	wxBAUD_460800=460800,
	wxBAUD_921600=921600
};

enum wxParity {
	wxPARITY_NONE,
	wxPARITY_ODD,
	wxPARITY_EVEN,
	wxPARITY_MARK,
	wxPARITY_SPACE
};

enum wxSerialLineState {
	wxSERIAL_LINESTATE_DCD = 0x040,		// Data Carrier Detect (read only)
	wxSERIAL_LINESTATE_CTS = 0x020,	 	// Clear To Send (read only)
	wxSERIAL_LINESTATE_DSR = 0x100, 		// Data Set Ready (read only)
	wxSERIAL_LINESTATE_DTR = 0x002, 		// Data Terminal Ready (write only)
	wxSERIAL_LINESTATE_RING = 0x080,		// Ring Detect (read only)
	wxSERIAL_LINESTATE_RTS = 0x004,		// Request To Send (write only)
	wxSERIAL_LINESTATE_NULL = 0x000		// no active line state, use this for clear
};


struct SerialPort_DCS {
	wxBaud baud;					// the baudrate
	wxParity parity;				// the parity
	unsigned char wordlen;		// the wordlen
	unsigned char stopbits;		// count of stopbits
	bool rtscts;					// rtscts flow control
	bool xonxoff;					// XON/XOFF flow control

	SerialPort_DCS() {
		baud = wxBAUD_19200;
		parity = wxPARITY_NONE;
		wordlen = 8;
		stopbits = 1;
		rtscts = false;
		xonxoff = false;
	};

	char* GetSettings(char* buf,size_t bufsize) {
		const char ac[5] = {'N','O','E','M','S'};
		snprintf(buf,bufsize,"%i%c%i %i", wordlen, ac[parity], stopbits, baud);
		return buf;
	};

}; 


struct wxSerialPort_EINFO {
	int brk;				// number of breaks
	int frame;			// number of framing errors
	int overrun;		// number of overrun errors
	int parity;			// number of parity errors
	wxSerialPort_EINFO() {
		brk = frame = overrun = parity = 0;
	};
};

enum {
	CTB_SER_GETEINFO = CTB_SERIAL,
	CTB_SER_GETBRK,
	CTB_SER_GETFRM,
	CTB_SER_GETOVR,
	CTB_SER_GETPAR,
	CTB_SER_GETINQUE
};

#define wxCOM1 "/dev/ttyS0"
#define wxCOM2 "/dev/ttyS1"
#define wxCOM3 "/dev/ttyS2"
#define wxCOM4 "/dev/ttyS3"
#define wxCOM5 "/dev/ttyS4"
#define wxCOM6 "/dev/ttyS5"
#define wxCOM7 "/dev/ttyS6"
#define wxCOM8 "/dev/ttyS7"
#define wxCOM9 "/dev/ttyS8"


/***********************************************************************************

***********************************************************************************/

class Serialx : public wxIOBase {
	protected:
		SerialPort_DCS m_dcs;
		char devname[WXSERIALPORT_NAME_LEN];
	public:
		Serialx() {
                    devname[0] = '\0';
                }
		virtual ~Serialx() {};
		const char* ClassName() {return "wxSerialPort";};
		virtual int ChangeLineState(wxSerialLineState flags) = 0;
		virtual int ClrLineState(wxSerialLineState flags) = 0;
		virtual int GetLineState(wxSerialLineState* flags) = 0;
		virtual int GetSettingsAsString(char* str, size_t size) = 0;
		virtual int Ioctl(int cmd,void* args) {return -1;};
		virtual int SendBreak(int duration) = 0;
		virtual int SetBaudRate(wxBaud baudrate) = 0;
		virtual int SetLineState(wxSerialLineState flags) = 0;
};


/***********************************************************************************

***********************************************************************************/

class Serial : public Serialx {

	private:
		SerialPort_DCS dcs;
		int bp;
		unsigned long shutDownDelay;
		int portnum;
                bool try_usb;
                bool usb;

#ifdef _DEBUG
		FILE *rawstream;
#endif
                char error_string[256];
                void init(void);

	protected:
		int fd;
		struct termios t, save_t;
		struct serial_icounter_struct save_info, last_info;
		speed_t AdaptBaudrate(wxBaud baud);
		int CloseDevice();
		int OpenDevice(void* dcs);

	public:
		Serial();
		Serial(const char *_portstr, const char *_baudstr, Logger *_logg);
		~Serial();
                void set_try_usb(bool _b)  { try_usb = _b; }
                void set_portnum(int _i)  { portnum = _i; }
		int ChangeLineState(wxSerialLineState flags);
		int ClrLineState(wxSerialLineState flags);
		int GetLineState(wxSerialLineState* flags);
		int GetSettingsAsString(char* str, size_t size);
		int getPortNumber(void);
		int Ioctl(int cmd,void* args);
		//int IsOpen();
                const char *get_error_string(void)  { return error_string; }
                bool is_open(void);
		int rx(char* buf,size_t len);
		void flushrx(DWORD ms);
		int SendBreak(int duration);
		int SetBaudRate(wxBaud baudrate);
		int SetLineState(wxSerialLineState flags);
		//int Write(char* buf,size_t len,int* timeout_flag,bool nice = false);
		static char* GetSettingsFromDCS(char* buf, size_t bufsize, SerialPort_DCS& dcs);

                inline int getfd(void)  { return fd; }
                inline bool is_usb(void)  { return usb; }
                
		int tx(const char* buf, size_t len);
		int txx(const unsigned char* buf, size_t len);

		//void send(const char *str, int flush_flag);
		void setShutDownDelay(unsigned long _ms)  { shutDownDelay = _ms; }
		int expect(const char *str, DWORD timeout);
                void flush_rawstream(void);
		void flush_tx(void);


};

#endif         // #ifndef _LINUX_SERIAL_H_

