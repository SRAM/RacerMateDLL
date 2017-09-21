
#pragma warning(disable:4996)					// for vista strncpy_s

#include <string.h>

#include <utils.h>
#include <file_rotator.h>


/****************************************************************************

****************************************************************************/

fileRotator::fileRotator(char *_filename, int _n)  {
	int i;
	char fname1[256] = {0};
	char fname2[256] = {0};

	memset(stub, 0, sizeof(stub));
	memset(extension, 0, sizeof(extension));

	strncpy(stub, _filename, sizeof(stub)-1);
	strip_extension(stub);

	strncpy(extension, _filename, sizeof(extension)-1);
	get_extension(extension);

	n = _n;

	//	1 = newest file
	// 10 = oldest file

	// copy xxx-1.ext to xxx-2.ext, etc.

	for(i=n; i>=2; i--)  {
		sprintf(fname1, "%s-%d.%s", stub, i, extension);
		sprintf(fname2, "%s-%d.%s", stub, i-1, extension);
		if (exists(fname2))  {
			CopyFile(fname2, fname1, FALSE);					// existing, new
		}
	}

	CopyFile(_filename, fname2, FALSE);					// existing, new

	unlink(_filename);

}

/****************************************************************************

****************************************************************************/

fileRotator::~fileRotator(void) {

}

/****************************************************************************

****************************************************************************/

void fileRotator::get_name(int _k, char *_fname, int _len)  {
	sprintf(_fname, "%s-%d.%s", stub, _k, extension);
	return;
}

