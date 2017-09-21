
#ifndef COURSE_H_							// don't use _COURSE_H_ because _course.h uses that!!
#define COURSE_H_

#include <glib_defines.h>

#ifdef WIN32
	//#define DO_COURSE_GL								// define for TTS
																// undef for dll
	#include <glib_types.h>
#endif


#ifdef DO_COURSE_GL
	#include <ogl.h>
#endif

/*-------

 a course is a collection of x, y points

--------*/

class Course {

    public:
		Course(const char *_name=NULL);
        Course(std::vector<FPOINT> _points, const char *_name=NULL);
        virtual ~Course();

#ifdef DO_COURSE_GL
        int draw(OGL *ogl);
#endif

        //float get_total_meters(void)  { return total_meters; }
        float get_total_meters(void);
        //float get_total_miles(void)  { return total_miles; }
        
        float get_grade(float _meters, int *_last_grade_i=NULL)  { return 0.0f; }

        void set_outer_rect(FRECT *_r);
        FRECT get_inner_rect(void) { return inner_rect; }

        inline float get_bottom(void)  { return bottom; }
        inline float get_top(void)  { return top; }

		void write(const char *_description, FILE *_outstream);
		int get_npoints(void)  { return npoints; }
		const char *get_name(void)  { return name; }

    private:
        #define N_VERTEX_POINTS 6            // number of points per vertex, 6 = x,y,z,r,g,b .... use 7 for alpha

		char name[MAX_PATH];
		float bottom, top;
        int draws;
        int bp;
        FRECT outer_rect;
        FRECT inner_rect;
        Logger *logg;
        float seek_meters;
        float total_meters;
        //float total_miles;
        float total_feet;
        float total_km;

        float minel_meters;
        float maxel_meters;

        std::vector<FPOINT> points;             // physical course points
        std::vector<FPOINT> screen_points;      // screen course points
        std::vector<float> grades;

		char str[256];
        int npoints;

        void init(void);
        void destroy(void);

        void compute_grades(void);


			#ifdef WIN32
				#ifdef DO_COURSE_GL
	            static GLvoid __stdcall glu_tess_vertex(GLvoid *vertex);
		         static GLvoid __stdcall glu_tess_begin(GLenum which);
			      static GLvoid __stdcall glu_tess_end(void);
				   static GLvoid __stdcall glu_tess_error(GLenum errorCode);
					static GLvoid __stdcall glu_tess_combine(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut );
				#endif
			#else
#ifdef DO_COURSE_GL
				// mac
            static GLvoid glu_tess_vertex(GLvoid *vertex);
            static GLvoid glu_tess_begin(GLenum which);
            static GLvoid glu_tess_end(void);
            static GLvoid glu_tess_error(GLenum errorCode);
            static GLvoid glu_tess_combine(GLdouble coords[3], GLdouble *vertex_data[4], GLfloat weight[4], GLdouble **dataOut );
#endif
	#endif
        
};

#endif		// #ifndef COURSE_H_


