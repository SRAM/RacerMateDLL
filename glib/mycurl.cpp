
#pragma warning(disable:4996)					// for vista strncpy_s

#include <glib_defines.h>
#include <utils.h>
#include <mycurl.h>


/******************************************************************************************

	download example:

		MYCURL *curl = NULL;
		char fname[256];
		int status, bp = 0;
		const char *url = "http://iz.hindbrain.net/x.x";

		strncpy (fname, mybasename(url), sizeof(fname)-1);

		curl = new MYCURL();
		status = curl->download(url, "x.x");
		switch (status)  {
			case 0:
				break;
			case 1:
				bp = 1;
				break;
			case 2:
				bp = 2;
				break;
			case 3:
				bp = 3;
				break;
			default:
				bp = 4;
				break;
		}

		DEL(curl);

	upload example:

******************************************************************************************/

MYCURL::MYCURL(void)  {
	init();

}

/******************************************************************************************

******************************************************************************************/

MYCURL::~MYCURL()  {
	if (curl)  {
		curl_easy_cleanup(curl);
		curl = NULL;
	}
	curl_global_cleanup();

	FCLOSE(logstream);
}

/******************************************************************************************

******************************************************************************************/

void MYCURL::init(void)  {
	logstream = fopen("curl.log", "wt");
	basename = NULL;
	bp = 0;
	res = CURLE_OK;
	memset(error_string, 0, sizeof(error_string));
	memset(fullpath, 0, sizeof(fullpath));

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	return;
}

/******************************************************************************************

******************************************************************************************/

size_t MYCURL::write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
#ifdef _DEBUG
	const char *cptr = (const char *)ptr;
#endif
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/******************************************************************************************

******************************************************************************************/

int MYCURL::download(const char *url, const char *outfilename)  {

    if (!curl) {
		return 1;
	}

    FILE *fp;
	fp = fopen(outfilename,"wb");
	if (fp==NULL)  {
		return 2;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);

	switch(res)  {
		case CURLE_OK:
			break;
		default:
			fclose(fp);
			return 3;
			break;
	}
	//curl_easy_cleanup(curl);
	fclose(fp);

	return 0;
}


/******************************************************************************************

******************************************************************************************/

void MYCURL::reset(void)  {
	bp = 0;
	res = CURLE_OK;

	return;
}

/**********************************************************************************************************

**********************************************************************************************************/

#if UPMETH==1

size_t MYCURL::read_callback(void *ptr, size_t size, size_t nmemb, void *userp)  {
	struct WriteThis *pooh = (struct WriteThis *)userp;
	char c;

	if(size*nmemb < 1)
		return 0;
 
	if(pooh->sizeleft) {
		//fprintf(pooh->stream, "%s\n", pooh->readptr);
		c = pooh->readptr[0];
		fprintf(pooh->stream, "%c", c);

		*(char *)ptr = pooh->readptr[0];		// copy one single byte
		pooh->readptr++;						// advance pointer
		pooh->sizeleft--;						// less data left
		return 1;								// we return 1 byte at a time!
	}
 
	return 0;									// no more data left to deliver
}
#endif

/******************************************************************************************

******************************************************************************************/


int MYCURL::upload(const char *_fullpath, const char *_url ) {
	//struct curl_httppost *post=NULL;
	//struct curl_httppost *last=NULL;
	int rc = 0;

#if 0
	CURL *curl=NULL;
	CURLcode res=CURLE_OK;
	CURLINFO info;
	FILE *stream=NULL;
	double dsize;
	/*
		http://www.racermate.net/tts/upload.php
		http://www.racermate.net/tts/admin.php  and log in
			larry barzot
			ray ray_at_fscn
			norm ****_****
	*/
	struct curl_slist *headerlist=NULL;
	char fname[256] = {0};
	curl_global_init(CURL_GLOBAL_ALL);
	strcpy(fname, "c:\\users\\larry\\desktop\\tts.db");
	if (!exists(fname))  {
		return;
	}
#endif


#if UPMETH==0

<form action="upload.php" enctype="multipart/form-data" method="post">
	Enter file: <input type="file" name="datafile" size="40">
	<input type="hidden" name="MAX_FILE_SIZE" value="1000000">
	<input type="submit" value="Upload">
</form>
request:
	Array (
			[MAX_FILE_SIZE] => 1000000
	)
files:
	Array (
		[datafile] => Array (
						[name] => tts.db
						[type] => application/octet-stream 
						[tmp_name] => /tmp/phpNL4snC
						[error] => 0
						[size] => 3072
					) 
	)
	www.racermate.net:80 192.168.1.20 - - [14/Dec/2011:09:09:54 -0500] "POST /tts/upload.php HTTP/1.1" 200 393 "-" "-"

#elif UPMETH==1

	struct curl_httppost *post=NULL;
	struct curl_httppost *last=NULL;
	long code;
	const char *cptr;
	//FILE  *stream2;
	//stream2 = fopen("curl.log", "wt" );
	if (logstream)  {
		//curl_easy_setopt(curl, CURLOPT_STDERR, logstream );
	}

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L );
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.racermate.net/tts/upload.php");
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

	//curl_easy_setopt(curl, CURLOPT_STDERR, );
	//curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, );

	curl_formadd(&post, &last,   CURLFORM_COPYNAME, "name",				CURLFORM_COPYCONTENTS, "daniel", CURLFORM_END);
	curl_formadd(&post, &last,   CURLFORM_COPYNAME, "project",			CURLFORM_COPYCONTENTS, "curl", CURLFORM_END);
	curl_formadd(&post, &last,   CURLFORM_COPYNAME, "logotype-image",   CURLFORM_FILECONTENT, "curl.png", CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post));		// if we don't provide POSTFIELDSIZE, libcurl will strlen() by itself
 	curl_easy_setopt(curl, CURLOPT_READDATA, post);								// pointer to pass to our read function

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)  {
		cptr = curl_easy_strerror(res);

		curl_easy_getinfo(curl, CURLINFO_HTTP_CONNECTCODE, &code);
		curl_easy_getinfo(curl, CURLINFO_HEADER_SIZE, &code);


		if (res==CURLE_READ_ERROR)  {					// "Failed to open/read local data from file/application"
			bp = 1;
		}
		bp = 2;
	}

	//curl_formfree(post);
	curl_easy_cleanup(curl);				// always cleanup
	curl = NULL;
	curl_formfree(post);					// then cleanup the formpost chain
	//curl_slist_free_all (headerlist);		// free slist
	//curl_global_cleanup();

	//FCLOSE(stream2);
#elif UPMETH==2
	// http://curl.haxx.se/libcurl/c/postit2.html

/* Example code that uploads a file name 'foo' to a remote script that accepts
 * "HTML form based" (as described in RFC1738) uploads using HTTP POST.
 *
 * The imaginary form we'll fill in looks like:
 *
 * <form method="post" enctype="multipart/form-data" action="examplepost.cgi">
 * Enter file: <input type="file" name="sendfile" size="40">
 * Enter file name: <input type="text" name="filename" size="30">
 * <input type="submit" value="send" name="submit">
 * </form>
 *
 * This exact source code has not been verified to work.
 */ 
 
	//#include <stdio.h>
	//#include <string.h>
	//#include <curl/curl.h>
 
	//CURL *curl;
	//CURLcode res;

	if (!exists(_fullpath))  {
		return 1;
	}

	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headerlist=NULL;
	static const char buf[] = "Expect:";

	//curl_global_init(CURL_GLOBAL_ALL);
 
	// Fill in the file upload field
	curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "sendfile",
               CURLFORM_FILE, _fullpath,
               CURLFORM_END);
 
	// Fill in the filename field
	curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "filename",
               CURLFORM_COPYCONTENTS, _fullpath,
               CURLFORM_END);
 
 
	// Fill in the submit field too, even if this is rarely needed
	curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "submit",
               CURLFORM_COPYCONTENTS, "send",
               CURLFORM_END);
 
	//curl = curl_easy_init();
	//if(curl) {
	//	return 1;
	//}

	headerlist = curl_slist_append(headerlist, buf);	// initalize custom header list (stating that Expect: 100-continue is not wanted

	// what URL that receives this POST
	curl_easy_setopt(curl, CURLOPT_URL, _url);

	//if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )  {
	if (false)  {
		// only disable 100-continue header if explicitly requested
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	}

	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
 
	// Perform the request, res will get the return code
	res = curl_easy_perform(curl);
	// Check for errors
	if(res != CURLE_OK)  {
		//fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		strncpy(error_string, curl_easy_strerror(res), sizeof(error_string)-1);
		// "Failed to open/read local data from file/application"
		rc = 2;
	}

	// always cleanup
	curl_easy_cleanup(curl);
	curl = NULL;
 
	// then cleanup the formpost chain
	curl_formfree(formpost);
	// free slist
	curl_slist_free_all (headerlist);
#elif UPMETH==3
	// http://stackoverflow.com/questions/2657318/how-can-i-curl-post-a-file-using-file-pointer-in-c

	if (!exists(_fullpath))  {
		return 1;
	}
	if(!curl)  {
		return 2;
	}

	struct curl_httppost *formpost=NULL;
	struct curl_httppost *lastptr=NULL;
	struct curl_slist *headers=NULL;

	strncpy(fullpath, _fullpath, sizeof(fullpath)-1);
	basename = mybasename(fullpath);

	headers = curl_slist_append(headers, "Content-Type: multipart/form-data");

	std::string s = file_get_contents(_fullpath);
	const char *p = s.c_str();

	curl_formadd(
					&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "x1",
					CURLFORM_FILE, fullpath,
					CURLFORM_END
				);

	curl_formadd(
					&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "x2",
					CURLFORM_COPYCONTENTS, (char*)p,
					CURLFORM_END
				);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_URL, _url);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)  {
		rc = 3;
	}

	curl_formfree(formpost);
	curl_slist_free_all (headers);

	s.clear();

#endif								// #if UPMETH==X

	return rc;
}									// upload()


