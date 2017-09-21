import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.*;
import java.net.*;
import java.util.Random;

import javax.swing.JFrame;
import javax.swing.JPanel;
//import javax.swing.JScrollPane;
//import javax.swing.JTextArea;
//import javax.swing.text.DefaultCaret;

//-------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------

public class server extends JFrame implements KeyListener  {
	int calls=0;
	long lastnow;
	long tot = 0;
	long totcnt = 0;
	private int bp = 0;
	Random rand = null;		
	private long RAM = 0;
	boolean runflag = false;
	String host = "127.0.0.1";
	int port = 9072;
	ServerSocket serverSocket;
    Socket clientSocket;
    //JTextArea text = null;
    //JScrollPane sa = null;		
    JPanel jp = null;
	DataInputStream in = null;
	DataOutputStream out = null;
	boolean connected_to_client = false;							// connected to client, but not yet to a computrainer
	boolean connected_to_computrainer = false;
	private class hboutput	{
		/*
		int	ss0;			// next 3 spinscan bytes or zeros
		int	ss1;
		int	ss2;
		int	keys;			// key state, spinscan sync, odd parity or Gary flag
		int	vhi;			// data selector, 0-15, using 1-12
		int	vlo;			// 12 bit data, signed or not, sends low 12 bits
		*/
		byte	ss0;			// next 3 spinscan bytes or zeros
		byte	ss1;
		byte	ss2;
		byte	keys;			// key state, spinscan sync, odd parity or Gary flag
		byte	vhi;			// data selector, 0-15, using 1-12
		byte	vlo;			// 12 bit data, signed or not, sends low 12 bits
	};


	private class hbdata {
		float	f_mph=23.0f;
		int	watts = 340;
		int	hrate = 90;
		int	hrflags = 0x01;
		int	rpm = 145;
		int	rpmflags = 0x08;
		int	minhr = 45;
		int	maxhr = 167;
		int	rdrag = 512;			// added integer rdrag * 256, what is sent on wire
//		float	f_rdrag;			// removed float rdrag
		int	rd_measured = 0;
		int	slip = 2345;
		int	stat = 0x123;
		int	version = 4543;
		int	keys = 0;
		int	ss_sync = 0;			// null pointer
		//byte sscan;
		byte sscan[];
		hbdata()  {
			sscan = new byte[24];
			//Arrays.fill(sscan, 0);
			int i;
			for(i=0; i<24; i++)  {
				sscan[i] = 0;
			}
		}
	};
	
	class	hbcontrol {
		int		control = 0;
		float	f_grade = 0.0f;
		float	f_wind = 0.0f;
		float	f_weight = 0.0f;
		int		minhr = 0;
		int		maxhr = 0;
		float	f_aerodrag = 0.0f;
		float	f_tdrag = 0.0f;
		float	f_manwatts = 0.0f;
	};

	hbcontrol hbc;
	hbdata hbd;
	hboutput hbout;
	int	ix = 0;						// id scan index
	int	ssix = 25;					// ss send index
	byte ssbuf[];					// packed spinscan for send_pc()
	byte ssbars[];					// manual spinscan bars, packed
	int	vlo, vhi;					// for send_pc() and setval()
	byte txbuf[];
	final byte[] packet_mask = { 0x60, 0x50, 0x48, 0x44, 0x42, 0x41 };
	byte buf[];
	boolean break_parity = false;	// breaks send byte 7 parity	
	float sweep_mph, sweep_watts, sweep_rpm;
	boolean	enable_sweep = false;	// enable sweep modulation
	float	sweep = 1.0f;			// modulates mph,rpm,watts along with ss
	final float[] ssthrust = {
		0.31f, 2.64f, 6.67f, 11.33f, 15.36f, 17.69f, 17.69f, 15.36f, 11.33f, 6.67f, 2.64f, 0.31f,
		0.34f, 2.93f, 7.41f, 12.59f, 17.07f, 19.66f, 19.66f, 17.07f, 12.59f, 7.41f, 2.93f, 0.34f
	};
	
	float ss_scaling;			// scales ssthrust[]/19.66
	float	ss_min = 4.0f;		// min scaling value
	float ss_max = 19.5f; 		// max scaling value
	int	ss_rate = 20;			// seconds from min to max or back
	float scaleval = 0.0f;
	int direction = 0;
	final int initialdebug = 2+1;
	int debug = 0;
	int tick = 0;
	int packetinIndex = 0;
	byte packetin[];
	byte workbuf[];
	int	packet_timeout = 0;								//for send_pc, getting good pc packets if > 100
	int linkup_index = 0;
	//final String message = "RacerMate";
	byte racermate[];
	byte packet[];
	//Random rand = null;		
	boolean manual_ss = true;							// enable send ss from ssbars
	int cadence = 110;									// pedal rpm, sent to pc and triggers spinscan
	int cadence_ok = 1;									// nz is cadence pickup connected
	//float scaleval = 0.0f;
	//int direction = 0;
	int	ss_trigger = 0;								// accumulates cadence to trigger ss send
	long diff = 0;
	long totaldiff = 0;
	long diffcnt = 0;
	int hb_mode = 0;							// wind, ergo, cal mode, copy of hbc.control for now.
	int	fast_print = 0;
	//private long RAM = 0;
	boolean notified = false;

	
	
	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	server()  {
		super("server");
		
		rand = new Random(System.currentTimeMillis());		
		bp = 0;
		ssbuf = new byte[24];			//packed spinscan for send_pc()
		ssbars = new byte[24];			//manual spinscan bars, packed
		txbuf = new byte[6];
		buf = new byte[7];
		packetin = new byte[16];
		workbuf = new byte[32];
		racermate = "RacerMateX".getBytes();
		//racermate = new byte[16];
		//racermate = "RacerMate".getBytes();
		racermate[9] = 0x00;
		
		packet = new byte[7];
		hbd = new hbdata();
		hbout = new hboutput();
		hbc = new hbcontrol();
		
		sweep = enable_sweep ? modulate_all( 0.2f, 1.0f, ss_rate): 1.0f;
		sweep_mph = sweep;
		sweep_watts = sweep;
		sweep_rpm = sweep/3.0f + 0.7f;

		debug = initialdebug;
		
		

		RAM = Runtime.getRuntime().totalMemory();
		
		host = "127.0.0.1";
		port = 9072;
		
		try {
			serverSocket = new ServerSocket(port);
		}
		catch (IOException e1) {
			e1.printStackTrace();
		}

		/*
		text = new JTextArea(6, 20);								// rows, cols
		DefaultCaret caret = (DefaultCaret)text.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
		
        sa = new JScrollPane(text);
        */
		
        jp = new JPanel();
        jp.setLayout(new BorderLayout());
        //jp.add(sa, BorderLayout.CENTER);
		jp.setBackground(new Color(255,255,0));

		/*
        //sa.setBackground(new Color(255, 25, 0));
		sa.setOpaque(true);
        sa.getViewport().setBackground(Color.CYAN);
        */
		
		setTitle("server");
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();					// 1920 x 1080
		setSize ( (int)(.5*d.getWidth()), (int)(.5*d.getHeight()) );
		setLocation((int)(.25*d.getWidth()), (int) (.25*getHeight()));
		// pack();

		setContentPane(jp);
        setTitle("Server");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
        
		//setFocusable( true );
		/*
		sa.setFocusable(true);
		sa.requestFocus();
		sa.addKeyListener(this);		
		*/
		
		jp.setFocusable(true);
		jp.requestFocus();
		jp.addKeyListener(this);
		
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				endApp();								// invoked when the window is closed ( x'd ), or when escape has been hit
			}
		});

		//addKeyListener(this);

		/*
		try {
			run();
		}
		catch (IOException e) {
			e.printStackTrace();
		}
		catch (InterruptedException e) {
			e.printStackTrace();
		}
		*/
	}											// constructor

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	public static void main(String[] args) throws IOException {
		//JFrame x = new server();
		server x = new server();
		try {
			x.run();
		}
		catch (InterruptedException e) {
			e.printStackTrace();
		}
		
	}											// main

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	private void endApp()  {
		String s;

		try {
			Thread.sleep(500);
		}
		catch (InterruptedException ignored)  {
		}

		//System.out.printf("\nending\n");
		
		if (totcnt > 0)  {
			System.out.printf("avg = %.3f ms, samples = %d\n", (float)tot / (float)totcnt, totcnt );				// 10.000 ms!!!!
		}
		
		//text.setText("\nending\n");
		System.out.printf("\nending\n");
	}

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	public void keyPressed(KeyEvent e) {
		bp = 1;
	}

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	public void keyReleased(KeyEvent e) {
		bp = 2;
		
	}

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	public void keyTyped(KeyEvent e) {
		char c = e.getKeyChar();

		switch((int)c)  {
			case (int)'x':
			case (int)'X':
				bp = 0;
				break;
				
			case KeyEvent.VK_ESCAPE:
				System.exit(0);										// this causes endapp() to be called
				break;

			case KeyEvent.VK_RIGHT:
				bp = 2;
				break;
				
			default:
				bp = 1;
				break;
		}							// switch(c)

		//sa.requestFocus();
		jp.requestFocus();

		return;
	}											// keyTyped()

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------
	
	boolean is_connected_to_client()  {
		
		if (clientSocket.isClosed())  {
			in = null;
			out = null;
			try {
				clientSocket.close();
			}
			catch (IOException e) {
				e.printStackTrace();
			}
			clientSocket = null;
			connected_to_client = false;
			connected_to_computrainer = false;
			return false;
		}
		
		/*
		if (!serverSocket.isconnected())  {
			break;
		}
		*/
		
		return true;
	}							// is_connected_to_client()
	
	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	void send_pc()  {
		byte keys = (byte) (hbd.keys & 0x3f);

		if (hbd.ss_sync != 0) {
			//hbd.ss_sync = 0;
			ssix = 0;
			keys |= 0x40;
		}

		if (ssix < 24) {
			hbout.ss0 = (byte) (ssbuf[ssix++] ^ 0xff);
			hbout.ss1 = (byte) (ssbuf[ssix++] ^ 0xff);
			hbout.ss2 = (byte) (ssbuf[ssix++] ^ 0xff);

		}
		else {
			hbout.ss0 = 0;
			hbout.ss1 = 0;
			hbout.ss2 = 0;
		}

			hbout.keys = (byte) ((paritychk(keys) != 0) ? keys : keys | 0x80);

		if (hbd.ss_sync != 0) {
			hbd.ss_sync = 0;
			setval(0, 4095, hbd.slip);
			vhi |= 0xa0;

		}
		else {

			ix = ix >= 13 ? 0 : ix+1;		// next id 0-12

			switch (ix) {
				case 0:
				case 6:
					setval(0, 4095, (int)(hbd.f_mph*sweep_mph*50.0f+0.5f));
					vhi |= 0x10;
					break;
					case 1:
					setval(0, 4095, (int)( hbd.watts * sweep_watts + 0.5f));
					vhi |= 0x20;
					break;
				case 2:
					setval(0, 255, hbd.hrate);
					vhi = 0x30 | hbd.hrflags & 0x0f;
					break;
				case 3:
					vlo = 0;
					vhi = 0x40;
					break;
				case 4:
					setval(0, 255, hbd.rpm-7);
					vhi = 0x50;
					break;
				case 5:
					setval(0, 255, (int)(hbd.rpm * sweep_rpm + 0.5f));
					vhi = 0x60 | hbd.rpmflags & 0x0f;
					break;
				case 7:
					setval(0, 255, hbd.minhr);
					vhi |= 0x70;
					break;
				case 8:
					setval(0, 255, hbd.maxhr);
					vhi |= 0x80;
					break;
				case 9:
					setval(0, 2047, hbd.rdrag);
					vhi |= ((hbd.rd_measured != 0) ? 0x98 : 0x90);
					break;
					// send this when spinscan triggers, new crank turn
				case 10:
					//	setval(0, 4095, hbd.slip);
					//	vhi |= 0xa0;
					//	break;

					ix++;			// instead, skip to 11 and fall into it.
				case 11:
					setval(0, 4095, hbd.stat);
					vhi |= 0xb0;
					break;
				case 12:
					setval(0, 4095, hbd.version);
					vhi |= 0xc0;
					break;
				default:
					vlo = 0;
					vhi = 0;
					break;
			}
		}

		hbout.vhi = (byte) (vhi & 0xff);
		hbout.vlo = (byte) (vlo & 0xff);
		
		send_pcdata();
	}											// send_pc()

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------
	
	void send_pcdata() {
		int i;
		byte work;
		
		txbuf[0] = (byte)hbout.ss0;
		txbuf[1] = (byte)hbout.ss1;
		txbuf[2] = (byte)hbout.ss2;
		txbuf[3] = (byte)hbout.keys;
		txbuf[4] = (byte)hbout.vhi;
		txbuf[5] = (byte)hbout.vlo;
		
		//work = 0x80;                                                            // header byte
		work = (byte) (break_parity ? 0xc0 : 0x80);
		//byte buf[7];

		for (i=0; i<6; i++)  {
			buf[i] = (byte)((txbuf[i]>>1)&0x7f);									// tlm

			if ( (txbuf[i]&0x01) != 0)  {
				work ^= packet_mask[i];
			}
		}

		buf[6] = (byte)work;
		
		try {
			out.write(buf, 0, 7);											// blocks?
			String s;

			s = bytesToHex(txbuf).trim();
			System.out.printf("%10d  %s", System.currentTimeMillis(), s);
			
			s = bytesToHex(buf).trim();
			System.out.printf("      %s\n", s);
			
			if ((debug&64) != 0)  {
				bp = 2;
			}
		}
		catch (IOException e) {
			e.printStackTrace();
			System.out.printf("\nwrite to hbar\n");
		}

		return;
	}							// send_pcdata()

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------
	
	int paritychk(int x) {
		int c = x;
		c ^= c >> 1;
		c ^= c >> 2;
		c ^= c >> 4;
		return (c & 0x01);
	}												// paritychk()

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------
	
	void setval(int min, int max, int x) {
		vhi = x;

		if (vhi < min) {
			vhi = min;
		}

		if (vhi > max) {
			vhi = max;
		}

		vlo = vhi & 0xff;
		vhi = vhi >> 8 & 0x0f;
	}

	//-------------------------------------------------------------------------------------------
	// ramp mph,rpm,watts scaling value from min-max, over rate seconds, called at 100hz
	//-------------------------------------------------------------------------------------------
	
	float modulate_all(float min, float max, int rate) {

		if (rate < 1) {
			rate = 1;
		}

		float step = (max - min) / (100 * rate);
		scaleval += (direction==1) ? step : -step;

		if (scaleval > max) {
			scaleval = max;
			direction = 0;
		}

		if (scaleval < min) {
			scaleval = min;
			direction = 1;
		}

		return scaleval;
	}							// modulate_all()

	//-------------------------------------------------------------------------------------------
	//	read socket input, look for packed data or RacerMate
	//	returns 1 if new control message is in packet[],
	//	2 if 'RacerMate' string matched,
	//	0 if no new chars or not finished packet or racermate match.
 	//-------------------------------------------------------------------------------------------

	int read_pc()  {
		byte work;		// packet[7];
		byte c;
		int	n = 0;
		int	i, j, id;
		int	err = 0;
		
		calls++;
		if (calls==2)  {
			bp = 4;
		}
		
		//n = workbuf.length;									32
		
		try {
		    n = in.read(workbuf, 0, workbuf.length);
		}
		catch (SocketTimeoutException se) {
			//bp = 1;
		}
		catch (IOException e) {
			//e.printStackTrace();
			connected_to_client = false;
			System.out.printf("client disconnected 1\n");
		}
		
		if (n < 0) {
			//err = errno;
			//fprintf(logfile, "read_pc returned %d, errno %d: %s\n",
			//        n, err, strerror(err));
			if (!notified)  {
				notified = true;
				connected_to_client = false;
				System.out.printf("client disconnected 2\n");
			}
			return n;
		}

		if (n==0)  {
			return 0;
		}

		bp = n;
		
		for (i=0; i<n; i++)  {
			c = (byte) (workbuf[i] & 0x7f);

			if (c == racermate[linkup_index]) {
				if ((debug&8) != 0)  {
					System.out.printf("read_pc: c = %c, match at ix=%d\n", c, linkup_index);
				}

				linkup_index++;

				if (racermate[linkup_index] == '\0') {
					packetinIndex = 0;
					linkup_index = 0;
					return 2;
				}

			}
			else {
				if ((linkup_index!=0)) {
					if ((debug&8) != 0)  {
						System.out.printf("read_pc: c = %c, nomatch to %c at msg[%d]\n", c, racermate[linkup_index],linkup_index);
					}
				}

				linkup_index = 0;
			}

			if (packetinIndex > 10) {
				packetinIndex = 10;
			}

			packetin[packetinIndex] = workbuf[i];
			packetinIndex++;

			if (packetinIndex >=8 && (debug&8) != 0) {
				System.out.printf("read_pc: no sync after 8, byte = %02x\n", workbuf[i]);
			}

			if ((workbuf[i] & 0x80) != 0)  {
				if ( (paritychk(workbuf[i])!=0) &&  (packetinIndex==7) )  {
					work = (byte) (packetin[6] & 0x7f);

					for (j=0; j<6; j++)  {
						packet[j] = (byte)((packetin[j]<<1) | ((work>>(5-j))&0x01));
					}

					if ((debug&8)!=0) {
						/*
						fprintf(logfile, "%s  in: %02x %02x %02x %02x %02x %02x %02x = %02x %02x %02x %02x %02x %02x\n",
						        s_time_ms, packetin[0],packetin[1],packetin[2],packetin[3], packetin[4],packetin[5],packetin[6],
						        packet[0],packet[1],packet[2],packet[3],packet[4],packet[5]);
						*/
					}
				}

				// got good packet
				packetinIndex = 0;
				return 1;
			}
		}                                       // for(i...

		//n = 0, no chars read, or not a new good packet yet.
		return 0;
	}													// read_pc()

	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	void decode_pc()  {
		int id, vhi, vlow, val, sval, x;
		
		id = (int)(packet[4] & 0x00f0);
		vhi = (int)(packet[4] & 0x0f);
		vlow = (int)(packet[5] & 0x00ff);
		val = (int)((vhi << 8) | vlow);
		sval = (int) ( ((val & 0x800) != 0) ? val | 0xfffff000 : val);
		
		hbc.control = (int)(packet[3] & 0x7f);

		switch (id)  {
			case 0x10:
				hbc.f_grade = (float)sval / 10.0f;
				break;
			case 0x20:
				hbc.f_wind = (float)sval;
				break;
			case 0x30:
				hbc.f_weight = (float)val;
				break;
			case 0x40:
				hbc.minhr = val;
				break;
			case 0x50:
				hbc.maxhr = val;
				break;
			case 0x60:
				hbc.f_aerodrag = (float)val/256.0f;
				break;
			case 0x70:
				hbc.f_tdrag = (float)val/256.0f;
				break;
			case 0x80:
				hbc.f_manwatts = (float)val;
				break;
			default:
				bp=2;
				break;
		}
	}						// decode_pc()
	
	//-------------------------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------------------------

	void send_LinkUp() {
		
		txbuf = "LinkUp".getBytes();
		try {
			out.write(txbuf);
		}
		catch (IOException e) {
			e.printStackTrace();
		}
	}									// send_LinkUp()

	//-------------------------------------------------------------------------------------------
	//	ramp ss scaling value from min-max, over rate seconds, called at 100hz
	//-------------------------------------------------------------------------------------------
	
	float modulate_spinscan(float min, float max, int rate) {

		if (rate < 1) {
			rate = 1;
		}

		float step = (max - min) / (100 * rate);
		scaleval += (direction==1) ? step : -step;

		if (scaleval > max) {
			scaleval = max;
			direction = 0;
		}

		if (scaleval < min) {
			scaleval = min;
			direction = 1;
		}

		return scaleval;
	}								// modulate_spinscan()

	//-------------------------------------------------------------------------------------------
	//	load (fake) new spinscan into ssbuf[], point hbd.ss to it, set hbd.ss_sync
	//-------------------------------------------------------------------------------------------
	
	void load_spinscan() {
		int	i, j;

		for (i=0; i<24; i++) {
			if (manual_ss) 
				//ssbuf[i] = ssbars[i] ? (ssbars[i]&0x1f) << 3 | (ssbars[i] >> 5): 0x08;
				ssbuf[i] = ssbars[i];
			else
				ssbuf[i] = pack_ss(ssthrust[i]*ss_scaling/19.66f);


			if ((debug&4) != 0) {
				System.out.printf("sst[%02d] = %8.4f, ssb = %02x, up = %8.4f\n",i, ssthrust[i], ssbuf[i], unpack_ss(ssbuf[i]));
			}
		}

		hbd.ss_sync = 1;
		hbd.sscan = ssbuf;
	}													// load_spinscan()
	
	//-------------------------------------------------------------------------------------------
	// pack 0-64 lbs to spinscan byte format
	//-------------------------------------------------------------------------------------------
	
	byte pack_ss(float inp) {
		float f = inp;
		int ex = 7;			// init exponent counter
		int i;

		//	printf("in:   f= %4.1f, ex = %d\n",f,ex);
		if (f >=63.5f) {
			return (byte)0xff;
		}

		if (f <= 0.0f) {
			return (byte)0x08;
		}

		while (f < 32.0f && (--ex != 0) ) {
			//	printf("   f= %4.1f, ex = %d\n",f,ex);
			f *= 2.0f;
		}

		i = (((int)f & 0x1f) << 3 | ex);
		//	printf("f= %4.1f, i = %02x, ex = %d\n",f,i,ex);
		return (byte) ((i!=0) ? i : 0x08);
	}

	//-------------------------------------------------------------------------------------------
	//		unpack spinscan byte format to 0-64lbs float
	//-------------------------------------------------------------------------------------------
		
	
	float unpack_ss(byte p) {
		//unsigned short n;
		short n;
		float f;
		n = (short)(p & 0x07);           // n = exponent

		if (n!=0)  {
			f = (float)(((p&0xf8) | 0x100) << (n-1));
			return f/512.0f;

		}
		else  {
			f = (float)(p & 0xf8);
			return f/512.0f;
		}
	}

	/*******************************************************************************
		zero pc control vars, when no packets rcvd.
	*******************************************************************************/
	
	void clear_hbc() {
		hbc.control = 0;
		hbc.f_grade = 0.0f;
		hbc.f_wind = 0.0f;
		hbc.f_weight = 0.0f;
		hbc.minhr = 0;
		hbc.maxhr = 0;
		hbc.f_aerodrag = 0.0f;
		hbc.f_tdrag = 0.0f;
		hbc.f_manwatts = 0.0f;
		return;
	}											// clear_hbc()

//-------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------
	
void compute_average_time()  {
	long now;
	
	now = System.currentTimeMillis();
	//now = System.nanoTime();
	diff = now - lastnow;
	tot += diff;													// 10.000 ms
	totcnt++;
	lastnow = now;
	return;
}

final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();

//-------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------

private String bytesToHex(byte[] bytes) {
    char[] hexChars = new char[bytes.length * 3];
    for ( int j = 0; j < bytes.length; j++ ) {
        int v = bytes[j] & 0xFF;
        hexChars[j*3] = hexArray[v >>> 4];
        hexChars[j*3+1] = hexArray[v & 0x0F];
        hexChars[j*3+2] = ' ';
    }
    return new String(hexChars);
}

//-------------------------------------------------------------------------------------------
//	entry:
//		serverSocket is already created
//-------------------------------------------------------------------------------------------

int run() throws IOException, InterruptedException  {
	runflag = true;
	int status;
	
	try {
		serverSocket.setSoTimeout(1000);
	}
	catch (SocketException e1) {
		e1.printStackTrace();
	}
	
	//text.setText("waiting for a connection\n");
	//System.out.printf("waiting for a connection\n");
	long cnt=0;
	
	while(runflag)  {

		if (!connected_to_client)  {
			cnt++;
			if (cnt==1)  {
				//text.setText("waiting for a connection\n");
				System.out.printf("\nwaiting for a connection\n");
			}
			
			//-----------------------------
			// listen for connections:
			//-----------------------------
		
			try  {
				clientSocket = serverSocket.accept();								// server socket times out every 1000 ms
				clientSocket.setSoTimeout(10);										// this is our main loop 10 ms timer
				in = new DataInputStream(clientSocket.getInputStream());
				out = new DataOutputStream(clientSocket.getOutputStream());
				connected_to_client = true;
				connected_to_computrainer = false;
				//text.setText("\nclient socket connected\n");
				cnt = 0;
				System.out.printf("client socket connected\n");
			}
			catch (SocketTimeoutException stoe)  {
				// serverSocket timeout
				continue;
			}
			catch(Exception e)  {
				bp = 2;
				continue;
			}
		}
		
		

		if (!is_connected_to_client())  {
			continue;
		}
		
		lastnow = System.currentTimeMillis();

		/////////////////////////// start of dummy loop ///////////////////////////////////
		
		/*
		n = in.available();							// 9
			
		if (n==0)  {
			// nothing available
			bp = 3;
		}
		else if (n<0)  {
			bp = 2;
		}
		else  {
		    n = in.read(bytes, 0, bytes.length);
			//for(int i = 0; i < data.length; i++)  {
			//	data[i] = in.readByte();
			//}
		    String s = new String(bytes, "UTF-8");	
		    s = s.trim();
		    text.setText("\n" + s + "\n");
			    
		    if (s.equals("RacerMate"))  {
		    	out.write("LinkUp".getBytes());
		    	connected_to_computrainer = true;
		    }
		    else  {
		    	bp = 2;
		    }
		}

		try {
			Thread.sleep(100);
		}
		catch (InterruptedException e) {
			e.printStackTrace();
		}				// sleep 100 ms
		continue;
		/////////////////////////// end of dummy loop ///////////////////////////////////	
		*/
			
			
			
			
		//////////////////////// non-dummy
 			
		//---------------------------------------------
		// poll and handle received pc chars
		//---------------------------------------------
		
		status = read_pc();							// fills in packet[], times out every 10 ms
		
		switch (status) {
			case 0:									// no data, 10 ms timeout
				break;
				
			case -1:
				// gets here if client closes connection
				clientSocket.close();
				clientSocket = null;
				connected_to_client = false;
				connected_to_computrainer = false;
				break;
			
			case 1:
				//compute_average_time();
				decode_pc();						// decodes packet into hbc
				packet_timeout += 28;

				if (packet_timeout > 255) {
					packet_timeout = 255;
				}
				break;
			case 2:									// received "RacerMate"
				packet_timeout = 0;
				send_LinkUp();
				break;
			default:
				bp = 4;
				break;
		}

		tick++;						// 0-99 10ms ticks == 1/sec.
		if (tick >= 100) {
			tick = 0;
		}

		//compute_average_time();
		
		sweep = enable_sweep ? modulate_all( 0.2f, 1.0f, ss_rate): 1.0f;
		sweep_mph = sweep;
		sweep_watts = sweep;
		sweep_rpm = sweep/3.0f + 0.7f;

		manual_ss = enable_sweep;
		
		ss_scaling = (float) ((cadence >= 30) ? modulate_spinscan(ss_min, ss_max, ss_rate): 0.0f);

		ss_trigger += cadence > 30 ? cadence : 30;	// min rate 30 rpm

		if (ss_trigger >= 6000) {
			ss_trigger -= 6000;

			if ((hbc.control & 0x20) != 0) {
				load_spinscan();							// hbd, ss_scaling
				//output_ssdata(logfile);
			}
		}
		
		//-------------------------------------------------------------------------------------------
		// 	count down packet_timeout to zero at 100 hz. It is +=28 when new pc packet received
		// 	bound to 255. If it is over 100 allow sending of hb data to pc.
		// 	if it reaches zero, clear the pc data struct hbc, fake stand-alone mode.
		//-------------------------------------------------------------------------------------------
		
		packet_timeout--;

		if (packet_timeout < 0) {
			packet_timeout = 0;
		}

		if (packet_timeout == 0) {
			clear_hbc();
		}

		hb_mode = hbc.control;			// update local mode from possible new pc command
		hbd.rpm = cadence;				// update rpm output
		//hbd.rpmflags = cadence_ok ? 0x08 : 0;

		if (packet_timeout >= 100) {
			
			if ((tick+1)%3 == 0) {
				compute_average_time();
				send_pc();
			}
		}


		if (tick%20 == 0) {
			if ((fast_print != 0)) {
				fast_print--;
			}

			//if (fast_print || tick == 0) {						// 5/sec if keyboard matched, else 1/sec
			//if (debug&2) {
			//fprintf(logfile, "~%s  ", s_time_s);		//tlm
			//output_both(logfile);
			//domenu();
			//}

			if ((debug&16)!=0) {
				//fprintf(logfile, "-%s  ", s_time_ms);		//tlm
				//output_data(logfile);
			}

			if ((debug&32)!=0) {
				//fprintf(logfile, "+%s  ", s_time_ms);		//tlm
				//output_control(logfile);
			}

			//}

			//console->rfrsh();
			//console->set_bg(COLOR_BLACK);
			//console->set_fg(COLOR_WHITE);
		}
		
		//------------------------------------
		// print stuff out once a second
		//------------------------------------
		
		//////////////////////// non-dummy

	}											// while(runflag)

	clientSocket.close();
	serverSocket.close();
	
	return 0;
}											// run()
	
}												// class

