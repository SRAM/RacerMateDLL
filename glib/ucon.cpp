
#ifdef _DEBUG
	#if defined(WIN32)
		#include <io.h>
	#endif
	//#include <ios.h>
#endif

#include <memory.h>

#include <ucon.h>

/*********************************************************************

*********************************************************************/


Console::Console(int _rows, int _cols, int _bgcolor)  {

	rows = _rows;
	cols = _cols;
	maxx = cols - 1;
	maxy = rows - 1;

	bgcolor = _bgcolor;

	initialized = false;
//	memset(&csbi, 0, sizeof(csbi));
	memset(&cbuf, 0, sizeof(cbuf));
	attr = 0x07;

/*
	hstdin = GetStdHandle(STD_INPUT_HANDLE); 
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE); 
	if (hstdin == INVALID_HANDLE_VALUE || hstdout == INVALID_HANDLE_VALUE) {
		return;
	}
*/

	// Save the current text colors. 

	/*
	if (! GetConsoleScreenBufferInfo(hstdout, &csbi))  {
		return;
	}
	*/

	SMALL_RECT srWindowRect;     // hold the new console size
	/*
	COORD coordScreen;                   // co-ordinates of new window
	coordScreen = GetLargestConsoleWindowSize(hstdout);

	// define the new console window size
	// it will be the lesser of 100 * 40 or the largest possible buffer
	srWindowRect.Right = (SHORT) (min(100, coordScreen.X));
	srWindowRect.Bottom = (SHORT) (min(40, coordScreen.Y));
	*/


	srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
	srWindowRect.Right = cols-1;
	srWindowRect.Bottom = rows-1;

	// keep bigger buffer than window size so that scroll bars appear
	//coordScreen.X = 2*cols;
	//coordScreen.Y = 2*rows;
//	coordScreen.X = cols;
//	coordScreen.Y = rows;

//	SetConsoleScreenBufferSize(hstdout, coordScreen);
//	SetConsoleWindowInfo(hstdout, TRUE, &srWindowRect);

	initialized = true;

	return;

}

/*********************************************************************

*********************************************************************/


Console::~Console()  {
	textattr(7);
	showcursor();
	clrscr();

}


/**************************************************************************

**************************************************************************/

void Console::clrscr(void)  {

	/*
	COORD coord;
	DWORD count;

	coord.X = 0;
	coord.Y = 0;
	*/

	/*
	FillConsoleOutputCharacter(hstdout, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
	//FillConsoleOutputAttribute(hstdout, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
	FillConsoleOutputAttribute(hstdout, (WORD)attr, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
	SetConsoleCursorPosition(hstdout, coord);
	*/

	return;
}

/**************************************************************************

**************************************************************************/

void Console::hidecursor(void)  {
	/*
	if(GetConsoleCursorInfo(hstdout, &cciCursor)) {
		cciCursor.bVisible = FALSE;
		SetConsoleCursorInfo(hstdout, &cciCursor);
	}
	*/
	return;
}

/**************************************************************************

**************************************************************************/

void Console::showcursor(void)  {
	/*
	if(GetConsoleCursorInfo(hstdout, &cciCursor))  {
		cciCursor.bVisible=TRUE;
		SetConsoleCursorInfo(hstdout, &cciCursor);
	}
	*/
	return;
}

/**************************************************************************

**************************************************************************/

void Console::print(const char *_str, int len)  {

	/*
	DWORD count;
	if (len==-1)  {
		len = strlen(_str);
	}

	WriteConsole(hstdout, _str, len, &count, NULL);
	*/

	return;
}

/**************************************************************************

**************************************************************************/

/*
void Console::printxy(int x, int y, const char *CharBuffer, int len)  {
	DWORD count;
	COORD coord = {x, y}; 

	SetConsoleCursorPosition(hstdout, coord); 
	WriteConsole(hstdout, CharBuffer, len, &count, NULL);
	return;
}
*/

/**************************************************************************

**************************************************************************/

void Console::printxy(int x, int y, const char *buf, int len)  {

	/*
	DWORD count;
	COORD coord = {x, y}; 

	SetConsoleCursorPosition(hstdout, coord);

#ifdef _DEBUG
	//GetConsoleScreenBufferInfo(hstdout, &csbi);
#endif

	if (len==-1)  {
		len = strlen(buf);
	}
	WriteConsole(hstdout, buf, len, &count, NULL);
	*/
	return;
}


/**************************************************************************

**************************************************************************/

void Console::putxy(const int x, const int y, const char c)  {

	/*
	COORD coord = {x, y};
	DWORD count;
	cbuf[0] = c;

	SetConsoleCursorPosition(hstdout, coord); 
	WriteConsole(hstdout, cbuf, 1, &count, NULL);
	*/

	return;
}


/**************************************************************************
0 = Black
1 = Blue
2 = Green
3 = Aqua
4 = Red
5 = Purple
6 = Yellow
7 = White
8 = Gray
9 = Light Blue
A = Light Green
B = Light Aqua
C = Light Red
D = Light Purple
E = Light Yellow
F = Bright White

**************************************************************************/

WORD Console::textattr(WORD _attr)  {
	//WORD w;

	attr = _attr;

	/*
	if(GetConsoleScreenBufferInfo(hstdout, &csbi))  {		// we use csbi for the wAttributes word
		w = attr;
		SetConsoleTextAttribute(hstdout, w);     
	}

	return csbi.wAttributes;
	*/
	return 0;
}


/**************************************************************************

**************************************************************************/

void Console::gotoxy(int x, int y)  {
	/*
	COORD coord = {x, y};
	SetConsoleCursorPosition(hstdout, coord);
	*/
	return;
}

/**************************************************************************

**************************************************************************/

int Console::scroll_absolute(int iRows)  {
	//CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	//SMALL_RECT srctWindow; 
 
	// Get the current screen buffer size and window position. 
/* 
	if (! GetConsoleScreenBufferInfo(hstdout, &csbi))  {
		//printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError()); 
		return 0;
	}
 
	// Set srctWindow to the current window size and location. 
 
	win = csbi.srWindow; 
 */

	// Check whether the window is too close to the screen buffer top
/* 
	if ( win.Top >= iRows )  { 
		win.Top -= (SHORT)iRows;     // move top up
		win.Bottom -= (SHORT)iRows;  // move bottom up

		if (! SetConsoleWindowInfo( 
			hstdout,          // screen buffer handle 
			TRUE,             // absolute coordinates 
			&win))     // specifies new location 
			{
			//printf("SetConsoleWindowInfo (%d)\n", GetLastError()); 
			return 0;
		}
		return iRows;
	}
	else  {
		//printf("\nCannot scroll; the window is too close to the top.\n");
		return 0;
	}
	*/

	return 0;
}												// scroll_absolute()


/**************************************************************************
	This will clear the console while setting the forground and background
	colors.
	colors are 0-15
**************************************************************************/

void Console::ClearConsoleToColors(int ForgC, int BackC)  {
	/*
	DWORD count;
	COORD coord = {0, 0};
	WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
	*/

	/*
	SetConsoleTextAttribute(hstdout, wColor);
	FillConsoleOutputCharacter(hstdout, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
	FillConsoleOutputAttribute(hstdout, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
	SetConsoleCursorPosition(hstdout, coord);
	*/
	return;
}


/**************************************************************************
	colors are 0-15

0 = Black	8 = Gray
1 = Blue		9 = Light Blue
2 = Green	A = Light Green
3 = Aqua		B = Light Aqua
4 = Red		C = Light Red
5 = Purple	D = Light Purple
6 = Yellow	E = Light Yellow
7 = White	F = Bright White

**************************************************************************/

void Console::set_text_color(int _color)  {
	//WORD wColor;
/*
	wColor = (csbi.wAttributes & 0xf0) + (_color & 0x0f);

	SetConsoleTextAttribute(hstdout, wColor);
*/

	return;
}

/**************************************************************************
	This will set the forground and background color for printing in a
	console window.
	colors are 0-15
**************************************************************************/

void Console::SetColorAndBackground(int ForgC, int BackC)  {
	/*
	WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);; 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
	*/
	return;
}

/**************************************************************************

**************************************************************************/

bool Console::is_cursor_on(void)  {

	/*
	if(GetConsoleCursorInfo(hstdout, &cciCursor))  {
		if (cciCursor.bVisible)  {
			return true;
		}
	}
	*/

	return false;
}


/**************************************************************************

**************************************************************************/

int Console::scroll_relative(int iRows)  {

	/*
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	//SMALL_RECT srctWindow; 

	// Get the current screen buffer window position. 
 
	if (! GetConsoleScreenBufferInfo(hstdout, &csbiInfo))  {
		//printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError()); 
		return 0;
	}
 
	// Check whether the window is too close to the screen buffer top
 
	if (csbiInfo.srWindow.Top >= iRows)  { 
		win.Top =- (SHORT)iRows;     // move top up
		win.Bottom =- (SHORT)iRows;  // move bottom up 
		win.Left = 0;         // no change 
		win.Right = 0;        // no change 
 
	if (! SetConsoleWindowInfo( 
		hstdout,          // screen buffer handle 
		FALSE,            // relative coordinates
		&win))     // specifies new location 
		{
			//printf("SetConsoleWindowInfo (%d)\n", GetLastError()); 
			return 0;
		}
		return iRows;
	}
	else  {
		//printf("\nCannot scroll; the window is too close to the top.\n");
		return 0;
	}
	*/

	return 0;
}										// scroll_relative

/**************************************************************************

**************************************************************************/

void Console::scroll_area(SMALL_RECT _r, int count)  {

#if 1

#else
	if (_r.Top > rows-1)  {
		return;
	}

	B = GetConsoleScreenBufferInfo(hstdout, &csbi);
	if (!B)  {
		return;
	}

	dest.X = _r.Left;
	dest.Y = _r.Top;

	_r.Top += 1;

	GetConsoleScreenBufferInfo(hstdout, &csbi);
 	clip_rect = scroll_rect;		// The clipping rectangle is the same as the scrolling rectangle. The destination row is left unchanged. 
 
	fill.Attributes = csbi.wAttributes;
	fill.Char.AsciiChar = (char)' '; 

	B = ScrollConsoleScreenBuffer(  
        hstdout,				// screen buffer handle 
        &_r,					// the console screen buffer rectangle to be moved
        //&clip_rect,		// the console screen buffer rectangle that is affected by the scrolling
		  NULL,
        dest,					// top left destination cell
									// upper-left corner of the new location of the lpScrollRectangle contents, in characters
        &fill);				// fill character and color

	if (!B)  {
        return;
	}
#endif

	return;
}										// scroll_area()

/**************************************************************************

**************************************************************************/

void Console::clear_win(SMALL_RECT _r, WORD _attr)  {

#if 1
	return;
#else
	COORD coord;
	DWORD count;
	int i, size;

	if (_r.Left == 0 && _r.Right == cols-1)  {
		coord.X = _r.Left;
		coord.Y = _r.Top;

		size = (_r.Bottom - _r.Top + 1) * (_r.Right - _r.Left + 1);

		FillConsoleOutputCharacter(hstdout, ' ', size, coord, &count);
		//FillConsoleOutputAttribute(hstdout, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
		FillConsoleOutputAttribute(hstdout, (WORD)_attr, size, coord, &count );
		SetConsoleCursorPosition(hstdout, coord);
	}
	else  {
		coord.X = _r.Left;
		size = _r.Right - _r.Left;

		for(i=_r.Top; i<=_r.Bottom; i++)  {
			coord.Y = i;
			FillConsoleOutputCharacter(hstdout, ' ', size, coord, &count);
			//FillConsoleOutputAttribute(hstdout, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count );
			FillConsoleOutputAttribute(hstdout, (WORD)_attr, size, coord, &count );
			SetConsoleCursorPosition(hstdout, coord);
		}
	}
#endif

	return;
}
