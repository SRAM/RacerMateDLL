
#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#else
	#include <string.h>
	#include <linux_utils.h>
#endif

/********************************************************************************
Name:		TGA.cpp																*
/Header:	tga.h																*
/Purpose:	Load Compressed and Uncompressed TGA files							*
/Functions:	LoadTGA(Texture * texture, char * filename)							*
/			LoadCompressedTGA(Texture * texture, char * filename, FILE * stream)	*
/			LoadUncompressedTGA(Texture * texture, char * filename, FILE * stream)*	
*******************************************************************************/

#include <fatalerror.h>
#include "tga.h"

GLubyte TGA::uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
GLubyte TGA::cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header

/***********************************************************************************
	constructor 1
***********************************************************************************/

TGA::TGA(const char *_fname, Logger *_logg)  {
	strncpy(fname, _fname, sizeof(fname)-1);
	logg = _logg;
	init();
	return;
}

/***********************************************************************************
	constructor 2
***********************************************************************************/

TGA::TGA(Logger *_logg)  {
	logg = _logg;
	init();
	return;
}

/***********************************************************************************
	destructor
***********************************************************************************/

TGA::~TGA()  {
	if (data)  {
		//delete[] data;
		free(data);
		data = NULL;
	}

	return;
}

/********************************************************************************

********************************************************************************/

int TGA::load(Texture * texture)  {
	FILE *stream;

	stream = fopen(fname, "rb");

	if(stream==NULL)  {
		MessageBox(NULL, "Could not open texture file", "ERROR", MB_OK);
		return 1;
	}

	if(fread(&tgaheader, sizeof(TGAHeader), 1, stream) == 0)  {
		MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);
		fclose(stream);
		return 2;
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)  {
		LoadUncompressedTGA(texture, stream);
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)  {
		LoadCompressedTGA(texture, stream);
	}
	else  {
		MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK);
		fclose(stream);
		return 3;
	}

	return 0;
}

/*********************************************************************************

*********************************************************************************/

bool TGA::LoadUncompressedTGA(Texture * texture, FILE * stream)  {

	if(fread(info.header, sizeof(info.header), 1, stream) == 0)  {										
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);
		if(stream != NULL)  {
			fclose(stream);
		}
		return false;
	}	

	texture->width  = info.header[1] * 256 + info.header[0];
	texture->height = info.header[3] * 256 + info.header[2];
	texture->bpp	= info.header[4];
	info.Width		= texture->width;
	info.Height		= texture->height;
	info.Bpp			= texture->bpp;

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))  {
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);
		if(stream)  {
			fclose(stream);
		}
		return false;
	}

	if(texture->bpp == 24)
		texture->type	= GL_RGB;
	else
		texture->type	= GL_RGBA;

	info.bytesPerPixel	= (info.Bpp / 8);
	info.imageSize		= (info.bytesPerPixel * info.Width * info.Height);
	texture->imageData	= (GLubyte *)malloc(info.imageSize);

	if(texture->imageData == NULL)  {
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);
		fclose(stream);
		return false;
	}

	if(fread(texture->imageData, 1, info.imageSize, stream) != info.imageSize)  {
		MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);
		if(texture->imageData != NULL)  {
			free(texture->imageData);
		}
		fclose(stream);
		return false;
	}

	for(GLuint cswap = 0; cswap < (int)info.imageSize; cswap += info.bytesPerPixel)  {
		texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
		texture->imageData[cswap] ^= texture->imageData[cswap+2];
	}

	fclose(stream);
	return true;
}

/*********************************************************************************

*********************************************************************************/

bool TGA::LoadCompressedTGA(Texture * texture, FILE * stream)  { 
	if(fread(info.header, sizeof(info.header), 1, stream) == 0)  {
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);
		if(stream)  {
			fclose(stream);
		}
		return false;
	}

	texture->width  = info.header[1] * 256 + info.header[0];
	texture->height = info.header[3] * 256 + info.header[2];
	texture->bpp	= info.header[4];
	info.Width		= texture->width;
	info.Height		= texture->height;
	info.Bpp			= texture->bpp;

	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))  {
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	// If it isnt...Display error
		if(stream)  {
			fclose(stream);
		}
		return false;
	}

	if(texture->bpp == 24)
		texture->type = GL_RGB;
	else
		texture->type = GL_RGBA;

	info.bytesPerPixel = (info.Bpp / 8);
	info.imageSize = (info.bytesPerPixel * info.Width * info.Height);
	texture->imageData = (GLubyte *)malloc(info.imageSize);

	if(texture->imageData == NULL)  {
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);
		fclose(stream);
		return false;
	}

	GLuint pixelcount	= info.Height * info.Width;
	GLuint currentpixel	= 0;
	GLuint currentbyte	= 0;
	GLubyte * colorbuffer = (GLubyte *)malloc(info.bytesPerPixel);

	do
	{
		GLubyte chunkheader = 0;

		if(fread(&chunkheader, sizeof(GLubyte), 1, stream) == 0)
		{
			MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);
			if(stream != NULL)
			{
				fclose(stream);
			}
			if(texture->imageData != NULL)
			{
				free(texture->imageData);
			}
			return false;
		}

		if(chunkheader < 128)  {																// If the ehader is < 128, it means the that is the number of RAW color packets minus 1 that follow the header
			chunkheader++;																		// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++)  {							// Read RAW color values
				if(fread(colorbuffer, 1, info.bytesPerPixel, stream) != info.bytesPerPixel)  {	// Try to read 1 pixel
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);
					goto abort;
				}
				texture->imageData[currentbyte] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
				texture->imageData[currentbyte + 1	] = colorbuffer[1];
				texture->imageData[currentbyte + 2	] = colorbuffer[0];

				if(info.bytesPerPixel == 4)  {												// if its a 32 bpp image
					texture->imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
				}

				currentbyte += info.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if(currentpixel > pixelcount)  {											// Make sure we havent read too many pixels
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!
					goto abort;
				}
			}
		}
		else  {																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, info.bytesPerPixel, stream) != info.bytesPerPixel)  {		// Attempt to read following color values
				MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);
				goto abort;
			}

			for(short counter = 0; counter < chunkheader; counter++)  {					// copy the color into the image data as many times as dictated by the header
				texture->imageData[currentbyte] = colorbuffer[2];						// switch R and B bytes areound while copying
				texture->imageData[currentbyte + 1] = colorbuffer[1];
				texture->imageData[currentbyte + 2] = colorbuffer[0];

				if(info.bytesPerPixel == 4)  {											// If TGA images is 32 bpp
					texture->imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
				}

				currentbyte += info.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if(currentpixel > pixelcount)  {											// Make sure we havent written too many pixels
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);				// if there is too many... Display an error!
					goto abort;
				}
			}
		}
	}
	while(currentpixel < pixelcount);													// Loop while there are still pixels left

	fclose(stream);																		// Close the file
	return true;


abort:
	if(stream)  {
		fclose(stream);
	}
	if(colorbuffer != NULL)  {
		free(colorbuffer);
	}
	if(texture->imageData != NULL)  {
		free(texture->imageData);
	}
	return false;
}

/*********************************************************************************

*********************************************************************************/

void TGA::init(void)  {
	w = 0;
	h = 0;
	depth = 0;
	data = NULL;
	return;
}

/***********************************************************************************

***********************************************************************************/

int TGA::load(const char *_fname)  {
	FILE *fh = NULL;
	int md, t;

	// Allocate memory for the info structure

	//tga = malloc(sizeof(tga_data_t));

//	char buf[256];
//	GetCurrentDirectory(256, buf);

	fh = fopen(_fname, "rb");
	if (fh == NULL) {
		fprintf(stderr, "Error: problem opening TGA file (%s).\n", _fname);
		return 1;
	}


	// Load information about the tga, aka the header

	fseek(fh, 12, SEEK_SET);
	fread(&w, sizeof(short int), 1, fh);

	fseek(fh, 14, SEEK_SET);
	fread(&h, sizeof(short int), 1, fh);

	fseek(fh, 16, SEEK_SET);
	fread(&depth, sizeof(short int), 1, fh);

	// Mode = components per pixel.
	md = depth / 8;

	// Total bytes = h * w * md.
	t = h * w * md;

	if (logg)  {
		logg->write(10,0,1,"tga file = %s\n", _fname);
		logg->write(10,0,1,"h = %d\n", h);
		logg->write(10,0,1,"w = %d\n", w);
		logg->write(10,0,1,"md = %d\n", md);
		logg->write(10,0,1,"t = %d\n", t);
	}

	//dprintf("Reading %d bytes.\n", t);
	if (data)  {
		//delete[] data;
		free(data);
		data = NULL;
	}


	data = (unsigned char *)malloc(t);
	if (data==NULL)  {
		throw (fatalError(__FILE__, __LINE__, "Out Of Memory"));
	}
	//logg

	// Seek to the image data.
	fseek(fh, 18, SEEK_SET);
	fread(data, sizeof(unsigned char), t, fh);

	fclose(fh);

	// Mode 3 = RGB, Mode 4 = RGBA
	// TGA stores RGB(A) as BGR(A) so
	// we need to swap red and blue

	if (md >= 3) {
		unsigned char c;

		for (int i = 0; i < t; i+= md) {
			c = data[i];
			data[i] = data[i + 2];
			data[i + 2] = c;
		}
	}

	//dprintf("Loaded texture -> (%s)\nWidth: %d\nHeight: %d\nDepth: %d\n", _fname, w, h, depth);
	return 0;
}

