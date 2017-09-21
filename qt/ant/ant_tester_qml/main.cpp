#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDebug>
#include <QScreen>
#include <QQmlContext>
#include <qqml.h>

#include "SleepDetector.h"

#include "ant.h"

int main(int argc, char *argv[]) {

	QGuiApplication app(argc, argv);

//	qDebug() << "App path = " << qApp->applicationDirPath();							// /mnt/minty/home/larry/data/_fs/qt/ant/ant_tester/Desktop_Qt_5_6_1_GCC_64bit-Debug

//	QString pwd("");
//	char * PWD;
//	PWD = getenv ("PWD");
//	pwd.append(PWD);
//	//cout << "Working directory : " << pwd << flush;
//	qDebug() << "pwd = " << pwd;																// /mnt/minty/home/larry/data/_fs/qt/ant/ant_tester



//	QLibrary library("librmant.so.1");

//	if (!library.load()) {
//		qDebug() << library.errorString();
//	}

//	if (library.load()) {
//		qDebug() << "library loaded";
//	}

	int debug_level = 2;
	QQmlApplicationEngine engine;          // need to create after app is created
	QQmlContext* rootContext = engine.rootContext();

	// expose some stuff to qml:

	SleepDetector sleepDetector;
	ANT ant(debug_level);

	//QScopedPointer<SleepDetector> sleepDetector(new sleepDetector());

	rootContext->setContextProperty("sleepDetector", &sleepDetector);


	rootContext->setContextProperty("ant", &ant);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));            // creates a myClass  object


	return app.exec();

}

