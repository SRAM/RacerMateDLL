#include "SleepDetector.h"

#include <QGuiApplication>

SleepDetector::SleepDetector(QObject* parent) : QObject(parent)  {
	connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
			  SLOT(handleApplicationState(Qt::ApplicationState)));
}

void SleepDetector::handleApplicationState(Qt::ApplicationState state)  {
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
	if (state == Qt::ApplicationInactive)  {
		emit applicationFellAsleep();
	}
#else
	int bp = 0;
	Q_UNUSED(bp);

	switch(state)  {
		case Qt::ApplicationSuspended:			// 0x00000000
			bp = 1;
			break;
		case Qt::ApplicationHidden:				// 0x00000001
			bp = 2;
			break;
		case Qt::ApplicationInactive:				// 0x00000002	gets here when other breakpoints are hit or when the main window goes out of focus
			//emit applicationFellAsleep();
			break;
		case Qt::ApplicationActive:				// 0x00000004	gets here when this is first created and when main window comes back in focus
			bp = 4;
			break;
		default:
			bp = 5;
			break;
	}
#endif
}


