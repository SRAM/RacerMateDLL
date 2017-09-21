
#include <QDebug>

#include "mythread.h"

/****************************************************************************

****************************************************************************/


myThread::myThread(QObject *_parent) : QThread(_parent)  {

	qDebug()<<"myThread:: thread id = " << QThread::currentThreadId();
	connect(this, SIGNAL(started()), this, SLOT(onStarted()), Qt::DirectConnection);

	start();
	bp = 0;

	/*
	connect(
		&_net_acc_mgr,	SIGNAL(finished(QNetworkReply*)),
		this,				SLOT(onReplyFinished(QNetworkReply*))
	);
	*/
}

/****************************************************************************

****************************************************************************/

//void myThread::onReplyFinished(QNetworkReply* net_reply)  {
//	emit onFinished(net_reply);
//}

/****************************************************************************

****************************************************************************/

//void myThread::setPostData(const QString& post_data)  {
//	_post_data = post_data;
//}

/****************************************************************************

****************************************************************************/

void myThread::run()  {
	bp = 1;
	//_net_acc_mgr.post(QNetworkRequest(QUrl("http://[omitted]")), QByteArray(_post_data.toStdString().c_str()));
}

/****************************************************************************

****************************************************************************/

void myThread::onStarted()  {
	qDebug()<<"onStarted thread id = " << QThread::currentThreadId();

	timer = new QTimer(this);

	connect(timer, SIGNAL(timeout()), this, SLOT(timer_slot()));
	timer->start(10);

}												// onStarted()

/*********************************************************
	10 ms timer
*********************************************************/

void myThread::timer_slot() {
	bp = 3;
	return;
}
