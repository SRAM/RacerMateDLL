
#define STRICT
#define WIN32_LEAN_AND_MEAN

#if 0
	// from sourceforge:
#define PROGRAM "hex2bin"
#define VERSION "1.0.10"

// to compile with Microsoft Visual Studio
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// size in bytes
#define MEMORY_SIZE 4096*1024

/* We use buffer to speed disk access. */
#ifdef USE_FILE_BUFFERS
#define BUFFSZ 4096
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* FIXME how to get it from the system/OS? */
#define MAX_FILE_NAME_SIZE 81

#ifdef DOS
#define MAX_EXTENSION_SIZE 4
#else
#define MAX_EXTENSION_SIZE 16
#endif

/* The data records can contain 255 bytes: this means 512 characters. */
#define MAX_LINE_SIZE 1024

#define NO_ADDRESS_TYPE_SELECTED 0
#define LINEAR_ADDRESS 1
#define SEGMENTED_ADDRESS 2
#if defined(MSDOS) || defined(__DOS__) || defined(__MSDOS__) || defined(_MSDOS)
#define _COND_(x)	 (((x) == '-') || ((x) == '/'))
#else
/* Assume unix and similar */
/* We don't accept an option beginning with a '/' because it could be a file name. */
#define _COND_(x)	 ((x) == '-')
#endif

typedef char filetype[MAX_FILE_NAME_SIZE];
typedef int boolean;
typedef unsigned char byte;
typedef unsigned short word;

filetype    Filename;           /* string for opening files */
char        Extension[MAX_EXTENSION_SIZE];       /* filename extension for output files */

FILE        *Filin,             /* input files */
  *Filout;            /* output files */

#ifdef USE_FILE_BUFFERS
char		*FilinBuf,          /* text buffer for file input */
  *FiloutBuf;         /* text buffer for file output */
#endif

int Pad_Byte = 0xFF;

void usage(void)
{
  fprintf (stderr,
		   "\n"
		   "usage: "PROGRAM" [OPTIONS] filename\n"
		   "Options:\n"
		   "  -s [address]  Starting address in hex (default: 0)\n"
		   "  -l [length]   Maximal Length (Starting address + Length -1 is Max Address)\n"
		   "                File will be filled with Pattern until Max Address is reached\n"
		   "                Length must be a power of 2 in hexadecimal:\n"
           "                     Hex      Decimal\n"
           "                    1000   =    4096   (4ki)\n"
           "                    2000   =    8192   (8ki)\n"
           "                    4000   =   16384  (16ki)\n"
           "                    8000   =   32768  (32ki)\n"
           "                   10000   =   65536  (64ki)\n"
           "                   20000   =  131072 (128ki)\n"
           "                   40000   =  262144 (256ki)\n"
           "                   80000   =  524288 (512ki)\n"
           "                  100000   = 1048576   (1Mi)\n"
           "                  200000   = 2097152   (2Mi)\n"
           "                  400000   = 4194304   (4Mi)\n"
           "                  800000   = 8388608   (8Mi)\n"
		   "  -e [ext]      Output filename extension (without the dot)\n"
		   "  -c            Enable hex file checksum verification\n"
		   "  -p [value]    Pad-byte value in hex (default: %x)\n\n"
		   "  -k [0|1|2]    Select checksum type\n"
		   "                       0 = 8-bit,\n"
		   "                       1 = 16-bit little endian,\n"
		   "                       2 = 16-bit big endian\n"
		   "  -r [start] [end]     Range to compute checksum over (default is min and max addresses)\n"
		   "  -f [address] [value] Address and value of checksum to force\n\n",
		   Pad_Byte);
  exit(1);
} /* procedure USAGE */

void *NoFailMalloc (size_t size)
{
  void *result;

  if ((result = malloc (size)) == NULL)
    {
	  fprintf (stderr,"Can't allocate memory.\n");
	  exit(1);
    }
  return (result);
}

/* Open the input file, with error checking */
void NoFailOpenInputFile (char *Flnm)
{
  while ((Filin = fopen(Flnm,"r")) == NULL)
    {
	  fprintf (stderr,"Input file %s cannot be opened. Enter new filename: ",Flnm);
	  if (fgets (Flnm,MAX_FILE_NAME_SIZE,stdin) == NULL) exit(1); /* modified error checking */
    }

#ifdef USE_FILE_BUFFERS
  FilinBuf = (char *) NoFailMalloc (BUFFSZ);
  setvbuf(Filin, FilinBuf, _IOFBF, BUFFSZ);
#endif
} /* procedure OPENFILIN */

/* Open the output file, with error checking */
void NoFailOpenOutputFile (char *Flnm)
{
  while ((Filout = fopen(Flnm,"wb")) == NULL)
    {
	  /* Failure to open the output file may be
		 simply due to an insufficiant permission setting. */
	  fprintf(stderr,"Output file %s cannot be opened. Enter new file name: ", Flnm);
	  if (fgets(Flnm,MAX_FILE_NAME_SIZE,stdin) == NULL) exit(1);
    }

#ifdef USE_FILE_BUFFERS
  FiloutBuf = (char *) NoFailMalloc (BUFFSZ);
  setvbuf(Filout, FiloutBuf, _IOFBF, BUFFSZ);
#endif

} /* procedure OPENFILOUT */

void GetLine(char* str,FILE *in)
{
  char *result;

  result = fgets(str,MAX_LINE_SIZE,in);
  if ((result == NULL) && !feof (in)) fprintf(stderr,"Error occurred while reading from file\n");
}

/* Filters out the error conditions */
int GetHex(const char *str)
{
  int result, value;

  result = sscanf(str,"%x",&value);

  if (result == 1) return value;
  else
	{
	  fprintf(stderr,"Some error occurred when parsing options.\n");
	  exit (1);
	}
}

void GetExtension(const char *str,char *ext)
{
  if (strlen(str) > MAX_EXTENSION_SIZE)
	usage();

  strcpy(ext, str);
}

/* Adds an extension to a file name */
void PutExtension(char *Flnm, char *Extension)
{
  char        *Period;        /* location of period in file name */
  boolean     Samename;

  /* This assumes DOS like file names */
  /* Don't use strchr(): consider the following filename:
	 ../my.dir/file.hex
  */
  if ((Period = strrchr(Flnm,'.')) != NULL)
	*(Period) = '\0';

  Samename = FALSE;
  if (strcmp(Extension, Period+1) == 0)
	Samename = TRUE;

  strcat(Flnm,".");
  strcat(Flnm, Extension);
  if (Samename)
    {
	  fprintf (stderr,"Input and output filenames (%s) are the same.", Flnm);
	  exit(1);
    }
}

int main (int argc, char *argv[])
{
  /* line inputted from file */
  char Line[MAX_LINE_SIZE];

  /* flag that a file was read */
  boolean Fileread;
  boolean Enable_Checksum_Error = FALSE;
  boolean Status_Checksum_Error = FALSE;

  /* cmd-line parameter # */
  char *p;

  int Param;

  /* Application specific */

  unsigned int Nb_Bytes;
  unsigned int First_Word, Address, Segment, Upper_Address;
  unsigned int Lowest_Address, Highest_Address, Starting_Address;
  unsigned int Max_Length = MEMORY_SIZE;

  /* This mask is for mapping the target binary inside the
	 binary buffer. If for example, we are generating a binary
	 file with records starting at FFF00000, the bytes will be
	 stored at the beginning of the memory buffer. */
  unsigned int Address_Mask;

  unsigned int Phys_Addr, Type;
  unsigned int temp;
  unsigned int Record_Nb;

  /* We will assume that when one type of addressing is selected, it will be valid for all the
	 current file. Records for the other type will be ignored. */
  unsigned int	Seg_Lin_Select = NO_ADDRESS_TYPE_SELECTED;

  boolean Starting_Address_Setted = FALSE;
  boolean Max_Length_Setted = FALSE;

  int temp2;

  byte	Data_Str[MAX_LINE_SIZE];
  byte 	Checksum = 0; // 20040617+ Added initialisation to remove GNU compiler warning about possible uninitialised usage

#define CKS_8     0
#define CKS_16LE  1
#define CKS_16BE  2

  unsigned short int wCKS;
  unsigned short int w;
  unsigned int Cks_Type = CKS_8;
  unsigned int Cks_Start = 0, Cks_End = 0, Cks_Addr = 0, Cks_Value = 0;
  boolean Cks_range_set = FALSE;
  boolean Cks_Addr_set = FALSE;

  /* This will hold binary codes translated from hex file. */
  byte *Memory_Block;

  fprintf (stdout,PROGRAM" v"VERSION", Copyright (C) 2012 Jacques Pelletier & contributors\n\n");

  if (argc == 1)
	usage();

  strcpy(Extension, "bin"); /* default is for binary file extension */

  /* read file */
  Starting_Address = 0;

  /*
	use p for parsing arguments
	use i for number of parameters to skip
	use c for the current option
  */
  for (Param = 1; Param < argc; Param++)
    {
	  int i = 0;
	  char c;

	  p = argv[Param];
      c = *(p+1); /* Get option character */

	  if ( _COND_(*p) ) {
		/* Last parameter is not a filename */
		if (Param == argc-1) usage();

		switch(tolower(c))
		  {
			/* file extension */
		  case 'e':
			GetExtension(argv[Param + 1],Extension);
			i = 1; /* add 1 to Param */
			break;
		  case 'c':
			Enable_Checksum_Error = TRUE;
			i = 0;
			break;
		  case 's':
			Starting_Address = GetHex(argv[Param + 1]);
			Starting_Address_Setted = TRUE;
			i = 1; /* add 1 to Param */
			break;
		  case 'l':
			Max_Length = GetHex(argv[Param + 1]);
			Max_Length_Setted = TRUE;
			i = 1; /* add 1 to Param */
			break;
		  case 'p':
			Pad_Byte = GetHex(argv[Param + 1]);
			i = 1; /* add 1 to Param */
			break;

			/* New Checksum parameters */
		  case 'k':
			Cks_Type = GetHex(argv[Param + 1]);
			{
			  if (Cks_Type != CKS_8 &&
				  Cks_Type != CKS_16LE &&
				  Cks_Type != CKS_16BE ) usage();
			}
			i = 1; /* add 1 to Param */
			break;

		  case 'r':
			Cks_Start = GetHex(argv[Param + 1]);
			Cks_End = GetHex(argv[Param + 2]);
			Cks_range_set = TRUE;
			i = 2; /* add 2 to Param */
			break;

		  case 'f':
			Cks_Addr = GetHex(argv[Param + 1]);
			Cks_Value = GetHex(argv[Param + 2]);
			Cks_Addr_set = TRUE;
			i = 2; /* add 2 to Param */
			break;

		  case '?':
		  case 'h':
		  default:
			usage();
		  } /* switch */

		/* if (Param + i) < (argc -1) */
		if (Param < argc -1 -i) Param += i; else usage();

	  } else
		break;
	  /* if option */

	} /* for Param */

  /* when user enters input file name */

  /* Assume last parameter is filename */
  strcpy(Filename,argv[argc -1]);

  /* Just a normal file name */
  NoFailOpenInputFile (Filename);
  PutExtension(Filename, Extension);
  NoFailOpenOutputFile(Filename);
  Fileread = TRUE;

  /* allocate a buffer */
  Memory_Block = (byte *) NoFailMalloc(Max_Length);

  /* For EPROM or FLASH memory types, fill unused bytes with FF or the value specified by the p option */
  memset (Memory_Block,Pad_Byte,Max_Length);

  /* To begin, assume the lowest address is at the end of the memory.
	 While reading each records, subsequent addresses will lower this number.
	 At the end of the input file, this value will be the lowest address.

	 A similar assumption is made for highest address. It starts at the
	 beginning of memory. While reading each records, subsequent addresses will raise this number.
	 At the end of the input file, this value will be the highest address. */
  Lowest_Address = Max_Length - 1;
  Highest_Address = 0;
  Segment = 0;
  Upper_Address = 0;
  Record_Nb = 0;

  /* Max length must be in powers of 2: 1,2,4,8,16,32, etc. */
  Address_Mask = Max_Length -1;

  /* Read the file & process the lines. */
  do /* repeat until EOF(Filin) */
	{
	  unsigned int i;

	  /* Read a line from input file. */
	  GetLine(Line,Filin);
	  Record_Nb++;

	  /* Remove carriage return/line feed at the end of line. */
	  i = strlen(Line)-1;

	  if (Line[i] == '\n') Line[i] = '\0';

	  /* Scan the first two bytes and nb of bytes.
		 The two bytes are read in First_Word since its use depend on the
		 record type: if it's an extended address record or a data record.
	  */
	  sscanf (Line, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);

	  Checksum = Nb_Bytes + (First_Word >> 8) + (First_Word & 0xFF) + Type;

	  p = (char *) Data_Str;

	  /* If we're reading the last record, ignore it. */
	  switch (Type)
		{
		  /* Data record */
		case 0:
		  if (Nb_Bytes == 0)
			{
			  fprintf(stderr,"0 byte length Data record ignored\n");
			  break;
			}

		  Address = First_Word;

		  if (Seg_Lin_Select == SEGMENTED_ADDRESS)
			Phys_Addr = (Segment << 4) + Address;
		  else
			/* LINEAR_ADDRESS or NO_ADDRESS_TYPE_SELECTED
			   Upper_Address = 0 as specified in the Intel spec. until an extended address
			   record is read. */
			Phys_Addr = ((Upper_Address << 16) + Address) & Address_Mask;

		  /* Check that the physical address stays in the buffer's range. */
		  if ((Phys_Addr + Nb_Bytes) <= Max_Length)
			{
			  /* Set the lowest address as base pointer. */
			  if (Phys_Addr < Lowest_Address)
				Lowest_Address = Phys_Addr;

			  /* Same for the top address. */
			  temp = Phys_Addr + Nb_Bytes -1;

			  if (temp > Highest_Address)
				Highest_Address = temp;

			  /* Read the Data bytes. */
			  /* Bytes are written in the Memory block even if checksum is wrong. */
			  i = Nb_Bytes;

			  do
				{
				  sscanf (p, "%2x",&temp2);
				  p += 2;

				  /* Overlapping record will erase the pad bytes */
				  if (Memory_Block[Phys_Addr] != Pad_Byte) fprintf(stderr,"Overlapped record detected\n");

				  Memory_Block[Phys_Addr++] = temp2;
				  Checksum = (Checksum + temp2) & 0xFF;
				}
			  while (--i != 0);

			  /* Read the Checksum value. */
			  sscanf (p, "%2x",&temp2);

			  /* Verify Checksum value. */
			  if ((((Checksum + temp2) & 0xFF) != 0) && Enable_Checksum_Error)
				{
				  fprintf(stderr,"Checksum error in record %d: should be %02X\n",Record_Nb, (256 - Checksum) & 0xFF);
				  Status_Checksum_Error = TRUE;
				}
			}
		  else
			{
			  if (Seg_Lin_Select == SEGMENTED_ADDRESS)
				fprintf(stderr,"Data record skipped at %4X:%4X\n",Segment,Address);
			  else
				fprintf(stderr,"Data record skipped at %8X\n",Phys_Addr);
			}

		  break;

		  /* End of file record */
		case 1:
		  /* Simply ignore checksum errors in this line. */
		  break;

		  /* Extended segment address record */
		case 2:
		  /* First_Word contains the offset. It's supposed to be 0000 so
			 we ignore it. */

		  /* First extended segment address record ? */
		  if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
			Seg_Lin_Select = SEGMENTED_ADDRESS;

		  /* Then ignore subsequent extended linear address records */
		  if (Seg_Lin_Select == SEGMENTED_ADDRESS)
			{
			  sscanf (p, "%4x%2x",&Segment,&temp2);

			  /* Update the current address. */
			  Phys_Addr = (Segment << 4) & Address_Mask;

			  /* Verify Checksum value. */
			  Checksum = (Checksum + (Segment >> 8) + (Segment & 0xFF) + temp2) & 0xFF;

			  if ((Checksum != 0) && Enable_Checksum_Error)
				Status_Checksum_Error = TRUE;
			}
		  break;

		  /* Start segment address record */
		case 3:
		  /* Nothing to be done since it's for specifying the starting address for
			 execution of the binary code */
		  break;

		  /* Extended linear address record */
		case 4:
		  /* First_Word contains the offset. It's supposed to be 0000 so
			 we ignore it. */

		  /* First extended linear address record ? */
		  if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
			Seg_Lin_Select = LINEAR_ADDRESS;

		  /* Then ignore subsequent extended segment address records */
		  if (Seg_Lin_Select == LINEAR_ADDRESS)
			{
			  sscanf (p, "%4x%2x",&Upper_Address,&temp2);

			  /* Update the current address. */
			  Phys_Addr = (Upper_Address << 16) & Address_Mask;

			  /* Verify Checksum value. */
			  Checksum = (Checksum + (Upper_Address >> 8) + (Upper_Address & 0xFF) + temp2)
				& 0xFF;

			  if ((Checksum != 0) && Enable_Checksum_Error)
				Status_Checksum_Error = TRUE;
			}
		  break;

		  /* Start linear address record */
		case 5:
		  /* Nothing to be done since it's for specifying the starting address for
			 execution of the binary code */
		  break;
		default:
		  fprintf(stderr,"Unknown record type\n");
		  break;
		}
	} while (!feof (Filin));


  /*-----------------------------------------------------------------------------*/

  // Max_Length is set; the memory buffer is already filled with pattern before
  // reading the hex file. The padding bytes will then be added to the binary file.
  if (Max_Length_Setted==TRUE) Highest_Address = Starting_Address + Max_Length-1;

  fprintf(stdout,"Lowest address  = %08X\n",Lowest_Address);
  fprintf(stdout,"Highest address = %08X\n",Highest_Address);
  fprintf(stdout,"Pad Byte        = %X\n",  Pad_Byte);

  /* Add a checksum to the binary file */
  wCKS = 0;
  if (!Cks_range_set)
	{
	  Cks_Start = Lowest_Address;
	  Cks_End = Highest_Address;
	}
  switch (Cks_Type)
	{
	  unsigned int i;

	case CKS_8:

	  for (i=Cks_Start; i<=Cks_End; i++)
		{
		  wCKS += Memory_Block[i];
		}

	  fprintf(stdout,"8-bit Checksum = %02X\n",wCKS & 0xff);
	  if( Cks_Addr_set )
		{
		  wCKS = Cks_Value - (wCKS - Memory_Block[Cks_Addr]);
		  Memory_Block[Cks_Addr] = (byte)(wCKS & 0xff);
		  fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, wCKS & 0xff);
		}
	  break;

	case CKS_16BE:

	  for (i=Cks_Start; i<=Cks_End; i+=2)
		{
		  w =  Memory_Block[i+1] | ((word)Memory_Block[i] << 8);
		  wCKS += w;
		}

	  fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
	  if( Cks_Addr_set )
		{
		  w = Memory_Block[Cks_Addr+1] | ((word)Memory_Block[Cks_Addr] << 8);
		  wCKS = Cks_Value - (wCKS - w);
		  Memory_Block[Cks_Addr] = (byte)(wCKS >> 8);
		  Memory_Block[Cks_Addr+1] = (byte)(wCKS & 0xff);
		  fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
		}
	  break;

	case CKS_16LE:

	  for (i=Cks_Start; i<=Cks_End; i+=2)
		{
		  w =  Memory_Block[i] | ((word)Memory_Block[i+1] << 8);
		  wCKS += w;
		}

	  fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
	  if( Cks_Addr_set )
		{
		  w = Memory_Block[Cks_Addr] | ((word)Memory_Block[Cks_Addr+1] << 8);
		  wCKS = Cks_Value - (wCKS - w);
		  Memory_Block[Cks_Addr+1] = (byte)(wCKS >> 8);
		  Memory_Block[Cks_Addr] = (byte)(wCKS & 0xff);
		  fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
		}

	default:
	  ;
	}

  /* This starting address is for the binary file,

	 ex.: if the first record is :nn010000ddddd...
	 the data supposed to be stored at 0100 will start at 0000 in the binary file.

	 Specifying this starting address will put pad bytes in the binary file so that
	 the data supposed to be stored at 0100 will start at the same address in the
	 binary file.
  */

  if(Starting_Address_Setted)
	{
	  Lowest_Address = Starting_Address;
	}

  /* write binary file */
  fwrite (&Memory_Block[Lowest_Address],
		  1,
		  Highest_Address - Lowest_Address +1,
		  Filout);

  free (Memory_Block);

#ifdef USE_FILE_BUFFERS
  free (FilinBuf);
  free (FiloutBuf);
#endif

  fclose (Filin);
  fclose (Filout);

  if (Status_Checksum_Error && Enable_Checksum_Error)
	{
	  fprintf(stderr,"Checksum error detected.\n");
	  return 1;
	}

  if (!Fileread)
	usage();
  return 0;
}

#endif								// #if 0



#pragma warning(disable:4996)					// for vista strncpy_s

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <string.h>

#include <utils.h>
#include <fatalerror.h>
#include <intel.h>

extern char gstring[2048];

/****************************************************************************************************

intel hex format:

	:xx	00 - number of bytes of data in this line
	xxxx	03 - beginning address in rom of this line
	00	07,08 - always
	data	09 - beginning of data
	xx	1 byte checksum

****************************************************************************************************/

INTEL::INTEL(void)  {

#ifdef _DEBUG
	bp = 0;
#endif

	blockmask = 0L;
	binsize = 0;

	return;
}

/****************************************************************************************************

****************************************************************************************************/

INTEL::~INTEL(void)  {

	return;
}

/****************************************************************************************************
	this utility converts an Intel hex format file to binary
	syntax is 'hextobin filename', where 'filename' is the name of an intel hex format file
****************************************************************************************************/

void INTEL::makebin(const char *_infname)  {
	unsigned char outbuf[128];
	unsigned long n;
	FILE *instream,*outstream;
	char infname[256];
	char outfname[256];
	unsigned long k;
	unsigned short checksum=0;
	char *cptr;
	int i;
	char inbuf[1024];
	unsigned char buf[8];
	unsigned long address, hiaddr = 0L;
	unsigned char type;


	strncpy(infname, _infname, sizeof(infname)-1);
	strncpy(outfname, _infname, sizeof(infname)-1);

	if (!has_extension(infname))  {
		strcat(infname, ".hex");
	}

	instream = fopen(infname, "rt");
	if (instream==NULL)  {
		throw (fatalError(__FILE__, __LINE__));
	}
	for (i=0;i<32;i++)  {
		if (outfname[i]=='.') break;
	}
	outfname[i] = 0;
	strcat(outfname, ".bin");
	outstream = fopen(outfname, "wb");

	while(1)  {
		memset(inbuf, 0, sizeof(inbuf));
		cptr = fgets(inbuf, sizeof(inbuf), instream);		// get intel hex line
		if (cptr==NULL)  {
			break;
		}

		trimcrlf(inbuf);

		memset(buf, 0, sizeof(buf));

		buf[0] = inbuf[7];
		buf[1] = inbuf[8];
		type = (unsigned char)tobin((char *)buf);		// type

		buf[0] = inbuf[1];
		buf[1] = inbuf[2];
		n = tobin((char *)buf);			// number of bytes in line

		/*
: n     addr     type    n bytes of data + checksum

: 02    0000     04      00 03 ... F7

: 40    C000     00      01000A000000466C6F5363616E20466C6F4E657400CDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCD4E3254442D3644422D324B0000000000000000000000000000009F
: 40    C040     00      0000000000004E3130303000CDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCD3030303030373737303030303037373833335340CDCDCDCDCDCDED
: 40    C080     00      06063BE6894100089445A9BFFE410028974502AB8F420058A045D0390C430030A54542194F430060A74591F2D14300D8AC45000000000000000000000000000037
: 40    C0C0     00      0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040
: 40    C100     00      0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000507A7D41009C7E465647E7410064EE
: 40    C140     00      764632D4454200E87C4616D9024300907F463C6C6A4300888146A175DF430058834600000000000000000000000000000000000000000000000000000000000046
: 40    C180     00      000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007F
: 40    C1C0     00      00000000000000000000000000000000000000000000000000000000000000000000323030382D30342D3033CDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDCDFB4AA5
: 04    0000     05      00 00 01 08 EE

: 00    0000     01      FF
		*/

		/*
		enum  {
			DATR,				// 00 - Data Record (8-, 16-, or 32-bit formats)
			EOFR,				// 01 - End of File Record (8-, 16-, or 32-bit formats)
			ESAR,				// 02 - Extended Segment Address Record (16- or 32-bit formats)
			SSAR,				// 03 - Start Segment Address Record (16- or 32-bit formats)
			ELAR,				// 04 - Extended Linear Address Record (32-bit format only)
			SLAR				// 05 - Start Linear Address Record (32-bit format only)
		};
		*/

		if (type==ELAR)  {						// 4
			buf[0] = inbuf[9];
			buf[1] = inbuf[10];
			buf[2] = inbuf[11];
			buf[3] = inbuf[12];

			hiaddr = tobin2( (char *)buf);
			hiaddr = hiaddr << 16;
		}

		else if (type==DATR)  {						// 0
			buf[0] = inbuf[3];
			buf[1] = inbuf[4];
			buf[2] = inbuf[5];
			buf[3] = inbuf[6];				// address
			address = hiaddr + tobin2((char *)buf);

			for(i=0; i<(int)n; i++) {
				buf[0] = inbuf[9+i*2];
				buf[1] = inbuf[9+i*2+1];
				outbuf[i] = (char)tobin((char *)buf);
				fputc(outbuf[i], outstream);
				checksum += (unsigned short)outbuf[i];
			}
		}
		else if (type==EOFR)  {						// 1, done, just ignore this type
#ifdef _DEBUG
			bp = 1;
#endif
		}
		else if (type==SLAR)  {						// 5, ignore?
#ifdef _DEBUG
			bp = 2;
#endif
		}
		else  {
			fclose(instream);
			fclose(outstream);
			sprintf(gstring, "unhandled type: %d", type);
			throw (fatalError(__FILE__, __LINE__, gstring) );
		}


	}


	fclose(instream);
	fclose(outstream);

	return;
}

/****************************************************************************************************

	infname = hexfile to load
	outfile = binary data file (no address info)

intel hex format:

	:xx	00 - number of bytes of data in this line
	xxxx	03 - beginning address in rom of this line
	xx	07,08 - type
	data	09 - beginning of data
	xx	1 byte checksum

****************************************************************************************************/

void INTEL::loadhex(const char *_infname, const char *_outfname)  {
	int type;
	int linecount = 0;
	bool done = false;
	int i, n;
	unsigned char cs;
	unsigned char cs2;				// the cs in the hex file
	char *cptr;
	unsigned char c;
	char str[1024];
	char infname[256];
	FILE *instream = NULL;
	FILE *outstream = NULL;
	char inbuf[1024];
	unsigned long address;
	unsigned long hiaddr = 0L;
	int datacount = 1;
	unsigned long start_address=0L, end_address=0L;


	strncpy(infname, _infname, sizeof(infname)-1);
	instream = fopen(infname, "rb");
	if (instream==NULL)  {
		sprintf(str, "Can't find %s", infname);
		throw (fatalError(__FILE__, __LINE__, str) );
	}


	if (_outfname)  {
		outstream = fopen(_outfname, "wb");
	}


	while(!done)  {
		cptr = fgets(inbuf, sizeof(inbuf), instream);		// get intel hex line
		if (cptr==NULL)  {
			break;
		}
		trimcrlf(inbuf);
		linecount++;

		n = gethexnum(inbuf, 1, 2);
		address = gethexnum(inbuf, 3, 4);
		address |= hiaddr;
		type = gethexnum(inbuf, 7, 2);

		cs = 0;
		cs += (unsigned char)gethexnum(inbuf, 1, 2);					// len
		cs += (unsigned char)gethexnum(inbuf, 3, 2);					// addr
		cs += (unsigned char)gethexnum(inbuf, 5, 2);					// addr
		cs += (unsigned char)gethexnum(inbuf, 7, 2);					// type

		/*

:	n		addr	type	n bytes of data

:	02		0000	04		00 03 F7
:	40		C000	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
:	40		C040	00		00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000C0
:	40		C080	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000080
:	40		C0C0	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040
:	40		C100	00		00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000FF
:	40		C140	00		00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000BF
:	40		C180	00		000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007F
:	40		C1C0	00		000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003F
:	00		0000	01		FF
		*/

		switch(type)  {
			case DATR:  {			// 00 - data record
				if (datacount==1)  {
					start_address = address;
				}
				datacount++;
				end_address = address;

				for(i=0; i<(int)n; i++) {
					c = (unsigned char) gethexnum(inbuf, 9+i*2, 2);
					if (outstream)  {
						fputc(c, outstream);
					}
					cs += c;
				}
				cs2 = (unsigned char) gethexnum(inbuf, 9+i*2, 2);
				break;
			}

			case EOFR:  {			// 01 - end-of-file record
				cs = EOFR;
				cs2 = (unsigned char) gethexnum(inbuf, 9, 2);
				done = true;
				//bp = 1;
				break;
			}

			case ELAR:  {			// 04 - Extended Linear Address Record (32-bit format only)
				assert(n==2);
				for(i=0; i<(int)n; i++) {
					c = (unsigned char) gethexnum(inbuf, 9+i*2, 2);
					if (i==0)  {
						hiaddr = c;
						hiaddr <<= 24;
					}
					else  {
						hiaddr |= (c<<16);
					}
					cs += c;
				}
				address = hiaddr;
				cs2 = (unsigned char) gethexnum(inbuf, 9+i*2, 2);
				break;
			}

			case SLAR:  {			// 05 - Start Linear Address Record (32-bit format only)
				for(i=0; i<(int)n; i++) {
					c = (unsigned char) gethexnum(inbuf, 9+i*2, 2);
					cs += c;
				}
				cs2 = (unsigned char) gethexnum(inbuf, 9+i*2, 2);
				break;
			}

			//---------------------------------------------------------------------------

			case ESAR:				// 02 - extended segment address record
				throw (fatalError(__FILE__, __LINE__, "ESAR not implemented"));
				break;
			case SSAR:				// 03 - extended linear address record
				throw (fatalError(__FILE__, __LINE__, "SSAR not implemented"));
				break;
			default:
				throw (fatalError(__FILE__, __LINE__));
				break;
		}



		cs = ~cs + 1;

#ifdef _DEBUG
		if (cs != cs2)  {
			throw (fatalError(__FILE__, __LINE__));
		}
		assert(cs==cs2);
#endif

		//bp = 1;
	}

	fclose(instream);
	FCLOSE(outstream);

	// calculate the number of 8k blocks

	float f = (float)(end_address - start_address)/8192.0f;								// 7.10938f

	binsize = end_address - start_address;

	//int nblocks = (int)ceil(f);
//#pragma warning(once:4244)
	int nblocks = (int)ceil(f);

	blockmask = 0L;

	unsigned long mask = 0x00000001;

	for(i=0; i<nblocks; i++)  {
		blockmask |= mask;
		mask <<= 1;
	}

	return;
}

/****************************************************************************************************
	addr = rom org (hex)
****************************************************************************************************/

void INTEL::makehex(const char *_infname, unsigned long _start_addr)  {
	char infname[256];
	char outfname[256];
	unsigned long addr;
	int i, j;
	FILE *instream,*outstream;
	char line[128];
	char c;
	unsigned short checksum;

	strncpy(infname, _infname, sizeof(infname)-1);
	addr = _start_addr;

	strncpy(outfname, _infname, sizeof(infname)-1);

   if (!has_extension(infname))  {
      strcat(infname, ".bin");
   }
   instream = fopen(infname, "rb");
   if (instream==NULL)  {
		throw (fatalError(__FILE__, __LINE__));
   }

	for (i=0;i<32;i++)  {
		if (outfname[i]=='.')  {
			break;
		}
	}

   outfname[i] = 0;
   strcat(outfname, ".hex");
   outstream = fopen(outfname, "wt");

   line[0] = ':';
   line[1] = '1';
   line[2] = '0';
   line[7] = '0';
   line[8] = '0';
   line[43] = 0;

   for (i=0;i<1024;i++)  {							// for a 16k chip
      sprintf(&line[3], "%04X", addr);
      line[7] = '0';

      checksum = (unsigned short) (0x10 + (addr & 0x00ff) + (addr >> 8));

      for (j=0;j<16;j++)  {
			c = fgetc(instream);
			sprintf(&line[j*2+9], "%02X", c);
			checksum += c;
      }

      checksum = ~checksum + 1;		/* 2's complement */
      sprintf(&line[41],"%02X\n",checksum);
      fprintf(outstream,"%s",line);
      addr += 16;
   }

	// add closing record

   fprintf(outstream,":00000001FF");

   fclose(instream);
   fclose(outstream);

	return;
}


/****************************************************************************************************

****************************************************************************************************/

unsigned long INTEL::gethexnum(char *buf, int offs, int len)  {
	unsigned long ul;
	int n;
	char c;

	c = buf[offs+len];
	buf[offs+len] = 0;

	n = sscanf(&buf[offs], "%lx", &ul);

	buf[offs+len] = c;

	if (n != 1)  {
		throw (fatalError(__FILE__, __LINE__));
	}

	return ul;
}

/****************************************************************************************************
	creates a intel hex file from buf[size] at addr
****************************************************************************************************/
		/*

:	n		addr	type	n bytes of data

:	02		0000	04		00 03 F7
:	40		C000	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
:	40		C040	00		00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000C0
:	40		C080	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000080
:	40		C0C0	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040
:	40		C100	00		00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000FF
:	40		C140	00		00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000BF
:	40		C180	00		000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007F
:	40		C1C0	00		000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003F
:	00		0000	01		FF
		*/

int INTEL::makehex(unsigned char *buf, int size, unsigned long _addr, const char *outfilename)  {
	FILE *stream;
	int i, j, k, rows, remainder;
	unsigned long addr;
	unsigned short us;
	unsigned char c, c1, c2, cs;
	unsigned short addr16;

	rows = size / 64;
	remainder = size - rows*64;
	if (remainder != 0)  {
		return 1;
	}

	stream = fopen(outfilename, "wt");

	addr = _addr;
	us = (unsigned short)(addr>>16);

	c1 = (us>>8) & 0x00ff;
	c2 = us & 0x00ff;
	cs = 0x02 + c1 + c2 + 0x04;
	cs = ~cs + 1;
	fprintf(stream, ":02000004%02X%02X%02X\n", c1, c2, cs);

	cs = 0;
	addr16 = (unsigned short) (addr & 0x0000ffff);

	for(i=0; i<rows; i++)  {
		k = i*64;
		// :	40		C000	00		0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
		fprintf(stream, ":40%04X00", addr16);
		cs = 0x40 + (addr16&0x00ff) + ((addr16>>8)&0x00ff) + 0x00;

		for(j=0; j<64; j++)  {
			c = buf[k+j];
			fprintf(stream, "%02X", c);
			cs += c;
		}
		cs = ~cs + 1;
		fprintf(stream, "%02X\n", cs);
		addr16 += 64;
	}

	fprintf(stream, ":00000001FF\n");

	FCLOSE(stream);

	return 0;
}

