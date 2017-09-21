
#ifndef _LINUX_UTILS_H_
#define _LINUX_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <vector>
#include <string>

//#include <jni.h>

#ifndef WIN32
	#ifndef MYOSX
    //#include <gtk/gtk.h>				// mac
	#endif
#endif

#include <glib_defines.h>			// for RECT, SYSTEMTIME, etc
#include <glib_types.h>

#include <logger.h>
#include <serial.h>
//#include <config.h>

/*
typedef struct  {
	char fname[256];
	unsigned long size;
} ENCREC;
*/

#ifdef DOJNI
xxxxxxxxxxx
	unsigned char low(unsigned short us);
	unsigned char high(unsigned short us);
	JNIEXPORT void mybeep(void);
	JNIEXPORT void bang(void);
	JNIEXPORT void buzz(void);
	JNIEXPORT void alert(void);
	JNIEXPORT unsigned short getRandomNumber(void);
	JNIEXPORT BOOL JNICALL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	//JNIEXPORT BOOL JNICALL CreateDirectory( const char *lpPathName, SECURITY_ATTRIBUTES *lpSecurityAttributes);
	JNIEXPORT bool JNICALL direxists(const char *path);
	JNIEXPORT void JNICALL time_format(DWORD tms, char *str);
	JNIEXPORT void JNICALL convtime(DWORD ms, int &h, int &m, int &s, int &ts);
	JNIEXPORT void JNICALL show_info(GtkWidget *widget, gpointer window);
	JNIEXPORT void JNICALL show_error(GtkWidget *widget, gpointer window);
	JNIEXPORT void JNICALL show_question(GtkWidget *widget, gpointer window);
	JNIEXPORT void JNICALL show_warning(GtkWidget *widget, gpointer window);
	JNIEXPORT void JNICALL GetLocalTime( SYSTEMTIME *SystemTime );
	JNIEXPORT void JNICALL timeBeginPeriod(unsigned long ms);
	JNIEXPORT void JNICALL timeEndPeriod(unsigned long ms);
	JNIEXPORT int JNICALL MessageBox(int _hwnd, const char * msg, const char *_title, unsigned long _style);
	JNIEXPORT void JNICALL CopyRect(RECT *dest, RECT *src);
	JNIEXPORT void JNICALL CopyFRect(FRECT *dest, FRECT *src);
	JNIEXPORT void JNICALL flush(Logger *logg, Serial *port, DWORD timeout, BOOL echo);
	JNIEXPORT int JNICALL WritePrivateProfileString(const char *section, const char *entry, const char *buffer, const char *file_name);
	JNIEXPORT int JNICALL read_line(FILE *fp, char *bp);
	JNIEXPORT int JNICALL GetPrivateProfileString(const char *section, const char *entry, const char *def, char *buffer, int buffer_len, char *file_name);
	JNIEXPORT int JNICALL GetPrivateProfileInt(char *section, char *entry, int def, char *file_name);
	JNIEXPORT bool JNICALL SetCurrentDirectory(char *dir);
	JNIEXPORT void JNICALL SetRect(RECT*, int, int, int, int);
	JNIEXPORT bool JNICALL CopyFile(const char *lpExistingFileName, const char *lpNewFileName, bool bFailIfExists);
	JNIEXPORT unsigned long JNICALL GetCurrentDirectory(int _size, char *_curdir);
	JNIEXPORT unsigned long JNICALL timeGetTime(void);
	JNIEXPORT char * JNICALL strupr(char *str);
	JNIEXPORT char * JNICALL strlwr(char *str);
	JNIEXPORT void JNICALL echo_off(void);
	JNIEXPORT void JNICALL echo_on(void);
	JNIEXPORT void JNICALL set_keypress(void);
	JNIEXPORT void JNICALL reset_keypress(void);
	JNIEXPORT int JNICALL enc(char *in_name);
	JNIEXPORT int JNICALL dec(char *in_name, bool _recursive);
//	JNIEXPORT bool JNICALL valid_filename(const char *fname);
	JNIEXPORT void JNICALL leastsquare(std::vector<float> &v, float *m, float *b);
	JNIEXPORT double JNICALL finterp(double a1, double a, double a2, double b1, double b2);
	JNIEXPORT bool JNICALL is_open(const char *fname);
	JNIEXPORT int JNICALL getbit(unsigned char c, int i);
	JNIEXPORT int JNICALL pv(RECT, float);
	JNIEXPORT int JNICALL ph(RECT, float);
	JNIEXPORT bool JNICALL deq(double f1, double f2, double tolerance=.000001);
	JNIEXPORT void JNICALL crand(char *str, int len);
	JNIEXPORT bool JNICALL brand(void);
	JNIEXPORT void JNICALL explode(const char *str, const char *sep, std::vector<std::string> &a);
	JNIEXPORT int JNICALL sign(float f);
	JNIEXPORT float JNICALL frand(float min, float max);
	JNIEXPORT bool JNICALL rm(const char *fname);
	JNIEXPORT void JNICALL strip_extension(char *fname);
	JNIEXPORT bool JNICALL has_extension(char *_fname);
	JNIEXPORT bool JNICALL change_extension(char *infname, char *ext1, char *outfname, int len, char *ext2);
	JNIEXPORT void JNICALL get_extension(char *fname);			// returns the extension (without the '.') in fname
	JNIEXPORT void JNICALL trimcrlf(char *string);
	JNIEXPORT bool JNICALL isodd(int i);
	JNIEXPORT unsigned long JNICALL filesize_from_stream(FILE *stream);
	JNIEXPORT unsigned long JNICALL filesize_from_name(const char *filename);
	JNIEXPORT int JNICALL indexIgnoreCase(const char *str1, const char *str2, int startpos=0, bool reverse=false);
	JNIEXPORT int JNICALL myindex(const char *str1, const char *str2, int startpos=0);
	JNIEXPORT int JNICALL myindex(char *str1, const char *str2, std::vector<int> &a);
	JNIEXPORT bool JNICALL strEqual(char *str1, char *str2);
	JNIEXPORT void JNICALL strip_path(char *fname);
	JNIEXPORT void JNICALL strip_filename(char *fname);
	JNIEXPORT int JNICALL replace(char *string, const char *oldstr, const char *newstr, int flag=0);
	JNIEXPORT int JNICALL indexCharFromEnd(char *str1, char c);
	JNIEXPORT void JNICALL ltrim(char *string, int trim_tabs=1, char *ends=NULL);
	JNIEXPORT void JNICALL trim(char *string, const char *ends=NULL);
	JNIEXPORT int JNICALL gethex(unsigned char *str);
	JNIEXPORT void JNICALL bit_reverse(unsigned char *b);
	JNIEXPORT void JNICALL myremove(const char *pat, char *str);
	JNIEXPORT unsigned long JNICALL Checksum(char *filename);
	JNIEXPORT unsigned short JNICALL checksum(unsigned char *, int);
	JNIEXPORT void JNICALL dump2(FILE *dumpstream, unsigned char *mb, int cnt);
	JNIEXPORT float JNICALL bmr(float _lb, float _inches, float _years, char _sex, int _activityIndex);
	JNIEXPORT int JNICALL insert(char *str1, const char *str2, int k);
	JNIEXPORT void JNICALL print_amortization_table(float principal, float interest, int years, int month);
	JNIEXPORT float JNICALL compute_payment(float P, float I, int years);
	JNIEXPORT double JNICALL compute_remaining_principal(double _P, double _I, int _L, int _t);
	JNIEXPORT void JNICALL dump(unsigned char *mb, int cnt, FILE *stream, char *leader=NULL);
	JNIEXPORT void JNICALL dump(unsigned char *mb, int cnt);
	JNIEXPORT void JNICALL dump(char *_infname, char *_outfname, unsigned long _addr=0);
	JNIEXPORT bool JNICALL isNumericString(char *_str);
	JNIEXPORT int JNICALL roundmod(float f, int mod);
	JNIEXPORT int JNICALL round_a(int n, int mod);
	JNIEXPORT int JNICALL round_af(float f, int mod);
	JNIEXPORT int JNICALL countbits(long i);
	JNIEXPORT void JNICALL swap(int *a, int *b);
	JNIEXPORT void JNICALL reverse_long_bytes(unsigned long *ul);
	JNIEXPORT long JNICALL countlines(const char *fname, bool skip_blank = false);
	JNIEXPORT long JNICALL searchFile(char *fname, long _offset, char *pat, int direction);
	JNIEXPORT long read_until_char(char *fname, long _offset, char cc, char *buf, int bufsize);
	JNIEXPORT bool JNICALL getFileBuf(char *fname, long offset1, long offset2, char *buf, int buflen);
	JNIEXPORT void JNICALL get_last_line(char *fname, char *str, int size);
	JNIEXPORT bool JNICALL substr(char *_str, char _startchar, char _endchar, bool _inclusive, char *_outstr, int _outbufsize, int *i1, int *i2);
	JNIEXPORT bool JNICALL exists(const char *fname);
	JNIEXPORT bool JNICALL replace_extension(char *fname, const char *new_extension);
	JNIEXPORT int JNICALL count_strings(char **strs);
	JNIEXPORT void JNICALL leastsquare(float *x,float *y,short n,float *m,float *b);
	JNIEXPORT void JNICALL leastsquares(float *x,float *y,short n,float *m,float *b);
	JNIEXPORT double JNICALL mean(float *data,short n);
	JNIEXPORT double JNICALL median(float *data,short n);
	JNIEXPORT double JNICALL statmode(float *data,short n);
	JNIEXPORT double JNICALL rms(float *data,short n);
	JNIEXPORT double JNICALL geometric_mean(float *data,short n);
	JNIEXPORT double JNICALL harmonic_mean(float *data,short n);
	JNIEXPORT void JNICALL gaussfit(float *data,short n);
	JNIEXPORT int JNICALL getmonth(char *str);
	JNIEXPORT int JNICALL fix_date(char *raw, char *processed);
	JNIEXPORT unsigned short JNICALL tobin(char *buf);
	JNIEXPORT unsigned short JNICALL tobin2(char *buf);
	JNIEXPORT void JNICALL lshift(unsigned char *buf, int len, int n);
	JNIEXPORT void JNICALL timestamp(char *str);
	JNIEXPORT int JNICALL hexbuf_to_binbuf(char *hexbuf, unsigned char *binbuf, int hexbufsize, int binbufsize);
	JNIEXPORT int JNICALL binbuf_to_hexbuf(unsigned char *binbuf, char *hexbuf, int binbufsize, int hexbufsize);
	JNIEXPORT void JNICALL substring(const char *str, int pos1, int pos2, char *substr );
	JNIEXPORT void JNICALL explode2(char *str, const char *sep, std::vector<std::string> &a);
	JNIEXPORT unsigned long JNICALL unpack(unsigned long n);
	JNIEXPORT void JNICALL pad(char *str, int strsize, char c);
	JNIEXPORT bool JNICALL equals(char *s1, char *s2);
	JNIEXPORT bool JNICALL equalsIgnoreCase(char *s1, char *s2);
	JNIEXPORT bool JNICALL startsWith(char *haystack, char *needle);
	JNIEXPORT bool JNICALL startsWithIgnoreCase(char *haystack, char *needle);
#else
	//void flush(Logger *logg, Serial *port, DWORD timeout, BOOL echo);

	void send(Serial *port, char *str, FILE *_stream=NULL, int flush_flag=0);
	int doof(const char *_fname);
	void mybeep(void);
	void bang(void);
	void buzz(void);
	void swap_floats(float *f1, float *f2);

	void alert(void);
	void CopyFRect(FRECT *dest, FRECT *src);
	void dprintf(const char *format, ...);
        //double deq(double d1, double d2, double delta);
	void CopyRect(RECT *dest, RECT *src);
	void CopyRect(FRECT *dest, RECT *src);
	void CopyRect(FRECT *dest, FRECT *src);

	unsigned short getRandomNumber(void);
	BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
	bool direxists(const char *path);
	void time_format(DWORD tms, char *str);
	void time_format2(DWORD tms, char *str);
	void convtime(DWORD ms, int &h, int &m, int &s, int &ts);

	#ifndef MYOSX
//	void show_info(GtkWidget *widget, gpointer window);
//	void show_error(GtkWidget *widget, gpointer window);
//	void show_question(GtkWidget *widget, gpointer window);
//	void show_warning(GtkWidget *widget, gpointer window);
	#endif

	void GetLocalTime( SYSTEMTIME *SystemTime );
	void timeBeginPeriod(unsigned long ms);
	void timeEndPeriod(unsigned long ms);
	int MessageBox(int _hwnd, const char * msg, const char *_title, unsigned long _style);
	int WritePrivateProfileString(const char *section, const char *entry, const char *buffer, const char *file_name);
	int read_line(FILE *fp, char *bp);
	int GetPrivateProfileString(const char *section, const char *entry, const char *def, char *buffer, int buffer_len, char *file_name);
	int GetPrivateProfileInt(char *section, char *entry, int def, char *file_name);
	bool SetCurrentDirectory(char *dir);
	void SetRect(RECT*, int, int, int, int);
	bool CopyFile(const char *lpExistingFileName, const char *lpNewFileName, bool bFailIfExists);
	unsigned long GetCurrentDirectory(int _size, char *_curdir);
	unsigned long timeGetTime(void);
	//char * strupr(char *str);
	//char * strlwr(char *str);
	void echo_off(void);
	void echo_on(void);
	void set_keypress(void);
	void reset_keypress(void);
	int enc(char *in_name);
	int dec(char *in_name, bool _recursive);
	bool valid_filename(const char *fname);
	void leastsquare(std::vector<float> &v, float *m, float *b);
	double finterp(double a1, double a, double a2, double b1, double b2);
	bool is_open(const char *fname);
	int getbit(unsigned char c, int i);
	int pv(RECT, float);
	int ph(RECT, float);
	bool deq(double f1, double f2, double tolerance=.000001);
	void crand(char *str, int len);
	bool brand(void);
	void explode(char *str, const char *sep, std::vector<std::string> &a);
	int sign(float f);
	float frand(float min, float max);
	bool rm(const char *fname);
	void strip_extension(char *fname);
	bool has_extension(char *_fname);
	bool change_extension(char *infname, char *ext1, char *outfname, int len, char *ext2);
	void get_extension(char *fname);			// returns the extension (without the '.') in fname
	void trimcrlf(char *string);
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
	char * fulltrim(char *string);
	void ltrimcrlf(char *string);
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
	double compute_remaining_principal(double, double, int, int);
	void dump(unsigned char *mb, int cnt, FILE *stream, char *leader=NULL);
	void dump(unsigned char *mb, int cnt);
	void dump(char *_infname, char *_outfname, unsigned long _addr=0);
	bool isNumericString(char *_str);
	int roundmod(float f, int mod);
	int round_a(int n, int mod);
	int round_af(float f, int mod);
	int countbits(long i);
	void swap(int *a, int *b);
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
	void leastsquare(float *x,float *y,short n,float *m,float *b);
	void leastsquares(float *x,float *y,short n,float *m,float *b);
	double mean(float *data,short n);
	double median(float *data,short n);
	double statmode(float *data,short n);
	double rms(float *data,short n);
	double geometric_mean(float *data,short n);
	double harmonic_mean(float *data,short n);
	void gaussfit(float *data,short n);
	int getmonth(char *str);
	const char *getmonth(int month, bool longform);			// 1-12
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
	bool float_is_integer(float f);
	UINT32 fourcc(const char * a);
	const char *fourcc(UINT32 x);
	char *mybasename(const char *path);
	int getmyip(void);
#endif

#endif				// #ifndef _LINUX_UTILS_H_

