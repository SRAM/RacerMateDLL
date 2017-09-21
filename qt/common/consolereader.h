#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H

#include <QThread>
#include <QFile>
#include <QTextStream>

class ConsoleReader : public QThread  {
	Q_OBJECT

	signals:
		void KeyPressed(char ch);

	public:
		ConsoleReader();
		~ConsoleReader();
		void run();

	private:
		void init(int echo);
		void restore(void);
		int bp;
		QString s;
		QFile file;
		QTextStream *qtin=NULL;


#ifndef WIN32
		char getch(void);
#endif

};

#endif

