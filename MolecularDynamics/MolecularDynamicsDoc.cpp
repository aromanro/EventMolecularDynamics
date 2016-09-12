
// MolecularDynamicsDoc.cpp : implementation of the CMolecularDynamicsDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MolecularDynamics.h"
#endif

#include "MolecularDynamicsDoc.h"
#include "MolecularDynamicsView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMolecularDynamicsDoc

IMPLEMENT_DYNCREATE(CMolecularDynamicsDoc, CDocument)

BEGIN_MESSAGE_MAP(CMolecularDynamicsDoc, CDocument)
END_MESSAGE_MAP()


// CMolecularDynamicsDoc construction/destruction

CMolecularDynamicsDoc::CMolecularDynamicsDoc()
	: nrsteps(1), simulationTime(0), theThread(nullptr), nrParticles(0)
{
	// TODO: add one-time construction code here

}

CMolecularDynamicsDoc::~CMolecularDynamicsDoc()
{
	if (theThread) {
		theThread->Terminate = true;
		
		theThread->WakeUp();

		delete theThread;
	}
}

BOOL CMolecularDynamicsDoc::OnNewDocument()
{
	CWaitCursor cursor;

	if (!CDocument::OnNewDocument())
		return FALSE;
	CString title;

	title.Format(L"%d Hard Smooth Spheres", theApp.options.nrSpheres);
	SetTitle(title);

	bool reused = false;

	if (theThread) {
		theThread->Terminate = true;

		theThread->WakeUp();

		delete theThread;

		reused = true;
	}


	std::queue<MolecularDynamics::ComputationResult> emptyQueue;

	resultsQueue.swap(emptyQueue);

	// copy the options, they can change during running and we need the ones set at initialization of the 'document'
	options = theApp.options;

	theThread = new MolecularDynamics::MolecularDynamicsThread();

	nrParticles = theThread->Init();
	simulationTime = 0;
	
	// now copy data 
	curResult.particles = theThread->GetParticles();
	curResult.nextEventTime = theThread->GetNextEventTime();

	if (reused) GetMainView()->Reset();
	else GetMainView()->Setup();

	theThread->doc = this;
	theThread->Start();

	return TRUE;
}

CMolecularDynamicsView* CMolecularDynamicsDoc::GetMainView(void)
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CMolecularDynamicsView)))
			return (CMolecularDynamicsView*)pView;
	}

	return NULL;
}




// CMolecularDynamicsDoc serialization

void CMolecularDynamicsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CMolecularDynamicsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CMolecularDynamicsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMolecularDynamicsDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMolecularDynamicsDoc diagnostics

#ifdef _DEBUG
void CMolecularDynamicsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMolecularDynamicsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMolecularDynamicsDoc commands


void CMolecularDynamicsDoc::Advance()
{
	bool isEmpty;

	while (curResult.nextEventTime < simulationTime)
	{
		{
			std::lock_guard<std::mutex> lock(dataSection);
			isEmpty = resultsQueue.empty();
		}

		if (isEmpty) {			
			theThread->WakeUp();

			theThread->WaitForData();
		}
		else {
			{
				std::lock_guard<std::mutex> lock(dataSection);
				curResult = std::move(resultsQueue.front());
				resultsQueue.pop();
			}
			theThread->WakeUp();
		}
	}

}



