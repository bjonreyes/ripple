#include "OpenGLHelperFunctions.h"


MatrixSet::MatrixSet() :
m_theta(0.0), m_fi(0.0), m_twistAngle(0.0), m_zNear(0.5), m_zFar(10.0)
{
}

MatrixSet::~MatrixSet()
{
}

void MatrixSet::SetCameraPosition(glm::vec3 position)
{
	m_CameraPosition = position;
}

void MatrixSet::LookAt(glm::vec3 point)
{
}

glm::mat4 MatrixSet::GetFinalMatrix()
{
	return m_matrices.translation*m_matrices.upDown*m_matrices.leftRight*m_matrices.twist*m_matrices.perspective;
}



void MatrixSet::i_RefreshAllMatrices()
{
	i_RefreshVerticalRotation();
	i_RefreshHorizontalRotation();
	i_RefreshTwist();
	i_RefreshTranslation();
	// Perspective Not included because it rarely needs refreshing, and when it does you'll know
}

/*
* Each of these four functions endevour to re-build their respective matrices using the new
* angles and whatnot stored in the class. It would be more efficient in on-disk memory
* to have seperate functions for initializing the matrices and for editing them.
* It could be done with something as simple as an 'firstTime' boolean in the class,
* and an if statement inside the function, but that would not be as efficient as two functions
*/
void MatrixSet::i_RefreshVerticalRotation()
{
	glm::mat4* updown = &(m_matrices.upDown);
	float sine = std::sin(m_theta);
	float cosine = std::cos(m_theta);

	(*updown) = glm::mat4(0.0);
	(*updown)[0].x = 1.0;
	(*updown)[1].y = cosine;
	(*updown)[1].z = sine;
	(*updown)[2].y = -sine;
	(*updown)[2].z = cosine;
	(*updown)[3].w = 1.0;
}
void MatrixSet::i_RefreshHorizontalRotation()
{
	glm::mat4* leftright = &(m_matrices.leftRight);
	float sine = std::sin(m_fi);
	float cosine = std::cos(m_fi);

	(*leftright) = glm::mat4(0.0);
	(*leftright)[0].x = cosine;
	(*leftright)[0].z = -sine;
	(*leftright)[1].y = 1.0;
	(*leftright)[2].x = sine;
	(*leftright)[2].z = cosine;
	(*leftright)[3].w = 1.0;
}
void MatrixSet::i_RefreshTwist()
{
	glm::mat4* twist = &(m_matrices.twist);
	float sine = std::sin(m_twistAngle);
	float cosine = std::cos(m_twistAngle);

	(*twist) = glm::mat4(0.0);
	(*twist)[0].x = cosine;
	(*twist)[0].y = -sine;
	(*twist)[1].x = sine;
	(*twist)[1].y = cosine;
	(*twist)[2].z = 1.0;
	(*twist)[3].w = 1.0;
}
void MatrixSet::i_RefreshTranslation()
{
	glm::mat4* translation = &(m_matrices.translation);
	
	(*translation) = glm::mat4(0.0);
	(*translation)[0].x = m_xScale;
	(*translation)[1].y = m_yScale;
	(*translation)[2].z = 1.0;
	(*translation)[3].x = m_CameraPosition.x;
	(*translation)[3].y = m_CameraPosition.y;
	(*translation)[3].z = m_CameraPosition.z;
	(*translation)[3].w = 1.0;
}
//should only be called upon window resize
void MatrixSet::i_RefreshPerspective()
{
	glm::mat4* perspective = &(m_matrices.perspective);

	(*perspective) = glm::mat4(0.0);

	//there could be a parameter called a frustrum scale that could be multiplied by 0x and 1y, but I haven't found it useful in the past
	(*perspective)[0].x = g_windowHeight / (float)g_windowWidth;
	(*perspective)[1].y = 1.0;
	float difference = m_zNear - m_zFar;
	(*perspective)[2].z = -(m_zFar + m_zNear)/difference;
	(*perspective)[2].w = -1.0;
	(*perspective)[3].z = -(2*m_zNear*m_zFar)/difference;
}

/*
function: OGLErrorCheck
This function is for checking the OpenGL error flag.
Parameters:
    lineNumber: the line number that is displayed beside the error message
        -supposed to be __LINE__
Return Value: SUCCESS when no error. FAILURE when there is an error
*/
int OGLErrorCheck(int lineNumber)
{
    //get the last error from the state machine
    GLenum errorCode = glGetError();
    //output an message depending on what the error is
    switch (errorCode)
    {
        case GL_NO_ERROR:
            return SUCCESS;
            break;
        case GL_INVALID_ENUM:
            printf("%d: The enum value passed in was not a valid one\n", lineNumber);
            break;
        case GL_INVALID_VALUE:
            printf("%d: The numeric value passed in was not a valid one\n", lineNumber);
            break;
        case GL_INVALID_OPERATION:
            printf("%d: This operation is not allowed in the current state\n",lineNumber);
            break;
        case GL_STACK_OVERFLOW:
            printf("%d: This operation would cause a stack overflow\n", lineNumber);
            break;
        case GL_STACK_UNDERFLOW:
            printf("%d: This operation would cause a stack underflow\n", lineNumber);
            break;
        case GL_OUT_OF_MEMORY:
            printf("%d: There is no memory available for this operation\n", lineNumber);
            break;
        /*case GLU_INCOMPATIBLE_GL_VERSION:
            std::cout << lineNumber << ": The version of OpenGL is not compatible with this operation" << std::endl;
            break;*/ //put this back in if we include the glu library
        default:
        {
            //std::cout << lineNumber << ": " << gluErrorString(errorCode) << std::endl;
            printf("%d: Unknown error\n",lineNumber);
            break;
        }
    }
    return FAILURE;
}

/*
function: CompileShader
This function is for compiling a single shader. 
Parameters:
    eShaderType: GL_VERTEX_SHADER | GL_GEOMETRY_SHADER | GL_FRAGMENT_SHADER
        //if other types of shaders pop up, they will work as well without modification
    filename: a string object with the filename of the shader in it
Return Value: The identifier for the compiled shader
*/
GLuint CompileShader(GLenum eShaderType, const char* filename, int debugOption)
{
    if (debugOption) printf("Compilation of Shader: %s\n", filename);
    GLuint shader = glCreateShader(eShaderType);
    FILE* fin;
    fin = fopen(filename, "r");
    if (!fin)
    {
        printf("\tCould not open shader file %s\n", filename);
		return 0;
    }
    fseek(fin, 0, SEEK_END);
    int size = ftell(fin);
    rewind(fin);
    GLchar* buffer = (GLchar*)malloc(size);
    if (!buffer)
    {
        printf("Out of memory\n");
        return 0;
    }
    //copy the data
    fread(buffer, sizeof(GLchar), size, fin);
    fclose(fin);

    /*Now the actual compilation takes place */
    glShaderSource(shader, 1, (const GLchar**)&buffer, NULL);
    glCompileShader(shader);
    free(buffer);
    //get the compilation status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    //if it failed report the error message
    if (status == GL_FALSE)
    {
        //get the log length
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        //print the message
        GLchar *strInfoLog = (GLchar*)malloc(sizeof(GLchar)*(infoLogLength + 1));
        GLsizei useless;
        glGetShaderInfoLog(shader, infoLogLength, &useless, strInfoLog);

        printf("\tcompiling of %s failed\n\t%s\n", filename, strInfoLog);
        free(strInfoLog);
        return 0;
    }
    //print a success message
    else
    {
        if (debugOption) printf("\tCompiled Cleanly\n");
    }
    if (debugOption) printf("Finished Compilation of Shader: %s\n", filename);
    //check errors for fun
    OGLErrorCheck(__LINE__);
    return shader;
}

/*
function:CreateProgram
This function is to compile and link a full program from 3 filenames
Parameters:
    vertFileName: a pointer to the filename of the vertex shader(can be null)
    geoFileName:a pointer to the filename of the geometry shader(can be null)
    fragFileName: a pointer to the filename of the fragment shader(can be null)
Return Value: The identifier of the linked and compiled program
*/
GLint MakeShaderProgram(const char* vertFileName, const char* geoFileName, const char* fragFileName, int debugOption)
{
    GLint programID = glCreateProgram();

    //compile each of the shaders that isn't null
    if (vertFileName != '\0')
    {
        GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertFileName, debugOption);
        if (vertShader == 0)
            return 0;
        glAttachShader(programID, vertShader);
    }
    if (geoFileName != '\0')
    {
        GLuint geoShader = CompileShader(GL_GEOMETRY_SHADER, geoFileName, debugOption);
        if (geoShader == 0)
            return 0;
        glAttachShader(programID, geoShader);
    }
    if (fragFileName != '\0')
    {
        GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragFileName, debugOption);
        if (fragShader == 0)
            return 0;
        glAttachShader(programID, fragShader);
    }
    if (debugOption) printf("\tLinking Program\n");
    glLinkProgram(programID);

    //get the linking status
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    //if linking failed, output the error message
    if (status == GL_FALSE)
    {
        //we have to get the log length first
        GLint infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        //get the program log
        GLchar* infoLog = (GLchar*)malloc((sizeof(GLchar))*(infoLogLength + 1));
        (*infoLog) = '\0';
        glGetProgramInfoLog(programID, infoLogLength, NULL, infoLog);
        printf("\t%s\n", infoLog);
        free(infoLog);
    }
    //print an success message
    else if (debugOption)
    {
        printf("\tLinked Successfully\n");
    }

    //now set up the uniform locations
    /*matrixUniformLocation = glGetUniformLocation(programID, "finalMatrix");
    if (hasLighting)
    {
        wcLightLocationUniformLocation = glGetUniformLocation(programID, "wcLightLocation");
        normalTransformMatrixUniformLocation = glGetUniformLocation(programID, "normalTransformMatrix");
    }
    else
    {
        wcLightLocationUniformLocation = 0;
        normalTransformMatrixUniformLocation = 0;
    }*/ //I'm not sure if this has any value, but I'll keep it here for a bit to find out
    return programID;
}

