#ifndef _CSV_H_
#define _CSV_H_

#include <stdio.h>
#include <string>
#include <vector>

class CSV {
	public:
		CSV(char *_fname, bool _has_header);
		~CSV();
		inline bool is_loaded(void)  { return loaded; }
		inline int getnfields() const { return cols; }
		inline int getnrows(void) const { return rows; }
		const char *GetFieldName(int _j)  {
			//return hdr[_j].c_str();
			return colinfo[_j].name.c_str();
		}
		bool get_data(int _row, int _col, std::string& str);

		/*
		bool getfield(int n, std::string& str);
		const char * getfield(int n);
		*/

		int GetFieldType(int _j);
		void fix(char *_str);
		void dump(char *_fname="csvdump.txt");

	private:
		typedef struct  {
			std::string name;
			char type;
			int maxlen;
		} COLINFO;

		std::vector<COLINFO> colinfo;

		bool has_header;
		char fname[256];
		FILE *stream;
		void load_header(FILE *stream);
		//std::vector<std::string> hdr;
		std::vector< std::vector<std::string> > data;				// two dimensional array of strings
		int rows, cols;
		void init(void);
		bool loaded;
		//std::vector<int> maxlen;			// maximum length of data in each column
		bool uses_quotes;

#ifdef _DEBUG
		int bp;
#endif

};
#endif		// #ifndef _CSV_H_


