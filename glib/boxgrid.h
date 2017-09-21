
#ifndef _BOXGRID_H_
#define _BOXGRID_H_

#ifdef WIN32
	#include <Windows.h>
#endif

#include <gen_types.h>

#ifdef WIN32
	#ifdef DOC
		extern "C" {
	#endif
#endif

#ifdef WIN32
class __declspec(dllexport) BoxGrid  {
#else
class BoxGrid  {
#endif


	public:

		enum BOXGRIDSTATUS {
			BOXGRID_OK = 0,
			BOXGRID_TOO_WIDE = INT_MIN,			// 0x80000000
			BOXGRID_TOO_HIGH,					// 0x80000001
			BOXGRID_OTHER_ERROR
		};


		typedef struct  {
			float top;
			float bot;
		} ROW;

		typedef struct  {
			float left;
			float right;
		} COL;

		//BoxGrid(void);
		BoxGrid(
				int _nrows, 
				int _ncols, 

				// top/bottom:
				float _ph, 
				float _ptopmargin, 
				float _pbottommargin, 
				float _pvpadding,

				// left/right:
				float _pw, 
				float _pleftmargin, 
				float _prightmargin, 
				float _phpadding
				);
		virtual ~BoxGrid(void);
		void resize(FRECT *_win);
		inline int get_initialized(void)  { return initialized; }
		inline BOXGRIDSTATUS get_err(void)  { return err; }
		static float compute_h_padding(int NCOLS, float pleftmargin, float prightmargin, float &pw);

		ROW *rows;
		COL *cols;

	private:
		BoxGrid(const BoxGrid&);
		BoxGrid &operator = (const BoxGrid&);		// unimplemented

		//int err;
		BOXGRIDSTATUS err;
		int bp;
		bool initialized;

		float mx, bx;
		float my, by;

		FRECT win;
		float winw;							// overall window width
		float winh;							// overall window height

		int nrows;
		int ncols;

		// top/bottom:
		float ptopmargin;
		float pbottommargin;
		float pvpadding;					// vertical padding

		float pleftmargin;
		float prightmargin;
		float phpadding;					// horizontal padding

		float ph;							// box height (percentage)
		float pw;							// box width (percentage)

		// real coordinates, not percentages:

		// top/bottom:
		float vpadding;
		float topmargin;
		float bottommargin;

		// left/right:
		float hpadding;
		float leftmargin;
		float rightmargin;

		float boxw;
		float boxh;
};

#ifdef WIN32
	#ifdef DOC
		}							// extern "C" {
	#endif
#endif

#endif		// #ifndef _X_H_
