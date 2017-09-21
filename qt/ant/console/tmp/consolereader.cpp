#include "consolereader.h"
#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#include <termios.h>
static struct termios oldSettings;
static struct termios newSettings;
#endif


//-------------------------------------------
//
//-------------------------------------------

ConsoleReader::ConsoleReader()  {
	init(0);
}

//-------------------------------------------
//
//-------------------------------------------

ConsoleReader::~ConsoleReader()  {
	restore();
}

//-------------------------------------------
//
//-------------------------------------------

void ConsoleReader::init(int echo)  {


#ifdef WIN32
    Q_UNUSED(echo);
#else
    tcgetattr(0, &oldSettings);							// grab old terminal i/o settings
    newSettings = oldSettings;							// make new settings same as old settings
	newSettings.c_lflag &= ~ICANON;						// disable buffered i/o
    newSettings.c_lflag &= echo ? ECHO : ~ECHO;         // set echo mode
	tcsetattr(0, TCSANOW, &newSettings);				// use these new terminal i/o settings now
#endif

}

// restore old terminal i/o settings
void ConsoleReader::restore(void)  {
#ifndef WIN32
    tcsetattr(0, TCSANOW, &oldSettings);
#endif
}

//-------------------------------------------
// read 1 character without echo
//-------------------------------------------

char ConsoleReader::getch(void)  {
	return getchar();
}

//-------------------------------------------
//
//-------------------------------------------

void ConsoleReader::run()  {

	forever  {
		char key = getch();
		emit KeyPressed(key);
	}

}
