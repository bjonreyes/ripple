#include "ripple.h"



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
    if (debugOption) printf("Compilation of Shader: %s", filename);
    GLuint shader = glCreateShader(eShaderType);
    FILE* fin;
    fin = fopen(filename, "r");
    if (!fin)
    {
        printf("\tCould not open shader file %s", filename);
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
        GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertFileName, 0);
        if (vertShader == 0)
            return 0;
        glAttachShader(programID, vertShader);
    }
    if (geoFileName != '\0')
    {
        GLuint geoShader = CompileShader(GL_GEOMETRY_SHADER, geoFileName, 0);
        if (geoShader == 0)
            return 0;
        glAttachShader(programID, geoShader);
    }
    if (fragFileName != '\0')
    {
        GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragFileName, 0);
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

