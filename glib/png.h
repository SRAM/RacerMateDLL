#ifndef PNG_IS_INCLUDED
#define PNG_IS_INCLUDED

#ifndef PNG_RESULT_IS_DEFINED
	#define PNG_ERR 0
	#define PNG_OK 1
#endif

/***************************************************************************************

***************************************************************************************/

class PngHeader {
	public:
		unsigned int width,height;
		unsigned int bitDepth,colorType;
		unsigned int compressionMethod,filterMethod,interlaceMethod;

		void decode(unsigned char dat[]);
};

/***************************************************************************************

***************************************************************************************/

class PngPalette {
	public:
		unsigned int nEntry;
		unsigned char *entry;

		PngPalette();
		~PngPalette();
		int decode(unsigned length,unsigned char dat[]);
};

/***************************************************************************************

***************************************************************************************/

class PngTransparency {
	public:
		unsigned int col[3];

		// For color type 3, up to three transparent colors is supported.
		int decode(unsigned length,unsigned char dat[],unsigned int colorType);
};

/***************************************************************************************

***************************************************************************************/

class GenericPngDecoder {
	public:
		enum  {
			gamma_default=100000
		};

		PngHeader hdr;
		PngPalette plt;
		PngTransparency trns;
		unsigned int gamma;

		GenericPngDecoder();
		void Initialize(void);
		int CheckSignature(FILE *fp);
		int ReadChunk(unsigned &length,unsigned char *&buf,unsigned &chunkType,unsigned &crc,FILE *fp);
		int decode(const char fn[]);

		virtual int PrepareOutput(void);
		virtual int Output(unsigned char dat);
		virtual int EndOutput(void);
};

/***************************************************************************************

***************************************************************************************/

class PNG : public GenericPngDecoder {
	public:
		PNG();
		~PNG();
		int w;
                int h;
		unsigned char *rgba;  // Raw data of R,G,B,A
		int autoDeleteRgbaBuffer;
		int filter,x,y,firstByte;
		int inLineCount;
		int inPixelCount;
		unsigned int r,g,b,msb;  // msb for reading 16 bit depth
		unsigned int index;
		unsigned int interlacePass;
		// For filtering
		unsigned char *twoLineBuf8,*curLine8,*prvLine8;
		void ShiftTwoLineBuf(void);
		virtual int PrepareOutput(void);
		virtual int Output(unsigned char dat);
		virtual int EndOutput(void);
		void flip(void);				// For drawing in OpenGL
                void dump(void);
};



#endif
