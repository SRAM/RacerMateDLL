
/************************************************************************************************
	http://treyweaver.blogspot.com/2013/02/qt-console-application-template-tutorial.html
************************************************************************************************/

#include <QCoreApplication>
#include <QTimer>

#include <mainclass.h>

int main(int argc, char *argv[])  {
	 QCoreApplication app(argc, argv);

	 int debug_level = 2;

	 MainClass mc(debug_level);

	 QObject::connect(&mc, SIGNAL(finished()), &app, SLOT(quit()));
	 QObject::connect(&app, SIGNAL(aboutToQuit()), &mc, SLOT(aboutToQuitApp()));

	 // This code will start the messaging engine in QT and in
	 // 10ms it will start the execution in the MainClass.run routine;

    //QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

	 QTimer::singleShot(10, &mc, SLOT(run()));

	 return app.exec();
}


