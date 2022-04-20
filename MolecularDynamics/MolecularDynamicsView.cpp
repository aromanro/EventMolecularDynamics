
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

#include <numeric>

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
	posBuffer(nullptr), colorBuffer(nullptr), scaleBuffer(nullptr),
	billboardProgram(nullptr), billboardRectangle(nullptr), billboardTexture(nullptr)
{
	// TODO: add construction code here
}

CMolecularDynamicsView::~CMolecularDynamicsView()
{
	delete sphere;
	ClearProgram();
	ClearBillboardProgram();

	delete posBuffer;
	delete colorBuffer;
	delete scaleBuffer;

	delete billboardRectangle;
	delete billboardTexture;
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

		//glFlush(); // not really needed, SwapBuffers should take care of things
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
		static long long int ticks = 0;

		CMolecularDynamicsDoc *doc = GetDocument();

		if (doc) {
			if (++ticks % 100 == 0)
			{
				PaintBillboarChart();
				doc->RetrieveStatistics();
			}

			doc->simulationTime = doc->simulationTime + doc->nrsteps * timeStep;
			doc->Advance();
		}

		camera.Tick();
		if (keyDown) camera.Move(movement);

		RedrawWindow(0, 0, RDW_INTERNALPAINT | RDW_NOERASE | RDW_NOFRAME | RDW_UPDATENOW);		
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

bool CMolecularDynamicsView::HandleKeyPress(WPARAM wParam, bool ctrlflag, bool shiftflag)
{
	bool handled = false;

	switch (wParam)
	{
	case VK_UP:
		HandleUp(ctrlflag, shiftflag);
		handled = true;
		break;
	case VK_DOWN:
		HandleDown(ctrlflag, shiftflag);
		handled = true;
		break;
	case VK_LEFT:
		HandleLeft(ctrlflag, shiftflag);
		handled = true;
		break;
	case VK_RIGHT:
		HandleRight(ctrlflag, shiftflag);
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

void CMolecularDynamicsView::HandleUp(bool ctrlflag, bool shiftflag)
{
	if (ctrlflag) movement = OpenGL::Camera::Movements::pitchUp;
	else if (shiftflag) movement = OpenGL::Camera::Movements::moveUp;
	else movement = OpenGL::Camera::Movements::moveForward;
}

void CMolecularDynamicsView::HandleDown(bool ctrlflag, bool shiftflag)
{
	if (ctrlflag) movement = OpenGL::Camera::Movements::pitchDown;
	else if (shiftflag) movement = OpenGL::Camera::Movements::moveDown;
	else movement = OpenGL::Camera::Movements::moveBackward;
}

void CMolecularDynamicsView::HandleLeft(bool ctrlflag, bool shiftflag)
{
	if (ctrlflag) movement = OpenGL::Camera::Movements::yawLeft;
	else if (shiftflag) movement = OpenGL::Camera::Movements::rollLeft;
	else movement = OpenGL::Camera::Movements::moveLeft;
}

void CMolecularDynamicsView::HandleRight(bool ctrlflag, bool shiftflag)
{
	if (ctrlflag) movement = OpenGL::Camera::Movements::yawRight;
	else if (shiftflag) movement = OpenGL::Camera::Movements::rollRight;
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




void CMolecularDynamicsView::Resize(GLsizei h, GLsizei w)
{
	if (h == 0) return;

	glViewport(0, 0, w, h);

	const double ar = static_cast<double>(w) / h;
	perspectiveMatrix = glm::perspective(cameraAngle, ar, nearPlaneDistance, farPlaneDistance);
}

void CMolecularDynamicsView::SetSpeeds(double translate, double rotate)
{
	camera.SetSpeeds(translate, rotate);
}

void CMolecularDynamicsView::SetBillboardText(const char* text)
{
	if (!billboardTexture) return;

	memoryBitmap.WriteText(text, font);

	wglMakeCurrent(m_hDC, m_hRC);
	memoryBitmap.SetIntoTexture(*billboardTexture);
	wglMakeCurrent(NULL, NULL);
}


bool CMolecularDynamicsView::SetDataIntoChart()
{
	CMolecularDynamicsDoc* doc = GetDocument();
	if (!doc) return false;

	chart.useSpline = doc->options.useSpline;
	
	std::vector<unsigned int> results1;
	std::vector<unsigned int> results2;

	const unsigned int nrBins = doc->options.nrBins;

	{
		std::lock_guard<std::mutex> lock(doc->dataSection);
		if (doc->results1.empty() || doc->results2.empty()) return false;

		if (doc->results1.size() != nrBins || doc->results2.size() != nrBins) return false;

		results1.swap(doc->results1);
		results2.swap(doc->results2);
	}

	const double maxSpeed = doc->options.maxSpeed;
	const double vu = maxSpeed / nrBins;

	std::vector<double> dresults1(nrBins);
	std::vector<double> dresults2(nrBins);

	std::transform(results1.begin(), results1.end(), dresults1.begin(), [doc](unsigned int v) { return static_cast<double>(v) / doc->nrBigSpheres; });
	results1.clear();
	std::transform(results2.begin(), results2.end(), dresults2.begin(), [doc](unsigned int v) { return static_cast<double>(v) / doc->nrSmallSpheres; });
	results2.clear();

	chart.clear();
	chart.AddDataSet(vu / 2., vu, dresults1.data(), nrBins, static_cast<float>(doc->options.lineThickness), doc->options.bigSphereColor);
	chart.AddDataSet(vu / 2., vu, dresults2.data(), nrBins, static_cast<float>(doc->options.lineThickness), doc->options.smallSphereColor);

	chart.XAxisMax = doc->options.maxSpeed;
	const unsigned int ymax = static_cast<unsigned int>(100 * std::max(*std::max_element(dresults1.begin(), dresults1.end()), *std::max_element(dresults2.begin(), dresults2.end()))) + 1;
	chart.YAxisMax = ymax / 100.;
	
	return true;
}

void CMolecularDynamicsView::PaintBillboarChart()
{
	SetDataIntoChart();

	memoryBitmap.DrawChart(chart);

	wglMakeCurrent(m_hDC, m_hRC);
	memoryBitmap.SetIntoTexture(*billboardTexture);
	wglMakeCurrent(NULL, NULL);
}


