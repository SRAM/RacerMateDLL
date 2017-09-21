#ifndef _X_HANDLER_H_X_
#define _X_HANDLER_H_X_

#include <QtCore>

namespace RACERMATE  {

	class Handler : public QObject  {
			Q_OBJECT
		public:
			explicit Handler(QObject *parent = 0);
			~Handler();

		private slots:
			//void connected_to_server_slot(bool _b);

		signals:
			//void sig(int);
	};                            // class CLIENT
}											// namespace


#endif                        // #ifndef _X_CLIENT_H_X_


