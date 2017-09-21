
#ifndef _QT_PERSON_H_
#define _QT_PERSON_H_

#include <memory.h>
#include <string>
#include "qt_defines.h"

namespace MYQT  {

class qt_Person  {

protected:
	 int year;
	 int month;
	 int day;
	 double age;
	 double kgs;
	 char sex;
	 float cm;
	 std::string lastname;
	 std::string firstname;
	 std::string username;
	 std::string email;
	 std::string city;							// where they live
	 std::string state;							// where they live
	 std::string country;						// where they live
	 std::string zip;							// where they live
	 std::string birthdate;
	 char name[16];								// for display, eg 'DebbieT'

private:
	 char gstr[256];
	 void convertDOB(void);
	 char fullname[128];
	 char middlename[32];
	 char regularname[128];						// "Roger W Moore"
	 char dbkey[32];
	 unsigned char bmo;			// birth month (1-12)
	 unsigned char bday;			// birth day (1-31)
	 short byear;					// birth year (eg, 2003)
	 char phone[32];
	 char address[64];
	 void destroy(void);
	 char dob[32];					// eg, 12/7/1941
	 void init(void);
	 char initials[8];
	 char name2[16];
	 void setInitials(void);
	 void setName2(void);
	 char mi[4];						// middle initial
	 char econtact[40];
	 char ephone[20];

public:
	 qt_Person(void);
	 virtual ~qt_Person();

	 void dump(char *_fname);
	 void my_export(FILE *stream, bool _metric);

	 //--------------
	 // getters:
	 //--------------

	 char *getFullName(void)  {						// used to construct a folder name that will sort by last name
		  if (mi[0] != 0)  {
				sprintf(fullname, "%s_%s_%s", lastname.c_str(), mi, firstname.c_str());
		  }
		  else  {
				sprintf(fullname, "%s_%s", lastname.c_str(), firstname.c_str());
		  }
		  return fullname;
	 }

	 int getlnamesize(void)  {
		  return sizeof(lastname.c_str());
	 }


	 int getfnamesize(void)  {
		  return sizeof(firstname.c_str());
	 }

	 char *getDOB(void)  {
		  return dob;
	 }

	 double getAge(void)  {
		  return age;
	 }

	 float getpounds(void)  {
		  return (float) (KGSTOLBS*kgs);
	 }

	 double getkgs(void)  {
		  return kgs;
	 }

	 const char *getlname(void)  {
		  return lastname.c_str();
	 }
	 const char *getfname(void)  {
		  return firstname.c_str();
	 }
	 char *getmname(void)  {
		  return middlename;
	 }
	 float getcm(void)  {
		  return cm;
	 }

	 int getbmo(void)  {
		  return bmo;
	 }
	 int getbday(void)  {
		  return bday;
	 }
	 int getbyear(void)  {
		  return byear;
	 }

	 char *getphone(void)  {
		  return phone;
	 }

	 char *getaddress(void)  {
		  return address;
	 }

	 const char *getcity(void)  {
		  return city.c_str();
	 }

	 const char *getstate(void)  {
		  return state.c_str();
	 }

	 const char *getcountry(void)  {
		  return country.c_str();
	 }

	 const char *getzip(void)  {
		  return zip.c_str();
	 }

	 const char *getemail(void)  {
		  return email.c_str();
	 }

	 char *getecontact(void)  {
		  return econtact;
	 }

	 char *getephone(void)  {
		  return ephone;
	 }

	 char getsex(void)  {
		  return sex;
	 }

	 const char *getsexString(void)  {
		  if (sex=='M')  {
				return "Male";
		  }
		  else if (sex=='F')  {
				return "Female";
		  }
		  else  {
				return "Unknown";
		  }
	 }

	 char *getInitials(void)  {
		  return initials;
	 }

	 char *getName2(void)  {
		  return name2;
	 }

	 char *getmi(void)  {				// returns the middle initial
		  return mi;
	 }

	 char *getName(void)  {
		  if (mi[0]==0 || middlename[0]==0)  {		// no middle name
				//if (firstname[0] && lastname[0])  {
				sprintf(regularname, "%s %s", firstname.c_str(), lastname.c_str());
				//}
		  }
		  else  {
				sprintf(regularname, "%s %s %s", firstname.c_str(), mi, lastname.c_str());
		  }
		  return regularname;
	 }

	 float getInches(void)  {
		  return (float) (CMTOINCHES*cm);
	 }

	 const char *get_firstname(void)  {
		  return firstname.c_str();
	 }

	 const char *get_lastname(void)  {
		  return lastname.c_str();
	 }

	 //--------------
	 // setters:
	 //--------------

	 void setDOB(const char *_dob)  {			// eg, 	// "07/04/1776"
		  strncpy(dob, _dob, sizeof(dob)-1);
		  convertDOB();						// maintain the date numerically
		  return;
	 }

	 void setpounds(double _pounds)  {
		  kgs = LBSTOKGS * _pounds;
		  return;
	 }

	 void setkgs(double _kgs)  {
		  kgs = _kgs;
		  return;
	 }

	 void setlname(char *_c)  {
		  lastname = _c;
		  return;
	 }

	 void setfname(char *_c)  {
		  firstname = _c;
		  return;
	 }

	 void setmname(char *_c)  {
		  strncpy(middlename, _c, sizeof(middlename)-1);
		  mi[0] = middlename[0];
		  //QString s =
		  //strupr(mi);
		  return;
	 }

	 void setcm(float _cm)  {
		  cm = _cm;
		  return;
	 }

	 void setInches(float _inches)  {
		  cm = (float) (INCHES_TO_CM*_inches);
		  return;
	 }

	 void setbmo(int _i)  {
		  bmo = _i;
		  return;
	 }
	 void setbday(int _i)  {
		  bday = _i;
		  return;
	 }
	 void setbyear(int _i)  {
		  byear = _i;
		  return;
	 }

	 void setphone(char *_c)  {
		  strncpy(phone, _c, sizeof(phone)-1);
		  return;
	 }

	 void setaddress(char *_c)  {
		  strncpy(address, _c, sizeof(address)-1);
		  return;
	 }

	 void setsex(char _c)  {
		  sex = _c;
		  return;
	 }

	 void setcity(char *_c)  {
		  city = _c;
		  return;
	 }

	 void setstate(char *_c)  {
		  state = _c;
		  return;
	 }

	 void setzip(char *_c)  {
		  zip = _c;
		  return;
	 }

	 void setemail(char *_c)  {
		  email = _c;
		  return;
	 }
	 void setecontact(char *_c)  {
		  strncpy(econtact, _c, sizeof(econtact)-1);
		  return;
	 }
	 void setephone(char *_c)  {
		  strncpy(ephone, _c, sizeof(ephone)-1);
		  return;
	 }

};
}

#endif		// #ifndef _PERSON_H_

