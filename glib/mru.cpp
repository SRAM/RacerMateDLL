
#ifdef WIN32
	#pragma warning(disable:4996)			// strncpy_s
	//#define STRICT
	#define WIN32_LEAN_AND_MEAN
#else
    #include <stdio.h>
#endif


#include <mru.h>


/**********************************************************************

**********************************************************************/

MRU::MRU(void)  {

	maxsize = data.max_size();						// -1
	bp = 0;

	return;

}


/**********************************************************************

**********************************************************************/

MRU::~MRU()  {

	clear_data();

}

/*******************************************************************************

*******************************************************************************/

void MRU::setItem(int i, const char *_str)  {
	int j, n;
	std::string s;

	n = data.size();
	if (i<0)  {
		return;
	}

	if (n>0)  {
		if (i>(n-1))  {
			if(i==n)  {
				data.push_back(_str);
			}
			return;
		}
		s = _str;
		p = data.begin();
		for(j=0; j<i; j++) p++;
		data.insert(p, s);				// insert string s before p
		data.erase(p);
	}
	else  {
		bp = 2;
		data.push_back(_str);
		#ifdef _DEBUG
			n = data.size();
			assert(n==1);
		#endif
	}
	return;
}

/*******************************************************************************
	indexes are 0-based.
*******************************************************************************/

void MRU::move(int from, int to)  {						// moves the item in the 'from' position to the 'to' position
	if (from<0 || to<0)  {
		return;
	}
	int n = data.size() - 1;
	if (from>n || to>n)  {
		return;
	}
	if (from==to)  {
		return;
	}

	std::string sf, st;
	int i = 0;
	std::list<std::string>::iterator pf, pt;

	for(p = data.begin(); p!=data.end(); p++)  {
		if (i==from)  {
			sf = (*p).c_str();
			pf = p;
		}
		if (i==to)  {
			//st = (*p).c_str();
			pt = p;
		}
		i++;
	}

	//dump();
	//data.remove(sf);
	data.erase(pf);					// remove 'from'
	data.insert(pt, sf);				// insert string sf before pt

	//dump();
	//bp = 1;

	return;
}

/*******************************************************************************

*******************************************************************************/

void MRU::dump(void)  {
	const char *cptr;

	for(p = data.begin(); p!=data.end(); p++)  {
		cptr = (*p).c_str();
		bp = 1;
	}

	bp = 2;

	return;
}

/*******************************************************************************

*******************************************************************************/

void MRU::ini_write(Ini *ini, char *section)  {
	char *cptr;
	int i;
	char key[32];
	char s[256];

	sprintf(s, "[%s]", section);

	ini->clear_section(s);

	for(p = data.begin(), i=0; p!=data.end(); p++,i++)  {
		cptr = (char *)(*p).c_str();
		sprintf(key, "%d", i);
		ini->writeString(section, key, cptr);
	}

	return;
}

/*******************************************************************************

*******************************************************************************/

void MRU::clear_data(void)  {
	while(data.size())  {
		data.pop_back();
	}
	return;
}

/*******************************************************************************

*******************************************************************************/

int MRU::ini_read(Ini *ini, char *_section)  {
	const char *cptr;
	char key[32];
	int i=0;

#ifdef _DEBUG
	int size = data.size();
#endif

	if (data.size() != 0)  {
		clear_data();
	}

	while (1)  {
		sprintf(key, "%d", i);
		cptr = ini->getString(_section, key, "", false);
		if (*cptr==0)  {
			bp = 1;
			break;
		}
		data.push_back(cptr);
		i++;
	}

	return i;
}

/*******************************************************************************

*******************************************************************************/

void MRU::sync(const char *_str)  {
	int pos;

	if (data.size()==0)  {
		push_front(_str);
		return;
	}

	if (strcmp(_str, getItem(0)) != 0)  {
		pos = getIndex(_str);
		if (pos==-1)  {				// not found
			push_front(_str);
		}
		else  {							// found, but not at the top
			move(pos, 0);				// move to the top
		}
	}
	return;
}
