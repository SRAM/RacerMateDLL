import java.io.IOException;


/************************************************************************************

************************************************************************************/

public class Clients   {
	
	private int bp = 0;
	private Client c1 = null;
	private Client c2 = null;


	/************************************************************************************

	************************************************************************************/

	Clients()  {
		
		super();
		
	}											// constructor()

	/************************************************************************************

	************************************************************************************/

	public static void main(String[] args) throws IOException  {
		Clients clients = new Clients();
		
		clients.c1 = new Client("client 1");
		clients.c2 = new Client("client 2");
		
		clients.bp = 1;	
	}											// main

}												// class

