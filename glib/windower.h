
#ifndef _WINDOWER_H_
#define _WINDOWER_H_

#ifdef WIN32
	#include <windows.h>
#endif

class Windower {
	friend class scrollingChart;

	private:
		void destroy(void);
		int left;
		int right;
		int records;
		int bp;
		RECT r;
		bool zoomed_in;
		int w;
		float m, b, mc, bc;
		long minsamp,maxsamp;
		float delta;
#ifdef WIN32
		HDC hdc;
		HPEN cursorpen;
#endif
		void vline(int x, int k);
		int lastx[2];

	public:
#ifdef WIN32
		Windower(HDC _hdc, RECT _waverect, int _records=0);
#else
		Windower(RECT _waverect, int _records=0);
#endif

		~Windower();
		void enter(void);
		void moveleft(void);
		void moveright(void);
		void movein(void);
		void moveout(void);
		void home(void);
		long getLeft(void);
		long getRight(void);

		void draw(void);

		void setright(long _right)  {
			right = (int)_right;
		}
		void setleft(long _left)  {
			left = (int)_left;
		}
		void setsize(int _records);
};

#endif


