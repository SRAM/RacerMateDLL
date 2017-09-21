
#ifdef WIN32
#define STRICT
#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#include <string.h>
#include <assert.h>
#include <float.h>

#ifdef _DEBUG
/*
	#include <crtdbg.h>
	#include <malloc.h>
	#define _CRTDBG_MAP_ALLOC // map malloc to the debug version
	#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
	*/
#endif



#include <glib_defines.h>
#include <fatalerror.h>
//#include <bmp.h>
#include "bmp.h"					// do it this way because ffmpeg now has a bmp.h
#include <utils.h>


#ifndef WIN32
	#include <minmax.h>
#endif


/**********************************************************************
	constructor used to create a DIB in memory from scratch
**********************************************************************/

//BMP::BMP(int _w, int _h, int _bpp, unsigned char *_image=NULL, const char *_fname=NULL)  {
BMP::BMP(int _w, int _h, int _bpp, unsigned char *_image, const char *_fname)  {
	fname[0] = 0;

	init();

	w = _w;
	h = _h;
	bpp = _bpp;

#ifdef WIN32
	bmfh.bfType = 0x4d42;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	//bmfh.bfSize = bmfhSize + bmihSize + w*h*3;

	if (bpp==8)  {
		bmfh.bfSize =	bmfhSize + bmihSize + 256*sizeof(PALETTEENTRY) + w*h;
	}
	else  {
		bmfh.bfSize =	bmfhSize + bmihSize + 0*sizeof(PALETTEENTRY) + w*h*3;
	}

	bmfh.bfOffBits = bmfhSize + bmihSize;

	//---------------------------------------------
	// INITIALIZE THE BITMAPINFOHEADER STRUCTURE:
	//---------------------------------------------

	/*
	if (bpp==8) bmih.biBitCount      = (WORD) bpp;
	else if (bpp==16) bmih.biBitCount = 24;
//	else bmih.biBitCount = 24;

  bmih.biCompression   = BI_RGB;		// uncompressed format

//	if (bpp==8) bmih.biSizeImage		= 0;				// 640*480 must be set to 0 for BI_RGB
//	else if (bpp==16) bmih.biSizeImage = w*h*3;
//	else bmih.biSizeImage = w*h*3;
	*/

	bmih.biClrImportant = 0;
	bmih.biClrUsed = 0;
	bmih.biHeight = h;
	bmih.biPlanes = 1;
	bmih.biSize = bmihSize;
	bmih.biWidth = w;
	bmih.biXPelsPerMeter = 0;
	bmih.biYPelsPerMeter = 0;

	bmih.biCompression = 0;									// ???
	bmih.biBitCount = 24;									// ??? bpp
	bmih.biSizeImage = w*h*(bmih.biBitCount/8);		// ???

	/*
	if (bpp==8)  {
		if((lp=(LOGPALETTE*)malloc(sizeof(LOGPALETTE) + sizeof(RGBQUAD)*255))== NULL)  {
			log->write(0,1,1,"Could not allocate palette memory\n");
			return false;
		}
		log->write(0,1,1,"allocated palette memory\n");
		lp->palVersion = 0x300;
		lp->palNumEntries = 256;
		for(i=0; i < lp->palNumEntries; i++)  {
			lp->palPalEntry[i].peRed = palette[i].peRed;
			lp->palPalEntry[i].peGreen = palette[i].peGreen;
			lp->palPalEntry[i].peBlue = palette[i].peBlue;
			lp->palPalEntry[i].peFlags = 0;
		}
		if ((hpal = CreatePalette(lp))==NULL)  {
			log->write(0,1,1,"Could not create palette\n");
			free(lp);
			return false;
		}
	}
	*/

	if(!_image)  {
		return;
	}

	if (!_fname)  {
		return;
	}

	FILE *outstream;
	int status;
	LOGPALETTE *lp = NULL;
	HPALETTE	hpal = NULL;

	if ((outstream = fopen(_fname, "wb")) == NULL)  {
		throw fatalError(__FILE__, __LINE__);
		return;
	}

	//----------------
	// WRITE THE BMFH
	//----------------

	if (fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), outstream) < sizeof(BITMAPFILEHEADER))  {
		fclose(outstream);
		throw fatalError(__FILE__, __LINE__);
	}

	//-----------------
	// WRITE THE BMIH
	//-----------------

	status = fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, outstream);
	if (status != 1)  {
		fclose(outstream);
		throw fatalError(__FILE__, __LINE__);
	}


	/*
	if (bpp==8)  {
		//---------------------------------------------
		// WRITE THE PALETTE (WITH BLUE & RED SWAPPED)
		//---------------------------------------------

		log->write(0,1,1,"writing palette\n");


		for(i=0;i<256;i++)  {
			temp = palette[i].peBlue;								// swap blue & red
			palette[i].peBlue = palette[i].peRed;
			palette[i].peRed = temp;

			status = fwrite(&palette[i], sizeof(PALETTEENTRY), 1, outstream);

			temp = palette[i].peBlue;								// swap blue & red
			palette[i].peBlue = palette[i].peRed;
			palette[i].peRed = temp;

			if (status != 1)  {
				log->write(0,1,1,"palette write error, i = %d\n",i);
				break;
			}
			else  {
				log->write(0,2,0,"%3d - %3d %3d %3d\n",
					i, palette[i].peRed, palette[i].peGreen, palette[i].peBlue);
			}
		}

		if (i==256)  {
			log->write(0,2,1,"success writing palette\n");
		}

		log->write(0,1,1,"total file size so far: %ld\n",
			sizeof(BITMAPFILEHEADER) +
			sizeof(BITMAPINFOHEADER) +
			256*sizeof(PALETTEENTRY));
	}
	*/

	fwrite(_image, sizeof(unsigned char), bmih.biSizeImage, outstream);
	fclose(outstream);


	if (hpal) DeleteObject(hpal);
	if (lp) free(lp);								// free palette
#endif
	
	return;
}



/**********************************************************************

**********************************************************************/

BMP::BMP(char *_fname)  {

	if (!exists(_fname))  {
		throw(1);
	}


	strncpy(fname, _fname, sizeof(fname)-1);
	init();

#ifdef WIN32

	if (hBitmap)  {
		DeleteObject(hBitmap);
		hBitmap = 0;
	}

	hBitmap = (HBITMAP)LoadImage (
					0, 
					fname, 
					IMAGE_BITMAP, 
					0, 
					0, 
					LR_LOADFROMFILE);

	if (hBitmap == 0)  {
		throw fatalError(__FILE__, __LINE__);
	}

	GetObject(hBitmap, sizeof (bm), &bm);

	// get abbreviated copies of width & height

	/*
		typedef struct tagBITMAP
		  {
			 LONG        bmType;
			 LONG        bmWidth;
			 LONG        bmHeight;
			 LONG        bmWidthBytes;
			 WORD        bmPlanes;
			 WORD        bmBitsPixel;
			 LPVOID      bmBits;
		  } BITMAP;
	*/

	w = bm.bmWidth;
	h = bm.bmHeight;
	bpp = bm.bmBitsPixel;

	//----------------------------------------
	// now get the RAW file information
	//----------------------------------------

	long status;
	FILE *stream;

	file_size = filesize_from_name(fname);

	stream = fopen(fname, "rb");
	if (stream==NULL)  {
		cleanup();
		throw fatalError(__FILE__, __LINE__, "Can't open .bmp file.");
	}

	//------------------------------
	// READ THE BITMAPFILEHEADER
	//------------------------------

	/*
		typedef struct tagBITMAPFILEHEADER {
				  WORD    bfType;
				  DWORD   bfSize;
				  WORD    bfReserved1;
				  WORD    bfReserved2;
				  DWORD   bfOffBits;
		} BITMAPFILEHEADER, FAR *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
	*/

	status = fread(&bmfh, 1, sizeof(BITMAPFILEHEADER), stream);
	assert(status == bmfhSize);																// 14
	assert(bmfh.bfSize==file_size);							// 24632

#ifdef _DEBUG
	int offs = bmfhSize + bmihSize;		// debugging
	assert(offs==bmfh.bfOffBits);				// shoule be 54
	float f = (float)file_size / 4.0f;
	offs += w*h*3;
	f = (float)offs / 4.0f;
	//int size = offs / 4.0f;
	//int size = ((offs + 31)/32)*4;
	int calc_size = (bmfhSize + bmihSize + w*h*3) % 4 ;							// file_size must be mod 4
	calc_size += (bmfhSize + bmihSize + w*h*3) ;
	assert(calc_size==file_size);

#endif


	//--------------------------------------------------------------------------------
	// bmfh.bfOffBits is the offset from the beginning of the file of the image!!!!
	// Ie, there is a gap between BITMAPINFO and the data!!!!
	//--------------------------------------------------------------------------------

	if (status != (long) sizeof(BITMAPFILEHEADER))  {
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}

	if (bmfh.bfType != 0x4d42) {						// "BM" backwards
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}

	dibSize = bmfh.bfSize - sizeof(BITMAPFILEHEADER);

	lpDib = new char[dibSize];
	if (lpDib==NULL)  {
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}

	//--------------------------------------------------------------
	// READ IN THE BITMAPINFO HEADER AND THE IMAGE AT THE SAME TIME
	//--------------------------------------------------------------

	status = fread(lpDib, 1, dibSize, stream);
	if ((unsigned int)status != dibSize)  {
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}

	fclose(stream);						// cgerr
	stream = NULL;

	//-----------------------------------------
	// set the pointers into the DIB
	//-----------------------------------------

	lpbi = (LPBITMAPINFO) lpDib;
	memcpy(&bmih, &lpbi->bmiHeader, bmihSize);

	int n = sizeof(BITMAPINFOHEADER);					// 40 (28)
	n = sizeof(BITMAPINFO);									// 44 (2c) (includes 1 RGBQUAD?)
	image = (unsigned char *) (lpDib - sizeof(BITMAPFILEHEADER) + bmfh.bfOffBits);

	// sanity checks:
	assert(w==lpbi->bmiHeader.biWidth);
	assert(h==lpbi->bmiHeader.biHeight);

	if (lpbi->bmiHeader.biBitCount==8)  {
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}
	else  {
		assert(bmfh.bfOffBits==(bmfhSize + bmihSize));
	}
#endif
	
	return;
}


/**********************************************************************

**********************************************************************/

BMP::~BMP()  {

	//#ifdef _DEBUG
	//int status = _CrtIsValidHeapPointer(this);
	//assert(_CrtIsValidHeapPointer(this));
	//#endif

	cleanup();

	#ifdef _DEBUG
//		heapdump();
	#endif

}

/**********************************************************************

**********************************************************************/

void BMP::cleanup(void)  {

#ifdef WIN32

	if (hpal)  {
		DeleteObject(hpal);
	}

	if (lpDib)  {
		delete[] lpDib;
		lpDib = NULL;
	}

	DEL(lp);

	if (hBitmap)  {
		DeleteObject(hBitmap);
		hBitmap = 0;
	}
#endif
	return;
}


/**********************************************************************

**********************************************************************/

void BMP::init(void)  {

#ifdef WIN32

	image = NULL;
	bpp = 0;
	dibSize = 0;
	lp = NULL;
	hpal = 0;
	file_size = 0;
	lpbi = NULL;
	lpDib = NULL;
	hBitmap = 0;
	w = 0;
	h = 0;
	memset(&bmfh, 0, sizeof(bmfh));
	memset(&bmih, 0, sizeof(bmih));
	//bm = 0;
	bmfhSize = sizeof(BITMAPFILEHEADER);
	bmihSize = sizeof(BITMAPINFOHEADER);
	rgbQuadSize = sizeof(RGBQUAD);
#endif
	
	minred = 0xff;
	mingreen = 0xff;
	minblue = 0xff;

	maxred = 0x00;
	maxgreen = 0x00;
	maxblue = 0x00;

	brightness = 0.0f;
	min_brightness = FLT_MAX;
	max_brightness = -FLT_MAX;

	return;
}


/**********************************************************************

**********************************************************************/
#ifdef WIN32

void BMP::paint(HDC hdc, RECT *winrect)  {
	BOOL b;

	if (hBitmap)  {
		HDC memdc = CreateCompatibleDC(NULL);

		HBITMAP h = (HBITMAP)SelectObject(memdc, hBitmap);

		int ww = winrect->right - winrect->left;			// +1?
		int hh = winrect->bottom - winrect->top;			// +1?


		b = StretchBlt(
				hdc,				// handle to destination DC
				winrect->left,	// x-coord of destination upper-left corner
				winrect->top,	// y-coord of destination upper-left corner
				ww,					// width of destination rectangle
				hh,				// height of destination rectangle
				memdc,			// handle to source DC
				0,					// x-coord of source upper-left corner
				0,					// y-coord of source upper-left corner
				bm.bmWidth,		// width of source rectangle
				bm.bmHeight,	// height of source rectangle
				SRCCOPY			// raster operation code
			);

		DeleteDC(memdc);

		if (b==FALSE)  {
			cleanup();
			throw fatalError(__FILE__, __LINE__);
		}
	}

	return;
}

/**********************************************************************
	another method of painting, using the raw file information
**********************************************************************/

void BMP::paint2(HDC hdc, RECT *winrect)  {
	int status;

	if (image==NULL)  {
		return;
	}

	int dibFlags;
	Escape(hdc, QUERYDIBSUPPORT, sizeof(lpbi), (const char *)lpbi, &dibFlags);
	if ( !(dibFlags & QDI_DIBTOSCREEN))  {
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}


	int w = winrect->right - winrect->left;			// +1?
	int h = winrect->bottom - winrect->top;			// +1?

	//SetStretchBltMode(hdc, COLORONCOLOR);

	status = StretchDIBits(
		hdc,									// dest hdc
		winrect->left,						// dest x
		winrect->top,						// dest y
		w,										// dest width
		h,										// dest height

		0,										// source x
		0,										// source y
		lpbi->bmiHeader.biWidth,		// source width
		lpbi->bmiHeader.biHeight,		// source height

		image,								// data
		lpbi,
		DIB_RGB_COLORS,					// palette
		SRCCOPY								// rop
	);

	if (status == GDI_ERROR) {
		cleanup();
		throw fatalError(__FILE__, __LINE__);
	}

	// alternative:	use SetDIBits() + StretchBlt()

	return;
}
#endif

/**********************************************************************

**********************************************************************/

void BMP::save(char *_fname, unsigned char *image)  {
	FILE *stream;
	stream = fopen(_fname, "wb");

#ifdef WIN32
	int status;
	long offset;

	status = fwrite(&bmfh, bmfhSize, 1, stream);
	status = fwrite(&bmih, bmihSize, 1, stream);

	offset = ftell(stream);
	assert(offset = bmfh.bfOffBits);

	status = fwrite(image, bmih.biSizeImage, 1, stream);
#endif
	
	fclose(stream);
	return;
}

/**********************************************************************

**********************************************************************/

void BMP::save(void)  {
#ifdef WIN32
	int status;
	long offset;
#endif

	FILE *stream;

	stream = fopen(fname, "wb");

#ifdef WIN32

	status = fwrite(&bmfh, bmfhSize, 1, stream);
	status = fwrite(&bmih, bmihSize, 1, stream);

	offset = ftell(stream);
	assert(offset = bmfh.bfOffBits);

	status = fwrite(image, bmih.biSizeImage, 1, stream);
#endif
	
	fclose(stream);
	return;
}


/**********************************************************************

**********************************************************************/

void BMP::getpixel(int j, int i, unsigned char *r, unsigned char *g, unsigned char *b)  {

	if (j<0 || j>(w-1))  {
		*r = 0;
		*g = 0;
		*b = 0;
		return;
	}

	if (i<0 || i>(h-1))  {
		*r = 0;
		*g = 0;
		*b = 0;
		return;
	}

	int k;

	k = i*w*3 + j*3;
	//k = i*(w*3) + 3*j;

	// note: red & blue are swapped in bmp images

	*b = image[k++];
	*g = image[k++];
	*r = image[k];

	return;
}

/**********************************************************************

**********************************************************************/

void BMP::stats(void)  {
	unsigned char r, g, b;
	int i, j, k;

	for(i=0; i<h; i++)  {
		for(j=0; j<w; j++)  {
			k = i*w*3 + j*3;
			b = image[k++];
			g = image[k++];
			r = image[k];

			minred = MIN(minred, r);
			mingreen = MIN(mingreen, g);
			minblue = MIN(minblue, b);

			maxred = MAX(maxred, r);
			maxgreen = MAX(maxgreen, g);
			maxblue = MAX(maxblue, b);

			brightness = 0.3f * r + 0.59f * g + 0.11f * b;

			min_brightness = MIN(min_brightness, brightness);
			max_brightness = MAX(max_brightness, brightness);

		}
	}

	return;
}

/**********************************************************************

**********************************************************************/

void BMP::setpixel(int j, int i, unsigned char r, unsigned char g, unsigned char b)  {

	if (j<0 || j>(w-1))  {
		return;
	}

	if (i<0 || i>(h-1))  {
		return;
	}

	int k;

	k = i*w*3 + j*3;

	// note: red & blue are swapped in bmp images

	image[k++] = b;
	image[k++] = g;
	image[k] = r;

	return;
}
