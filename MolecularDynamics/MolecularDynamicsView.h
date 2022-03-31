
// MolecularDynamicsView.h : interface of the CMolecularDynamicsView class
//

#pragma once

#include <GL\glew.h>
#include <gl\gl.h>      // OpenGL Libraries
#include <gl\glu.h>     // GLU OpenGL Libraries

#include <glm.hpp>

#include "Camera.h"

#include "Program.h"
#include "Sphere.h"
#include "MolecularDynamicsGLProgram.h"
#include "BillboardGLProgram.h"
#include "Rectangle.h"
#include "Texture.h"
#include "MemoryBitmap.h"


class CMolecularDynamicsView : public CView
{
protected: // create from serialization only
	CMolecularDynamicsView();
	DECLARE_DYNCREATE(CMolecularDynamicsView)

// Attributes
private:
	CMolecularDynamicsDoc* GetDocument() const;

	HGLRC m_hRC;     // Rendering Context
	HDC m_hDC;       // Device Context
	CPalette m_GLPalette;   // Logical Palette
	UINT_PTR timer;

	int Width, Height;

	int wheelAccumulator;

	OpenGL::Camera camera;


	glm::mat4 perspectiveMatrix;

	MolecularDynamicsGLProgram *program;

	OpenGL::Sphere *sphere;

	OpenGL::VertexBufferObject *posBuffer;
	std::vector<GLfloat> position;

	OpenGL::VertexBufferObject *colorBuffer;
	std::vector<GLfloat> colors;

	OpenGL::VertexBufferObject *scaleBuffer;
	std::vector<GLfloat> scale;


	BillboardGLProgram* billboardProgram;
	OpenGL::Rectangle* billboardRectangle;
	OpenGL::Texture* billboardTexture;
	MemoryBitmap memoryBitmap;
	CFont font;


	bool keyDown;
	bool ctrl;
	bool shift;
	OpenGL::Camera::Movements movement;

	bool inited;

	void InitializePalette(void);
	void Resize(GLsizei h, GLsizei w);
	Vector3D<double> GetTowardsVector(CPoint& point, const Vector3D<double>& forward);

// Operations
public:
// Overrides
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
	virtual ~CMolecularDynamicsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Generated message map functions
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void ClearProgram();
	void ClearBillboardProgram();
	void Setup();
	bool SetupShaders();
	bool SetupBillboardShaders();
	void RenderScene();
	void Reset();
	bool KeyPressHandler(MSG* pMsg);
	void SetColors();
	void SetSpeeds(double translate, double rotate);

	void EnableAntialias();
	void DisableAntialias();
	void DisplayBilboard(glm::mat4& mat);
	void SetBillboardText(const char* text);

protected:
	bool HandleKeyPress(WPARAM wParam, bool ctrl, bool shift);
	void SetupProgram();
	void SetupBillboardProgram();
	void SetupSpheres();
	void SetupGl();

	void HandleUp(bool ctrl, bool shift);
	void HandleDown(bool ctrl, bool shift);
	void HandleLeft(bool ctrl, bool shift);
	void HandleRight(bool ctrl, bool shift);
};

#ifndef _DEBUG  // debug version in MolecularDynamicsView.cpp
inline CMolecularDynamicsDoc* CMolecularDynamicsView::GetDocument() const
   { return reinterpret_cast<CMolecularDynamicsDoc*>(m_pDocument); }
#endif

