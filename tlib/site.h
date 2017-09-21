
#ifndef _SITE_H_
#define _SITE_H_

#include <string>

#if 1

class SITE  {
	public:
		SITE(void)  {								// constructor
			kgs = 0.0f;
			metric = 0;
			port = 0;
			rider_index = 0;
		}

		int port;
		float kgs;
		int metric;								// whether kgs is metric
		int rider_index;
		std::string name;							// site name
		std::string city;
		std::string state;
		std::string zip;
		std::string country;

		SITE(const SITE& copy)  {					// copy constructor
			name = copy.name.c_str();
			city = copy.city.c_str();
			state = copy.state.c_str();
			zip = copy.zip.c_str();
			country = copy.country.c_str();
			kgs = copy.kgs;
			metric = copy.metric;
			port = copy.port;
			rider_index = copy.rider_index;
		}

};


#else

struct SITE  {

	SITE(void)  {								// constructor
	}

	int port;
	float kgs;
	bool metric;								// whether kgs is metric
	std::string name;							// site name
	std::string city;
	std::string state;
	std::string country;
	std::string zip;

	SITE(const SITE& copy)  {					// copy constructor
		name = copy.name.c_str();
		city = copy.city.c_str();
		state = copy.state.c_str();
		state = copy.country.c_str();
		state = copy.zip.c_str();
		kgs = copy.kgs;
		metric = copy.metric;
		port = copy.metric;
	}

};
#endif

#endif		// #ifndef _SITE_H_

