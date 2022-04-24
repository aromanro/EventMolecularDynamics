#pragma once

#include <glm.hpp>

#include "Program.h"

class BillboardGLProgram :
	public OpenGL::Program
{
public:
	BillboardGLProgram();
	virtual ~BillboardGLProgram();

	bool SetupShaders();

	// vertex shader parameters
	GLint matLocation;
	GLint modelMatLocation;

	// fragment shader parameters
	GLint alphaLoc;
	GLint textureLoc;

protected:
	void getUniformsLocations();
	bool SetupVertexShader();
	bool SetupFragmentShader();
};

