import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

//import Client.hbdata;

/*******************************************************************************

*******************************************************************************/

//public class mythread implements Runnable  {
//public class mythread  {
public class mythread extends Thread {

	private boolean runflag = false;
	private boolean running = false;
	//private boolean flag = false;
	private int trainer_id = -1;
	//private int trys = 0;
	private boolean ipdiscover = false;
	private String bcaddr = "192.168.1.255";					// hard coded for now
	private final int bcport = 9071;
	
	private class hbdata {
		float	f_mph=16.0f;
		int i_mph = 16;
		int	watts = 100;
		int	hrate = 70;
		int	hrflags = 0x01;
		int	rpm = 60;
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
	}							// class hbdata
	
	private hbdata hbd;
	
		private class hboutput	{
			int	ss0;			// next 3 spinscan bytes or zeros
			int	ss1;
			int	ss2;
			int	keys;			// key state, spinscan sync, odd parity or Gary flag
			int	vhi;			// data selector, 0-15, using 1-12
			int	vlo;			// 12 bit data, signed or not, sends low 12 bits
		};

		private class	hbcontrol {
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

	
		private long lastnow = 0;
		private Socket socket = null;
		private DatagramSocket broadcast_socket = null;
		private boolean socket_connected = false;
		private boolean trainer_connected = false;
		private String datastr;
		private String host = "127.0.0.1";
		private int port = -1;
		private boolean manual_ss = true;							// enable send ss from ssbars
		private int cadence = 110;									// pedal rpm, sent to pc and triggers spinscan
		private int cadence_ok = 1;									// nz is cadence pickup connected
		//float scaleval = 0.0f;
		//int direction = 0;
		private int	ss_trigger = 0;								// accumulates cadence to trigger ss send
		private long diff = 0;
		private long totaldiff = 0;
		private long diffcnt = 0;
		private int hb_mode = 0;							// wind, ergo, cal mode, copy of hbc.control for now.
		private int	fast_print = 0;

		private int bp = 0;
		private int debug = 0;
		private int tick = 0;
		private int pktinIndex = 0;
		private byte pktin[];
		private byte workbuf[];
		private int	pkt_timeout = 0;								//for send_pc, getting good pc packets if > 100
		private int linkup_index = 0;
		private byte racermate[];
		private byte packet[];
		
		private int id;									// id, 0, 1, 2, ...
		private String tid;								// thread id string
		private DataInputStream in = null;
		private DataOutputStream out = null;
		private hbcontrol hbc;
		private hboutput hbout;
		private int	ix = 0;						// id scan index
		private int	ssix = 25;					// ss send index
		private byte ssbuf[];					// packed spinscan for send_pc()
		private byte ssbars[];					// manual spinscan bars, packed
		private int	vlo, vhi;					// for send_pc() and setval()
		private byte txbuf[];
		private final byte[] pkt_mask = { 0x60, 0x50, 0x48, 0x44, 0x42, 0x41 };
		private byte buf[];
		private boolean break_parity = false;	// breaks send byte 7 parity	
		private float sweep_mph, sweep_watts, sweep_rpm;
		private boolean	enable_sweep = false;	// enable sweep modulation
		private float	sweep = 1.0f;			// modulates mph,rpm,watts along with ss
		private final float[] ssthrust = {
			0.31f, 2.64f, 6.67f, 11.33f, 15.36f, 17.69f, 17.69f, 15.36f, 11.33f, 6.67f, 2.64f, 0.31f,
			0.34f, 2.93f, 7.41f, 12.59f, 17.07f, 19.66f, 19.66f, 17.07f, 12.59f, 7.41f, 2.93f, 0.34f
		};
		
		private float ss_scaling;			// scales ssthrust[]/19.66
		private float	ss_min = 4.0f;		// min scaling value
		private float ss_max = 19.5f; 		// max scaling value
		private int	ss_rate = 20;			// seconds from min to max or back
		private float scaleval = 0.0f;
		private int direction = 0;
		private final int initialdebug = 2+1;

		/*******************************************************************************
		
		*******************************************************************************/
		
		void send_pcdata() {
			int i, work;
			
			txbuf[0] = (byte)hbout.ss0;
			txbuf[1] = (byte)hbout.ss1;
			txbuf[2] = (byte)hbout.ss2;
			txbuf[3] = (byte)hbout.keys;
			txbuf[4] = (byte)hbout.vhi;
			txbuf[5] = (byte)hbout.vlo;
			
			//work = 0x80;                                                            // header byte
			work = break_parity ? 0xc0 : 0x80;
			//byte buf[7];

			for (i=0; i<6; i++)  {
				buf[i] = (byte)(txbuf[i]>>1);

				if ( (txbuf[i]&0x01) != 0)  {
					work ^= pkt_mask[i];
				}
			}

			buf[6] = (byte)work;
			
			try {
				out.write(buf, 0, 7);
				if ((debug&64) != 0)  {
					/*
					fprintf(logfile, "%s  out: %02x %02x %02x %02x %02x %02x = %02x %02x %02x %02x %02x %02x %02x\n",
					        s_time_ms, txbuf[0],txbuf[1],txbuf[2],txbuf[3], txbuf[4],txbuf[5],
					        buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
					*/
				}
			}
			catch (IOException e) {
				e.printStackTrace();
				System.out.printf("\nwrite to hbar\n");
			}

		}							// send_pcdata()
		
		/***********************************************************************************

		************************************************************************************/

		void send_pc()  {
			int keys = hbd.keys & 0x3f;

			if (hbd.ss_sync != 0) {
				//hbd.ss_sync = 0;
				ssix = 0;
				keys |= 0x40;
			}

			if (ssix < 24) {
				hbout.ss0 = ssbuf[ssix++] ^ 0xff;
				hbout.ss1 = ssbuf[ssix++] ^ 0xff;
				hbout.ss2 = ssbuf[ssix++] ^ 0xff;

			} else {
				hbout.ss0 = 0;
				hbout.ss1 = 0;
				hbout.ss2 = 0;
			}

				hbout.keys = (paritychk(keys) != 0) ? keys : keys | 0x80;

			if (hbd.ss_sync != 0) {
				hbd.ss_sync = 0;
				setval(0, 4095, hbd.slip);
				vhi |= 0xa0;

			} else {


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
						//vlo = 0;
						//vhi = 0x50;
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

			hbout.vhi = vhi;
			hbout.vlo = vlo;
			
			send_pcdata();
		}											// send_pc()

		/*******************************************************************************
		read socket input, look for packed data or RacerMate
		returns 1 if new control message is in pkt[],
		2 if 'RacerMate' string matched,
		0 if no new chars or not finished pkt or racermate match.
 
		 *******************************************************************************/

		//int read_pc(int hbarfd, unsigned char *pkt) {
		int read_pc()  {
			byte work;		// pkt[7];
			byte c;
			int	n = 0;
			int	i, j, id;
			int	err = 0;
			long now;
			
			//n = read(hbarfd, &workbuf, 32);
			now = System.currentTimeMillis();
			
			try {
			    n = in.read(workbuf, 0, workbuf.length);
			}
			catch (SocketTimeoutException se) {
				long ms = System.currentTimeMillis() - now;
				bp = 1;
			}
			catch (IOException e) {
				e.printStackTrace();
			}
			
			if (n < 0) {
				//err = errno;
				//fprintf(logfile, "read_pc returned %d, errno %d: %s\n",
				//        n, err, strerror(err));
				//System.out.printf("ooops\n");
				return n;
			}
	
			if (n==0)  {
				bp = 1;
			}
			else if (n!=0) {
				long now2 = System.currentTimeMillis();
				System.out.printf("%d   ", now2);
				
				for (i=0; i<n; i++)  {
					if (i<n-1)  {
						System.out.printf("%02x ", workbuf[i]);
					}
					else  {
						System.out.printf("%02x\n", workbuf[i]);
					}

					
					c = (byte) (workbuf[i] & 0x7f);
					
					if (c == racermate[linkup_index]) {
						if ((debug&8) != 0)  {
							System.out.printf("read_pc: c = %c, match at ix=%d\n", c, linkup_index);
						}
	
						linkup_index++;
	
						if (racermate[linkup_index] == '\0') {
							pktinIndex = 0;
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
	
					if (pktinIndex > 10) {
						pktinIndex = 10;
					}
	
					pktin[pktinIndex] = workbuf[i];
					pktinIndex++;
	
					if (pktinIndex >=8 && (debug&8) != 0) {
						System.out.printf("read_pc: no sync after 8, byte = %02x\n", workbuf[i]);
					}
	
					if ((workbuf[i] & 0x80) != 0)  {
						if ( (paritychk(workbuf[i])!=0) &&  (pktinIndex==7) )  {
							work = (byte) (pktin[6] & 0x7f);
	
							for (j=0; j<6; j++)  {
								packet[j] = (byte)((pktin[j]<<1) | ((work>>(5-j))&0x01));
							}
	
							if ((debug&8)!=0) {
								/*
								fprintf(logfile, "%s  in: %02x %02x %02x %02x %02x %02x %02x = %02x %02x %02x %02x %02x %02x\n",
								        s_time_ms, pktin[0],pktin[1],pktin[2],pktin[3], pktin[4],pktin[5],pktin[6],
								        pkt[0],pkt[1],pkt[2],pkt[3],pkt[4],pkt[5]);
								*/
							}
						}
	
						// got good pkt
						pktinIndex = 0;
						return 1;
					}
				}                                       // for(i...
				bp = 9;
			}				//if(n != 0)
	
			//n = 0, no chars read, or not a new good pkt yet.
			return 0;
		}													// read_pc()
	
		/*******************************************************************************
	 
		*******************************************************************************/
	
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
		
		/*******************************************************************************
		 
		*******************************************************************************/
	
		void send_LinkUp() {
			
			txbuf = "LinkUp".getBytes();
			try {
				out.write(txbuf);
			}
			catch (IOException e) {
				e.printStackTrace();
			}
		}									// send_LinkUp()
	
		/*******************************************************************************
			ramp ss scaling value from min-max, over rate seconds, called at 100hz
		*******************************************************************************/
		
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
	
		/*******************************************************************************
			load (fake) new spinscan into ssbuf[], point hbd.ss to it, set hbd.ss_sync
		*******************************************************************************/
		
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
		
		/*******************************************************************************
		
		*******************************************************************************/
		// pack 0-64 lbs to spinscan byte format
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
	
		/*******************************************************************************
			unpack spinscan byte format to 0-64lbs float
		*******************************************************************************/
		
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
			zero pc control vars, when no pkts rcvd.
		*******************************************************************************/
		
		void clear_hbc() {
			if (hbc==null)  {
				return;
			}
			
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


		/*******************************************************************************
		
		*******************************************************************************/
		
		int paritychk(int x) {
			int c = x;
			c ^= c >> 1;
			c ^= c >> 2;
			c ^= c >> 4;
			return (c & 0x01);
		}												// paritychk()

		/*******************************************************************************
		
		*******************************************************************************/
		
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

		/*******************************************************************************
		 ramp mph,rpm,watts scaling value from min-max, over rate seconds, called at 100hz
		********************************************************************************/
		
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
		
	
	/*******************************************************************************

	********************************************************************************/
	
	public void run() {
		String s;
		boolean first = true;
		int n=-1;
		int status = 0;
		// hardcode a host for non ipdiscovery for my internal testing only
		//host = "127.0.0.1";
		host = "192.168.1.150";
		byte[] bytes = null;

		
		if (ipdiscover)  {
			try {
				InetAddress x = null;
	
				try {
					x = InetAddress.getByName("0.0.0.0");
				}
				catch (UnknownHostException e4) {
					e4.printStackTrace();
				}
				
				try {
					broadcast_socket = new DatagramSocket(bcport, x);
					broadcast_socket.setBroadcast(true);
					broadcast_socket.setSoTimeout(1000);					
				}
				catch (SocketException e3) {
					e3.printStackTrace();
				}

				byte[] recvBuf = new byte[15000];
				DatagramPacket packet = new DatagramPacket(recvBuf, recvBuf.length);
				
				running = false;
				runflag = true;
				first = true;
				
				while(runflag)  {
					if (first)  {
						//System.out.println(getClass().getName() + ">>>Ready to receive broadcast packets!");
						System.out.printf("%s: Ready to receive broadcast packets on port %d\n", getClass().getName(), bcport);
						running = true;
						first = false;
					}
					
					try {
						broadcast_socket.receive(packet);
					}
					catch (IOException e1) {
						System.out.printf("   %s: timeout waiting for broadcast on port %d\n", getClass().getName(), bcport);
						// socket timeout
						Thread.sleep(200);
						continue;
					}

					s = new String(packet.getData()).trim();
					System.out.printf("received broadcast:  %s\n", s);
					
					System.out.printf("Discovery packet received from %s:  %s\n", packet.getAddress().getHostAddress(), s);
					
					if (s.startsWith("RacerMate:"))  {							// case sensitive
						String s2 = s.substring(10);							// "9072"
						port = -1;
						try  {
							port = Integer.parseInt(s2);
						}
						catch (NumberFormatException e)  {
							bp = 3;
						}
						if (port != -1)  {
							//byte[] sendData = "DISCOVER_FUIFSERVER_RESPONSE".getBytes();
							//DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, packet.getAddress(), packet.getPort());
							//broadcast_socket.send(sendPacket);
							//System.out.println(getClass().getName() + ">>>Sent packet to: " + sendPacket.getAddress().getHostAddress());
							host = packet.getAddress().getHostAddress();
							break;
						}
					}							// if (s.startsWith("RacerMate:"))  {
					Thread.sleep(200);
				}				// while(runflag)
			}					// try
			catch (InterruptedException e) {
				e.printStackTrace();
			}
			
			
			if (!runflag)  {						// aborted by creator
				//System.out.printf("trainer %d giving up hearing broadcast\n", trainer_id);
				running = false;
				return;
			}
			else  {
			}
		}							// if (ipdiscover)
		else  {
			port = 9072;
			running = false;
			runflag = true;
			first = true;
		}
		
		bp = 1;
		
		while(runflag)  {
			if (first)  {
				running = true;
				first = false;
			}
			//-------------------------------------------------
			// loop that tries to connect to server
			// breaks out of this loop when we've connected
			//-------------------------------------------------
				
			long now = 0;
			long now2 = 0;

			System.out.printf("\n");
			//System.out.printf("\ntrainer %d trying to connect to server on port %d\n", trainer_id, port);
			//System.out.flush();
				
			while(true)  {
				if (runflag==false)  {
					//System.out.printf("trainer %d giving up\n", trainer_id);
					break;
				}
				
				try {
					socket = new Socket(host, port);
					socket.setSoTimeout(1000);
					//socket.setSoTimeout(50);										// tries to read the pc for 50 ms before timing out
					in = new DataInputStream(socket.getInputStream( ));
					out = new DataOutputStream(socket.getOutputStream());
					break;
				}
			  	catch (UnknownHostException e) {
					System.err.println("Don't know about host " + host);
					System.exit(1);
				}
			  	catch (IOException e) {
			  		//System.out.printf("%s\n", e.getMessage());			// connection refused, server not running yet, socket is null
			  		bp = 3;
				}
				catch (Exception e)  {
					bp = 4;
				}
					
			
				now = System.currentTimeMillis();
					
				try {
					Thread.sleep(500);
				}
				catch (InterruptedException e) {
					e.printStackTrace();
				}				// sleep 100 ms

				diff = System.currentTimeMillis() - now;				// diff should equal 1000 ms (it did)
				totaldiff += diff;
					
				now = System.currentTimeMillis();
				//now2 = System.nanoTime();
				diffcnt++;
					
				if ( (now - lastnow) >= 1000)  {
					lastnow = now;

					/*
					dotcount++;
					if (dotcount==10)  {
						dotcount = 0;
						System.out.printf(".\n");
					}
					else  {
						System.out.printf(".");
					}
					*/
					if (runflag)  {
						System.out.printf("trainer %d trying to connect to server on port %d\n", trainer_id, port);
					}
				}
			}									// while(true)		trying to connect to server
			
			if (runflag==false)  {
				break;
			}
				
			System.out.printf("trainer %d connected to server at %s on port %d\n", trainer_id, host, port);
			System.out.flush();
			socket_connected = true;
			
			totaldiff = diff = 0;
				
			bytes = new byte[9];
			long now3 = System.currentTimeMillis();

			//---------------------------------------
			// main loop
			// connected to server
			// tries to read pc data
			// socket times out every 1000 ms
			//---------------------------------------
			
			int tocnt = 0;				// timeout  count
			
			while(true)  {
				/*
				try {
				    n = in.read(bytes, 0, 9);							// times out after 1 second
				    bp = 0;
				}
				catch (SocketTimeoutException se) {
					tocnt++;
					System.out.printf("   %d data socket timed out waiting for 9 bytes\n", tocnt);
					diff = System.currentTimeMillis() - now3;				// diff should equal 1000 ms (it did)
					totaldiff += diff;
					diffcnt++;
					if (runflag==false)  {
						break;
					}
					continue;
				}
			  	catch (IOException e)  {
			  		// server disconnected
			  		System.out.printf("%s, n = %d\n", e.getMessage(), n);					// connection reset
					System.out.flush();

			  		//continue;
			  		//socket = null;
			  		socket_connected = false;
			  		if (socket != null)  {
				  		try {
							socket.close();
						}
				  		catch (IOException e1) {
							e1.printStackTrace();
						}
				  		socket = null;
			  		}
			  		bp = 1;
			  		break;								// break to the outer loop
				}
				*/
				
				if (runflag==false)  {
					break;
				}
				
				//---------------------------------------------
				// poll and handle received pc chars
				//---------------------------------------------
					
				status = read_pc();							// fills in pkt[]
					
				switch (status) {
					case 1:
						decode_pc();						// decodes pkt into hbc
						pkt_timeout += 28;

						if (pkt_timeout > 255) {
							pkt_timeout = 255;
						}
						break;
					case 2:
						pkt_timeout = 0;
						send_LinkUp();
						break;
					case -1:
						// server disconnected
				  		System.out.printf("server disconnected\n");					// connection reset
						System.out.flush();

				  		//continue;
				  		//socket = null;
				  		socket_connected = false;
				  		if (socket != null)  {
					  		try {
								socket.close();
							}
					  		catch (IOException e1) {
								e1.printStackTrace();
							}
					  		socket = null;
				  		}
				  		bp = 1;
						break;
					default:
						break;
				}
					
				if (status<0)  {
					// server disconnected
					bp = 3;
					break;									// break to outer loop
				}
				
				
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
					
				//
				// 	count down pkt_timeout to zero at 100 hz. It is +=28 when new pc packet received
				// 	bound to 255. If it is over 100 allow sending of hb data to pc.
				// 	if it reaches zero, clear the pc data struct hbc, fake stand-alone mode.
				//
					
				pkt_timeout -= 1;

				if (pkt_timeout < 0) {
					pkt_timeout = 0;
				}

				if (pkt_timeout == 0) {
					clear_hbc();
				}

				hb_mode = hbc.control;			// update local mode from possible new pc command
				hbd.rpm = cadence;				// update rpm output
				//hbd.rpmflags = cadence_ok ? 0x08 : 0;

				if (pkt_timeout >= 100) {
					if ((tick+1)%3 == 0) {
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
				bp = 6;
			}										// while(true)
			
			// if we get here we were connected but either the server disconnected or we are closing the connection
			
			if (runflag==false)  {
				break;
			}
			bp = 7;
		}										// while(runflag)

		running = false;
		
		//System.out.printf("\nTHREADX   %s: id = %d\n", Thread.currentThread().getName(), id);		// Thread-3
	}				// run()
	
	// setters
	
	public void set_mph(int i)  { hbd.i_mph = i; hbd.f_mph = i;	}
	public void set_rpm(int i)  { hbd.rpm = i; }
	public void set_hrate(int i)  { hbd.hrate = i; }
	public void set_minhr(int i)  { hbd.minhr = i; }
	public void set_maxhr(int i)  { hbd.maxhr = i; }
	public void set_watts(int i)  { hbd.watts = i; }
	public void set_rpm_flags(int i)  { hbd.rpmflags = i; }
	public void set_hr_flags(int i)  { hbd.hrflags = i; }
	public void set_keys(int i)  { hbd.keys = i; }
	public void clear_runflag()  { runflag = false; }
	public boolean is_running()  { return running; }
	public boolean is_socket_connected()  { return socket_connected; }

	/*******************************************************************************

	********************************************************************************/
	
	mythread(int _trainer_id)  {			// constructor
		trainer_id = _trainer_id;
		ssbuf = new byte[24];			//packed spinscan for send_pc()
		ssbars = new byte[24];			//manual spinscan bars, packed
		txbuf = new byte[6];
		buf = new byte[7];
		pktin = new byte[16];
		workbuf = new byte[32];
		racermate = "RacerMate".getBytes();
		packet = new byte[7];
		hbd = new hbdata();
		hbc = new hbcontrol();
	
		sweep = enable_sweep ? modulate_all( 0.2f, 1.0f, ss_rate): 1.0f;
		sweep_mph = sweep;
		sweep_watts = sweep;
		sweep_rpm = sweep/3.0f + 0.7f;
		debug = initialdebug;
	}									// mythread()

	/*******************************************************************************

	********************************************************************************/
	
	int shutdown()  {
		int rc = 0;
		

		if (running)  {
			if (socket != null)  {
				System.out.printf("trainer %d is disconnecting at user's request\n", trainer_id);
				bp = 2;
			}
			else  {
				if (broadcast_socket != null)  {
					System.out.printf("trainer %d giving up hearing broadcast\n", trainer_id);
				}
				else  {
					System.out.printf("trainer %d giving up\n", trainer_id);
				}
			}
		}
		else  {
			bp = 6;
		}
		
		clear_runflag();
		
		long now = System.currentTimeMillis();
		boolean b = true;
		
		while(b)  {
			if (!is_running())  {
				break;
			}
			if ((System.currentTimeMillis() - now) >= 5000)  {
				rc = 1;
				break;
			}
			try {
				Thread.sleep(24);
			}
			catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		
		if (broadcast_socket != null)  {
			broadcast_socket.close();
			broadcast_socket = null;
		}
		
		
		if (socket == null)  {
			// no server running so socket is still null
		}
		else  {
			try {
				socket.close();
				//socket_connected = false;
			}
			catch (IOException e) {
				e.printStackTrace();
				rc = 2;
			}
			socket = null;
		}
	
	
		return rc;
	}									// shutdown()
	
}					// class mythread
