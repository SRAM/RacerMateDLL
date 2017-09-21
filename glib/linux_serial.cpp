
////////////////////////////// LINUX  VERION //////////////////////////////////

#ifndef _LINUX_SERIAL_CPP_
#define _LINUX_SERIAL_CPP_

#include <serial.h>
#include <logger.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <utils.h>


/**********************************************************************

**********************************************************************/

Serial::Serial() : Serialx() {
   fd = -1;
}

/**********************************************************************
    _portstr = "com1", eg
**********************************************************************/

Serial::Serial(const char *_portstr, const char *_baudstr, Logger *_logg)  {

    init();

   //strcpy(comportstr, "com1");

   //port = new Serial();
    int baud;
    portnum = atoi(&_portstr[3]);
    baud = atoi(_baudstr);
   dcs.baud = (wxBaud) baud;
   dcs.parity = wxPARITY_NONE;
   dcs.wordlen = 8;
   dcs.stopbits = 1;
   dcs.rtscts = false;
   dcs.xonxoff = false;
   Open(&dcs);            // tries to open portnum, returns fd (int)

   if (fd==-1)  {
       try_usb = true;
       Open(&dcs);
       bp = fd;
    }

   //printf("baud = %d\r\n", baud);

}


/**********************************************************************

**********************************************************************/

Serial::~Serial() {
   Close();
}

/**********************************************************************

**********************************************************************/

speed_t Serial::AdaptBaudrate(wxBaud baud) {
	switch(baud) {
		case wxBAUD_150: return B150;
		case wxBAUD_300: return B300;
		case wxBAUD_600: return B600;
		case wxBAUD_1200: return B1200;
		case wxBAUD_2400: return B2400;
		case wxBAUD_4800: return B4800;
		case wxBAUD_9600: return B9600;
		case wxBAUD_38400: return B38400;
		case wxBAUD_57600: return B57600;
		case wxBAUD_115200: return B115200;
		case wxBAUD_230400: return B230400;
		case wxBAUD_460800: return B460800;
		case wxBAUD_921600: return B921600;
		default: return B19200;
	}
}

/**********************************************************************

**********************************************************************/

int Serial::CloseDevice() {
	int err = 0;
	// only close an open file handle
	if(fd < 0) return EBADF;
	// With some systems, it is recommended to flush the serial port's 
	// Output before closing it, in order to avoid a possible hang of
	// the process...
	// Thanks to Germain (I couldn't answer you, because your email
	// address was invalid)
	tcflush(fd, TCOFLUSH);
	// recovery original settings
	tcsetattr(fd,TCSANOW,&save_t);
	// and close device
	err = close(fd);
	fd = -1;
	return err;
}

/**********************************************************************

**********************************************************************/

int Serial::ChangeLineState(wxSerialLineState flags) {
	return ioctl(fd,TIOCMSET,&flags);
}

/**********************************************************************

**********************************************************************/

int Serial::ClrLineState(wxSerialLineState flags) {
	return ioctl(fd,TIOCMBIC,&flags);
}

/**********************************************************************

**********************************************************************/

int Serial::GetLineState(wxSerialLineState* flags) {
	return ioctl(fd,TIOCMGET,flags);
}

/**********************************************************************

**********************************************************************/

int Serial::GetSettingsAsString(char* str, size_t size) {
	const char ac[5] = {'N','O','E','M','S'};
	return snprintf(str,size,"%i%c%i %i %s",
			m_dcs.stopbits,
			ac[m_dcs.parity],
			m_dcs.wordlen,
			m_dcs.baud,
			devname);
}

//
// included from /usr/include/linux/serial.h
//
//  struct serial_icounter_struct {
//  	int cts, dsr, rng, dcd;
//  	int rx, tx;
//  	int frame, overrun, parity, brk;
//  	int buf_overrun;
//  	int reserved[9];
//  };
//
/**********************************************************************

**********************************************************************/

int Serial::Ioctl(int cmd, void* args) {
	int count = 0;
	int err = 0;
	struct serial_icounter_struct info;
	wxSerialPort_EINFO einfo;

	switch(cmd) {
		case CTB_RESET:
			return SendBreak(0);
		case CTB_SER_GETEINFO:
			err = ioctl(fd,TIOCGICOUNT,&info);
			if(err) return err;
			einfo.brk = info.brk - save_info.brk;
			einfo.frame = info.frame - save_info.frame;
			einfo.overrun = info.overrun - save_info.overrun;
			einfo.parity = info.parity - save_info.parity;
			*(wxSerialPort_EINFO*)args = einfo;
			break;
		case CTB_SER_GETBRK:
			err = ioctl(fd,TIOCGICOUNT,&info);
			if(err) return err;
			if(last_info.brk != info.brk) count = 1;
			break;
		case CTB_SER_GETFRM:
			err = ioctl(fd,TIOCGICOUNT,&info);
			if(err) return err;
			if(last_info.frame != info.frame) count = 1;
			break;
		case CTB_SER_GETOVR:
			err = ioctl(fd,TIOCGICOUNT,&info);
			if(err) return err;
			if(last_info.overrun != info.overrun) count = 1;
			break;
		case CTB_SER_GETPAR:
			err = ioctl(fd,TIOCGICOUNT,&info);
			if(err) return err;
			if(last_info.parity != info.parity) count = 1;
			break;
		case CTB_SER_GETINQUE:
			err = ioctl(fd,TIOCINQ,&count);
			if(err) return err;
			*(int*)args = count;
			return 0;
		default:
			return -1;
	}
	last_info = info;
	return 0;
}

/**********************************************************************

**********************************************************************/

bool Serial::is_open(void)  {
    return (fd != -1);
}

/**********************************************************************

**********************************************************************/

int Serial::OpenDevice(void* dcs) {
    // if dcs isn't NULL, type cast
    //char devname[32];

    if(dcs)  {
        m_dcs = *(SerialPort_DCS*)dcs;
    }

    if (!try_usb)  {
        sprintf(devname, "/dev/tty%d", portnum-1);								// #define wxCOM1 "/dev/ttyS0"
    }
    else  {
        sprintf(devname, "/dev/ttyUSB%d", portnum-1);								// #define wxCOM1 "/dev/ttyS0"
    }


    // open serial comport device for reading and writing, don't wait (O_NONBLOCK)

    fd = open(devname, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if(fd >= 0) {
        if (try_usb)  {
            usb = true;
        }
        
	tcgetattr(fd,&t);
	save_t = t;

	// save the device name
	//strncpy(m_devname,(char*)devname,sizeof(m_devname));
	// we write an eos to avoid a buffer overflow
	//m_devname[sizeof(m_devname)-1] = '\0';

	// fill the internal terios struct
	// default input/output speed is 19200 baud
	cfsetispeed(&t,AdaptBaudrate(m_dcs.baud));
	cfsetospeed(&t,AdaptBaudrate(m_dcs.baud));
	// parity settings
	if(m_dcs.parity == wxPARITY_NONE) 
            t.c_cflag &= ~PARENB;
	else {
            t.c_cflag |= PARENB;
            if(m_dcs.parity == wxPARITY_ODD)
                t.c_cflag |= PARODD;
            else
                t.c_cflag &= ~PARODD;
	}
	// stopbits
	if(m_dcs.stopbits == 2)
            t.c_cflag |= CSTOPB;
	else
            t.c_cflag &= ~CSTOPB;

        // wordlen
        t.c_cflag &= ~CSIZE;
	if(m_dcs.wordlen == 7) t.c_cflag |= CS7;
	else if(m_dcs.wordlen == 6) t.c_cflag |= CS6;
	else if(m_dcs.wordlen == 5) t.c_cflag |= CS5;
	// this is the default
	else t.c_cflag |= CS8;
	// rts/cts
	if(m_dcs.rtscts == false)
            t.c_cflag &= ~CRTSCTS;
	else
            t.c_cflag |= CRTSCTS;

	t.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
	t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | IXOFF | IXANY);
	t.c_iflag |= IGNPAR;
	t.c_oflag &= ~OPOST;

	if(m_dcs.xonxoff == true) {
            t.c_iflag |= (IXON | IXOFF);
	}

	// look out!
	// MIN = 1 means, in TIME (1/10 secs) defined timeout
	// will be started AFTER receiving the first byte
	// so we must set MIN = 0. (timeout starts immediately, abort
	// also without readed byte)
	t.c_cc[VMIN] = 0;
	// timeout in 1/10 secs
	// no timeout for non blocked transfer
	t.c_cc[VMIN] = 0;
	// write the settings
	tcsetattr(fd,TCSANOW,&t);
	// it's careless, but in the moment we don't test
	// the return of tcsetattr (normally there is no error)

	// request the actual numbers of breaks, framing, overrun
	// and parity errors (because Linux summing all of them during
	// system lifetime, not only while serial port is open.
	ioctl(fd,TIOCGICOUNT,&save_info);
	// it's also careless, but we assume, that there was no error
	last_info = save_info;
    }

    else  {
        const char *cptr;
    
        bp = 1;

	switch(errno)  {
            case EEXIST:		// pathname already exists and O_CREAT and O_EXCL were used.
                cptr = strerror(errno);
		break;
            case EISDIR:		// pathname refers to a directory and the access requested involved writing (that is, O_WRONLY or O_RDWR is set).
                cptr = strerror(errno);
		break;
            case EACCES:		// permission denied
		cptr = strerror(errno);
		break;
            case ENAMETOOLONG:		// pathname was too long.
		cptr = strerror(errno);
		break;
            case ENOENT:		// O_CREAT is not set and the named file does not exist. Or, a directory component in pathname does not exist or is a dangling symbolic link.
		cptr = strerror(errno);
		break;
            case ENOTDIR:		// A component used as a directory in pathname is not, in fact, a directory, or O_DIRECTORY was specified and pathname was not a directory.
		cptr = strerror(errno);
		break;
            case ENXIO:		// O_NONBLOCK | O_WRONLY is set, the named file is a FIFO and no process has the file open for reading. Or, the file is a device special file and no corresponding device exists.
		cptr = strerror(errno);
		break;
            case ENODEV:		// pathname refers to a device special file and no corresponding device exists. (This is a Linux kernel bug - in this situation ENXIO must be returned.)
		cptr = strerror(errno);
		break;
            case EROFS:		// pathname refers to a file on a read-only filesystem and write access was requested.
		cptr = strerror(errno);
		break;
            case ETXTBSY:		// pathname refers to an executable image which is currently being executed and write access was requested.
		cptr = strerror(errno);
		break;
            case EFAULT:		// pathname points outside your accessible address space.
		cptr = strerror(errno);
		break;
            case ELOOP:		// Too many symbolic links were encountered in resolving pathname, or O_NOFOLLOW was specified but pathname was a symbolic link.
		cptr = strerror(errno);
		break;
            case ENOSPC:		// pathname was to be created but the device containing pathname has no room for the new file.
		cptr = strerror(errno);
		break;
            case ENOMEM:		// Insufficient kernel memory was available.
		cptr = strerror(errno);
		break;
            case EMFILE:		// The process already has the maximum number of files open.
		cptr = strerror(errno);
		break;
            case ENFILE:		// The limit on the total number of files open on the system has been reached.
		cptr = strerror(errno);
		break;
            default:
		cptr = "TLM unknown error";
		bp = 0;
		break;
	}

        sprintf(error_string, "can't open %s: %s\r\n", devname, cptr);
    }

    return fd;
}

/**********************************************************************

 **********************************************************************/

//int wxSerialPort::Read(char* buf,size_t len)
int Serial::rx(char* buf, long len) {
	// Read() (using read() ) will return an 'error' EAGAIN as it is 
	// set to non-blocking. This is not a true error within the 
	// functionality of Read, and thus should be handled by the caller.
	int n = read(fd,buf,len);
	if(n < 0)  {
		if (errno == EAGAIN)  {
			return 0;
		}
		return 0;
	}

	return n;
}

/**********************************************************************

 **********************************************************************/

int Serial::SendBreak(int duration) {
	// the parameter is equal with linux
	return tcsendbreak(fd,duration);
}

/**********************************************************************

 **********************************************************************/

int Serial::SetBaudRate(wxBaud baudrate) {
	speed_t baud = AdaptBaudrate(baudrate);
	// setting the input baudrate
	if(cfsetispeed(&t,baud) < 0) {
		return -1;
	}
	// setting the output baudrate
	if(cfsetospeed(&t,baud) < 0) {
		return -1;
	}
	// take over
	m_dcs.baud = baudrate;
	return tcsetattr(fd,TCSANOW,&t);    
}

/**********************************************************************

 **********************************************************************/

int Serial::SetLineState(wxSerialLineState flags) {
	return ioctl(fd,TIOCMBIS,&flags);
}

/**********************************************************************

 **********************************************************************/

//int wxSerialPort::Write(char* buf,size_t len)
int Serial::txx(const unsigned char* buf,size_t len) {
	// Write() (using write() ) will return an 'error' EAGAIN as it is 
	// set to non-blocking. This is not a true error within the 
	// functionality of Write, and thus should be handled by the caller.
	int n = write(fd,buf,len);
	if((n < 0) && (errno == EAGAIN)) return 0;
	return n;
}

/**********************************************************************

 **********************************************************************/

//int wxSerialPort::Write(char* buf,size_t len)
int Serial::tx(const char* buf, size_t len) {
	// Write() (using write() ) will return an 'error' EAGAIN as it is 
	// set to non-blocking. This is not a true error within the 
	// functionality of Write, and thus should be handled by the caller.
	int n = write(fd,buf,len);
	if((n < 0) && (errno == EAGAIN)) return 0;
	return n;
}

/**********************************************************************

**********************************************************************/

void Serial::send(const char* buf, int flush_flag)  {
	write(fd, buf, strlen(buf) );
	/*
	if((n < 0) && (errno == EAGAIN))  {
		return 0;
	}
	return n;
	*/
	return;
}

/**********************************************************************
	waits for a result
   returns 0 if it is found, 1 otherwise.
**********************************************************************/

int Serial::expect(const char *str, DWORD timeout)   {
	int len,i=0;
	unsigned char c[8];
	DWORD start,end;
	int n;

	len = strlen(str);

	start = timeGetTime();

	while(1)  {
		n = rx((char *)c, 1);

      if (n==1)  {
			if (c[0]==str[i])  {
         	i++;
				if (i==len)  {
					return 0;
				}
			}
			else  {
				i = 0;
			}
		}
		else  {
			end = timeGetTime();
         if ((end-start)>timeout)  {
				return 1;
         }
		}
	}

	return 1;
}

/**********************************************************************

**********************************************************************/

void Serial::flushrx(DWORD ms)  {
	int total;

	if (ms > 0)  {
		DWORD then, now;
		int n;
		char buf[32];

		then = timeGetTime();
		total = 0;

		while(1)  {
			n = rx((char *)buf, sizeof(buf));
			if (n==0)  {
				now = timeGetTime();
				if ((now - then) > ms) {
					break;;								// no comms for 1 sec
				}
				continue;
			}
			else  {
				total += n;
				then = timeGetTime();
			}
		}
	}

	/*
	if(hCommPort != NULL)  {
		PurgeComm(hCommPort, PURGE_RXABORT | PURGE_RXCLEAR);
	}
	*/
	
	//rxoutptr = rxinptr;
}						// void Serial::flushrx(DWORD ms) {

/**********************************************************************

**********************************************************************/

int Serial::getPortNumber(void) {

	return portnum;
}

/**********************************************************************

**********************************************************************/

void Serial::flush_tx(void)  {

    return;
}

/**********************************************************************

**********************************************************************/

void Serial::init(void)  {
    memset(error_string, 0, sizeof(error_string));
    bp = 0;
    try_usb = false;
    usb = false;
    
    return;
}



#ifdef _DEBUG
/**********************************************************************

**********************************************************************/

void Serial::flush_rawstream(void)  {

	if (rawstream)  {
		fflush(rawstream);
		fclose(rawstream);
	}
	//rawstream = fopen(RAWNAME, "a+b");
	return;
}

#endif


#endif				// #ifndef _LINUX_SERIAL_CPP_


