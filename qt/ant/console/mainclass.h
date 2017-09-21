#ifndef MAINCLASS_H
#define MAINCLASS_H
#include <QCoreApplication>
#include <QTimer>
#include <QFile>
#include <QSettings>

#include <smav.h>
#include <tmr.h>
#include <sdirs.h>

#include <ant.h>


class MainClass : public QObject  {
	 Q_OBJECT
private:
	bool have_sticks = false;
	sMav smav = sMav(11);
	const char *sticks = NULL;

	char logname[256] = {0};
	void lock_logging(void);
	void unlock_logging(void);
	QMutex logmutex;
	QElapsedTimer et;
	qint64 maxlogwait = -1;

	 QCoreApplication *app;
	 RACERMATE::ANT *ant;
	 //RACERMATE::SDIRS *sdirs;
	 qint64 start_time = 0;
	 int bp = 0;
	 void logg(const char *format, ...);
	 char logstr[1024] = {0};
		FILE *logstream = NULL;
	 QTimer *timer = NULL;
	 //QString program_dir;
	 //QString personal_dir;
	 //QString settings_dir;
	 //QString debug_dir;

	 int debug_level;
	 QFile file;
	 QSettings *settings;
	RACERMATE::Tmr *tmr;
	qint64 start=0, now=0;
	QObject *parent;
	 void init();
	 int scan_ticks=-1;
	 int display_ticks = -1;


public:
	 explicit MainClass(int _debug_level, QObject *_parent = 0);
	 void quit();					// Call this to quit application

signals:
	 void finished();				// Signal to finish, this is connected to Application Quit

public slots:
	 void run();					// This is the slot that gets called from main to start everything but, everthing is set up in the Constructor
	 void aboutToQuitApp();		// slot that get signal when that application is about to quit

private slots:
	void OnConsoleKeyPressed(char c);
	void timeout_slot();
};

#endif // MAINCLASS_H


