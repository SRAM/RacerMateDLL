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
	Socket kkSocket = null;
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

			System.out.printf("trying to connect to server\n");
			
			while(true)  {
				
				try {
					kkSocket = new Socket(host, port);
					out = new PrintWriter(kkSocket.getOutputStream(), true);
					
					/*
					InputStream is = socket.getInputStream();
            		InputStreamReader isr = new InputStreamReader(is);
            		BufferedReader br = new BufferedReader(isr); 
					*/
					in = new BufferedReader(new InputStreamReader(kkSocket.getInputStream()));
					break;
				}
			  	catch (UnknownHostException e) {
					System.err.println("Don't know about host " + host);
					System.exit(1);
				}
			  	catch (IOException e) {
			  		//System.out.printf("%s\n", e.getMessage());
			  		//bp = 3;
				}
				
				try {
					Thread.sleep(100);
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
			
			System.out.printf("connected to server\n");

			//---------------------------------------------------------------
			// keep reading from server as long as we're connected
			//---------------------------------------------------------------

			while(true)  {
				
				/*
				try {
					data = in.readLine();
				}
				catch (IOException e) {
					//e.printStackTrace();
					String s = e.getMessage();
					if (s.equals("xxx"))  {
						break;
					}
				}
				*/
				
				//StringBuffer sb = new StringBuffer();
				
				try {
					while (in.ready()) {
					    char[] c = new char[] { 1024 };
					    int n = in.read(c);
					    //sb.append(c);
					    data = c.toString();
						//data = sb.toString();
						if (data.equals("RacerMate"))  {
							bp = 2;
						}
					}
				}
				catch (IOException e) {
					e.printStackTrace();
				}				
				
/////////////////
				
				/*
				//Case 2: the message has a fixed length

				// let's pretend message is always 80 long
				int messageLength = 7;

				StringBuilder messageBuffer = new StringBuilder();
				int charactersRead = 0;
				int value;
				// reads to the end of the stream or till end of message
				
				try {
					while((value = in.read()) != -1) {
					    char c = (char)value;
					    // end?  jump out
					    if (++charactersRead >= messageLength) {
					        break;
					    }
					    // else, add to buffer
					    messageBuffer.append(c);
					}
				}
				catch (IOException e) {
					e.printStackTrace();
				}
				// message is complete!
				String message = messageBuffer.toString();
				*/
				
/////////////////
				bp = 1;
				
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
				
			}												// while(1)  {


			
		}										// while(flag)

		
		return 0;
	}											// run()

}												// class



