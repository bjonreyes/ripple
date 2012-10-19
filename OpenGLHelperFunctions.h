#ifndef OPENGL_HELPER_HEADER_INCLUDE
#define OPENGL_HELPER_HEADER_INCLUDE
#include "CommonDefines.h"
#include <GL/glew.h>
#include <GL/gl.h>

typedef struct
{
	glm::mat4 upDown;
	glm::mat4 leftRight;
	glm::mat4 twist;
	glm::mat4 translation;
	glm::mat4 perspective;
	glm::mat4 final;
} stMatrices;

/*
* This class is the data and operations necessary for a matrix set that needs sending to the OpenGL shaders.
* Each vertex will be multiplied by this final matrix, but the actual render function will be done elsewhere.
* The only dependancy of this class will be glm, and not opengl itself.
*/
class MatrixSet
{
public:
	MatrixSet();
	~MatrixSet();
	void SetCameraPosition(glm::vec3 position);
	void LookAt(glm::vec3 point);
	glm::mat4 GetFinalMatrix();

private:
	//data (the structure that holds all of the matrices necessary for this operation)
	stMatrices m_matrices;
	glm::vec3 m_CameraPosition;
	float m_xScale, m_yScale; //translation scale
	double m_theta; //angle around the center and fi, the angle up
	double m_fi; //the angles that the world is rotated by to match the camera in radians
	double m_twistAngle;
	float m_zNear, m_zFar; //perspective stuff

	//private functions (to operate on the data set out above)
	void i_RefreshAllMatrices();
	void i_RefreshVerticalRotation();
	void i_RefreshHorizontalRotation();
	void i_RefreshTwist();
	void i_RefreshTranslation();
	void i_RefreshPerspective();
};

//other opengl helper functions
int OGLErrorCheck(int lineNumber);
GLuint CompileShader(GLenum eShaderType, const char* filename, int debugOption);
GLint MakeShaderProgram(const char* vertFileName, const char* geoFileName, const char* fragFileName, int debugOption);

#endif //OPENGL_HELPER_HEADER_INCLUDE
