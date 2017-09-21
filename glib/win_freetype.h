#ifndef FREE_NEHE_H
#define FREE_NEHE_H

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <windows.h>		//(the GL headers need it)
#include <GL/gl.h>
#include <GL/glu.h>

#include <vector>
#include <string>

// Using the STL exception library increases the
// chances that someone else using our code will corretly
// catch any exceptions that we throw.

#include <stdexcept>

// MSVC will spit out all sorts of useless warnings if
// you create vectors of strings, this pragma gets rid of them.

#pragma warning(disable: 4786) 

// Wrap everything in a namespace, that we can use common
// function names like "print" without worrying about
// overlapping with anyone else's code.

namespace freetype {
	// inside of this namespace, give ourselves the ability to write just "vector" instead of "std::vector"

	using std::vector;
	using std::string;			// ditto for string

	// this holds all of the information related to any freetype font that we want to create

	//#define CHNG
	#ifdef CHNG
	struct FONT {
	#else
	struct font_data {
	#endif
		float h;											//  holds the height of the font
		GLuint *textures;									//	holds the texture id's 
		GLuint list_base;									//	holds the first display list id
		void init(const char * fname, unsigned int h);		// the init function will create a font of of the height h from the file fname
		void clean();										// free all the resources assosiated with the font
	};


	// the flagship function of the library - this thing will print
	// out text at window coordinates x,y, using the font ft_font.
	// the current modelview matrix will also be applied to the text

	#ifdef CHNG
	void printxy(const FONT &ft_font, float x, float y, const char *fmt, ...) ;
	#else
	void printxy(const font_data &ft_font, float x, float y, const char *fmt, ...) ;
	#endif

}

#endif