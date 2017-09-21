#ifndef SLEEPDETECTOR_H
#define SLEEPDETECTOR_H

#include <QObject>

class SleepDetector : public QObject  {
	Q_OBJECT

	public:
		SleepDetector(QObject* parent = 0);

	signals:
		void applicationFellAsleep();

	private slots:
		void handleApplicationState(Qt::ApplicationState state);
};

#endif		// SLEEPDETECTOR_H
