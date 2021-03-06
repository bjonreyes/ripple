#include "ripple.h"
#include "OpenGLHelperFunctions.h"

/*Project Purpose:
* This will be a shot of a surface that bounces up and down in a sine wave propogating outwards in a ripple fashion
* The purpose of this project is to facilitate the learning of OpenCL and practising openGL
*/

#define OPENGL_VERTEX_SHADER "shaders/VertexShader.glsl"
#define OPENGL_GEOMETRY_SHADER 0
#define OPENGL_FRAGMENT_SHADER "shaders/FragmentShader.glsl"

/* major functions */
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
int constructElementArray();
int deleteElementArray();
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
int g_windowWidth = 600;
int g_windowHeight = 800;
int swapFlag;

/* OpenGL global vars */
#ifdef OPENGL
GLint programID;
GLuint vertex_buffer_object;
MatrixSet g_matrix;
GLint matrixUniformLocation;
GLushort* indexArray;
#endif


/* beginning of program */
int main()
{
	assert(createVertexPositions() == SUCCESS);
	/* initialize opengl */
	assert(initWindow() == SUCCESS);
	assert(glewInit() == GLEW_OK);
	assert(initOpenCL() == SUCCESS);
	assert(initOpenGL() == SUCCESS);

	assert(initVertices() == SUCCESS);

	/* loop for ten thousand iterations */
	long i = 0;
	while (i < ITERATIONS)
	{
		/* update the vertices */
		assert(updateVertices(i) == SUCCESS);
		
		assert(setupOpenGLRender() == SUCCESS);
		/* render the new scene */
		assert(Render() == SUCCESS);
		assert(closeOpenGLRender() == SUCCESS);
		sleep(1);
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
	// Generate the buffer that will store the vertices
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*NUM_VERTICES_X*NUM_VERTICES_Z*3, vertex_positions, GL_STREAM_DRAW);
	//testing
	//float test_buffer[] = { 0.75, 0.75, 0.0, 0.75, 0.25, 0.0, 0.25, 0.25, 0.0};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9, test_buffer, GL_STATIC_DRAW);

	//Compile the shaders
	programID = MakeShaderProgram(OPENGL_VERTEX_SHADER, OPENGL_GEOMETRY_SHADER, OPENGL_FRAGMENT_SHADER, 1);
	if (!programID) return FAILURE;

	//register the uniform variables
	matrixUniformLocation = glGetUniformLocation(programID, "transformationMatrix");
	OGLErrorCheck(__LINE__);
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
	//Set the appropriate uniform variables
	glUseProgram(programID);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	g_matrix.SetCameraPosition(glm::vec3(-1.0, 0.2, 1.0));
	glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, glm::value_ptr(g_matrix.GetFinalMatrix()));

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	OGLErrorCheck(__LINE__);
#endif //OPENGL
	return SUCCESS;
}
int closeOpenGLRender()
{
#ifdef OPENGL
	//glDisableVertexAttribArray(0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
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
		SDL_WINDOWPOS_CENTERED, g_windowHeight, g_windowWidth,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	SDLCheckError(__LINE__);
	context = SDL_GL_CreateContext(window);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
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
	//This one is useful for creating 3 blocks, one of X, one of Y, and one of Z
	/*while (x < NUM_VERTICES)
	{
		//Set the block of X coords to a float between 0.0 and 1.0
		vertex_positions[x] = ((x % NUM_VERTICES_X) / (float)NUM_VERTICES_X);
		x++;
	}
	while (x < 2*NUM_VERTICES)
	{
		x++;
	}
	while (x < 3*NUM_VERTICES)
	{
		//Set the block of Z coords to a float between 0.0 and 1.0
		//the integer divide is purposeful, and gets the proper value of z
		vertex_positions[x] = ((x / NUM_VERTICES_Z) / (float)NUM_VERTICES_Z);
		x++;
	}*/
	/*while (x < NUM_VERTICES*3)
	{
		vertex_positions[x] = ((x % NUM_VERTICES_X) / 3) / (float)NUM_VERTICES_X; 
		x++;
		vertex_positions[x] = 0.0;
		x++;
		vertex_positions[x] = (x / NUM_VERTICES_X) / (float)NUM_VERTICES_Z;
		x++;
	}*/
	for (long z = 0; z < NUM_VERTICES_Z; z++)
	{
		float zScaled = z / NUM_VERTICES_Z;
		for (long x = 0; x < NUM_VERTICES_X; x++)
		{
			float xScaled = x / NUM_VERTICES_X;
			int idx = z*x*3;
			vertex_positions[idx] = xScaled;
			vertex_positions[(idx + 1)] = 0.0;
			vertex_positions[(idx + 2)] = zScaled;
		}
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
	double time = (double)iteration / ITERATIONS;
	//double time = (double)clock() / (double)CLOCKS_PER_SEC;
	double centerPointX = NUM_VERTICES_X / 2;
	double centerPointZ = NUM_VERTICES_Z / 2;
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
	/*for (int idx = 0; idx < NUM_VERTICES; idx++)
	{
		double dx = (vertex_positions[idx] - 0.5), dz = (vertex_positions[(idx + 2*NUM_VERTICES)] - 0.5);
		distanceFromCenter = sqrt(pow(dx,2) + pow(dz,2));
		vertex_positions[(idx + NUM_VERTICES)] = amplitude*cos(omega*time + distanceFromCenter);
	}*/
	for (long z = 0; z < NUM_VERTICES_Z; z++)
	{
		double dz = (z - centerPointZ) / NUM_VERTICES_Z;
		for (long x = 0; x < NUM_VERTICES_X; x++)
		{
			double dx = (x - centerPointX) / NUM_VERTICES_X;
			double distanceFromCenter = sqrt(pow(dx,2) + pow(dz,2));
			vertex_positions[(x*z*3+1)] = amplitude * cos(omega*time + distanceFromCenter);
		}
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
	/*for (int x = 0; x < NUM_VERTICES; x++)
	{
	}*/
	for (int z = 0; z < NUM_VERTICES_Z; z++)
	{
		for (int x = 0; x < NUM_VERTICES_X; x++)
		{
			printf(" %f ", vertex_positions[(x*z*3+1)]);
		}
		printf("\n");
	}
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_LINE_STRIP, 0, NUM_VERTICES);
	//glDrawArrays(GL_TRIANGLES, 0, 3);//testing
	if (swapFlag)
		SDL_GL_SwapWindow(window);
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

int constructElementArray()
{
	//the index array will break the vertices into n - 1 * n - 1 squares that each form 2 triangles(or 6 indexes)
	int idx = 0;
	indexArray = (GLushort*)malloc(sizeof(GLushort)*(NUM_VERTICES_X - 1)*(NUM_VERTICES_Z - 1)*6);

	for (int z = 0; z < NUM_VERTICES_Z - 1; z++)
	{
		for (int x = 0; x < NUM_VERTICES_X - 1; x++)
		{
			idx++;
		}
	}
	return SUCCESS;
}
int deleteElementArray()
{
	free(indexArray);
	return SUCCESS;
}
