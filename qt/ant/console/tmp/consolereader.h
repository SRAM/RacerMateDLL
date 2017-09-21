#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H

#include <QThread>

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
		char getch(void);

};

#endif
