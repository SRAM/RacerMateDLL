#include <windows.h>
#include <stdio.h>
#include <gl\glu.h>
#include <string.h>

#ifdef WIN32
	#pragma warning(disable:4996)					// for vista strncpy_s
#endif

#include <comutils.h>

#include <glib_defines.h>
#include <utils.h>
#include <fatalerror.h>

#include <splash.h>

/***************************************************************************************
	constructor 1
***************************************************************************************/

Splash::Splash(const char *_picname, unsigned long _timeout, HWND _hwnd) {
	MSG msg;

	using_caller_window = true;

	strncpy(picname, _picname, sizeof(picname)-1);
	timeout = _timeout;
	hwnd = _hwnd;
	fullscreen = false;
	x = 0;
	y = 0;
	w = 0;
	h = 0;

	init();


	while(1)  {
		if (timeout != 0L)  {
			dt = timeGetTime() - start;
			if (dt>=timeout)  {
				break;
			}
		}
		else  {
			if (dt<5000L)  {
				dt = timeGetTime() - start;
			}
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))  {
			if (msg.message==WM_QUIT)  {
				break;
			}
			else  {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else  {
			if (keys[VK_ESCAPE])  {
				keys[VK_ESCAPE] = false;
				break;
			}
			/*
			if (keys[VK_RIGHT])  {
				alpha += .01f;
				if (alpha>1.0f)  {
					alpha = 1.0f;
				}
			}
			if (keys[VK_LEFT])  {
				alpha -= .01f;
				if (alpha<0.0f)  {
					alpha = 0.0f;
				}
			}
			*/
			if (active)  {
				render();
			}
			SwapBuffers(hdc);
			Sleep(1);
		}
	}

	KillGLWindow();
	return;


}										// constructor


/***************************************************************************************
	constructor 2
***************************************************************************************/

Splash::Splash(const char *_picname, unsigned long _timeout, int _x, int _y, int _w, int _h, bool _fullscreen) {
	MSG msg;

	using_caller_window = false;

	strncpy(picname, _picname, sizeof(picname)-1);
	timeout = _timeout;
	fullscreen = _fullscreen;
	x = _x;
	y = _y;
	w = _w;
	h = _h;

	init();


	while(1)  {
		if (timeout != 0L)  {
			dt = timeGetTime() - start;
			if (dt>=timeout)  {
				break;
			}
		}
		else  {
			if (dt<5000L)  {
				dt = timeGetTime() - start;
			}
		}

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))  {
			if (msg.message==WM_QUIT)  {
				break;
			}
			else  {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else  {
			if (keys[VK_ESCAPE])  {
				keys[VK_ESCAPE] = false;
				break;
			}
			/*
			if (keys[VK_RIGHT])  {
				alpha += .01f;
				if (alpha>1.0f)  {
					alpha = 1.0f;
				}
			}
			if (keys[VK_LEFT])  {
				alpha -= .01f;
				if (alpha<0.0f)  {
					alpha = 0.0f;
				}
			}
			*/
			if (active)  {
				render();
			}
			SwapBuffers(hdc);
			Sleep(1);
		}
	}

	KillGLWindow();
	return;


}										// constructor

/***************************************************************************************

***************************************************************************************/

Splash::~Splash()  {

}

/***************************************************************************************

***************************************************************************************/

int Splash::init(void)  {

	inverty = true;
	parentproc = 0L;
	olddata = 0L;

	fadems = 2000;				// fade in in 2 seconds
	hdc=NULL;
	hRC=NULL;
	if (!using_caller_window)  {
		hwnd = NULL;
	}
	memset(keys, false, 256*sizeof(bool));
	active = true;
	dt = 0L;
	alpha = 0.0f;
	r = 1.0f;
	g = 1.0f;
	b = 1.0f;

	mode = ORTHOGONAL;

	//fullscreen = true;
	fov = 45.0f;

#ifdef WIN32
	//RECT war;	
	//BOOL B;
	//B = SystemParametersInfo(SPI_GETWORKAREA, 0,&war, 0);		// 1920 x 1028
	screenw = GetSystemMetrics(SM_CXSCREEN);		// 1920
	screenh = GetSystemMetrics(SM_CYSCREEN);		// 1080 (vs 1028 above)
#else
	int screenw = 1;		// 1280
	int screenh = 1;		// 1024
#endif

	ratio = (float)screenw / (float)screenh;			// not widescreen = 1.25
	if (ratio < 1.6)  {									// widescreen = 1.777777
		widescreen = false;
	}
	else  {
		widescreen = true;
	}

	//width = (int) (.9f*screenw);
	//height = (int) (.9f*screenh);

	//width = 512;
	//height = 512;

	if (using_caller_window)  {
		GetWindowRect(hwnd, &windowrect);
		GetClientRect(hwnd, &clientrect);
		width = clientrect.right;
		height = clientrect.bottom;
	}
	else  {
		memset(&clientrect, 0, sizeof(RECT));
		width = screenw;
		height = screenh;
	}

	if (!CreateGLWindow("", 16))	{
		return 1;
	}

	start = timeGetTime();
	//timeout = 6000L;
	/*
	unsigned long ul;
	if (timeout==0L)  {
		ul = 2000;
	}
	else  {
		ul = timeout;
	}
	*/

	map(0.0f, (float)fadems, 0.0f, 1.0f, &malpha, &balpha);
	//map(0.0f, (float)fadems, 1.0f, 0.0f, &malpha, &balpha);

	map(0.0f, (float)fadems, 0.0f, 1.0f, &mr, &br);
	map(0.0f, (float)fadems, 0.0f, 1.0f, &mg, &bg);
	map(0.0f, (float)fadems, 0.0f, 1.0f, &mb, &bb);
	//r = 1.0f;
	//g = 1.0f;
	//b = 1.0f;

	return 0;
}

/**************************************************************************************************

**************************************************************************************************/

LRESULT CALLBACK Splash::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)  {
	int bp;

	Splash *s = (Splash *)GetWindowLong (hwnd, GWL_USERDATA);

	switch (msg) {
		case WM_ACTIVATE:  {
			if (!HIWORD(wparam))  {
				if (s) s->active=true;
			}
			else  {
				if (s) s->active=false;
			}
			return 0;
		}

		case WM_SYSCOMMAND:  {
			switch (wparam)  {
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		case WM_CLOSE:  {
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:  {
			//int i = VK_ESCAPE;			// 0x1b
			if (s)  {
				s->keys[wparam] = true;
			}
			else  {
				bp = 2;
			}
			return 0;
		}

		case WM_KEYUP:  {
			if (s) s->keys[wparam] = false;
			return 0;
		}

		case WM_CHAR:  {
			//if (s) s->keys[wParam] = true;
			if (s) s->bp = 1;
			return 0;
		}

		case WM_SIZE:  {
			if (s) s->resize(LOWORD(lparam),HIWORD(lparam));
			return 0;
		}
	}

	//if (s)  {
	//	return CallWindowProc(s->oldproc, hwnd, msg, wparam, lparam);
	//}
	//else  {
	return DefWindowProc(hwnd, msg, wparam, lparam);
	//}
}


/**************************************************************************************************

**************************************************************************************************/

int Splash::LoadGLTexture(void)  {
	TGA *tga = new TGA(picname);
	int status = tga->load(&tex);
	DEL(tga);
	if (status)  {
		return 1;
	}

	/*
	glBindTexture (GL_TEXTURE_2D, textureImage.id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);					// GL_MODULATE, GL_DECAL, GL_REPLACE
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glTexImage2D(
			GL_TEXTURE_2D,								// target
			0,												// level
			4,												// int internalFormat, Specifies the number of color components in the texture. Must be 1, 2, 3, or 4, or one of the following symbolic constants
			textureImage.width,
			textureImage.height,
			0,												// border
			GL_RGBA,										// format, Specifies the format of the pixel data. The following symbolic values are accepted
			GL_UNSIGNED_BYTE,							// type
			tga->get_data()								// GLvoid * data
	);

*/
	glGenTextures(1, &tex.id);
	glBindTexture(GL_TEXTURE_2D, tex.id);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);					// GL_MODULATE, GL_DECAL, GL_REPLACE

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		tex.bpp / 8,								// 3
		tex.width,
		tex.height,
		0,
		tex.type,									// GL_RGB = 1907, GL_RGBA = 1908
		GL_UNSIGNED_BYTE,
		tex.imageData
		);

	/*
	glTexImage2D(
			GL_TEXTURE_2D,								// target
			0,												// level
			4,												// int internalFormat, Specifies the number of color components in the texture. Must be 1, 2, 3, or 4, or one of the following symbolic constants
			tex.width,
			tex.height,
			0,												// border
			GL_RGBA,										// format, Specifies the format of the pixel data. The following symbolic values are accepted
			GL_UNSIGNED_BYTE,							// type
			//tga->get_data()								// GLvoid * data
			tex.imageData
	);
	*/

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if (tex.imageData)  {
		free(tex.imageData);
	}

	return 0;
}								// int Splash::LoadGLTextures(void)  {

/**************************************************************************************************
	called from wm_size()
**************************************************************************************************/

GLvoid Splash::resize(GLsizei _w, GLsizei _h)  {

	width = _w;
	height = _h;

   if (height == 0)  {								// Prevent A Divide By Zero If The Window Is Too Small
       height = 1;
	}

	aspect = (GLfloat)_w / (GLfloat)_h;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//-----------------------------------------------
	// negative z goes into the screen
	// positive z is toward YOU
	// Note that near and far can be confusing - they are the negatives of the Z values for the corresponding planes.
	//-----------------------------------------------

	if (mode==PERSPECTIVE)  {
		gluPerspective(fov, aspect, 0.1f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else  {
		glOrtho(
				0.0f,						// left
				(GLfloat)(width-1),			// right
				0.0f,						// bottom
				(GLfloat)(height-1),		// top
				0.0f,						// znear, distance FROM the camera, z = -1.0f
				1000.0f						// zfar, distance FROM the camera, z = -1000.0f
		);
		if (inverty)  {
			glScalef(1, -1, 1);										// invert Y axis so increasing Y goes down.
			glTranslatef(0, -(float)(height-1), 0);			// shift origin up to upper-left corner.
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}



	// the camera is at 0. The near plane is at -.1 and the far plane is at -1,000

	GetClientRect(hwnd, &clientrect);
	GetWindowRect(hwnd, &windowrect);

	return;
}													// resize()

/**************************************************************************************************

**************************************************************************************************/

int Splash::InitGL(GLvoid)  {

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtex);						// 16384
	resize(width, height);												// 1350 x 914

	glShadeModel(GL_SMOOTH);

	if (mode==PERSPECTIVE)  {
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	}
	else  {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}


	if (mode==ORTHOGONAL)  {
		glDisable(GL_DEPTH_TEST);
	}
	else  {
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	resize(width, height);

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtex);						// 16384

	glFlush();

	if (LoadGLTexture())  {
		return 0;
	}

	return 1;													// Initialization Went OK
}																// InitGL()

/**************************************************************************************************

**************************************************************************************************/

int Splash::render(GLvoid)  {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (mode==PERSPECTIVE)  {
		glTranslatef(0.0f, 0.0f, -2.0f);
		glScalef(1.6f, 1.0f, 1.0f);
	}

	glEnable(GL_BLEND);

	alpha = malpha*dt + balpha;
	/*
	r = mr*dt + br;
	g = mg*dt + bg;
	b = mb*dt + bb;
	*/

	//glColor4f(r, g, b, alpha);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, alpha);


#if 1
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);					// GL_MODULATE, GL_DECAL, GL_REPLACE
	glBindTexture(GL_TEXTURE_2D, tex.id);

	float z = 0.0f;

	glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f ((float)clientrect.left, (float)clientrect.bottom, z);

		glTexCoord2f (1.0, 0.0);
		glVertex3f ((float)clientrect.right, (float)clientrect.bottom, z);

		glTexCoord2f (1.0, 1.0);
		glVertex3f ((float)clientrect.right, (float)clientrect.top, z);

		glTexCoord2f (0.0, 1.0);
		glVertex3f ((float)clientrect.left, (float)clientrect.top, z);
	glEnd();

#endif

	//check();

#ifdef DRAW_TEST_PATTERN

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glLineWidth(2.0f);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glBegin(GL_LINES);
		glVertex3f((float)(width-1), 0.0f, z);
		glVertex3f(0.0f, (float)(height-1), z);

		glVertex3f(0.0f, 0.0f, z);
		glVertex3f((float)(width-1), (float)(height-1), z);
	glEnd();


	glLineWidth(1.0f);

	glBegin(GL_LINE_STRIP);
		glVertex3f(0.0f, 0.0f, z);
		glVertex3f((float)(width-1), 0.0f, z);
		glVertex3f((float)(width-1), (float)(height-1), z);
		glVertex3f(0.0f, (float)(height-1), z);
		glVertex3f(0.0f, 0.0f, z);
	glEnd();
#endif

	return 1;
}										// render()

/**************************************************************************************************

**************************************************************************************************/

GLvoid Splash::KillGLWindow(GLvoid)  {

	SetWindowLong(hwnd, GWL_USERDATA, olddata);

	if (using_caller_window)  {
		SetWindowLong (hwnd, GWL_WNDPROC, parentproc);
	}

	if (fullscreen)  {
		ChangeDisplaySettings(NULL,0);
		ShowCursor(true);
	}

	if (hRC)  {
		if (!wglMakeCurrent(NULL,NULL))  {
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))  {
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;
	}

	if (hdc && !ReleaseDC(hwnd,hdc))  {
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hdc=NULL;
	}

	if (!using_caller_window)  {
		if (hwnd && !DestroyWindow(hwnd))  {
			MessageBox(NULL,"Could Not Release hwnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hwnd=NULL;
		}

		if (!UnregisterClass("splash",hInstance))  {
			MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hInstance=NULL;
		}
	}
}

/**************************************************************************************************

**************************************************************************************************/

bool Splash::CreateGLWindow(char* title, int bits)  {
	GLuint PixelFormat;

	if (!using_caller_window)  {
		WNDCLASS wc;
		DWORD dwExStyle;
		DWORD dwStyle;

		/*
		winrect.left = 0;
		winrect.right = width;
		winrect.top = 0;
		winrect.bottom = height;
		*/
		winrect.left = x;
		winrect.right = x+w;
		winrect.top = y;
		winrect.bottom = y+h;

		hInstance = GetModuleHandle(NULL);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC) wndproc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "splash";

		if (!RegisterClass(&wc))  {
			MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
	
		if (fullscreen)  {
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth	= width;
			dmScreenSettings.dmPelsHeight	= height;
			dmScreenSettings.dmBitsPerPel	= bits;
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)  {
				if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","xxxGL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)  {
					fullscreen=false;
				}
				else  {
					MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
					return false;
				}
			}
			dwExStyle=WS_EX_APPWINDOW;
			dwStyle=WS_POPUP;
			ShowCursor(false);
		}
		else  {
			dwExStyle = 0;
			dwStyle = 0;
		}

		AdjustWindowRectEx(&winrect, dwStyle, false, dwExStyle);

		if (!(hwnd=CreateWindowEx(
									dwExStyle,
									"splash",
									title,
									dwStyle,
									0, 0,
									winrect.right-winrect.left,
									winrect.bottom-winrect.top,
									NULL,
									NULL,
									hInstance,
									NULL)))
		{
			KillGLWindow();
			MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return false;
		}

		unsigned long oldstyle = SetWindowLong(hwnd, GWL_STYLE, 0);		//remove all window styles
	}
	else  {										// use the creator's window
		parentproc = GetWindowLong(hwnd, GWL_WNDPROC);
		#ifdef _DEBUG
			//WNDPROC oldproc = (WNDPROC) SetWindowLong(hwnd, GWL_WNDPROC, (LPARAM)wndproc);		// subclass to intercept messages
		#endif
	}

	olddata = SetWindowLong(hwnd, GWL_USERDATA, (long)this);

	static	PIXELFORMATDESCRIPTOR pfd =	{
		sizeof(PIXELFORMATDESCRIPTOR),							// Size Of This Pixel Format Descriptor
		1,														// Version Number
		PFD_DRAW_TO_WINDOW |									// Format Must Support Window
		PFD_SUPPORT_OPENGL |									// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,										// Must Support Double Buffering
		(BYTE)PFD_TYPE_RGBA,											// Request An RGBA Format
		(BYTE)bits,													// Select Our Color Depth
		(BYTE)0, (BYTE)0, (BYTE)0, (BYTE)0, (BYTE)0, (BYTE)0,										// Color Bits Ignored
		(BYTE)0,														// No Alpha Buffer
		(BYTE)0,														// Shift Bit Ignored
		(BYTE)0,														// No Accumulation Buffer
		(BYTE)0,(BYTE)0, (BYTE)0, (BYTE)0,												// Accumulation Bits Ignored
		(BYTE)16,														// 16Bit Z-Buffer (Depth Buffer)  
		(BYTE)0,														// No Stencil Buffer
		(BYTE)0,														// No Auxiliary Buffer
		(BYTE)PFD_MAIN_PLANE,											// Main Drawing Layer
		(BYTE)0,														// Reserved
		0, 0, 0													// Layer Masks Ignored
	};
	
	if (!(hdc=GetDC(hwnd)))  {
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if (!(PixelFormat=ChoosePixelFormat(hdc,&pfd)))  {
		KillGLWindow();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if(!SetPixelFormat(hdc,PixelFormat,&pfd))  {
		KillGLWindow();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if (!(hRC=wglCreateContext(hdc)))  {
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if(!wglMakeCurrent(hdc,hRC))  {
		KillGLWindow();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	ShowWindow(hwnd,SW_SHOW);
	SetForegroundWindow(hwnd);
	center_window(hwnd);

	SetFocus(hwnd);
	//resize(width, height);
	GetWindowRect(hwnd, &windowrect);

	if (!InitGL())  {
		KillGLWindow();
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}						// createglwindow()

#ifdef _DEBUG
/********************************************************************************************

********************************************************************************************/

void Splash::check(void)  {
	GLenum gle;
	const GLubyte *cptr;
	//cptr = gluErrorString(gle);
	int cnt = 0;

	//if (!initialized)  {
	//	return;
	//}

	gle=glGetError();
	if (gle == GL_NO_ERROR)  {
		return;
	}

	bp = 0;


	while ( (gle=glGetError()) != GL_NO_ERROR)  {
		cnt++;
		if (cnt>1000)  {
			throw (fatalError(__FILE__, __LINE__, "check() count = 1000"));
		}

		cptr = gluErrorString(gle);


		switch(gle)  {
			case GL_INVALID_ENUM:		// An unacceptable value is specified for an enumerated argument. The offending function is ignored, having no side effect other than to set the error flag.
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				printf("GL_INVALID_ENUM\n");
				break;

			case GL_INVALID_VALUE:		// A numeric argument is out of range. The offending function is ignored, having no side effect other than to set the error flag.
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				printf("GL_INVALID_VALUE\n");
				break;

			case GL_INVALID_OPERATION:	// The specified operation is not allowed in the current state. The offending function is ignored, having no side effect other than to set the error flag.
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				//printf("GL_INVALID_OPERATION\n");
				break;

			case GL_STACK_OVERFLOW:		// This function would cause a stack overflow. The offending function is ignored, having no side effect other than to set the error flag.
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				printf("GL_STACK_OVERFLOW\n");
				break;

			case GL_STACK_UNDERFLOW:	// This function would cause a stack underflow. The offending function is ignored, having no side effect other than to set the error flag.
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				printf("GL_STACK_UNDERFLOW\n");
				break;

			case GL_OUT_OF_MEMORY:
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				printf("GL_OUT_OF_MEMORY\n");
				break;

			default:
				bp = 1;
				//throw (fatalError(__FILE__, __LINE__));
				printf("check() default error\n");
				break;
		}
	}				// while()

	return;
}					// check();
#endif

