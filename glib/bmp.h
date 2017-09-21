
#ifndef _BMP_H_
#define _BMP_H_

#include <stdio.h>

#ifdef WIN32
	#include <windows.h>
#endif


	class BMP  {
		private:
#ifdef WIN32
			HBITMAP hBitmap;
			BITMAP bm;
#endif

			int bpp;

#ifdef WIN32
			HPALETTE hpal;
			LOGPALETTE *lp;
			char *lpDib;
			DWORD dibSize;
			BITMAPFILEHEADER bmfh;
			BITMAPINFO *lpbi;
			BITMAPINFOHEADER bmih;
#endif
			char fname[256];
			void cleanup(void);
			void init(void);
#ifdef WIN32
			unsigned int bmfhSize;
			unsigned int bmihSize;
			long file_size;
			unsigned int rgbQuadSize;
#endif
			unsigned char minred;
			unsigned char maxred;
			unsigned char mingreen;
			unsigned char maxgreen;
			unsigned char minblue;
			unsigned char maxblue;
			float brightness;
			float min_brightness;
			float max_brightness;

		public:
			BMP(char *_fname);
			BMP(int w, int h, int bpp, unsigned char * _image=NULL, const char *_fname=NULL);
			virtual ~BMP();
#ifdef WIN32
			void paint(HDC hdc, RECT *);
			void paint2(HDC hdc, RECT *winrect);
#else
#endif
			void save(char *_fname, unsigned char *image);
			void save(void);
			int w;
			int h;
			unsigned char *image;
			int getbpp(void)  { return bpp; }
			void getpixel(int j, int i, unsigned char *r, unsigned char *g, unsigned char *b);
			void setpixel(int j, int i, unsigned char r, unsigned char g, unsigned char b);
			void stats(void);
	};



#endif

