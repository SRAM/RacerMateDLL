/* 

Normally, when you search for a sub-string in a string, the match should be
exact. So if we search for a sub-string "abc" then the string being searched
should contain these exact letters in the same sequence for a match to be
found. We can extend this kind of search to a case insensitive search where the
sub-string "abc" will find strings like "Abc", "ABC" etc. That is, the case is
ignored but the sequence of the letters should be exactly the same. Sometimes,
a case insensitive search is also not enough. For example, if we want to search
for numeric digit, then we basically end up searching for each digit
independantly. This is where regular expressions come in to our help. 
(continued)



Regular expressions are text patterns that are used for string matching.
Regular expressions are strings that contains a mix of plain text and special
characters to indicate what kind of matching to do. Here's a very brief
turorial on using regular expressions before we move on to the code for
handling regular expressions. 

Suppose, we are looking for a numeric digit then the regular expression we
would search for is "[0-9]". The brackets indicate that the character being
compared should match any one of the characters enclosed within the bracket.
The dash (-) between 0 and 9 indicates that it is a range from 0 to 9.
Therefore, this regular expression will match any character between 0 and 9,
that is, any digit. If we want to search for a special character literally we
must use a backslash before the special character. For example, the single
character regular expression "\*" matches a single asterisk. In the table below
the special characters are briefly described. 

Character
 Description
 
^
 Beginning of the string. The expression "^A" will match an A only at the
beginning of the string.
 
^
 The caret (^) immediately following the left-bracket ([) has a different
meaning. It is used to exclude the remaining characters within brackets from
matching the target string. The expression "[^0-9]" indicates that the target
character should not be a digit.
 
$
 The dollar sign ($) will match the end of the string. The expression "abc$"
will match the sub-string "abc" only if it is at the end of the string.
 
|
 The alternation character (|) allows either expression on its side to match
the target string. The expression "a|b" will match a as well as b.
 
.
 The dot (.) will match any character.
 
*
 The asterix (*) indicates that the character to the left of the asterix in the
expression should match 0 or more times.
 
+
 The plus (+) is similar to asterix but there should be at least one match of
the character to the left of the + sign in the expression.
 
?
 The question mark (?) matches the character to its left 0 or 1 times.
 
()
 The parenthesis affects the order of pattern evaluation and also serves as a
tagged expression that can be used when replacing the matched sub-string with
another expression.
 
[]
 Brackets ([ and ]) enclosing a set of characters indicates that any of the
enclosed characters may match the target character.
 


The parenthesis, besides affecting the evaluation order of the regular
expression, also serves as tagged expression which is something like a
temporary memory. This memory can then be used when we want to replace the
found expression with a new expression. The replace expression can specify a &
character which means that the & represents the sub-string that was found. So,
if the sub-string that matched the regular expression is "abcd", then a replace
expression of "xyz&xyz" will change it to "xyzabcdxyz". The replace expression
can also be expressed as "xyz\0xyz". The "\0" indicates a tagged expression
representing the entire sub-string that was matched. Similarly we can have
other tagged expression represented by "\1", "\2" etc. Note that although the
tagged expression 0 is always defined, the tagged expression 1,2 etc. are only
defined if the regular expression used in the search had enough sets of
parenthesis. Here are few examples. 

String
 Search
 Replace
 Result
 
Mr.
 (Mr)(\.)
 \1s\2
 Mrs.
 
abc
 (a)b(c)
 &-\1-\2
 abc-a-c
 
bcd
 (a|b)c*d
 &-\1
 bcd-b
 
abcde
 (.*)c(.*)
 &-\1-\2
 abcde-ab-de
 
cde
 (ab|cd)e
 &-\1
 cde-cd
 

The code for regular expression search is given below. This code was derived
from work done by Henry Spencer. Besides adding a C++ wrapper around C code, a
couple of functions were added to enable search and replace operations. 

*/

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

//#define NSUBEXP  10

/************************************************************************************

	example usage:

		#include <cregexp.h>

		CRegExp r;
		char *buf = "abcdatefg";
		char *pattern = "dat";

		r.RegComp(pattern);				// set pattern
		int pos = r.RegFind(buf);		// find pattern in buf


************************************************************************************/

class CRegExp {
	public:
		CRegExp();
		~CRegExp();

		CRegExp *RegComp( const TCHAR *re );

		int RegFind(const TCHAR *str);

		TCHAR* GetReplaceString( const TCHAR* sReplaceExp );

		int GetFindLen() {
			if( startp[0] == NULL || endp[0] == NULL )  {
				return 0;
			}
			return endp[0] - startp[0];
		}


	private:

		#define NSUBEXP  10

		// definition	number	opnd?	meaning 
		#define	END		0		// no	End of program. 
		#define	BOL		1		// no	Match beginning of line. 
		#define	EOL		2		// no	Match end of line. 
		#define	ANY		3		// no	Match any character. 
		#define	ANYOF	4		// str	Match any of these. 
		#define	ANYBUT	5		// str	Match any but one of these. 
		#define	BRANCH	6		// node	Match this, or the next..\&. 
		#define	BACK	7		// no	"next" ptr points backward. 
		#define	EXACTLY	8		// str	Match this string. 
		#define	NOTHING	9		// no	Match empty string. 
		#define	STAR	10		// node	Match this 0 or more times. 
		#define	PLUS	11		// node	Match this 1 or more times. 
		#define	OPEN	20		// no	Sub-RE starts here. 
								//		OPEN+1 is number 1, etc. 
		#define	CLOSE	30		// no	Analogous to OPEN. 

		// Utility definitions.
 
		#define	FAIL(m)		{ regerror(m); return(NULL); }
		#define	ISREPN(c)	((c) == _T('*') || (c) == _T('+') || (c) == _T('?'))
		#define	META		"^$.[()|?+*\\"

		// Flags to be passed up and down.
 
		#define	HASWIDTH	01	// Known never to match null string. 
		#define	SIMPLE		02	// Simple enough to be STAR/PLUS operand. 
		#define	SPSTART		04	// Starts with * or +. 
		#define	WORST		0	// Worst case. 

		TCHAR *regnext(TCHAR *node);
		void reginsert(TCHAR op, TCHAR *opnd);

		int regtry(TCHAR *string);
		int regmatch(TCHAR *prog);
		size_t regrepeat(TCHAR *node);
		TCHAR *reg(int paren, int *flagp);
		TCHAR *regbranch(int *flagp);
		void regtail(TCHAR *p, TCHAR *val);
		void regoptail(TCHAR *p, TCHAR *val);
		TCHAR *regpiece(int *flagp);
		TCHAR *regatom(int *flagp);

		// Inline functions

	//private:
		TCHAR OP(TCHAR *p) {return *p;};
		TCHAR *OPERAND( TCHAR *p) {return (TCHAR*)((short *)(p+1)+1); };

		// regc - emit (if appropriate) a byte of code
		void regc(TCHAR b) {
			if (bEmitCode)
				*regcode++ = b;
			else
				regsize++;
		};

		#ifdef _DEBUG
			char dbgstr[64];
		#endif

		// regnode - emit a node
		TCHAR *	regnode(TCHAR op) {
			if (!bEmitCode) {
				regsize += 3;
				return regcode;
			}

			*regcode++ = op;
			*regcode++ = _T('\0');		/* Null next pointer. */
			*regcode++ = _T('\0');

			return regcode-3;
		};


	//private:
		BOOL bEmitCode;
		BOOL bCompiled;
		TCHAR *sFoundText;

		TCHAR *startp[NSUBEXP];
		TCHAR *endp[NSUBEXP];
		TCHAR regstart;		// Internal use only. 
		TCHAR reganch;		// Internal use only. 
		TCHAR *regmust;		// Internal use only. 
		int regmlen;		// Internal use only. 
		TCHAR *program;		// Unwarranted chumminess with compiler. 

		TCHAR *regparse;	// Input-scan pointer. 
		int regnpar;		// () count. 
		TCHAR *regcode;		// Code-emit pointer; ®dummy = don't. 
		TCHAR regdummy[3];	// NOTHING, 0 next ptr 
		long regsize;		// Code size. 

		TCHAR *reginput;	// String-input pointer. 
		TCHAR *regbol;		// Beginning of input, for ^ check. 
};

