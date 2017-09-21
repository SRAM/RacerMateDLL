
#include "consolereader.h"
#include <stdio.h>

#ifdef WIN32
//	#include <QFile>
//	#include <conio.h>
//#include <QIODevice>
//	#include <QTextStream>
#else
	#include <unistd.h>
	#include <termios.h>
	static struct termios oldSettings;
	static struct termios newSettings;
#endif

#include <QThread>

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

	bp = 0;
	file.open(stdin, QIODevice::ReadOnly);
	//qtin(&file);
	qtin = new QTextStream(&file);

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

//-------------------------------------------
//
//-------------------------------------------

// restore old terminal i/o settings
void ConsoleReader::restore(void)  {
#ifdef WIN32
	file.close();
#else
    tcsetattr(0, TCSANOW, &oldSettings);
#endif
}

//-------------------------------------------
// read 1 character without echo
//-------------------------------------------

#ifndef WIN32
//#if 1
char ConsoleReader::getch(void)  {

#ifdef WIN32
	//char c;
	//QIODevice::getChar(&c);
	//QIODevice *d = new QIODevice(this);
	//d->getChar(&c);
	//QTextStream in(stdin);
	//in.getChar(c);
	//QString s = in.read(1);
	//if (s.isEmpty())  {
	//	return -1;
	//}
	//return c;
#else
	return getchar();
#endif

}
#endif

//-------------------------------------------
//
//-------------------------------------------

void ConsoleReader::run()  {

	//char key;


#ifdef WIN32
//#if 0
	while(1)  {
		/*
		if (kbhit())  {
			char key = getch();
			emit KeyPressed(key);
		}
		*/

		*qtin >> s;

		if (!s.isEmpty())  {
			bp = 1;
		}


		//file.close();


//		if (_kbhit())  {
//			bp = 1;
//		}
		//if (kbhit())  {
			//char key =_getch();
			//key = getchar();
			//key = _getch();
			/*
			QTextStream cin(stdin);

			QChar key;
			cin >> key;
			*/

			/*
			switch(key)  {
				case -1:
					QThread::msleep(100);
					continue;
				default:
					break;
			}
			*/

			//char c = key.toLatin1();
			//emit KeyPressed(c);
		//}

		QThread::msleep(100);
	}

#else
	forever  {
		char key = getch();
		if (key==-1)  {
			QThread::msleep(100);
			continue;
		}
		emit KeyPressed(key);
	}
#endif

}

