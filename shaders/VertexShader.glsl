#version 130

//layout(location = 0) in vec3 vertexPosition;
uniform mat4 transformationMatrix;

void main()
{
	//gl_Position = vec4(vertexPosition, 1.0)*transformationMatrix;
	gl_Position = gl_Vertex*transformationMatrix;
}
