class STAT {
	public:
		double total;
		unsigned long count;
		double avg;
		double min;
		double max;

	public:
		STAT(void);
		~STAT();
		void reset(void);
		void update(double);
};

