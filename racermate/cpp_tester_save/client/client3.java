import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.*;
import java.net.*;

//import com.sun.glass.events.KeyEvent;

/************************************************************************************

************************************************************************************/

public class client  {
	private int bp = 0;
	boolean flag = false;
	String host = "127.0.0.1";
	int port = 9072;
	long lastnow = 0;
	PrintWriter out = null;
	BufferedReader in = null;
	Socket socket = null;
	boolean connected = false;
	String data;
	
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

	/**
	 * @throws IOException **********************************************************************************

	************************************************************************************/

	int run() throws IOException  {
	    char[] c = new char[] { 1024 };
		String s;
	    int n=-1;
		flag = true;
		host = "127.0.0.1";
		port = 9072;
		String decoded = null;		
	
		while(flag)  {

			//-------------------------------------------------
			// try to connect to server:
			//-------------------------------------------------

			DataInputStream inStream = null;
			
			System.out.printf("trying to connect to server\n");
			
			while(true)  {
				
				try {
					socket = new Socket(host, port);
					//out = new PrintWriter(kkSocket.getOutputStream(), true);
					//in = new BufferedReader(new InputStreamReader(kkSocket.getInputStream()));
					inStream = new DataInputStream(socket.getInputStream( ));
					//inStream = new DataInputStream(new BufferedInputStream(socket.getInputStream( )));					
					//inStream.read();					
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
				
				try {
					Thread.sleep(75);
				}
				catch (InterruptedException e) {
					e.printStackTrace();
				}				// sleep 100 ms
				
				long now = System.currentTimeMillis();
				long now2 = System.nanoTime();
				if ( (now - lastnow) >= 1000)  {
					lastnow = now;
					System.out.printf(".");
				}
			}									// while(1)		trying to connect to server
			
			System.out.printf("\nconnected to server\n");

			bp = 2;

			//-------------------------------------
			// now try to receive 'RacerMate'
			//-------------------------------------
			
			// wait for response. This is a blocking example.
			byte[] bytes = new byte[9];
			int bytesRead = 0;
			
			while(true)  {
				n = inStream.available();
				
				if (n > 0)  {
					try {
					    bytesRead = inStream.read(bytes, 0, 9);								// blocks
					}
					catch (IOException e1) {
					    e1.printStackTrace();
					}
				 
					if (bytesRead != 9) {
						// communication error. Abort.
						bp = 9;
					}
					
					try {
						decoded = new String(bytes, "UTF-8");
					}
					catch (UnsupportedEncodingException e) {
						e.printStackTrace();
					}
				
					if (decoded.equals("RacerMate"))  {
						connected = true;
						break;
					}
				}
				else  {
					try {
						Thread.sleep(100);
					}
					catch (InterruptedException e) {
						e.printStackTrace();
					}				// sleep 100 ms
				}
			
				bp = 0;
			}												// while waiting for "RacerMate"
			
			bp = 3;
			
			//---------------------------------------------------------------
			// keep reading from server as long as we're connected
			//---------------------------------------------------------------

			/*
			int cnt = 0;
			
			while(true)  {
				cnt++;
				
				try {
					n = inStream.read();
				}
				catch (IOException e1) {
					e1.printStackTrace();
				}					

				if (n > 0)  {
					s = inStream.toString();
					System.out.printf("n = %d, cnt = %d\n", n, cnt);
					System.out.print(s);
					bp = 2;
				}

				try {
					Thread.sleep(100);
				}
				catch (InterruptedException e) {
					e.printStackTrace();
				}				// sleep 100 ms
				*/
			
				//usleep(100000);
				
				/*
				try {
					data = null;
					int cnt = 0;
					while (in.ready()) {
					    n = in.read(c);
					    if (n != 1)  {
					    	bp = 4;
					    }
					    cnt += n;
					    //sb.append(c);
					    s = c.toString();
					    data.concat(s);
						//data = sb.toString();
						//if (data.equals("RacerMate"))  {
						//	bp = 2;
						//}
					}
					
					bp = 1;
				}
				catch (IOException e) {
					e.printStackTrace();
				}				
				*/
				
				//bp = 1;
				
				/*
				//data = socket_read($socket, 2048, $type);
				if (data===false)  {
					s = socket_strerror(socket_last_error());
					if (strpos($s, "forcibly closed by the remote host"))  {
						printf("server disconnected\n");
					}
					else  {
						printf("\n%s\n", $s);
					}
					connected = false;
					break;
				}
				else if (data=="")  {
				}
				else if (data=="RacerMate")  {
					printf("rx: %s\n", $data);
					$n = @socket_write($socket, "LinkUp", 6);
					if ($n != 6)  {
						printf("\n%s\n", socket_strerror(socket_last_error()));
					}
					printf("tx: %s\n", "LinkUp");
					$connected = true;								// connected to the server app
					printf("connected to server application\n");
				}
				else  {
					printf("------------------> %s\n", $data);
				}

				if ($connected)  {
					//updateHardware($socket);
					usleep(200000);								// sleep 200 ms
				}
				else  {
					usleep(100000);
				}
				*/
				
			//}												// while(1)  {


			
		}										// while(flag)

		
		return 0;
	}											// run()

}												// class



