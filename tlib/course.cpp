
#ifdef WIN32
	#pragma warning(disable:4996 4005)					// for vista strncpy_s
	#include <windows.h>
	#include <windowsx.h>
#endif

#include <assert.h>

#include <comglobs.h>
#include <globals.h>
#include <oldsdirs.h>

#include <utils.h>
//#include <ogl.h>
#include <tdefs.h>
#include <course.h>


#ifdef DO_COURSE_GL
std::vector<GLdouble *> course_combine_vertices;

 #ifdef _DEBUG
   int dbg_vertex_i = 0;
	float dbg_min_y;
	float dbg_max_y;
#endif

#endif






/**********************************************************************
	don't need this default constructor any more, delete later
**********************************************************************/

Course::Course(const char *_name)  {
	if (_name)  {
		strncpy(name, _name, sizeof(name)-1);
	}
	else  {
		memset(name, 0, sizeof(name));
	}
	logg = NULL;


#if 0											// xxmm
#ifdef DOGLOBS
	sprintf(str, "%s%smc.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
#else
	sprintf(str, "%s%smaincourse.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR);
#endif

	logg = new Logger(str);
#ifdef _DEBUG
	if (logg==NULL)  {
		bp = 1;
	}
#endif
	if (logg) logg->write(10,0,1,"Main course\n");
#endif

	init();
	return;
}


/**********************************************************************

**********************************************************************/

Course::Course(std::vector<FPOINT> _points, const char *_name)  {

#ifdef DOGLOBS
	sprintf(str, "%s%smaincourse.log", dirs[DIR_DEBUG].c_str(), FILESEPSTR);
#else
	sprintf(str, "%s%smaincourse.log", SDIRS::dirs[DIR_DEBUG].c_str(), FILESEPSTR);
#endif
	logg = new Logger(str);
	if (logg==NULL)  {
		bp = 1;
	}
	logg->write(10,0,1,"Main course\n");

	if (_name)  {
		strncpy(name, _name, sizeof(name)-1);
	}
	else  {
		memset(name, 0, sizeof(name));
	}

	if (logg) logg->write(10,0,1,"name = %s\n", name);

	init();

	points = _points;

	npoints = (int)points.size();

	if (logg) logg->write(10,0,1,"npoints = %d\n", npoints);
	if (logg) logg->write(10,0, 1, "minel_meters = %.2f\n", minel_meters);
	if (logg) logg->write(10, 0, 1, "maxel_meters = %.2f\n", maxel_meters);

	if (logg)  {
		int i;
		for(i=0; i<npoints; i++)  {
			logg->write("%.2f, %.2f\n", points[i].x, points[i].y );
		}
	}
	if (logg) logg->write(10,0,1,"\n");


	compute_grades();

#ifdef _DEBUG
//	logg = new Logger("course.log");
#endif

	if (logg) logg->write("minel_meters = %.2f\n", minel_meters);
	if (logg) logg->write("maxel_meters = %.2f\n", maxel_meters);
	if (logg) logg->write("npoints = %d\n", npoints);

	if (logg) logg->write(10,0,1,"\n");
	if (logg) logg->write(0,0,0,"   I             X            Y\n\n");


#ifdef _DEBUG
	int i;
	for(i=0; i<npoints; i++)  {
		sprintf(str, "%4d    %10.2f   %10.2f\n", i, points[i].x, points[i].y);
		if (logg) logg->write(str);
	}
#endif

	bp = 0;
}						// constructor



/**********************************************************************
	Destructor
**********************************************************************/

Course::~Course()  {
	destroy();
	return;
}

/**********************************************************************

**********************************************************************/

void Course::init(void)  {

	draws = 0;
	grades.clear();
	points.clear();
	memset(str, 0, sizeof(str));
	memset(&outer_rect, 0, sizeof(outer_rect));
	bp = 0;
	//logg = NULL;
	npoints = 0;
	seek_meters = 0.0f;
	total_meters = 0.0f;
	//total_miles = 0.0f;
	total_feet = 0.0f;
	total_km = 0.0f;
	minel_meters = 0.0f;
	maxel_meters = 0.0f;

	return;
}

/**********************************************************************

**********************************************************************/

void Course::destroy(void)  {


#ifdef DO_COURSE_GL
	int i, len;
	len = course_combine_vertices.size();
	for(i=0; i<len; i++)  {
		free (course_combine_vertices[i]);
	}
	course_combine_vertices.clear();
#endif

	DEL(logg);
	return;
}

#ifdef DO_COURSE_GL

/**********************************************************************
	this is inside a compile block
	entry: blending is enabled

    top:    117, 165, 201
    bottom: 15, 76, 121
ogl->set_color(15, 76, 121);

**********************************************************************/

int Course::draw(OGL *ogl)  {
	float mx2, bx2, my2, by2;
	int i;
	FPOINT fp;
    double *vertices=NULL;
    double z = ogl->get_orthoz();
    unsigned int k;
    float mr, br, mg, bg, mb, bb;
	float rbot, bbot, gbot;
	float rtop, btop, gtop;
	double dy;
	int len;

	draws++;

	if (logg) logg->write(10,0,1,"\n\ndc1\n");

#ifdef _DEBUG
	ogl->check();
#endif

#if 0
	// fill in the background
	OGL::FCOLOR topcolor = {1.0f, 1.0f, 1.0f, 1.0f};
	OGL::FCOLOR bottomcolor = {0.5f, 0.5f, 0.5f, 1.0f};
	ogl->gradient_box(outer_rect, topcolor, topcolor, bottomcolor, bottomcolor);
#endif

	bottom = outer_rect.top + .90f*(outer_rect.bottom-outer_rect.top);
	top =		outer_rect.top + .10f*(outer_rect.bottom-outer_rect.top);

	if (logg) logg->write(10,0,1,"dc2\n");

	//-----------------------------
	// CALCULATE THE COURSE:
	//-----------------------------

	map(0.0f, (float)total_meters, inner_rect.left+1.0f, inner_rect.right-1.0f, &mx2, &bx2);
	map(minel_meters, maxel_meters, bottom, top, &my2, &by2);


	if (logg) logg->write(10,0,1,"\n\n");
	if (logg) logg->write("draws = %d\n", draws);
	if (logg) logg->write("courserect = %.2f, %.2f    %.2f %.2f\n\n", outer_rect.left, outer_rect.right, outer_rect.top, outer_rect.bottom);
	if (logg) logg->write("   I      SCREEN X     SCREEN Y\n\n");

	screen_points.clear();
	
	if (logg) logg->write(10,0,1,"dc3\n");

	for(i=0; i<npoints; i++)  {
		fp.x = mx2 * points[i].x + bx2;
		fp.y = my2 * points[i].y  +  by2;
		screen_points.push_back(fp);
#ifdef _DEBUG
		sprintf(str, "%4d    %10.2f   %10.2f\n", i, fp.x, fp.y);
		if (logg) logg->write(str);
#endif
	}

	if (logg) logg->write(10, 0, 1, "\n");

	//--------------------------------------------------------------
	// REPLACE Y[] WITH THE MAV() FOR COMPLEX COURSES (>50 LEGS)
	//--------------------------------------------------------------

    if (npoints > 50)  {										// tlm: changed from 20 on 20040311 for multi-rider
        //Utils::mav(y, npoints, 5, 0);				// 5 point moving average
    }

    //------------------------------------------------------------------------------
    // FIRST ERASE THE COURSE AREA IN CASE WE ARE REDRAWING A DIFFERENT COURSE
    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    // DRAW THE COURSE:
    //------------------------------------------------------------------------------


#define EXPTS 3				// 3 extra points, 2 for base,  1 for closing point

    k = (unsigned int) (N_VERTEX_POINTS*(npoints + EXPTS));

    vertices = new double[k];

    for(i=0; i<npoints; i++)  {
        vertices[i*N_VERTEX_POINTS] = screen_points[i].x;
        vertices[i*N_VERTEX_POINTS+1] = screen_points[i].y;
        vertices[i*N_VERTEX_POINTS+2] = z;
        vertices[i*N_VERTEX_POINTS+3] = -1.0;         //  15.0/255.0;
        vertices[i*N_VERTEX_POINTS+4] = -1.0;         //  76.0/255.0;
        vertices[i*N_VERTEX_POINTS+5] = -1.0;         // 121.0/255.0;
        #if N_VERTEX_POINTS==7
            vertices[i*N_VERTEX_POINTS+6] = i;
        #endif
    }

	// set the color of the bottom of the course
	vertices[i*N_VERTEX_POINTS] = screen_points[npoints-1].x;
	vertices[i*N_VERTEX_POINTS+1] = outer_rect.bottom-1.0f;
	vertices[i*N_VERTEX_POINTS+2] = z;
	vertices[i*N_VERTEX_POINTS+3] = -1.0;       // 15.0/255.0;
	vertices[i*N_VERTEX_POINTS+4] = -1.0;       // 76.0/255.0;
	vertices[i*N_VERTEX_POINTS+5] = -1.0;       // 121.0/255.0;
    #if N_VERTEX_POINTS==7
        vertices[i*N_VERTEX_POINTS+6] = i;
	#endif

	i++;
	vertices[i*N_VERTEX_POINTS] = screen_points[0].x;
	vertices[i*N_VERTEX_POINTS+1] = outer_rect.bottom-1.0f;
	vertices[i*N_VERTEX_POINTS+2] = z;
	vertices[i*N_VERTEX_POINTS+3] = -1.0;       // 15.0/255.0;
	vertices[i*N_VERTEX_POINTS+4] = -1.0;       // 76.0/255.0;
	vertices[i*N_VERTEX_POINTS+5] = -1.0;       // 121.0/255.0;
    #if N_VERTEX_POINTS==7
        vertices[i*N_VERTEX_POINTS+6] = i;
    #endif

	// this closes the polygon

	i++;
	vertices[i*N_VERTEX_POINTS] = screen_points[0].x;
	vertices[i*N_VERTEX_POINTS+1] = screen_points[0].y;
	vertices[i*N_VERTEX_POINTS+2] = z;
	vertices[i*N_VERTEX_POINTS+3] = -1.0;       // 15.0/255.0;
	vertices[i*N_VERTEX_POINTS+4] = -1.0;       // 76.0/255.0;
	vertices[i*N_VERTEX_POINTS+5] = -1.0;       // 121.0/255.0;
    #if NPOINTS==7
        vertices[i*NPOINTS+6] = i;
    #endif


	// set up the colors of the vertices

	rtop =  58.0f / 255.0f;
	gtop = 113.0f / 255.0f;
	btop = 147.0f / 255.0f;

	rbot = 143.0f / 255.0f;
	gbot = 183.0f / 255.0f;
	bbot = 211.0f / 255.0f;

	map(top, outer_rect.bottom - 1.0f, rtop, rbot, &mr, &br);
	map(top, outer_rect.bottom - 1.0f, gtop, gbot, &mg, &bg);
	map(top, outer_rect.bottom - 1.0f, btop, bbot, &mb, &bb);

	bp = 0;

    for(i=0; i<npoints+EXPTS; i++)  {
		dy = vertices[i*N_VERTEX_POINTS+1];
		vertices[i*N_VERTEX_POINTS+3] = mr*dy + br;
        vertices[i*N_VERTEX_POINTS+4] = mg*dy + bg;
        vertices[i*N_VERTEX_POINTS+5] = mb*dy + bb;
	}


	#ifdef _DEBUG
	if (logg)  {
		
		logg->write("\nminel_meters = %.2f meters\n", minel_meters);
		logg->write("maxel_meters = %.2f meters\n", maxel_meters);

		logg->write("\ntop = %.2f\n", top);
		logg->write("bottom = %.2f\n", bottom);

		logg->write("\ncourserect.left = %.2f\n", outer_rect.left);
		logg->write("courserect.top = %.2f\n", outer_rect.top);
		logg->write("courserect.right = %.2f\n", outer_rect.right);
		logg->write("courserect.bottom = %.2f\n", outer_rect.bottom);

		double *p;
		logg->write(10, 0, 1, "\nvertices:\n\n  i       x        y        z      R     G     B\n\n");

		for(i=0; i<npoints+EXPTS; i++)  {
			p = &vertices[N_VERTEX_POINTS * i];
			logg->write("%3d %7.2lf, %7.2lf, %7.2lf   %4d, %4d, %4d\n",
				i,
				*p,
				*(p+1),
				*(p+2),
				(int) (.5 + (255.0*(*(p+3)))),
				(int) (.5 + (255.0*(*(p+4)))),
				(int) (.5 + (255.0*(*(p+5))))
			);
			if (i==npoints-1)  {
				logg->write("\n");
			}

	    }
		logg->write(10, 0, 1, "\n");
	}
	#endif


#ifdef _DEBUG
	ogl->check();
#endif

		len = course_combine_vertices.size();
		for(i=0; i<len; i++)  {
			free (course_combine_vertices[i]);
		}
		course_combine_vertices.clear();

#if 1

#ifdef _DEBUG
		dbg_vertex_i = 0;
		dbg_min_y = 1000000.0f;
		dbg_max_y = -1000000.0f;

		//printf("\ntesselation:\n\n:");
		//printf("  I       x        y        z\n\n");
#endif

        GLUtesselator* tess = gluNewTess();

		#ifdef WIN32
			gluTessCallback(tess, GLU_TESS_BEGIN,	( GLvoid(__stdcall *)() ) &glu_tess_begin);
			gluTessCallback(tess, GLU_TESS_END,     (GLvoid (__stdcall *) ()) &glu_tess_end);
			gluTessCallback(tess, GLU_TESS_ERROR,	(GLvoid (__stdcall *) ())	&glu_tess_error);
			gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (__stdcall *) ()) &glu_tess_vertex);
			gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (__stdcall *) ()) &glu_tess_combine);
			//gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (__stdcall *) ()) &glVertex3dv);
			//gluTessCallback(tess, GLU_TESS_END,		(GLvoid (__stdcall *) ())							&glu_tess_end);
			//gluTessCallback(tess, GLU_TESS_ERROR,	(GLvoid (__stdcall *) ())							&glu_tess_error);
		#else
			gluTessCallback(tess, GLU_TESS_BEGIN,	( GLvoid(*)() ) &glu_tess_begin);
			gluTessCallback(tess, GLU_TESS_END,     (GLvoid (*) ()) &glu_tess_end);
			gluTessCallback(tess, GLU_TESS_ERROR,	(GLvoid (*) ())	&glu_tess_error);
			gluTessCallback(tess, GLU_TESS_VERTEX,  (GLvoid (*) ()) &glu_tess_vertex);                  //
			gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (*) ()) &glu_tess_combine);               // only needed for self-intersecting polygons
			//gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*) ()) &glVertex3dv);                     // this uses the default if you have no color
		#endif

		glShadeModel(GL_SMOOTH);			// CAUSES MULTICOLORED LINES TO NOT WORK!!!!!!!!!! USE GL_SMOOTH TO UNDO IF YOU USE THIS!!!!!!

		#ifdef _DEBUG
			ogl->check();
		#endif

		gluTessBeginPolygon(tess, NULL);				// causes glerror
			#ifdef _DEBUG
				ogl->check();
			#endif
			gluTessBeginContour(tess);
				#ifdef _DEBUG
					ogl->check();
				#endif

				for(i=0; i<npoints+EXPTS; i++)  {
					gluTessVertex(tess, &vertices[i*N_VERTEX_POINTS], &vertices[i*N_VERTEX_POINTS] );                    //
					#ifdef _DEBUG
						ogl->check();
					#endif
                }
            gluTessEndContour(tess);
			#ifdef _DEBUG
				ogl->check();
			#endif

		gluTessEndPolygon(tess);		// this actually invokes glu_tess_vertex() for each vertex

		#ifdef _DEBUG
			ogl->check();
		#endif
	bp = 4;

	gluDeleteTess(tess);
	tess = NULL;
	DELARR(vertices);

#ifdef _DEBUG
	//printf("\n");
	//printf("dbg_min_y = %.2f\n", dbg_min_y);
	//printf("dbg_max_y = %.2f\n", dbg_max_y);
#endif


	//glShadeModel(GL_FLAT);			// CAUSES MULTICOLORED LINES TO NOT WORK!!!!!!!!!! USE GL_SMOOTH TO UNDO IF YOU USE THIS!!!!!!
#endif


#if 1

//#ifdef _DEBUG
#if 0
	// draw a red box around the courserect for reference debugging
	ogl->set_line_width(1.0f);
	ogl->set_color(255, 0, 0);
	ogl->orthorect(outer_rect.left, outer_rect.bottom, outer_rect.right, outer_rect.top, false);
#endif

	//--------------------------------------------------------------------------------------------
	// redraw top of course with dark line in order to get rid of the polygon aliasing
	//--------------------------------------------------------------------------------------------

#ifdef _DEBUG
	//int sz = course_combine_vertices.size();
#endif

	ogl->set_line_width(1.0f);
	//ogl->set_color(255, 0, 0);
	ogl->set_colorf(rtop, gtop, btop);

	glBegin(GL_LINE_STRIP);
		for(i=0; i<npoints; i++)  {
			glVertex3f(screen_points[i].x, screen_points[i].y, (GLfloat)z);
		}
	glEnd();

	#if 0
		ogl->set_color(0, 0, 0);
		ogl->set_line_width(1.0f);
		ogl->ortholine(outer_rect.left+1.0f, bottom, outer_rect.right-1.0f, bottom);
	#endif

#endif


#ifdef _DEBUG
	ogl->check();
#endif

	return 0;
}					// draw()




/**********************************************************************

**********************************************************************/

#ifdef WIN32
GLvoid CALLBACK Course::glu_tess_begin(GLenum which)  {
#else
GLvoid Course::glu_tess_begin(GLenum which)  {
#endif


   glBegin(which);

#ifdef _DEBUG
	if (glGetError() != GL_NO_ERROR)  {
		//int bp = 0;
	}
#endif

	return;
}


/**********************************************************************

**********************************************************************/

#ifdef WIN32
GLvoid CALLBACK Course::glu_tess_end(void)  {
#else
GLvoid Course::glu_tess_end(void)  {
#endif

   glEnd();
#ifdef _DEBUG
	if (glGetError() != GL_NO_ERROR)  {
		//int bp = 0;
	}
#endif

	return;
}

/**********************************************************************

**********************************************************************/

#ifdef WIN32
GLvoid CALLBACK Course::glu_tess_error(GLenum errorCode)  {
#else
GLvoid Course::glu_tess_error(GLenum errorCode)  {
#endif

	const GLubyte *estring;

   estring = gluErrorString(errorCode);
   fprintf (stderr, "Tessellation Error: %s\n", estring);
   exit (0);
	//return;
}

/**********************************************************************
	vertex is an array of 3 doubles (x, y, z)
	x is optional data that can be passed (like color info)
**********************************************************************/

#ifdef WIN32
GLvoid CALLBACK Course::glu_tess_vertex(GLvoid *vertex)  {
#else
GLvoid Course::glu_tess_vertex(GLvoid *vertex)  {
#endif

	const GLdouble *p;

 #ifdef _DEBUG
  //dbg_vertex_i;
	if (glGetError() != GL_NO_ERROR)  {
		//int bp = 0;
	}
#endif

    p = (GLdouble *) vertex;
	glColor3dv(p+3);
    glVertex3dv(p);

#ifdef _DEBUG
    #if N_VERTEX_POINTS==7
        int k = (int)(*(p+6));
        //printf("%3d %3d %7.2lf, %7.2lf, %7.2lf   color =  %4d, %4d, %4d\n", i, k, *p, *(p+1), *(p+2), (int) (.5 + (255.0*(*(p+3)))), (int) (.5 + (255.0*(*(p+4)))), (int) (.5 + (255.0*(*(p+5)))) );
    #else
        //printf("%3d %7.2lf, %7.2lf, %7.2lf   color =  %4d, %4d, %4d\n", i, *p, *(p+1), *(p+2), (int) (.5 + (255.0*(*(p+3)))), (int) (.5 + (255.0*(*(p+4)))), (int) (.5 + (255.0*(*(p+5)))) );
        //printf("%4d %7.2lf, %7.2lf, %7.2lf\n", dbg_vertex_i, *p, *(p+1), *(p+2) );
    #endif

   dbg_vertex_i++;
	
	if (*(p+1) > dbg_max_y)  {
		dbg_max_y = (float)(*(p+1));
	}
	if (*(p+1) < dbg_min_y)  {
		dbg_min_y = (float)(*(p+1));
	}

#endif
    return;
}



/*
GLvoid Course::Course::glu_tess_vertex(float x, float y, float z)  {
	glVertex3f(x, y, z);
	return;
}
*/

/**********************************************************************

**********************************************************************/

#ifdef WIN32
GLvoid CALLBACK Course::glu_tess_combine(
							   GLdouble coords[3],
							   GLdouble *vertex_data[4],
							   GLfloat weight[4],
							   GLdouble **dataOut)  {
#else
GLvoid Course::glu_tess_combine(
							   GLdouble coords[3],
							   GLdouble *vertex_data[4],
							   GLfloat weight[4],
							   GLdouble **dataOut)  {
#endif

	GLdouble *vertex;
	int i, j;

#ifdef _DEBUG
	if (glGetError() != GL_NO_ERROR)  {
		//int bp = 0;
	}
#endif


   vertex = (GLdouble *) malloc(N_VERTEX_POINTS * sizeof(GLdouble));

#ifdef _DEBUG
   i = N_VERTEX_POINTS;
#endif

   vertex[0] = coords[0];
   vertex[1] = coords[1];
   vertex[2] = coords[2];

	for (i=3; i < 6; i++)  {										// the r, g, b values
		vertex[i] = 0.0;
		for(j=0; j<4; j++)  {
			if (vertex_data[j])  {
				vertex[i] += weight[j] * vertex_data[j][i];
			}
		}
		/*
		vertex[i] =	weight[0] * vertex_data[0][i] +
					weight[1] * vertex_data[1][i] +
					weight[2] * vertex_data[2][i] +
					weight[3] * vertex_data[3][i];
		*/
	}

#if N_VERTEX_POINTS==7
	vertex[i] = -1.0;
#endif

	course_combine_vertices.push_back(vertex);								// save to deallocate later

   *dataOut = vertex;				// must deallocate later!!!!!!!!!

	return;
}

#endif			// #ifdef DO_COURSE_GL





/**********************************************************************

**********************************************************************/

void Course::compute_grades(void)  {
	int i;
	float dx, dy, y, percent_grade;
	if (points.size()==0)  {
		return;
	}

	minel_meters = maxel_meters = points[0].y;

	for(i=1; i<npoints; i++)  {
		y = points[i].y;
		dx = points[i].x - points[i-1].x;
		dy = y - points[i-1].y;
		percent_grade = 100.0f * (dy / dx);
		grades.push_back(percent_grade);

		if (y > maxel_meters)  {
			maxel_meters = y;
		}
		if (y < minel_meters)  {
			minel_meters = y;
		}
	}

	total_meters = points[npoints-1].x;
	
	
#ifdef _DEBUG
	//int n;
	//n = (int)points.size();
	//n = (int)grades.size();
#endif

	return;
}								// void Course::compute_grades(void)  {

/**********************************************************************

**********************************************************************/

void Course::set_outer_rect(FRECT *_r)  {

	memcpy( &outer_rect, _r, sizeof(FRECT) );
	memcpy( &inner_rect, _r, sizeof(FRECT) );
	inner_rect.left += 20.0f;
	inner_rect.right -= 20.0f;

	return;
}

/**********************************************************************

**********************************************************************/

void Course::write(const char *_description, FILE *_stream)  {
	COURSE_HEADER ch;	// = {0};
	int i;

	memset(&ch, 0, sizeof(ch));
	ch.sec.type = SEC_COURSE;
	ch.sec.structsize = sizeof(ch);
	//ch.npoints = npoints;
	ch.sec.n = npoints;
	strncpy(ch.name, _description, sizeof(ch.name)-1);

	fwrite(&ch, sizeof(ch), 1, _stream);

	for(i=0; i<(int)ch.sec.n; i++)  {
        //std::vector<FPOINT> points;             // physical course points
        //std::vector<FPOINT> screen_points;      // screen course points
        //std::vector<float> grades;
		fwrite(&points[i], sizeof(FPOINT), 1, _stream);					// x, y
		//fwrite(&grades[i], sizeof(float), 1, _stream);					// x, y
		bp = i;
	}

	bp = 2;
	return;
}

/**********************************************************************

**********************************************************************/

float Course::get_total_meters(void)  {
	if (logg)  {
		logg->write(10,0,1,"gtm\n");
	}
	return total_meters;
}
