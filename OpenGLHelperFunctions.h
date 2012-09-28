#ifndef OPENGL_HELPER_HEADER_INCLUDE
#define OPENGL_HELPER_HEADER_INCLUDE
#include "ripple.h"

int OGLErrorCheck(int lineNumber);
GLuint CompileShader(GLenum eShaderType, const char* filename, int debugOption);
GLint MakeShaderProgram(const char* vertFileName, const char* geoFileName, const char* fragFileName, int debugOption);
#endif //OPENGL_HELPER_HEADER_INCLUDE
