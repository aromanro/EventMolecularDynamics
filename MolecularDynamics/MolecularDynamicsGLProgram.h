#pragma once

#include <glm.hpp>

#include "Program.h"

class MolecularDynamicsGLProgram :
	public OpenGL::Program
{
public:
	MolecularDynamicsGLProgram();
	virtual ~MolecularDynamicsGLProgram();

	bool SetupShaders();


	struct Light {
		GLint lightDirPos;
		GLint attenPos;
		glm::vec3 lightPos;
		float atten;

		Light() : lightDirPos(0), attenPos(0), lightPos(0, 0, 0), atten(0) {}
	};

	std::vector<Light> lights;

	void SetupLights();

	// vertex shader parameters
	GLint matLocation;
	GLint scaleLocation;
	GLint modelPosLocation;

	// fragment shader uniform parameters
	GLint colorLocation;
	GLint viewPosLocation;

protected:
	void getUniformsLocations();
	bool SetupVertexShader();
	bool SetupFragmentShader();
};

