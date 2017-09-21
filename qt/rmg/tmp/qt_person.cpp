

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
    //#include <time.h>
    #include <windows.h>
#endif

#include "qt_utils.h"
#include "qt_person.h"

#include "qt_defines.h"

namespace MYQT  {


/**********************************************************************

**********************************************************************/

qt_Person::qt_Person(void)  {

    init();

    return;
}



/**********************************************************************
    Destructor
**********************************************************************/

qt_Person::~qt_Person()  {
    destroy();
}

/**********************************************************************

**********************************************************************/

void qt_Person::destroy(void)  {

    /*
    if (lname)  {
        delete[] lname;
        lname = NULL;
    }

    if (fname)  {
        delete[] fname;
        fname = NULL;
    }

    if (mname)  {
        delete[] mname;
        mname = NULL;
    }

    if (phone)  {
        delete[] phone;
        phone = NULL;
    }

    if (address)  {
        delete[] address;
        address = NULL;
    }
    */

    return;
}

/**********************************************************************
        char *lname;
        char *fname;
        char *mname;
        float cm;				// height in cm
        char sex;				// 0 = male, 1 = female
        int years;				// age in years
        float kgs;				// weight in kgs
        int bmo;					// birth month (1-12)
        int bday;				// birth day (1-31)
        int byear;				// birth year (eg, 2003)
        char *phone;
        char *address;

**********************************************************************/

void qt_Person::dump(char *_fname)  {

    FILE *stream;

    stream = fopen(_fname, "wt");
    fprintf(stream, "last name = %s\n", lastname.c_str());
    fprintf(stream, "first name = %s\n", firstname.c_str());
    fprintf(stream, "middle name = %s\n", middlename);
    fprintf(stream, "cm = %.2f\n", cm);
    fprintf(stream, "sex = %s\n", sex ? "F":"M");
    //fprintf(stream, "age = %d\n", years);
    fprintf(stream, "age = %.0f\n", age);
    fprintf(stream, "kgs = %.0f\n", kgs);
    fprintf(stream, "birth month = %d\n", bmo);
    fprintf(stream, "birth day = %d\n", bday);
    fprintf(stream, "birth year = %d\n", byear);
    fprintf(stream, "phone = %s\n", phone);
    fprintf(stream, "address = %s\n", address);

    fclose(stream);

    return;
}

/**********************************************************************
    extracts byear, bmo, bday , years from strings like:

    "07/04/1776"
    "1/4/1978"

    entry:
        dob[] contains the string to extract data from

**********************************************************************/

void qt_Person::convertDOB(void)  {

    char seps[]   = "/";
    char *token;
    int count = 0;
    char str[32];
    strncpy(str, dob, sizeof(str)-1);

   token = strtok(str, seps);

   while( token != NULL )  {
        count++;
        if (count==1)  {
            bmo = (unsigned char)atoi(token);
            if (bmo<1 || bmo>12)  {
                bmo = 7;
            }
        }
        else if (count==2)  {
            bday = (unsigned char)atoi(token);
            if (bday<1 || bday > 31)  {
                bday = 4;
            }
        }
        else  {
            byear = (unsigned char)atoi(token);
            if (byear<0 || byear > 3000)  {
                byear = 1776;
            }
        }

      token = strtok( NULL, seps );
   }

#ifdef WIN32
    SYSTEMTIME x;
    GetLocalTime(&x);
    //years = (unsigned char) (x.wYear - byear);
    age = (double) (x.wYear - byear);
    //age2 = x.wYear - byear;
#else
    //years = 2;
    age = 25.0f;
#endif

    return;
}

/**********************************************************************

**********************************************************************/

void qt_Person::setInitials(void)  {
    initials[0] = firstname[0];
    initials[1] = lastname[0];
    mi[0] = middlename[0];
    strupr(initials);
    strupr(mi);
    return;
}

/**********************************************************************
    eg, "RogerM"
**********************************************************************/

void qt_Person::setName2(void)  {
    memset(name2, 0, sizeof(name2));
    //strncpy(name2, firstname, sizeof(name2)-2);
    strncpy(name2, firstname.c_str(), sizeof(name2)-2);

    int i = (int)strlen(name2);
    name2[i] = lastname[0];
    //name2[i+1] = 0;

    //name2[0] = firstname[0];
    //initials[1] = lastname[0];
    //strupr(initials);
    return;
}

/**********************************************************************

**********************************************************************/

void qt_Person::init(void)  {
    //memset(lastname, 0, sizeof(lastname));
    //memset(firstname, 0, sizeof(firstname));
    memset(middlename, 0, sizeof(middlename));
    memset(fullname, 0, sizeof(fullname));
    memset(mi, 0, sizeof(mi));
    memset(phone, 0, sizeof(phone));
    memset(address, 0, sizeof(address));
    memset(dob, 0, sizeof(dob));
    strcpy(dob, "");
    //memset(xbdobstr, 0, sizeof(xbdobstr));
    //strcpy(xbdobstr, "");
//	memset(city, 0, sizeof(city));
//	memset(state, 0, sizeof(state));
//	memset(country, 0, sizeof(country));
//	memset(zip , 0, sizeof(zip));
//	memset(email , 0, sizeof(email));
    memset(econtact , 0, sizeof(econtact));
    memset(ephone , 0, sizeof(ephone));
    memset(initials, 0, sizeof(initials));
    memset(name2, 0, sizeof(name2));
    memset(gstr, 0, sizeof(gstr));
    memset(regularname, 0, sizeof(regularname));
    memset(dbkey, 0, sizeof(dbkey));

//	metric = false;
    cm = 0.0f;
    sex = (char)0xff;
    //years = 0;
    age = 0.0;
    kgs = 0.0001;			// to prevent possible divide by 0 in watts/kg

    bmo = 1;
    bday = 1;
    byear = 1900;

    sex = 'M';

    return;
}

/**********************************************************************

**********************************************************************/

void qt_Person::my_export(FILE *stream, bool _metric)  {
    float f;

    fprintf(stream, "\tName: %s\n", getName());

    if (_metric)  {
        f = (float)kgs;
        fprintf(stream, "\tWeight: %.2f kilograms\n", f);
    }
    else  {
        f = (float)getpounds();
        fprintf(stream, "\tWeight: %.2f pounds\n", f);
    }

    if (_metric)  {
        f = cm;
        fprintf(stream, "\tHeight: %.2f cm\n", f);
    }
    else  {
        f = getInches();
        fprintf(stream, "\tHeight: %.2f inches\n", f);
    }

    fprintf(stream, "\tGender: %s\n", getsexString() );
    fprintf(stream, "\tDate Of Birth: %s\n", dob);
    fprintf(stream, "\tPhone Number: %s\n", phone);
    fprintf(stream, "\tAddress: %s\n", address);

    return;
}

}

