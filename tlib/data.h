#ifndef _DATA_H_
#define _DATA_H_

class DATA  {
	public:
		DATA(void);
		float mph;
		float watts;
		float rpm;
		float lata;
		float rata;
		float miles;
		float ss;
		float lss;
		float rss;
		float lwatts;
		float rwatts;
		float avgss;
		float avglss;
		float avgrss;
		float hr;
		float cals;

		unsigned short hrflags;

		void reset(void);

	private:
		void init(void);

};

#endif


