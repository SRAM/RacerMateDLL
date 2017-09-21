#ifndef __TGA_H__
#define __TGA_H__

#ifdef _WIN32
	#include <windows.h>									// Standard windows header
	#include <GL\gl.h>										// Header for OpenGL32 library
#else
	#include <gl.h>
	//#include <GL/gl.h>										// Header for OpenGL32 library
#endif


#include <stdio.h>										// Standard I/O header 
//#include "texture.h"
#include <logger.h>

class TGA  {
	public:
		typedef	struct  {									
			GLubyte	*imageData;									// Image Data (Up To 32 Bits)
			GLuint	bpp;											// Image Color Depth In Bits Per Pixel
			GLuint	width;											// Image Width
			GLuint	height;											// Image Height
			GLuint	id;											// Texture ID Used To Select A Texture
			GLuint	type;											// Image Type (GL_RGB, GL_RGBA)
			char name[256];
		} Texture;	

	private:
		typedef struct  {
			GLubyte Header[12];									// TGA File Header
		} TGAHeader;

		typedef struct  {
			GLubyte	header[6];									// First 6 Useful Bytes From The Header
			GLuint	bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
			GLuint	imageSize;									// Used To Store The Image Size When Setting Aside Ram
			GLuint	temp;										// Temporary Variable
			GLuint	type;	
			GLuint	Height;										// Height of Image
			GLuint	Width;										// Width ofImage
			GLuint	Bpp;										// Bits Per Pixel
		} TGAINFO;

		TGAHeader tgaheader;									// TGA header
		TGAINFO info;												// TGA image data
		static GLubyte uTGAcompare[12];							// Uncompressed TGA Header
		static GLubyte cTGAcompare[12];							// Compressed TGA Header
		bool LoadUncompressedTGA(Texture *, FILE *);			// Load an Uncompressed file
		bool LoadCompressedTGA(Texture *, FILE *);				// Load a Compressed file
		char fname[MAX_PATH];
		Logger *logg;
        unsigned char depth;									// from the old tga class
        short int w, h;											// from the old tga class
        unsigned char *data;									// from the old tga class
		void init(void);

	public:
		TGA(const char *_fname, Logger *_logg=NULL);
        TGA(Logger *logg=NULL);								// from the old tga class
        virtual ~TGA();
		int load(Texture *);
        int load(const char *_fname);							// from the old tga class
        inline int getw(void)  { return w; }					// from the old tga class
        inline int geth(void)  { return h; }					// from the old tga class
        inline unsigned char * get_data(void)  { return data; }	// from the old tga class

};

#endif






