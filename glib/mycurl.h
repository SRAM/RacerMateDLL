
#ifndef _MYCURL_H_
#define _MYCURL_H_

#include <curl/curl.h>

/************************************************************************************************************

************************************************************************************************************/

class MYCURL  {

	public:
		MYCURL(void);
		virtual ~MYCURL();
		int download(const char *url, const char *outfilename);
		int upload(const char *_fullpath, const char *_url );
		void reset(void);
		const char *get_error_string(void)  {
			if (error_string[0] != 0)  {
				return error_string;
			}
			else  {
				return NULL;
			}
		}

	private:
		#define UPMETH 3

		struct WriteThis {
		  const char *readptr;
		  int sizeleft;
		  FILE *stream;
		};
		FILE *logstream;
		char fullpath[MAX_PATH];
		char error_string[256];
		static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
#if UPMETH==1
		static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp);
#endif

	    CURL *curl;
		CURLcode res;
		char *basename;
		int bp;
		void init(void);

};

#endif		//#ifndef _MYCURL_H_


