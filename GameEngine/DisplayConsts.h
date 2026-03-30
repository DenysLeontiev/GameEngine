#ifndef DISPLAY_CONSTS_H
#define DISPLAY_CONSTS_H

#include <unordered_map>
#include <string>

#include <GL/glew.h>

class DisplayConsts {
public:
	static inline const std::unordered_map<std::string, GLenum> DRAWING_TYPES_MAP = {
	{"TRIANGLES", GL_TRIANGLES},
	{"TRIANGLE STRIP", GL_TRIANGLE_STRIP},
	{"TRIANGLE FAN", GL_TRIANGLE_FAN},
	{"LINES", GL_LINES},
	{"LINE STRIP", GL_LINE_STRIP},
	{"LINE LOOP", GL_LINE_LOOP},
	{"POINTS", GL_POINTS},
	{"PATCHES", GL_PATCHES}
	};
};

#endif