#ifndef _UCON_H_
#define _UCON_H_

typedef struct  {
	int Top;
	int Bottom;
	int Left;
	int Right;
} SMALL_RECT;

#define WORD unsigned short
#define SHORT short
#define BOOL bool

class Console  {
	public:
		Console(int _rows=25, int _cols=80, int _bgcolor=0);
		~Console();
		void clrscr(void);
		void clear_win(SMALL_RECT _r, WORD _attr);

		void hidecursor(void);
		void showcursor(void);
		void print(const char *CharBuffer, int len=-1);
		void printxy(int x, int y, const char *CharBuffer, int len=-1);
		void putxy(const int x, const int y, const char c);
		WORD textattr(WORD attr);
		void gotoxy(int x, int y);
		void scroll_area(SMALL_RECT _r, int count);
		int scroll_absolute(int iRows);
		int scroll_relative(int iRows);
		int getmaxx(void)  { return maxx; }
		int getmaxy(void)  { return maxy; }
		int getw(void)  { return cols; }
		int geth(void)  { return rows; }

		void ClearConsoleToColors(int ForgC, int BackC);
		void set_text_color(int _color);
		void SetColorAndBackground(int ForgC, int BackC);
		bool is_cursor_on(void);

	private:
		char cbuf[8];									// buffer for outputting a single character
		bool initialized;
		//int hstdout;
		//int hstdin;
		//CONSOLE_SCREEN_BUFFER_INFO csbi;
		//CONSOLE_CURSOR_INFO cciCursor;
		int rows;
		int cols;
		int maxx;
		int maxy;
		int bgcolor;
		//SMALL_RECT win;
		//SMALL_RECT ScrollUp;				// = {0, 1, 0, 1};
		WORD attr;
		//BOOL B;
		//SMALL_RECT scroll_rect;
		//SMALL_RECT clip_rect;
		//COORD dest;
		//CHAR_INFO fill;

};

#endif


