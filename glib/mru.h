#ifndef _MRU_H_
#define _MRU_H_

#ifdef WIN32
	#pragma warning(disable:4101 4786)				// "str" unreferenced
	#include <windows.h>
#else
	#include <string.h>
#endif

#include <assert.h>

#ifdef USE_LIST
	#error "USE_LIST is already defined"
#endif

#define USE_LIST

#ifdef USE_LIST
	#include <list>
	#include <algorithm>
	#include <ini.h>
#else
	#include <vector>
#endif

#include <string>

/*********************************************************************

	stack:
		LIFO
		FIFO (=queue)


	0	item0
	1	item1
	....
	n	itemn

  items added by increasing index

	0 1 ... N


*********************************************************************/

class MRU  {

	private:
		MRU(const MRU&);
		MRU &operator = (const MRU&);		// unimplemented
		#ifdef USE_LIST
			std::list<std::string> data;
			std::list<std::string>::iterator p;
			int maxsize;									// starts off as -1
		#else
			std::vector<std::string> data;
		#endif

		int bp;

	public:

		MRU(void);
		virtual ~MRU();

		inline void push_back(const char *_str)  {
			data.push_back(_str);
			return;
		}
		inline void push_front(const char *_str)  {
			data.push_front(_str);
			return;
		}

		inline int size(void)  {
			return (int)data.size();
		}

		inline const char *getItem(int i)  {
			#ifdef USE_LIST
				p = data.begin();
				for(int j=0; j<i; j++) {
					p++;
				}
				//p = p + i;
				return (*p).c_str();
			#else
				return data[i].c_str();
			#endif
		}

		void setItem(int i, const char *_str);


		inline int getIndex(const char *_str)  {
			//p = std::find(data.begin(), data.end(), _str);
			int i = 0;
			for(p = data.begin(); p!=data.end(); p++)  {
				if (strcmp((*p).c_str(),_str)==0)  {
					return i;
				}
				i++;
			}

			return -1;												// not found
		}

		void move(int from, int to);					// moves the item in the 'from' position to the 'to' position
		void dump(void);
		void ini_write(Ini *ini, char *section);
		void clear_data(void);
		int ini_read(Ini *ini, char *_section);
		void sync(const char *_str);

};

#endif			// #ifndef _MRU_H_

