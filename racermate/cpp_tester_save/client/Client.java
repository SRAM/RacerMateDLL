import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.*;
import java.net.*;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Statement;
import java.util.Collections;
import java.util.Date;
import java.util.Enumeration;
import java.util.Properties;
import java.util.Random;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFormattedTextField;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;
import javax.swing.JTabbedPane;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

//import mythread.hbdata;
import utils.*;

/************************************************************************************
	simulates the wifi radio / handlebar combination. This is a tcp client that
	trys to connect to 'host' on 'port'. Once connected it listens for "RacerMate",
	sends "LinkUp" and then sends data to the server (pc).
************************************************************************************/

public class Client extends JFrame implements KeyListener, ChangeListener, ActionListener, ItemListener  {

	private final int MAXTRAINERS = 8;						// Number of Trainers (threads)
	mythread[] t = null;
	int ix;											// which trainer we are now controlling (0, 1, 2, ...)
	private boolean flag = false;
	private boolean clean = false;
	private int bp = 0;
	private Properties sysprops = null;
	private Random rand = null;		
	private long RAM = 0;
	
	
	private JTabbedPane jt = null;
    private JPanel jp = null;
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

	private JComboBox client_combo = null;
	
	private ChangeListener listener = null;				// single listener for all spinners
	
	private ButtonGroup radioGroup;
	private JRadioButton[] rpmbut;
	private String[] rpmcolors = {"SW", "OP", "NV"};
	private JCheckBox[] hrcb;
	private String[] hrcolors = {"out", "L", "H", "E", "b"};
	private JButton[] buttons;
	private String[] button_labels = {"Reset", "F2", "+", "F1", "F3", "-"};
	private String name = "not set";
	private Connection conn = null;
	boolean verbose = false;
	private DatabaseMetaData dma;
	private JButton connect_button=null;
	private JButton disconnect_button=null;
	//private boolean[] connected = null;
	
	/**
	 * @throws IOException **********************************************************************************

	************************************************************************************/

	Client(String _name) throws IOException  {
		super();
		
		int i;
		
		/*
		try {
			test();
		}
		catch (Exception e1) {
			e1.printStackTrace();
		}
		System.exit(0);
		*/
		
		
		RAM = Runtime.getRuntime().totalMemory();
	    System.out.println("memory at beginning of constructor " + RAM + " bytes.");

	    name = _name;
	    ix = 0;
		t = new mythread[MAXTRAINERS];
		for(i=0; i<MAXTRAINERS; i++)  {
			t[i] = null;
		}
	    
	    sysprops = System.getProperties();
	
		rand = new Random(System.currentTimeMillis());		
		bp = 0;
		
		jt = new JTabbedPane();
		
		jp = new JPanel(new RatioLayout());
		jp.setBackground(new Color(100, 100, 100));
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
		rpm_spinnerModel = new SpinnerNumberModel(60, 0, 200, 1);		// initial value, min, max, step
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
		mph_spinnerModel = new SpinnerNumberModel(16, 0, 60, 1);		// initial value, min, max, step
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
		watts_spinnerModel = new SpinnerNumberModel(100, 0, 1000, 10);		// initial value, min, max, step
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
		hr_spinnerModel = new SpinnerNumberModel(70, 40, 255, 1);		// initial value, min, max, step
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
		minhr_spinnerModel = new SpinnerNumberModel(45, 40, 255, 1);		// initial value, min, max, step
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
		maxhr_spinnerModel = new SpinnerNumberModel(167, 40, 255, 1);		// initial value, min, max, step
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

		connect_button = new JButton("Connect");
		connect_button.setOpaque(true);
		connect_button.setFont(font);
		connect_button.setForeground(Color.white);
		connect_button.setBackground(Color.RED);
		connect_button.setEnabled(true);
		connect_button.addActionListener(this);
		px = .5f;
		py = .5f;
		pw = .15f;
		ph = .05f;
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px, py, pw, ph);
		jp.add(s , connect_button);

		disconnect_button = new JButton("Disconnect");
		disconnect_button.setOpaque(true);
		disconnect_button.setFont(font);
		disconnect_button.setForeground(Color.white);
		disconnect_button.addActionListener(this);
		disconnect_button.setBackground(Color.RED);
		disconnect_button.setEnabled(false);
		px = .5f;
		py = .6f;
		pw = .15f;
		ph = .05f;
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px, py, pw, ph);
		jp.add(s , disconnect_button);
		
		//-----------------------
		// client_combo
		//-----------------------

		client_combo = new JComboBox();
		client_combo.addActionListener(this);
		client_combo.addItemListener(this);
		
		//jp.add(client_combo);
		px = .8f;
		py = .2f;
		pw = .1f;
		ph = .05f;
		s = String.format("%.02f, %.02f, %.02f, %.02f ", px, py, pw, ph);
		jp.add(s , client_combo);
		
		for(i=0; i<MAXTRAINERS; i++)  {
			s = String.valueOf(i);
			client_combo.addItem(makeObj(s));
		}
		
		s = getIpAddress();								// works: "192.168.1.20"

		/*
		connected = new boolean[MAXTRAINERS];
		for(i=0; i<MAXTRAINERS; i++)  {
			connected[i] = false;
		}
		*/
		

		/*
		InetAddress iad = InetAddress.getLocalHost();
		
		// get the broadcast address
		System.setProperty("java.net.preferIPv4Stack" , "true");
	
		int cnt = 0;
		Enumeration<NetworkInterface> nets = NetworkInterface.getNetworkInterfaces();
		for (NetworkInterface netint : Collections.list(nets))  {
			cnt++;														// cnt = 88 without ipv4 set
			displayInterfaceInformation(cnt, netint);
		}
		
		
		
		Enumeration<NetworkInterface> interfaces = null;
		InetAddress broadcast = null;
		
		try {
			interfaces = NetworkInterface.getNetworkInterfaces();
		}
		catch (SocketException e) {
			e.printStackTrace();
		}
		boolean flag = true;
		*/
		
		/*
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
		*/
		
		
		//jt.addTab("Tab 1", null, jp, "Does nothing");
		
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();					// 1920 x 1080
		int w;
		w = (int)d.getWidth();							// 1920
		w = (int)(.5*d.getWidth());						// 960
		setSize ( w, (int)(.5*d.getHeight()) );
		int x = (int)d.getWidth() - w - 20;
		setLocation(x, (int) (.25*getHeight()));
		// pack();

		setContentPane(jp);
		setTitle("wifi hbsim" + "  " + name);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        
		setVisible(true);
		jp.setFocusable(true);
		jp.requestFocus();
		
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				//System.out.printf("\n%s: calling cleanup()\n", Thread.currentThread().getName());		// Thread-2
				cleanup();
				//System.out.printf("sleeping\n");
				try {
					Thread.sleep(1000);
				}
				catch (InterruptedException e) {
					e.printStackTrace();
				}
				System.out.printf("\n\ndone\n");
			}
		});
		
		RAM = Runtime.getRuntime().totalMemory();
	    System.out.println("memory at end of constructor " + RAM + " bytes.");
		
	}											// constructor()

	/************************************************************************************

	************************************************************************************/

	private void myrun()  {								// not runnable!
	    int i;
		flag = true;

		System.out.printf("%s: myrun 1\n", Thread.currentThread().getName());
		
		
		/*
		String s;
		
		for(i=0; i<NT; i++)  {
			//t[i] = new Thread(mythread(i)).start();
			t[i] = new mythread(i);
			//s = i.
			s = String.valueOf(i+1);
			client_combo.addItem(makeObj(s));
			//client_combo.addItem(makeObj("2"));
			//t[i].run();
		}
		
		for(i=0; i<NT; i++)  {
			//(new Thread(new HelloRunnable())).start();
			//t[i].s
			t[i].start();
		}
		*/
		
		boolean b = true;
		
		while(b)  {
			try {
				
				// loop through any trainer threads to reset things in case server shut down
				
				for(i=0; i<MAXTRAINERS; i++)  {
					if (t[i] != null)  {
						if (t[i].is_socket_connected() == false)  {
							// update the button enable (and color?)
							//disconnect_button.setEnabled(false);
							//connect_button.setEnabled(true);
							
						}
					}
				}
				Thread.sleep(233);
			}
			catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		System.out.printf("%s: myrun() ending\n", Thread.currentThread().getName());
		
		return;
	}											// myrun()
 
	




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
		int i;
		boolean b = true;
		long now;
		int status;
		
		if (clean)  {
			return;
		}

		clean = true;
		
		//System.out.printf("%s: cleanup 1\n", Thread.currentThread().getName());								// Thread-2

		for(i=0; i<MAXTRAINERS; i++)  {
			if (t[i] != null)  {
				status = t[i].shutdown();
				if (status != 0)  {
					bp = 4;
				}
			}
		}

		/*
		int killed = 0;
		
		for(i=0; i<NT; i++)  {
			if (t[i] == null)  {
				continue;
			}
			
			now = System.currentTimeMillis();

			while(b)  {
				if (!t[i].is_running())  {
					killed++;
					break;
				}
				if ((System.currentTimeMillis() - now) >= 5000)  {
					break;
				}
			}
			
			if (t[i] != null)  {
				try {
					Thread.sleep(15);
				}
				catch (InterruptedException ignored)  {
					bp = 8;
				}
			}
		}									// for(i=0; i<NT; i++)
		*/
		
		
		/*
		if (killed != NT)  {
			bp = 1;
			System.out.printf("killed = %d\n", killed);
		}
		else  {
			System.out.printf("killed all threads\n");
		}
		*/
		
		
  		//rtd.setCurrent(3.0);			// stop the velotron

		try {
			Thread.sleep(500);
		}
		catch (InterruptedException ignored)  {
		}

		/*
		if (diffcnt != 0)  {
			System.out.printf("\naverage dt 2 = %.3f ms, diffcnt = %d\n", (float)(totaldiff / diffcnt), diffcnt );
		}
		*/
		
		
		try {
			if (conn != null)  {
				conn.close();
				conn = null;
			}
		}
		catch (SQLException e) {
			e.printStackTrace();
		}
		
		System.gc();
		System.runFinalization();

		//System.out.printf("\ncleanup x\n");
	}										// cleanup()

	/*******************************************************************************
	 	event listener for
	 		spinners
	*******************************************************************************/

	public void stateChanged(ChangeEvent e) {
		String s;
		Object o;
		int i;

		if (ix<0 || ix>=MAXTRAINERS)  {
			return;
		}

		o = e.getSource();
		s = ((JSpinner)o).getName();

		
		if (o==rpm_spinner)  {
			i = (int) ((JSpinner)o).getValue();			
			t[ix].set_rpm(i);
		}
		else if (o==mph_spinner)  {
			i = (int) ((JSpinner)o).getValue();			
			t[ix].set_mph(i);
		}
		else if (o==watts_spinner)  {
			i = (int) ((JSpinner)o).getValue();			
			t[ix].set_watts(i);
		}
		else if (o==hr_spinner)  {
			i = (int) ((JSpinner)o).getValue();			
			t[ix].set_hrate(i);
		}
		else if (o==minhr_spinner)  {
			i = (int) ((JSpinner)o).getValue();			
			t[ix].set_minhr(i);
		}
		else if (o==maxhr_spinner)  {
			i = (int) ((JSpinner)o).getValue();			
			t[ix].set_maxhr(i);
		}
		else  {
			bp = 1;
		}
		
		jp.requestFocus();
		//statusLabel.setText("Value : " + ((JSpinner)e.getSource()).getValue());
		return;
	}						// stateChanged()

	/*******************************************************************************
		event listener for:
 	 		radiobuttons
 	 		buttons				button listener
 	 		combo boxes
	*******************************************************************************/

	public void actionPerformed(ActionEvent e)  {
		String s;
		int i, n;
		
		s = e.getActionCommand();

		bp = 1;
		
		if (s.equals("SW"))  {
			//hbd.rpmflags = 1;
			t[ix].set_rpm_flags(1);
			
		}
		else if (s.equals("OP"))  {
			t[ix].set_rpm_flags(2);
		}
		else if (s.equals("NV"))  {
			t[ix].set_rpm_flags(4);
		}
		
		else if (s.equals("out"))  {
			t[ix].set_hr_flags(0x00);
			
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
			t[ix].set_hr_flags(0x01);
		}
		else if (s.equals("H"))  {
			t[ix].set_hr_flags(0x02);
		}
		else if (s.equals("E"))  {
			t[ix].set_hr_flags(0x04);
		}
		else if (s.equals("b"))  {
			t[ix].set_hr_flags(0x08);
		}
		else if (s.equals("F1"))  {
			t[ix].set_keys(0x01);
		}
		else if (s.equals("F2"))  {
			t[ix].set_keys(0x02);
		}
		else if (s.equals("UP"))  {
			t[ix].set_keys(0x04);
		}
		else if (s.equals("Fn"))  {
			t[ix].set_keys(0x08);
		}
		else if (s.equals("F3"))  {
			t[ix].set_keys(0x10);
		}
		else if (s.equals("DN"))  {
			t[ix].set_keys(0x20);
		}
		//xxx
		else if (s.equals("comboBoxChanged"))  {				// itemstatechanged() has already been called
			bp = 1;
		}
		else if (s.equals("Connect"))  {
			//if (!connected[ix])  {
			if (t[ix] == null)  {
				t[ix] = new mythread(ix);
				t[ix].start();
				//connected[ix] = true;
				connect_button.setEnabled(false);
				disconnect_button.setEnabled(true);
			}
		}
		else if (s.equals("Disconnect"))  {
			//if (connected[ix])  {
			if (t[ix] != null)  {
				//System.out.printf("calling shutdown\n");
				int status = t[ix].shutdown();
				//System.out.printf("back from shutdown, status = %d\n", status);
				try {
					//System.out.printf("joining\n");
					t[ix].join();
					//System.out.printf("join ok\n");
				}
				catch (InterruptedException e1) {
					e1.printStackTrace();
				}
				t[ix] = null;
			}
			disconnect_button.setEnabled(false);
			connect_button.setEnabled(true);
			//connected[ix] = false;
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
		System.out.printf("main creating Client\n");
		Client c = new Client("Clients Tester");
		System.out.printf("%s: creating Client\n", Thread.currentThread().getName());
		System.out.printf("%s: created Client\n", Thread.currentThread().getName());
		System.out.printf("%s: running Client\n", Thread.currentThread().getName());
		c.myrun();										// loops until mythread() is finished or until client is destroyed

		System.out.printf("main, %s: back from myrun()\n", Thread.currentThread().getName());
		System.out.printf("main, %s: exiting\n", Thread.currentThread().getName());
		
	}											// main


	/************************************************************************************

	************************************************************************************/

	public Connection Connect(String url, String user, String pw) {
		try {
			//Class.forName("interbase.interclient.Driver");
			Class.forName ("com.mysql.jdbc.Driver").newInstance();

			if (verbose )  {
				//DriverManager.setLogStream(System.out);
			}
			else  {
				DriverManager.setLogStream(null);
			}

			conn = DriverManager.getConnection(url, user, pw);

			//-----------------------------------------------------
			// If we were unable to connect, an exception
			// would have been thrown.		So, if we get here,
			// we are successfully connected to the URL
			//-----------------------------------------------------

			//-----------------------------------------------------
			// Check for, and display and warnings generated
			// by the connect.
			//-----------------------------------------------------

			checkForWarning(conn.getWarnings());

			//-----------------------------------------------------
			// Get the DatabaseMetaData object and display
			// some information about the connection
			//-----------------------------------------------------

			dma = conn.getMetaData();

			if (verbose)  {
				/*
					System.out.println("\nConnected to " + dma.getURL());
					System.out.println("Driver			" + dma.getDriverName());
					System.out.println("Version		" + dma.getDriverVersion());
					System.out.println("");
					*/
			}
		}
		catch(SQLException ex) {

			//-----------------------------------------------------
			// A SQLException was generated.		Catch it and
			// display the error information.		Note that there
			// could be multiple error objects chained
			// together
			//-----------------------------------------------------

			System.out.println("\n*** SQLException caught ***\n");

			while(ex != null) {
				System.out.println("SQLState: " + ex.getSQLState());
				System.out.println("Message:		" + ex.getMessage());
				System.out.println("Vendor:	" + ex.getErrorCode());
				ex = ex.getNextException();
				System.out.println("");
			}
			System.exit(1);
		}
		catch(java.lang.Exception ex) {
			ex.printStackTrace();
			System.exit(1);
		}
		return conn;
	}

	//-------------------------------------------------------------------
	// checkForWarning
	// Checks for and displays warnings.		Returns true if a warning
	// existed
	//-------------------------------------------------------------------

	private boolean checkForWarning(SQLWarning warn) throws SQLException {
		boolean rc = false;

		// If a SQLWarning object was given, display the
		// warning messages.		Note that there could be
		// multiple warnings chained together

		if(warn != null) {
			if (verbose)  {
				System.out.println("\n *** Warning ***\n");
			}
			rc = true;
			while(warn != null) {
				if (verbose)  {
					/*
						System.out.println("SQLState: " + warn.getSQLState());
						System.out.println("Message:		" + warn.getMessage());
						System.out.println("Vendor:	" + warn.getErrorCode());
						System.out.println("");
						*/
				}
				warn = warn.getNextWarning();
			}
		}
		return rc;
	}

	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------

	private Object makeObj(final String item)  {
		return new Object() { public String toString() { return item; } };
	}

	//-------------------------------------------------------------------
	// listener for
	//	combo box *changes*
	//-------------------------------------------------------------------

	@Override
	public void itemStateChanged(ItemEvent e) {
		Object o;
		String s;
		int state;
		
		o = e.getItem();
		s = o.toString();					// "1", "2", etc
		
		state = e.getStateChange();
		
		//xxx
		if (state == ItemEvent.SELECTED)  {
			ix = Integer.parseInt(s);
			
			if (ix<0 || ix>=MAXTRAINERS)  {
				ix = 0;
				client_combo.setSelectedIndex(ix);
			}
			else  {
				//if (connected != null)  {
				//if (!connected[ix])  {
				//if ((t != null) && (t[ix]==null))  {
				if (t[ix]==null)  {
					connect_button.setEnabled(true);
					disconnect_button.setEnabled(false);
				}
				else  {
					connect_button.setEnabled(false);
					disconnect_button.setEnabled(true);
				}
				//}
			}
		}
		else  {								// if (state == ItemEvent.SELECTED)  {
			
		}
		
		return;
	}
	 
	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------

	void displayInterfaceInformation(int cnt, NetworkInterface netint) throws SocketException {
		
        System.out.printf("%2d  Display name: %s\n", cnt, netint.getDisplayName());
        System.out.printf("      Name: %s\n", netint.getName());
        
        Enumeration<InetAddress> inetAddresses = netint.getInetAddresses();
        
        for (InetAddress inetAddress : Collections.list(inetAddresses))  {
        	System.out.printf("      InetAddress: %s\n", inetAddress);
        }
        
        System.out.printf("\n");
     }
	
	//-------------------------------------------------------------------
	//
	//-------------------------------------------------------------------

	String getIpAddress() { 
    	InetAddress broadcast = null;
        InetAddress inetAddress = null;
    	
        try {
            for (Enumeration en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements();)  {
                NetworkInterface intf = (NetworkInterface) en.nextElement();
                
                for (Enumeration enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements();)  {
                    inetAddress = (InetAddress) enumIpAddr.nextElement();
                    
                    if (!inetAddress.isLoopbackAddress() && inetAddress instanceof Inet4Address)  {
                    	/*
                    	for (InterfaceAddress interfaceAddress : intf.getInterfaceAddresses()) {
                    		broadcast = interfaceAddress.getBroadcast();
                    		if (broadcast == null)  {
                    			continue;
                    		}
                    		else  {
                    			bp = 1;
                    		}
                    	}
                    	*/
                    	
                        String ipAddress=inetAddress.getHostAddress().toString();
                        return ipAddress;
                    }
                }
            }
        }
        catch (SocketException ex) {
            //Log.e("Socket exception in GetIP Address of Utilities", ex.toString());
        	bp = 1;
        }
        return null; 
	}
	
	
	/************************************************************************************

	************************************************************************************/

	/*
	private void close() {
	    try {
	      if (resultSet != null) {
	        resultSet.close();
	      }

	      if (statement != null) {
	        statement.close();
	      }

	      if (connect != null) {
	        connect.close();
	      }
	    } catch (Exception e) {

	    }
	  }				// close()
		*/
	
	/************************************************************************************

	************************************************************************************/

	private void writeMetaData(ResultSet resultSet) throws SQLException {
	    //   Now get some metadata from the database
	    // Result set get the result of the SQL query
	    
	    System.out.println("The columns in the table are: ");
	    
	    System.out.println("Table: " + resultSet.getMetaData().getTableName(1));
	    for  (int i = 1; i<= resultSet.getMetaData().getColumnCount(); i++){
	      System.out.println("Column " +i  + " "+ resultSet.getMetaData().getColumnName(i));
	    }
	  }
	
	/************************************************************************************

	************************************************************************************/

	private void writeResultSet(ResultSet resultSet) throws SQLException {
	    // ResultSet is initially before the first data set
	    while (resultSet.next()) {
	      // It is possible to get the columns via name
	      // also possible to get the columns via the column number
	      // which starts at 1
	      // e.g. resultSet.getSTring(2);
	      String user = resultSet.getString("myuser");
	      String website = resultSet.getString("webpage");
	      String summery = resultSet.getString("summery");
	      Date date = resultSet.getDate("datum");
	      String comment = resultSet.getString("comments");
	      System.out.println("User: " + user);
	      System.out.println("Website: " + website);
	      System.out.println("Summery: " + summery);
	      System.out.println("Date: " + date);
	      System.out.println("Comment: " + comment);
	    }
	  }
	
	/**
	 * @throws Exception **********************************************************************************

	************************************************************************************/

	private void test() throws Exception  {
		String tdate;
		String chknum;
		String what;
		String debit;
		String credit;
		String who;
		String line;
		String[] a = null;
		int len;
		String cmd;
		
		Connection connect = null;
		Statement statement = null;
		PreparedStatement preparedStatement = null;
		ResultSet resultSet = null;
		
		
		try {
			/// This will load the MySQL driver, each DB has its own driver
		      Class.forName("com.mysql.jdbc.Driver");
		      // Setup the connection with the DB
		      connect = DriverManager.getConnection("jdbc:mysql://localhost/feedback?user=sqluser&password=sqluserpw");

		      // Statements allow to issue SQL queries to the database
		      statement = connect.createStatement();
		      // Result set get the result of the SQL query
		      resultSet = statement.executeQuery("select * from feedback.comments");
		      writeResultSet(resultSet);

		      // PreparedStatements can use variables and are more efficient
		      preparedStatement = connect
		          .prepareStatement("insert into  feedback.comments values (default, ?, ?, ?, ? , ?, ?)");
		      // "myuser, webpage, datum, summery, COMMENTS from feedback.comments");
		      // Parameters start with 1
		      preparedStatement.setString(1, "Test");
		      preparedStatement.setString(2, "TestEmail");
		      preparedStatement.setString(3, "TestWebpage");
		      preparedStatement.setDate(4, new java.sql.Date(2009, 12, 11));
		      preparedStatement.setString(5, "TestSummary");
		      preparedStatement.setString(6, "TestComment");
		      preparedStatement.executeUpdate();

		      preparedStatement = connect
		          .prepareStatement("SELECT myuser, webpage, datum, summery, COMMENTS from feedback.comments");
		      resultSet = preparedStatement.executeQuery();
		      writeResultSet(resultSet);

		      // Remove again the insert comment
		      preparedStatement = connect
		      .prepareStatement("delete from feedback.comments where myuser= ? ; ");
		      preparedStatement.setString(1, "Test");
		      preparedStatement.executeUpdate();
		      
		      resultSet = statement
		      .executeQuery("select * from feedback.comments");
		      writeMetaData(resultSet);
		      
		    }
		catch (Exception e) {
		      throw e;
		}
		finally {
			//close();
		    try {
			      if (resultSet != null) {
			        resultSet.close();
			      }

			      if (statement != null) {
			        statement.close();
			      }

			      if (connect != null) {
			        connect.close();
			      }
			    }
		    catch (Exception e) {

			}
			
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		//String fname = "c:\\users\\larry\\desktop\\";
		String fname = "/home/larry/Desktop/asb/dec.csv";
		//conn = Connect("jdbc:mysql://192.168.1.200/checking", "larry", "barbaz99" );
	
		BufferedReader br;
		
		br = new BufferedReader(new FileReader(fname));
		
		bp = 0;
		
		while ((line = br.readLine()) != null) {
			if (line.length()==0)  {
				continue;
			}
			if (line.startsWith("<Date>"))  {
				continue;
			}
			
			a = line.split(",");

			len = a.length;
			switch(len)  {
				case 4:
					tdate = a[0];
					chknum = a[1];
					what = a[2];
					debit = a[3];
					credit = "";
					who = "";
					break;
				case 5:
					tdate = a[0];
					chknum = a[1];
					what = a[2];
					debit = a[3];
					credit = a[4];
					who = "";
					break;
				case 6:
					tdate = a[0];
					chknum = a[1];
					what = a[2];
					debit = a[3];
					credit = a[4];
					who = a[5];
					break;
				case 7:								// embedded comma inside quotes for 'who'
					tdate = a[0];
					chknum = a[1];
					what = a[2];
					debit = a[3];
					credit = a[4];
					who = a[5];
					who = who + ", " + a[6];
					break;
				default:
					System.out.printf("\nlen = %d\n", len);
					System.exit(1);
					break;
			}
			
			System.out.printf("\n%d   ", a.length);
			
			for(String token : a) {
				System.out.printf("%-20s   ", token);
			}
			// insert into mysql databse
			cmd = "insert into checks set amount=";
			bp++;
		}
		
		br.close();
		
		try {
			if (conn != null)  {
				conn.close();
				conn = null;
			}
		}
		catch (SQLException e) {
			e.printStackTrace();
		}
		
		return;
	}								// test()
	
}												// class client

