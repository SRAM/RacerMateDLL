
#include <QString>

/**********************************************************************************************************

**********************************************************************************************************/

QString rstrip(const QString& str) {
	int n = str.size() - 1;

	for (; n >= 0; --n) {
		if (!str.at(n).isSpace()) {
			//if (!str.at(n)==0x0a) {
			return str.left(n + 1);
		}
	}
	return "";
}



