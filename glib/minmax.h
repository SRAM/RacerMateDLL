
#ifndef MINMAX__H
#define MINMAX__H

#ifndef WIN32
	template<class T> inline T min(T a, T b) {return (a < b) ? a : b; }
	template<class T> inline T max(T a, T b) {return (a > b) ? a : b; }

	template<class T> inline T MIN(T a, T b) {return (a < b) ? a : b; }
	template<class T> inline T MAX(T a, T b) {return (a > b) ? a : b; }

#endif
	


#endif	// MINMAX__H
