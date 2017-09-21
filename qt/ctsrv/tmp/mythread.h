#ifndef _MYTHREAD_H_
#define _MYTHREAD_H_

#include <QThread>
#include <QTimer>

class myThread : public QThread {
	Q_OBJECT

	public:
		myThread(QObject *_parent = 0);
		void run();
		//void setPostData(const QString& post_data);

	//QString _post_data;
	//QNetworkAccessManager _net_acc_mgr;
	private:
		int bp;
		QTimer *timer;

	signals:
		//void onFinished(QNetworkReply* net_reply);

	private slots:
		void onStarted();
		void timer_slot();

		//void onReplyFinished(QNetworkReply* net_reply);

};

#endif


