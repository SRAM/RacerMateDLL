
#ifdef WIN32
	#pragma warning(disable:4101 4786 4065)
	#include <windows.h>
	#include <stdio.h>
	#include <vector>
	#include <string>
	//#include <gdiplus.h>
	//using namespace Gdiplus;

	#include <glib_defines.h>
	#include <glib_types.h>
	//#include <config.h>
	#include <logger.h>
	#include <serial.h>
	#include <ini.h>
	//#include <ogl.h>

	#ifdef DOC
		extern "C" {
	#endif

	typedef struct TICKSUMMARY {
		int s, m, l, n;
	} ticksummary;

	enum  {
		CSV_STRING,
		CSV_FLOAT,
		CSV_INT
	};

	//#ifdef _DEBUG
		//__declspec(dllexport)
		void checkMsg(UINT msg, FILE *stream);
	//#endif

	std::string file_get_contents(const char *fname);
	unsigned char low(unsigned short us);
	unsigned char high(unsigned short us);
	void SetClientSize(HWND hw, int x, int y);
	void dprintf(const char *format, ...);
	bool valid_filename(const char *fname);
	char get_system_drive(void);
	//int check_memory(void);
	void CopyRect(FRECT *rd, RECT *rs);
	void CopyRect(FRECT *rd, FRECT *rs);

#if 0
	__declspec(dllexport) void beep(void);
	__declspec(dllexport) void bang(void);
	__declspec(dllexport) void buzz(void);
	__declspec(dllexport) void alert(void);
	__declspec(dllexport) int mycopy(const char *existing_name, const char *new_name);
	__declspec(dllexport) void select_color(HWND hwnd, COLORREF &color, Ini *ini=NULL);
	__declspec(dllexport) WCHAR *to_wide(const char *str);
	__declspec(dllexport) void to_narrow(WCHAR *wstr, char *str, int len);
	__declspec(dllexport) void charToW(const char* cPtr, int cSize, wchar_t*wPtr);
	__declspec(dllexport) void convertAnsiStringToWide(WCHAR *wstrDestination, const CHAR *strSource, int cchDestChar);
	__declspec(dllexport) void convertWideStringToAnsi(CHAR *strDestination, const WCHAR *wstrSource, int cchDestChar);
	__declspec(dllexport) void SetClientRect(HWND hwnd, RECT *r);
	__declspec(dllexport) int move_to(FILE *stream, int line);
	__declspec(dllexport) void compute_sine(float *sine, int n, int T);
	__declspec(dllexport) void convtime(DWORD ms, int &h, int &m, int &s, int &ts);
	__declspec(dllexport) int getmonth(const char *str);
	__declspec(dllexport) int backup(FILE *stream, long n);
	__declspec(dllexport) void remove_empty_lines(char *fname);
	__declspec(dllexport) void reverse_file(char *fname);
	__declspec(dllexport) void fix_crlf(char *fname);
	__declspec(dllexport) void play(char *sound);
	__declspec(dllexport) double vmag(double x, double y, double z);
	__declspec(dllexport) double get_angle(double x1, double y1, double z1, double x2, double y2, double z2, bool degrees);
	__declspec(dllexport) void strtolower(char *);
	__declspec(dllexport) void strtoupper(char *);
	__declspec(dllexport) char *basename(const char *path);
	__declspec(dllexport) void dbase_to_csv(char *dbfname);
	__declspec(dllexport) void box(HDC hdc, RECT r, COLORREF=RGB(128, 128, 128), bool fill=false, HPEN pen=0);
	__declspec(dllexport) void hide(char *fname);
	__declspec(dllexport) void unhide(char *fname);
	__declspec(dllexport) void get_os_serial_number(unsigned char *sn, DWORD size);
	__declspec(dllexport) void getUniqueFileName(char *fname, int size);
	__declspec(dllexport) int doof(const char *_fname);
	__declspec(dllexport) int csv_add(char *buf, int buflen, int type, void *p, int _precision, bool _last=false);
	__declspec(dllexport) char *MessageName(UINT msg);
	__declspec(dllexport) bool direxists(const char *path);
	__declspec(dllexport) int DeleteFolder(char *path);
	__declspec(dllexport) double nicenum(double x, int round);
	__declspec(dllexport) ticksummary linaxspec(double span, int maxtix, int maxnums, double* scale);
	__declspec(dllexport) int printfile(char *_filename, int _copies=1, bool _dialog=false, bool _cursorhidden=false, bool _landscape=false, int _linesPerPage=-1);
	__declspec(dllexport) void OutputHeading(const char * explanation);
	__declspec(dllexport) void SetRValue( COLORREF* pColor, BYTE value );
	__declspec(dllexport) void SetGValue( COLORREF* pColor, BYTE value );
	__declspec(dllexport) void SetBValue( COLORREF* pColor, BYTE value );
	__declspec(dllexport) int fcompare( const void *p1, const void *p2 );
	__declspec(dllexport) int irand(int min, int max);
	__declspec(dllexport) void repaintNow(HWND hwnd);
	__declspec(dllexport) void heapdump(void);
	__declspec(dllexport) double dlerp(double t, double x, double y);
	__declspec(dllexport) void buzz(void);
	__declspec(dllexport) LPWORD lpwAlign ( LPWORD lpIn);
	__declspec(dllexport) int nCopyAnsiToWideChar (LPWORD lpWCStr, LPSTR lpAnsiIn);
	__declspec(dllexport) HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);
	__declspec(dllexport) 	int expect(Logger *logg, char *str, Serial *port, DWORD timeout);
	__declspec(dllexport) 	void flush(Logger *logg, Serial *port,DWORD timeout, BOOL echo);
	__declspec(dllexport) 	void GetMachineCaps(Logger *log);
	__declspec(dllexport) int getnline(char *line, int linelength, FILE *stream);
	__declspec(dllexport) void delay(DWORD ms);
	__declspec(dllexport) SYSTEMTIME filetime(char *fname);
	__declspec(dllexport) void circle(HDC hdc,int xc,int yc,int ir, bool filled=true);
	__declspec(dllexport) void ellipse(HDC hdc,int xc,int yc,int irx, int iry, bool filled);
	__declspec(dllexport) void calc_fps(void);
	__declspec(dllexport) void mav(float *x,int n,int m,int flag);
	__declspec(dllexport) void center_window(HWND hwnd);
	__declspec(dllexport) void getClientRect(HWND hwnd, RECT *clientrect);
	__declspec(dllexport) unsigned short getRandomNumber(void);
	__declspec(dllexport) float fround(float number, int digits);
	__declspec(dllexport) double dround(double value, int precision);
	__declspec(dllexport) void time_format(DWORD tms, char *str);
	__declspec(dllexport) void TransparentStretchBlt(
							 				HDC hDC,							// dest
							 				LONG left,						// dest
							 				LONG top,						// dest
							 				LONG width,						// dest
							 				LONG height,					// dest
								
							 				HBITMAP hBitmap,				// source bitmap
							 				LONG bmLeft,					// source
							 				LONG bmTop,						// source
							 				LONG bmWidth,					// source
							 				LONG bmHeight,					// scource
							 				COLORREF colorMask);			// transparent color
	__declspec(dllexport) bool valid_filename(const char *fname);
	__declspec(dllexport) double finterp(double a1, double a, double a2, double b1, double b2);
	__declspec(dllexport) bool is_open(const char *fname);
	__declspec(dllexport) int getbit(unsigned char c, int i);
	__declspec(dllexport) int pv(RECT, float);
	__declspec(dllexport) int ph(RECT, float);
	__declspec(dllexport) bool deq(double f1, double f2, double tolerance=.000001);
	void hot(void);
	__declspec(dllexport) void crand(char *str, int len);
	__declspec(dllexport) bool brand(void);
	__declspec(dllexport) void myexplode(const char *str, const char *sep, std::vector<std::string> &a);
	__declspec(dllexport) int sign(float f);
	__declspec(dllexport) float frand(float min, float max);
	__declspec(dllexport) bool rm(const char *fname);
	__declspec(dllexport) void strip_extension(char *fname);
	__declspec(dllexport) bool has_extension(char *_fname);
	__declspec(dllexport) bool change_extension(char *infname, char *ext1, char *outfname, int len, char *ext2);
	__declspec(dllexport) void get_extension(char *fname);			// returns the extension (without the '.') in fname
	__declspec(dllexport) void trimcrlf(char *string);
	__declspec(dllexport) bool isodd(int i);
	__declspec(dllexport) unsigned long filesize_from_stream(FILE *stream);
	__declspec(dllexport) unsigned long filesize_from_name(const char *filename);
	__declspec(dllexport) int indexIgnoreCase(const char *str1, const char *str2, int startpos=0, bool reverse=false);
	__declspec(dllexport) int myindex(const char *str1, const char *str2, int startpos=0);
	__declspec(dllexport) bool strEqual(char *str1, char *str2);
	__declspec(dllexport) void strip_path(char *fname);
	__declspec(dllexport) void strip_filename(char *fname);
	__declspec(dllexport) int replace(char *string, const char *oldstr, const char *newstr, int flag=0);
	__declspec(dllexport) int indexCharFromEnd(char *str1, char c);
	__declspec(dllexport) void ltrim(char *string, int trim_tabs=1, char *ends=NULL);
	__declspec(dllexport) void trim(char *string, const char *ends=NULL);
	__declspec(dllexport) int gethex(unsigned char *str);
	__declspec(dllexport) void bit_reverse(unsigned char *b);
	__declspec(dllexport) void myremove(char *pat, char *str);
	__declspec(dllexport) unsigned long Checksum(char *filename);
	__declspec(dllexport) unsigned short checksum(unsigned char *, int);
	__declspec(dllexport) void dump2(FILE *dumpstream, unsigned char *mb, int cnt);
	__declspec(dllexport) float bmr(float _lb, float _inches, float _years, char _sex, int _activityIndex);
	__declspec(dllexport) int insert(char *str1, const char *str2, int k);
	__declspec(dllexport) void print_amortization_table(float principal, float interest, int years, int month);
	__declspec(dllexport) float compute_payment(float P, float I, int years);
	__declspec(dllexport) double compute_remaining_principal(double _P, double _I, int _L, int _t);
	__declspec(dllexport) void dump(unsigned char *mb, int cnt, FILE *stream, char *leader=NULL);
	//__declspec(dllexport) void dump(unsigned char *mb, int cnt);
	//__declspec(dllexport) void dump(char *_infname, char *_outfname, unsigned long _addr=0);
	__declspec(dllexport) bool isNumericString(char *_str);
	__declspec(dllexport) int roundmod(float f, int mod);
	__declspec(dllexport) int round_a(int n, int mod);
	__declspec(dllexport) int round_af(float f, int mod);
	__declspec(dllexport) int countbits(long i);
	__declspec(dllexport) void swap(int *a, int *b);
	void swap_floats(float *f1, float *f2);
	__declspec(dllexport) void reverse_long_bytes(unsigned long *ul);
	__declspec(dllexport) long countlines(const char *fname, bool skip_blank = false);
	__declspec(dllexport) long searchFile(char *fname, long _offset, char *pat, int direction);
	__declspec(dllexport) long read_until_char(char *fname, long _offset, char cc, char *buf, int bufsize);
	__declspec(dllexport) bool getFileBuf(char *fname, long offset1, long offset2, char *buf, int buflen);
	__declspec(dllexport) void get_last_line(char *fname, char *str, int size);
	__declspec(dllexport) bool substr(char *_str, char _startchar, char _endchar, bool _inclusive, char *_outstr, int _outbufsize, int *i1, int *i2);
	__declspec(dllexport) bool exists(const char *fname);
	__declspec(dllexport) bool replace_extension(char *fname, const char *new_extension);
	__declspec(dllexport) int count_strings(char **strs);
	__declspec(dllexport) void leastsquare(std::vector<float> &v, float *m, float *b);
	//__declspec(dllexport) void leastsquare(float *x,float *y,short n,float *m,float *b);
	__declspec(dllexport) void leastsquares(float *x,float *y,short n,float *m,float *b);
	__declspec(dllexport) double mean(float *data,short n);
	__declspec(dllexport) double median(float *data,short n);
	__declspec(dllexport) double statmode(float *data,short n);
	__declspec(dllexport) double rms(float *data,short n);
	__declspec(dllexport) double geometric_mean(float *data,short n);
	__declspec(dllexport) double harmonic_mean(float *data,short n);
	__declspec(dllexport) void gaussfit(float *data,short n);
	//__declspec(dllexport) int getmonth(char *str);
	__declspec(dllexport) int fix_date(char *raw, char *processed);
	__declspec(dllexport) unsigned short tobin(char *buf);
	__declspec(dllexport) unsigned short tobin2(char *buf);
	__declspec(dllexport) void lshift(unsigned char *buf, int len, int n);
	__declspec(dllexport) void timestamp(char *str);
	__declspec(dllexport) int hexbuf_to_binbuf(char *hexbuf, unsigned char *binbuf, int hexbufsize, int binbufsize);
	__declspec(dllexport) int binbuf_to_hexbuf(unsigned char *binbuf, char *hexbuf, int binbufsize, int hexbufsize);
	__declspec(dllexport) void substring(const char *str, int pos1, int pos2, char *substr );
	__declspec(dllexport) void explode2(char *str, const char *sep, std::vector<std::string> &a);
	__declspec(dllexport) unsigned long unpack(unsigned long n);
	__declspec(dllexport) void pad(char *str, int strsize, char c);
	__declspec(dllexport) bool equals(char *s1, char *s2);
	__declspec(dllexport) bool equalsIgnoreCase(char *s1, char *s2);
	__declspec(dllexport) bool startsWith(char *haystack, char *needle);
	__declspec(dllexport) bool startsWithIgnoreCase(char *haystack, char *needle);
	__declspec(dllexport) void CopyFRect(FRECT *dest, FRECT *src);
#else
	int fcmp( float _f1, float _f2, int _decimal_places );
	int getmyip(void);
	UINT32 FourCC(char a, char b, char c, char d);
	int write_long(FILE *out, int n);
	int write_word(FILE *out, int n);

	int read_long(FILE *in);
	int read_word(FILE *in);

	int read_chars(FILE *in, char *s, int count);
	int read_chars_bin(FILE *in, char *s, int count);
	int write_chars(FILE *out, char *s);
	int write_chars_bin(FILE *out, char *s, int count);
	int strcasecmp(const char *s1, const char *s2);

	void line(HDC _hdc, int x1, int y1, int x2, int y2);
	void beep(void);
	void bang(void);
	void buzz(void);
	void alert(void);
	int mycopy(const char *existing_name, const char *new_name);
	void select_color(HWND hwnd, COLORREF &color, Ini *ini=NULL);
	WCHAR *to_wide(const char *str);
	void to_narrow(WCHAR *wstr, char *str, int len);
	void charToW(const char* cPtr, int cSize, wchar_t*wPtr);
	void convertAnsiStringToWide(WCHAR *wstrDestination, const CHAR *strSource, int cchDestChar);
	void convertWideStringToAnsi(CHAR *strDestination, const WCHAR *wstrSource, int cchDestChar);
	void SetClientRect(HWND hwnd, RECT *r);
	int move_to(FILE *stream, int line);
	void compute_sine(float *sine, int n, int T);
	void convtime(DWORD ms, int &h, int &m, int &s, int &ts);
	int getmonth(const char *str);
	const char *getmonth(int month, bool longform);			// 1-12

	int backup(FILE *stream, long n);
	void remove_empty_lines(char *fname);
	void reverse_file(char *fname);
	void fix_crlf(char *fname);
	void play(char *sound);
	double vmag(double x, double y, double z);
	double get_angle(double x1, double y1, double z1, double x2, double y2, double z2, bool degrees);
	void strtolower(char *);
	void strtoupper(char *);
	char *mybasename(const char *path);
	void dbase_to_csv(char *dbfname);
	void box(HDC hdc, RECT r, COLORREF=RGB(128, 128, 128), bool fill=false, HPEN pen=0);
	void hide(char *fname);
	void unhide(char *fname);
	void get_os_serial_number(unsigned char *sn, DWORD size);
	void getUniqueFileName(char *fname, int size);
	int doof(const char *_fname);
	int csv_add(char *buf, int buflen, int type, void *p, int _precision, bool _last=false);
	const char *MessageName(UINT msg);
	bool direxists(const char *path);
	int DeleteFolder(char *path);
	double nicenum(double x, int round);
	ticksummary linaxspec(double span, int maxtix, int maxnums, double* scale);
	int printfile(char *_filename, int _copies=1, bool _dialog=false, bool _cursorhidden=false, bool _landscape=false, int _linesPerPage=-1);
	void OutputHeading(const char * explanation);
	void SetRValue( COLORREF* pColor, BYTE value );
	void SetGValue( COLORREF* pColor, BYTE value );
	void SetBValue( COLORREF* pColor, BYTE value );
	int fcompare( const void *p1, const void *p2 );
	int irand(int min, int max);
	void repaintNow(HWND hwnd);
	void heapdump(void);
	double dlerp(double t, double x, double y);
	void buzz(void);
	LPWORD lpwAlign ( LPWORD lpIn);
	int nCopyAnsiToWideChar (LPWORD lpWCStr, LPSTR lpAnsiIn);
	HRGN BitmapToRegion (HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance);
		int expect(Logger *logg, char *str, Serial *port, DWORD timeout);
	void flush(Logger *logg, Serial *port,DWORD timeout, BOOL echo);
		void GetMachineCaps(Logger *log);
	int getnline(char *line, int linelength, FILE *stream);
	void delay(DWORD ms);
	SYSTEMTIME filetime(char *fname);
	void circle(HDC hdc,int xc,int yc,int ir, bool filled=true);
	void ellipse(HDC hdc,int xc,int yc,int irx, int iry, bool filled);
	void calc_fps(void);
	void mav(float *x,int n,int m,int flag);
	void center_window(HWND hwnd);
//	void center_child_window(HWND, HWND);

	void getClientRect(HWND hwnd, RECT *clientrect);
	unsigned short getRandomNumber(void);
	float fround(float number, int digits);
	double dround(double value, int precision);
	void time_format(DWORD tms, char *str);
	void time_format2(DWORD tms, char *str);
	void TransparentStretchBlt(
							 				HDC hDC,							// dest
							 				LONG left,						// dest
							 				LONG top,						// dest
							 				LONG width,						// dest
							 				LONG height,					// dest
								
							 				HBITMAP hBitmap,				// source bitmap
							 				LONG bmLeft,					// source
							 				LONG bmTop,						// source
							 				LONG bmWidth,					// source
							 				LONG bmHeight,					// scource
							 				COLORREF colorMask);			// transparent color
	bool valid_filename(const char *fname);
	double finterp(double a1, double a, double a2, double b1, double b2);
	bool is_open(const char *fname);
	int getbit(unsigned char c, int i);
	int pv(RECT, float);
	int ph(RECT, float);
	bool deq(double f1, double f2, double tolerance=.000001);
	void hot(void);
	void crand(char *str, int len);
	bool brand(void);
	void myexplode(const char *str, const char *sep, std::vector<std::string> &a);
	int sign(float f);
	float frand(float min, float max);
	bool rm(const char *fname);
	void strip_extension(char *fname);
	bool has_extension(char *_fname);
	bool change_extension(char *infname, char *ext1, char *outfname, int len, char *ext2);
	void get_extension(char *fname);			// returns the extension (without the '.') in fname
	void trimcrlf(char *string);
	char * fulltrim(char *string);
	void ltrimcrlf(char *string);

	bool isodd(int i);
	unsigned long filesize_from_stream(FILE *stream);
	unsigned long filesize_from_name(const char *filename);
	int indexIgnoreCase(const char *str1, const char *str2, int startpos=0, bool reverse=false);
	int myindex(const char *str1, const char *str2, int startpos=0);
	bool strEqual(char *str1, char *str2);
	void strip_path(char *fname);
	void strip_filename(char *fname);
	int replace(char *string, const char *oldstr, const char *newstr, int flag=0);
	int indexCharFromEnd(char *str1, char c);
	void ltrim(char *string, int trim_tabs=1, char *ends=NULL);
	void trim(char *string, const char *ends=NULL);
	int gethex(unsigned char *str);
	void bit_reverse(unsigned char *b);
	void myremove(char *pat, char *str);
	unsigned long Checksum(char *filename);
	unsigned short checksum(unsigned char *, int);
	void dump2(FILE *dumpstream, unsigned char *mb, int cnt);
	float bmr(float _lb, float _inches, float _years, char _sex, int _activityIndex);
	int insert(char *str1, const char *str2, int k);
	void print_amortization_table(float principal, float interest, int years, int month);
	float compute_payment(float P, float I, int years);
	double compute_remaining_principal(double _P, double _I, int _L, int _t);
	void dump(unsigned char *mb, int cnt, FILE *stream, char *leader=NULL);
	//void dump(unsigned char *mb, int cnt);
	void dump(char *_infname, char *_outfname, unsigned long _addr=0);
	void dump(unsigned char *mb, int cnt);

	bool ends_with(const char *str, const char *pat);
	bool isNumericString(const char *_str);
	int roundmod(float f, int mod);
	int round_a(int n, int mod);
	int round_af(float f, int mod);
	int countbits(long i);
	void swap(int *a, int *b);
	void swap_floats(float *f1, float *f2);
	void reverse_long_bytes(unsigned long *ul);
	long countlines(const char *fname, bool skip_blank = false);
	long searchFile(char *fname, long _offset, char *pat, int direction);
	long read_until_char(char *fname, long _offset, char cc, char *buf, int bufsize);
	bool getFileBuf(char *fname, long offset1, long offset2, char *buf, int buflen);
	void get_last_line(char *fname, char *str, int size);
	bool substr(char *_str, char _startchar, char _endchar, bool _inclusive, char *_outstr, int _outbufsize, int *i1, int *i2);
	bool exists(const char *fname);
	bool replace_extension(char *fname, const char *new_extension);
	int count_strings(char **strs);
	void leastsquare(std::vector<float> &v, float *m, float *b);
	//void leastsquare(float *x,float *y,short n,float *m,float *b);
	void leastsquares(float *x,float *y,short n,float *m,float *b);
	double mean(float *data,short n);
	double median(float *data,short n);
	double statmode(float *data,short n);
	double rms(float *data,short n);
	double geometric_mean(float *data,short n);
	double harmonic_mean(float *data,short n);
	void gaussfit(float *data,short n);
	//int getmonth(char *str);
	int fix_date(char *raw, char *processed);
	unsigned short tobin(char *buf);
	unsigned short tobin2(char *buf);
	void lshift(unsigned char *buf, int len, int n);
	void timestamp(char *str);
	int hexbuf_to_binbuf(char *hexbuf, unsigned char *binbuf, int hexbufsize, int binbufsize);
	int binbuf_to_hexbuf(unsigned char *binbuf, char *hexbuf, int binbufsize, int hexbufsize);
	void substring(const char *str, int pos1, int pos2, char *substr );
	void explode2(char *str, const char *sep, std::vector<std::string> &a);
	unsigned long unpack(unsigned long n);
	void pad(char *str, int strsize, char c);
	bool equals(char *s1, char *s2);
	bool equalsIgnoreCase(char *s1, char *s2);
	bool startsWith(char *haystack, char *needle);
	bool startsWithIgnoreCase(char *haystack, char *needle);
	void CopyFRect(FRECT *dest, FRECT *src);
	bool float_is_integer(float f);
	//void rounded_rect(Gdiplus::Graphics *g, float pw, Gdiplus::Color pencolor, RECT rect, float r, Gdiplus::Color fillcolor );
	//void rounded_rect(Graphics *g, float pw, Color pencolor, RECT rect, float r, Color fillcolor );
	UINT32 fourcc(const char * a);
	const char *fourcc(UINT32 x);
	int gettimeofday(struct timeval *tv, struct timezone *tz);
	void TransparentStretchBlt(
				HDC hDC,							// dest
				LONG left,						// dest
				LONG top,						// dest
				LONG width,						// dest
				LONG height,					// dest

				HBITMAP hBitmap,				// source bitmap
				LONG bmLeft,					// source
				LONG bmTop,						// source
				LONG bmWidth,					// source
				LONG bmHeight,					// scource
				COLORREF colorMask);			// transparent color

#endif


#ifdef DOC
	}				// extern "C" {
#endif

#endif		// #ifdef WIN32

