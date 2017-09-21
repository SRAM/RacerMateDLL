import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.*;
import java.net.*;
import java.util.Enumeration;
import java.util.Properties;
import java.util.Random;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFormattedTextField;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import utils.*;


/************************************************************************************
	simulates the wifi radio / handlebar combination. This is a tcp client that
	trys to connect to 'host' on 'port'. Once connected it listens for "RacerMate",
	sends "LinkUp" and then sends data to the server (pc).
************************************************************************************/

//public class client extends JFrame implements ActionListener, KeyListener, MouseListener, MouseMotionListener, rtdListener  {
public class Client extends JFrame implements KeyListener, ChangeListener, ActionListener  {

	
	private String bcaddr = null;
	private int bp = 0;
	private boolean flag = false;
	private String host = "127.0.0.1";
	private int port = 8899;
	private long lastnow = 0;
	private Socket socket = null;
	private boolean connected = false;
	private String data;
	private DataInputStream in = null;
	private DataOutputStream out = null;
	private Properties sysprops = null;

	private class hboutput	{
		int	ss0;			// next 3 spinscan bytes or zeros
		int	ss1;
		int	ss2;
		int	keys;			// key state, spinscan sync, odd parity or Gary flag
		int	vhi;			// data selector, 0-15, using 1-12
		int	vlo;			// 12 bit data, signed or not, sends low 12 bits
	};


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

	private hbcontrol hbc;
	private hbdata hbd;
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
	private int debug = 0;
	private int tick = 0;
	private int pktinIndex = 0;
	private byte pktin[];
	private byte workbuf[];
	private int	pkt_timeout = 0;								//for send_pc, getting good pc packets if > 100
	private int linkup_index = 0;
	private byte racermate[];
	private byte packet[];
	private Random rand = null;		
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
	private long RAM = 0;
    private JPanel jp = null;
	//private Color leftColor;
	//private BevelBorder bevelborder1;
	private Font font;
	private JSpinner rpm_spinner = null;
	private SpinnerModel rpm_spinnerModel = null;

	private JSpinner mph_spinner = null;
	private SpinnerModel mph_spinnerModel = null;
	
	private JSpinner watts_spinner = null;
	private SpinnerModel watts_spinnerModel = null;
	
	private JSpinner hr_spinner = null;
	private SpinnerModel hr_spinnerModel = null;

	private JSpinner minhr_spinner = null;
	private SpinnerModel minhr_spinnerModel = null;
	
	private JSpinner maxhr_spinner = null;
	private SpinnerModel maxhr_spinnerModel = null;
	
	private ChangeListener listener = null;				// single listener for all spinners
	
	private ButtonGroup radioGroup;
	private JRadioButton[] rpmbut;
	private String[] rpmcolors = {"SW", "OP", "NV"};
	private JCheckBox[] hrcb;
	private String[] hrcolors = {"out", "L", "H", "E", "b"};
	private JButton[] buttons;
	private String[] button_labels = {"Reset", "F2", "+", "F1", "F3", "-"};
	private String name = "not set";
	
	/************************************************************************************

	************************************************************************************/

	Client(String _name)  {
		
		super();
		RAM = Runtime.getRuntime().totalMemory();
	    System.out.println("memory at beginning of constructor " + RAM + " bytes.");

	    name = _name;
	    sysprops = System.getProperties();
	
		rand = new Random(System.currentTimeMillis());		
		bp = 0;
		ssbuf = new byte[24];			//packed spinscan for send_pc()
		ssbars = new byte[24];			//manual spinscan bars, packed
		txbuf = new byte[6];
		buf = new byte[7];
		pktin = new byte[16];
		workbuf = new byte[32];
		racermate = "RacerMate".getBytes();
		packet = new byte[7];
		hbd = new hbdata();

		sweep = enable_sweep ? modulate_all( 0.2f, 1.0f, ss_rate): 1.0f;
		sweep_mph = sweep;
		sweep_watts = sweep;
		sweep_rpm = sweep/3.0f + 0.7f;
		debug = initialdebug;
		
		jp = new JPanel(new RatioLayout());
		jp.setBackground(new Color(100, 100, 100));
		//leftColor = new java.awt.Color(128, 128, 255);			// 200, 200, 0
		//bevelborder1 = new BevelBorder(0);
		font = new Font("Dialog", Font.BOLD | Font.ITALIC, 20);
		jp.addKeyListener(this);
		
		JFormattedTextField tf;
		JLabel lab;
		Border border;
		String s;
		//SpinnerModel sm;
		float px1, px2, py, pw1, pw2, ph;
		float rpmb_px, rpmb_pw;
		
		px1 = .01f;
		px2 = px1 + .09f;
		pw1 = .08f;
		pw2 = .1f;
		ph = .05f;
		
		rpmb_px = px2 + pw2 + .02f;
		
		border = BorderFactory.createLineBorder(Color.BLACK, 1);


		
		
		py = .03f;
		rpm_spinnerModel = new SpinnerNumberModel(hbd.rpm, 0, 200, 1);		// initial value, min, max, step
		rpm_spinner = new JSpinner(rpm_spinnerModel);
		rpm_spinner.setFont(font);
		rpm_spinner.addChangeListener(this);
		rpm_spinner.setName("rpm");
		tf = ((JSpinner.DefaultEditor)rpm_spinner.getEditor()).getTextField();
		tf.setHorizontalAlignment(JFormattedTextField.LEFT);		
		lab = new JLabel("RPM");
		lab.setFont(font);
		lab.setForeground(Color.white);
		//lab.setBorder(border);
		lab.setHorizontalAlignment(JLabel.RIGHT);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px1, py, pw1, ph);
		jp.add(s, lab);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px2, py, pw2, ph);
		jp.add(s, rpm_spinner);
		
		py = .1f;
		mph_spinnerModel = new SpinnerNumberModel(hbd.i_mph, 0, 60, 1);		// initial value, min, max, step
		mph_spinner = new JSpinner(mph_spinnerModel);
		mph_spinner.setFont(font);
		mph_spinner.addChangeListener(this);
		mph_spinner.setName("mph");
		tf = ((JSpinner.DefaultEditor)mph_spinner.getEditor()).getTextField();
		tf.setHorizontalAlignment(JFormattedTextField.LEFT);		
		lab = new JLabel("MPH");
		lab.setFont(font);
		lab.setForeground(Color.white);
		//lab.setBorder(border);
		lab.setHorizontalAlignment(JLabel.RIGHT);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px1, py, pw1, ph);
		jp.add(s, lab);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px2, py, pw2, ph);
		jp.add(s, mph_spinner);

		py = .17f;
		watts_spinnerModel = new SpinnerNumberModel(hbd.watts, 0, 1000, 10);		// initial value, min, max, step
		watts_spinner = new JSpinner(watts_spinnerModel);
		watts_spinner.setFont(font);
		watts_spinner.addChangeListener(this);
		watts_spinner.setName("watts");
		tf = ((JSpinner.DefaultEditor)watts_spinner.getEditor()).getTextField();
		tf.setHorizontalAlignment(JFormattedTextField.LEFT);		
		lab = new JLabel("Watts");
		lab.setFont(font);
		lab.setForeground(Color.white);
		//lab.setBorder(border);
		lab.setHorizontalAlignment(JLabel.RIGHT);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px1, py, pw1, ph);
		jp.add(s, lab);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px2, py, pw2, ph);
		jp.add(s, watts_spinner);

		//-----------------------
		// hr spinner
		//-----------------------
		
		py = .24f;
		hr_spinnerModel = new SpinnerNumberModel(hbd.hrate, 40, 255, 1);		// initial value, min, max, step
		hr_spinner = new JSpinner(hr_spinnerModel);
		hr_spinner.setFont(font);
		hr_spinner.addChangeListener(this);
		hr_spinner.setName("hr");
		tf = ((JSpinner.DefaultEditor)hr_spinner.getEditor()).getTextField();
		tf.setHorizontalAlignment(JFormattedTextField.LEFT);		
		lab = new JLabel("HR");
		lab.setFont(font);
		lab.setForeground(Color.white);
		//lab.setBorder(border);
		lab.setHorizontalAlignment(JLabel.RIGHT);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px1, py, pw1, ph);
		jp.add(s, lab);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px2, py, pw2, ph);
		jp.add(s, hr_spinner);

		//-----------------------
		// minhr_spinner
		//-----------------------

		px1 = .2f;
		px2 = px1 + .09f;
		minhr_spinnerModel = new SpinnerNumberModel(hbd.minhr, 40, 255, 1);		// initial value, min, max, step
		minhr_spinner = new JSpinner(minhr_spinnerModel);
		minhr_spinner.setFont(font);
		minhr_spinner.addChangeListener(this);
		minhr_spinner.setName("maxhr");
		tf = ((JSpinner.DefaultEditor)minhr_spinner.getEditor()).getTextField();
		tf.setHorizontalAlignment(JFormattedTextField.LEFT);		
		lab = new JLabel("MINHR");
		lab.setFont(font);
		lab.setForeground(Color.white);
		//lab.setBorder(border);
		lab.setHorizontalAlignment(JLabel.RIGHT);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px1, py, pw1, ph);
		jp.add(s, lab);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px2, py, pw2, ph);
		jp.add(s, minhr_spinner);

		//-----------------------
		// maxhr_spinner
		//-----------------------

		px1 = .4f;
		px2 = px1 + .09f;
		maxhr_spinnerModel = new SpinnerNumberModel(hbd.maxhr, 40, 255, 1);		// initial value, min, max, step
		maxhr_spinner = new JSpinner(maxhr_spinnerModel);
		maxhr_spinner.setFont(font);
		maxhr_spinner.addChangeListener(this);
		maxhr_spinner.setName("maxhr");
		tf = ((JSpinner.DefaultEditor)maxhr_spinner.getEditor()).getTextField();
		tf.setHorizontalAlignment(JFormattedTextField.LEFT);		
		lab = new JLabel("MAXHR");
		lab.setFont(font);
		lab.setForeground(Color.white);
		//lab.setBorder(border);
		lab.setHorizontalAlignment(JLabel.RIGHT);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px1, py, pw1, ph);
		jp.add(s, lab);
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px2, py, pw2, ph);
		jp.add(s, maxhr_spinner);
		
		py += .07f;
		float px = .03f;
		float pw = .1f;
		ph = .05f;
		int i;
		
		hrcb = new JCheckBox[hrcolors.length];
		
		for (i=0; i<hrcb.length; i++)  {
			hrcb[i] = new JCheckBox(hrcolors[i]);
			hrcb[i].setHorizontalTextPosition(SwingConstants.LEFT);
			hrcb[i].setOpaque(false);
			hrcb[i].setFont(font);
			hrcb[i].setForeground(Color.white);
			hrcb[i].addActionListener(this);
			
			s = String.format("%.02f, %.02f, %.02f, %.02f ", px, py, pw, ph);
			jp.add(s , hrcb[i]);
			px += pw + .02f;
		}
		
		
		rpmbut = new JRadioButton[rpmcolors.length];
		
		py = .03f;
		rpmb_pw = .07f;
		
		radioGroup = new ButtonGroup();
		
		for (i=0; i<rpmbut.length; i++)  {
			rpmbut[i] = new JRadioButton(rpmcolors[i]);
			rpmbut[i].setHorizontalTextPosition(SwingConstants.LEFT);
			rpmbut[i].setOpaque(false);
			rpmbut[i].setFont(font);
			rpmbut[i].setForeground(Color.white);
			rpmbut[i].addActionListener(this);
			radioGroup.add(rpmbut[i]);
			if (i>0)  {
				rpmb_px += (rpmb_pw + .02);
			}
			s = String.format("%.02f, %.02f, %.02f, %.02f ", rpmb_px, py, rpmb_pw, ph);
			jp.add(s, rpmbut[i]);
		}		
		rpmbut[0].setSelected(true);

		
		
		buttons = new JButton[button_labels.length];
		
		px = .01f;
		py = .40f;
		pw = .1f;
		ph = .06f;
		
		for (i=0; i<buttons.length; i++)  {
			buttons[i] = new JButton(button_labels[i]);
			//keys[i].setHorizontalTextPosition(SwingConstants.LEFT);
			buttons[i].setOpaque(true);
			buttons[i].setFont(font);
			buttons[i].setForeground(Color.white);
			switch(i)  {
				case 0:
					buttons[i].setBackground(new Color(255, 155, 37));
					break;
				case 1:
					buttons[i].setBackground(Color.LIGHT_GRAY);
					break;
				case 2:
					buttons[i].setBackground(new Color(175, 214, 235));
					break;
				case 3:
					buttons[i].setBackground(Color.LIGHT_GRAY);
					break;
				case 4:
					buttons[i].setBackground(Color.LIGHT_GRAY);
					break;
				case 5:
					buttons[i].setBackground(new Color(175, 214, 235));
					break;
			}
			buttons[i].addActionListener(this);
			
			s = String.format("%.02f, %.02f, %.02f, %.02f ", px, py, pw, ph);
			jp.add(s , buttons[i]);
			px += pw + .02f;
			if (i==2)  {
				px = .01f;
				py += .10f;
			}
		}
		
		// get the broadcast address
		
		System.setProperty("java.net.preferIPv4Stack" , "true");
		
		Enumeration<NetworkInterface> interfaces = null;
		InetAddress broadcast = null;
		
		try {
			interfaces = NetworkInterface.getNetworkInterfaces();
		}
		catch (SocketException e) {
			e.printStackTrace();
		}
		boolean flag = true;
		
		while (flag && interfaces.hasMoreElements())  {
			NetworkInterface networkInterface = interfaces.nextElement();
			try {
				if (networkInterface.isLoopback())  {
					continue;
				}
			}
			catch (SocketException e) {
				e.printStackTrace();
			}
			// Don't want to broadcast to the loopback interface
			for (InterfaceAddress interfaceAddress : networkInterface.getInterfaceAddresses()) {
				broadcast = interfaceAddress.getBroadcast();
				if (broadcast == null)  {
					continue;
				}
				
				flag = false;
				break;
			}
		}		
		
		if (broadcast != null)  {
			s = broadcast.toString().trim();
			bcaddr = s.substring(1);
		}
		
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();					// 1920 x 1080
		setSize ( (int)(.5*d.getWidth()), (int)(.5*d.getHeight()) );
		setLocation((int)(.25*d.getWidth()), (int) (.25*getHeight()));
		// pack();

		setContentPane(jp);
		setTitle("wifi hbsim" + "  " + name);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
		setVisible(true);
		jp.setFocusable(true);
		jp.requestFocus();
		
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				System.out.printf("\n%s: calling cleanup()\n", Thread.currentThread().getName());		// Thread-2
				cleanup();
			}
		});
		
		RAM = Runtime.getRuntime().totalMemory();
	    System.out.println("memory at end of constructor " + RAM + " bytes.");
		
	}											// constructor()

	/************************************************************************************

	************************************************************************************/

	private void myrun()  {								// not runnable!
	    int n=-1;
	    int status = 0;
		flag = true;
		host = "127.0.0.1";
		port = 9072;
		String decoded = null;		
		byte[] bytes = null;
		//byte[] txbuf = null;
		boolean ipdiscover = false;

		//threadMessage("where am I?");									// main
		System.out.printf("%s: myrun 1\n", Thread.currentThread().getName());
		
		// start a bunch of customers (threads)
		
		//x
		
		
		while(flag)  {
			
			//-------------------------------------------------
			// try to connect to server:
			//-------------------------------------------------
			
			long now = 0;
			long now2 = 0;

			System.out.printf("trying to connect to server\n");
			
			while(true)  {

				try {
					if (ipdiscover)  {
						socket = new Socket(bcaddr, port);
					}
					else  {
						socket = new Socket(host, port);
					}
					
					socket.setSoTimeout(1000);
					in = new DataInputStream(socket.getInputStream( ));
					out = new DataOutputStream(socket.getOutputStream());
					break;
				}
			  	catch (UnknownHostException e) {
					System.err.println("Don't know about host " + host);
					System.exit(1);
				}
			  	catch (IOException e) {
			  		//System.out.printf("%s\n", e.getMessage());			// connection refused, server not running yet
			  		//bp = 3;
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
					System.out.printf(".");
				}
			}									// while(true)		trying to connect to server
			
			System.out.printf("\nconnected to server\n");

			totaldiff = diff = 0;
			
			bytes = new byte[9];
			long now3 = System.currentTimeMillis();

			//-----------------------------
			// main loop
			//-----------------------------
			
			while(true)  {
				try {
				    n = in.read(bytes, 0, 9);
				    bp = 0;
				}
				catch (SocketTimeoutException se) {
					System.out.println("Socket timed out!");
					diff = System.currentTimeMillis() - now3;				// diff should equal 1000 ms (it did)
					totaldiff += diff;
					diffcnt++;
					continue;
				}
			  	catch (IOException e) {
			  		System.out.printf("%s, n = %d\n", e.getMessage(), n);					// connection reset
			  		//continue;
			  		//socket = null;
			  		break;								// break to the outer loop
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
					default:
						break;
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
				
				/****************************************************************************************
				 	count down pkt_timeout to zero at 100 hz. It is +=28 when new pc packet received
				 	bound to 255. If it is over 100 allow sending of hb data to pc.
				 	if it reaches zero, clear the pc data struct hbc, fake stand-alone mode.
				****************************************************************************************/
				
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
				
			}										// while(true)

			
			
			
			
/*			
			//-------------------------------------------
			// now try to receive 'RacerMate'
			// the socket will time out every 1000 ms
			//-------------------------------------------
			
			bytes = new byte[9];
			long now3 = System.currentTimeMillis();

			while(true)  {
				try {
				    n = in.read(bytes, 0, 9);
				}
				catch (SocketTimeoutException se) {
					System.out.println("Socket timed out!");
					long diff = System.currentTimeMillis() - now3;				// diff should equal 1000 ms (it did)
					continue;
				}
			  	catch (IOException e) {
			  		System.out.printf("%s, n = %d\n", e.getMessage(), n);					// connection reset
			  		continue;
				}
				
		        if (n != 9) {
		        	System.out.printf("bytes read = %d\n", n);
		        	continue;
				}
						
				decoded = new String(bytes, "UTF-8");
				
				if (decoded.equals("RacerMate"))  {
					connected = true;
					break;
				}
			}													// while(true) trying to receive "RacerMate"
			
			bp = 3;

			send_LinkUp();
			
			try {
				Thread.sleep(700);
			}
			catch (InterruptedException e) {
				e.printStackTrace();
			}				// sleep 100 ms
			
			//--------------------------------------------------------------------------
			// keep reading from and writing to server as long as we're connected
			// this is the real main loop after we're connected
			//--------------------------------------------------------------------------

			socket.setSoTimeout(10);
			
			bytes = new byte[256];
			int len = bytes.length;
			int cnt = 0;
			
			while(true)  {
				
				try {
				    n = in.read(bytes, 0, len);								// times out every 10 ms, 0 = offset into bytes
				}
				catch (SocketTimeoutException se) {
					long diff = System.currentTimeMillis() - now3;				// diff should equal 10 ms (it did)
					cnt++;
					//send_pc();
					tick++;						// 0-99 10ms ticks == 1/sec.

					if (tick >= 100) {
						tick = 0;
					}

					
					//continue;
				}

				if (n!=-1)  {
					// new data arrived
				}

				//---------------------------------------------
				// poll and handle received pc chars
				//---------------------------------------------
				
				status = read_pc();
				
				switch (status) {
					case 1:
						//decode_pc(pkt, &hbc);
						decode_pc();
						pkt_timeout += 28;

						if (pkt_timeout > 255) {
							pkt_timeout = 255;
						}
						break;
					case 2:
						pkt_timeout = 0;
						//send_LinkUp(cc[hbar].fd);
						send_LinkUp();
						//fprintf(logfile, "LinkUp sent\n");
						break;
					default:
						break;
				}

				
			}												// while(true)  {
*/			
		}										// while(flag)

		
		return;
	}											// run() foo() mythread2()
 
	
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
		
		//n = read(hbarfd, &workbuf, 32);
		
		try {
		    n = in.read(workbuf, 0, workbuf.length);
		}
		catch (SocketTimeoutException se) {
		}
		catch (IOException e) {
			e.printStackTrace();
		}
		
		if (n < 0) {
			//err = errno;
			//fprintf(logfile, "read_pc returned %d, errno %d: %s\n",
			//        n, err, strerror(err));
			System.out.printf("ooops\n");
			return n;
		}

		if (n!=0) {
			for (i=0; i<n; i++)  {
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
		}				//if(n)

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

	public void keyPressed(KeyEvent e) {
		bp = 1;
	}

	/*******************************************************************************

	*******************************************************************************/

	public void keyReleased(KeyEvent e) {
		bp = 2;
		
	}

	/*******************************************************************************

	*******************************************************************************/

	public void keyTyped(KeyEvent e) {
		char c = e.getKeyChar();

		switch((int)c)  {
			case (int)'x':
			case (int)'X':
				bp = 0;
				break;
			/*
			case 0x1b:
				bp = 2;
				break;
			*/
				
			case KeyEvent.VK_ESCAPE:
				/*
				if (diffcnt != 0)  {
					System.out.printf("\naverage dt 1 = %.3f ms\n", (float)(totaldiff / diffcnt) );
				}
				*/
				
				System.exit(0);										// this causes endapp() to be called
				break;

			case KeyEvent.VK_RIGHT:
				bp = 2;
				break;
				
			default:
				bp = 1;
				break;
		}							// switch(c)

		return;
	}

	/*******************************************************************************
	 
	*******************************************************************************/

	private void cleanup()  {
		String s;
		System.out.printf("%s: cleanup 1\n", Thread.currentThread().getName());								// Thread-2

  		//rtd.setCurrent(3.0);			// stop the velotron

		try {
			Thread.sleep(500);
		}
		catch (InterruptedException ignored)  {
		}

		if (diffcnt != 0)  {
			System.out.printf("\naverage dt 2 = %.3f ms, diffcnt = %d\n", (float)(totaldiff / diffcnt), diffcnt );
		}
		
		System.out.printf("\nending\n");
	}

	/*******************************************************************************
	 	spinner event listener
	*******************************************************************************/

	public void stateChanged(ChangeEvent e) {
		String s;
		Object o;
		
		o = e.getSource();
		s = ((JSpinner)o).getName();
		
		if (o==rpm_spinner)  {
			hbd.rpm = (int) ((JSpinner)o).getValue();
		}
		else if (o==mph_spinner)  {
			hbd.i_mph = (int) ((JSpinner)o).getValue();
			hbd.f_mph = hbd.i_mph;
		}
		else if (o==watts_spinner)  {
			hbd.watts = (int) ((JSpinner)o).getValue();
		}
		else if (o==hr_spinner)  {
			hbd.hrate = (int) ((JSpinner)o).getValue();
		}
		else if (o==minhr_spinner)  {
			hbd.minhr = (int) ((JSpinner)o).getValue();
		}
		else if (o==maxhr_spinner)  {
			hbd.maxhr = (int) ((JSpinner)o).getValue();
		}
		else  {
			bp = 1;
		}
		
		//statusLabel.setText("Value : " + ((JSpinner)e.getSource()).getValue());
		return;
	}						// stateChanged()

	/*******************************************************************************
 	 radiobutton event listener
	*******************************************************************************/

	public void actionPerformed(ActionEvent e)  {
		String s;
		int i, n;
		
		s = e.getActionCommand();
		
		if (s.equals("SW"))  {
			hbd.rpmflags = 1;
		}
		else if (s.equals("OP"))  {
			hbd.rpmflags = 2;
		}
		else if (s.equals("NV"))  {
			hbd.rpmflags = 4;
		}
		
		else if (s.equals("out"))  {
			hbd.hrflags = 0x00;
			JCheckBox cb = (JCheckBox) e.getSource();
			n = hrcb.length;
			
			if (cb.isSelected())  {
				for(i=1; i<n; i++)  {
					hrcb[i].setEnabled(false);
				}
			}
			else  {
				for(i=1; i<n; i++)  {
					hrcb[i].setEnabled(true);
				}
			}
		}
		else if (s.equals("L"))  {
			hbd.hrflags = 0x01;
		}
		else if (s.equals("H"))  {
			hbd.hrflags = 0x02;
		}
		else if (s.equals("E"))  {
			hbd.hrflags = 0x04;
		}
		else if (s.equals("b"))  {
			hbd.hrflags = 0x08;
		}
		else if (s.equals("F1"))  {
			hbd.keys = 0x01;
		}
		else if (s.equals("F2"))  {
			hbd.keys = 0x02;
		}
		else if (s.equals("UP"))  {
			hbd.keys = 0x04;
		}
		else if (s.equals("Fn"))  {
			hbd.keys = 0x08;
		}
		else if (s.equals("F3"))  {
			hbd.keys = 0x10;
		}
		else if (s.equals("DN"))  {
			hbd.keys = 0x20;
		}
		else  {
        	bp = 2;
        }		

		jp.requestFocus();
		
		return;
	}								// actionPerformed()
	
	/************************************************************************************

	************************************************************************************/

	public static void main(String[] args) throws IOException, InterruptedException {
		//Client client = new Client("Client 1");
		
		/*
		Thread c1 = new Thread(new Client("Client 1"));
		c1.start();
		System.out.printf("\nstarted Client 1\n");
		 */
		/*
		Thread c2 = new Thread(new Client("Client 2"));
		c2.start();
		System.out.printf("started Client 2\n");
		*/
		
		System.out.printf("main creating Client1\n");
		Client c1 = new Client("Client 1");
		System.out.printf("%s: creating Client1\n", Thread.currentThread().getName());
		System.out.printf("%s: created Client1\n", Thread.currentThread().getName());
		System.out.printf("%s: running Client1\n", Thread.currentThread().getName());
		c1.myrun();

		System.out.printf("%s: back from myrun()\n", Thread.currentThread().getName());
		System.out.printf("%s: done\n", Thread.currentThread().getName());
		
		/*
		long wait_time = 1000 * 10;							// 10 seconds
		long start;
		
		threadMessage("Starting mythread");
		start = System.currentTimeMillis();

		Thread t = new Thread(c1.new mythread());
		t.start();

		threadMessage("Waiting for MessageLoop thread to finish");

		// loop until MessageLoop thread exits

		while (t.isAlive())  {
			threadMessage("waiting...");
			t.join(1000);			// wait maximum of 1 second for mythread to finish

			if (((System.currentTimeMillis() - start) >= wait_time) && t.isAlive()) {
				threadMessage("Tired of waiting!");
				t.interrupt();
				t.join();
			}
		}
		
		threadMessage("Finally!");
		*/
		
		/*
		client.threadMessage("Starting mythread2");

		Thread t = new Thread(client.new mythread2());
		t.start();

		client.threadMessage("mythread2 starting");

		while (t.isAlive())  {
			//threadMessage("waiting...");
			t.join(1000);			// wait maximum of 1 second for mythread to finish

			//if (((System.currentTimeMillis() - start) >= wait_time) && t.isAlive()) {
			//	threadMessage("Tired of waiting!");
			//	t.interrupt();
			//	t.join();
			//}
		}
		
		client.threadMessage("done!");
		
		System.exit(0);
		*/
		
	}											// main

	/*******************************************************************************

	*******************************************************************************/

	/*
	private class mythread implements Runnable  {
		public void run() {
			int i;
			String importantInfo[] = {
				"Mares eat oats",
				"Does eat oats",
				"Little lambs eat ivy",
				"A kid will eat ivy too"
			};
			
			try  {
				for (i=0; i<importantInfo.length; i++)  {
					Thread.sleep(4000);
					threadMessage(importantInfo[i]);
				}
			}
			catch (InterruptedException e) {
				threadMessage("I wasn't done!");
			}
		}				// run()
	}					// class MessageLoop
	*/
	
	
}												// class client

