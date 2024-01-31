#pragma once

#include <glm.hpp>

#include "Program.h"

class MolecularDynamicsGLProgram :
	public OpenGL::Program
{
public:
	MolecularDynamicsGLProgram();
	~MolecularDynamicsGLProgram() override;

	bool SetupShaders();


	struct Light {
		GLint lightDirPos = 0;
		GLint attenPos = 0;
		glm::vec3 lightPos{0,0,0};
		float atten = 0.;
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

