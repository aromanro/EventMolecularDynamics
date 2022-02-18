
// MolecularDynamicsView.cpp : implementation of the CMolecularDynamicsView class
//

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



// CMolecularDynamicsView

IMPLEMENT_DYNCREATE(CMolecularDynamicsView, CView)

BEGIN_MESSAGE_MAP(CMolecularDynamicsView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMolecularDynamicsView construction/destruction

CMolecularDynamicsView::CMolecularDynamicsView()
	: timer(NULL), sphere(NULL), inited(false),
	keyDown(false), ctrl(false), shift(false), wheelAccumulator(0),
	movement(OpenGL::Camera::Movements::noMove), m_hRC(0), m_hDC(0),
	Width(0), Height(0), program(nullptr),
	posBuffer(nullptr), colorBuffer(nullptr), scaleBuffer(nullptr)
{
	// TODO: add construction code here
}

CMolecularDynamicsView::~CMolecularDynamicsView()
{
	delete sphere;
	ClearProgram();

	delete posBuffer;
	delete colorBuffer;
	delete scaleBuffer;
}

BOOL CMolecularDynamicsView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	cs.style &= ~WS_BORDER;
	if (!CView::PreCreateWindow(cs)) return FALSE;

	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	const wchar_t *className = L"OpenGLClass";
	if (!(::GetClassInfo(hInst, className, &wndcls)))
	{
		if (::GetClassInfo(hInst, cs.lpszClass, &wndcls))
		{
			wndcls.lpszClassName = className;
			wndcls.style |= CS_OWNDC;
			wndcls.hbrBackground = NULL;

			if (!AfxRegisterClass(&wndcls))
				return FALSE;
		}
		else return FALSE;
	}

	return TRUE;
}

// CMolecularDynamicsView drawing

void CMolecularDynamicsView::OnDraw(CDC* /*pDC*/)
{
	if (inited)
	{
		wglMakeCurrent(m_hDC, m_hRC);

		Resize(Height, Width);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderScene();

		glFlush();
		SwapBuffers(m_hDC);
		wglMakeCurrent(m_hDC, NULL);
	}
}

void CMolecularDynamicsView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	// TODO: Implement something here if needed
}

void CMolecularDynamicsView::OnContextMenu(CWnd* /* pWnd */, CPoint /*point*/)
{
#ifndef SHARED_HANDLERS
#endif
}


// CMolecularDynamicsView diagnostics

#ifdef _DEBUG
void CMolecularDynamicsView::AssertValid() const
{
	CView::AssertValid();
}

void CMolecularDynamicsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMolecularDynamicsDoc* CMolecularDynamicsView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMolecularDynamicsDoc)));
	return dynamic_cast<CMolecularDynamicsDoc*>(m_pDocument);
}
#endif //_DEBUG


// CMolecularDynamicsView message handlers


int CMolecularDynamicsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_hDC = ::GetDC(m_hWnd);

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,0,0,0,0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,0,0,0 };

	const int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, nPixelFormat, &pfd);

	return 0;
}


void CMolecularDynamicsView::OnTimer(UINT_PTR nIDEvent)
{
	if (inited)
	{
		CMolecularDynamicsDoc *doc = GetDocument();
		if (doc) {
			doc->simulationTime = doc->simulationTime + doc->nrsteps * timeStep;

			doc->Advance();
		}

		camera.Tick();
		if (keyDown) camera.Move(movement);

		Invalidate();
	}

	CView::OnTimer(nIDEvent);
}


void CMolecularDynamicsView::OnDestroy()
{
	if (inited)
	{
		wglDeleteContext(m_hRC);
		::ReleaseDC(m_hWnd, m_hDC);

		KillTimer(timer);
	}

	CView::OnDestroy();
}


void CMolecularDynamicsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (inited)
	{
		Height = cy;
		Width = cx;
	}
}


BOOL CMolecularDynamicsView::OnEraseBkgnd(CDC* /*pDC*/)
{
	return FALSE;
}


// Initializes the CPalette object
void CMolecularDynamicsView::InitializePalette(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	BYTE RedRng = 0, GreenRng = 0, BlueRng = 0;

	const int nPixelFormat = GetPixelFormat(m_hDC);
	DescribePixelFormat(m_hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (!(pfd.dwFlags & PFD_NEED_PALETTE)) return;

	const WORD nColors = static_cast<WORD>(1 << pfd.cColorBits);

	LOGPALETTE *pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	if (pPal)
	{
		pPal->palVersion = 0x300;
		pPal->palNumEntries = nColors;

		RedRng = static_cast<BYTE>((1 << pfd.cRedBits) - 1);
		GreenRng = static_cast<BYTE>((1 << pfd.cGreenBits) - 1);
		BlueRng = static_cast<BYTE>((1 << pfd.cBlueBits) - 1);

		for (unsigned int i = 0; i < nColors; i++)
		{
			pPal->palPalEntry[i].peRed = static_cast<BYTE>((i >> pfd.cRedShift) & RedRng);
			pPal->palPalEntry[i].peRed = static_cast<unsigned char>(static_cast<double>(pPal->palPalEntry[i].peRed) * 255.0 / RedRng);
			pPal->palPalEntry[i].peGreen = static_cast<BYTE>((i >> pfd.cGreenShift) & GreenRng);
			pPal->palPalEntry[i].peGreen = static_cast<unsigned char>(static_cast<double>(pPal->palPalEntry[i].peGreen) * 255.0 / GreenRng);
			pPal->palPalEntry[i].peBlue = static_cast<BYTE>((i >> pfd.cBlueShift) & BlueRng);
			pPal->palPalEntry[i].peBlue = static_cast<unsigned char>(static_cast<double>(pPal->palPalEntry[i].peBlue) * 255.0 / BlueRng);
			pPal->palPalEntry[i].peFlags = 0;
		}

		m_GLPalette.CreatePalette(pPal);
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);
		RealizePalette(m_hDC);

		free(pPal);
	}
}

BOOL CMolecularDynamicsView::OnQueryNewPalette()
{
	if ((HPALETTE)m_GLPalette)
	{
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);
		const unsigned int nRet = RealizePalette(m_hDC);
		InvalidateRect(NULL, FALSE);

		return nRet ? TRUE : FALSE;
	}

	return CView::OnQueryNewPalette();
}


void CMolecularDynamicsView::OnPaletteChanged(CWnd* pFocusWnd)
{
	if (((HPALETTE)m_GLPalette != NULL) && (pFocusWnd != this))
	{
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);
		RealizePalette(m_hDC);
		UpdateColors(m_hDC);
		return;
	}

	CView::OnPaletteChanged(pFocusWnd);
}


BOOL CMolecularDynamicsView::PreTranslateMessage(MSG* pMsg)
{
	if (!KeyPressHandler(pMsg)) return CView::PreTranslateMessage(pMsg);

	return TRUE;
}


bool CMolecularDynamicsView::KeyPressHandler(MSG* pMsg)
{
	bool handled = false;

	if (pMsg->message == WM_KEYDOWN)
	{
		keyDown = true;
		ctrl = ((::GetKeyState(VK_CONTROL) & 0x8000) != 0 ? true : false);
		shift = ((::GetKeyState(VK_SHIFT) & 0x8000) != 0 ? true : false);

		// pMSG->wParam contains the key code
		handled = HandleKeyPress(pMsg->wParam, ctrl, shift);

		Invalidate();
	}
	else if (pMsg->message == WM_KEYUP) keyDown = false;

	return handled;
}

bool CMolecularDynamicsView::HandleKeyPress(WPARAM wParam, bool ctrl, bool shift)
{
	bool handled = false;

	switch (wParam)
	{
	case VK_UP:
		HandleUp(ctrl, shift);
		handled = true;
		break;
	case VK_DOWN:
		HandleDown(ctrl, shift);
		handled = true;
		break;
	case VK_LEFT:
		HandleLeft(ctrl, shift);
		handled = true;
		break;
	case VK_RIGHT:
		HandleRight(ctrl, shift);
		handled = true;
		break;
	case VK_ADD:
	case VK_OEM_PLUS:
	{
		CMolecularDynamicsDoc* doc = GetDocument();
		if (doc->nrsteps < MAX_NRSTEPS)
		{
			++doc->nrsteps;
			CMFCToolBarSlider::SetPos(ID_SLIDER, (int)doc->nrsteps);
		}
	}
	handled = true;
	break;
	case VK_SUBTRACT:
	case VK_OEM_MINUS:
	{
		CMolecularDynamicsDoc* doc = GetDocument();
		if (doc->nrsteps > 1)
		{
			--doc->nrsteps;
			CMFCToolBarSlider::SetPos(ID_SLIDER, (int)doc->nrsteps);
		}
	}
	handled = true;
	break;
	default:
		movement = OpenGL::Camera::Movements::noMove;
	}

	return handled;
}

void CMolecularDynamicsView::HandleUp(bool ctrl, bool shift)
{
	if (ctrl) movement = OpenGL::Camera::Movements::pitchUp;
	else if (shift) movement = OpenGL::Camera::Movements::moveUp;
	else movement = OpenGL::Camera::Movements::moveForward;
}

void CMolecularDynamicsView::HandleDown(bool ctrl, bool shift)
{
	if (ctrl) movement = OpenGL::Camera::Movements::pitchDown;
	else if (shift) movement = OpenGL::Camera::Movements::moveDown;
	else movement = OpenGL::Camera::Movements::moveBackward;
}

void CMolecularDynamicsView::HandleLeft(bool ctrl, bool shift)
{
	if (ctrl) movement = OpenGL::Camera::Movements::yawLeft;
	else if (shift) movement = OpenGL::Camera::Movements::rollLeft;
	else movement = OpenGL::Camera::Movements::moveLeft;
}

void CMolecularDynamicsView::HandleRight(bool ctrl, bool shift)
{
	if (ctrl) movement = OpenGL::Camera::Movements::yawRight;
	else if (shift) movement = OpenGL::Camera::Movements::rollRight;
	else movement = OpenGL::Camera::Movements::moveRight;
}



BOOL CMolecularDynamicsView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	wheelAccumulator += zDelta;

	int distanceTicks = abs(wheelAccumulator);

	if (distanceTicks >= WHEEL_DELTA) {
		distanceTicks /= WHEEL_DELTA;
		const bool forward = (wheelAccumulator > 0 ? true : false);
		wheelAccumulator %= WHEEL_DELTA;

		camera.ProgressiveMove(forward ? OpenGL::Camera::Movements::moveForward : OpenGL::Camera::Movements::moveBackward, distanceTicks, true);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

Vector3D<double> CMolecularDynamicsView::GetTowardsVector(CPoint& point, const Vector3D<double>& forward)
{
	CRect rect;
	GetClientRect(rect);

	const double pixelSize = 2.*nearPlaneDistance*tan(cameraAngle / 2.) / rect.Height();

	point.x -= rect.Width() / 2;
	point.y -= rect.Height() / 2;
	point.y = -point.y;

	const Vector3D<double> right = (forward % camera.up).Normalize();
	const Vector3D<double> up = (right % forward).Normalize();

	return (nearPlaneDistance*forward + pixelSize * point.x * right + pixelSize * point.y * up).Normalize();
}


void CMolecularDynamicsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	Vector3D<double> forward = (camera.lookAt - camera.eyePos).Normalize();
	Vector3D<double> towards = GetTowardsVector(point, forward);

	double angle = acos(towards * forward);
	const int numticks = static_cast<int>(angle / camera.GetRotateAngle());

	camera.RotateTowards(angle - numticks * camera.GetRotateAngle(), towards);
	camera.ProgressiveRotate(towards, numticks);

	CView::OnLButtonDown(nFlags, point);
}


void CMolecularDynamicsView::ClearProgram()
{
	delete program;
	program = nullptr;
}


void CMolecularDynamicsView::Setup()
{
	if (inited) return;

	SetupGl();

	SetupSpheres();

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	SetupProgram();

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

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);

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


void CMolecularDynamicsView::RenderScene()
{
	if (NULL == program) return;

	CMolecularDynamicsDoc *doc = GetDocument();
	if (!doc) return;

	glm::mat4 mat = perspectiveMatrix*(glm::mat4)camera;

	program->Use();

	glUniform3f(program->viewPosLocation, static_cast<float>(camera.eyePos.X), static_cast<float>(camera.eyePos.Y), static_cast<float>(camera.eyePos.Z));
	glUniformMatrix4fv(program->matLocation, 1, GL_FALSE, value_ptr(mat));


	for (unsigned int i = 0; i < program->lights.size(); ++i)
	{
		glm::vec3 lightDir = program->lights[i].lightPos;
		lightDir = glm::normalize(lightDir);
		glUniform3f(program->lights[i].lightDirPos, lightDir.x, lightDir.y, lightDir.z);
		glUniform1f(program->lights[i].attenPos, program->lights[i].atten);
	}

	// now particles!!!

	int index = 0;
	for (const auto &particle : doc->curResult.particles)
	{
		Vector3D<double> particlePos = particle.GetPosition(doc->simulationTime/*, spaceSize*/);
		glm::vec3 pos = glm::vec3(particlePos.X, particlePos.Y, particlePos.Z);
		//glm::vec3 pos = glm::vec3(particle.position.X, particle.position.Y, particle.position.Z);

		position[3ULL * index] = pos.x;
		position[3ULL * index + 1ULL] = pos.y;
		position[3ULL * index + 2ULL] = pos.z;

		++index;
	}

	glEnableVertexAttribArray(2);
	posBuffer->setData(NULL, doc->nrParticles * 3 * sizeof(GLfloat), GL_STREAM_DRAW); // orphaning
	posBuffer->setSubData(position.data(), doc->nrParticles * 3 * sizeof(GLfloat));

	sphere->DrawInstanced(doc->nrParticles);

	program->UnUse();
}


void CMolecularDynamicsView::Resize(GLsizei h, GLsizei w)
{
	if (h == 0) return;

	glViewport(0, 0, w, h);

	const double ar = static_cast<double>(w) / h;
	perspectiveMatrix = glm::perspective(cameraAngle, ar, nearPlaneDistance, farPlaneDistance);
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

	ClearProgram();

	inited = false;

	Setup();
}


void CMolecularDynamicsView::SetColors()
{
	CMolecularDynamicsDoc *doc = GetDocument();

	if (doc)
	{
		int index = 0;
		const double avgRadius = (doc->options.exteriorSpheresRadius + doc->options.interiorSpheresRadius) / 2;

		for (const auto &particle : doc->curResult.particles)
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


void CMolecularDynamicsView::SetSpeeds(double translate, double rotate)
{
	camera.SetSpeeds(translate, rotate);
}
