
#ifndef _RIDER2_H_
#define _RIDER2_H_

#include <comdefs.h>
#include <person.h>

class  Rider2 : public Person  {

	 public:
		  float lower_hr;
		  float upper_hr;
		  int hr_beep_enabled;
		  int draft_enable;
		  float drag_aerodynamic;
		  float drag_tire;
		  int cal;												// unsigned short for computrainer
		  float watts_factor;
		  float ftp;
		  int metric;


		  Rider2(void)  {									// constructor
				pd.age = 25.0;
				pd.year = 0;
				pd.month = 0;
				pd.day = 0;
				pd.kgs = 180.0*TOKGS;
				cm = 0.0f;
				pd.sex = 'M';

				lower_hr = 60.0f;;
				upper_hr = 70.0f;
				draft_enable = false;
				drag_tire = .006f;
				cal = 0x0000;							// unsigned short
				watts_factor = 1.0f;
				ftp = 1.0f;
				drag_aerodynamic = 8.0f * watts_factor;
				hr_beep_enabled = false;
				metric = 0;
		  }

		  ~Rider2()  {					// destructor
				ftp = 0.0f;
		  }

		  /*
		  Rider(const Rider& copy)  {					// copy constructor
				year = copy.year;
				month = copy.month;
				day = copy.day;
				age = copy.age;
				kgs = copy.kgs;
				sex = copy.sex;
				cm = copy.cm;

				lastname = copy.lastname;
				firstname = copy.firstname;
				username = copy.username;
				email = copy.email;
				city = copy.city;							// where they live
				state = copy.state;							// where they live
				country = copy.country;						// where they live
				zip = copy.zip;							// where they live
				birthdate = copy.birthdate;
				memcpy(name, copy.name, sizeof(name));								// for display, eg 'DebbieT'

				lower_hr = copy.lower_hr;
				upper_hr = copy.upper_hr;
				hr_beep_enabled = copy.hr_beep_enabled;
				draft_enable = copy.draft_enable;
				drag_aerodynamic = copy.drag_aerodynamic;
				drag_tire = copy.drag_tire;
				cal = copy.cal;
				watts_factor = copy.watts_factor;
				ftp = copy.ftp;
				metric = copy.metric;

		  }
		  */
};

#endif		// #ifndef _RIDER_H_

