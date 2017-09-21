
#ifndef _FILE_ROTATATOR_H
#define _FILE_ROTATATOR_H

class fileRotator {
	public:
		fileRotator(char *_basename, int _n);
		virtual ~fileRotator();
		void get_name(int _k, char *_fname, int _len);

	private:
		int n;						// number of copies to keep
		char stub[256];
		char extension[32];

};

#endif
