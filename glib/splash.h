#ifndef SPLASH_IS_INCLUDED
#define SPLASH_IS_INCLUDED

#include <gl\gl.h>
#define SMETH 3

#ifdef WIN32
	#include <windows.h>				// for MAX_PATH
#else
	#include <defines.h>				// for MAX_PATH
#endif

#include <tga.h>

/***************************************************************************************

***************************************************************************************/

class Splash  {

	private:
		//#define DRAW_TEST_PATTERN			// to test the projection matrix
		int x,y,w,h;
		char picname[MAX_PATH];
		int init(void);
		LONG parentproc;
		unsigned long olddata;
		static LRESULT CALLBACK wndproc(HWND, UINT, WPARAM, LPARAM);
#ifdef _DEBUG
		void check(void);
#endif
		HDC hdc;
		HGLRC hRC;
		bool using_caller_window;
		HWND hwnd;
		HINSTANCE hInstance;
		bool keys[256];
		bool active;
		bool fullscreen;
		TGA::Texture tex;
		bool CreateGLWindow(char* title, int bits);
		int render(GLvoid);
		GLvoid KillGLWindow(GLvoid);
		GLvoid resize(GLsizei width, GLsizei height);
		int LoadGLTexture(void);
		int loadTGATexture(int _texid, const char *filename);
		int InitGL(GLvoid);
		int screenw, screenh;				// screen
		int width, height;					// window
		float ratio;
		bool widescreen;
		bool inverty;
		float aspect;
		int bp;
		int maxtex;
		RECT winrect;
		RECT clientrect;
		RECT windowrect;
		float fov;
		float alpha;

		enum PROJECTION_MODE {
			PERSPECTIVE,
			ORTHOGONAL
		};

		int mode;
		float malpha, balpha;
		float mr, br;
		float mg, bg;
		float mb, bb;
		unsigned long start;
		unsigned long timeout;
		unsigned long dt;
		float r, g, b;
		unsigned long fadems;				// the number of milliseconds that it takes to fade in

	public:
		Splash(const char *_jpg, unsigned long _timeout, int _x, int _y, int _w, int _h, bool _fullscreen);				// 0 is infinite
		Splash(const char *_jpg, unsigned long _timeout, HWND _hwnd);
		~Splash();
};

#endif
