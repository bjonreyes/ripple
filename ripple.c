#include "ripple.h"
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include "OpenGLHelperFunctions.h"

/*Project Purpose:
* This will be a shot of a surface that bounces up and down in a sine wave propogating outwards in a ripple fashion
* The purpose of this project is to facilitate the learning of OpenCL and practising openGL
*/

#define OPENGL_VERTEX_SHADER "VertexShader.glsl"
#define OPENGL_GEOMETRY_SHADER ""
#define OPENGL_FRAGMENT_SHADER "FragShader.glsl"

/* main functions */
int initOpenGL();
int initOpenCL();
int initWindow();
int deinitWindow();
int deinitOpenCL();
int deinitOpenGL();
int initVertices();
int updateVertices(int iterations);
int Render();

/* other supporting functions */
int createVertexPositions();
int deleteVertexPositions();
int setupOpenGLRender();
int closeOpenGLRender();

/*global vars */
//double vertex_positions[NUM_VERTICES_X][NUM_VERTICES_Z][3]; //This is the buffer for holding the vertex positions, and will be an openGL buffer eventually
float* vertex_positions;
double theta = 0.0;
double omega = 2.0*PI;
double amplitude = 1.0;

//windowing System global vars
SDL_Window* window;
SDL_GLContext context;
const int windowWidth = 600;
const int windowHeight = 800;
int swapFlag;

/* OpenGL global vars */
#ifdef OPENGL
GLint programID;
GLuint vertex_buffer_object;
#endif


/* beginning of program */
int main()
{
	assert(createVertexPositions() == SUCCESS);
	/* initialize opengl */
	assert(glewInit() == GLEW_OK);
	assert(initWindow() == SUCCESS);
	assert(initOpenCL() == SUCCESS);
	assert(initOpenGL() == SUCCESS);

	assert(initVertices() == SUCCESS);

	/* loop for ten thousand iterations */
	long i = 0;
	while (i < ITERATIONS)
	{
		/* update the vertices */
		assert(updateVertices(i) == SUCCESS);
		
		assert(setupOpenGLRender());
		/* render the new scene */
		//assert(Render() == SUCCESS);
		assert(closeOpenGLRender());
		//sleep(1);
		i++;
	}
	/* clean up */
	assert(deinitOpenGL() == SUCCESS);
	assert(deinitOpenCL() == SUCCESS);
	assert(deinitWindow() == SUCCESS);
	assert(deleteVertexPositions() == SUCCESS);
	return 0;
}


/* we are giving these functions empty definitions for now because we want to have the vertex
* stuff done and debugged before displaying it */
/* The context stuff is taken care of by the window creation, so this will just be dealing
* with OpenGL resources such as buffers, programs, and shaders that need to be created */
int initOpenGL()
{
#ifdef OPENGL
	/* Generate the buffer that will store the vertices */
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*NUM_VERTICES_X*NUM_VERTICES_Z*3, vertex_positions, GL_STREAM_DRAW);

	/*Compile the shaders */
	//const char* vert, geo, frag;
	MakeShaderProgram(OPENGL_VERTEX_SHADER, OPENGL_GEOMETRY_SHADER, OPENGL_FRAGMENT_SHADER, 0);
	//CreateStandardMatrices();
#endif //OPENGL
	return SUCCESS;
}
int deinitOpenGL()
{
#ifdef OPENGL
	glDeleteBuffers(1, &vertex_buffer_object);
#endif //OPENGL
	return SUCCESS;
}
int initOpenCL()
{
#ifdef OPENCL
#endif //OPENCL
	return SUCCESS;
}
int deinitOpenCL()
{
#ifdef OPENCL
#endif //OPENCL
	return SUCCESS;
}
int setupOpenGLRender()
{
#ifdef OPENGL
#endif //OPENGL
	return SUCCESS;
}
int closeOpenGLRender()
{
#ifdef OPENGL
#endif //OPENGL
	return SUCCESS;
}
/* just a helper function here for utility */
int SDLCheckError(int lineNum)
{
    const char* error = SDL_GetError();
    if ((*error) != '\0')
    {
        printf("SDL Error: %d: %s\n", lineNum, error);
        SDL_ClearError();
        return FAILURE;
    }
    return SUCCESS;
}
int initWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL failed to initialize\n");
		SDLCheckError(__LINE__);
		SDL_Quit();
		return FAILURE;
	}
	SDLCheckError(__LINE__);
	window = SDL_CreateWindow("Some Random Simulation", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, windowHeight, windowWidth,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	SDLCheckError(__LINE__);
	context = SDL_GL_CreateContext(window);
	SDLCheckError(__LINE__);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDLCheckError(__LINE__);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDLCheckError(__LINE__);

	int doubleBuffered;
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffered);
	swapFlag = doubleBuffered;
	SDLCheckError(__LINE__);
	return SUCCESS;
}
int deinitWindow()
{
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return SUCCESS;
}


/* Set the x and z components of the vertices, which will remain constant throughout the program */
int initVertices()
{
	/*for (int x = 0; x < NUM_VERTICES_X; x++)
	{
		for (int z = 0; z < NUM_VERTICES_Z; z++)
		{
			vertex_positions[x][z][0] = (double)x / NUM_VERTICES_X;
			vertex_positions[x][z][2] = (double)z / NUM_VERTICES_Z;
		}
	}*/
	long x = 0;
	while (x < NUM_VERTICES)
	{
		/*Set the block of X coords to a float between 0.0 and 1.0 */
		vertex_positions[x] = ((x % NUM_VERTICES_X) / (float)NUM_VERTICES_X);
		x++;
	}
	while (x < 2*NUM_VERTICES)
	{
		x++;
	}
	while (x < 3*NUM_VERTICES)
	{
		/*Set the block of Z coords to a float between 0.0 and 1.0
		* the integer divide is purposeful, and gets the proper value of z */
		vertex_positions[x] = ((x / NUM_VERTICES_Z) / (float)NUM_VERTICES_Z);
		x++;
	}
	return SUCCESS;
}

/*
This function will update the vertices to their new positions.
At each vertex, the new position is a function of the current time and the distance of the vertex from the center
*/
int updateVertices(int iteration)
{
	/* take a snapshot of the time before beginning. Any time will do */
	//time_t time = clock();
	double distanceFromCenter;
	//double time = (double)iteration / ITERATIONS;
	double time = (double)clock() / (double)CLOCKS_PER_SEC;
	/*for (int z = 0; z < NUM_VERTICES_Z; z++)
	{
		for (int x = 0; x < NUM_VERTICES_X; x++)
		{
			int dx = (x - centerPointX), dz = (z - centerPointZ);
			distanceFromCenter = sqrt(pow((double)dx,2) + pow((double)dz,2));
			//printf("relativeIterations: %f\tdistanceFromCenter: %f\ttotal: %f \n", (double)iteration/ITERATIONS, distanceFromCenter, (double)iteration/ITERATIONS + distanceFromCenter);
			vertex_positions[x][z][1] = amplitude*cos(omega*time + distanceFromCenter);
		}
	}*/
	for (int idx = 0; idx < NUM_VERTICES; idx++)
	{
		double dx = (vertex_positions[idx] - 0.5), dz = (vertex_positions[(idx + 2*NUM_VERTICES)] - 0.5);
		distanceFromCenter = sqrt(pow(dx,2) + pow(dz,2));
		vertex_positions[(idx + NUM_VERTICES)] = amplitude*cos(omega*time + distanceFromCenter);
	}
	return SUCCESS;
}

/* This just prints to the screen right now, but later it will be a whole bunch of opengl work */
int Render()
{
	/*for (int z = 0; z < NUM_VERTICES_Z; z++)
	{
		for (int x = 0; x < NUM_VERTICES_X; x++)
		{
			//printf(" (%f,%f,%f) ", vertex_positions[x][z][0], vertex_positions[x][z][1], vertex_positions[x][z][2]);
			printf(" %f ", vertex_positions[x][z][1]);
		}
		printf("\n");
	}*/
	for (int x = 0; x < NUM_VERTICES; x++)
	{
	}
	printf("\n\n");
	return SUCCESS;
}


int createVertexPositions()
{
	/*vertex_positions = (float***)malloc(NUM_VERTICES_X*sizeof(float**));
	if (vertex_positions)
	{
		for (int i = 0; i < NUM_VERTICES_X; i++)
		{
			vertex_positions[i] = (float**)malloc(NUM_VERTICES_Z*sizeof(float*));
			if (vertex_positions[i])
			{
				for (int j = 0; j < NUM_VERTICES_Z; j++)
				{
					vertex_positions[i][j] = (float*)malloc(3*sizeof(float));
					if (!vertex_positions[i][j])
						printf("out of memory\n");
				}
			}
			else
			{
				printf("out of memory\n");
				return FAILURE;
			}
		}
	}
	else
	{
		printf("out of memory\n");
		return FAILURE;
	}
	*/
	vertex_positions = (float*)malloc(3*NUM_VERTICES*sizeof(float));
	return vertex_positions? SUCCESS : FAILURE;
}
int deleteVertexPositions()
{
	/*for (int i = 0; i < NUM_VERTICES_X; i++)
	{
		for (int j = 0; j < NUM_VERTICES_Z; j++)
		{
			free(vertex_positions[i][j]);
		}
		free(vertex_positions[i]); vertex_positions[i] = NULL;
	}
	free(vertex_positions); vertex_positions = NULL;*/
	free(vertex_positions);
	return SUCCESS;
}
