#ifndef RIPPLE_HEADER_INCLUDE
#define RIPPLE_HEADER_INCLUDE
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>

/* Compile options */
#define OPENGL 1
//#define OPENCL 1

#define SUCCESS 0
#define FAILURE 1

#define PI 3.14159265358979323846264338327950288419716939937510582097494459230

//the plane is the XZ plane
#define NUM_VERTICES_X 400
#define NUM_VERTICES_Z 400
#define NUM_VERTICES NUM_VERTICES_X*NUM_VERTICES_Z

#define ITERATIONS 10000
#endif //RIPPLE_HEADER_INCLUDE
