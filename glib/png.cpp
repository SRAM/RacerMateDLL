#include <stdio.h>

#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#include <utils.h>
#include "png.h"										// avoid ffmpeg's version

/* Supported color and depth

	Non-Interlaced
	1bit Grayscale
	4bit Indexed Color
	8bit Grayscale
	8bit Grayscale with Alpha
	8bit Indexed Color
	8bit True Color (24bit per pixel)
	8bit True Color with Alpha (32bit per pixel)
	16bit True Color (48bit per pixel)

	Interlaced
	8bit Grayscale
	8bit Grayscale with Alpha
	8bit Indexed Color
	8bit True color (24bit per pixel)
	8bit True Color with Alpha (32bit per pixel)
	16bit True Color (48bit per pixel)

 */

//   PNG Data Format http://www.w3.org/TR/PNG

#define MakeDword(a,b,c,d) ((a)*0x1000000+(b)*0x10000+(c)*0x100+(d))

#define IHDR MakeDword('I','H','D','R')
#define IDAT MakeDword('I','D','A','T')
#define PLTE MakeDword('P','L','T','E')
#define IEND MakeDword('I','E','N','D')
#define pH MakeDword('p','H','y','s')
#define tRNS MakeDword('t','R','N','S')
#define gAMA MakeDword('g','A','M','A')

static inline unsigned int PngGetUnsignedInt(const unsigned char dat[4]) {
	return (unsigned)dat[3]+(unsigned)dat[2]*0x100+(unsigned)dat[1]*0x10000+(unsigned)dat[0]*0x1000000;
}

/***************************************************************************************

***************************************************************************************/

void PngHeader::decode(unsigned char dat[]) {
	width=PngGetUnsignedInt(dat);
	height=PngGetUnsignedInt(dat+4);
	bitDepth=dat[8];
	colorType=dat[9];
	compressionMethod=dat[10];
	filterMethod=dat[11];
	interlaceMethod=dat[12];

	dprintf("Width=%d Height=%d\n", width, height);
	dprintf("bitDepth=%d\n", bitDepth);
	dprintf("colorType=%d\n", colorType);
	dprintf("compressionMethod=%d\n", compressionMethod);
	dprintf("filterMethod=%d\n", filterMethod);
	dprintf("interlaceMethod=%d\n", interlaceMethod);
}

/***************************************************************************************

***************************************************************************************/

PngPalette::PngPalette() {
	nEntry=0;
	entry=NULL;
}

/***************************************************************************************

***************************************************************************************/

PngPalette::~PngPalette() {
	if(entry!=NULL) {
		delete [] entry;
	}
}

/***************************************************************************************

***************************************************************************************/

int PngPalette::decode(unsigned length,unsigned char dat[]) {
	if(length%3!=0) {
		return PNG_ERR;
	}

	if(entry!=NULL) {
		delete [] entry;
		nEntry=0;
		entry=NULL;
	}

	if(length>0) {
		entry=new unsigned char [length];
		if(entry!=NULL) {
			unsigned int i;
			nEntry=length/3;
			dprintf("%d palette entries\n",nEntry);
			for(i=0; i<length; i++) {
				entry[i]=dat[i];
			}
		}
	}

	return PNG_OK;
}

/***************************************************************************************

***************************************************************************************/

int PngTransparency::decode(unsigned int length,unsigned char dat[],unsigned int colorType) {
	
	/*
	unsigned int i;
	switch(colorType) {
		case 0:
			if(length>=2) {
				col[0]=(unsigned int)dat[0]*256+(unsigned int)dat[1];
				return PNG_OK;
			}
			break;
		case 2:
			if(length>=6) {
				col[0]=(unsigned int)dat[0]*256+(unsigned int)dat[1];
				col[2]=(unsigned int)dat[2]*256+(unsigned int)dat[3];
				col[3]=(unsigned int)dat[4]*256+(unsigned int)dat[5];
				return PNG_OK;
			}
			break;
		case 3:
			for(i=0; i<3 && i<length; i++) {
				col[i]=dat[i];
			}
			return PNG_OK;
	}
	*/
	
	return PNG_ERR;
}

/***************************************************************************************

***************************************************************************************/

GenericPngDecoder::GenericPngDecoder() {
	Initialize();
}

/***************************************************************************************

***************************************************************************************/

void GenericPngDecoder::Initialize(void) {
	gamma=gamma_default;
	trns.col[0]=0x7fffffff;
	trns.col[1]=0x7fffffff;
	trns.col[2]=0x7fffffff;
}

/***************************************************************************************

***************************************************************************************/

int GenericPngDecoder::CheckSignature(FILE *fp) {
	unsigned char buf[8];
	fread(buf,1,8,fp);
	if(buf[0]==0x89 && buf[1]==0x50 && buf[2]==0x4e && buf[3]==0x47 && buf[4]==0x0d && buf[5]==0x0a && buf[6]==0x1a && buf[7]==0x0a)  {
		return PNG_OK;
	}
	return PNG_ERR;
}

/***************************************************************************************

***************************************************************************************/

int GenericPngDecoder::ReadChunk(unsigned &length,unsigned char *&buf,unsigned &chunkType,unsigned &crc,FILE *fp) {
	unsigned char dwBuf[4];

	if(fread(dwBuf,1,4,fp)<4) {
		return PNG_ERR;
	}
	length=PngGetUnsignedInt(dwBuf);

	if(fread(dwBuf,1,4,fp)<4) {
		return PNG_ERR;
	}
	chunkType=PngGetUnsignedInt(dwBuf);

	dprintf("Chunk name=%c%c%c%c\n",dwBuf[0],dwBuf[1],dwBuf[2],dwBuf[3]);

	if(length>0) {
		buf=new unsigned char [length];
		if(fread(buf,1,length,fp)<length) {
			return PNG_ERR;
		}
	}
	else {
		buf=NULL;
	}

	if(fread(dwBuf,1,4,fp)<4) {
		return PNG_ERR;
	}
	crc=PngGetUnsignedInt(dwBuf);

	return PNG_OK;
}

/***************************************************************************************

***************************************************************************************/

class PngHuffmanTree {
	public:
		PngHuffmanTree();
		~PngHuffmanTree();
		PngHuffmanTree *zero,*one;
		unsigned dat;
		static int leakTracker;
};

int PngHuffmanTree::leakTracker = 0;

/***************************************************************************************

***************************************************************************************/

PngHuffmanTree::PngHuffmanTree() {
	zero=NULL;
	one=NULL;
	dat=0x7fffffff;
	leakTracker++;
}

/***************************************************************************************

***************************************************************************************/

PngHuffmanTree::~PngHuffmanTree() {
	leakTracker--;
}

/***************************************************************************************

***************************************************************************************/

class PngUncompressor {
	public:
		GenericPngDecoder *output;

		inline unsigned int GetNextBit(const unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr) {
			unsigned a;
			a=dat[bytePtr]&bitPtr;
			bitPtr<<=1;
			if(bitPtr>=256)
			{
				bitPtr=1;
				bytePtr++;
			}
			return (a!=0 ? 1 : 0);
		}
		inline unsigned int GetNextMultiBit(const unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr,unsigned n) {
			unsigned value,mask,i;
			value=0;
			mask=1;
			for(i=0; i<n; i++) {
				if(GetNextBit(dat,bytePtr,bitPtr)) {
					value|=mask;
				}
				mask<<=1;
			}
			return value;
		}

		void MakeFixedHuffmanCode(unsigned hLength[288],unsigned hCode[288]);
		void MakeDynamicHuffmanCode(unsigned hLength[288],unsigned hCode[288],unsigned nLng,unsigned lng[]);
		int decodeDynamicHuffmanCode
			(unsigned int &hLit,unsigned int &hDist,unsigned int &hCLen,
			 unsigned int *&hLengthLiteral,unsigned int *&hCodeLiteral,
			 unsigned int *&hLengthDist,unsigned int *&hCodeDist,
			 unsigned int hLengthBuf[322],unsigned int hCodeBuf[322],
			 const unsigned char dat[],unsigned int &bytePtr,unsigned int &bitPtr);

		PngHuffmanTree *MakeHuffmanTree(unsigned n,unsigned hLength[],unsigned hCode[]);
		void DeleteHuffmanTree(PngHuffmanTree *node);

		unsigned GetCopyLength(unsigned value,unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr);
		unsigned GetBackwardDistance(unsigned distCode,unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr);

		int Uncompress(unsigned length,unsigned char dat[]);
};

/***************************************************************************************

***************************************************************************************/

void PngUncompressor::MakeFixedHuffmanCode(unsigned hLength[288],unsigned hCode[288]) {
	unsigned i;
	for(i=0; i<=143; i++) {
		hLength[i]=8;
		hCode[i]=0x30+i;
	}
	for(i=144; i<=255; i++) {
		hLength[i]=9;
		hCode[i]=0x190+(i-144);
	}
	for(i=256; i<=279; i++) {
		hLength[i]=7;
		hCode[i]=i-256;
	}
	for(i=280; i<=287; i++) {
		hLength[i]=8;
		hCode[i]=0xc0+(i-280);
	}
}

/***************************************************************************************

***************************************************************************************/

void PngUncompressor::MakeDynamicHuffmanCode(unsigned hLength[],unsigned hCode[],unsigned nLng,unsigned lng[]) {
	unsigned i,maxLng,code,*bl_count,*next_code,bits,n;

	for(i=0; i<nLng; i++) {
		hLength[i]=lng[i];
		hCode[i]=0;
	}

	maxLng=0;
	for(i=0; i<nLng; i++) {
		if(maxLng<lng[i]) {
			maxLng=lng[i];
		}
	}

	bl_count=new unsigned [maxLng+1];
	next_code=new unsigned [maxLng+1];
	for(i=0; i<maxLng+1; i++) {
		bl_count[i]=0;
		next_code[i]=0;
	}
	for(i=0; i<nLng; i++) {
		bl_count[lng[i]]++;
	}

	// for(i=0; i<maxLng+1; i++)
	// {
	// 	dprintf("bl_count[%d]=%d\n",i,bl_count[i]);
	// }

	// See RFC1951 Specification
	code=0;
	bl_count[0]=0;
	for(bits=1; bits<=maxLng; bits++) {
		code=(code+bl_count[bits-1])<<1;
		next_code[bits]=code;
	}

	for(n=0; n<nLng; n++) {
		unsigned len;
		len=lng[n];
		if(len>0)
		{
			hCode[n]=next_code[len]++;
		}
	}

	if(bl_count!=NULL) {
		delete [] bl_count;
	} if(next_code!=NULL) {
		delete [] next_code;
	}
}

/***************************************************************************************

***************************************************************************************/

int PngUncompressor::decodeDynamicHuffmanCode
	(unsigned int &hLit,unsigned int &hDist,unsigned int &hCLen,
	unsigned int *&hLengthLiteral,unsigned int *&hCodeLiteral,
	unsigned int *&hLengthDist,unsigned int *&hCodeDist,
	unsigned int hLengthBuf[322],unsigned int hCodeBuf[322],
	const unsigned char dat[],unsigned int &bytePtr,unsigned int &bitPtr)  {

	unsigned int i;
	hLit=0;
	hDist=0;
	hCLen=0;

	hLit=GetNextMultiBit(dat,bytePtr,bitPtr,5);
	hDist=GetNextMultiBit(dat,bytePtr,bitPtr,5);
	hCLen=GetNextMultiBit(dat,bytePtr,bitPtr,4);

	dprintf("hLit=%d hDist=%d hCLen=%d\n",hLit,hDist,hCLen);

	const unsigned int codeLengthLen=19;
	unsigned codeLengthOrder[codeLengthLen]=
	{
		16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
	};
	unsigned codeLengthCode[codeLengthLen];
	for(i=0; i<codeLengthLen; i++)
	{
		codeLengthCode[i]=0;
	}
	for(i=0; i<hCLen+4; i++)
	{
		codeLengthCode[codeLengthOrder[i]]=GetNextMultiBit(dat,bytePtr,bitPtr,3);
		//dprintf("Code length code[%d]=%d (for %d)\n",
		//     codeLengthOrder[i],codeLengthCode[i],codeLengthOrder[i]);
	}


	unsigned hLengthCode[codeLengthLen],hCodeCode[codeLengthLen];
	MakeDynamicHuffmanCode(hLengthCode,hCodeCode,codeLengthLen,codeLengthCode);

	// for(i=0; i<codeLengthLen; i++)
	// {
	// 	dprintf("[%d] %08x %08xx\n",i,hLengthCode[i],hCodeCode[i]);
	// }

	hLengthLiteral=hLengthBuf;
	hCodeLiteral=hCodeBuf;
	hLengthDist=hLengthBuf+hLit+257;
	hCodeDist=hCodeBuf+hLit+257;

	PngHuffmanTree *lengthTree,*lengthTreePtr;
	lengthTree=MakeHuffmanTree(codeLengthLen,hLengthCode,hCodeCode);

	unsigned int nExtr;
	nExtr=0;
	lengthTreePtr=lengthTree;
	while(nExtr<hLit+257+hDist+1)
	{
		if(GetNextBit(dat,bytePtr,bitPtr))
		{
			lengthTreePtr=lengthTreePtr->one;
		}
		else
		{
			lengthTreePtr=lengthTreePtr->zero;
		}
		if(lengthTreePtr->zero==NULL && lengthTreePtr->one==NULL)
		{
			unsigned value,copyLength;
			value=lengthTreePtr->dat;

			// dprintf("Value=%d\n",value);

			if(value<=15)
			{
				hLengthBuf[nExtr++]=value;
			}
			else if(value==16)
			{
				copyLength=3+GetNextMultiBit(dat,bytePtr,bitPtr,2);
				// dprintf("copyLength=%d\n",copyLength);
				while(copyLength>0)
				{
					hLengthBuf[nExtr]=hLengthBuf[nExtr-1];
					nExtr++;
					copyLength--;
				}
			}
			else if(value==17)
			{
				copyLength=3+GetNextMultiBit(dat,bytePtr,bitPtr,3);
				// dprintf("copyLength=%d\n",copyLength);
				while(copyLength>0)
				{
					hLengthBuf[nExtr++]=0;
					copyLength--;
				}
			}
			else if(value==18)
			{
				copyLength=11+GetNextMultiBit(dat,bytePtr,bitPtr,7);
				// dprintf("copyLength=%d\n",copyLength);
				while(copyLength>0)
				{
					hLengthBuf[nExtr++]=0;
					copyLength--;
				}
			}

			lengthTreePtr=lengthTree;

			// dprintf("nExtr=%d/%d\n",nExtr,hLit+257+hDist+1);
		}
	}

	// for(i=0; i<hLit+257; i++)
	// {
	// 	dprintf("Literal [%d]  Length %d\n",i,hLengthLiteral[i]);
	// }
	// for(i=0; i<hDist+1; i++)
	// {
	// 	dprintf("Dist [%d] Length %d\n",i,hLengthDist[i]);
	// }

	dprintf("Making Huffman Code from Code Lengths\n");
	MakeDynamicHuffmanCode(hLengthLiteral,hCodeLiteral,hLit+257,hLengthLiteral);
	MakeDynamicHuffmanCode(hLengthDist,hCodeDist,hDist+1,hLengthDist);

	DeleteHuffmanTree(lengthTree);

	return PNG_OK;
}

/***************************************************************************************

***************************************************************************************/

PngHuffmanTree *PngUncompressor::MakeHuffmanTree(unsigned n,unsigned hLength[],unsigned hCode[])  {
	unsigned i,j,mask;
	PngHuffmanTree *root,*ptr;
	root=new PngHuffmanTree;

	for(i=0; i<n; i++)  {
		if(hLength[i]>0)
		{
			ptr=root;
			mask=(1<<(hLength[i]-1));
			for(j=0; j<hLength[i]; j++)
			{
				if(hCode[i]&mask)
				{
					if(ptr->one==NULL)
					{
						ptr->one=new PngHuffmanTree;
					}
					ptr=ptr->one;
				}
				else
				{
					if(ptr->zero==NULL)
					{

						ptr->zero=new PngHuffmanTree;
					}
					ptr=ptr->zero;
				}
				mask>>=1;
			}
			ptr->dat=i;
		}
	}

	return root;
}

/***************************************************************************************

***************************************************************************************/

void PngUncompressor::DeleteHuffmanTree(PngHuffmanTree *node)
{
	if(node!=NULL)
	{
		DeleteHuffmanTree(node->zero);
		DeleteHuffmanTree(node->one);
		delete node;
	}
}

/***************************************************************************************

***************************************************************************************/

unsigned PngUncompressor::GetCopyLength(unsigned value,unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr)
{
	unsigned copyLength;

	if(value<=264)
	{
		copyLength=3+(value-257);
	}
	else if(value>=285)
	{
		copyLength=258;
	}
	else
	{
		unsigned base,offset,extBits;
		extBits=1+(value-265)/4;
		base=(8<<((value-265)/4))+3;
		offset=((value-265)&3)*(2<<((value-265)/4));

		copyLength=GetNextMultiBit(dat,bytePtr,bitPtr,extBits);
		copyLength+=base+offset;
	}

	return copyLength;
}

/***************************************************************************************

***************************************************************************************/

unsigned PngUncompressor::GetBackwardDistance(unsigned distCode,unsigned char dat[],unsigned &bytePtr,unsigned &bitPtr)  {
	unsigned backDist;

	if(distCode<=3)
	{
		backDist=distCode+1;
	}
	else
	{
		unsigned base,offset,extBits;

		base=(4<<((distCode-4)/2))+1;
		offset=(distCode&1)*(2<<((distCode-4)/2));
		extBits=(distCode-2)/2;

		backDist=GetNextMultiBit(dat,bytePtr,bitPtr,extBits);
		backDist+=base+offset;
	}

	return backDist;
}

/***************************************************************************************

***************************************************************************************/


int PngUncompressor::Uncompress(unsigned length,unsigned char dat[])
{
	unsigned windowUsed;
	unsigned char *windowBuf;
	unsigned nByteExtracted;

	windowBuf=NULL;

	unsigned bytePtr,bitPtr;
	bytePtr=0;
	bitPtr=1;
	nByteExtracted=0;

	PngHuffmanTree *codeTree,*codeTreePtr;
	PngHuffmanTree *distTree,*distTreePtr;
	codeTree=NULL;
	distTree=NULL;

	dprintf("Begin zLib block length=%d bytePtr=%d bitPtr=0x%02x\n",length,bytePtr,bitPtr);

	unsigned char cmf=0, flg;
	cmf = dat[bytePtr++];
	flg=dat[bytePtr++];

	unsigned cm=0, cInfo, windowSize;
	cm = cmf & 0x0f;
	if(cm!=8)
	{
		dprintf("Unsupported compression method! (%d)\n",cm);
		goto ERREND;
	}

	cInfo=(cmf&0xf0)>>4;
	windowSize=1<<(cInfo+8);
	dprintf("cInfo=%d, Window Size=%d\n",cInfo,windowSize);

	windowBuf=new unsigned char [windowSize];
	windowUsed=0;



	unsigned fCheck,fDict,fLevel;
	fCheck=(flg&15);
	fDict=(flg&32)>>5;
	fLevel=(flg&192)>>6;
	dprintf("fCheck=%d fDict=%d fLevel=%d\n",fCheck,fDict,fLevel);



	if(fDict!=0)
	{
		dprintf("PNG is not supposed to have a preset dictionary.\n");
		goto ERREND;
	}



	while(1)
	{
		unsigned bFinal,bType;

		bFinal=GetNextBit(dat,bytePtr,bitPtr);
		bType=GetNextMultiBit(dat,bytePtr,bitPtr,2);

		if(bytePtr>=length)
		{
			dprintf("Buffer overflow\n");
			goto ERREND;
		}

		dprintf("bFinal=%d bType=%d\n",bFinal,bType);

		if(bType==0) // No Compression
		{
			unsigned len;
			if(bitPtr!=0)
			{
				bitPtr=0;
				bytePtr++;
			}
			if(bytePtr>=length)
			{
				dprintf("Buffer overflow\n");
				goto ERREND;
			}

			len=dat[bytePtr]*256+dat[bytePtr+1];
			bytePtr+=4;

			// Feed len bytes

			bytePtr+=len;
		}
		else if(bType==1 || bType==2)
		{
			codeTree=NULL;

			if(bType==1)
			{
				unsigned hLength[288],hCode[288];
				MakeFixedHuffmanCode(hLength,hCode);
				codeTree=MakeHuffmanTree(288,hLength,hCode);
				distTree=NULL;
			}
			else
			{
				unsigned hLit,hDist,hCLen;
				unsigned *hLengthLiteral,*hCodeLiteral;
				unsigned *hLengthDist,*hCodeDist;
				unsigned hLengthBuf[322],hCodeBuf[322];

				decodeDynamicHuffmanCode
					(hLit,hDist,hCLen,
					 hLengthLiteral,hCodeLiteral,hLengthDist,hCodeDist,hLengthBuf,hCodeBuf,
					 dat,bytePtr,bitPtr);

				dprintf("Making Huffman Tree\n");
				codeTree=MakeHuffmanTree(hLit+257,hLengthLiteral,hCodeLiteral);
				distTree=MakeHuffmanTree(hDist+1,hLengthDist,hCodeDist);
			}


			dprintf("Huffman table paprared\n");


			codeTreePtr=codeTree;
			if(codeTree!=NULL)
			{
				while(1)
				{
					if(GetNextBit(dat,bytePtr,bitPtr))
					{
						codeTreePtr=codeTreePtr->one;
					}
					else
					{
						codeTreePtr=codeTreePtr->zero;
					}

					if(codeTreePtr==NULL)
					{
						dprintf("Huffman Decompression: Reached NULL node.\n");
						goto ERREND;
					}

					if(codeTreePtr->zero==NULL && codeTreePtr->one==NULL)
					{
						//dprintf("[%d]\n",codeTreePtr->dat);

						unsigned value;
						value=codeTreePtr->dat;
						if(value<256)
						{
							windowBuf[windowUsed++]=value;
							windowUsed&=(windowSize-1);
							if(output->Output(value)!=PNG_OK)
							{
								goto ERREND;
							}
							nByteExtracted++;
						}
						else if(value==256)
						{
							break;
						}
						else if(value<=285)
						{
							unsigned copyLength,distCode,backDist;
							copyLength=GetCopyLength(value,dat,bytePtr,bitPtr);
							//dprintf("CopyLength %d\n",copyLength);

							if(bType==1)
							{
								distCode=16*GetNextBit(dat,bytePtr,bitPtr);  // 5 bits fixed
								distCode+=8*GetNextBit(dat,bytePtr,bitPtr);  // Reversed order
								distCode+=4*GetNextBit(dat,bytePtr,bitPtr);
								distCode+=2*GetNextBit(dat,bytePtr,bitPtr);
								distCode+=  GetNextBit(dat,bytePtr,bitPtr);
							}
							else
							{
								distTreePtr=distTree;
								while(distTreePtr->zero!=NULL || distTreePtr->one!=NULL)
								{
									if(GetNextBit(dat,bytePtr,bitPtr))
									{
										distTreePtr=distTreePtr->one;
									}
									else
									{
										distTreePtr=distTreePtr->zero;
									}
								}
								distCode=distTreePtr->dat;
							}
							backDist=GetBackwardDistance(distCode,dat,bytePtr,bitPtr);
							//dprintf("DistCode %d BackDist %d\n",distCode,backDist);


							unsigned i;
							for(i=0; i<copyLength; i++)
							{
								unsigned char dat;
								dat=windowBuf[(windowUsed-backDist)&(windowSize-1)];
								if(output->Output(dat)!=PNG_OK)
								{
									goto ERREND;
								}
								nByteExtracted++;
								windowBuf[windowUsed++]=dat;
								windowUsed&=(windowSize-1);
							}
						}

						codeTreePtr=codeTree;
					}

					if(length<=bytePtr)
					{
						goto ERREND;
					}
				}
			}


			DeleteHuffmanTree(codeTree);
			DeleteHuffmanTree(distTree);
			codeTree=NULL;
			distTree=NULL;
		}
		else
		{
			dprintf("Unknown compression type (bType=3)\n");
			goto ERREND;
		}


		if(bFinal!=0)
		{
			break;
		}
	}

	delete [] windowBuf;
	windowBuf=NULL;


	dprintf("End zLib block length=%d bytePtr=%d bitPtr=0x%02x\n",length,bytePtr,bitPtr);
	dprintf("Huffman Tree Leak Tracker = %d\n",PngHuffmanTree::leakTracker);
	dprintf("Output %d bytes.\n",nByteExtracted);

	return PNG_OK;

ERREND:
	if(windowBuf!=NULL)
	{
		delete [] windowBuf;
	}
	if(codeTree!=NULL)
	{
		DeleteHuffmanTree(codeTree);
	}
	if(distTree!=NULL)
	{
		DeleteHuffmanTree(distTree);
	}
	return PNG_ERR;
}




/***************************************************************************************

***************************************************************************************/
//#include <windows.h>

int GenericPngDecoder::decode(const char fn[])
{
	FILE *fp;
	unsigned fileSize;
//char curdir[256];
//GetCurrentDirectory(sizeof(curdir), curdir);

	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		fseek(fp,0,2 /* SEEK_END */);
		fileSize = (unsigned int)ftell(fp);
		fseek(fp,0,0 /* SEEK_SET */);

		if(CheckSignature(fp)!=PNG_OK)
		{
			dprintf("The file does not have PNG signature.\n");
			goto ERREND;
		}

		unsigned datBufUsed;
		unsigned char *datBuf;


		datBufUsed=0;
		datBuf=new unsigned char [fileSize];


		unsigned char *buf;
		unsigned length,chunkType,crc;
		while(ReadChunk(length,buf,chunkType,crc,fp)==PNG_OK && chunkType!=IEND)
		{
			switch(chunkType)
			{
				default:
					if(buf!=NULL)
					{
						delete [] buf;
					}
					break;
				case IHDR:
					if(buf!=NULL)
					{
						if(length>=13)
						{
							hdr.decode(buf);
						}
						delete [] buf;
					}
					break;
				case PLTE:
					if(buf!=NULL)
					{
						if(plt.decode(length,buf)!=PNG_OK)
						{
							delete [] buf;
							goto ERREND;
						}
						delete [] buf;
					}
					break;
				case tRNS:
					if(buf!=NULL)
					{
						trns.decode(length,buf,hdr.colorType);
						delete [] buf;
					}
					break;
				case gAMA:
					if(buf!=NULL && length>=4)
					{
						gamma=PngGetUnsignedInt(buf);
						dprintf("Gamma %d (default=%d)\n",gamma,gamma_default);
						delete [] buf;
					}
					break;
				case IDAT:
					if(buf!=NULL)
					{
						unsigned i;
						for(i=0; i<length; i++)
						{
							datBuf[datBufUsed+i]=buf[i];
						}
						datBufUsed+=length;
						delete [] buf;
					}
			}
		}



		if(PrepareOutput()==PNG_OK)  {				// creates the rgba array
			PngUncompressor uncompressor;
			uncompressor.output=this;
			uncompressor.Uncompress(datBufUsed,datBuf);

			EndOutput();
		}


		delete [] datBuf;
		fclose(fp);
		return PNG_OK;
	}

ERREND:
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return -1;
}

int GenericPngDecoder::PrepareOutput(void)
{
	return PNG_OK;
}

int GenericPngDecoder::Output(unsigned char dat)
{
	return PNG_OK;
}

int GenericPngDecoder::EndOutput(void)
{
	return PNG_OK;
}



/***************************************************************************************

***************************************************************************************/

static inline unsigned char Paeth(unsigned int ua,unsigned int ub,unsigned int uc)
{
	int a,b,c,p,pa,pb,pc;

	a=(int)ua;
	b=(int)ub;
	c=(int)uc;

	p=a+b-c;
	pa=(p>a ? p-a : a-p);
	pb=(p>b ? p-b : b-p);
	pc=(p>c ? p-c : c-p);

	if(pa<=pb && pa<=pc)
	{
		return a;
	}
	else if(pb<=pc)
	{
		return b;
	}
	else
	{
		return c;
	}
}

/***************************************************************************************

***************************************************************************************/

static inline void Filter8(unsigned char curLine[],unsigned char prvLine[],int x,int y,int unitLng,int filter)
{
	int i;
	switch(filter)
	{
		case 1:
			if(x>0)
			{
				for(i=0; i<unitLng; i++)
				{
					curLine[x*unitLng+i]+=curLine[x*unitLng+i-unitLng];
				}
			}
			break;
		case 2:
			if(y>0)
			{
				for(i=0; i<unitLng; i++)
				{
					curLine[x*unitLng+i]+=prvLine[x*unitLng+i];
				}
			}
			break;
		case 3:
			for(i=0; i<unitLng; i++)
			{
				unsigned int a;
				a=(x>0 ? curLine[x*unitLng+i-unitLng] : 0);
				a+=(y>0 ? prvLine[x*unitLng+i] : 0);
				curLine[x*unitLng+i]+=a/2;
			}
			break;
		case 4:
			for(i=0; i<unitLng; i++)
			{
				unsigned int a,b,c;
				a=(x>0 ? curLine[x*unitLng+i-unitLng] : 0);
				b=(y>0 ? prvLine[x*unitLng+i] : 0);
				c=((x>0 && y>0) ? prvLine[x*unitLng-unitLng+i] : 0);
				curLine[x*unitLng+i]+=Paeth(a,b,c);
			}
			break;
	}
}

/***************************************************************************************

***************************************************************************************/

PNG::PNG() {
	w = 0;
	h = 0;
	rgba = NULL;
	twoLineBuf8 = NULL;

	curLine8 = NULL;
	prvLine8 = NULL;

	autoDeleteRgbaBuffer = 1;

	filter = 0;
	x = 0;
	y = 0;
	firstByte = 0;
	inLineCount = 0;
	inPixelCount = 0;
	r = g = b = 0;
	msb = 0;
	index = 0;
	interlacePass = 0;

}

/***************************************************************************************

***************************************************************************************/

PNG::~PNG()  {
	if(autoDeleteRgbaBuffer==1 && rgba!=NULL)
	{
		delete [] rgba;
	}
	if(twoLineBuf8!=NULL)
	{
		delete [] twoLineBuf8;
	}
}

/***************************************************************************************

***************************************************************************************/

void PNG::ShiftTwoLineBuf(void)
{
	if(twoLineBuf8!=NULL)
	{
		unsigned char *swap;
		swap=curLine8;
		curLine8=prvLine8;
		prvLine8=swap;
	}
}

/***************************************************************************************

***************************************************************************************/

int PNG::PrepareOutput(void)
{
	int supported;

	supported=0;
	switch(hdr.colorType)
	{
		case 0:   // Greyscale
			switch(hdr.bitDepth)
			{
				case 8:
				case 1:
					supported=1;
					break;
				case 2:
				case 4:
				case 16:
					break;
			}
			break;
		case 2:   // Truecolor
			switch(hdr.bitDepth)
			{
				case 8:
				case 16:
					supported=1;
					break;
			}
			break;
		case 3:   // Indexed-color
			switch(hdr.bitDepth)
			{
				case 4:
				case 8:
					supported=1;
					break;
				case 1:
				case 2:
					break;
			}
			break;
		case 4:   // Greyscale with alpha
			switch(hdr.bitDepth)
			{
				case 8:
					supported=1;
					break;
				case 16:
					break;
			}
			break;
		case 6:   // Truecolor with alpha
			switch(hdr.bitDepth)
			{
				case 8:
					supported=1;
					break;
				case 16:
					break;
			}
			break;
	}

	if(supported==0)
	{
		dprintf("Unsupported colorType-bitDepth combination.\n");
		return PNG_ERR;
	}



	w = hdr.width;
	h = hdr.height;
	if(autoDeleteRgbaBuffer==1 && rgba!=NULL)  {
		delete [] rgba;
		rgba=NULL;
	}
	rgba=new unsigned char [w*h*4];
	x=-1;
	y=0;
	filter=0;
	inLineCount=0;
	inPixelCount=0;
	firstByte=1;
	index=0;
	interlacePass=1;

	if(twoLineBuf8!=NULL)
	{
		delete [] twoLineBuf8;
		twoLineBuf8=NULL;
	}


	// See PNG Specification 11.2 for Allowed combinations of color type and bit depth
	unsigned int twoLineBufLngPerLine;
	switch(hdr.colorType)
	{
		case 0:   // Greyscale
			switch(hdr.bitDepth)
			{
				case 1:
					twoLineBufLngPerLine=(hdr.width+7)/8;
					break;
				case 2:
					twoLineBufLngPerLine=(hdr.width+3)/4;
					break;
				case 4:
					twoLineBufLngPerLine=(hdr.width+1)/2;
					break;
				case 8:
					twoLineBufLngPerLine=hdr.width;
					break;
				case 16:
					twoLineBufLngPerLine=hdr.width*2;
					break;
			}
			break;
		case 2:   // Truecolor
			switch(hdr.bitDepth)
			{
				case 8:
					twoLineBufLngPerLine=hdr.width*3;
					break;
				case 16:
					twoLineBufLngPerLine=hdr.width*6;
					break;
			}
			break;
		case 3:   // Indexed-color
			switch(hdr.bitDepth)
			{
				case 1:
					twoLineBufLngPerLine=(hdr.width+7)/8;
					break;
				case 2:
					twoLineBufLngPerLine=(hdr.width+3)/4;
					break;
				case 4:
					twoLineBufLngPerLine=(hdr.width+1)/2;
					break;
				case 8:
					twoLineBufLngPerLine=hdr.width;
					break;
			}
			break;
		case 4:   // Greyscale with alpha
			switch(hdr.bitDepth)
			{
				case 8:
					twoLineBufLngPerLine=hdr.width*2;
					break;
				case 16:
					twoLineBufLngPerLine=hdr.width*4;
					break;
			}
			break;
		case 6:   // Truecolor with alpha
			switch(hdr.bitDepth)
			{
				case 8:
					twoLineBufLngPerLine=hdr.width*4;
					break;
				case 16:
					twoLineBufLngPerLine=hdr.width*8;
					break;
			}
			break;
	}

	twoLineBuf8=new unsigned char [twoLineBufLngPerLine*2];
	curLine8=twoLineBuf8;
	prvLine8=twoLineBuf8+twoLineBufLngPerLine;

	return PNG_OK;
}

/***************************************************************************************

***************************************************************************************/

int PNG::Output(unsigned char dat)
{
	unsigned int i;
	unsigned int colIdx;
	unsigned int interlaceWid,interlaceHei,interlaceX,interlaceY;

	if(y>=h)
	{
		return PNG_ERR;
	}

	if(x==-1)  // First byte is filter type for the line.  
	{
		filter=dat;   // See PNG Specification 4.5.4 Filtering, 9 Filtering
		inLineCount=0;
		inPixelCount=0;
		x++;
		//dprintf("y=%d filter=%d\n",y,filter);
		return PNG_OK;
	}
	else 
	{
		switch(hdr.interlaceMethod)
		{
			// Non-Interlace
			case 0:
				switch(hdr.colorType)  // See PNG Specification 6.1 Colour types and values
				{
					// Grayscale
					case 0:
						switch(hdr.bitDepth)
						{
							case 1:
								curLine8[x/8]=dat;
								Filter8(curLine8,prvLine8,x/8,y,1,filter);

								for(i=0; i<8 && x<w; i++)
								{
									colIdx=(curLine8[x/8]>>(7-i))&1;
									colIdx=(colIdx<<1)+colIdx;
									colIdx=(colIdx<<2)+colIdx;
									colIdx=(colIdx<<4)+colIdx;

									rgba[index  ]=colIdx;
									rgba[index+1]=colIdx;
									rgba[index+2]=colIdx;
									rgba[index+3]=0;
									x++;
									index+=4;
								}
								break;

							case 8:
								curLine8[x]=dat;
								Filter8(curLine8,prvLine8,x,y,1,filter);
								colIdx=curLine8[x];

								rgba[index  ]=curLine8[x];
								rgba[index+1]=curLine8[x];
								rgba[index+2]=curLine8[x];
								if(curLine8[x]==trns.col[0] || curLine8[x]==trns.col[1] || curLine8[x]==trns.col[2])
								{
									rgba[index+3]=0;
								}
								else
								{
									rgba[index+3]=255;
								}

								x++;
								index+=4;
								break;
						}
						break;



						// True color
					case 2:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==3)
								{
									Filter8(curLine8,prvLine8,x,y,3,filter);
									rgba[index  ]=curLine8[inLineCount];
									rgba[index+1]=curLine8[inLineCount+1];
									rgba[index+2]=curLine8[inLineCount+2];

									if(curLine8[inLineCount  ]==trns.col[0] && 
											curLine8[inLineCount+1]==trns.col[1] && 
											curLine8[inLineCount+2]==trns.col[2])
									{
										rgba[index+3]=0;
									}
									else
									{
										rgba[index+3]=255;
									}

									x++;
									index+=4;
									inLineCount+=3;
									inPixelCount=0;
								}
								break;
							case 16:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==6)
								{
									Filter8(curLine8,prvLine8,x,y,6,filter);
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount+2];
									rgba[index+2]=curLine8[inLineCount+4];

									r=curLine8[inLineCount  ]*256+curLine8[inLineCount+1];
									g=curLine8[inLineCount+2]*256+curLine8[inLineCount+3];
									b=curLine8[inLineCount+4]*256+curLine8[inLineCount+5];
									if(r==trns.col[0] && g==trns.col[1] && b==trns.col[2])
									{
										rgba[index+3]=0;
									}
									else
									{
										rgba[index+3]=255;
									}
									x++;
									index+=4;
									inLineCount+=6;
									inPixelCount=0;
								}
								break;
						}
						break;



						// Indexed color
					case 3:
						switch(hdr.bitDepth)
						{
							case 4:
								curLine8[x/2]=dat;
								Filter8(curLine8,prvLine8,x/2,y,1,filter);

								for(i=0; i<2 && x<w; i++)
								{
									colIdx=(curLine8[x/2]>>((1-i)*4))&0x0f;

									if(colIdx<plt.nEntry)
									{
										rgba[index  ]=plt.entry[colIdx*3  ];
										rgba[index+1]=plt.entry[colIdx*3+1];
										rgba[index+2]=plt.entry[colIdx*3+2];
										if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
										{
											rgba[index+3]=0;
										}
										else
										{
											rgba[index+3]=255;
										}
									}
									x++;
									index+=4;
								}
								break;

							case 8:
								curLine8[x]=dat;
								Filter8(curLine8,prvLine8,x,y,1,filter);
								colIdx=curLine8[x];

								if(colIdx<plt.nEntry)
								{
									rgba[index  ]=plt.entry[colIdx*3  ];
									rgba[index+1]=plt.entry[colIdx*3+1];
									rgba[index+2]=plt.entry[colIdx*3+2];
									if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
									{
										rgba[index+3]=0;
									}
									else
									{
										rgba[index+3]=255;
									}
								}
								x++;
								index+=4;
								break;
						}
						break;



						// Greyscale with alpha
					case 4:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==2)
								{
									Filter8(curLine8,prvLine8,x,y,2,filter);
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount  ];
									rgba[index+2]=curLine8[inLineCount  ];
									rgba[index+3]=curLine8[inLineCount+1];
									index+=4;
									x++;
									inLineCount+=2;
									inPixelCount=0;
								}
								break;
						}
						break;



						// Truecolor with alpha
					case 6:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==4)
								{
									Filter8(curLine8,prvLine8,x,y,4,filter);
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount+1];
									rgba[index+2]=curLine8[inLineCount+2];
									rgba[index+3]=curLine8[inLineCount+3];
									index+=4;
									x++;
									inLineCount+=4;
									inPixelCount=0;
								}
								break;
						}
						break;
				}  // switch(hdr.colorType)

				if(x>=w)  {
					y++;
					x=-1;
					ShiftTwoLineBuf();
				}

				return PNG_OK;


				// Interlace
			case 1:
				//   1 6 4 6 2 6 4 6
				//   7 7 7 7 7 7 7 7
				//   5 6 5 6 5 6 5 6
				//   7 7 7 7 7 7 7 7
				//   3 6 4 6 3 6 4 6
				//   7 7 7 7 7 7 7 7
				//   5 6 5 6 5 6 5 6
				//   7 7 7 7 7 7 7 7
				switch(interlacePass)
				{
					case 1:
						interlaceWid=(w+7)/8;
						interlaceHei=(h+7)/8;
						interlaceX=x*8;
						interlaceY=y*8;
						break;
					case 2:
						interlaceWid=(w+3)/8;
						interlaceHei=(h+7)/8;
						interlaceX=4+x*8;
						interlaceY=y*8;
						break;
					case 3:
						interlaceWid=(w+3)/4;
						interlaceHei=(h+3)/8;
						interlaceX=x*4;
						interlaceY=4+y*8;
						break;
					case 4:
						interlaceWid=(w+1)/4;
						interlaceHei=(h+3)/4;
						interlaceX=2+x*4;
						interlaceY=y*4;
						break;
					case 5:
						interlaceWid=(w+1)/2;
						interlaceHei=(h+1)/4;
						interlaceX=x*2;
						interlaceY=2+y*4;
						break;
					case 6:
						interlaceWid=(w  )/2;
						interlaceHei=(h+1)/2;
						interlaceX=1+x*2;
						interlaceY=y*2;
						break;
					case 7:
						interlaceWid=w;
						interlaceHei=h/2;
						interlaceX=x;
						interlaceY=1+y*2;
						break;
					default:
						return PNG_ERR;
				} // switch(interlacePass)

				switch(hdr.colorType)  // See PNG Specification 6.1 Colour types and values
				{
					// Grayscale
					case 0:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount]=dat;
								Filter8(curLine8,prvLine8,x,y,1,filter);

								index=interlaceX*4+interlaceY*w*4;
								rgba[index  ]=curLine8[inLineCount];
								rgba[index+1]=curLine8[inLineCount];
								rgba[index+2]=curLine8[inLineCount];

								if(curLine8[inLineCount]==trns.col[0] || 
										curLine8[inLineCount]==trns.col[1] || 
										curLine8[inLineCount]==trns.col[2])
								{
									rgba[index+3]=0;
								}
								else
								{
									rgba[index+3]=255;
								}

								x++;
								inLineCount++;
								break;
						}
						break;


						// True color
					case 2:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==3)
								{
									Filter8(curLine8,prvLine8,x,y,3,filter);

									index=interlaceX*4+interlaceY*w*4;
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount+1];
									rgba[index+2]=curLine8[inLineCount+2];

									if(curLine8[inLineCount  ]==trns.col[0] && 
											curLine8[inLineCount+1]==trns.col[1] && 
											curLine8[inLineCount+2]==trns.col[2])
									{
										rgba[index+3]=0;
									}
									else
									{
										rgba[index+3]=255;
									}

									x++;
									inLineCount+=3;
									inPixelCount=0;
								}
								break;

							case 16:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==6)
								{
									Filter8(curLine8,prvLine8,x,y,6,filter);
									index=interlaceX*4+interlaceY*w*4;
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount+2];
									rgba[index+2]=curLine8[inLineCount+4];

									r=curLine8[inLineCount  ]*256+curLine8[inLineCount+1];
									g=curLine8[inLineCount+2]*256+curLine8[inLineCount+3];
									b=curLine8[inLineCount+4]*256+curLine8[inLineCount+5];

									if(curLine8[inLineCount  ]==trns.col[0] && 
											curLine8[inLineCount+1]==trns.col[1] && 
											curLine8[inLineCount+2]==trns.col[2])
									{
										rgba[index+3]=0;
									}
									else
									{
										rgba[index+3]=255;
									}

									x++;
									inLineCount+=6;
									inPixelCount=0;
								}
								break;
						}
						break;



						// Indexed color
					case 3:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount]=dat;
								Filter8(curLine8,prvLine8,x,y,1,filter);

								index=interlaceX*4+interlaceY*w*4;
								colIdx=curLine8[inLineCount  ];
								if(colIdx<plt.nEntry)
								{
									rgba[index  ]=plt.entry[colIdx*3  ];
									rgba[index+1]=plt.entry[colIdx*3+1];
									rgba[index+2]=plt.entry[colIdx*3+2];
									if(colIdx==trns.col[0] || colIdx==trns.col[1] || colIdx==trns.col[2])
									{
										rgba[index+3]=0;
									}
									else
									{
										rgba[index+3]=255;
									}
								}

								x++;
								inLineCount++;
								break;
						}
						break;



						// Greyscale with alpha
					case 4:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==2)
								{
									Filter8(curLine8,prvLine8,x,y,2,filter);

									index=interlaceX*4+interlaceY*w*4;
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount  ];
									rgba[index+2]=curLine8[inLineCount  ];
									rgba[index+3]=curLine8[inLineCount+1];

									x++;
									inLineCount+=2;
									inPixelCount=0;
								}
								break;
						}
						break;



						// Truecolor with alpha
					case 6:
						switch(hdr.bitDepth)
						{
							case 8:
								curLine8[inLineCount+inPixelCount]=dat;
								inPixelCount++;
								if(inPixelCount==4)
								{
									Filter8(curLine8,prvLine8,x,y,4,filter);

									index=interlaceX*4+interlaceY*w*4;
									rgba[index  ]=curLine8[inLineCount  ];
									rgba[index+1]=curLine8[inLineCount+1];
									rgba[index+2]=curLine8[inLineCount+2];
									rgba[index+3]=curLine8[inLineCount+3];

									x++;
									inLineCount+=4;
									inPixelCount=0;
								}
								break;
						}
						break;
				} // switch(hdr.colorType)

				if(x>=(int)interlaceWid) {
					y++;
					x=-1;
					ShiftTwoLineBuf();
					if(y>=(int)interlaceHei)  {
						y=0;
						interlacePass++;
						dprintf("Interlace Pass %d\n",interlacePass);
					}
				}

				return PNG_OK;
			default:
				dprintf("Unsupported interlace method.\n");
				return PNG_ERR;
		}
	}
	return PNG_ERR;
}

/***************************************************************************************

***************************************************************************************/

int PNG::EndOutput(void)
{
	dprintf("Final Position (%d,%d)\n",x,y);
	return PNG_OK;
}

/***************************************************************************************

***************************************************************************************/

void PNG::flip(void)  {				// For drawing in OpenGL
	int x,y,bytePerLine;
	unsigned int swp;
	bytePerLine=w*4;

	for(y=0; y<h/2; y++)  {
		for(x=0; x<bytePerLine; x++)  {
			swp = rgba[y*bytePerLine+x];
			rgba[y*bytePerLine+x] = rgba[(h-1-y)*bytePerLine+x];
			rgba[(h-1-y)*bytePerLine+x] = swp;
		}
	}
	return;
}

/***************************************************************************************

***************************************************************************************/

void PNG::dump(void)  {				// For drawing in OpenGL
	int i, j, k;
	//FILE *stream;
	unsigned char r,g,b,a;
	//int bp = 0;

	//stream = fopen("png.txt", "wt");

	for(i=0; i<h; i++)  {
		for(j=0; j<w; j++)  {
			k = i*w + j;
			r = rgba[k+0];
			g = rgba[k+1];
			b = rgba[k+2];
			a = rgba[k+3];
			//fprintf(stream, "%02x %02x %02x %02x - ", r, g, b, a);
			if (a>0)  {
				//bp = 1;
			}
		}
		//fprintf(stream, "\n");
	}


	//FCLOSE(stream);

	return;
}
