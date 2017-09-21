
#ifndef _NVRAM_H_
#define _NVRAM_H_

#ifdef WIN32
	#include <windows.h>
#else
	#include <memory.h>
#endif



#include <serial.h>
#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

#ifdef WIN32
//class __declspec(dllexport) NVRAM  {
class NVRAM  {
#else
class NVRAM  {
#endif

	friend class Debugger;
	friend class Calibrator;

	public:

		#pragma pack(push, 1)

		typedef struct  {
				unsigned char version;
				unsigned char build;
				unsigned long factoryCalibration;
				unsigned char fill[248];
				unsigned short nv_checksum;
		} DATA;

		union U {
			DATA d;
			char buf[256];
		};

		#pragma pack(pop)


		NVRAM(char *_portstr);
		NVRAM(Serial *_port);
		virtual ~NVRAM();
		int read(void);
		int write(void);
		void connect(void);
		void disconnect(void);
		void setFactoryCalibration(unsigned long);
		U *getBuf(void)  {
			return &u;
		}

		unsigned char getVersion(void)  {return u.d.version; }
		unsigned char getBuild(void)  {return u.d.build; }
		void reset(void)  {memset(u.buf, 0, sizeof(u.buf)); }
		void setVersion(unsigned char _ver)  { u.d.version = _ver; }
		void setBuild(unsigned char _build)  { u.d.build = _build; }

	private:

		U u;
		Serial *port;
		void cleanup(void);
		char portstr[32];
		bool gotver;

};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif				// #ifndef _NVRAM_H_


