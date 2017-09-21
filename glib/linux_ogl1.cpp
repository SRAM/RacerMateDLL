
#include <string.h>
#include <sys/time.h>

#include <ogl.h>			// KEEP THIS BEFORE ANY INCLUDE FILE THAT INCLUDES DEFINES.H OR YOU GET A COMPILE ERROR FROM XMD.H !!!!!!!!!
#include <utils.h>
// attributes for a single buffered visual in RGBA format with at least
// 4 bits per color and a 16 bit depth buffer


#ifdef DO_ALARM
	//int OGL::alarm_sig = 0;
#endif

int OGL::attrListSgl[] = {
	GLX_RGBA,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};

// attributes for a double buffered visual in RGBA format with at least
// 4 bits per color and a 16 bit depth buffer

int OGL::attrListDbl[] = { 
	GLX_RGBA,
	GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};

/**********************************************************************
  constructor 1

 **********************************************************************/

OGL::OGL(
		bool _fullscreen,
		int _mode,
		const char *_title,
		bool _animated,
		void (*_runfunc)(void *),
		int (*_keyboardfunc)(void *, unsigned short c),
		void (*_mousefunc)(void *, int _mousex, int _mousey),
		void (*_resizefunc)(void *, unsigned int _w, unsigned int _h),
		void *_obj,
		bool _inverty,
		const char *_icon
)	{
	int status;

	/*
	fullscreen = _fullscreen;
	mode = _mode;
	animated = _animated;
	runfunc = _runfunc;
	keyboardfunc = _keyboardfunc;
	mousefunc = _mousefunc;
	obj = _obj;
	inverty = _inverty;;
	strncpy(title, _title, sizeof(title)-1);
	*/

	//	memset(fonts, 0, MAXFONTS*sizeof(FONT));


	runfunc = NULL;
	keyboardfunc = NULL;
	mousefunc = NULL;
	resizefunc = NULL;
	obj =  NULL;


	// linux specific inits:

#ifdef THROTTLE
	x11_fd = -1;
	memset(&tv, 0, sizeof(tv));
	memset(&stv, 0, sizeof(stv));
	memset(&tv_period, 0, sizeof(tv_period));
#endif


#ifdef _DEBUG
	ft = new fastTimer("oglft");
#endif


	// cross-platform inits:

	status = init();
	if (status != 0)  {
		return;
	}

	// set the input parameters:

	fullscreen = _fullscreen;
	mode = _mode;
	animated = _animated;
	runfunc = _runfunc;
	keyboardfunc = _keyboardfunc;
	mousefunc = _mousefunc;
	resizefunc = _resizefunc;
	obj = _obj;
	inverty = _inverty;;
	strncpy(title, _title, sizeof(title)-1);
	initialized = true;

	return;
}

/**********************************************************************
  constructor 2
 **********************************************************************/

OGL::OGL(
		int _w,
		int _h,
		int _bits,
		bool _fullscreen,
		int _mode,
		const char *_title,
		bool _animated,
		bool _inverty
		)	{
	int status;

	width = _w;
	height = _h;
	bits = _bits;
	fullscreen = _fullscreen;
	mode = _mode;
	strncpy(title, _title, sizeof(title)-1);
	animated = _animated;
	inverty = _inverty;

	runfunc = NULL;
	keyboardfunc = NULL;
	mousefunc = NULL;
#ifdef _DEBUG
	ft = new fastTimer("oglft");
#endif


	status = init();
	if (status != 0)  {
		return;
	}

	return;
}

/**********************************************************************
  constructor 3
 **********************************************************************/

OGL::OGL(
		int _mode,
		const char *_title,
		bool _animated,
		bool _inverty
		)	{
	int status;

	mode = _mode;
	strncpy(title, _title, sizeof(title)-1);
	animated = _animated;
	inverty = _inverty;

	width = 1200;
	height = 800;
	bits = 24;
	fullscreen = false;

	runfunc = NULL;
	keyboardfunc = NULL;
	mousefunc = NULL;
#ifdef _DEBUG
	ft = new fastTimer("oglft");
#endif


	status = init();
	if (status != 0)  {
		return;
	}

	return;
}

/**********************************************************************

 **********************************************************************/

OGL::~OGL()  {
	destroy();
	int i;
	DEL(movie);

	for(i=0; i<MAX_FONTS; i++)  {
		DEL(fonts[i]);
	}
	
	
	/*
	for(i=1; i<(int)fonts.size(); i++)  {					// skipping fonts[0] since it is the default font destroyed by the parent class
		XFreeFont(disp, fonts[i].font);
		fonts[i].font = 0;
		glDeleteLists(fonts[i].base, 96);
	}
	fonts.clear();
	*/

}

#ifdef DO_ALARM
/**********************************************************************

**********************************************************************/

void OGL::sig_alarm(int _sig) {
	//printf("sig_alarm\n");
	#ifdef _DEBUG
	//	OGL::at->update();				// .9999 ms
	#endif
	//alarm_sig = _sig;
	return;
}
#endif


/**********************************************************************
  this is the main loop

  to run a timer:
http://www.linuxquestions.org/questions/programming-9/xnextevent-select-409355/

 **********************************************************************/

void OGL::run(void)  {


#ifdef THROTTLE
	int nready;

	gettimeofday(&stv, 0);
	timeradd(&stv, &tv_period, &stv);

	tv.tv_sec = tv_period.tv_sec;
	tv.tv_usec = tv_period.tv_usec;

#ifdef DO_ALARM
#if 0
	ualarm( 2000000, 10000);				// once per second starting in 1 second
	signal(SIGALRM, sig_alarm);
#else
	struct itimerval tout_val;
	tout_val.it_interval.tv_sec = 0;


	#ifdef DOTHREAD
		tout_val.it_interval.tv_usec = ROUND(1000.0f*1000.0f / 30.0f);					// set to 34 frames per second
	#else
		tout_val.it_interval.tv_usec = 1000;
	#endif

	tout_val.it_value.tv_sec = 0;					// set timer for "INTERVAL seconds
	tout_val.it_value.tv_usec = 100000;
	signal(SIGALRM, sig_alarm);					// set the Alarm signal capture
	setitimer(ITIMER_REAL, &tout_val, 0);
#endif
#endif


	// wait for events

	while (!done)  {

		if (!freerun)  {
			FD_ZERO(&in_fds);
			FD_SET(x11_fd, &in_fds);			// Create a File Description Set containing x11_fd

			// Wait for X Event or a Timer
			//t2 = timeGetTime();
			//tdiff = t2 - t1;
			//tv.tv_usec = 1000 * tdiff;						// tv_period.tv_usec;

			//nready = select(x11_fd+1, &in_fds, 0, 0, &tv);

#ifdef DO_ALARM
			nready = select(x11_fd+1, &in_fds, 0, 0, 0);
#else
			nready = select(x11_fd+1, &in_fds, 0, 0, &tv);
#endif

			switch(nready)  {
				case 0:										// timeout
					//t1 = timeGetTime();
					tv.tv_usec = 899;						// tv_period.tv_usec;
					//printf("nready = 0\n");
					//gettimeofday(&stv, 0);
					//timeradd(&stv, &tv_period, &stv);		// set stv = to now + tv_period
					#ifdef _DEBUG
						// xxx
						//at->update();								// .9999 ms
					#endif
					break;
				case -1:													// error
					//bp = errno;
					//perror("select()");							// "select(): Interrupted system call"
					bp = 2;
					break;
				default:													// > 0
					//gettimeofday(&tv, 0);
					//timersub(&stv, &tv, &tv);			// set tv = remaining time.
					tv.tv_usec = 899;						// 1 ms, tv_period.tv_usec;
					break;
			}
		}
		else  {
			bp = 1;					// 'f' key toggles this on/off
		}

#ifdef DO_ALARM
//		if (alarm_sig)  {
//			alarm_sig = 0;
			#ifdef _DEBUG
			//at->update();				// 1.42 ms
			#endif
//		}
#endif

#ifdef _DEBUG
		//at->update();								// freerun: 2.85 ms = 351 fps, throttled: 32.53 ms
#endif

		// Handle XEvents in the queue and flush the input

		while (XPending(disp) > 0)  {
			XNextEvent(disp, &event);

			switch (event.type)  {
				case Expose:										// only called when window is dirty
					if (event.xexpose.count != 0)  {
						break;
					}
					//#ifdef _DEBUG
					printf("Expose calling runfunc\n");
					//#endif
					//runfunc(obj);
					break;

				case ConfigureNotify:					// call resizeGLScene only if our window-size changed
					if ((event.xconfigure.width != width) || (event.xconfigure.height != height))  {
#ifdef _DEBUG
						static int resizecounts = 0;
						resizecounts++;
#endif
							
						width = event.xconfigure.width;
						height = event.xconfigure.height;
						printf("Resize event, w = %d, h = %d\n", width, height);
//#ifdef _DEBUG
#if 0
	if (event.xconfigure.height==1005)  {
		if (event.xconfigure.width==1920)  {
			bp = 7;
		}
	}
#endif
						get_x_geometry();
						resize(event.xconfigure.width, event.xconfigure.height);
					}
					break;

				case ButtonPress:						// exit in case of a mouse button press
					//done = True;
					break;

				case KeyPress:  {
										 //KeySym ks;
										 char c;
										 int status;
										 c = (char)XLookupKeysym(&event.xkey, 0);
										 status = keyboardfunc(obj, c);											// send the keystroke to our creator
										 if (status ==1)  {
											 done = True;
										 }
										 /*
										 //if (XLookupKeysym(&event.xkey, 0) == XK_Escape)  {
										 if (c == XK_Escape)  {
										 done = True;
										 }
										 //if (XLookupKeysym(&event.xkey,0) == XK_F1)  {
										 if (c == XK_F1)  {
										 killGLWindow();
										 fullscreen = !fullscreen;
										 create_window(640, 480, 24, fullscreen);
										 }
										  */
										 break;
				}

				case ClientMessage:
					 if (*XGetAtomName(disp, event.xclient.message_type) == *"WM_PROTOCOLS")  {
						 printf("Exiting sanely...\n");
						 done = True;
					 }
#ifdef _DEBUG
					 else  {
						 bp = 2;
					 }
#endif
					 break;
				default:
					break;

				case MotionNotify:  {
																		// store the mouse button coordinates in 'int' variables.
																		// also store the ID of the window on which the mouse was pressed
																		//x = event.xmotion.x;
																		//y = event.xmotion.y;
																		//the_win = an_event.xbutton.window;
																		if (mousefunc)  {
																			mousefunc(obj, event.xmotion.x, event.xmotion.y);
																		}
																		// if the 1st mouse button was held during this event, draw a pixel at the mouse pointer location.
																		//if (event.xmotion.state & Button1Mask) {
																		// draw a pixel at the mouse position.
																		//XDrawPoint(display, the_win, gc_draw, x, y);
																		//}
																		break;
				}
			}								// switch
		}									// while (XPending(disp) > 0)  {

#ifdef DO_ALARM
		if (nready==-1)  {
			#ifdef _DEBUG
			//at->update();				//  29.4 ms
			#endif
			runfunc(obj);				// renders only
		}
		else  {
			runfunc(obj);				// renders only
		}
#else
		if (nready==0)  {
			#ifdef _DEBUG
			//at->update();				//  1.000 ms
			#endif
			runfunc(obj);
		}
		else  {
			runfunc(obj);
		}
#endif

	}							// while (!done)

#else
			// wait for events

			while (!done)  {
#ifdef _DEBUG
				//at->update();								// 5.8 ms = 172 fps
#endif

				// handle the events in the queue
				while (XPending(disp) > 0)  {
					XNextEvent(disp, &event);

					switch (event.type)  {
						case Expose:										// only called when window is dirty
							if (event.xexpose.count != 0)  {
								break;
							}
							//#ifdef _DEBUG
							printf("Expose calling runfunc\n");
							//#endif
							//runfunc(obj);
							break;

						case ConfigureNotify:					// call resizeGLScene only if our window-size changed
							if ((event.xconfigure.width != width) || (event.xconfigure.height != height))  {
								width = event.xconfigure.width;
								height = event.xconfigure.height;
								printf("Resize event, w = %d, h = %d\n", width, height);
								resize(event.xconfigure.width, event.xconfigure.height);
							}
							break;

						case ButtonPress:						// exit in case of a mouse button press
							done = True;
							break;

						case KeyPress:  {
												 //KeySym ks;
												 char c;
												 int status;
												 c = (char)XLookupKeysym(&event.xkey, 0);
												 status = keyboardfunc(obj, c);											// send the keystroke to our creator
												 if (status ==1)  {
													 done = True;
												 }

												 /*
												 //if (XLookupKeysym(&event.xkey, 0) == XK_Escape)  {
												 if (c == XK_Escape)  {
												 done = True;
												 }
												 //if (XLookupKeysym(&event.xkey,0) == XK_F1)  {
												 if (c == XK_F1)  {
												 killGLWindow();
												 fullscreen = !fullscreen;
												 create_window(640, 480, 24, fullscreen);
												 }
												  */
												 break;
											 }

												 case ClientMessage:
											 if (*XGetAtomName(disp, event.xclient.message_type) == *"WM_PROTOCOLS")  {
												 printf("Exiting sanely...\n");
												 done = True;
											 }
#ifdef _DEBUG
											 else  {
												 bp = 2;
											 }
#endif
											 break;

												 default:
											 break;


												 case MotionNotify:  {
																				// store the mouse button coordinates in 'int' variables.
																				// also store the ID of the window on which the mouse was pressed
																				//x = event.xmotion.x;
																				//y = event.xmotion.y;
																				//the_win = an_event.xbutton.window;
																				if (mousefunc)  {
																					mousefunc(obj, event.xmotion.x, event.xmotion.y);
																				}
																				// if the 1st mouse button was held during this event, draw a pixel at the mouse pointer location.
																				//if (event.xmotion.state & Button1Mask) {
																				// draw a pixel at the mouse position.
																				//XDrawPoint(display, the_win, gc_draw, x, y);
																				//}
																				break;
																			}

											 }
											 }
											 if (animated)  {
												 runfunc(obj);
											 }
					}				// while (!done)
#endif


					killGLWindow();
					printf("ogl returning from run()\n");
					return;
				}					// run()

				/************************************************************************************
				  function to release/destroy our resources and restoring the old desktop
				 ************************************************************************************/

				GLvoid OGL::killGLWindow(void)  {

					printf(".....killGLWindow() start\n");

					if (ctx)  {
						if (!glXMakeCurrent(disp, None, NULL))		{
							printf("Could not release drawing context.\n");
						}
						glXDestroyContext(disp, ctx);
						ctx = NULL;
					}
					/* switch back to original desktop resolution if we were in fs */
					if (fullscreen)  {
						XF86VidModeSwitchToMode(disp, screen, &deskMode);
						XF86VidModeSetViewPort(disp, screen, 0, 0);
					}
					XCloseDisplay(disp);
					printf(".....killGLWindow() end\n");

					return;
				}				// killGLWindow()



				/**********************************************************************

				 **********************************************************************/

				int OGL::create_window(int _width, int _height, int _bits, bool _fullscreen)  {
					int i;

					//printf("create_window: _width = %d, _height = %d\n", _width, _height);

					// set best mode to current
					bestMode = 0;
					// get a connection
					disp = XOpenDisplay(0);
					screen = DefaultScreen(disp);
					XF86VidModeQueryVersion(disp, &vidModeMajorVersion, &vidModeMinorVersion);
					printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion, vidModeMinorVersion);
					XF86VidModeGetAllModeLines(disp, screen, &modeNum, &modes);
					// save desktop-resolution before switching modes
					deskMode = *modes[0];

					// force to maximized window

					width = deskMode.hdisplay;
					height = deskMode.vdisplay;
					windowrect.left = 0;
					windowrect.top = 0;
					windowrect.right = width;
					windowrect.bottom = height;
					//printf("create_window: width = %d, height = %d\n", width, height);


					bp = 1;

					// look for mode with requested resolution

					for (i = 0; i < modeNum; i++)  {
						if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))  {
							bestMode = i;
						}
					}

					// get an appropriate visual

					visual_info = glXChooseVisual(disp, screen, attrListDbl);

					if (visual_info == NULL)  {
						visual_info = glXChooseVisual(disp, screen, attrListSgl);
						doubleBuffered = False;
						printf("Only Singlebuffered Visual!\n");
					}
					else  {
						doubleBuffered = True;
						printf("Got Doublebuffered Visual!\n");
					}

					glXQueryVersion(disp, &glxMajorVersion, &glxMinorVersion);
					printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);

					// create a GLX context

					ctx = glXCreateContext(disp, visual_info, 0, GL_TRUE);
					// create a color map

					cmap = XCreateColormap(disp, RootWindow(disp, visual_info->screen), visual_info->visual, AllocNone);
					attr.colormap = cmap;
					attr.border_pixel = 0;
					//attr.background_pixel = 0xff0000;                       // rgb

					if (fullscreen)  {
						XF86VidModeSwitchToMode(disp, screen, modes[bestMode]);
						XF86VidModeSetViewPort(disp, screen, 0, 0);
						displayWidth = modes[bestMode]->hdisplay;
						displayHeight = modes[bestMode]->vdisplay;
						printf("Resolution %dx%d\n", displayWidth, displayHeight);
						XFree(modes);

						// create a fullscreen window

						attr.override_redirect = True;
						attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask;

						win = XCreateWindow(disp, RootWindow(disp, visual_info->screen), 0, 0, displayWidth, displayHeight, 0, visual_info->depth, InputOutput, visual_info->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &attr);

						XWarpPointer(disp, None, win, 0, 0, 0, 0, 0, 0);
						XMapRaised(disp, win);
						XGrabKeyboard(disp, win, True, GrabModeAsync, GrabModeAsync, CurrentTime);
						XGrabPointer(disp, win, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, win, None, CurrentTime);
					}
					else  {			// create a window in window mode
						attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask | PointerMotionMask;
						win = XCreateWindow(
								disp,
								RootWindow(disp, visual_info->screen),
								0,
								0,
								width,
								height,
								0,
								visual_info->depth,
								InputOutput,
								visual_info->visual,
								CWBorderPixel | CWColormap | CWEventMask,
								&attr
								);

						bp = 3;
						bp = 2;

						wmDelete = XInternAtom(disp, "WM_DELETE_WINDOW", True);
						XSetWMProtocols(disp, win, &wmDelete, 1);

						XSetStandardProperties(disp, win, title, title, None, NULL, 0, NULL);		// only set window title and handle wm_delete_events if in windowed mode
						XMapRaised(disp, win);
					}


#ifdef THROTTLE
					// This returns the FD of the X11 connection (or something like that)
					x11_fd = ConnectionNumber(disp);

					// Set Timer Period (this is how you configure the frequency):
					// xxx

					tv_period.tv_sec = 0;
					tv_period.tv_usec = 1000;			// 1 ms

#endif

					// connect the glx-context to the window

					glXMakeCurrent(disp, win, ctx);


					get_x_geometry();

#if 0
					unsigned int r2, b2;
					XGetGeometry(
							disp,
							win,
							&root_window,
							&clientrect.left,		// return the x and y coordinates that define the location of the drawable. For a window, these coordinates
							&clientrect.top,			// specify the upper-left outer corner relative to its parent's origin. For pixmaps, these coordinates are always zero.
							&r2,							// return the drawable's dimensions (width and height). For a window, these dimensions specify the inside size, not including the border.
							&b2,
							&border_width,			// returns the border width in pixels. If the drawable is a pixmap, it returns zero.
							&depth
							);
					//clientrect.right = r2-1;
					//clientrect.bottom = b2-1;

					// change clientrect to 0 origin for opengl

					int dx = clientrect.left;
					int dy = clientrect.top;
					dx = 0;

					clientrect.left -= dx;
					clientrect.right = r2 - dx - 1;

					clientrect.top -= dy;
					//clientrect.bottom = b2 - dy - 1;
					clientrect.bottom = b2 - 1;
#endif


					/*
					if (glXIsDirect(disp, ctx))
						printf("Congrats, you have Direct Rendering!\n");
					else
						printf("Sorry, no Direct Rendering possible!\n");
					*/

					initGL();

					return 0;

				}							// int OGL::create_window(int _width, int _height, int _bits, bool _fullscreen)  {

				/**********************************************************************

				 **********************************************************************/

				void OGL::swap(void)  {

					frames++;
					now = timeGetTime();
					if ((now - then) >= 1000)  {
						fps = (float)frames;
						frames = 0;
						then = now;
					}

					if (doubleBuffered)  {
						glXSwapBuffers(disp, win);
					}
					return;
				}

				/************************************************************************************
				  use xfontsel utility to browse fonts

				  The following table describes the meaning of each field.

				  FIELD 	DESCRIPTION

				  fndry 	The type foundry that digitized and supplied the font data.
				  fmly		The name of the typographic style (for example, `courier').
				  wght		The weight of the font, or its nominal blackness, the degree of boldness or thickness of its characters. Values include `heavy', `bold', `medium', `light', and `thin'.
				  slant 	The posture of the font, usually `r' (for `roman', or upright), `i' (`italic', slanted upward to the right and differing in shape from the roman counterpart), or `o' (`oblique', slanted but with the shape of the roman counterpart).
				  swdth 	The proportionate width of the characters in the font, or its nominal width, such as `normal', `condensed', `extended', `narrow', and `wide'.
				  adstyl 	Any additional style descriptions the particular font takes, such as `serif' (fonts that have small strokes drawn on the ends of each line in the character) or `sans serif' (fonts that omit serifs).
				  pxlsz 	The height, in pixels, of the type. Also called body size.
				  ptsz		The height, in points, of the type.
				  resx		The horizontal screen resolution the font was designed for, in dpi ("dots per inch").
				  resy		The vertical screen resolution the font was designed for, in dpi.
				  spc		The kind of spacing used by the font (its escapement class); either `p' (a proportional font containing characters with varied spacing), `m' (a monospaced font containing characters with constant spacing), or `c' (a character cell font containing characters with constant spacing and constant height).
				  avgwdth 	The average width of the characters used in the font, in 1/10th pixel units.
				  rgstry 	The international standards body, or registry, that owns the encoding.
				  encdng 	The registered name of this character set, or its encoding.

xfontsel: Selecting an X font name.
xlsfonts: Listing available fonts.
xfd: Viewing a character set in a font.
xterm Font: Setting the font in an Xterm.

int XTextWidth(font_struct, string, count)
XFontStruct *font_struct;
char *string;
int count;

				 ************************************************************************************/

#if 0
				void OGL::create_default_font(void)  {

					/*
						nfonts = 1;

						fonts[nfonts-1].base = glGenLists(96);
						fonts[nfonts-1].font = XLoadQueryFont(disp, "-*-helvetica-bold-r-normal--16-*-*-*-p-*-iso8859-1");
						if (fonts[nfonts-1].font == NULL) {
					// this really *should* be available on every X Window System...
					fonts[nfonts-1].font = XLoadQueryFont(disp, "fixed");
					if (fonts[nfonts-1].font == NULL) {
					printf("Problems loading fonts :-(\n");
					exit(1);
					}
					}
					glXUseXFont(fonts[nfonts-1].font->fid, 32, 96, fonts[nfonts-1].base);		// build 96 display lists out of our font starting at char 32
					 */

					FONT fnt = {0};
					fonts.push_back(fnt);
					nfonts = fonts.size();
					int i = nfonts - 1;

					fonts[i].size = 0;
					fonts[i].base = glGenLists(96);
					fonts[i].font = XLoadQueryFont(disp, "-*-helvetica-bold-r-normal--16-*-*-*-p-*-iso8859-1");

					if (fonts[i].font == NULL) {
						// this really *should* be available on every X Window System...
						fonts[i].font = XLoadQueryFont(disp, "fixed");
						if (fonts[i].font == NULL) {
							printf("Problems loading fonts :-(\n");
							exit(1);
						}
					}
					glXUseXFont(fonts[i].font->fid, 32, 96, fonts[i].base);		// build 96 display lists out of our font starting at char 32



					return;
				}

				/**********************************************************************

				 **********************************************************************/

				void OGL::destroy_default_font(void)  {
					int len = fonts.size();

					if (fonts.size()>0)  {
						XFreeFont(disp, fonts[0].font);
						glDeleteLists(fonts[0].base, 96);
					}
					return;
				}

				/************************************************************************************

				 ************************************************************************************/

				void OGL::destroy_fonts(void) {
					int i;

					for(i=1; i<fonts.size(); i++)  {
						if (fonts[i].font)  {
							XFreeFont(disp, fonts[i].font);										// free our XFontStruct since we have our display lists
							fonts[i].font = 0;
							glDeleteLists(fonts[i].base, 96);
						}
					}

					return;
				}

#endif				// #if 0


				/************************************************************************************

				 ************************************************************************************/

				int OGL::getwid(int k, const char *str)  {
					#if 0
					int len;

					if (k>nfonts-1)  {
						return 0;
					}

					//len = XTextWidth(fonts[k].font, str, strlen(str));
					len = XTextWidth(fonts[k], str, strlen(str));

					if (len<0)  {
						return 0;
					}
					if (len>width)  {
						return 0;
					}

					return len;
					return 0;
					#endif

					if (k >= nfonts || k<0)  {
						return 0;
					}

					return fonts[k]->getwid(str);

				}

				/************************************************************************************

				 ************************************************************************************/

				void OGL::getextent(int k, const char *str, SIZE *sz)  {

					#if 0
					int dir, asc, desc;
					XCharStruct overall;

					sz->cx = 0;
					sz->cy = 0;

					if (k>nfonts-1)  {
						return;
					}

					XTextExtents(fonts[k].font, str, strlen(str), &dir, &asc, &desc, &overall);

					sz->cx = overall.width;
					sz->cy = asc;
					#endif
					
					return;
				}

				/************************************************************************************

				 ************************************************************************************/

		//int OGL::addfont(const char * desc)  {

		int addfont(const char *_ttfname, int _h)  {
					return 0;
#if 0
					/*
						nfonts++;
						fonts[nfonts-1].base = glGenLists(96);
						fonts[nfonts-1].font = XLoadQueryFont(disp, desc );
						if (fonts[nfonts-1].font == NULL) {
					// this really *should* be available on every X Window System...
					fonts[nfonts-1].font = XLoadQueryFont(disp, "fixed");
					if (fonts[nfonts-1].font == NULL) {
					printf("Problems loading fonts :-(\n");
					exit(1);
					}
					}
					glXUseXFont(fonts[nfonts-1].font->fid, 32, 96, fonts[nfonts-1].base);						// build 96 display lists out of our font starting at char 32

					return fonts[nfonts-1].base;
					 */

					FONT fnt;
					fnt.base = glGenLists(96);
					fnt.font = XLoadQueryFont(disp, desc );
					if (fnt.font == NULL) {
						// this really *should* be available on every X Window System...
						fnt.font = XLoadQueryFont(disp, "fixed");
						if (fnt.font == NULL) {
							printf("Problems loading fonts :-(\n");
							exit(1);
						}
					}
					glXUseXFont(fnt.font->fid, 32, 96, fnt.base);						// build 96 display lists out of our font starting at char 32
					fonts.push_back(fnt);
					return fnt.base;
#endif


				}

				/************************************************************************************

				 ************************************************************************************/
#if 0
				void OGL::start_timer(void)  {
					timer_event = timeSetEvent(MSINTRATE, ACCURACY, TimerProc, (DWORD)this, TIME_PERIODIC);		// generates accurate 1.0 ms tick
					return;
				}

/******************************************************************************************************

******************************************************************************************************/

//int OGL::get_font_height(int k)  {

	if (k >= nfonts || k<0)  {
		return k = 0;
	}
	//return fonts[k]->getwid(str);

	return fonts[k]->getheight();

}
#endif


/******************************************************************************************************

******************************************************************************************************/

void OGL::get_x_geometry(void)  {
	unsigned int r2, b2;
	int dx, dy;

	XGetGeometry(
						disp,
						win,
						&root_window,
						&clientrect.left,		// return the x and y coordinates that define the location of the drawable. For a window, these coordinates
						&clientrect.top,			// specify the upper-left outer corner relative to its parent's origin. For pixmaps, these coordinates are always zero.
						&r2,							// return the drawable's dimensions (width and height). For a window, these dimensions specify the inside size, not including the border.
						&b2,
						&border_width,			// returns the border width in pixels. If the drawable is a pixmap, it returns zero.
						&depth
						);
	//clientrect.right = r2-1;
	//clientrect.bottom = b2-1;

	// change clientrect to 0 origin for opengl

	dx = clientrect.left;
	dy = clientrect.top;
	dx = 0;

	clientrect.left -= dx;
	clientrect.right = r2 - dx - 1;

	clientrect.top -= dy;
	//clientrect.bottom = b2 - dy - 1;
	clientrect.bottom = b2 - 1;

	return;
}

/******************************************************************************************************

******************************************************************************************************/

void OGL::set_title(const char *_title)  {

	return;
}


/******************************************************************************************************

******************************************************************************************************/

int OGL::open_movie(const char *_movie_name)  {
	movie = new GLAvi();

	if (!movie->open(_movie_name))  {
		return 1;
	}
	//movie->GetTexCrds(&coords[0]);

	return 0;
}

/******************************************************************************************************

******************************************************************************************************/

//bool OGL::GetNextFrame(GLuint ticks)  {
bool OGL::GetNextFrame(void)  {
	if (!movie) return false;
	//return movie->GetNextFrame(ticks);
	return movie->GetNextFrame();
}

/******************************************************************************************************
		glVertex3f(r.left, r.top, orthoz);				// Top Left
		glVertex3f(r.right, r.top, orthoz);				// Top Right
		glVertex3f(r.right, r.bottom, orthoz);			// Bottom Right
		glVertex3f(r.left, r.bottom, orthoz);			// Bottom Left

******************************************************************************************************/

void OGL::draw_frame(RECT *r)  {
	//xxx

	glEnable( GL_TEXTURE_2D );
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//logo_texture_id = ogl->loadTGATexture("logo2.tga");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBindTexture( GL_TEXTURE_2D, logo_texture_id );

	glBegin(GL_QUADS);
		glTexCoord2f (0.0, 0.0);
		glVertex3f((float)r->left, (float)r->top,  0.0f);				// left, top

		glTexCoord2f (1.0f, 0.0);
		glVertex3f( (float)r->right, (float)r->top,  0.0f);			// right, top

		glTexCoord2f (1.0f, 1.0f);
		glVertex3f( (float)r->right,  (float)r->bottom,  0.0f);		// right, bottom

		glTexCoord2f (0.0, 1.0f);
		glVertex3f( (float)r->left,  (float)r->bottom,  0.0f);		// left, bottom
	glEnd();

	glDisable( GL_TEXTURE_2D );
	
	return;
}


