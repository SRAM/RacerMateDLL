
/************************************************************************************************
	http://treyweaver.blogspot.com/2013/02/qt-console-application-template-tutorial.html
************************************************************************************************/

#include <QCoreApplication>
#include <QTimer>

#include <ant.h>

int main(int argc, char *argv[])  {
	 QCoreApplication app(argc, argv);

	 int debug_level = 2;

	 /*
     if (argc != 4)  {
         printf("???\n");
     }
     QString ssid = argv[1];
     QString pass = argv[2];
     QString s = argv[3];
     bool b = false;
     if (s=="true")  {
         b = true;
     }
	  //Radio myMain(debug_level, ssid, pass, b);                  // true = use admin/admin to get into web pages
	  */


	  ANT ant(debug_level);
                                                                // false = use ctnet/ctnet to get into web pages

	 QObject::connect(&ant, SIGNAL(finished()), &app, SLOT(quit()));
	 QObject::connect(&app, SIGNAL(aboutToQuit()), &ant, SLOT(aboutToQuitApp()));

	 // This code will start the messaging engine in QT and in
	 // 10ms it will start the execution in the MainClass.run routine;

    //QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

	 QTimer::singleShot(10, &ant, SLOT(run()));

	 return app.exec();
}


