
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#include "utils.h"
#include "unix_serial.h"

/*********************************************************************
	constructor 1
*********************************************************************/

Serial::Serial(const char *_device,	const char *pszBaudRate,	Logger *_logg)  {

	//strncpy(baudstr, pszBaudRate, sizeof(baudstr)-1);

	init();
	
	logstream = fopen("serial.log", "wt");

	log("serial constructor 1\n", true);



	//strncpy(ident, _ident, sizeof(ident)-1);
	strncpy(device, _device, sizeof(device)-1);
	baud = atoi(pszBaudRate);
	switch(baud)  {
		case 2400:
			baud = B2400;
			break;
		case 9600:
			baud = B9600;
			break;
		case 38400:
			baud = 38400;
			break;
		default:
			baud = B9600;
			break;
	}

	//ttymode = N8R;
	//ttymode = _ttymode;
    
	opn();

	return;
}


/*********************************************************************
	constructor 2
	_baud is the REAL baud rate
*********************************************************************/

Serial::Serial( char *_ident, char *_device, int _baud, int _ttymode)  {


	init();

	logstream = fopen("serial.log", "wt");

	log("serial constructor 2\n", true);

	strncpy(ident, _ident, sizeof(ident)-1);
	strncpy(device, _device, sizeof(device)-1);

	switch(_baud)  {
		case 2400:
			baud = B2400;
			break;
		case 9600:
			baud = B9600;
			break;
		case 38400:
			baud = 38400;
			break;
		default:
			baud = B9600;
			break;
	}



	 ttymode = _ttymode;

	sprintf(str, "device = %s, ident = %s, baud = %d, ttymode = %d\n", device, ident, baud, ttymode);
    log(str, true);

    opn();

	return;			
}


/*********************************************************************
	default constructor
*********************************************************************/

Serial::Serial(void)  {

	init();

	return;

}

/*********************************************************************

*********************************************************************/


Serial::~Serial()  {
	clse();
	FCLOSE(logstream);
}

/*********************************************************************

*********************************************************************/


const char *Serial::get_port_name(void)  {
	return device;
}


/******************************************************************************************************
 read serial input, look for packed data or RacerMate
 returns 1 if new control message is in pkt[],
 2 if 'RacerMate' string matched,
 0 if no new chars or not finished pkt or racermate match.

******************************************************************************************************/

long Serial::rx(void)   {
	int i;
    long n;


	n = read(fd, rxbuf, sizeof(rxbuf));

	if (n < 0) {
		return n;
	}

	if (n) {
		for (i=0; i<n; i++)  {
		}                                       // for(i...
	}				//if(n)

	//n = 0, no chars read, or not a new good pkt yet.
	return 0;
}						// rx()


/******************************************************************************************************

******************************************************************************************************/

int Serial::rx(char* buf, long len)  {
	int n;

	n = read(fd, buf, len);

#ifdef _DEBUG
	if (n!=0)  {
		bp = 2;
	}
#endif

	return n;
}


/******************************************************************************************************

******************************************************************************************************/

int Serial::tx(const unsigned char *buf, long len)  {
	long i;

	i = write(fd, (char *)buf, len);

	if (i != len) {
		return 1;
	}
	return 0;
}							// tx()

/******************************************************************************************************

******************************************************************************************************/

int Serial::txx(const unsigned char *buf, long len)  {
	long i;

	i = write(fd, (char *)buf, len);

	if (i != len) {
		return 1;
	}
	return 0;
}							// tx()

/*********************************************************************

*********************************************************************/

int Serial::send(const char *str, int flush_flag)  {
	long i;
	int len;

	len = strlen(str);

	i = write(fd, (char *)str, len);

	if (i != len) {
		return 1;
	}

	if (flush_flag)  {
		//fflush(fd);
	}

	return 0;

}							// send()

/******************************************************************************************************
 
******************************************************************************************************/

void Serial::opn(void) {
	struct termios t;
	int i = -1;
	
	fd = open(device, O_RDWR|O_NONBLOCK);

	if (fd<0) {
		log("failed to open device\n", true);
		return;
	}

	sprintf(str, "device = %d\n", fd);

	log(str, true);

	i = ttymode;


	tcgetattr(fd, &t);
	
	sprintf(str, "i = %d\n", i);
   log(str, true);

	switch (i) {
		case N8R:
			log("N8R\n");
			t.c_iflag = (IGNBRK);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|CREAD|CS8);
			t.c_lflag = 0x0;
			t.c_cc[VMIN]=0;
			t.c_cc[VTIME]=0;
			break;
		case N8R1:
			t.c_iflag = (IGNBRK);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|CREAD|CS8);
			t.c_lflag = 0x0;
			t.c_cc[VMIN]=1;
			t.c_cc[VTIME]=0;
			break;
		case E7R:
			t.c_iflag = (IGNBRK|ISTRIP|ICRNL);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|CREAD|PARENB|CS7);
			t.c_lflag = 0x0;
			t.c_cc[VMIN]=240;
			t.c_cc[VTIME]=64;
			break;
		case N8CRLF:
			t.c_iflag = (IGNBRK|ISTRIP|IGNCR);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|CREAD|CS8);
			t.c_lflag = (ICANON);
			break;
		case N8CR:
			t.c_iflag = (IGNBRK|ISTRIP|ICRNL);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|CREAD|CS8);
			t.c_lflag = (ICANON);
			break;
		case E7CRLF:
			t.c_iflag = (IGNBRK|ISTRIP|IGNCR);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|PARENB|CREAD|CS7);
			t.c_lflag = (ICANON);
			break;
		case E7CR:
		default:
			t.c_iflag = (IGNBRK|ISTRIP|ICRNL);
			t.c_oflag = 0x0;
			t.c_cflag = (CLOCAL|HUPCL|PARENB|CREAD|CS7);
			t.c_lflag = (ICANON);
			break;
	}


	cfsetispeed(&t, baud);
	cfsetospeed(&t, baud);
	int status = tcsetattr(fd, TCSANOW, &t);
	bp = 4;
    
	return;
}

/******************************************************************************************************

******************************************************************************************************/

int Serial::clse(void) {
	int stat= -1;

	if (fd >=0) {
		sprintf(str, "closing %d\n", fd);
		log(str, true);
		stat = close(fd);
		fd = -1;
	}

	return stat;
}						// close()

/******************************************************************************************************

******************************************************************************************************/

void Serial::init(void) {

	memset(ident, 0, sizeof(ident));
	memset(device, 0, sizeof(device));
    memset(rxbuf, 0, sizeof(rxbuf));
    
  	logstream = NULL;  

	baud = 0;
	ttymode = N8R;
	fd = -1;
#ifdef _DEBUG
	bp = 0;
#endif
	return;
}

/******************************************************************************************************
 
 ******************************************************************************************************/

//bool Serial::isopen(void) {
bool Serial::is_open(void) {
	 if (fd==-1)  {
        return false;
    }
    return true;
}

/******************************************************************************************************

******************************************************************************************************/

void Serial::log(const char *str, bool flush)  {

	if (!logstream)  {
		return;
	}

	fprintf(logstream, "%s", str);

	if (flush)  {
		fflush(logstream);
	}

	return;
}							// log()




/*********************************************************************

*********************************************************************/

int Serial::expect(const char *str, DWORD timeout)  {
	int len,i=0;
	char c[8] = { 0 };
	DWORD start,end;
	int n;

	len = strlen(str);

	start = timeGetTime();

	while(1)  {
		n = rx(c, 1);

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
#ifdef _DEBUG
				DWORD dt = end - start;
#endif
				break;
			}
		}
		usleep(1000);
	}							// while(1)

	return 1;

}							// expect()

/*********************************************************************

*********************************************************************/

void Serial::setShutDownDelay(unsigned long _ms)  {
	shutDownDelay = _ms;
}													// setShutdownDelay()

