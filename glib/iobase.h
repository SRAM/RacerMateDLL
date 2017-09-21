#ifndef _WX_IOBASE_H
#define _WX_IOBASE_H

#include <sys/types.h>
#include <stddef.h>

#define CTB_COMMON 0x0000
#define CTB_SERIAL 0x0100
#define CTB_GPIB   0x0200

enum {
	CTB_RESET = CTB_COMMON
};

class wxIOBase {
    private:
        int bp;

	protected:
		virtual int CloseDevice() = 0;
		virtual int OpenDevice(void* dcs = 0L) = 0;

	public:
		wxIOBase() {
                    bp = 0;
                };

		virtual ~wxIOBase() {};

		virtual const char* ClassName() {return "wxIOBase";};
		int Close() {return CloseDevice();};
		virtual int Ioctl(int cmd,void* args) {return -1;};
		virtual bool is_open(void) = 0;
		int Open(void* dcs=0L);
		int Open(const char *_portstr, void* dcs=0L);               // used by nvram class

		virtual int rx(char* buf,size_t len) = 0;			// tlm
		virtual int ReadUntilEOS(char** readbuf, char* eosString = "\n", long timeout_in_ms = 1000L);
		int Read(char* buf,size_t len,int* timeout_flag,bool nice=false);
		virtual int tx(const char* buf,size_t len) = 0;
		int Write(char* buf,size_t len,int* timeout_flag,bool nice = false);
		void flush(void) { return; }
};

#endif



