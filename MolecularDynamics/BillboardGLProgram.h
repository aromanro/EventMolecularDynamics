#pragma once

#include <glm.hpp>

#include "Program.h"

class BillboardGLProgram :
	public OpenGL::Program
{
public:
	bool SetupShaders();

	// vertex shader parameters
	GLint matLocation = 0;
	GLint modelMatLocation = 0;

	// fragment shader parameters
	GLint alphaLoc = 0;
	GLint textureLoc = 0;

protected:
	void getUniformsLocations();
	bool SetupVertexShader();
	bool SetupFragmentShader();
};

