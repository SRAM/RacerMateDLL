#include <unistd.h>
#include <stdio.h>
#include	<memory.h>
#include "nreadudp.h"


#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

//       #define BUF_SIZE 500

//const char *node = "192.168.5.1";
const char *node = NULL;
const	char *port = "9072";
struct addrinfo hints;
struct addrinfo *result, *rp;
extern int sfd;
struct sockaddr_storage peer_addr;
socklen_t peer_addr_len;

extern FILE *logfile;
extern char *s_time_ms;
extern char *s_time_s;
extern int	debug, filler1, filler2, msgsize;
extern unsigned short	xseqnum;

/*

Wwifi udp packets. Source and format indicated by 4 char
string at start. will change some for other formats

 int32s, int16s are in network byte order, ms byte first.

 sent from HB to PC.

 pkt start:

	byte
w:	0	'R'
	1	'M'
	2	'C'
	3	'T'

x:	4	offset to ending checksum byte y-x
	5	~offset, this^offset == 0xff for good packet

a:	6	size of first sect, b-a, add to here to find next size
	7	first section type
		payload
		...
b: 	size of next section, c-b
		next section type
		payload
		...
c:		size x - c
		section type
		lather, rinse, repeat
		...
x:		size = 0 is end of sections
y:		checksum, -sum from w to x 
z:	
	pkt end:

Sections:
 
-----------
First section always sent, device # and pkt sequence #

	0	SECT_DEVNUM
	1	hb devnum hby
	2	hb devnum lby
	3	hb sequence hby
	4	hb sequence lby

-----------

These are sent when receiving valid PC control pkts:

2nd section always sent when HB is under PC control
	0	char	SECT_FAST
	1	char   keys
	2	char   kpress
	3	char mph*256 hby
	4	char mph*256 lby
	5	char watts hby
	6	char watts lby
	7	char   hrflags
	8	char   hrate
	9	char rpmflags
	10	char   rpm

-----------
3rd section Slow hb data, alternates with spinscan

 0 char SECT_SLOW
 1 char minhr
 2 char maxhr
 3 char rdrag*256 hby + cal flag in bit 15
 4 char rdrag*256 lby
 5 char slip hby
 6 char slip lby
 7 char sw version hby
 8 char sw version lby
 9 char status3
 10 char status2
 11 char status1
 12 char status0 

-----------
other 3rd section Spinscan data, alternates with slow hb data

0	SECT_SS
1	char crank turn count hby
2	char crank turn count lby
3	char crank event time hby, 10 ms steps?
4	char crank event time lby
5	char ss peak lbs*256 hby
6	char ss peak lbs*256 lby
7	char bar1, linear 0-200
...
30	char bar24

------------

2nd section		//NOT YET IMPLEMENTED
sent after each idle pkt when getting good PC idle pkts

	0		SECT_HERE
	1       char received device # hby
	2       char received device # lby
	3       char received sequence # hby
	4       char received sequence # lby

------------

2nd section sent at 1/3hz when NOT getting good control pkts

	0		SECT_HERE
	1       char device # hby
	2       char device # lby
	3       char device type, enum of CT, Velotron, Slug, ???
	4       char hb software version hby
	5       char hb software version lby
	6       char source ip addr hi
	7       char source ip addr mh
	8       char source ip addr ml
	9       char source ip addr lo
	10      char source port hby
	11      char source port lby

*/



#define RXQLEN 2048
#define DGMLEN 256

static unsigned char rxq[RXQLEN];		// ring buffer for udp receive or serial in from lpt100 module
static int	rxinptr = 0;							// rxq write index, should use pointers...
static int	rxoutptr = 0;							// read index

static unsigned char dgm[DGMLEN];		// extracted complete udp packet

extern unsigned short	bad_msg;						// bad pkt counter
extern unsigned short	good_msg;						// good pkt counter


unsigned short pcdevnum = 12345;

/*
// used for both directions
enum  {
	SECT_DEVNUM = 1,
	SECT_FAST,
	SECT_SLOW,
	SECT_SS,
	SECT_IDLE,
	SECT_HBCTL,
	SECT_VAR,
	SECT_HERE=255
};
*/

//	section types, used for both directions
#define	SECT_DEVNUM	0x01
#define	SECT_FAST	0x02
#define	SECT_SLOW	0x03
#define	SECT_SS		0x04
#define	SECT_IDLE	0x05
#define	SECT_HBCTL	0x06

#define	SECT_HERE	0xff

#define	SECT_DBG1	0x81
#define	SECT_DBG2	0x82

#define	SECT_VAR		0x60

//prototypes

void dbgout(int dbg, const char *fmt, ...);

int	get_ubermsg(unsigned char* d);	// puts a good RMCT packet payload into &d
												// returns len if complete one found, else 0

int	test_sections(int ix);			// walk section chain, test csm
												// return # of sections found or -1 for bad csm or format

void	decode_fast(int ix, struct nhbdata *d);
void	decode_slow(int ix, struct nhbdata *d);
int	decode_ss(int ix, struct nhbdata *d);

//int	send_ctlmsg(int hb_fd, int msgtype);
int gen_devnum( unsigned char * buf, int ix);
int gen_hbctl( unsigned char * buf, int ix);
int gen_varmsg( unsigned char * buf, int ix, int size);

/****************************************************/
// read from some file descriptor to rxq ring buffer
int	read_input(int fd) {
	unsigned char buf[256];
	int n, i;

	n = read(fd, &buf, sizeof(buf));

	if(n < 0) {
		//handle error somehow;
		return 0;
	}

	if(n > 0) {
		dbgout(0x40, "\n n=%2d:",n);
		rxinptr %= sizeof(rxq);
		for( i = 0; i < n; i++) {
			rxq[rxinptr++] = buf[i];
			rxinptr %= sizeof(rxq);
			dbgout(0x40," %02x", buf[i]);
		}
		dbgout(0x40,"\n ");
	}
	return n;
}

//--------------------------------------------
int	open_udp(void) {
	int	sfd, s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;    /* Allow IPv4 only */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	// s = getaddrinfo(NULL, argv[1], &hints, &result);
	s = getaddrinfo(node, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully bind(2).
	If socket(2) (or bind(2)) fails, we (close the socket
	and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			 break;                  /* Success */
		}
		close(sfd);
	}

	if (rp == NULL) {               /* No address succeeded */
	fprintf(stderr, "Could not bind\n");
	exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	return	sfd;
}
/****************************************************/
// read from open udp socket
int	read_udp(int sfd) {
	int	s, i;
	ssize_t nread;
	unsigned char buf[512];
	char host[NI_MAXHOST], service[NI_MAXSERV];

	peer_addr_len = sizeof(struct sockaddr_storage);
	nread = recvfrom(sfd, buf, 512, 0,
			(struct sockaddr *) &peer_addr, &peer_addr_len);

	if (nread == -1) {
		if(errno == EAGAIN) {
			nread = 0;
		}
	}
	if(nread > 0) {

		if(debug&0x40) {
			s = getnameinfo((struct sockaddr *) &peer_addr,
				 peer_addr_len, host, NI_MAXHOST,
				 service, NI_MAXSERV, NI_NUMERICSERV);
			if (s == 0) {
				dbgout(0x40,"\nReceived %ld bytes from %s:%s\n", (long) nread, host, service);
			} else {
				dbgout(0x40, "getnameinfo: %s\n", gai_strerror(s));
			}
		}
		dbgout(0x40, " n=%2d:",nread);
		rxinptr %= sizeof(rxq);
		for( i = 0; i < nread; i++) {
			rxq[rxinptr++] = buf[i];
			rxinptr %= sizeof(rxq);
			dbgout(0x40," %02x", buf[i]);
		}
		dbgout(0x40,"\n ");
	}
	return (int) nread;
}
		
/****************************************************/
// read a byte from rxq. return byte or -1 if none
int	read_rxq(void) {
	int c = -1;
	if(rxinptr != rxoutptr) {
		rxoutptr %= sizeof(rxq);
		c = rxq[rxoutptr++];
		rxoutptr %= sizeof(rxq);
	}
	return c;
}

/****************************************************/
// test if any chars in rxq
int	check_rxq(void) {
	return (rxinptr-rxoutptr)%sizeof(rxq);
}

/****************************************************/
// get any pkts, decode them. Call at 10hz?
// reads from rxq until empty or the end of one RMCT msg
// return 0 if no new pkts decoded.
// return n < 0 for errors, E_CSM, ...
// return n > 0 is number of sections decoded, 1 - ?

int	handle_pkts(void) {

	int	devnum, seqnum;
	int	nsections;
	int	i,j,k;
	int	ix;
	int	b;
	int	dgmlen;
	int	retval = 0;
	float	sst[24];

// doesn't work somehow, using nhbd directly
//	struct nhbdata *hbd = &nhbd;		// defined in readudp.h, default destination

	b = check_rxq();

	if(b) dbgout(0x100,"\n%d new bytes\n", b);

	while(check_rxq()) {				// loop until no more bytes to read

	//dbgout(0x100," here2\n");
		dgmlen = get_ubermsg(dgm);		// fetch next RMCT payload

		if(dgmlen == 0) {
			retval = 0;
			break;						// rxq is empty, might as well exit while
		//	continue;
		}

		if(dgmlen < 0 ) {
			dbgout(0x100,"  Bad RMCT, err= %d\n", dgmlen);
			retval = dgmlen;
			break;					// return with error
		}

		// dgmlen > 0 here, got a good RMCT wrapper

		if(dgm[0] == 0) {
			dbgout(0x100," No sections in msg\n");
			retval = E_NSECT;
			break;
		}

		// verify the section chain

		for( i = dgm[0]; (i<dgmlen); i += dgm[i]) {
			if(dgm[i] == 0) {
				break;
			}
		}
		if(i != (dgmlen-1)) {
			dbgout(0x100," Bad chain: i= %d  dgmlen= %d\n", i, dgmlen);
			retval = E_WALK;
			break;
		}

		// first section MUST be devnum
		if(dgm[1] != SECT_DEVNUM) {

			dbgout(0x100," Not devnum sect: %d\n", dgm[1]);
			retval = E_NDEV;
			break;
		}
		devnum = dgm[2]<<8 | dgm[3];
		seqnum = dgm[4]<<8 | dgm[5];
		nhbd.devnum = devnum;
		nhbd.pktnum = seqnum;

		dbgout(0x100,"devnum= %d  seqnum= %d\n", devnum, seqnum);

		ix = dgm[0];				// skip over devnum section
		retval = 1;					// count devnum sect for return

		while( ix < dgmlen && dgm[ix] != 0) {
				
			dbgout(0x100,"  ix= %d dgm[ix]= %d  ", ix, dgm[ix]);

			switch(dgm[ix+1]) {			// section types
				case	SECT_FAST:
					decode_fast(ix+2, &nhbd);		// payload follows size, section
					retval++;
					dbgout(0x100,"got FAST: keys= %02x kp= %02x mph= %0.1f watts= %d  rpm= %3d\n",
							nhbd.keys, nhbd.keypress, nhbd.f_mph, nhbd.watts, nhbd.rpm);
					break;
				case	SECT_SLOW:
					decode_slow(ix+2, &nhbd);
					retval++;
					dbgout(0x100,"got SLOW: rdrag=%0.2f%c  slip= %d vers=%4.2f stat= %08x\n",
							nhbd.f_rdrag, nhbd.rd_measured?'c':'U', nhbd.slip, (float)nhbd.version/100.0F, nhbd.stat);
					break;
				case	SECT_SS:
					decode_ss(ix+2, &nhbd);
					retval++;
					dbgout(0x100,"got SS: turns= %d  dt= %d  peak= %4.2f\n  bars:",
						nhbd.turns, nhbd.deltat, nhbd.f_peak*200);

					for(i = 0; i<24; i++) {
						dbgout(0x100," %5d", nhbd.bars[i]);
					}
					dbgout(0x100,"\n  lbs: "); 
					for(i = 0; i<24; i++) {
						dbgout(0x100,"%6.1f", nhbd.f_peak * (float) nhbd.bars[i]);
					}
					dbgout(0x100,"\n");
					break;
				case	SECT_HERE:
					dbgout(0x100,"got HERE: decoded RealSoonNow\n");
					retval++;
					break;

				default:
					dbgout(0x100,"got UNKNOWN type= %d len= %d\n",
							dgm[ix+1], dgm[ix]);
					b = 0;
					dbgout(0x100,"          ");
					for(i=ix+2; i < ix+dgm[ix]; i++) {
						b++;
						dbgout(0x100," %02x", dgm[i]);
						if(b%24 == 0) {
						dbgout(0x100,"\n          ");
						}
					}
					dbgout(0x100,"\n");
					break;
			}
			ix += dgm[ix];			// skip to next section
		}	// while more sections

		//dbgout(0x100,"retval= %2d  bad= %d  good= %d\n\n", retval, bad_msg, good_msg);


	}	// while any chars in rxq
	if(retval > 0) good_msg++;
	if(retval < 0) bad_msg++;
	if(retval) dbgout(0x100,"retval= %2d  bad= %d  good= %d\n\n", retval, bad_msg, good_msg);
	return retval;
}
/****************************************************/
/****************************************************/
// read from rxq[], look for RMCT<len><~len><bytes..><csm>
//
// returns zero if rxq empty and no complete msg yet.
// -1 if csm bad, -2 if length bad, -3 if ~len not match
// returns section(s) in &d, ret value is length of &d
// &d starts at first section length byte

int	get_ubermsg(unsigned char *d) {
	
	const char*  header = "RMCT";

	static	int csum;
	static	int minptr;
	static	int msglen;
	static	int mstate;
	static	int datalen;
	static	unsigned char buf[128];
	
	int	c;

	while(1) {

		c = read_rxq();				// get next input byte or -1 if rxq is empty

		if(c < 0) {
			return	0;
		}
//		dbgout(0x100," %d:%02x",mstate, c);
		csum += c;							// add every byte to checksum

		switch(mstate) {
				
			case	0:									// looking for "RMCT"
				csum = c;							// init checksum to first char
			case	1:
			case	2:
			case	3:
				if(c != header[mstate++]) {
					mstate = 0;					// no match, start again
				}
				break;

			case	4:
				msglen = c;				// byte 4 is uberlength from here upto checksum
				mstate++;

				if(msglen < 2 || msglen >= (sizeof(buf) - 1)) {
					mstate = 0;					// go back to looking for RMCT
					return E_LEN;
				}
				break;

			case	5:				// byte 4 is ~uberlength, must match len
				if((c ^ msglen) != 0xff) {
					mstate = 0;					// go back to looking for RMCT
					return E_CLEN;
					break;
				}

				minptr = 0;			// start buffer on first section size byte
				datalen = msglen - 2;  // starting after msglen, ~msglen 
				mstate++;
				break;

			case	6:									// reading msglen chars into buf 
				buf[minptr++] = c;
				if(minptr >= datalen) {
					mstate++;				// adv state after last data, next char will be csm
				}
				break;

			case	7:				// csm byte came in
				mstate = 0;
				if(csum & 0xff) {
					return E_CSM;				// return -1 on bad csm
				}
				memcpy(d, buf, msglen);	// good pkt, copy to caller
				return	minptr;			// return size of dgm, may be 1

			default:
				mstate = 0;
				break;

		}	//switch

		//dbgout(0x100," %d:%02x\n",mstate, c);
	}		// while forever
}

/****************************************************/
	
/****************************************************/

// decode spinscan into struct nhbdata
// entry ix -> turn hby
// returns the # of new crank events, 0 is no new ones

int	decode_ss(int ix, struct nhbdata *d)	{
	unsigned short turns, time, peak;
	float f_peak;
	int i,j;

	j = ix;
	turns = dgm[j++] << 8;
	turns |= dgm[j++];

	time = dgm[j++] << 8;
	time |= dgm[j++];

	peak = dgm[j++] << 8;	//highest bar is 200. peak is lbs*256 at bar==200
	peak |= dgm[j++];
	f_peak = (float)peak /( 200.0F * 256.0F);		// bar[]*f_peak -> lbs thrust
	d->f_peak = f_peak;

	for(i = 0; i < 24; i++) {
		d->bars[i] = dgm[j++];
	}

	if((d->turns = (turns - d->prevturns)) & 0xffff) {
		d->deltat = (time - d->prevtime) & 0xffff;
		d->prevturns = turns;
		d->prevtime = time;
	}
	return d->turns;
}
		

/*******************************************************************************/
/*******************************************************************************/
//
// decode section SECT_FAST, fast hb data.
// enter with ix -> keys

void	decode_fast(int ix, struct nhbdata *d)	{
	//int i;
	int j;
	unsigned short in;

	j = ix;

	d->keys = dgm[j++];				// keys
	d->keypress = dgm[j++];			// new key presses

	in = dgm[j++] << 8;				// int mph*256
	in |= dgm[j++];
	d->f_mph = (float)in / 256.0F;

	in = dgm[j++] << 8;				// watts
	in |= dgm[j++];
	d->watts = in;

	d->hrflags = dgm[j++];
	d->hrate = dgm[j++];

	d->rpmflags = dgm[j++];
	d->rpm = dgm[j++];

}
		

/*******************************************************************************/
// section SECT_SLOW, enter with ix at minhr
//
void	decode_slow(int ix, struct nhbdata *d )	{
	//int i;
	int in,j;

	j = ix;

	d->minhr	=	dgm[j++];
	d->maxhr	=	dgm[j++];

	in = dgm[j++] << 8;				// rdrag * 256
	in |= dgm[j++];
	d->rd_measured =  (in & 0x8000) ? 1 : 0;
	in &= 0x7fff;
	d->f_rdrag = (float)in / 256.0F;


	in = dgm[j++] << 8;				// slip count
	in |= dgm[j++];
	d->slip = in;

	in = dgm[j++] << 8;				// version*100
	in |= dgm[j++];
	d->version = in;

	in = dgm[j++] << 24;				// status ms byte
	in |= dgm[j++] << 16;
	in |= dgm[j++] << 8;
	in |= dgm[j++];				// status ls byte
	d->stat = in;

}
		

/*******************************************************************************/
// send pc control msg.


int	send_ctlmsg(int hb_fd, int msgtype) {

//	static unsigned short	xseqnum = 0;
	unsigned char obuf[256];		// = { 'R','M','P','C' };

	int	i, b;
	int	start, secstart;
	int	j = 0;			// obuf index starting after RMPC
	int	csm, cs, csstart;

	//fprintf(logfile,"snd_ctlmsg: type= %d  fd= %d\n", msgtype, hb_fd);

	if( msgtype == 0) {
		return 0;
	}
	xseqnum++;

	for(j=0;j<filler1;j++) {
		obuf[j]=0xcc;
	}
	csstart = j;
	obuf[j++] = 'R';
	obuf[j++] = 'M';
	obuf[j++] = 'P';
	obuf[j++] = 'C';

	start = j++;			// save payload start ix for overall length, leave room for it
	j++;						// space for ~start

	j = gen_devnum(obuf, j);

	if(msgtype & 0x02) {
		if(msgsize) {
			j = gen_varmsg(obuf, j, msgsize);
		} else {
			j = gen_hbctl(obuf, j);
		}

	}

	obuf[j++] = 0;				// put section = 0 for end of data

	obuf[start] = j - start;		// put payload size to overall byte count
	obuf[start+1] = ~obuf[start];		// put complement of payload size to overall byte count

	csm = 0;
	for( i=csstart; i<j; i++) {
		csm += obuf[i];		// checksum from start to sect 0
		csm &=0xff;
	}

	obuf[j++] = -csm  & 0xff;	// should add up to zero

	for(i=0;i<filler2;i++) {
		obuf[j++] = 0xaa;
	}
	
	if(debug&0x80) {
		fprintf(logfile," hbdevnum= %d sent %d bytes csm= %02x  f1= %d  f2= %d  sz= %d",
			  	nhbc.hbdevnum, j, csm, filler1, filler2, msgsize);
		b = 0;
		for(i=0; i < j; i++) {
			if(b%16 == 0) {
			fprintf(logfile,"\n    ");
			}
			fprintf(logfile," %02x", obuf[i]);
			b++;
		}
		fprintf(logfile,"\n\n");
	}

	//i = write(hb_fd, obuf, j);
	
	i = sendto(sfd, obuf, j, 0, (struct sockaddr *) &peer_addr, peer_addr_len);
	if (i != j) {
		fprintf(stderr, "Error sending response\n");
		return -1;
	}
	fsync(hb_fd);
	return 0;
}

/*
               if (sendto(sfd, buf, nread, 0,
                           (struct sockaddr *) &peer_addr,
                           peer_addr_len) != nread)
                   fprintf(stderr, "Error sending response\n");
*/
/*******************************************************************************/
// make the devnum secton 
int gen_devnum( unsigned char * buf, int ix) {
	int j = ix;

	int secstart = j++;		// uber-header start, save space
	buf[j++] = SECT_DEVNUM;	

	buf[j++] = (pcdevnum >> 8) & 0xff;
	buf[j++] = pcdevnum & 0xff;


	buf[j++] = (xseqnum >> 8) & 0xff;
	buf[j++] = xseqnum & 0xff;

	buf[secstart] = j - secstart;	// put uber-header data size

	return j;
}

/*******************************************************************************/
// make the hb control secton 
int gen_hbctl( unsigned char * buf, int ix) {
	int j = ix;

	int secstart = j++;		// uber-header start, reserve space

	buf[j++] = SECT_HBCTL;	

	buf[j++] = (nhbc.hbdevnum >> 8) & 0xff;
	buf[j++] = nhbc.hbdevnum & 0xff;

	buf[j++] = nhbc.ctl & 0xff;

	buf[j++] = (nhbc.grade >> 8) & 0xff;
	buf[j++] = nhbc.grade & 0xff;

	buf[j++] = nhbc.wind & 0xff;

	buf[j++] = (nhbc.weight >> 8) & 0xff;
	buf[j++] = nhbc.weight & 0xff;

	buf[j++] = (nhbc.manwatts >> 8) & 0xff;
	buf[j++] = nhbc.manwatts & 0xff;

	buf[j++] = nhbc.minhr & 0xff;

	buf[j++] = nhbc.maxhr & 0xff;

	buf[j++] = (nhbc.adrag >> 8) & 0xff;
	buf[j++] = nhbc.adrag & 0xff;

	buf[j++] = (nhbc.tdrag >> 8) & 0xff;
	buf[j++] = nhbc.tdrag & 0xff;

	buf[secstart] = j - secstart;	// put uber-header data size

	return j;
}

// generate a variable hbctl message
int gen_varmsg( unsigned char * buf, int ix, int size) {
	int j = ix;
	int i;

	int secstart = j++;		// uber-header start, reserve space

	buf[j++] = SECT_VAR;	

	for(i=0;i<size;i++) {
		buf[j++] = i+1;
	}
	buf[secstart] = j - secstart;	// put uber-header data size

	return j;
}

