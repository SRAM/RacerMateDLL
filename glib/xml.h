
#ifndef _XML_H_
#define _XML_H_

#include <vector>
#include <string>
#include <tinyxml.h>
#include <logger.h>

class XML  {

	private:
		Logger *logg;
		char fname[MAX_PATH];
		char path[MAX_PATH];				// does not include trailing '\'
		char buf[256];
		char str[2048];
		char error_string[256];
		const char *errptr;

		int init(void);
		void destroy(void);
		int bp;
		TiXmlElement *root_element;
		TiXmlElement *e;
		TiXmlText *titex;

		const char * getIndent( unsigned int numIndents );
		void dump( TiXmlNode* pParent, int level=0 );
		int dump_attribs(TiXmlElement* pElement, unsigned int indent);
		bool loadOkay;
		int collect(std::vector<std::string> &strs, int n, va_list args );
		void set_element_text(TiXmlElement *e, const char *str);


//#ifdef _DEBUG
		//char str[256];
//#endif
		const char *value;
		const char *text;
		TiXmlDocument *doc;

	public:
		//XML(void);
		//XML(const XML& copy);					// copy constructor
		XML( const char *_fname, Logger *_logg=NULL );
		virtual ~XML(void);
		const char *gettext(const char *_value);
		TiXmlElement *getelement(const char *_value);
		void dump(const char *_debugdir);
		void dump(void);
		TiXmlHandle *dochandle;
		//const char *get_text(const char *s1, const char *s2);
		//const char *get_text(const char *v, ...);
		//const char *get_text(std::vector<std::string>);
		const char *get_text(int n, ...);
		const char *get_text(int ix, int n, ...);
		const char *get_attribute(int ix, int n, ...);

		void set_text(const char *text, int n, ...);
		void set_text(int ix, const char *text, int n, ...);

		int get_int(int n, ...);
		int get_int(int ix, int n, ...);				// to get the 'which' child
		void set_int(int val, int n, ... );
		void set_int(int ix, int val, int n, ... );

		const char *get_error_string(void)  { return errptr; }


		/*
		float get_float(int n, ...);
		void set_float(float f, int n, ... );
		void set_float(int ix, float f, int n, ... );
		*/

		double get_double(int n, ...);
		double get_double(int ix, int n, ...);
		void set_double(double d, int n, ... );
		void set_double(int ix, double d, int n, ... );

		int get_child_count(int n, ...);
		void remove_child(int which, int n, ...);
		int add_child(int n, ...);

		void save(const char *_fname=NULL);

};

#endif			// #ifndef _XML_H_


