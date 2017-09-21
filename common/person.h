
#ifndef _PERSON_H_
#define _PERSON_H_

#include <stdio.h>
#include <string>

#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#else
	#include <string.h>
	#include <comutils.h>
#endif

#include "comdefs.h"

/*
struct PERSON_DATA  {
	PERSON_DATA(void)  {
		years = 25;
		kgs = (float) (160.0 * TOPOUNDS);
		sex = 'M';
		strcpy(fullname, "");
	}

	int years;				// age in years
	float kgs;                              // default to 160 lbs
	char sex;
	char fullname[128];

};
*/

#if 1

typedef struct {
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
} PERSON_DATA;


#else
//typedef struct {
struct PERSON_DATA  {
//class PERSON_DATA  {
//    public:
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

		  PERSON_DATA()  {
				lastname = "";                                              // ******** CRASHES HERE IN LINUX ****************
				firstname= "";
				username= "";
				email= "";
				city= "";							// where they live
				state= "";							// where they live
				country= "";						// where they live
				zip= "";							// where they live
				birthdate= "";
				year = 0;
		  }

		  ~PERSON_DATA()  {
#ifdef _DEBUG
				//const char *cptr = lastname.c_str();
#endif
				lastname = "";                                              // ******** CRASHES HERE IN LINUX ****************
				firstname= "";
				username= "";
				email= "";
				city= "";							// where they live
				state= "";							// where they live
				country= "";						// where they live
				zip= "";							// where they live
				birthdate= "";

				year = 0;

		  }
//} PERSON_DATA;
};
#endif

//#include <person_data.h>

#ifdef WIN32
//	#include <config.h>
	#ifdef DOC
		extern "C" {
	#endif
	class X_EXPORT Person  {
#else
class Person  {
#endif


	private:
		char xbdobstr[10];				// birthdate in xbase format

	public:
		//enum  {
		//	MALE,					// 0
		//	FEMALE				// 1
		//};

	protected:
		PERSON_DATA pd;						// struct 'needs to have dll-interface to be used by clients of class 'Person'
		char gstr[256];
		void convertDOB(void);
		char fullname[128];
		char middlename[32];

					 // moved to rd structure:
						  //char fullname[128];							// "Moore_Roger_W"
						  //float kgs;						// weight in kgs
						  //float lbs;
						  //char sex;						// 'M' = male, 'F' = female
						  //unsigned char years;			// age in years


		char regularname[128];						// "Roger W Moore"
		char dbkey[32];
		float cm;						// height in cm
		unsigned char bmo;			// birth month (1-12)
		unsigned char bday;			// birth day (1-31)
		short byear;					// birth year (eg, 2003)
		char phone[32];
		char address[64];
		//bool metric;
		void destroy(void);
		char dob[32];					// eg, 12/7/1941
		void init(void);
		char initials[8];
		char name2[16];
		void setInitials(void);
		void setName2(void);
		char mi[4];						// middle initial


		/*
	char lname[20];
	char fname[20];
	int age;
	char sex;
	char address[40];
	int fitness;
	int type;
	float ptime;								// predicted, for 10 k
	float pspeed;									// predicted, average, for 20 miles
		*/

//		char city[20];
//		char state[20];
//		char country[20];
//		char zip[12];
		//char phone[20];
//		char email[40];
		char econtact[40];
		char ephone[20];

	public:
		//Person(bool _metric=false);
		Person(void);
		virtual ~Person();

		void dump(char *_fname);
		virtual void my_export(FILE *stream, bool _metric);

		//--------------
		// getters:
		//--------------

		char *getFullName(void)  {						// used to construct a folder name that will sort by last name
			if (mi[0] != 0)  {
				sprintf(fullname, "%s_%s_%s", pd.lastname.c_str(), mi, pd.firstname.c_str());
			}
			else  {
				sprintf(fullname, "%s_%s", pd.lastname.c_str(), pd.firstname.c_str());
			}
			return fullname;
		}

		char *get_xb_dob(void)  {
			sprintf(xbdobstr, "%4d%02d%02d", byear, bmo, bday);
			return xbdobstr;
		}

		int getlnamesize(void)  {
			return sizeof(pd.lastname.c_str());
		}


		int getfnamesize(void)  {
			return sizeof(pd.firstname.c_str());
		}

		char *getDOB(void)  {
			return dob;
		}

		/*
		bool getMetric(void)  {
			return metric;
		}
		*/

		double getAge(void)  {
			return pd.age;
		}

		float getpounds(void)  {
			return (float) (KGSTOLBS*pd.kgs);
		}

		double getkgs(void)  {
			return pd.kgs;
		}

		const char *getlname(void)  {
			return pd.lastname.c_str();
		}
		const char *getfname(void)  {
			return pd.firstname.c_str();
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
			return pd.city.c_str();
		}

		const char *getstate(void)  {
			return pd.state.c_str();
		}

		const char *getcountry(void)  {
			return pd.country.c_str();
		}

		const char *getzip(void)  {
			return pd.zip.c_str();
		}

		const char *getemail(void)  {
			return pd.email.c_str();
		}

		char *getecontact(void)  {
			return econtact;
		}

		//char *getphone(void)  {
		//	return phone;
		//}

		char *getephone(void)  {
			return ephone;
		}

		char getsex(void)  {
			return pd.sex;
		}

		const char *getsexString(void)  {
			if (pd.sex=='M')  {
				return "Male";
			}
			else if (pd.sex=='F')  {
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
			//return initials;
			return name2;
		}

		char *getmi(void)  {				// returns the middle initial
			return mi;
		}

		char *getName(void)  {
			if (mi[0]==0 || middlename[0]==0)  {		// no middle name
				//if (firstname[0] && lastname[0])  {
				sprintf(regularname, "%s %s", pd.firstname.c_str(), pd.lastname.c_str());
				//}
			}
			else  {
				sprintf(regularname, "%s %s %s", pd.firstname.c_str(), mi, pd.lastname.c_str());
			}
			return regularname;
		}

		float getInches(void)  {
			return (float) (CMTOINCHES*cm);
		}

		const char *get_firstname(void)  {
			return pd.firstname.c_str();
		}

		const char *get_lastname(void)  {
			return pd.lastname.c_str();
		}

		//--------------
		// setters:
		//--------------

		void setDOB(const char *_dob)  {			// eg, 	// "07/04/1776"
			strncpy(dob, _dob, sizeof(dob)-1);
			convertDOB();						// maintain the date numerically
			return;
		}

		/*
		void setMetric(bool _metric)  {
			metric = _metric;
			return;
		}
		*/

		void setpounds(double _pounds)  {
			pd.kgs = TOKGS * _pounds;
			return;
		}

		void setkgs(double _kgs)  {
			pd.kgs = _kgs;
			return;
		}

		void setlname(char *_c)  {
			//strncpy(pd.lastname, _c, sizeof(pd.lastname)-1);
			pd.lastname = _c;
			return;
		}

		void setfname(char *_c)  {
			//strncpy(pd.firstname, _c, sizeof(pd.firstname)-1);
			pd.firstname = _c;
			return;
		}

		void setmname(char *_c)  {
			strncpy(middlename, _c, sizeof(middlename)-1);
			mi[0] = middlename[0];
			strupr(mi);
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
			/*
			if (phone)  {
				delete [] phone;
			}
			int n = strlen(_c)+1;
			phone = new char[n];
			strncpy(phone, _c, n);
			*/
			strncpy(phone, _c, sizeof(phone)-1);
			return;
		}

		void setaddress(char *_c)  {
			/*
			if (address)  {
				delete [] address;
			}
			int n = strlen(_c)+1;
			address = new char[n];
			strncpy(address, _c, n);
			*/
			strncpy(address, _c, sizeof(address)-1);
			return;
		}

		void setsex(char _c)  {
			pd.sex = _c;
			return;
		}

		/*
		char city[20];
		char state[20];
		char zip[12];
		char phone[20];
		char email[40];
		char econtact[40];
		char ephone[20];
		*/

		void setcity(char *_c)  {
			//strncpy(city, _c, sizeof(city)-1);
			pd.city = _c;
			return;
		}

		void setstate(char *_c)  {
			//strncpy(state, _c, sizeof(state)-1);
			pd.state = _c;
			return;
		}

		void setzip(char *_c)  {
			//strncpy(zip, _c, sizeof(zip)-1);
			pd.zip = _c;
			return;
		}
//		void setphone(char *_c)  {
//			strncpy(phone, _c, sizeof(phone)-1);
//			return;
//		}
		void setemail(char *_c)  {
			//strncpy(email, _c, sizeof(email)-1);
			pd.email = _c;
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

#ifdef WIN32
	#ifdef DOC
		}			// extern "C" {
	#endif
#endif

#endif		// #ifndef _PERSON_H_

