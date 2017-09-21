
#ifndef _USER_H_
#define _USER_H_

#include <stdio.h>


#include <tdefs.h>


#ifndef NEWUSER

#include <crf.h>
#include <ini.h>

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
#endif

typedef struct {
	char name[128];
	char sex[8];
	int age;
	float kgs;
	float lbs;
	float lower_hr;
	float upper_hr;
	BOOL metric;
	char date[16];
	char time[16];
	BOOL draft_enable;
	float drag_aerodynamic;
	float drag_tire;
	unsigned short rfdrag;
} USER_DATA;


#ifdef WIN32
class __declspec(dllexport) User  {
#else
class User  {
#endif


	private:
		int fsize;							// file size
		float watts_factor;
		void destroy(void);
		char caption[256];
		char fname[256];
		FILE *stream;
		int user_data_size;
		int file_size;
		char ueheader[23];

#ifdef WIN32
		HINSTANCE hinstance;
		HWND phwnd;
		HWND hwnd;
#endif
		
		void init(void);
		void make(void);
		int sanityCheck(void);
		DWORD version;

	public:
		User(void);
		User(char *_fname);
		User (USER_DATA *);
		virtual ~User();
		USER_DATA data;						// structure compatible with old data files
		bool canceled;
		inline float getWeight(void);
		inline float get_watts_factor(void)  { 
			return watts_factor;
		}
		void set_watts_factor(float _f);
		inline int get_version(void)  { return version; }
		float ftp;																// functional threshold power
		inline float get_ftp(void)  { return ftp; }
};

#ifdef WIN32
	#ifdef DOC
		}					// extern "C" {
	#endif
#endif

#endif			// #ifndef NEWUSER



#endif		// #ifndef _USER_H_

