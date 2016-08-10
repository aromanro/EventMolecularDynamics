#include "stdafx.h"
#include "MolecularDynamicsGLProgram.h"

#include "MolecularDynamics.h"

#define GLSL(src) "#version 330\n" #src

MolecularDynamicsGLProgram::MolecularDynamicsGLProgram()
{
}


MolecularDynamicsGLProgram::~MolecularDynamicsGLProgram()
{
}


void MolecularDynamicsGLProgram::SetupLights()
{
	USES_CONVERSION;

	// lights uniforms positions

	int lightno = 0;
	for (auto &light : lights)
	{
		CString param;
		param.Format(L"Lights[%d].lightDir", lightno);
		light.lightDirPos = glGetUniformLocation(getID(), W2A(param));

		param.Format(L"Lights[%d].atten", lightno);
		light.attenPos = glGetUniformLocation(getID(), W2A(param));

		++lightno;
	}
}

void MolecularDynamicsGLProgram::getUniformsLocations()
{
	// vertex shader parameters
	matLocation = glGetUniformLocation(getID(), "transformMat");
	scaleLocation = glGetUniformLocation(getID(), "scaleModel");
	modelPosLocation = glGetUniformLocation(getID(), "modelPos");

	// fragment shader uniform parameters
	colorLocation = glGetUniformLocation(getID(), "theColor");	
	viewPosLocation = glGetUniformLocation(getID(), "viewPos");
}


bool MolecularDynamicsGLProgram::SetupShaders()
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


bool MolecularDynamicsGLProgram::SetupVertexShader()
{
	OpenGL::VertexShader vertexShader;

	vertexShader.setSource(GLSL(

	layout(location = 0) in vec3 position;
	layout(location = 1) in vec3 normal;
	
	layout(location = 2) in vec3 center;
	layout(location = 3) in vec3 color;
	layout(location = 4) in float scale;

	uniform mat4 transformMat;
	
	out vec3 Normal;
	out vec3 FragPos;
	out vec3 theColor;

	void main()
	{
		mat4 mm = mat4(
			vec4(scale, 0.0, 0.0, 0.0),
			vec4(0.0, scale, 0.0, 0.0),
			vec4(0.0, 0.0, scale, 0.0),
			vec4(center.x, center.y, center.z, 1.0)
		);
		
		gl_Position = transformMat * mm * vec4(position.x, position.y, position.z, 1.0);

		Normal = normalize(transpose(inverse(mat3(mm))) * normal);
		
		FragPos = vec3(mm * vec4(position, 1.0));

		theColor = color;
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


bool MolecularDynamicsGLProgram::SetupFragmentShader()
{
	OpenGL::FragmentShader fragmentShader;

	CString shaderSrc;
	shaderSrc.Format(CString(GLSL(
		\n#define NRLIGHTS %d\n

		struct Light {
		vec3 lightDir;
		float atten;
	};
	uniform Light Lights[NRLIGHTS];

	//uniform vec4 theColor;
	
	uniform vec3 viewPos;
	uniform vec3 lightPos;

	in vec3 Normal;
	in vec3 FragPos;
	in vec3 theColor;

	out vec4 outputColor;

	vec3 CalcLight(in vec3 lightDir, in vec3 viewDir, in vec3 normal, in vec3 color)
	{
		// Diffuse shading
		float diff = max(dot(normal, lightDir), 0.0);

		// Specular shading
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);

		// Combine results								
		return (0.5 * diff + 0.3 * spec) * color;
	}

	void main()
	{	
		vec3 viewVec = viewPos - FragPos;
		vec3 viewDir = normalize(viewVec);
		vec3 normal = normalize(Normal);

		// ambient
		vec3 light = 0.2 * theColor;

		for (int i = 0; i < NRLIGHTS; ++i)
		{
			light += Lights[i].atten * CalcLight(normalize(Lights[i].lightDir), viewDir, normal, theColor);
		}

	    outputColor = vec4(light, 1);
	}

	)), lights.size());

	USES_CONVERSION;
	fragmentShader.setSource(W2A(shaderSrc));


	if (fragmentShader.getStatus() == false)
	{
		AfxMessageBox(CString("Fragment shader: ") + CString(fragmentShader.getStatusMessage()));
		theApp.PostThreadMessage(WM_QUIT, 0, 0);
		return false;
	}

	Attach(fragmentShader);

	return true;
}
