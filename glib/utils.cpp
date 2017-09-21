#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#endif

/*********************************************************************

*********************************************************************/

#include <math.h>
#include <time.h>
#include <assert.h>

#include <algorithm>

#include <comutils.h>

#include <glib_defines.h>
#include <crf.h>
//#include <md5.h>
#include <logger.h>


#include <utils.h>
#include "bmp.h"					// avoid ffmpegs version

/**************************************************************************

**************************************************************************/

bool valid_filename(const char *fname)  {
	int i, len;
	len = (int)strlen(fname);

	for(i=0; i<len; i++)  {
		if (fname[i] <' ' || fname[i]>0x7e)  {
			return false;
		}
	}

	return true;
}



/**************************************************************************

**************************************************************************/

unsigned char low(unsigned short us)  {
	return (us & 0x00ff);
}

/**************************************************************************

**************************************************************************/

unsigned char high(unsigned short us)  {
	return ((us>>8) & 0x00ff);
}

//#if 0
#ifdef WIN32
/**********************************************************************
	keeps reading until the timeout occurs
**********************************************************************/

#define FLUSH_STR_LEN 256

void flush(Logger *logg, Serial *port,DWORD timeout, BOOL echo)  {
	//int status;
	unsigned char c[8];
	DWORD start,end;
	int i;
	int n;
	char str[FLUSH_STR_LEN];


	if (logg) logg->write(0,0,0,"\nflush:\n");

//	start = end = GetTickCount();
	start = end = timeGetTime();
   i = 0;

	while(1)  {
//		status = rx(port,&c[0]);
//    if (status)  {

		//status = port->Receive(c,1,&n);

		n = port->rx((char *)c, 1);

//		if (status==0 && n==1)  {
		if (n==1)  {
      	str[i++] = c[0];
			if (echo) {
				if (logg) logg->write(0,0,0,"%c",c[0]);
			}
         if (i==FLUSH_STR_LEN-1)  {
	      	if (logg) logg->write(0,0,0,"i = 255 end =   %lu (%lu)\n",end,timeout);
   	   	if (logg) logg->write(0,0,0,"i = 255 start = %lu (%lu)\n",start,end-start);
            for(int j=0;j<FLUSH_STR_LEN;j++)  {
            	if (logg) logg->write(0,0,0,"%c",str[j]);
            }
         	break;
         }
		}
//      end = GetTickCount();
      end = timeGetTime();
      if ((end-start)>timeout)  {
      	if (logg) logg->write(0,0,0,"flush timeout end =   %lu (%lu)\n",end,timeout);
      	if (logg) logg->write(0,0,0,"flush timeout start = %lu (%lu)\n",start,end-start);
	     	break;
		}
	}

   str[i] = 0;

	return;
}							// flush()

#endif



/***********************************************************************************

***********************************************************************************/

void CopyFRect(FRECT *dest, FRECT *src)  {
	memcpy(dest, src, sizeof(FRECT));
	return;
}

/**********************************************************************

**********************************************************************/

int sign(float f)  {
	if (f<0.0f) return -1;
	if (f>0.0f) return 1;
	return 0;
}

	/*******************************************************************
	  returns the magnitude of a vector
	 *******************************************************************/

	//double mag(double x, double y, double z)  {
	double vmag(double x, double y, double z)  {
		return sqrt(x*x + y*y + z*z );
	}

/************************************************************************************

************************************************************************************/

float frand(float min, float max)  {
	return (max - min) * ((float) rand() / RAND_MAX) + min;
}


	/*********************************************************************

	 *********************************************************************/

	bool rm(const char *fname)  {

#ifdef _DEBUG
		char curdir[256];
		GetCurrentDirectory(sizeof(curdir), curdir);
#endif

		if (exists(fname))  {
			int status = unlink(fname);
			if (status!=0)  {
				return false;
			}
		}

		return true;
	}

/**********************************************************************
  t r i m c r l f
  trims trailing spaces and tabs and cr's and lf's from the string
**********************************************************************/

void trimcrlf(char *str)  {
	int i;

	i = (int)strlen(str)-1;

	if (i<0)  {
		return;							// already trimmed
	}


	while ((str[i]==0x0a) || (str[i]==0x0d) || (str[i]==' ') || (str[i]=='\t'))  {
		i--;
		if (i<0)  {
			break;
		}
	}

	i++;
	str[i] = 0;

	return;

}

/**********************************************************************
  t r i m c r l f
  trims trailing spaces and tabs and cr's and lf's from the string
**********************************************************************/

char *fulltrim(char *_str)  {
	int i;
	static char str[256]={0};

	i = (int)strlen(_str)-1;

	if (i<0 || i > (int)sizeof(str)-1)  {
		return _str;							// already trimmed
	}

	strcpy(str, _str);

	trimcrlf(str);
	ltrimcrlf(str);

	return str;
}							// fulltrim()

/**************************************************************************

 **************************************************************************/

unsigned long filesize_from_stream(FILE *stream)  {
	int size;
	long offset;

	offset = ftell(stream);				// save the offset
	fseek(stream, 0L, SEEK_END);
	size = (int)ftell(stream);
	fseek(stream, offset, SEEK_SET);		// go back to where we were
	return size;
}

/********************************************************************

  searches str1 for str2 beginning at startpos.

  returns string index if str2 is found.

  return -1 if str2 is not found.

  startpos is 0 if reverse is true.

 ********************************************************************/

int indexIgnoreCase(const char *str, const char *pat, int startpos, bool reverse)  {
	int len1, len2;
	int i,j;

	len1 = (int)strlen(str);
	len2 =(int)strlen(pat);
	j = 0;

	if (reverse)  {
		//strring s;
		//char tmpstr[256];
		//strncpy(tmpstr, pat, sizeof(tmpstr)-1);
		//reverse(tmpstr);

		for(i=len1-1-startpos; i>=0; i--)  {
			if (toupper(str[i]) == toupper(pat[j]))  {
				j++;
				if (j==len2)  {
					return i-len2+1;				// point to the beginning of the substring
				}
			}
			else  {
				j = 0;
			}
		}
	}
	else  {
		for(i=startpos; i<len1; i++)  {
			if (toupper(str[i]) == toupper(pat[j]))  {
				j++;
				if (j==len2)  {
					return i-len2+1;				// point to the beginning of the substring
				}
			}
			else  {
				j = 0;
			}
		}
	}
	return -1;
}


/********************************************************************

  searches str for str2 beginning at startpos.

  returns string index if str2 is found.

  return -1 if str2 is not found.

 ********************************************************************/

int myindex(const char *str1, const char *str2, int startpos )  {
	int len1, len2;
	int i,j;

	len1 = (int)strlen(str1);
	len2 = (int)strlen(str2);

	j = 0;


	for(i=startpos; i<len1; i++)  {
		if (str1[i] == str2[j])  {
			j++;
			if (j==len2)  {
				return i-len2+1;				// point to the beginning of the substring
			}
		}
		else  {
			j = 0;
		}
	}

	return -1;
}

/********************************************************************

  searches str1 for str2 beginning at startpos.

  returns string index if str2 is found.

  return -1 if str2 is not found.

 ********************************************************************/

int myindex(char *str1, const char *str2, std::vector<int> &a)  {
	int len1, len2;
	int i,j,k;


	len1 = (int)strlen(str1);
	len2 = (int)strlen(str2);

	j = 0;


	for(i=0; i<len1; i++)  {
		if (str1[i] == str2[j])  {
			j++;
			if (j==len2)  {
				//return i-len2+1;				// point to the beginning of the substring
				k = i-len2+1;
				a.push_back(k);
				j = 0;
			}
		}
		else  {
			j = 0;
		}
	}

	return (int)a.size();
}


/********************************************************************
  tests the equality of two strings.
 ********************************************************************/

bool strEqual(char *str1, char *str2)  {
	int len1, len2;

	len1 = (int)strlen(str1);
	len2 = (int)strlen(str2);
	if (len1 != len2)  {
		return false;
	}

	for(int i=0; i<len1; i++)  {
		if (str1[i] != str2[i])  {
			return false;
		}
	}

	return true;

}

/**********************************************************************

 **********************************************************************/

void get_extension(char *fname)  {
	int pos;

	pos = indexIgnoreCase(fname, ".", 0, true);
	if (pos == -1)  {
		return;
	}

	strcpy(fname, &fname[pos+1]);

	return;
}

/**********************************************************************

 **********************************************************************/

void strip_extension(char *fname)  {

	int pos;

	pos = indexIgnoreCase(fname, ".", 0, true);
	if (pos == -1)  {
		return;
	}

	fname[pos] = 0;

	return;
}

/**********************************************************************

**********************************************************************/

bool has_extension(char *_fname)  {
	if (myindex(_fname, ".") == -1)  {
		return false;
	}
	return true;
}


/**********************************************************************
  returns the path only of a full fileneme
 **********************************************************************/

void strip_filename(char *fname)  {
	int len;
	int i;
	//	char str[32];

	len = (int)strlen(fname);

	for(i=len-1;i>=0;i--)  {
		if (fname[i]==FILESEPCHAR)  {
			break;
		}
	}

	if(i==-1)  {
		return;
	}

	fname[i] = 0;

	return;
}

/**********************************************************************

 **********************************************************************/

void strip_path(char *fname)  {
	int len;
	int i,j,k;
	char str[256];

	len = (int)strlen(fname);

	for(i=len-1;i>=0;i--)  {
		if (fname[i]==FILESEPCHAR)  {			// '\'
			break;
		}
		if (fname[i]=='/')  {					// '/'
			break;
		}
	}

	if(i==-1)  {
		return;
	}

	k = 0;
	for(j=i+1;j<len;j++)  {
		str[k++] = fname[j];
		if (k==sizeof(str)-1)  {
			break;
		}
	}
	str[k] = 0;

	//if (logg) logg.write(0,0,1,"strip_path exit: str = %s\n",str);
	strcpy(fname, str);

	return;
}

	/***************************************************************************************

	 ***************************************************************************************/

	void bit_reverse(unsigned char *b)  {
		unsigned char c;

		c = *b;		// get the byte

		c = ((c >>  1) & 0x55) | ((c <<  1) & 0xaa);
		c = ((c >>  2) & 0x33) | ((c <<  2) & 0xcc);
		c = ((c >>  4) & 0x0f) | ((c <<  4) & 0xf0);

		*b = (unsigned char)c;

		/**************

note: to bit reverse 32 bits do this:

unsigned long n;

n = ((n >>  1) & 0x55555555L) | ((n <<  1) & 0xaaaaaaaaL);
n = ((n >>  2) & 0x33333333L) | ((n <<  2) & 0xccccccccL);
n = ((n >>  4) & 0x0f0f0f0fL) | ((n <<  4) & 0xf0f0f0f0L);
n = ((n >>  8) & 0x00ff00ffL) | ((n <<  8) & 0xff00ff00L);
n = ((n >> 16) & 0x0000ffffL) | ((n << 16) & 0xffff0000L);

		 ***************/


		return;
	}

	/***************************************************************************************
	  returns -1 if not a hex string
	 ***************************************************************************************/

	int gethex(unsigned char *str)  {
		unsigned char ctemp;
		unsigned char str2[3] = {0,0,0};
		unsigned char hi, lo;

		if (!isxdigit(str[0]))  {
			return -1;
		}
		if (!isxdigit(str[1]))  {
			return -1;
		}

		//	strncpy((char *)str2, (char *)str, 2);
		str2[0] = str[0];
		str2[1] = str[1];
		str2[2] = 0;
		strupr((char *)str2);


		if (str2[0]<'A')  {
			hi = (unsigned char) ((str2[0]-'0')*16);
		}
		else  {
			hi = (unsigned char) ((str2[0]-'A'+10)*16);
		}

		if (str2[1]<'A')  {
			lo = (str2[1]-'0');
		}
		else  {
			lo = (str2[1]-'A'+10);
		}

		ctemp = hi + lo;
		ctemp = hi | lo;

		return (ctemp & 0x00ff);
	}

	/**********************************************************************
	  trims leading spaces (not tabs) from the string if flag is 0.
	  Also trims tabs if trim_tabs is 1
	 **********************************************************************/

	void ltrim(char *string, int trim_tabs, char *ends)  {
		int i,j,k;

		if (ends==NULL)  {
			i = 0;

			if (trim_tabs)  {
				while (string[i] == ' ' || string[i]=='\t')  {
					i++;
				}
			}
			else  {
				while (string[i] == ' ')  {
					i++;
				}
			}


			k = (int)strlen(string);
			for(j=0;j<(k-i);j++)  {
				string[j] = string[j+i];
			}
			string[j] = 0;
		}
		else  {
			char c;
			bool runflag = true;
			int len;
			i = 0;

			len = (int)strlen(ends);

			while(runflag)  {
				c = string[i];

				for(j=0; j<len; j++)   {
					if (c==ends[j])  {
						i++;
						break;
					}
				}
				if (j==len)  {
					runflag = false;
				}
			}

			k = (int)strlen(string);
			for(j=0;j<(k-i);j++)  {
				string[j] = string[j+i];
			}
			string[j] = 0;
		}

	}


	/********************************************************************

	  searches str for 'c' beginning at end and working backwards

	  returns string index if str2 is found.

	  return -1 if str2 is not found.

	 ********************************************************************/

	int indexCharFromEnd(char *str1, char c)  {
		int len;
		int i;


		len = (int)strlen(str1);


		for(i=len-1; i>=0; i--)  {
			if (str1[i] == c)  {
				return i;
			}
		}

		return -1;
	}



	/************************************************************
	  replaces ALL occurrences of the substring oldstr with newstr int str.
	  if flag is 1

	  just the first occurrence if flag is 0


	  returns 0 if substitution was made
	  returns 1 if no substitution made

	 ************************************************************/


	int replace(char *string, const char *oldstr, const char *newstr, int flag)  {
		char tempstr[1024];
		int start, end;
		int i;
		int pos=0;


		while(1)  {

			start = myindex(string, oldstr, pos);
			if (start==-1)  {
				return 1;
			}

			end = start + (int)strlen(oldstr) - 1;

			for(i=0;i<start;i++)  {
				tempstr[i] = string[i];
			}
			tempstr[i] = 0;

			strcat(tempstr, newstr);
			strcat(tempstr, &string[end+1]);
			strcpy(string,tempstr);
			pos = end + (int)strlen(newstr);

			if (flag==0)  {
				break;
			}
		}

		return 0;

	}

	/**********************************************************************
	  t r i m
	  trims trailing spaces and tabs from the string.

		if ends is not null then each character within it is also trimmed
	 **********************************************************************/

	void trim(char *string, const char *ends)  {
		int i,j,m;

		i = (int)strlen(string)-1;
		if (i<0)  {
			return;							// already trimmed
		}

		if (ends)  {
			m = (int)strlen(ends);
		}

		//while ((string[i]==' ') || (string[i]=='\t'))  {
		while(1)  {
			if (string[i] != ' ')  {
				if (string[i] != '\t')  {
					if (ends)  {
						bool flag = false;
						for(j=0; j<m; j++)  {
							if (string[i] == ends[j])  {
								flag = true;
							}
						}
						if (!flag)  {
							break;
						}
					}
					else  {
						break;
					}
				}
			}

			i--;
			if (i<0)  {
				break;
			}
		}

		i++;
		string[i] = 0;

		return;
	}

	/************************************************************************************
	  removes pat from str
	 ************************************************************************************/


	void myremove(const char *pat, char *str)  {
		int i, len, patlen;
		int pos;

		pos = 0;

		while(1)  {
			pos = myindex(str, pat, pos);
			if (pos==-1)  {
				return;
			}
			patlen = (int)strlen(pat);
			len = (int)strlen(str);


			for(i=pos; i<(len-patlen); i++)  {
				str[i] = str[i+patlen];
			}

			str[i] = 0;
		}


		return;
	}

	/*********************************************************************

	*********************************************************************/

	bool isodd(int i)  {
		int k;
		k = i%2;
		if (k)
			return true;
		else
			return false;
	}

	/**********************************************************************

	 **********************************************************************/


	unsigned long Checksum(char *filename)  {
		FILE *stream;
		unsigned short ul=0L;
		int status;


		stream = fopen(filename, "rb");

		if (stream==NULL)  {
			return 0;
		}

		while(1)  {
			status = fgetc(stream);
			if (status==EOF)  {
				break;
			}
			ul += status;
		}



		fclose(stream);

		return ul;

	}

	/**********************************************************************
	  BIG NOTE: THIS CLOSES AND REOPENS DUMP2.TXT
	 **********************************************************************/

	void dump2(FILE *dumpstream, unsigned char *mb, int cnt)  {
		int fullrows, remainder;
		int i, j, k;


		fullrows = (cnt/16);
		remainder = (cnt - fullrows*16);


		for (i=0; i<fullrows; i++)  {
			for (j=0;j<16;j++)  {
				k = i*16 + j;
				fprintf(dumpstream, "%02x ", mb[k]);
			}

			fprintf(dumpstream, "    ");

			for (j=0;j<16;j++)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					fprintf(dumpstream, ".");
				}
				else  {
					fprintf(dumpstream, "%c", (char)mb[k]);
				}
			}

			fprintf(dumpstream, "\n");
		}

		//----------------------
		// now the remainder:
		//----------------------


		for (j=0;j<16;j++)  {
			k = i*16 + j;
			if (j<remainder)  {
				fprintf(dumpstream, "%02x ", mb[k]);
			}
			else  {
				fprintf(dumpstream,"   ");
			}
		}

		fprintf(dumpstream,"    ");

		for (j=0;j<16;j++)  {
			if (j<remainder)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					fprintf(dumpstream, ".");
				}
				else  {
					fprintf(dumpstream, "%c", (char)mb[k]);
				}
			}
			else  {
				//fprintf(stream," ");
			}
		}

		fprintf(dumpstream, "\n");

		fclose(dumpstream);
		dumpstream = fopen("dump2.txt", "a+t");

	}


	/**********************************************************************
	  BIG NOTE: THIS CLOSES AND REOPENS DUMP2.TXT
	 **********************************************************************/

	unsigned short checksum(unsigned char *ptr, int len)  {
		unsigned short cs=0;

		for (int i=0;i<len;i++)  {
			cs += *ptr++;
		}

		return cs;
	}

#ifdef _DEBUG
	/****************************************************************************************

	  computes the monthly payment for a loan

	  P = principle
	  I = yearly interest rate
	  years = total years for loan

	 ****************************************************************************************/

	float compute_payment(float P, float I, int years)  {
		float M;
		float J = I / 12.0f;		// interest rate per month
		int N = 12*years;			// total number of months for loan

		M = (float) (P * ( J / (1.0f - pow( (1.0f+J), -N) ) ) );

		return M;

	}

	/****************************************************************************************

	  computes the remaining principal

	  _P = principal, the initial amount of the loan
	  _I = the annual interest rate (from 1 to 100 percent)
	  _L = length, the length (in years) of the loan, or at least the length over which the loan is amortized.
	  _t=number of paid monthly loan payments

	  from www.interest.com

	  P = P * (1 - ((1 + J) ** t - 1) / ((1 + J) ** N - 1))


	 ****************************************************************************************/

	//float compute_remaining_principal(float _P, float _I, int _L, int _t)  {
	double compute_remaining_principal(double _P, double _I, int _L, int _t)  {
		double P;
		double J = _I / 12.0f;		// monthly interest
		int N = 12*_L;					// number of months over which loan is amortized = L x 12
		//double d;

		/*
		P = (float) (
				_P * (
					1.0 - (pow(1+J, _t) - 1.0) / (pow(1.0+J, N) - 1.0)
					)
				);
		*/

		P = ( _P * ( 1.0 - (pow(1+J, _t) - 1.0) / (pow(1.0+J, N) - 1.0) ) );

		return P;
	}

	/*****************************************************************************************
	  principal is the initial principal
	  interest is the anual interest rate
	  years is the number of years of the loan
	  month is the month of the year of the first payment (1-12). Used to print the accumulated
	  interest at the end of the calendar year.

	 *****************************************************************************************/

	void print_amortization_table(float principal, float interest, int years, int month)  {
		float payment;
		float remaining_principal;
		float principal_paid, interest_paid;
		int j;
		float ftemp;
		float yearly_interest = 0.0f;

		payment = compute_payment(principal, interest, years);
		remaining_principal = principal;

		printf("\nmonth  principal  interest   remaining   accum interest\n\n");

		for(j=1; j<=years*12; j++)  {
			ftemp = remaining_principal;
			remaining_principal = (float)compute_remaining_principal(principal, interest, years, j);
			principal_paid = ftemp - remaining_principal;
			interest_paid = payment - principal_paid;
			yearly_interest += interest_paid;
			printf("%3d    %7.2f    %7.2f    %7.2f   %7.2f\n", j, principal_paid, interest_paid, remaining_principal, yearly_interest);
			month--;
			if (month==0)  {
				printf("\n");
				month = 12;
				yearly_interest = 0.0f;
			}

		}


		return;
	}
#endif


	/**********************************************************************

	**********************************************************************/

	void dump(unsigned char *mb, int cnt, FILE *stream, char *leader)  {
		int fullrows, remainder;
		int i, j, k;


		fullrows = (cnt/16);
		remainder = (cnt - fullrows*16);


		for (i=0; i<fullrows; i++)  {
			if (leader) fprintf(stream, "%s", leader);
			for (j=0;j<16;j++)  {
				k = i*16 + j;
				fprintf(stream, "%02x ", mb[k]);
			}

			fprintf(stream, "    ");

			for (j=0;j<16;j++)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					fprintf(stream, ".");
				}
				else  {
					fprintf(stream, "%c", (char)mb[k]);
				}
			}

			fprintf(stream, "\n");
		}

		//----------------------
		// now the remainder:
		//----------------------

		if (leader) fprintf(stream, "%s", leader);

		for (j=0;j<16;j++)  {
			k = i*16 + j;
			if (j<remainder)  {
				fprintf(stream, "%02x ", mb[k]);
			}
			else  {
				fprintf(stream,"   ");
			}
		}

		fprintf(stream,"    ");

		for (j=0;j<16;j++)  {
			if (j<remainder)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					fprintf(stream, ".");
				}
				else  {
					fprintf(stream, "%c", (char)mb[k]);
				}
			}
			else  {
				//fprintf(stream," ");
			}
		}

		fprintf(stream, "\n");
	}

	/**********************************************************************

	**********************************************************************/

	void dump(unsigned char *mb, int cnt)  {
		int fullrows, remainder;
		int i, j, k;


		fullrows = (cnt/16);
		remainder = (cnt - fullrows*16);


		for (i=0; i<fullrows; i++)  {
			for (j=0;j<16;j++)  {
				k = i*16 + j;
				printf("%02x ", mb[k]);
			}

			printf("    ");

			for (j=0;j<16;j++)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					printf(".");
				}
				else  {
					printf("%c", (char)mb[k]);
				}
			}

			printf("\n");
		}

		//----------------------
		// now the remainder:
		//----------------------


		for (j=0;j<16;j++)  {
			k = i*16 + j;
			if (j<remainder)  {
				printf("%02x ", mb[k]);
			}
			else  {
				printf("   ");
			}
		}

		printf("    ");

		for (j=0;j<16;j++)  {
			if (j<remainder)  {
				k = i*16 + j;
				if (mb[k]<0x20 || mb[k] > 0x7e)  {
					printf(".");
				}
				else  {
					printf("%c", (char)mb[k]);
				}
			}
			else  {
				//fprintf(stream," ");
			}
		}

		printf("\n");

		return;
	}

/**********************************************************************

**********************************************************************/

void dump(char *_infname, char *_outfname, unsigned long _addr)  {
	int fullrows, remainder;
	int i, j, status;
	FILE *instream=NULL;
	FILE *outstream=NULL;
	int size;
	unsigned char c;
	unsigned char buf[16];
	unsigned long addr;

	instream = fopen(_infname, "rb");
	if (instream==NULL)  {
		return;
	}

	size = (int)filesize_from_name(_infname);
	if (size==0)  {
		return;
	}

	fullrows = (size/16);
	remainder = (size - fullrows*16);

	outstream = fopen(_outfname, "wt");
	addr = _addr-16;

	for (i=0; i<fullrows; i++)  {

		for (j=0;j<16;j++)  {
			status = fgetc(instream);
			if (status==EOF)  {
				break;
			}
			c = status & 0x000000ff;
			buf[j] = c;
		}

		addr += 16;
		fprintf(outstream, "%08lx  ", addr);
		for (j=0;j<16;j++)  {
			fprintf(outstream, "%02x ", buf[j]);
		}

		fprintf(outstream, "    ");

		for (j=0;j<16;j++)  {
			if (c<0x20 || c > 0x7e)  {
				fprintf(outstream, ".");
			}
			else  {
				fprintf(outstream, "%c", (char)buf[j] );
			}
		}

		fprintf(outstream, "\n");
	}

	//----------------------
	// now the remainder:
	//----------------------

	if (remainder > 0)  {
		addr += 16;
		fprintf(outstream, "%08lx  ", addr);

		for (j=0;j<16;j++)  {
			status = fgetc(instream);
			if (status==EOF)  {
				break;
			}
			c = status & 0x000000ff;
			buf[j] = c;

			if (j<remainder)  {
				fprintf(outstream, "%02x ", buf[j]);
			}
			else  {
				fprintf(outstream,"   ");
			}
		}

		fprintf(outstream,"    ");

		for (j=0;j<16;j++)  {
			if (j<remainder)  {
				if (c<0x20 || c>0x7e)  {
					fprintf(outstream, ".");
				}
				else  {
					fprintf(outstream, "%c", buf[j] );
				}
			}
			else  {
				//fprintf(outstream," ");
			}
		}

		fprintf(outstream, "\n");
	}


	FCLOSE(instream);
	FCLOSE(outstream);

	return;
}


/**********************************************************************
  inserts str2 into str1 at str1[k]
**********************************************************************/


	int insert(char *str1, const char *str2, int k)  {
		int len;
		char tmpstr[256];
		char *cptr = tmpstr;

		len = (int)strlen(str1) + (int)strlen(str2);
		if (len>255)  {
			//return 1;				// tmpstr not big enough
			cptr = new char[1024];
		}

		strcpy(cptr, &str1[k]);

		str1[k] = 0;

		strcat(str1, str2);
		strcat(str1, cptr);

		if (len>255)  {
			delete [] cptr;
		}
		return 0;
	}

#ifdef _DEBUG
	/**********************************************************************
	  this returns the number of calories that are required to maintain
	  the given weight.

	  "Harris-Benedict" formula for calculating the BMR:

	  "A Biometric Study of Basal Metabolism"
	  Publication no.279, Washington, D.C., Carnegie Institute of Washington, 1919


	  Harris Benedict Formula for Men
	  BMR = 66 + (13.7 X weight in kilos) + (5 X height in cm) - (6.8 X age in years)

Notes:
1 inch = 2.54 cm.
1 kilogram = 2.2 lbs.

Example of BMR
You are 25 years old
You are 6 feet tall
Your weight is 220 pounds
Your BMR is 66 + (1370) + (914) - (170) = 2180 calories

Harris Benedict Formula for Men - STEP 2

To determine your total daily calorie needs, now multiply your BMR by the
appropriate activity factor, as follows:

If you are Sedentary - little or no exercise
Calorie-Calculation = BMR X 1.2
If you are Lightly Active (light exercise/sports 1-3 days/week)
Calorie-Calculation = BMR X 1.375
If you are Moderately Active (moderate exercise/sports 3-5 days/week)
Calorie-Calculation = BMR X 1.55
If you are Very Active = BMR X 1.725 (hard exercise/sports 6-7 days/week)
Calorie-Calculation = BMR X 1.725
If you are Extra Active (very hard daily exercise/sports & physical job or 2X day training)
Calorie-Calculation = BMR X 1.9

Total Calorie Needs Example

If you are lightly active, multiply your BMR (2180) by 1.375 = 2997
Your total daily calorie requirement is therefore 2997 calories.

This is the total number of calories you need in order to MAINTAIN your current
weight.

---------------------------------------------------------------

Harris Benedict Formula for Women - STEP 1
BMR = 655 + (9.6 X weight in kilos) + (1.8 X height in cm) - (4.7 X age in years).

Notes:
1 inch = 2.54 cm.
1 kilogram = 2.2 lbs.

Example of BMR
You are 32 years old
You are 5 feet 4 inches tall (162.5 cm)
Your weight is 185 pounds (84 kilos)
Your BMR is 655 + (806) + (291) - (150) = 1602 calories

Harris Benedict Formula for Women - STEP 2
To determine your total daily calorie needs, now multiply your BMR by the appropriate activity factor, as follows:

0	If you are Sedentary - little or no exercise																		Calorie-Calculation = BMR X 1.2
1	If you are Lightly Active (light exercise/sports 1-3 days/week)											Calorie-Calculation = BMR X 1.375
2	If you are Moderately Active (moderate exercise/sports 3-5 days/week)									Calorie-Calculation = BMR X 1.55
3	If you are Very Active = BMR X 1.725 (hard exercise/sports 6-7 days/week)								Calorie-Calculation = BMR X 1.725
4	If you are Extra Active (very hard daily exercise/sports & physical job or 2X day training)		Calorie-Calculation = BMR X 1.9

Total Calorie Needs Example
	If you are sedentary, multiply your BMR (1602) by 1.2 = 1922
		Your total daily calorie requirement is therefore 1922 calories.
		This is the total number of calories you need in order to MAINTAIN your current weight.


		**********************************************************************/


		float bmr(float _lb, float _inches, float _years, char _sex, int _activityIndex)  {
			float f=0.0f;
			float kg = (float) (TOKGS*_lb);
			float cm = (float) (INCHES_TO_CM * _inches);
			float factor[5] = { 1.2f, 1.375f, 1.55f, 1.725f, 1.9f };

			if (_sex=='M' || _sex=='m')  {
				f = 66.0f + (13.7f * kg) + (5.0f * cm) - (6.8f * _years);
				f *= factor[_activityIndex];
			}
			else if (_sex=='F' || _sex=='f')  {
				f = 655.0f + (9.6f*kg) + (1.8f*cm) - (4.7f*_years);
				f *= factor[_activityIndex];
			}
			else  {
				return 0.0f;
			}

			return f;
		}
#endif		// #ifdef _DEBUG


	/*********************************************************************

	 *********************************************************************/

	bool isNumericString(const char *_str)  {
		int i, n;
		char c;

		n = (int)strlen(_str);
		if (n==0)  {
			return false;
		}

		for(i=0;i<n;i++)  {
			c = _str[i];
			if(!isdigit(c))  {											// 0 - 9
				if (c!='.' && c!='+' && c!='-')  {					// .+-
					return false;
				}
				if (c=='-')  {
					if (i != 0)  {
						// eg, a date like 2007-01 is not a numeric string
						return false;
					}
				}
			}
		}

		return true;
	}


	/******************************************************************************************

	 ******************************************************************************************/

	void swap(int *a, int *b)  {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
		// note that this is not portable:
		//a ^= b ^= a ^= b;
		return;
	}

	/******************************************************************************************

	 ******************************************************************************************/

	void swap_floats(float *f1, float *f2)  {
		float ftmp;
		ftmp = *f1;
		*f1 = *f2;
		*f2 = ftmp;
		return;
	}

	/******************************************************************************************
	  Bit counter by Ratko Tomic
	 ******************************************************************************************/

	int countbits(long i)  {
		i = ((i & 0xAAAAAAAAL) >>  1) + (i & 0x55555555L);
		i = ((i & 0xCCCCCCCCL) >>  2) + (i & 0x33333333L);
		i = ((i & 0xF0F0F0F0L) >>  4) + (i & 0x0F0F0F0FL);
		i = ((i & 0xFF00FF00L) >>  8) + (i & 0x00FF00FFL);
		i = ((i & 0xFFFF0000L) >> 16) + (i & 0x0000FFFFL);
		return (int)i;
	}


	/******************************************************************************************
	  Round an integer up to the nearsest alignment boundary.
	 ******************************************************************************************/

	int round_a(int n, int mod)  {
		int m;
		m = (n + mod - 1) - (n + mod - 1) % mod;

		//m = mod * ((n + mod - 1) / mod);

		// Assuming the mod is a power of 2, the rounding can take place using bitwise operators .
		//m = (n + mod - 1) & ~(mod - 1);

		return m;
	}

	/******************************************************************************************
	  Round a float up to the nearsest alignment boundary.
	 ******************************************************************************************/

	int round_af(float f, int mod)  {
		int m;
		int n;
		n = (int) (.5 + f);
		m = (n + mod - 1) - (n + mod - 1) % mod;

		//m = mod * ((n + mod - 1) / mod);

		// Assuming the mod is a power of 2, the rounding can take place using bitwise operators .
		//m = (n + mod - 1) & ~(mod - 1);

		return m;
	}

	/******************************************************************************************
	  returns f rounded to the nearest integer divisible by mod
	 ******************************************************************************************/

	int roundmod(float f, int mod)  {

		/*
			int i, k;

			i = (int)f;
			k = i%mod;
			k = (int)(.5 + mod/2.0);
			if (i%mod < k)  {
			i =  i - i%mod;
			}
			else  {
			i = i - (i%mod) + mod;
			}
			return i;
			*/


		/*
			int i, i2, k;
			k = (int)(mod / 2.0);
			i = (int)(.5 + f);
			i2 = ( i + k ) / mod;
			i = i2 * mod;
			*/

		int i;

		i = (int)f/mod;
		if ( (f-(i*mod)) >= (float)mod/2.0f)  {
			i = (i + 1)*mod;
		}
		else  {
			i = i*mod;
		}

		return i;

	}

	/******************************************************************************************
	  counts the number of lines in a text file
	 ******************************************************************************************/

	long countlines(const char *fname, bool skip_blank)  {

		FILE *stream = NULL;
		char *cptr;
		char buf[1024];
		long count=0;

		stream = fopen(fname, "rt");
		if (stream==NULL)  {
			return 0L;
		}

		while(1)  {
			cptr = fgets(buf, sizeof(buf), stream);
			if (cptr==NULL)  {
				break;
			}
			if (skip_blank)  {
				trimcrlf(buf);
				if (buf[0]==0)  {
					continue;
				}
			}

			count++;
		}

		fclose(stream);

		return count;
	}

	/************************************************************************************

	 ************************************************************************************/

	bool getFileBuf(char *fname, long offset1, long offset2, char *buf, int buflen)  {
		FILE *stream=NULL;
		int len, status;

		memset(buf, 0, buflen);
		len = (int)(offset2 - offset1 + 1);

		stream = fopen(fname, "rb");
		if (stream==NULL)  {
			return false;
		}

		status = fseek(stream, offset1, SEEK_SET);
		if (status != 0)  {
			fclose(stream);
			return false;
		}

		status = (int)fread(buf, 1, len, stream);
		if (status != len)  {
			fclose(stream);
			return false;
		}

		fclose(stream);
		return true;
	}

	/************************************************************************************

	 ************************************************************************************/

	void get_last_line(char *fname, char *str, int size)  {
		FILE *stream;
		char *cptr;
		char buf[256];

		stream = fopen(fname, "rt");
		if (stream==NULL)  {
			memset(str, 0, size);
			return;
		}
		while(1)  {
			cptr = fgets(buf, sizeof(buf), stream);
			if (cptr==NULL)  {
				break;
			}
		}

		fclose(stream);
		strncpy(str, buf, size);


		return;
	}

	/************************************************************************************
	  returns the substring between a start character and an end character
	 ************************************************************************************/

	bool substr(char *_str, char _startchar, char _endchar, bool _inclusive, char *_outstr, int _outbufsize, int *_i1, int *_i2 )  {
		int i, j, len, n;

		len = (int)strlen(_str);

		for(i=0;i<len;i++)  {
			if (_str[i]==_startchar)  {
				break;
			}
		}
		if (i==len)  {
			return false;
		}
		*_i1 = i;

		for(j=i+1;j<len;j++)  {
			if (_str[j]==_endchar)  {
				break;
			}
		}
		if (j==len)  {
			return false;
		}
		*_i2 = j;

		if (_inclusive)  {
			n = j - i + 1;						// the length of the substring
			if (n > (_outbufsize-1) )  {
				return false;
			}
			strncpy(_outstr, &_str[i], n);
			_outstr[n] = 0;
		}
		else  {
			n = j - i - 1;						// the length of the substring
			if (n > (_outbufsize-1))  {
				return false;
			}
			strncpy(_outstr, &_str[i+1], n);
			_outstr[n] = 0;
		}

		return true;
	}

	/***********************************************************************

	 ***********************************************************************/


	bool exists(const char *fname)  {
		FILE *stream;

		if (fname==NULL)  {
			return false;
		}

		if (fname[0]==0)  {
			return false;
		}

		char str[MAX_PATH];
		strncpy(str, fname, sizeof(str)-1);

//#ifndef WIN32
//		if (myindex(str, "\\") != -1)  {
//			replace(str, "\\", "/", 1);
//		}
//#endif

		stream = fopen(str,"rb");
		if (stream==NULL)  {
			return false;
		}
		fclose(stream);
		return true;
	}

	/***********************************************************************

	 ***********************************************************************/

	bool replace_extension(char *fname, const char *new_extension)  {

#if 1
		int i, len;
		len = (int)strlen(fname);

		for(i=len-1; i>=0; i--)  {
			if (fname[i]=='.')  {
				break;
			}
		}

		if (i==len)  {
			return false;
		}
		fname[i] = 0;
		strcat(fname, ".");
		strcat(fname, new_extension);
		return true;

#else
		int pos;

		pos = myindex(fname, ".");
		if (pos==-1)  {
			return false;
		}
		if (pos==0)  {									// in case of a leading '.'
			pos = myindex(fname, ".", 1);
			if (pos==-1)  {
				return false;
			}
		}

		fname[pos] = 0;
		strcat(fname, ".");
		strcat(fname, new_extension);
#endif

		return true;
	}

/*******************************************************************************************

*******************************************************************************************/

bool change_extension(char *infname, char *ext1, char *outfname, int len, char *ext2)  {
	int pos;

	pos = myindex(infname, ext1);
	if (pos==-1)  {
		return false;
	}

	if (pos > (int) (len-strlen(ext2)-1) )  {
		return false;
	}

	outfname[pos] = 0;
	strncpy(outfname, infname, pos);
	strcat(outfname, ext2);

	return true;
}


	/******************************************************************************************

	  define strings this way (with a terminator):

	  char *take[] = {
	  "cell phone",
	  "cell phone clip",
	  "cell phone headset",
	  "sun glasses",
	  "sun glass clips",
	  ""									// terminator
	  };

	 ******************************************************************************************/

	int count_strings(char **strs)  {
		int i;

		i = 0;
		while(1)  {
			if (strs[i]==0)  {
				break;
			}
			i++;
		}

		return i;
	}


	/**********************************************************************
	  M E A N
	 **********************************************************************/

	double mean(float *data,short n)  {
		double sum=0.0;
		short i;

		for(i=0;i<n;i++)  {
			sum += data[i];
		}

		return sum/(double)n;
	}

	/**********************************************************************
	  M E D I A N
	 **********************************************************************/

	/*
		double median(float *data,short n)  {
		short itemp;

		itemp = (short)n;

		qsort(data,itemp,sizeof(float),(int (*)(const void *,const void *))fcomp);
		if ((itemp%2)!=0)  {												// if odd
		return (double)data[itemp/2];
		}
		else  {																// even
		return (data[itemp/2] + data[(itemp/2)-1])/2.0;
		}

		}
		*/

	/**********************************************************************
	  M O D E
	  don't call this mode because mode is a common variable name
	 **********************************************************************/

	double statmode(float *data,short n)  {
		short i;
		long *counts;
		float fmin,fmax;
		float m,b;
		short index,maxindex;
		double dtemp;


		counts = (long *)malloc(n*sizeof(long));

		fmin = fmax = data[0];

		for(i=0;i<n;i++)  {
			counts[i] = 0L;
			if (data[i]<fmin) fmin = data[i];
			if (data[i]>fmax) fmax = data[i];
		}

		map(fmin,fmax,0.0f,(float)(n-1),&m,&b);

		for(i=0;i<n;i++)  {
			index = (int) (.5 + m*i + b);
			counts[index]++;
		}


		maxindex = 0;

		for(i=0;i<n;i++)  {
			if (counts[i]>maxindex) maxindex = i;
		}


		free(counts);


		map(0.0f,(float)(n-1),fmin,fmax,&m,&b);
		dtemp = m*maxindex + b;
		return dtemp;

	}

	/**********************************************************************
	  R M S
	 **********************************************************************/

	double rms(float *data,short n)  {
		double sum=0.0;
		short i;

		for(i=0;i<n;i++)  {
			sum += data[i]*data[i]/(double)n;
		}

		return sum;
	}


	/**********************************************************************
	  G E O M E T R I C _ M E A N
	 **********************************************************************/


	double geometric_mean(float *data,short n)  {
		double exp,sum=1.0;
		short i;

		for(i=0;i<n;i++)  {
			sum = sum*data[i];
		}
		exp = 1.0/(double)n;
		return pow(sum,exp);
	}


	/**********************************************************************
	  H A R M O N I C _ M E A N

	  WATCH OUT FOR DIVISION BY 0!
	 **********************************************************************/

	double harmonic_mean(float *data,short n)  {
		double sum=0.0;
		short i;

		for(i=0;i<n;i++)  {
			sum += 1.0/(double)data[i];		/* WATCH OUT FOR DIVISION BY 0! */
		}
		return sum/(double)n;
	}



	/**********************************************************************
	  G A U S S F I T
	 **********************************************************************/


	void gaussfit(float *data,short n)  {
		short i;
		double mean,sigma,dtemp;


		mean = 0.0;

		for(i=0;i<n;i++)  {
			mean += data[i];
		}

		mean = mean/n;


		sigma = 0.0;

		for(i=0;i<n;i++)  {
			sigma += (data[i]-mean)*(data[i]-mean);
		}

		sigma = sqrt(sigma/n);



		dtemp = 1.0/sigma*sqrt(2.0*PI);


		for(i=0;i<n;i++)  {
			data[i] = (float) (dtemp * exp(-.5*(data[i]-mean)*(data[i]-mean)/(sigma*sigma)));
		}

	}

	/**********************************************************************
	  L E A S T S Q U A R E
	 **********************************************************************/

	void leastsquare(float *x,float *y,short n,float *m,float *b)  {
		double xtot,ytot,x2tot,y2tot,xytot;
		short i;

		xtot = 0.0;
		ytot = 0.0;
		x2tot = 0.0;
		y2tot = 0.0;
		xytot = 0.0;

		for(i=0;i<n;i++)  {
			ytot += y[i];
			xtot += x[i];
			x2tot += x[i]*x[i];
			y2tot += y[i]*y[i];
			xytot += x[i]*y[i];
		}

		*b = (float) ((ytot*x2tot - xtot*xytot) / (n*x2tot-xtot*xtot));

		*m = (float) ((n*xytot - xtot*ytot) / (n*x2tot - xtot*xtot));

	}

	/**********************************************************************
	  L E A S T S Q U A R E S
	  least squares line obtained by TRANSFORMING the data
	 **********************************************************************/

	void leastsquares(float *x,float *y,short n,float *m,float *b)  {
		double x2tot,xytot;
		short i;
		double xmean,ymean;

		xmean = mean(x,n);
		ymean = mean(y,n);

		xytot = 0.0;
		x2tot = 0.0;

		for(i=0;i<n;i++)  {
			xytot += (x[i]-xmean)*(y[i]-ymean);
			x2tot += (x[i]-xmean)*(x[i]-xmean);
		}

		*m = (float) (xytot/x2tot);

		*b = (float) (ymean - *m*xmean);


	}

	/**********************************************************************

	 **********************************************************************/

	char *mybasename(const char *path)  {
		static char str[MAX_PATH];

		strncpy(str, path, sizeof(str)-1);
		strip_path(str);
		return str;
	}


	/************************************************************************************

	 ************************************************************************************/

	//void explode(char *str, const char *sep, std::vector<std::string> &a);
	void myexplode(const char *_str, const char *sep, std::vector<std::string> &a)  {
		//const char *cptr;
		int p1, p2, len;
		//char *str;
		int i, j, k;
		std::vector<int> b;
		char str2[256];

		a.clear();

		len = myindex((char *)_str, sep, b);
		if (len==0)  {
			return;
		}

		p1 = 0;
		k = 0;
		memset(str2, 0, sizeof(str2));

		for(i=0; i<len; i++)  {
			p2 = b[i];
			for(j=p1; j<p2; j++)  {
				str2[k] = _str[j];
				k++;							// add error checking here
			}
			trimcrlf(str2);
			std::string s(str2);
			a.push_back(s);
			k = 0;
			memset(str2, 0, sizeof(str2));
			p1 = p2 + 1;
		}

		for(j=p1; j<(int)strlen(_str); j++)  {
			str2[k] = _str[j];
			k++;							// add error checking here
		}
		trimcrlf(str2);
		std::string s(str2);
		a.push_back(s);

		len = (int)a.size();

#if 0
		len = strlen(_str);
		str = new char[len+1];
		strcpy(str, _str);
		
		p1 = 0;

		while(1)  {
			cptr = &str[p1];
			p2 = myindex((char *)&str[p1], sep);
			if (p2==-1)  {
				break;
			}

			str[p2] = 0;
			cptr = &str[p1];
			cptr = &str[p2+1];
			p1 = p2 + 1;
		}

		DELARR(str);
#endif


		/*
		int i, len, k;
		char buf[32];
		int bp = 0;

		len = strlen(str);
		k = 0;
		memset(buf, 0, sizeof(buf));

		for(i=0; i<len; i++)  {
			buf[k] = str[i];
			k = (k+1)%32;
			if (strcmp(buf, sep)==0)  {
				bp = 1;
			}
		}

		bp = 2;
		*/


#if 0
		char *token=NULL;

		std::string save(str);

		token = strtok(str, sep);

		while( token != NULL )  {
			std::string s(token);
			a.push_back(s);
			token = strtok(NULL, sep);
		}

		//	bp = 1;

		strcpy(str, save.c_str());
		//#endif
#endif

		return;
	}


	/**********************************************************************

	 **********************************************************************/

	void strtolower(char *str)  {
		int len = (int)strlen(str);
		int i;

		for(i=0;i<len;i++)  {
			str[i] = tolower(str[i]);
		}
		return;
	}


	/**********************************************************************

	 **********************************************************************/

	void strtoupper(char *str)  {
		int len = (int)strlen(str);
		int i;

		for(i=0;i<len;i++)  {
			str[i] = toupper(str[i]);
		}
		return;
	}

	/*******************************************************************
	  returns the angle between two points in 3d space

	  Formula
	  The angle t between two lines is the angle between two direction vectors of the lines.
	  Take line a with direction vector A(a,a',a") and line b with direction vector B(b,b',b").


	  A.B = ||A||.||B||.cos(t)
	  <=>
	  A.B
	  cos(t) = --------------
	  ||A||.||B||

	  Example
	  Take A(1,2,3) ; B(4,5,6) ; C(3,2,0)
	  Calculate the angle between the lines AB and AC.
	  The line AB has a direction numbers (3,3,3) and line AC has direction numbers (2,0,-3).
	  Hence


	  6 + 0 - 9
	  cos(t) = -----------------
	  sqrt(27) .sqrt(13)

	  for the sharp angle we find 80.78 degrees.


	  The dot product of the two lines a and b is length_a*length_b*cos(theta).  So
	  you can just use arccos((ax*bx+ay*by+az*bz)/(length_a*length_b)).  This is
	  provided that they start at a common point.
	  i.e.
	  ax = a_end_x - a_start_x
	  bx = b_end_x - b_start_x
	  etc.
	  provided that a_start = b_start.



	 *******************************************************************/

	double get_angle(double x1, double y1, double z1, double x2, double y2, double z2, bool degrees)  {
		double d;

		d = (x1*x2 + y1*y2 + z1*z2) / (vmag(x1,y1,z1)*vmag(x2,y2,z2)) ;

		if (d > 1.0)
			d = 1.0;
		else if (d<-1.0)
			d = -1.0;

		d = acos(d);

		if (degrees)  {
			return RADTODEG * d;
		}
		else  {
			return d;
		}
	}


	/**********************************************************************
	  repairs the cr/lfs of files
	 **********************************************************************/

	void fix_crlf(char *fname)  {
		FILE *instream, *outstream;
		int status;
		unsigned char lastchar;


		instream = fopen(fname, "rb");
		if (instream==NULL)  {
			return;
		}

		outstream = fopen("tmpcrlf.tmp", "wb");

		status = fgetc(instream);

		if (status >= 0)  {
			lastchar = (unsigned char)status;

			while(1)  {
				status = fgetc(instream);
				if (status < 0)  {
					fclose(instream);
					fclose(outstream);
					unlink("tmpcrlf.tmp");
					return;
				}

				if (status==0x0d)  {
					fputc(status, outstream);
				}
				else if (status==0x0a)  {
					if (lastchar != 0x0d)  {
						fputc(0x0d, outstream);
					}
					fputc(status, outstream);
				}
				else  {
					if (lastchar==0x0d)  {
						fputc(0x0a, outstream);
					}
					fputc(status, outstream);
				}

				lastchar = (unsigned char)status;

			}

			if (lastchar==0x0d)  {									// don't allow trailing cr's
				fputc(0x0a, outstream);
			}
		}


		fclose(instream);
		fclose(outstream);

		unlink(fname);
		rename("tmpcrlf.tmp", fname);

		return;
	}

	/**********************************************************************
	  reverses the line sequence of files

	  eg, file =
	  1
	  2
	  3

	  reversed file =
	  3
	  2
	  1

	 **********************************************************************/

	void reverse_file(char *fname)  {
		FILE *stream;
		char *cptr;
		char buf[256];


		stream = fopen(fname, "rt");
		if (stream==NULL)  {
			return;
		}

		std::vector<std::string> vec;

		while(1)  {
			cptr = fgets(buf, 256, stream);
			if (cptr==NULL)  {
				break;
			}
			trimcrlf(buf);
			std::string ts(buf);
			vec.push_back(ts);
		}

		fclose(stream);

		std::reverse(vec.begin(), vec.end());



		stream = fopen("rvrs.tmp", "wt");
		if (stream==NULL)  {
			return;
		}

		int n = (int)vec.size();

		for(int i=0; i<n; i++)  {
			fprintf(stream, "%s\n", vec[i].c_str());
		}

		fclose(stream);

		unlink(fname);
		rename("rvrs.tmp", fname);

		while(vec.size())  {
			vec.pop_back();
		}

		return;
	}


	/**********************************************************************
	  removes empty lines
	 **********************************************************************/

	void remove_empty_lines(char *fname)  {
		FILE *instream, *outstream;
		char *cptr;
		char buf[256];


		instream = fopen(fname, "rt");
		if (instream==NULL)  {
			return;
		}
		outstream = fopen("relzxx.tmp", "wt");

		while(1)  {
			cptr = fgets(buf, 256, instream);
			if (cptr==NULL)  {
				break;
			}
			trimcrlf(buf);
			if (buf[0]==0)  {
				continue;
			}
			fprintf(outstream, "%s\n", buf);
		}

		fclose(instream);
		fclose(outstream);

		unlink(fname);
		rename("relzxx.tmp", fname);


		return;
	}


	/**********************************************************************
	  backs up n lines from the end in a text file
	  file must be open in BINARY MODE!!!
	 ***********************************************************************/

	int backup(FILE *stream, long n)  {
		int i = 0;
		int status;
		//long offs;

		status = fseek(stream, -1L, SEEK_END);

		while(1)  {
			status = fgetc(stream);
			if (status<0)  {
				break;
			}

			if (status==0x0a)  {
				i++;
				if (i==n+1)  {
					break;
				}
			}

			fseek(stream, -2L, SEEK_CUR);
		}

		if (i != n+1)  {
			return 1;
		}

		return 0;
	}

	/**********************************************************************
	  returns the month (1-12) from a ctime month format
	 **********************************************************************/

	int getmonth(const char *str)  {
		static const char *months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		int i;

		for(i=0; i<12; i++)  {
			if (!strcmp(str, months[i]))  {
				//int bp = 1;
				break;
			}
		}
		if (i==12)  {
			//throw (fatalError(__FILE__, __LINE__));
			return -1;
		}

		return (i+1);
	}

	/**********************************************************************
		k is 1 - 12
	 **********************************************************************/

	const char *getmonth(int k, bool longform)  {
		static const char *shortmonths[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		static const char *longmonths[12] = { "January", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

		if (k<1 || k>12)  {
			return NULL;
		}

		if (longform)  {
			return longmonths[k-1];
		}
		else  {
			return shortmonths[k-1];
		}
	}

	/*******************************************************************************
	  compute a sine wave of length n with period T
	 *******************************************************************************/

	void compute_sine(float *sine, int n, int T)  {
		float m, b;
		int i;
		double theta;

		assert(T<=n);

		map(0.0f, (float)(T-1), 0.0f, (float)(2.0*PI), &m, &b);
		//map(0.0f, (float)T, 0.0f, (float)(2.0*PI), &m, &b);

		for(i=0; i<n; i++)  {
			theta = m*i + b;
			sine[i] = (float)sin(theta);
		}

		return;
	}

	/*******************************************************************
	  moves to the nth line of a text file
	 *******************************************************************/

	int move_to(FILE *stream, int line)  {
		int status;
		int count;
		char *cptr;
		char buf[80];

		status = fseek(stream, 0L, SEEK_SET);
		if (status)  {
			return 1;
		}

		count = 0;

		while(1)  {
			cptr = fgets(buf, sizeof(buf), stream);
			if (cptr==NULL)  {
				return 1;
			}
			count++;
			if (count==line)  {
				break;
			}
		}

		return 0;
	}

	/**************************************************************************
	  direction = 0 ==> forward search
	  direction = 1 ==> backward search
	 **************************************************************************/

	long searchFile(char *fname, long _offset, char *pat, int direction)  {
		FILE *stream=NULL;
		int status;
		int size;
		char c;
		int i;
		int patlen;
		long offset;
		int rc;

		size = (int)filesize_from_name(fname);

		if (_offset> (size-1))  {
			return -1;
		}

		stream = fopen(fname, "rb");
		if (stream==NULL)  {
			return -1;
		}

		fseek(stream, _offset, SEEK_SET);

		i = 0;
		patlen = (int)strlen(pat);
		offset = _offset;

		while(1)  {
			status = fgetc(stream);
			if (status==EOF)  {
				break;
			}

			if (direction==1)  {							// reverse direction?
				rc = fseek(stream, -2, SEEK_CUR);
				if (rc != 0)  {
					fclose(stream);
					printf("backup error\n");
					return -1;
				}
#ifdef _DEBUG
				offset = ftell(stream);
#endif
			}

			c = (char) (status&0x00ff);
			if (c==pat[i])  {
				if (i==0)  {
					if (direction==0)  {
						offset = ftell(stream)-1;
					}
					else  {
						offset = ftell(stream)+1;
					}
				}
				i++;
				if (i==patlen)  {
					fclose(stream);
					return offset;
				}
			}
			else  {
				i = 0;
			}
		}


		fclose(stream);

		return -1;
	}

/**************************************************************************

**************************************************************************/

long read_until_char(char *fname, long _offset, char cc, char *buf, int bufsize)  {
	FILE *stream=NULL;
	long offset = -1;
	unsigned char c;
	int i;
	int status;
	memset(buf, 0, bufsize);

	stream = fopen(fname, "rb");
	if (stream==NULL)  {
		return -1;
	}

	fseek(stream, _offset, SEEK_SET);
	offset = _offset;
	i = 0;

	while(1)  {
		status = fgetc(stream);
		if (status==EOF)  {
			break;
		}
		offset++;
		c = (unsigned char) (status & 0x00ff);

		if (c==(unsigned char)cc)  {
			buf[i] = 0;
			fclose(stream);
			return (offset-1);
		}

		buf[i] = c;
		i++;
		if (i==bufsize-1)  {
			buf[i] = 0;
		}
	}

	fclose(stream);
	return -1;
}


/***************************************************************************
	returns the vertical coord of percentage f of the rectangle.
***************************************************************************/

int pv(RECT r, float f)  {
	int i;
	if (f > 1.0f)  {
		f = 1.0f;
		return r.bottom;
	}

	if (f < 0.0f)  {
		f = 0.0f;
		return r.top;
	}

	i = ROUND(r.top + f * (r.bottom - r.top));
	return i;

}

/***************************************************************************
	returns the horizontal coord of percentage f of the rectangle.
***************************************************************************/

int ph(RECT r, float f)  {
	int i;
	if (f > 1.0f)  {
		f = 1.0f;
		return r.right;
	}

	if (f < 0.0f)  {
		f = 0.0f;
		return r.left;
	}

	i = ROUND(r.left + f * (r.right - r.left));
	return i;

}

/*****************************************************************************************

*****************************************************************************************/

int getbit(unsigned char c, int i)  {
	if (i<0 || i>7)  {
		return 0;
	}

	unsigned char mask = 0x01 << i;

	if ( (c & mask)==mask)  {
		return 1;
	}

	return 0;

}

/*****************************************************************************************
	converts a raw date string to a common-format processed date string.
*****************************************************************************************/

int fix_date(char *raw, char *processed)  {
	int n, month, day, year;
	std::vector<std::string> a;

	processed[0] = 0;

	if (raw[0]==0)  {
		return 0;
	}

	int i = myindex(raw, "ABT ");
	if (i==0)  {
		strcpy(processed, raw);
		replace(processed, "ABT", "ABOUT");
		return 0;
	}

	i = myindex(raw, "AFT ");
	if (i==0)  {
		strcpy(processed, raw);
		replace(processed, "AFT", "AFTER");
		return 0;
	}

	explode2(raw, " ", a);
	n = (int)a.size();

	/*
	if (!strcmp(raw, "JANUARY 01, 1966"))  {
		bp = 3;
	}
	*/

	if (n==0)  {
		//throw (fatalError(__FILE__, __LINE__) );
		return 1;
	}
	else if (n==1)  {
		char s[64];
		int n;
		strncpy(s, (char *)a[0].c_str(), sizeof(s)-1);
		n = sscanf(s, "%d", &year);
		if (n==1)  {
			if (year>1000 && year<2020)  {
				sprintf(processed, "%04d", year);
			}
		}
	}
	else if (n==2)  {
		char s1[64];
		char s2[64];

		strncpy(s1, (char *)a[0].c_str(), sizeof(s1)-1);
		s1[3] = 0;
		strncpy(s2, (char *)a[1].c_str(), sizeof(s2)-1);
		trim(s2, ",");
		month = getmonth(s1);

		if (month!=-1)  {
			n = sscanf(s2, "%d", &year);
			if (n!=1)  {
				return 0;
			}
			if (year>1000 && year<2020)  {
				sprintf(processed, "%04d-%02d", year, month);
			}
		}

	}
	else if (n==3)  {
		char str1[64];
		char str2[64];
		char str3[64];
		strncpy(str1, (char *)a[0].c_str(), sizeof(str1)-1);
		str1[3] = 0;
		strncpy(str2, (char *)a[1].c_str(), sizeof(str2)-1);
		trim(str2, ",");
		strncpy(str3, (char *)a[2].c_str(), sizeof(str3)-1);

		month = getmonth(str1);

		if (month!=-1)  {
			n = sscanf(str2, "%d", &day);
			if (n!=1)  {
				return 0;
			}
			if (day>=1 && day<=31)  {
				n = sscanf(str3, "%d", &year);
				if (n!=1)  {
					return 0;
				}
				if (year>1000 && year<2020)  {
					sprintf(processed, "%04d-%02d-%02d", year, month, day);
				}
			}
		}
	}
	else  {
		return 0;
	}

	return 0;
}

/******************************************************************

******************************************************************/

int getmonth(char *str)  {
	int month;

	if (!strcmp(str, "JAN"))  {
		month = 1;
	}
	else if (!strcmp(str, "FEB"))  {
		month = 2;
	}
	else if (!strcmp(str, "MAR"))  {
		month = 3;
	}
	else if (!strcmp(str, "APR"))  {
		month = 4;
	}
	else if (!strcmp(str, "MAY"))  {
		month = 5;
	}
	else if (!strcmp(str, "JUN"))  {
		month = 6;
	}
	else if (!strcmp(str, "JUL"))  {
		month = 7;
	}
	else if (!strcmp(str, "AUG"))  {
		month = 8;
	}
	else if (!strcmp(str, "SEP"))  {
		month = 9;
	}
	else if (!strcmp(str, "OCT"))  {
		month = 10;
	}
	else if (!strcmp(str, "NOV"))  {
		month = 11;
	}
	else if (!strcmp(str, "DEC"))  {
		month = 12;
	}
	else  {
		month = -1;
	}

	return month;
}

/******************************************************************

******************************************************************/

/*
bool isnumber(char *str)  {
	int n;
	unsigned long i;
	double d;

	n = sscanf(str, "%ld", &i);
	if (n==1)  {
		return true;
	}

	n = sscanf(str, "%lf", &d);
	if (n==1)  {
		return true;
	}

	return false;
}
*/

/******************************************************************

******************************************************************/

void reverse_long_bytes(unsigned long *ul)  {
	unsigned long temp;

	temp = *ul;

	// 33 22 11 00

	temp = 0;
	temp = *ul << 24;
	temp |= (*ul & 0x0000ff00) << 8;
	temp |= (*ul & 0x00ff0000) >> 8;
	temp |= (*ul & 0xff000000) >> 24;

	*ul = temp;

	return;
}


/******************************************************************

******************************************************************/

unsigned short tobin(char *buf)  {
	int num;

	if (buf[0]<0x3a)
		num = (buf[0]-'0') * 16;
	else
		num = (buf[0]-'A'+10)*16;

	if (buf[1] < 0x3a)
		num = num + buf[1] - '0';
	else
		num = num + buf[1] - 'A' + 10;

	return(num);

}

/******************************************************************

******************************************************************/

unsigned short tobin2(char *buf)  {
	unsigned short num;

	if (buf[0]<0x3a)
		num = (buf[0]-'0') * 4096;
	else
		num = (buf[0]-'A'+10)*4096;

	if (buf[1] < 0x3a)
		num = num + (buf[1] - '0')*256;
	else
		num = num + (buf[1] - 'A' + 10)*256;

	if (buf[2] < 0x3a)
		num = num + (buf[2] - '0')*16;
	else
		num = num + (buf[2] - 'A' + 10)*16;

	if (buf[3] < 0x3a)
		num = num + buf[3] - '0';
	else
		num = num + buf[3] - 'A' + 10;

	return num;

}

/******************************************************************

******************************************************************/

void lshift(unsigned char *buf, int len, int n)  {
	int i;

	for(i=0;i<len-n;i++)  {
		buf[i] = buf[i+n];
	}
	for(i=len-n;i<len;i++)  {
		buf[i] = 0;
	}
	return;
}

/**************************************************************************

**************************************************************************/

double finterp(double a1, double a, double a2, double b1, double b2)  {
	if (a2==a1)  {
		return 0.0;
	}
	return ( b1 + ((a - a1) * (b2-b1)) / (a2-a1) );
}

/**************************************************************************

**************************************************************************/

int mycopy(const char *existing_name, const char *new_name)  {
	FILE *instream, *outstream;
	int status;
	char c;
	int count;

	instream = fopen(existing_name, "rb");
	if (instream==NULL)  {
		return 1;
	}

	if (exists((char *)new_name))  {
		if (is_open((char *)new_name))  {
			//throw (fatalError(__FILE__, __LINE__));
			FCLOSE(instream);
			return 1;
		}
	}

	outstream = fopen(new_name, "wb");
	if (outstream==NULL)  {
		FCLOSE(instream);
		return 1;
	}

	count = 0;

	while(1)  {
		status = (int)fread(&c, 1, 1, instream);
		if (status != 1)  {
			break;
			//FCLOSE(instream);
			//FCLOSE(outstream);
			//return 1;
		}
		count++;

		status = (int)fwrite(&c, 1, 1, outstream);
		if (status != 1)  {
			FCLOSE(instream);
			FCLOSE(outstream);
			return 1;
		}

	}

	FCLOSE(instream);
	FCLOSE(outstream);

	return 0;
}

/**************************************************************************

**************************************************************************/

int hexbuf_to_binbuf(char *hexbuf, unsigned char *binbuf, int hexbufsize, int binbufsize)  {
	int i, hexbuflen, n, binbuflen;
	unsigned long c;


	hexbuflen = (int)strlen(hexbuf);
	if (hexbuflen > hexbufsize)  {
		return 0;
	}

	binbuflen = hexbuflen / 2;
	if (binbuflen > binbufsize)  {
		return 0;
	}

	memset(binbuf, 0, binbufsize);

	for(i=0; i<binbuflen; i++)  {
		n = sscanf(&hexbuf[i*2], "%02x", (unsigned int *) &c);
		if (n != 1)  {
			return 0;
		}
		binbuf[i] = (unsigned char) (c & 0x000000ff);
	}

#ifdef _DEBUG
	//int bp = 0;

	for(i=0; i<binbuflen; i++)  {
		c = binbuf[i];
		//bp = i;
	}
#endif

	return binbuflen;
}

/**************************************************************************

**************************************************************************/

int binbuf_to_hexbuf(unsigned char *binbuf, char *hexbuf, int binbufsize, int hexbufsize)  {
	int i;

	if (hexbufsize < 2*binbufsize)  {
		return 0;
	}

	for(i=0; i< binbufsize; i++)  {
		sprintf(&hexbuf[i*2], "%02x", binbuf[i]);
	}

	return binbufsize;
}


/**********************************************************************

**********************************************************************/

void leastsquare(std::vector<float> &v, float *m, float *b)  {
	float xtot,ytot,x2tot,y2tot,xytot;
	int i, n;

	n = (int)v.size();

	/*
	if (n != MAXRECS)  {
		return;
	}
	*/

	xtot = 0.0;
	ytot = 0.0;
	x2tot = 0.0;
	y2tot = 0.0;
	xytot = 0.0;

	for(i=0; i<n; i++)  {
		ytot += v[i];
		xtot += i;
		x2tot += i*i;
		y2tot += v[i] * v[i];
		xytot += i*v[i];
	}

	*b = (float) ((ytot*x2tot - xtot*xytot) / (n*x2tot-xtot*xtot));
	*m = (float) ((n*xytot - xtot*ytot) / (n*x2tot - xtot*xtot));

	return;
}


/***********************************************************************************

***********************************************************************************/

void substring(const char *str, int pos1, int pos2, char *substr )  {
	int i, j;

	j = 0;

	for(i=pos1; i<=pos2; i++)  {
		substr[j++] = str[i];
	}

	substr[j] = 0;
	return;
}

/***********************************************************************************

***********************************************************************************/

void explode2(char *str, const char *sep, std::vector<std::string> &a)  {
	int k;
	//int len;
	//char c;
	int pos;
	char substr[256];
	//const char *cptr;


	//len = (int)strlen(str);

	k = 0;
	//i = 0;

	// 1,2,,4
	// 012345

	while(1)  {
		pos = myindex(&str[k], sep);
		if (pos==-1)  {
			std::string s(&str[k]);
			a.push_back(s);
			break;
		}
		substring(&str[k], 0, pos, substr );
		trim(substr, sep);
		std::string s(substr);
		a.push_back(s);
		k += (pos + 1);
	}

	/*
	for(i=0; i<(int)a.size(); i++)  {
		cptr = a[i].c_str();
		bp = i;
	}
	*/

	return;
}

#if 0
/**********************************************************************
  t r i m c r l f
  trims trailing spaces and tabs and cr's and lf's from the string
**********************************************************************/

void trimcrlf2(TCHAR *str)  {
	int i;

	i = strlen(str)-1;

	if (i<0)  {
		return;							// already trimmed
	}


	while ((str[i]==0x0a) || (str[i]==0x0d) || (str[i]==' ') || (str[i]=='\t'))  {
		i--;
		if (i<0)  {
			break;
		}
	}

	i++;
	str[i] = 0;

	return;

}
#endif

/**********************************************************************

**********************************************************************/

unsigned long unpack(unsigned long n)  {
	unsigned long work;
	unsigned long hbits;

	work = n & 0x0fffffff;
	hbits = n & 0x70000000;

	if (hbits & 0x10000000)  {
		work |= 0x00000080;
	}
	if (hbits & 0x20000000)  {
		work |= 0x00008000;
	}
	if (hbits & 0x40000000)  {
		work |= 0x00800000;
	}
	return work;
}

/***********************************************************************************

***********************************************************************************/

void pad(char *str, int strsize, char c)  {
	int i, len;

	len = (int)strlen(str);
	if (len > strsize)  {
		str[strsize-1] = 0;
		return;
	}

	for(i=len; i<strsize-1; i++)  {
		str[i] = ' ';
	}
	str[i] = 0;

	return;
}

/***********************************************************************************

***********************************************************************************/

bool equals(char *s1, char *s2)  {
	int i, len;

	len = (int)strlen(s1);

	for(i=0; i<len; i++)  {
		if (s1[i] != s2[i])  {
			return false;
		}
	}

	return true;

}


/***********************************************************************************

***********************************************************************************/

bool equalsIgnoreCase(char *s1, char *s2)  {
	int i, len;

	len = (int)strlen(s1);

	for(i=0; i<len; i++)  {
		if (toupper(s1[i]) != toupper(s2[i]) )  {
			return false;
		}
	}

	return true;

}


/***********************************************************************************

***********************************************************************************/



bool startsWith(char *haystack, char *needle)  {
	int i, len;

	len = (int)strlen(needle);

	for(i=0; i<len; i++)  {
		if (needle[i] != haystack[i] )  {
			return false;
		}
	}

	return true;

}


/***********************************************************************************

***********************************************************************************/

bool startsWithIgnoreCase(char *haystack, char *needle)  {
	int i, len;

	len = (int)strlen(needle);

	for(i=0; i<len; i++)  {
		if (toupper(needle[i]) != toupper(haystack[i]) )  {
			return false;
		}
	}

	return true;

}

/***********************************************************************************

***********************************************************************************/

void timeBeginPeriod(unsigned long ms)  {
	return;
}

/***********************************************************************************

***********************************************************************************/

void timeEndPeriod(unsigned long ms)  {
	return;
}
/**********************************************************************

	entry:
		tms = elapsed time in milliseconds
		hh:mm:ss.t
**********************************************************************/


void time_format(DWORD tms, char *str)  {

	/*
	//static char str[32] = {"00:00:00.0"};
	//static char str[16] = {0};

	int hours=0, mins=0, secs, tenths, thousandths;
	int tsecs;

	tsecs = tms/1000;						// total seconds
	secs = tsecs % 60;

	if (tsecs>=3600)  {					// hours > 0
		hours = (tsecs / 3600) % 60;
		mins = (tsecs/60) % 60;
	}
	else if (tsecs>=60)  {				// minutes > 0
		mins = (tsecs/60) % 60;
	}

	thousandths = tms - (tms/1000)*1000;										// eg, 985

	tenths = (int) (.5f + (float)thousandths/100.0f);

	if (tenths==10)  {
		// round seconds, minutes, hours up
		tenths = 0;
		secs++;
		if (secs == 60)  {
			secs = 0;
			mins++;
		}
		if (mins == 60)  {
			mins = 0;
			hours++;
		}

	}
	sprintf(str,"%02d:%02d:%02d.%1d", hours, mins, secs, tenths);
	*/

	int h, m, s, ts;
	//float secs;
	convtime(tms, h, m, s, ts);
	//sprintf(str,"%02d:%02d:%02d.%1d", h, m, s, ts);
	sprintf(str,"%02d:%02d:%02d.%02d", h, m, s, ts);			// changed for video program

	return;

}

/**********************************************************************
	entry:
		tms = elapsed time in milliseconds
		m:ss.t
**********************************************************************/


void time_format2(DWORD tms, char *str)  {
	int h, m, s, ts;
	//float secs;
	convtime(tms, h, m, s, ts);
	sprintf(str,"%d:%02d.%02d", m, s, ts);			// changed for video program

	return;

}								// time_format2

/*************************************************************************
	computes hours, minutes, seconds, tenths from milliseconds
*************************************************************************/

void convtime(DWORD ms, int &h, int &m, int &s, int &ts)  {
	float secs = ms / 1000.0f;			// time in seconds
	h = (int) (secs / (60 * 60));
	secs -= h * (60 * 60);
	m = (int) (secs / 60);
	secs -= m * 60;
	s = (int) secs;
	secs -= s;
	ts = (int) (secs * 100);
	return;
}

/**********************************************************************

**********************************************************************/

unsigned short getRandomNumber(void)  {
	unsigned short seed;

	srand(unsigned(time(0)));
	seed = rand();

	return seed;
}


/***********************************************************************

***********************************************************************/

#if 0

/**********************************************
	compatible with mysql timestamps
**********************************************/

void timestamp(char *str)  {

#ifdef WIN32
	SYSTEMTIME x;
	GetLocalTime(&x);
	//sprintf(str, "%02d%02d%02d%02d.log",x.wMonth,x.wDay,x.wHour,x.wMinute);
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",
					x.wYear, x.wMonth, x.wDay,
					x.wHour, x.wMinute, x.wSecond);

#else
	struct tm *x;
	time_t aclock;

   time(&aclock);						// Get time in seconds
   x = localtime(&aclock);			// Convert time to struct tm form
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",
			x->tm_year+1900, x->tm_mon+1, x->tm_mday,
			x->tm_hour, x->tm_min, x->tm_sec);
#endif

	return;
}
#endif

//#ifdef WIN32

/*******************************************************************************

*******************************************************************************/

void dprintf(const char *format, ...)  {
	#ifdef _DEBUG
#if defined(WIN32) | defined(__MACH__)
		va_list ap;													// Argument pointer
		char s[1024];												// Output string
		int len;

		len = (int)strlen(format);
		if (len>1023)  {
			return;
		}
		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);
		printf("%s", s);
#endif
	#else
		/*
		va_list ap;													// Argument pointer
		char s[1024];												// Output string
		int len;

		len = strlen(format);
		if (len>1023)  {
			return;
		}
		va_start(ap, format);
		vsprintf(s, format, ap);
		va_end(ap);
		printf("%s", s);
		*/
	#endif

	return;
}
//#endif				// #ifdef WIN32



/*******************************************************************************

*******************************************************************************/

bool deq(double d1, double d2, double tolerance)   {
	double d;

	d = fabs(d1-d2);

	if (d <= tolerance)  {
		return true;
	}
	return false;
}

/***********************************************************************************************************
	will also do other gradients
***********************************************************************************************************/

void hot(void)  {
	int w = 8;
	int h = ROUND(920);
	int i, j, k;
	unsigned char r, g, b;
	float mr,br,mg,bg,mb,bb;
	//FILE *stream;

	//stream = fopen("c:\\users\\larry\\desktop\\x.x", "wt");

	unsigned char *image = new unsigned char[w*h*3];

	r = g = b = 0;

	int gstart = ROUND(.5f*(h-1));
	int bstart = ROUND(.75f*(h-1));

#ifdef HOT
	map(0.0f,			(float)(h-1), 64.0f, 255.0f, &mr, &br);
	map((float)gstart,	(float)(h-1), 0.0f, 255.0f, &mg, &bg);
	map((float)bstart,	(float)(h-1), 0.0f, 255.0f, &mb, &bb);
#else
	gstart = 0;
	bstart = 0;
	map(0.0f,	(float)(h-1), 0.0f, 0.0f, &mr, &br);
	map(0.0f,	(float)(h-1), 0.0f, 0.0f, &mg, &bg);
	map(0.0f,	(float)(h-1), 0.0f, 192.0f, &mb, &bb);
#endif

	for(i=0;i<h;i++)  {
		r = (unsigned char) (.5 + mr*i + br);
		g = (unsigned char) (.5 + mg*i + bg);
		b = (unsigned char) (.5 + mb*i + bb);

		if (i<gstart)  {
			g = 0;
		}
		if (i<bstart)  {
			b = 0;
		}

		//fprintf(stream, "%3d %3d %3d %3d\n", i, r, g, b);

		for(j=0;j<w;j++)  {
			k = i*(w*3) + 3*j;
			image[k] = b;
			image[k+1] = g;
			image[k+2] = r;
		}
	}


	BMP *bmp = new	BMP(w, h, 24, image, "c:\\users\\larry\\desktop\\hot.bmp");
	DEL(bmp);
	delete[] image;
	image = NULL;
	//FCLOSE(stream);

	return;
}

/*******************************************************************************
	returns true if a float is equal to an integer, or false if not
*******************************************************************************/

bool float_is_integer(float f)  {
	float f2;
	long l;

	if (f > 0.0f)  {
		f2 = 1000000.0f * f;
		l = (long)ROUND(f2);

		if (l==(long)f2)  {
			return true;
		}
	}

	return false;
}

/***********************************************************************************

***********************************************************************************/

void CopyRect(RECT *dest, RECT *src)  {
	memcpy(dest, src, sizeof(RECT));
	return;
}

/*******************************************************************************

*******************************************************************************/

void CopyRect(FRECT *rd, RECT *rs)  {
	rd->left = (float)rs->left;
	rd->top = (float)rs->top;
	rd->right = (float)rs->right;
	rd->bottom = (float)rs->bottom;

	return;
}

/*******************************************************************************

*******************************************************************************/

void CopyRect(FRECT *rd, FRECT *rs)  {
	rd->left = rs->left;
	rd->top = rs->top;
	rd->right = rs->right;
	rd->bottom = rs->bottom;

	return;
}

/*******************************************************************************

*******************************************************************************/

std::string file_get_contents(const char *fname)  {
	std::string s;
	FILE *stream;
	char *cptr;
	char buf[256];


	stream = fopen(fname, "rt");
	if (stream==NULL)  {
		return s;
	}

	while(1)  {
		cptr = fgets(buf, sizeof(buf)-1, stream);
		if (cptr==NULL)  {
			break;
		}
		s.append(cptr);
	}


	FCLOSE(stream);
	return s;
}						// std::string file_get_contents(const char *fname)  {

int write_long(FILE *out, int n) {
	putc((n&255),out);
	putc(((n>>8)&255),out);
	putc(((n>>16)&255),out);
	putc(((n>>24)&255),out);

	return 0;
}

int write_word(FILE *out, int n) {
	putc((n&255),out);
	putc(((n>>8)&255),out);

	return 0;
}

int read_long(FILE *in) {
	int c;

	c=getc(in);
	c=c+(getc(in)<<8);
	c=c+(getc(in)<<16);
	c=c+(getc(in)<<24);

	return c;
}

int read_word(FILE *in) {
	int c;

	c=getc(in);
	c=c+(getc(in)<<8);

	return c;
}

int read_chars(FILE *in, char *s, int count) {
	int t;

	for (t=0; t<count; t++)
	{
		s[t]=getc(in);
	}

	s[t]=0;

	return 0;
}

int read_chars_bin(FILE *in, char *s, int count) {
	int t;

	for (t=0; t<count; t++)
	{
		s[t]=getc(in);
	}

	return 0;
}

int write_chars(FILE *out, char *s) {
	int t;

	t=0;
	while(s[t]!=0 && t<255) {
		putc(s[t++],out);
	}

	return 0;
}

int write_chars_bin(FILE *out, char *s, int count) {
	int t;

	for (t=0; t<count; t++) {
		putc(s[t],out);
	}

	return 0;
}

int mystrcasecmp(const char *s1, const char *s2)
{
       while(tolower(*s1) == tolower(*s2++))
       {
               if(*s1++ == '\0')
               {
                       return 0;
               }
       }

       return *(unsigned char *)s1 - *(unsigned char *)(s2 - 1);
}

/************************************************************************************

************************************************************************************/

int doof(const char *_fname)  {
	FILE *instream=NULL;
	FILE *outstream=NULL;
	int status;
	unsigned char c;
	//#define NEW_R_NAME "ygrzdsl"
	char tmpname[260];
	CRF crf;


	instream = fopen(_fname, "rb");
	if (instream==NULL)  {
		//cleanup();
		//throw (fatalError(__FILE__, __LINE__));
		return 1;
	}

	strcpy(tmpname, _fname);
	strip_filename(tmpname);
#ifdef WIN32
	strcat(tmpname, "\\ygrzdsl");
#else
	strcat(tmpname, "/ygrzdsl");
#endif

	//strcpy(tmpname, "%s%sx.x", dirs[DIR_DEBUG].c_str(), FILESEPSTR);				// C:\Users\larry\Documents\tts.log

	crf.init();

	outstream = fopen(tmpname, "wb");

	while(1)  {
		status = fgetc(instream);
		if (status == EOF)  {
			break;
		}
		c = (unsigned char) status;
		crf.doo(&c, 1);
		fputc(c, outstream);
	}

	fclose(instream);
	fclose(outstream);

	if (!rm(_fname))  {								// unlink the encrypted file
		//cleanup();
		//throw (fatalError(__FILE__, __LINE__));
		return 2;
	}

	if (rename(tmpname, _fname))  {
		//cleanup();
		//throw (fatalError(__FILE__, __LINE__));
		return 3;
	}

	return 0;
}

/************************************************************************************

************************************************************************************/

//UINT FourCC(char a, char b, char c, char d) { 
//  return ( (UINT32) (((d)<<24) | ((c)<<16) | ((b)<<8) | (a)) );
//}
/*************************************************************************************************

*************************************************************************************************/

UINT32 fourcc(const char * a)  {
    if( strlen(a) != 4)
        return 0;               //Unknown or unspecified format

    return
    (
                (UINT32)
                (
                        ((*(a+3))<<24) |
                        ((*(a+2))<<16) |
                        ((*(a+1))<<8) |
                        (*a)
                )
    );
}

/*************************************************************************************************

*************************************************************************************************/

const char *fourcc(UINT32 a)  {
    static char str[8];

    memset(str, 0, sizeof(str));

    sprintf(str, "%c%c%c%c",
                        a&0x000000ff,
                        (a>>8) &  0x000000ff,
                        (a>>16) & 0x000000ff,
                        (a>>24) & 0x000000ff
                );

    return str;
}


/**************************************************************************

**************************************************************************/

bool ends_with(const char *str, const char *pat)  {
	int i, j;
	int len, patlen;


	len = (int)strlen(str);
	patlen = (int)strlen(pat);

	j = len - 1;


	for(i=patlen-1; i>=0; i--)  {
		if (pat[i] == str[j])  {
			j--;
		}
		else  {
			return false;
		}
	}


	return true;
}							// ends_with()


/**********************************************************************
	  trims leading cr, lf
 **********************************************************************/

void ltrimcrlf(char *string)   {
	int i,j,k;

#ifdef _DEBUG
	int len;
	len = (int)strlen(string);
#endif

	i = 0;

	//while (string[i] == ' ' || string[i]=='\t' || string[i]==0x0d || string[i]==0x0a)  {
	while (string[i]==0x0d || string[i]==0x0a)  {
		i++;
	}
	if (i==0)  {
		return;
	}

	k = (int)strlen(string);

	for(j=0;j<(k-i);j++)  {
		string[j] = string[j+i];
	}
	string[j] = 0;

#ifdef _DEBUG
	len = (int)strlen(string);
#endif
	return;
}											// ltrimcrlf()

/**********************************************

**********************************************/


void timestamp(char *str)  {
#ifdef WIN32
	SYSTEMTIME x;
	GetLocalTime(&x);
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d", x.wYear, x.wMonth,x.wDay, x.wHour, x.wMinute, x.wSecond);
#else
	struct tm *x;
	time_t aclock;
	
   time(&aclock);						// Get time in seconds
   x = localtime(&aclock);			// Convert time to struct tm form
	sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",
			x->tm_year+1900, x->tm_mon+1, x->tm_mday, 
			x->tm_hour, x->tm_min, x->tm_sec);
#endif

	return;
}

/**********************************************

**********************************************/

int fcmp( float _f1, float _f2, int _decimal_places )  {
	long l1, l2;

	switch(_decimal_places)  {
		case 0:
			l1 = ROUND(_f1*100);
			l2 = ROUND(_f2*100);
			break;

		case 1:
			l1 = ROUND(_f1*10);
			l2 = ROUND(_f2*10);
			break;

		case 2:
			l1 = ROUND(_f1*100);
			l2 = ROUND(_f2*100);
			break;

		case 3:
			l1 = ROUND(_f1*1000);
			l2 = ROUND(_f2*1000);
			break;

		// others to be done?
		default:
			return 0;
	}

	if (l1==l2)  {
		return 0;
	}
	else if (l1<l2)  {
		return -1;
	}
	else  {
		return 1;
	}
}

