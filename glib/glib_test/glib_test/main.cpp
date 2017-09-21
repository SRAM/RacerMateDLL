
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <termios.h>

#include <vector>
#include <string>

#include "../../glib_defines.h"
#include "../../smav.h"
#include "../../serial.h"

void test_smav(void);
void test_serial(void);

#define TEST

/***************************************************************************************************************
 
***************************************************************************************************************/

int main(int argc, const char * argv[])  {
	std::vector<int> ivec;
	std::vector<std::string> strs;
	
	strs.push_back("hello 1");
	strs.push_back("world 2");
	
	int i;
	
	ivec.push_back(22);
	ivec.push_back(34);
	
	for(i=0; i<(int)ivec.size(); i++)	{
		printf("%d\n", ivec[i]);
	}
	
	for(i=0; i<(int)strs.size(); i++)	{
		printf("%s\n", strs[i].c_str());
	}

	
#ifdef TEST
	#ifdef _DEBUG
		printf("\n_DEBUG and TEST are on\n");
	#else
		printf("\nTEST is on, _DEBUG is off\n");
	#endif
#else
	#ifdef _DEBUG
		printf("\nTEST is off, _DEBUG is on\n");
	#else
		printf("\nTEST and _DEBUG are off\n");
	#endif
#endif
	
	//test_smav();
	test_serial();
	
	printf("\ndone\n\n");
	return 0;
}

/***************************************************************************************************************
 
***************************************************************************************************************/

void test_smav(void)  {
	float f, avg;
	int i;
	int ir;
	
	srand((unsigned int)time(NULL));
	
	sMav *smav = new sMav(5);
	
	for(i=0; i<100; i++)  {
		ir = rand();																	// 0 to RAND_MAX
		f = ir % 100;																	// 0 to 99
		
		avg = smav->compute(f);
		printf("%3d  %5.2f   %5.2f\n", i, f, avg);
	}
	smav->compute(f);
	DEL(smav);
	
	return;
}												// test_smav()

/***************************************************************************************************************
 
 ***************************************************************************************************************/

void test_serial(void)  {
	Serial *port = NULL;
	long status;
	//long n;
	char device[32];
	char buf[32];
	long len;
	int count = 0;
	
	//strcpy(device, "/dev/ttys000");								// terminal
	strcpy(device, "/dev/tty.usbserial-FTE06HEY");				// usb-serial adapter
	
	strcpy(buf, "test\r\n");
	len = strlen(buf);
	
	
	port = new Serial((char *)"hbar", device, B2400, N8R);
   
	if (!port->isopen())  {
		printf("%s did not open\n\n", device);
		return;
	}
	
	while(1)  {
		/*
		 n = port->rx();
		 if (n>0)  {
		 //printf("n = %d\n", n);
		 }
		 */
			
		status = port->tx((unsigned char *)buf, len);
		if (status != 0)  {
			printf("tx status = %ld\n", status);
		}
		else  {
			printf("%d  %s", ++count, buf);
		}
		usleep(500000);
	}													// while(1)
	return;
}														// test_serial()





/******************************************************************************************************
 read serial input, look for packed data or RacerMate
 returns 1 if new control message is in pkt[],
 2 if 'RacerMate' string matched,
 0 if no new chars or not finished pkt or racermate match.
 
 ******************************************************************************************************/

/*
int read(void)   {
	int i, n;
	
	
	n = read();
	
	if (n < 0) {
		return n;
	}
	
	if (n) {
		for (i=0; i<n; i++)  {
		}                                       // for(i...
	}				//if(n)
	DEL(port);
	return;
}													// test_serial()

*/
