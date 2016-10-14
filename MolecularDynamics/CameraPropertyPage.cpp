// CameraPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "CameraPropertyPage.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "MolecularDynamics.h"
#include "Options.h"
#include "MolecularDynamicsDoc.h"
#include "MolecularDynamicsView.h"

#include "resource.h"

// CameraPropertyPage dialog

IMPLEMENT_DYNAMIC(CameraPropertyPage, CMFCPropertyPage)

CameraPropertyPage::CameraPropertyPage()
	: CMFCPropertyPage(IDD_CAMERAPROPERTYPAGE)
{

}

CameraPropertyPage::~CameraPropertyPage()
{
}

void CameraPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER1, m_Slider1);
	DDX_Control(pDX, IDC_SLIDER2, m_Slider2);
	DDX_Control(pDX, IDC_SLIDER3, m_Slider3);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_STATIC2, m_Static2);
	DDX_Control(pDX, IDC_STATIC3, m_Static3);
}


BEGIN_MESSAGE_MAP(CameraPropertyPage, CMFCPropertyPage)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &CameraPropertyPage::OnTRBNThumbPosChangingSlider1)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER2, &CameraPropertyPage::OnTRBNThumbPosChangingSlider2)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER3, &CameraPropertyPage::OnTRBNThumbPosChangingSlider3)
END_MESSAGE_MAP()


// CameraPropertyPage message handlers


BOOL CameraPropertyPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_Slider1.SetRange(25, 400);
	m_Slider2.SetRange(25, 400);
	m_Slider3.SetRange(10, 400);

	m_Slider1.SetPos((int)theApp.options.translationSpeed);
	m_Slider2.SetPos((int)theApp.options.rotationSpeed);
	m_Slider3.SetPos((int)theApp.options.scrollSpeed);

	CString str;
	str.Format(L"%d%%", theApp.options.translationSpeed);
	m_Static1.SetWindowText(str);

	str.Format(L"%d%%", theApp.options.rotationSpeed);
	m_Static2.SetWindowText(str);

	str.Format(L"%d%%", theApp.options.scrollSpeed);
	m_Static3.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CameraPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


void CameraPropertyPage::ApplyValues()
{
	theApp.options.translationSpeed = (unsigned int)m_Slider1.GetPos();
	theApp.options.rotationSpeed = (unsigned int)m_Slider2.GetPos();
	theApp.options.scrollSpeed = (unsigned int)m_Slider3.GetPos();

	theApp.options.Save();

	CMainFrame *mainFrm = (CMainFrame*)theApp.m_pMainWnd;
	if (!mainFrm) return;

	CMolecularDynamicsDoc* doc = (CMolecularDynamicsDoc*)mainFrm->GetActiveDocument();
	if (!doc) return;

	CMolecularDynamicsView *view = doc->GetMainView();
	if (!view) return;

	view->SetSpeeds(theApp.options.translationSpeed, theApp.options.rotationSpeed);
}



void CameraPropertyPage::OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static1.SetWindowText(str);

	SetModified();
}


void CameraPropertyPage::OnTRBNThumbPosChangingSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static2.SetWindowText(str);

	SetModified();
}


void CameraPropertyPage::OnTRBNThumbPosChangingSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static3.SetWindowText(str);

	SetModified();
}
