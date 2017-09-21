import java.io.*;
import java.net.*;

/************************************************************************************

************************************************************************************/

public class client {
	private int bp = 0;

	/************************************************************************************

	************************************************************************************/

	client()  {
		bp = 1;
	}

	/************************************************************************************

	************************************************************************************/

	public static void main(String[] args) throws IOException {

		String host = "127.0.0.1";
		int port = 9072;


		try (
			Socket kkSocket = new Socket(host, port);
			PrintWriter out = new PrintWriter(kkSocket.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(kkSocket.getInputStream()));
			 )  {

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
		}
	  	catch (UnknownHostException e) {
			System.err.println("Don't know about host " + host);
			System.exit(1);
		}
	  	catch (IOException e) {
			System.err.println("Couldn't get I/O for the connection to " + host);
			System.exit(1);
		}
	}


}



