
#ifdef WIN32
	#define STRICT
	#pragma warning(disable:4996 4005)					// for vista strncpy_s
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#include <tdefs.h>

#ifndef NEWUSER

#include <utils.h>
#include <fatalerror.h>
#include <user.h>

/**********************************************************************

**********************************************************************/

User::User(void)  {

#ifdef WIN32
	hinstance = NULL;
	phwnd = NULL;
#endif

	init();

	// set up some defaults

	data.metric = FALSE;
	strcpy(data.name, "N/A");
	strcpy(data.sex, "M");
	data.age = 21;
	data.lbs = 0.0f;
	data.kgs = (float) (150.0*TOKGS);
	data.lower_hr = 60.0f;
	data.upper_hr = 70.0f;

}

/**********************************************************************

**********************************************************************/

User::User(char *_fname)  {

#ifdef WIN32
	hinstance = NULL;
	phwnd = NULL;
#endif
	
	init();

	if (!exists(_fname))  {
		throw(fatalError(__FILE__, __LINE__, "Can't find user file"));
	}

	strncpy(fname, _fname, 255);

	fsize = filesize_from_name(fname);

	stream = fopen(fname, "rb");

	if (stream==NULL)  {
		if (indexIgnoreCase(fname, ".udf", 0) != -1)  {
			destroy();
			throw("Can't Open VDF File");
		}
	}
	make();
	fclose(stream);
	stream = NULL;
	return;
}

/**********************************************************************
	creates a user from a USER_DATA structure
	used in multi rider
**********************************************************************/

User::User(USER_DATA *_user_data)  {

#ifdef WIN32
	hinstance = NULL;
	phwnd = NULL;
#endif
	
	init();

	if (_user_data)  {
		memcpy(&data, _user_data, sizeof(data));
	}

	if (data.drag_aerodynamic==0.0f)  {
		watts_factor = 1.0f;
		data.drag_tire = .006f;
		data.drag_aerodynamic = 8.0f;
	}
	else  {
		data.drag_tire = .006f;
		watts_factor = data.drag_aerodynamic / 8.0f;;
	}


	return;
}

/**********************************************************************
	Destructor for User class
**********************************************************************/

User::~User()  {

	if (stream==NULL)  {
		if (fname[0]!=0)   {
			//watts_factor = data.drag_aerodynamic / 8.0f;;
			data.drag_aerodynamic = watts_factor * 8.0f;
		}
	}

	destroy();
}

/**********************************************************************

**********************************************************************/

int User::sanityCheck(void)  {
	if ( (data.age < 0) || (data.age > 200) ) {
		return 1;
	}
	if ( (data.kgs < 0.0f) || (data.kgs > 1000.0f) ) {
		return 1;
	}
	if (data.drag_aerodynamic==0.0)  {
		data.drag_aerodynamic = 8.0f;
	}
	if (data.drag_tire == 0.0)  {
		data.drag_tire = .006f;
	}
	return 0;
}

/**********************************************************************

**********************************************************************/

void User::destroy(void)  {
	if (stream)  {
		fclose(stream);
		stream = NULL;
	}

	//DEL(s);

	return;
}

/**********************************************************************

**********************************************************************/

void User::init(void)  {

	ftp = 200.0f;
	fsize = 0;
	watts_factor = 1.0f;
	version = 1;
	memset(&data, 0, sizeof(data));
	data.drag_aerodynamic = 8.0f;
	data.drag_tire = .006f;
	//s = NULL;
//	usr = this;
	stream = NULL;
	canceled = false;
#ifdef WIN32
	hwnd = NULL;
#endif

	caption[0] = 0;
	user_data_size = 0;
	file_size = 0;
	ueheader[0] = 0;
//	memset(&olddata, 0, sizeof(OLD_USER_DATA));
	fname[0] = 0;

	return;
}

/**********************************************************************

**********************************************************************/

void User::make(void)  {
	int status;

	status = fread(&version, sizeof(DWORD), 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__, "Error Reading VDF File"));
	}

	if (version == 1)  {
	}
	else if (version==2)  {
	}
	else  {
		throw(fatalError(__FILE__, __LINE__, "Bad Version In VDF File"));
	}

	if (fsize == 236)  {							// version 1
	}
	else if (fsize==240)  {						// version 2 (added ftp)
	}
	else  {
		throw(fatalError(__FILE__, __LINE__, "Bad file size in user file"));
	}

	status = fgetc(stream);					// get the global metric flag
	if (status==EOF)  {
		throw(fatalError(__FILE__, __LINE__));
	}

	status = fread(ueheader, 23, 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__, "VDF Error 1001"));
	}

	//int size = sizeof(USER_DATA);								// 208

	status = fread(&data, sizeof(USER_DATA), 1, stream);
	if (status != 1)  {
		throw(fatalError(__FILE__, __LINE__, "VDF Error 1001"));
	}

	if (data.drag_aerodynamic<=0.0f)  {
		watts_factor = 1.0f;
		data.drag_tire = .006f;
		data.drag_aerodynamic = 8.0f;
	}
	else  {
		data.drag_tire = .006f;
		watts_factor = data.drag_aerodynamic / 8.0f;;
	}

	if (version==2)  {
		float f;
		status = fread(&f, sizeof(float), 1, stream);
		if (status != 1)  {
			throw(fatalError(__FILE__, __LINE__, "VDF Error 1001"));
		}
		ftp = f;
	}


	return;
}

/**********************************************************************

**********************************************************************/

float User::getWeight(void)  {
	if (data.metric)  {
		return data.kgs;
	}
	else  {
		return data.lbs;
	}
}

/**********************************************************************

**********************************************************************/

void User::set_watts_factor(float _f)  {
	watts_factor = _f;
	data.drag_aerodynamic = 8.0f * watts_factor;
	return;
}


#endif			// #ifndef NEWUSER


