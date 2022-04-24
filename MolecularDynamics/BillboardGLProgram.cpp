#include "stdafx.h"
#include "BillboardGLProgram.h"
#include "MolecularDynamics.h"

#define GLSL(src) "#version 330\n" #src

BillboardGLProgram::BillboardGLProgram()
	: matLocation(0), modelMatLocation(0), 
	textureLoc(0)
{
}


BillboardGLProgram::~BillboardGLProgram()
{
}


void BillboardGLProgram::getUniformsLocations()
{
	// vertex shader parameters
	matLocation = glGetUniformLocation(getID(), "transformMat");
	modelMatLocation = glGetUniformLocation(getID(), "modelMat");

	// fragment shader uniform parameters
	alphaLoc = glGetUniformLocation(getID(), "alpha");
	textureLoc = glGetUniformLocation(getID(), "Texture");
}


bool BillboardGLProgram::SetupShaders()
{
	if (!SetupVertexShader()) return false;
	else if (!SetupFragmentShader()) return false;

	Bind();

	if (getStatus() == false)
	{
		AfxMessageBox(CString("Shaders compile: ") + CString(getStatusMessage()));
		theApp.PostThreadMessage(WM_QUIT, 0, 0);
		return false;
	}

	getUniformsLocations();

	return true;
}


bool BillboardGLProgram::SetupVertexShader()
{
	OpenGL::VertexShader vertexShader;

	vertexShader.setSource(GLSL(
		layout(location = 0) in vec3 position;
		layout(location = 1) in vec3 normal; // not used
		layout(location = 2) in vec2 texCoord;

		uniform mat4 transformMat;
		uniform mat4 modelMat;

		out vec2 TexCoord;

		void main()
		{
			gl_Position = transformMat * modelMat * vec4(position.x, position.y, position.z, 1.0);
			TexCoord = texCoord;
		}
	));

	if (vertexShader.getStatus() == false)
	{
		AfxMessageBox(CString("Vertex shader: ") + CString(vertexShader.getStatusMessage()));
		theApp.PostThreadMessage(WM_QUIT, 0, 0);
		return false;
	}

	Attach(vertexShader);

	return true;
}


bool BillboardGLProgram::SetupFragmentShader()
{
	OpenGL::FragmentShader fragmentShader;

	fragmentShader.setSource(GLSL(
		uniform float alpha;

		in vec2 TexCoord;
		out vec4 outputColor;

		uniform sampler2D Texture;

		void main()
		{
			vec4 color = texture(Texture, TexCoord);

			outputColor = vec4(color.xyz, alpha);
		}
	));


	if (fragmentShader.getStatus() == false)
	{
		AfxMessageBox(CString("Fragment shader: ") + CString(fragmentShader.getStatusMessage()));
		theApp.PostThreadMessage(WM_QUIT, 0, 0);
		return false;
	}

	Attach(fragmentShader);

	return true;
}

