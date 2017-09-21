import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.*;
import java.net.*;

//import com.sun.glass.events.KeyEvent;

/************************************************************************************

************************************************************************************/

public class client implements KeyListener {
	private int bp = 0;
	boolean flag = false;
	String host = "127.0.0.1";
	int port = 9072;
	long lastnow = 0;

	/************************************************************************************

	************************************************************************************/

	client()  {
		bp = 1;
	}

	/************************************************************************************

	************************************************************************************/

	public static void main(String[] args) throws IOException {

		client clnt = new client();
		
		clnt.run();
		
	}											// main

	/************************************************************************************

	************************************************************************************/

	int run()  {
		flag = true;
		host = "127.0.0.1";
		port = 9072;
	
		while(flag)  {

			//-------------------------------------------------
			// try to connect to server:
			//-------------------------------------------------
			
			char c;
			
			while(true)  {
				//int key= KeyEvent.getKeyCode();
				
				//if (kbhit())  {
				//	c = getc();
				//	if (c=="\x1b")  {
				//		flag = false;
				//		break;
				//	}
				//	if (c!=0)  {
				//		//printf("%c", c);
				//	}
				//}

				/*
				printf(".");
				$result = @socket_connect($socket, $address, $port);
				if ($result === false) {
					//echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
					$s = socket_strerror(socket_last_error($socket));
					if (strpos($s, "actively refused it"))  {
						continue;
					}
					else  {
						printf("%s\n", $s);
					}
				}
				else  {
					printf("\nconnected\n");
					break;
				}
				usleep(100000);
				*/
				
				try (
					Socket kkSocket = new Socket(host, port);
					PrintWriter out = new PrintWriter(kkSocket.getOutputStream(), true);
					BufferedReader in = new BufferedReader(new InputStreamReader(kkSocket.getInputStream()));
					 )  {
					bp = 7;
					break;
					
					/*
					BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
					String fromServer;
					String msg = "RacerMate";
			
					while ((fromServer = in.readLine()) != null) {
						System.out.println("Server: " + fromServer);
			
						if (fromServer.equals("Bye."))  {
							break;
						}
						System.out.printf("sending RacerMate\n");
						out.println(msg);
					}
					*/
					
				}
			  	catch (UnknownHostException e) {
					System.err.println("Don't know about host " + host);
					System.exit(1);
				}
			  	catch (IOException e) {
					//System.err.println("Couldn't get I/O for the connection to " + host);
			  		//System.err.println(e.getMessage());
					//System.exit(1);
			  		//System.out.printf("can't connect to server\n");
			  		bp = 3;
				}
				
				try {
					Thread.sleep(100);
				}
				catch (InterruptedException e) {
					e.printStackTrace();
				}				// sleep 100 ms
				
				/*
				try {
					int i = System.in.available();
					if (i!=0)  {
						bp = 8;
						System.out.printf("i = %d\n", i);
					}
				}
				catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				*/
				
				
				
				/*
				try {
					int ix = System.in.read();
					System.out.printf("ix = %d\n", ix);
				}
				catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				*/
				
				
				String line;
			    OutputStream stdin = null;
			    InputStream stderr = null;
			    InputStream stdout = null;

			    /*
			    // launch EXE and grab stdin/stdout and stderr
			    Process process = null;
				try {
					process = Runtime.getRuntime().exec("kbhit.exe");
				}
				catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			    stdin = process.getOutputStream ();
			    stderr = process.getErrorStream ();
			    stdout = process.getInputStream ();
			    bp = 5;
			    */
			    
			    /*
			      // "write" the parms into stdin
			      line = "param1" + "\n";
			      stdin.write(line.getBytes() );
			      stdin.flush();

			      line = "param2" + "\n";
			      stdin.write(line.getBytes() );
			      stdin.flush();

			      line = "param3" + "\n";
			      stdin.write(line.getBytes() );
			      stdin.flush();

			      stdin.close();				
				*/
			    
			    
				long now = System.currentTimeMillis();
				long now2 = System.nanoTime();
				
				if ( (now - lastnow) >= 1000)  {
					lastnow = now;
					System.out.printf("tick\n");
				}
			}									// while(1)		trying to connect to server
			
		}										// while(flag)
	
		return 0;
	}											// run()

	/************************************************************************************

	************************************************************************************/

	@Override
	public void keyPressed(KeyEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	/************************************************************************************

	************************************************************************************/

	@Override
	public void keyReleased(KeyEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	/************************************************************************************

	************************************************************************************/

	@Override
	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		bp = 1;
		int id = e.getID();
		System.out.printf("id = %d\n", id);
	}
}												// class



