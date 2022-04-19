#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MolecularDynamics.h"
#endif

#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries

#include <glm.hpp>


#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>


#include "Camera.h"

#include "Program.h"
#include "Sphere.h"

#include "MolecularDynamicsDoc.h"
#include "MolecularDynamicsView.h"

#include "MainFrm.h"
#include "MFCToolBarSlider.h"

#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CMolecularDynamicsView::ClearProgram()
{
	delete program;
	program = nullptr;
}


void CMolecularDynamicsView::ClearBillboardProgram()
{
	delete billboardProgram;
	billboardProgram = nullptr;
}


void CMolecularDynamicsView::Setup()
{
	if (inited) return;

	SetupGl();

	const double billboardAspectRatio = 2;
	billboardRectangle = new OpenGL::Rectangle(billboardAspectRatio);

	const int height = 1024;
	memoryBitmap.SetSize(static_cast<int>(billboardAspectRatio * height), height);

	if (!font.GetSafeHandle())
	{
		const int fontSize = static_cast<int>(height * 0.8);
		const int fontHeight = -MulDiv(fontSize, CDC::FromHandle(::GetDC(NULL))->GetDeviceCaps(LOGPIXELSY), 72);
		font.CreateFont(fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("Courier New"));
	}

	billboardTexture = new OpenGL::Texture();

	SetupSpheres();

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	SetupProgram();

	SetupBillboardProgram();

	//SetBillboardText("Test!");
	
	chart.title = L"Speed distribution";
	chart.XAxisLabel = L"Speed";
	chart.YAxisLabel = L"Number (normalized)";
	chart.useSpline = true;
	chart.antialias = true;
	chart.YAxisMax = chart.XAxisMin = 0;


	if (theApp.options.showBillboard)
		PaintBillboarChart();

	wglMakeCurrent(NULL, NULL);

	if (!timer) timer = SetTimer(1, 30, NULL);

	inited = true;
}

void CMolecularDynamicsView::SetupGl()
{
	if (!m_hRC)
	{
		m_hRC = wglCreateContext(m_hDC);
		VERIFY(wglMakeCurrent(m_hDC, m_hRC));

		glewInit();

		glClearColor(0, 0, 0, 0);

		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		EnableAntialias();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_RANGE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		glShadeModel(GL_SMOOTH);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		if (theApp.options.gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glMatrixMode(GL_MODELVIEW);

		CRect rect;
		GetClientRect(rect);
		Height = rect.Height();
		Width = rect.Width();
	}
	else {
		wglMakeCurrent(m_hDC, m_hRC);

		if (theApp.options.gammaCorrection) glEnable(GL_FRAMEBUFFER_SRGB);
		else glDisable(GL_FRAMEBUFFER_SRGB);
	}
}


void CMolecularDynamicsView::SetupSpheres()
{
	sphere = new OpenGL::Sphere(1, 32, 32, false);

	CMolecularDynamicsDoc* doc = GetDocument();

	if (doc)
	{
		position.resize(doc->nrParticles * 3ULL);
		colors.resize(doc->nrParticles * 3ULL);
		scale.resize(doc->nrParticles);
		int index = 0;
		const double avgRadius = (doc->options.exteriorSpheresRadius + doc->options.interiorSpheresRadius) / 2.;

		for (const auto& particle : doc->curResult.particles)
		{
			scale[index] = static_cast<GLfloat>(particle.radius);

			colors[3ULL * index] = static_cast<float>((particle.radius > avgRadius ? GetRValue(theApp.options.bigSphereColor) : GetRValue(theApp.options.smallSphereColor)) / 255.);
			colors[3ULL * index + 1ULL] = static_cast<float>((particle.radius > avgRadius ? GetGValue(theApp.options.bigSphereColor) : GetGValue(theApp.options.smallSphereColor)) / 255.);
			colors[3ULL * index + 2ULL] = static_cast<float>((particle.radius > avgRadius ? GetBValue(theApp.options.bigSphereColor) : GetBValue(theApp.options.smallSphereColor)) / 255.);

			++index;
		}
	}

	posBuffer = new OpenGL::VertexBufferObject();
	if (doc) {
		glEnableVertexAttribArray(2);
		posBuffer->setData(NULL, doc->nrParticles * 3 * sizeof(GLfloat), GL_STREAM_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	}

	colorBuffer = new OpenGL::VertexBufferObject();
	if (doc) {
		glEnableVertexAttribArray(3);
		colorBuffer->setData(colors.data(), doc->nrParticles * 3 * sizeof(GLfloat));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	}

	scaleBuffer = new OpenGL::VertexBufferObject();
	if (doc) {
		glEnableVertexAttribArray(4);
		scaleBuffer->setData(scale.data(), doc->nrParticles * sizeof(GLfloat));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), 0);
	}
}


void CMolecularDynamicsView::SetupProgram()
{
	if (!SetupShaders()) {
		ClearProgram();
		return;
	}
	program->DetachShaders();
	program->SetupLights();
}



bool CMolecularDynamicsView::SetupShaders()
{
	program = new MolecularDynamicsGLProgram();

	program->lights.emplace_back(MolecularDynamicsGLProgram::Light());
	program->lights.back().lightPos = glm::vec3(-0.3, 0.8, 0.5);
	program->lights.back().atten = 0.8f;

	return program->SetupShaders();
}

void CMolecularDynamicsView::SetColors()
{
	CMolecularDynamicsDoc* doc = GetDocument();

	if (doc)
	{
		int index = 0;
		const double avgRadius = (doc->options.exteriorSpheresRadius + doc->options.interiorSpheresRadius) / 2;

		for (const auto& particle : doc->curResult.particles)
		{
			colors[3ULL * index] = static_cast<float>((particle.radius > avgRadius ? GetRValue(theApp.options.bigSphereColor) : GetRValue(theApp.options.smallSphereColor)) / 255.);
			colors[3ULL * index + 1] = static_cast<float>((particle.radius > avgRadius ? GetGValue(theApp.options.bigSphereColor) : GetGValue(theApp.options.smallSphereColor)) / 255.);
			colors[3ULL * index + 2] = static_cast<float>((particle.radius > avgRadius ? GetBValue(theApp.options.bigSphereColor) : GetBValue(theApp.options.smallSphereColor)) / 255.);

			++index;
		}

		if (inited)	wglMakeCurrent(m_hDC, m_hRC);

		delete colorBuffer;
		colorBuffer = new OpenGL::VertexBufferObject();

		glEnableVertexAttribArray(3);
		colorBuffer->setData(colors.data(), doc->nrParticles * 3 * sizeof(GLfloat));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glVertexAttribDivisor(3, 1);

		wglMakeCurrent(NULL, NULL);
	}
}

void CMolecularDynamicsView::SetupBillboardProgram()
{
	if (!SetupBillboardShaders()) {
		ClearBillboardProgram();
		return;
	}
	billboardProgram->DetachShaders();
}



bool CMolecularDynamicsView::SetupBillboardShaders()
{
	billboardProgram = new BillboardGLProgram();

	return billboardProgram->SetupShaders();
}

void CMolecularDynamicsView::Reset()
{
	delete sphere;
	sphere = nullptr;

	delete posBuffer;
	posBuffer = nullptr;
	delete colorBuffer;
	colorBuffer = nullptr;
	delete scaleBuffer;
	scaleBuffer = nullptr;

	delete billboardRectangle;
	billboardRectangle = NULL;

	delete billboardTexture;
	billboardTexture = NULL;

	ClearProgram();
	ClearBillboardProgram();

	inited = false;

	Setup();
}

void CMolecularDynamicsView::EnableAntialias()
{
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_DITHER);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_MULTISAMPLE_ARB);
}


void CMolecularDynamicsView::DisableAntialias()
{
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_DITHER);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_MULTISAMPLE_ARB);
}

