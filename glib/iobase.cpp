/////////////////////////////////////////////////////////////////////////////
// Name:        iobase.cpp
// Purpose:
// Author:      Joachim Buermann
// Id:          $Id: iobase.cpp,v 1.1.1.1 2004/11/24 10:30:11 jb Exp $
// Copyright:   (c) 2001 Joachim Buermann
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "iobase.h"
//#include <timer.h>
#include "timer.h"					// ffmpeg now has a timer.h

#define DELTA_BUFSIZE 512 

/*
  Readv() calls the member function rx() repeatedly, til all
  demand bytes were received. To avoid an endless loop, you
  can refer an integer, which was set unequal zero after a
  specific time. (See the timer class)
*/
//int wxIOBase::Readv(char* buf,size_t len,int* timeout_flag,bool nice)

/********************************************************************************

********************************************************************************/

int wxIOBase::Open(void* dcs)  {

    if(!dcs)  {
        return -1;
    }

    int fd;

    /*
    SerialPort_DCS m_dcs;
    m_dcs = *(SerialPort_DCS*)dcs;
    */

    fd = OpenDevice(dcs);           // int Serial::OpenDevice(void* dcs),   returns fd
    if (fd==-1)  {
        bp = 1;
    }

    return fd;
}                       // int wxIOBase::Open(void* dcs)


/********************************************************************************
    used by the nvram class
********************************************************************************/

int wxIOBase::Open(const char *_portstr, void* dcs)  {

    if(!dcs)  {
        return -1;
    }

    int fd;

    /*
    SerialPort_DCS m_dcs;
    m_dcs = *(SerialPort_DCS*)dcs;
    */

    fd = OpenDevice(dcs);           // int Serial::OpenDevice(void* dcs),   returns fd

    if (fd==-1)  {
        /*
        try_usb = true;
        fd = OpenDevice(dcs);           // int Serial::OpenDevice(void* dcs),   returns fd
        if (fd==-1)  {
            bp = 1;
        }
        */
    }

    return fd;
}                           //int wxIOBase::Open(const char *_portstr, void* dcs)



int wxIOBase::Read(char* buf,size_t len,int* timeout_flag,bool nice)
{
    size_t toread = len;
    int n = 0;
    char *cp = buf;

    while(toread > 0) {
	   if(timeout_flag && (*timeout_flag > 0)) {
		  return (len - toread);
	   }
	   if((n = rx(cp,toread)) < 0) {
		  return (len - toread); 
	   }
	   if(!n && nice) {
		  sleepms(1);
	   }
	   if (n > 0) 
	   {
		  toread -= n;
		  cp += n;
	   }
    }
    // ok, all bytes received
    return(len - toread);
}

int wxIOBase::ReadUntilEOS(char** readbuf,char* eosString,long timeout_in_ms)
{
    int n = 0;
    int timeout = 0;
    int bufsize = DELTA_BUFSIZE;
    char* buf = new char[bufsize];
    char* des = buf;
    char* eos = eosString;
    char ch;

    timer t(timeout_in_ms,&timeout,NULL);
    t.start();

    while(!timeout && (*eos != '\0')) {
	   // read next byte
	   n = rx(&ch,1);
	   if(n < 0) {
		  // an error occured
		  break;
	   }
	   else if(n == 0) {
		  // no data available, give up the processor for some time
		  // to reduce the cpu last
		  sleepms(10);
		  continue;
	   }
	   // check for eos
	   if(ch == *eos) {
		  eos++;
		  continue;
	   }
	   if(des >= &buf[bufsize]) {
		  // buffer full, realloc more memory
		  char* tmp = new char[bufsize + DELTA_BUFSIZE];
		  memcpy(tmp,buf,bufsize);
		  delete buf;
		  buf = tmp;
		  des = &buf[bufsize];
		  bufsize += DELTA_BUFSIZE;
	   }
	   *des++ = ch;
    }
    *readbuf = buf;
    return des - buf;
}

/*
  Similar to Readv(). Writev() calls Write() repeatedly till
  all bytes are written.
*/
//int wxIOBase::Writev(char* buf,size_t len,int* timeout_flag,bool nice)
int wxIOBase::Write(char* buf,size_t len,int* timeout_flag,bool nice)
{
    size_t towrite = len;
    int n = 0;
    char *cp = buf;

    while(towrite > 0) {
	   if(timeout_flag && (*timeout_flag > 0)) {
		  return (len - towrite);
	   }
	   if((n = tx(cp,towrite)) < 0) {
		  // an error occurs
		  return (len - towrite);
	   }
	   if(!n && nice) {
		  sleepms(1);
	   }
	   towrite -= n;
	   cp += n;
    }
    return(len);
}
