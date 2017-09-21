

#pragma warning(disable:4996 4995 4005)					// for vista strncpy_s

#include <string.h>

#include <glib_defines.h>
#include <utils.h>


#include "csv.h"


extern char gstring[2048];

/**********************************************************************
	constructor
**********************************************************************/

CSV::CSV(char *_fname, bool _has_header)  {
	char *cptr;
	char *buf = gstring;
	int count = 0;
	int i, j;
	const char *ccptr;
	std::vector<std::string> vec;
	char str[64];


	strncpy(fname, _fname, sizeof(fname)-1);
	has_header = _has_header;

	init();

	int n = countlines(fname, true);
	if (n==0)  {
		return;
	}

	/*
	if (has_header)  {
		if (n==1)  {
			return;											// don't do this because they can't enter data in an empty csv file
		}
	}
	*/


	stream = fopen(fname, "rt");

	if (has_header)  {
		n--;
		load_header(stream);
	}

	rows = n;

	if (rows>0)  {
		std::vector<std::string> strs;
		for(int i=0; i<rows; i++)  {
			data.push_back(strs);
		}
	}

	// load the data:

	for(i=0; i<rows; i++)  {
		cptr = fgets(buf, sizeof(gstring)-1, stream);
		if (cptr==NULL)  {
			break;
		}
		trimcrlf(buf);
		if (buf[0]==0)  {
			continue;
		}

		count++;

		//explode(buf, ",", data[i]);
		vec.clear();
		explode2(buf, ",", vec );

		n = vec.size();
		if (n!=cols)  {
			sprintf(gstring, "column count: %d, %d", cols, n);
			MessageBox(NULL, gstring, "Error", MB_OK);
			return;
		}


		for(j=0; j<cols; j++)  {
			ccptr = vec[j].c_str();
			strncpy(str, ccptr, sizeof(str)-1);
			replace(str, "\"", "", 1);						// get rid of embedded quotes
			trimcrlf(str);
			data[i].push_back(str);
			if (i==0)  {
				if (isNumericString(str))  {
					colinfo[j].type = 'N';
				}
				else  {
					colinfo[j].type = 'C';
				}
			}
		}

		//bp = 1;

	}

	if (rows!=count)  {
		sprintf(gstring, "row count: %d, %d", rows, count);
		MessageBox(NULL, gstring, "Error", MB_OK);
		return;
	}


	int imax;
	for(j=0; j<cols; j++)  {
		//imax = -1;
		//imax = (int)strlen(hdr[j].c_str());
		imax = (int)strlen(colinfo[j].name.c_str());
		for(i=0; i<rows; i++)  {
			imax = MAX(imax, (int)strlen(data[i][j].c_str()));
		}
		colinfo[j].maxlen = imax;
	}

	loaded = true;

#ifdef _DEBUG
	//dump();
	bp = 0;
#endif

	return;
}

/**********************************************************************
	destructor
**********************************************************************/

CSV::~CSV()  {
	int i;

	FCLOSE(stream);

	while(colinfo.size())  {
		colinfo.pop_back();
	}

	int n = data.size();

	for(i=0; i<rows; i++)  {
		while(data[i].size())  {
			data[i].pop_back();
		}
	}
	
	while(data.size())  {
		data.pop_back();
	}

	//bp = 1;

}						// destructor

/**********************************************************************
	"AT","POS","FIRSTNAME","LASTNAME","BMO","BDAY","BYEAR","SEX","LBS","ECONTACT","EPHONE","PHONE","EMAIL","TYPE","STREET","CITY","STATE","ZIP","COUNTRY"
	AT,POS,FIRSTNAME,LASTNAME,BMO,BDAY,BYEAR,SEX,LBS,ECONTACT,EPHONE,PHONE,EMAIL,TYPE,STREET,CITY,STATE,ZIP,COUNTRY
**********************************************************************/

void CSV::load_header(FILE *stream)  {
	const char *cptr;
	char *buf = gstring;
	std::vector<std::string> vec;
	char str[64];
	int j;
	COLINFO ci;
	int pos;

	uses_quotes = false;

	while(1)  {
		cptr = fgets(buf, sizeof(gstring)-1, stream);
		if (cptr==NULL)  {
			return;
		}
		trimcrlf(buf);
		if (buf[0]==0)  {			// skip blank lines
			continue;
		}

		pos = myindex(buf, "\",\"");
		if (pos!=-1)  {
			uses_quotes = true;
		}

		explode2(buf, ",", vec);

		cols = vec.size();
		break;
	}

	//bp = -1;

	for(j=0; j<cols; j++)  {
		cptr = vec[j].c_str();
		strncpy(str, cptr, sizeof(str)-1);
		replace(str, "\"", "", 1);						// get rid of embedded quotes
		trimcrlf(str);
		ci.maxlen = 0;
		ci.name = str;
		ci.type = 0;
		ci.type = 'C';

		/*
		if (isNumericString(str))  {
			ci.type = 'N';
		}
		else  {
			ci.type = 'C';
		}
		*/

		colinfo.push_back(ci);
	}

	while(vec.size()) vec.pop_back();

#ifdef _DEBUG
	for(j=0; j<cols; j++)  {
		strncpy(str, colinfo[j].name.c_str(), sizeof(str)-1);
		strcat(str, "\n");
		OutputDebugString(str);
		bp = j;
	}
#endif

//	bp = 0;

	return;
}						// load_header()

/**********************************************************************

**********************************************************************/

void CSV::init(void)  {
	cols = 0;
	rows = 0;
	loaded = false;
	uses_quotes = false;
	stream = NULL;
	return;
}

/**********************************************************************

**********************************************************************/

void CSV::dump(char *_fname)  {
	int i, j;
	const char *cptr;
	FILE *outstream;
	char fmt[32];

	outstream = fopen(_fname, "wt");

	fprintf(outstream, "\n");

	for(j=0; j<cols; j++)  {
		sprintf(fmt, "%%-%ds", colinfo[j].maxlen+2);			// "%-5s"
		fprintf(outstream, fmt, colinfo[j].name.c_str());
	}

	fprintf(outstream, "\n");
	fprintf(outstream, "\n");

	for(i=0; i<rows; i++)  {
		for(j=0; j<cols; j++)  {
			cptr = data[i][j].c_str();
			sprintf(fmt, "%%-%ds", colinfo[j].maxlen+2);			// "%-5s"
			fprintf(outstream, fmt, cptr);
		}
		fprintf(outstream, "\n");
	}

	fprintf(outstream, "\n");

	fclose(outstream);

	return;
}

/*
//-----------------------------------------------------------------------------------------
// getfield ; return n-th field
//-----------------------------------------------------------------------------------------

const char * CSV::getfield(int j)  {
  if( j<0 || j>=cols )  {
	  return NULL;
  }
  else {
      return colinfo[j].name.c_str();
  }

}

//-----------------------------------------------------------------------------------------
// getfield ; return n-th field (of current row?)
//-----------------------------------------------------------------------------------------

bool CSV::getfield(int j, std::string& str) {
  if( j<0 || j>=cols )  {
      str = "";
      return false;
  }
  else {
	  str = colinfo[j].name.c_str();
      return true;
  }
}
*/

//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------

bool CSV::get_data(int _row, int _col, std::string& str) {
	if( _col<0 || _col>=cols )  {
      str = "";
      return false;
	}
	if( _row<0 || _row>=rows )  {
      str = "";
      return false;
	}

	str = data[_row][_col];
	return true;

}

/********************************************************************

********************************************************************/

int CSV::GetFieldType(int j)  {

	if (j<cols)  {
		return colinfo[j].type;
	}
	else  {
		return 'C';
	}
}

//-----------------------------------------------------------------------------------------
// removes leading and trailing quotes from a field
//-----------------------------------------------------------------------------------------

void CSV::fix(char *_str)  {
	int n = strlen(_str);

	if (_str[0] != 0x22)  {
		return;
	}

	if (_str[n-1] != 0x22)  {
		return;
	}

	strcpy(_str, &_str[1]);

	_str[n-2] = 0;


	return;
}
