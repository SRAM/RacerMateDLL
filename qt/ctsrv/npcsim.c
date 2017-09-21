
#include	"npcsim.h"
#include "nreadudp.h"

int	logtest[16];
unsigned char logtest_inh, logtest_inl;

unsigned char	dram[128], idram[128];
unsigned char	xdpage0l[128], xdpage0h[128];
unsigned char	xdpage[128];

unsigned char grab_buff[8192];

int	newdump;
int	xd_select;

//int	do_9600;
int	hbport;

//int	sssync;
//int	cad_rate = 5;
//int	cad_mptime, cad_mpcount, cad_pattern;

//int	snooph, snoopl;

int	sfd = NULL;

//int	grab_timeout = 0;

int	hb_block = 0;

int	hbretval = 0;

unsigned short	xseqnum = 0;

float cad = 0.0f;

int paritychk(int x);
//void output_logtest(void);
//void save_ramdump(unsigned char *pkt);
//void output_ramdump(FILE *fp);
//static void send_cadsw(void);
//void output_grab(FILE *fp);

void output_ndata(FILE *fp);
//void output_nhbc(FILE *fp);
void output_rxq(void);

void output_stats(FILE *fp);
void send_tohb(struct pcdata *pcd);
void xmit_RacerMate(int hb_fd);
/*******************************************************************************/
/*******************************************************************************/

int
main(int argc, char *argv[])
{
	struct termios t, tcon, tport;
	struct timeval tp, tp_begin;
	int msnow;
	int i=0, j=0, k=0, n=0;
	int status=0;
	int errcount=0;
	char *p = NULL;
	char *ps = NULL;
	char *pd = NULL;
	char ch;
	unsigned char inch;
	char inbuf[ MAXIN ]= "";
	char outbuf[ MAXIN + 64 ] = "";
	int  pass=0;
	int	outfd = -1;
	char sbuf[1024];
	int mesglen = 0;
	int tmrfd = -1;
	struct itimerspec new_value;
	uint64_t exp, tot_exp;
	ssize_t s;
	char outfname[256] = {"pcsimout.dat\0"};
	fd_set readset, rset;
	int nready = 0;
	int	fast_print = 0;

	int	last_msnow = 0;
	/****/
	logfile = stderr;
	debug = initialdebug;

//	hbport = hbar;
//	hbport = hbar115k;
	hbport = ttyU5;


	//nca+++ not using the output file currently
	if (argc>1)
		strncpy(outfname, argv[argc-1], sizeof(outfname));

	outfile = fopen(outfname,"w");

	if (outfile==NULL) handle_error(outfname);

	if (verbose&1) fprintf(logfile, " %s: sending raw data to file: %s\n",
		                       argv[0],outfname);

	fclose(outfile);
	outfile = fopen(outfname, "a");
	outfd = fileno(outfile);

//	for (i=0; i< NCHANS; i++) {
//		status=open_chan(&cc[i]);
//
//		if (status< 0) errcount++ ;
//	}

//	udp stuff:
//	if ( open_chan(&cc[hbport]) < 0) errcount++;
	sfd = open_udp();

	if(sfd <=0 ) {
		fprintf(stderr, "open_udp returned %d\n", sfd);
	}
//#ifdef	SEND_CADSW
//	if ( open_chan(&cc[cadsw]) < 0) errcount++;
//#endif

	new_value.it_value.tv_sec = 0;
//	new_value.it_value.tv_nsec = 100000000;
	new_value.it_value.tv_nsec = 10000000;
	new_value.it_interval.tv_sec = 0;
//	new_value.it_interval.tv_nsec = 100000000;	// 100 ms
	new_value.it_interval.tv_nsec = 10000000;	// 10 ms
	tmrfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	fprintf(logfile, "timerfd_create: tmrfd= %d\n",tmrfd);

	if (tmrfd == -1)
		handle_error("timerfd_create");

	bp = 1;

	if (timerfd_settime(tmrfd, 0, &new_value, NULL) == -1)
		handle_error("timerfd_settime");

	fcntl(0, F_SETFL, (fcntl(0, F_GETFL,0)|O_NONBLOCK));
	tcgetattr(0,&tcon);
	t = tcon;
	cfmakeraw(&t);
	t.c_oflag |= OPOST;
	t.c_oflag |= ONLCR;
	tcsetattr(0,TCSANOW,&t);
	FD_ZERO(&readset);
	FD_SET(STDIN_FILENO, &readset);
	//FD_SET(cc[hbport].fd, &readset);
	FD_SET(sfd, &readset);

	FD_SET(tmrfd, &readset);
	gettimeofday(&tp_begin,NULL);
	print_timer_res();


	for (;;) {
		rset = readset;
		nready = select(32, &rset, NULL, NULL, NULL);

		msnow = time_ms(&tp, &tp_begin);
		sprintf(s_time_ms,"%11.3f", (float)msnow / 1000.0f);
		sprintf(s_time_s,"%6.0f", (float)msnow / 1000.0f);		//tlm+++


		//returned -something, probably interrupt or signal, so quit program
		if (nready <= 0)
			break;

		// stdin has char, presumably the keyboard
		if (FD_ISSET(STDIN_FILENO, &rset)) {
			n = read(STDIN_FILENO, &ch, 1);
			ch &=0x7f;

			if (ch == 27)
				break;


			if (handle_pckbd(ch) < 0) {  			//returns 0 if char matched some case, -1 if not
				fprintf(logfile,"handle_pckbd: no match, ch = %02x, \"%c\"\n",
				        ch, isprint(ch) ? ch : '?');
				fprintf(logfile,"debug = %4x\n", debug);
			}
		}

		// handle serial input from handlebar
		//if (FD_ISSET(cc[hbport].fd, &rset)) {
		//	read_input(cc[hbport].fd);
		//}

		if (FD_ISSET(sfd, &rset)) {
			read_udp(sfd);
		}

		if (FD_ISSET(tmrfd, &rset)) {
			s = read(tmrfd, &exp, sizeof(uint64_t));

			if (s != sizeof(uint64_t))
				handle_error("read");


			hbretval = handle_pkts();			// receive hb
			//check for error return
			//if(hbretval < 0) {
			//	pkt_timeout = 0;						// 
			//}

			if(hbretval) {
				pkt_timeout = 500;					// 5 second timeout to clear hb data
			}


			bound_pcdata();
			pcd.control = hb_control_byte;

			if (hb_control_byte > 0) {
				hb_block++;
				if(hb_block >= 33) {
					//xmit_RacerMate(cc[hbport].fd);
					send_tohb(&pcd);
					
					hb_block = 0;
				}
			}

			if (hb_control_byte < 0) {
				hb_block = 0;
				send_tohb(&pcd);
				hb_control_byte = 0;
			//	xmit_RacerMate(cc[hbport].fd);
			}



			pkt_timeout--;

			if (pkt_timeout <=0) {
				pkt_timeout = 0;
				clear_hbd();
			}

			//if (debug&2) {
			//	fprintf(logfile, "%s  ", s_time_s);
			//	output_both(logfile);
			//}
			

			if(hbretval) {

				if (debug&4) {

					for(i = 0; i<24; i++) {
						ssthrust[i] =  nhbd.f_peak * (float) nhbd.bars[i];
					}

					fprintf(logfile,"\n");
					for(i=0;i<24;i++) {
						fprintf(logfile,"%2d%*c\n",i+1, (int)(ssthrust[i]*2.0f)+1,'*');
					}
					//fprintf(logfile,"\n");
					
					if(nhbd.turns) {
						cad = nhbd.deltat ? (float)nhbd.turns * 6000.0f / (float)nhbd.deltat : -1.0f;
					}
					fprintf(logfile, "%s  ", s_time_ms);
					fprintf(logfile,"  tcount= %5hu  time= %5hu, peak= %4.2f  rpm=%4hhu",
						nhbd.prevturns, nhbd.prevtime, nhbd.f_peak*200, nhbd.rpm);
					fprintf(logfile, "  dt=%5d, cad=%5.1f\n", nhbd.deltat, cad);
					

				}
				if(debug&8) {
					fprintf(logfile, "%s  ", s_time_ms);
					output_ndata(logfile);
				}
				if(debug&16) {
					fprintf(logfile, "%s  ", s_time_ms);
					output_control(logfile);
				}
				if(debug&32) {
					fprintf(logfile, "%s  ", s_time_ms);
					output_stats(logfile);
				}
			}
			
		}	// if(timerfd...

		fflush(outfile), fflush(logfile);
	}	// end of for{;;}

	// release kbd raw mode
	tcsetattr(0,TCSANOW,&tcon);

	// close serial port(s)
	for (i=0; i< NCHANS; i++) {
		status=close_chan(&cc[i]);

		if (status< 0) errcount++ ;
	}

	// close timer filedescriptor
	close(tmrfd);
	fflush(outfile),fflush(logfile);
	return errcount;
}

// end of main

/*

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
*/

/*******************************************************************************/

int static SETVAL(int min, int max, int num)
{
	return (num < min ? min : num > max ? max : num) & 0xffff;
}
/*********************************************************/
// scan through pcdata and send it and control byte to the handlebar
void
send_tohb(struct pcdata *pcd) {
	int	val = 0;	
	
	nhbc.hbdevnum = pcd->hbdevnum & 0xffff;
	nhbc.ctl = pcd->control & 0xff;
	nhbc.grade = SETVAL(-150, 150, (int)(pcd->f_grade * 10.0f));
	nhbc.wind = SETVAL(-50, 50, (int)pcd->f_wind);
	nhbc.weight = SETVAL(0, 500, (int)pcd->f_weight);
	nhbc.manwatts = SETVAL(0, 2000, (int)pcd->f_manwatts);
	nhbc.minhr = pcd->hrenable ?  SETVAL(0,198, pcd->minhr) : 0;
	nhbc.maxhr = pcd->hrenable ?  SETVAL(0,198, pcd->maxhr) : 0;
	nhbc.adrag = SETVAL(0, 4095, (int)(pcd->f_aerodrag * 256.0f));
	nhbc.tdrag = SETVAL(0, 4095, (int)(pcd->f_tdrag * 256.0f));

	send_ctlmsg(cc[hbport].fd, 3);

}


/*********************************************************/
/*******************************************************************************/
// bound pcd struct to reasonable ranges
void
bound_pcdata(void)
{
//	if (cad_rate > 80) cad_rate = 80;
//	if (cad_rate < 3) cad_rate = 3;
//	if (cad_mptime > 100) cad_mptime = 100;
//	if (cad_mptime < 0 ) cad_mptime = 0;
	if (pcd.f_grade > 15.0f) pcd.f_grade = 15.0f;
	if (pcd.f_grade < -15.0f) pcd.f_grade = -15.0f;
	if (pcd.f_wind > 40.0f) pcd.f_wind = 40.0f;
	if (pcd.f_wind < -40.0f) pcd.f_wind = -40.0f;
	if (pcd.f_weight > 400.0f) pcd.f_weight = 400.0f;
	if (pcd.f_weight < 0.0f) pcd.f_weight = 0.0f;
	if (pcd.f_aerodrag > 12.0f) pcd.f_aerodrag = 12.0f;
	if (pcd.f_aerodrag < 0.0f) pcd.f_aerodrag = 0.0f;
	if (pcd.f_tdrag > 3.0f) pcd.f_tdrag = 3.0f;
	if (pcd.f_tdrag < 0.0f) pcd.f_tdrag = 0.0f;
	if (pcd.f_manwatts > 1500.0f) pcd.f_manwatts = 1500.0f;
	if (pcd.f_manwatts < 50.0f) pcd.f_manwatts = 50.0f;
	if (pcd.maxhr > 199) pcd.maxhr = 199;
	if (pcd.maxhr < 42) pcd.maxhr = 42;
	if (pcd.minhr) {
		pcd.hrenable = 1;
		if (pcd.minhr < 40) pcd.minhr = 40;
		if (pcd.minhr >= pcd.maxhr) pcd.minhr = pcd.maxhr-1;
	}
	pcd.hrenable = pcd.minhr ? 1 : 0;
}

/*******************************************************************************/
// return 1 if 8 bit byte in x is odd parity, 0 if even
int
paritychk(int x)
{
	int c = x;
	c ^= c >> 1;
	c ^= c >> 2;
	c ^= c >> 4;
	return (c & 0x01);
}

/*******************************************************************************/
void output_rxq(void) {
	int	c;
	int	b = 0;

	//while(check_rxq()) {
	while((c = read_rxq()) >= 0) {
		b++;
		fprintf(logfile, " %02x", c);
		if(b%32 == 0) {
			fprintf(logfile, "\n            ");
		}
	}
	fprintf(logfile," len:%3d\n", b);
}
// 
/*******************************************************************************/
/*******************************************************************************/


// zero out received hb data (when no hb input)
void
clear_hbd(void)
{
	hbd.ss0 = 0;
	hbd.ss1 = 0;
	hbd.ss2 = 0;
	hbd.keys = 0;
	hbd.f_mph = 0.0;
	hbd.watts = 0;
	hbd.hrate = 0;
	hbd.hrflags = 0;
	hbd.stkfree = 0;
	hbd.mintime = 0;
	hbd.crpm = 0;
	hbd.rpm = 0;
	hbd.rpmflags = 0;
	hbd.minhr = 0;
	hbd.maxhr = 0;
	hbd.f_rdrag = 0.0;
	hbd.rd_measured = 0;
	hbd.slip = 0;
	hbd.stat = 0;
	hbd.version = 0;
}
/*******************************************************************************/
/*******************************************************************************/
//zz
/*
struct hbdata {
	int	ss0;
	int	ss1;
	int	ss2;
	int	keys;
	float	f_mph;
	int	watts;
	int	hrate;
	int	hrflags;
	int	rpm;
	int	rpmflags;
	int	minhr;
	int	maxhr;
	float	f_rdrag;
	int	rd_measured;
	int	stat;
	int	version;
} hbd;
  21748.000  13:rcv    c0  29.34  509   85 1   3   0   54   54 d   0 199    2.00U  2594 e01 4095
*/
/*
void
output_data(FILE *fp)
{
	fprintf(fp, "%02x:%s  %02x %6.2f %4d %4d %1x  %2d %3d %4d%1c %4d %1x %3d %3d  %6.2f%1c  %4d %03x %4d  %2d %3d\n",
	        pkt_timeout,
	        pkt_timeout > 0 ? "rcv  ":"norcv",
	        hbd.keys,
	        hbd.f_mph,
	        hbd.watts,
	        hbd.hrate,
	        hbd.hrflags,
			  hbd.stkfree,
			  hbd.mintime,
			  hbd.crpm,
			  sssync ? sssync=0,'+' : ' ',
	        hbd.rpm,
	        hbd.rpmflags,
	        hbd.minhr,
	        hbd.maxhr,
	        hbd.f_rdrag,
	        hbd.rd_measured ? 'c' : 'U',
			  hbd.slip,
	        hbd.stat,
	        hbd.version,
			  cad_rate,
			  cad_mptime
	       );
//		fflush(fp);
}
*/
void
output_data(FILE *fp)
{
	fprintf(fp, "%02x:%s  %02x %6.2f %4d %4d %4d %4d %4d %4d %4d %6.2f %1c  %03x %4d\n",
	        pkt_timeout,
	        pkt_timeout >= 100 ? "send " : "nosnd",
	        hbd.keys,
	        hbd.f_mph,
	        hbd.watts,
	        hbd.hrate,
	        hbd.hrflags,
	        hbd.rpm,
	        hbd.rpmflags,
	        hbd.minhr,
	        hbd.maxhr,
	        hbd.f_rdrag,
	        hbd.rd_measured ? 'c':'U',
	        hbd.stat,
	        hbd.version
	       );
//		fflush(fp);
}

/*******************************************************************************/
/*
struct nhbdata {

	unsigned short	devnum;
	unsigned short	pktnum;
	unsigned char	keys;
	unsigned char	keypress;
	float	f_mph;
	unsigned short	watts;
	unsigned char	hrflags;
	unsigned char	hrate;
	unsigned char	rpmflags;
	unsigned char	rpm;
	unsigned char	minhr;
	unsigned char	maxhr;
	float	f_rdrag;
	int	rd_measured;
	unsigned short	version;
	unsigned short	slip;
	unsigned int	stat;					// 32 bits of status3 .. 0

//spinscan stuff
	unsigned short	prevturns;		// crank turn count, storage for increase
	unsigned short	prevtime;		// time of last turn, 10ms steps
	int	turns;						// increase in turn count
	int	deltat;			// time difference, 10 ms steps
	float	f_peak;			// load at peak bar==200, lbs * 256
	unsigned char bars[24];			// bars, 0 - 200
};
*/

/*
void
output_ndata(FILE *fp)
{
	static unsigned int lstat = 0;
	fprintf(fp, " %2d  %5hd %5hd  %02hhx %02hhx %6.2f %4hu %4hhd:%02hhx %4hhu:%02hhx %4hhu %4hhu %6.2f%1c %4hu %4hu  %08x  %4hx  %4hx",
			hbretval,
			nhbd.pktnum,
			nhbd.devnum,
	        nhbd.keys,
	        nhbd.keypress,
	        nhbd.f_mph,
	        nhbd.watts,
	        nhbd.hrate,
	        nhbd.hrflags,
	        nhbd.rpm,
	        nhbd.rpmflags,
	        nhbd.minhr,
	        nhbd.maxhr,
	        nhbd.f_rdrag,
	        nhbd.rd_measured ? 'c':'U',
			  nhbd.slip,
	        nhbd.version,
	        nhbd.stat,
			xseqnum,
			  nhbd.slip
	       );
	if(nhbd.stat != lstat) {
		fprintf(fp,"  *");
		lstat = nhbd.stat;
	}
	fprintf(fp,"\n");
}
*/
void
output_ndata(FILE *fp)
{
	static unsigned int lstat = 0;
	fprintf(fp, " %05hd  %02hhx %02hhx %6.2f %4hu %4hhd:%02hhx %4hhu:%02hhx %4hhu %4hhu %6.2f%1c %4hu  %08x",
	//		hbretval,
	//		nhbd.pktnum,
			nhbd.devnum,
	        nhbd.keys,
	        nhbd.keypress,
	        nhbd.f_mph,
	        nhbd.watts,
	        nhbd.hrate,
	        nhbd.hrflags,
	        nhbd.rpm,
	        nhbd.rpmflags,
	        nhbd.minhr,
	        nhbd.maxhr,
	        nhbd.f_rdrag,
	        nhbd.rd_measured ? 'c':'U',
	//		  nhbd.slip,
	        nhbd.version,
	        nhbd.stat
	//		xseqnum,
	//		  nhbd.slip
	       );
	if(nhbd.stat != lstat) {
		fprintf(fp,"  *");
		lstat = nhbd.stat;
	}
	fprintf(fp,"\n");
}
/***********************************************************************************************/
void
output_stats(FILE *fp) {
	static unsigned int lstat = 0;
	static int	lx = 0;
	static int cjumpcnt = 0;
	static int djumpcnt = 0;
	unsigned short	x;
	int y,z;
	x = nhbd.pktnum - good_msg;
	y = x & 0x8000 ? x - 65536: x;

	if(x != lx) {
		djumpcnt++;
		lx = x;
	}

	x = nhbd.stat >> 16;
	z = x & 0x8000 ? x - 65536: x;

	//fprintf(fp, "%2d %5d %5d %5hd %6.2f %4hu %4hhu %6.2f%1c %4hu %4hu  %08x  %4hx  %4hx",
	fprintf(fp, "%2d %5d   %5hu %5u %+6d %4d      %5hu %5hu %+6d      %04hx  %4d",	//  %4hu %4hu  %4hx  %4hx",
			hbretval,
			bad_msg,
			nhbd.pktnum,
			good_msg,
			y,
			djumpcnt,
			xseqnum,
			nhbd.slip,
			z,
			nhbd.stat & 0xffff,
			cjumpcnt

//			xseqnum,
//			nhbd.slip

//			nhbd.stat,
	);
	if(nhbd.stat != lstat) {
		cjumpcnt++;
		fprintf(fp,"  *");
		lstat = nhbd.stat;
	}
	fprintf(fp,"\n");

}

/*******************************************************************************
grade wind lbs adrag tdrag mwatts  mode -- keys mph watts rpm Urdrag
2e:rxtx  +15.1  +30  123  8.0  0.95  1123  wrun   *--*--  23.4  1234  145nv   2.35U
"%02x:%s  %+5.1f  %+3.0f  %3d  %4.2f  %4.2f  %4d  %4s  %2s  %6s  %4.1f  %4d  %3d%s  %4.2f%c\n"
*******************************************************************************/

void
output_both(FILE *fp)
{
	char *mode = "mode";

	switch (pcd.control) {
		case HB_WIND_RUN:
			mode = "W_r";
			break;
		case HB_WIND_PAUSE:
			mode = "W_p";
			break;
		case HB_RFTEST:
			mode = "cal";
			break;
		case HB_ERGO_RUN:
			mode = "E_r";
			break;
		case HB_ERGO_PAUSE:
			mode = "E_p";
			break;
		default:
			mode = "unk";
			break;
	}

	fprintf(fp, "%02x:%s  %s  %+5.1f  %+3.0f  %3.0f  %4.2f  %4.2f  %4.0f    %c%c%c%c%c%c  %4.1f  %4d  %3d %3d%2s  %4.2f%c  %4d",
	        pkt_timeout,
	        pkt_timeout > 0 ? "rxok" : "norx",
			  mode,
	        pcd.f_grade,
	        pcd.f_wind,
	        pcd.f_weight,
	        pcd.f_aerodrag,
	        pcd.f_tdrag,
	        pcd.f_manwatts,
	        hbd.keys & 0x01 ? 'R':'.',
	        hbd.keys & 0x02 ? '1':'.',
	        hbd.keys & 0x10 ? '2':'.',
	        hbd.keys & 0x04 ? '3':'.',
	        hbd.keys & 0x08 ? '+':'.',
	        hbd.keys & 0x20 ? '-':'.',
	        hbd.f_mph,
	        hbd.watts,
			  hbd.crpm,
	        hbd.rpm,
	        hbd.rpmflags ? hbd.rpmflags & 0x04 ? "op" : "sw": "nv",
	        hbd.f_rdrag,
	        hbd.rd_measured ? 'c':'U',
			  hbd.slip
			  //snooph,
			  //snoopl,
			  //cad_rate,
			  //cad_mptime,
				//cad_pattern
	       );

	fprintf(logfile, "\n");
}

/*******************************************************************************/
void
output_control(FILE *fp)
{
	fprintf(fp, " %5hu %4hx  %02x %5.1f %5.0f %5.0f %5.0f %4d%1c %4d%1c  %5.3f  %5.3f\n",
			xseqnum,
			xseqnum,
	        pcd.control,
	        pcd.f_grade,
	        pcd.f_wind,
	        pcd.f_weight,
	        pcd.f_manwatts,
	        pcd.minhr,
			  pcd.hrenable ? 's' : ' ',
	        pcd.maxhr,
			  pcd.hrenable ? 's' : ' ',
	        pcd.f_aerodrag,
	        pcd.f_tdrag
	       );
//		fflush(fp);
}
/*******************************************************************************/
/*******************************************************************************/
/*
struct	pcdata {
	int	control;
	float	f_grade;
	float	f_wind;
	float	f_weight;
	int	minhr;
	int	maxhr;
	int	hrenable;
	float	f_aerodrag;
	float	f_tdrag;
	float	f_manwatts;
};
*/

/*********************************************************/
/*********************************************************/

/*
struct chancontrol {
	const char *ident;
	const char *device;
	const	int speed;
	const int ttymode;
	int fd;
	FILE *fp;
};
*/
/******************************************************************/
//zz
int
handle_pckbd(char ch)
{
	int found = 0;					//assume some match, good return code

	switch (ch) {
		case ' ':
			break;					// space just speed up printing
		case '0':
			debug = 0;
			fprintf(logfile, "\nDebug = %04x\n", debug);
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			debug ^= (1 << (ch-49));
			fprintf(logfile, "\nDebug = %04x\n", debug);
			break;
		case '*':
//			output_grab(outfile);
			break;
		case 'A':
			pcd.hbdevnum = 23456;
			break;
		case 'a':
			pcd.hbdevnum = 0;
			break;
		case 'S':
			pcd.hbdevnum = 12345;
			break;
		case 's':
		//	cad_mptime--;
			break;
		case 'Q':
			hb_control_byte = -1;
			break;
		case 'q':
			hb_control_byte = 0;
			break;
		case 'w':
			hb_control_byte = HB_WIND_RUN;
			break;
		case 'e':
			hb_control_byte = HB_WIND_PAUSE;
			break;
		case 'r':
			hb_control_byte = HB_RFTEST;
			break;
		case 't':
			hb_control_byte = HB_ERGO_RUN;
			break;
		case 'y':
			hb_control_byte = HB_ERGO_PAUSE;
			break;
		case 'G':
			pcd.f_aerodrag += 0.1f;
			break;
		case 'g':
			pcd.f_aerodrag -= 0.1f;
			break;
		case 'H':
			pcd.f_tdrag += 0.1f;
			break;
		case 'h':
			pcd.f_tdrag -= 0.1f;
			break;
		case 'Z':
			pcd.f_grade += 0.1f;
			break;
		case 'z':
			pcd.f_grade -= 0.1f;
			break;
		case 'X':
			pcd.f_wind += 1.0f;
			break;
		case 'x':
			pcd.f_wind -= 1.0f;
			break;
		case 'C':
			pcd.f_weight += 5.0;
			break;
		case 'c':
			pcd.f_weight -= 5.0;
			break;
		case 'V':
			pcd.f_manwatts += 10.0;
			break;
		case 'v':
			pcd.f_manwatts -= 10.0;
			break;
		case 'B':
			pcd.minhr += 1;
			if(pcd.minhr < 40) pcd.minhr = 40;
			if(pcd.maxhr < 42) pcd.maxhr = 42;
			if(pcd.minhr >= pcd.maxhr) pcd.minhr = pcd.maxhr - 1;
			break;
		case 'b':
			pcd.minhr -= 1;
			if(pcd.minhr < 40) pcd.minhr = 0;
			break;
		case 'N':
			pcd.maxhr += 1;
			if(pcd.maxhr > 199) pcd.maxhr = 199;
			break;
		case 'n':
			pcd.maxhr -= 1;
			if(pcd.maxhr < 42) pcd.maxhr = 42;
		   if(pcd.maxhr <= pcd.minhr) pcd.maxhr = pcd.minhr + 1;
			break;

			// for handlebar code debug
		case 'O':
			filler1 = filler1 < 40 ?  filler1+1: 40;
			break;

		case 'o':
			filler1 = filler1 > 0 ? filler1-1  : 0;
			break;

		case 'P':
			filler2 = filler2 < 40 ?  filler2+1: 40;
			break;

		case 'p':
			filler2 = filler2 > 0 ? filler2-1  : 0;
			break;

		case 'L':
			msgsize = msgsize < 20 ?  msgsize+1: 20;
			break;

		case 'l':
			msgsize = msgsize > 0 ? msgsize-1  : 0;
			break;

		case ';':
			break;
		default:
			found = -1;					// no match yet, assume bad return
			break;
	}
	if(found == 0) bound_pcdata();

	return found;
}

/************************************************************************/
// open serial device described in chancontrol struct ch.
// sets FILE ptr and file descriptor in ch, returns file descriptor.
// currently fp is null, we don't do fopen(), only using read() and write() to it.
int
open_chan(struct chancontrol *ch)
{
	struct termios t;
	int fd;
	FILE *fp = NULL;
	int i=-1;
	fd = open(ch->device,O_RDWR|O_NONBLOCK);

	if (fd<0) {
		fprintf(logfile,"open_chan open( %s )returned %d\n", ch->device, fd);
		return -1;
	}

	ch->fd=fd;
	ch->fp=fp;
	i = ch->ttymode;

	dbgout(0x01,"open_chan: %s, %s fd=%d, mode=%d\n",ch->ident, ch->device, fd, i);

	tcgetattr(fd,&t);

	switch (i) {
		case N8R:
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

	cfsetispeed(&t,ch->speed);
	cfsetospeed(&t,ch->speed);
	tcsetattr(fd,TCSANOW,&t);
	return fd;
}




/****************************************************************************************/
// print to fd logfile if debug & dbgval

void dbgout(int dbg, const char *fmt, ...) {
	
	va_list	ap;

	if(debug & dbg) {
		va_start(ap, fmt);
		vfprintf(logfile, fmt, ap);
	}
}

/****************************************************************************************/
/*******************************************************************************/
/*******************************************************************************/
int
close_chan(struct chancontrol *ch)
{
	int fd;
	FILE *fp;
	int stat= -1;
	fd= ch->fd;
	fp= ch->fp;

	dbgout(0x01,"\nclose_chan: %d %p\n",fd,fp);

	if (fp!=NULL) {
		stat=fclose(fp);

		dbgout(0x01,"close_chan fclose(%s)returned %d:\n",ch->device,stat);
	}

	if (fd >=0) {
		stat=close(fd);

		dbgout(0x01,"close_chan close(%s)returned %d:\n",ch->device,stat);
	}

	return stat;
}



/*******************************************************************************/
/*******************************************************************************/
// return elapsed time in milliseconds as integer
int
time_ms(struct timeval *t, struct timeval *tbegin)
{
	gettimeofday(t,NULL);
	return (int)((t->tv_sec-tbegin->tv_sec)*1000L + (t->tv_usec-tbegin->tv_usec)/1000L);
}


/*******************************************************************************/
/*******************************************************************************/

// stuff for testing timers
static void
print_timer_res(void)
{
	struct timespec curr;
	int nsecs;

	if (clock_getres(CLOCK_MONOTONIC, &curr) == -1)
		handle_error("clock_getres");

	nsecs = curr.tv_sec*1000000000 + curr.tv_nsec;
	fprintf(logfile,"CLOCK_MONOTONIC resolution is  %d nsecs\n", nsecs);
}

/*******************************************************************************/
/*******************************************************************************/
/*
static void
print_elapsed_time(void)
{
	static struct timespec start;
	struct timespec curr;
	static int first_call = 1;
	int secs, nsecs;

	if (first_call) {
		first_call = 0;

		if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
			handle_error("clock_gettime");
	}

	if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
		handle_error("clock_gettime");

	secs = curr.tv_sec - start.tv_sec;
	nsecs = curr.tv_nsec - start.tv_nsec;

	if (nsecs < 0) {
		secs--;
		nsecs += 1000000000;
	}

	fprintf(logfile,"%d.%03d: ", secs, (nsecs + 500000) / 1000000);
}
*/
