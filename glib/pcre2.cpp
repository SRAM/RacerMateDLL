
#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#include <string.h>

#include <glib_defines.h>

#include <pcre2.h>


/**********************************************************************

**********************************************************************/

Pcre::Pcre(const char *_subject, const char *_pat, FILE *_stream, int _options, bool _find_all)  {
	int i, status;

	subject = _subject;
	pat = _pat;
	stream = _stream;
	options = _options;
	find_all = _find_all;

	init();

	sublen = (int)strlen(subject);

	re = pcre_compile(
				pat,			// the pattern
				options,				// default options
				&error,					// for error message
				&erroffset,				// for error offset
				NULL					// use default character tables
			);

	// compilation failed: print the error message and exit

	if (re==NULL)  {
		if (stream) fprintf(stream, "   PCRE compilation failed at offset %d: %s\n", erroffset, error);
		goto done;
	}

	/*************************************************************************
		If the compilation succeeded, we call PCRE again, in order to do a
		pattern match against the subject string. This does just ONE match.
		If further matching is needed, it will be done below.
	*************************************************************************/

	status = pcre_exec(
						re,						// the compiled pattern
						NULL,					// no extra data - we didn't study the pattern
						subject,				// the subject string
						sublen,					// the length of the subject
						0,						// start at offset 0 in the subject
						0,						// default options
						ovector,				// output vector for substring information
						OVECCOUNT				// number of elements in the output vector
					);

	if (status < 0)  {
		// Matching failed: handle error cases
		switch(status)  {
			case PCRE_ERROR_NOMATCH:									// -1
				if (stream) fprintf(stream, "   no match\n");
				break;
			default:
				if (stream) fprintf(stream, "   matching error %d\n", status);
				break;
		}

		goto done;
	}

	// match succeded

	matches++;

	if (stream) fprintf(stream, "   match succeeded at offset %d\n", ovector[0]);

	/*************************************************************************
		We have found the first match within the subject string. If the output
		vector wasn't big enough, set its size to the maximum. Then output any
		substrings that were captured.
	*************************************************************************/

	// the output vector wasn't big enough

	if (status == 0)  {
		status = OVECCOUNT/3;
		if (stream) fprintf(stream, "   ovector only has room for %d captured substrings\n", status - 1);
	}

	// show substrings stored in the output vector by number

	for (i=0; i<status; i++)  {
		const char *substring_start = subject + ovector[2*i];
		int substring_length = ovector[2*i+1] - ovector[2*i];
		if (stream) fprintf(stream, "   %02d: %.*s\n", i, substring_length, substring_start);
	}


	/**************************************************************************
		That concludes the basic part of this demonstration program. We have
		compiled a pattern, and performed a single match. The code that follows
		first shows how to access named substrings, and then how to code for
		repeated matches on the same subject.
	**************************************************************************/

	// See if there are any named substrings, and if so, show them by name. First
	// we have to extract the count of named parentheses from the pattern.

	(void)pcre_fullinfo(
						re,						// the compiled pattern
						NULL,					// no extra data - we didn't study the pattern
						PCRE_INFO_NAMECOUNT,	// number of named substrings
						&namecount				// where to put the answer
					);

	if (namecount <= 0)  {
		if (stream) fprintf(stream, "   no named substrings\n");
	}
	else  {
		unsigned char *tabptr;
		if (stream) fprintf(stream, "   named substrings\n");

		// Before we can access the substrings, we must extract the table for
		// translating names to numbers, and the size of each entry in the table

		(void)pcre_fullinfo(
						re,							// the compiled pattern
						NULL,						// no extra data - we didn't study the pattern
						PCRE_INFO_NAMETABLE,		// address of the table
						&name_table					// where to put the answer
					);

		(void)pcre_fullinfo(
						re,							// the compiled pattern
						NULL,						// no extra data - we didn't study the pattern
						PCRE_INFO_NAMEENTRYSIZE,	// size of each entry in the table
						&name_entry_size			// where to put the answer
					);

		// now we can scan the table and, for each entry, print the number, the name, and the substring itself

		tabptr = name_table;

		for (i=0; i<namecount; i++)  {
			int n = (tabptr[0] << 8) | tabptr[1];
			if (stream) fprintf(stream, "      (%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2, ovector[2*n+1] - ovector[2*n], subject + ovector[2*n]);
			tabptr += name_entry_size;
		}
	}


	/*************************************************************************
	If the "-g" option was given on the command line, we want to continue
	to search for additional matches in the subject string, in a similar
	way to the /g option in Perl. This turns out to be trickier than you
	might think because of the possibility of matching an empty string.
	What happens is as follows:

	If the previous match was NOT for an empty string, we can just start
	the next match at the end of the previous one.

	If the previous match WAS for an empty string, we can't do that, as it
	would lead to an infinite loop. Instead, a special call of pcre_exec()
	is made with the PCRE_NOTEMPTY and PCRE_ANCHORED flags set. The first
	of these tells PCRE that an empty string is not a valid match; other
	possibilities must be tried. The second flag restricts PCRE to one
	match attempt at the initial string position. If this match succeeds,
	an alternative to the empty string match has been found, and we can
	proceed round the loop.
	*************************************************************************/

	if (!find_all)  {
		goto done;
	}

	// loop for second and subsequent matches

	while(1)  {
		int options = 0;                 /* Normally no options */
		int start_offset = ovector[1];   /* Start at end of previous match */

		  /*
			If the previous match was for an empty string, we are finished if we are
			at the end of the subject. Otherwise, arrange to run another match at the
			same point to see if a non-empty match can be found.
		  */

		if (ovector[0] == ovector[1])  {
			if (ovector[0] == sublen)  {
				break;
			}
			options = PCRE_NOTEMPTY | PCRE_ANCHORED;
		}

		// run the next matching operation

		status = pcre_exec(
							re,					// the compiled pattern
							NULL,				// no extra data - we didn't study the pattern
							subject,	// the subject string
							sublen,				// the length of the subject
							start_offset,		// starting offset in the subject
							options,			// options
							ovector,			// output vector for substring information
							OVECCOUNT			// number of elements in the output vector
						);

		/*
		This time, a result of NOMATCH isn't an error. If the value in "options"
		is zero, it just means we have found all possible matches, so the loop ends.
		Otherwise, it means we have failed to find a non-empty-string match at a
		point where there was a previous empty-string match. In this case, we do what
		Perl does: advance the matching position by one, and continue. We do this by
		setting the "end of previous match" offset, because that is picked up at the
		top of the loop as the point at which to start again.
		*/

		if (status == PCRE_ERROR_NOMATCH)  {
			if (options == 0)
				break;
			ovector[1] = start_offset + 1;
			continue;							// Go round the loop again
		}

		// Other matching errors are not recoverable

		if (status < 0) {
			if (stream) fprintf(stream, "Matching error %d\n", status);
			continue;
		}

		// match succeded
		matches++;
		if (stream) fprintf(stream, "\nMatch succeeded again at offset %d\n", ovector[0]);

		// the match succeeded, but the output vector wasn't big enough

		if (status == 0)  {
			status = OVECCOUNT/3;
			if (stream) fprintf(stream, "ovector only has room for %d captured substrings\n", status - 1);
		}

		// as before, show substrings stored in the output vector by number, and then
		// also any named substrings

		for(i=0; i<status; i++)  {
			const char *substring_start = subject + ovector[2*i];
			int substring_length = ovector[2*i+1] - ovector[2*i];
			if (stream) fprintf(stream, "%2d: %.*s\n", i, substring_length, substring_start);
		}

		if (namecount <= 0)  {
			if (stream) fprintf(stream, "No named substrings\n");
		}
		else  {
			unsigned char *tabptr = name_table;
			if (stream) fprintf(stream, "Named substrings\n");
			for (i=0; i<namecount; i++)  {
				int n = (tabptr[0] << 8) | tabptr[1];
				if (stream) fprintf(stream, "(%d) %*s: %.*s\n", n, name_entry_size - 3, tabptr + 2,
				ovector[2*n+1] - ovector[2*n], subject + ovector[2*n]);
				tabptr += name_entry_size;
			}
		}
	}				// end of loop to find second and subsequent matches

done:
	if (re)  {
		pcre_free(re);		// release memory used for the compiled pattern
		re = NULL;
	}
}

/**********************************************************************

**********************************************************************/

Pcre::~Pcre()  {
#ifdef _DEBUG
	bp = 1;
#endif

}

/**********************************************************************

**********************************************************************/

void Pcre::init(void)  {
	matches = 0;
	re = NULL;
	extra = NULL;
#ifdef _DEBUG
	bp = 0;
#endif

	#if !defined NOPOSIX					// There are still compilers that require no indent
		do_posix = 0;
	#endif

	return;
}

/**********************************************************************

**********************************************************************/

int Pcre::get_offset(int _match)  {
	int match, start;
	//int end, len;

	if (_match<1)  {
		return -1;
	}

	match = _match - 1;

	start = ovector[match*2];
	//end = ovector[match*2+1];
	//len = end - start;
	return start;
}

/**********************************************************************

**********************************************************************/

int Pcre::get_len(int _match)  {
	int match, start, end, len;

	if (_match<1)  {
		return -1;
	}

	match = _match - 1;

	start = ovector[match*2];
	end = ovector[match*2+1];
	len = end - start;
	return len;
}
