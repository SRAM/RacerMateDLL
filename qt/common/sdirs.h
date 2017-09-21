#ifndef _QT_SDIRS_H_
#define _QT_SDIRS_H_

#include <QString>

#include <comdefs.h>


namespace RACERMATE  {

/*************************************************************

*************************************************************/

class X_EXPORT SDIRS  {

	 public:
		//Tmr();
		SDIRS(int _debug_level, QString _base);
		~SDIRS();
		int get_rc() const { return rc; }
		QString get_personal_dir() const { return personal_dir; }
		QString get_program_dir() const { return program_dir; }
		QString get_debug_dir() const { return debug_dir; }
		QString get_settings_dir() const { return settings_dir; }


	private:
		QString personal_dir;
		QString program_dir;
		QString debug_dir;
		QString settings_dir;
		//int bp = 0;
		//QFile file;
		int rc = 0;
};
}



#endif



