//dependancies of the entire project
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <glm/glm/glm.hpp> //header only library for matrices (C++ only)
#include <glm/glm/gtc/type_ptr.hpp> //and this was for the value_ptr() function


/* Compile options */
#define OPENGL 1
//#define OPENCL 1

#define SUCCESS 0
#define FAILURE 1

#define PI 3.14159265358979323846264338327950288419716939937510582097494459230

//the plane is the XZ plane
#define NUM_VERTICES_X 10
#define NUM_VERTICES_Z 10
#define NUM_VERTICES NUM_VERTICES_X*NUM_VERTICES_Z

#define ITERATIONS 10

extern int g_windowHeight;
extern int g_windowWidth;
