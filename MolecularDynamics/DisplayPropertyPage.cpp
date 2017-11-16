// DisplayPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "DisplayPropertyPage.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "MolecularDynamics.h"
#include "Options.h"

#include "MolecularDynamicsDoc.h"
#include "MolecularDynamicsView.h"

#include "resource.h"


// DisplayPropertyPage dialog

IMPLEMENT_DYNAMIC(DisplayPropertyPage, CMFCPropertyPage)

DisplayPropertyPage::DisplayPropertyPage()
	: CMFCPropertyPage(IDD_DISPLAYPROPERTYPAGE)
{

}

DisplayPropertyPage::~DisplayPropertyPage()
{
}

void DisplayPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_ColorControl1);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON2, m_ColorControl2);
}


BEGIN_MESSAGE_MAP(DisplayPropertyPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON1, &DisplayPropertyPage::OnBnClickedMfccolorbutton)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON2, &DisplayPropertyPage::OnBnClickedMfccolorbutton)
END_MESSAGE_MAP()


// DisplayPropertyPage message handlers


BOOL DisplayPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


BOOL DisplayPropertyPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_ColorControl1.EnableOtherButton(_T("More Colors..."));
	m_ColorControl1.SetColor(theApp.options.bigSphereColor);

	m_ColorControl2.EnableOtherButton(_T("More Colors..."));
	m_ColorControl2.SetColor(theApp.options.smallSphereColor);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void DisplayPropertyPage::ApplyValues()
{
	theApp.options.bigSphereColor = m_ColorControl1.GetColor();
	theApp.options.smallSphereColor = m_ColorControl2.GetColor();

	theApp.options.Save();

	CMolecularDynamicsDoc* doc = dynamic_cast<CMolecularDynamicsDoc*>(dynamic_cast<CMainFrame*>(theApp.m_pMainWnd)->GetActiveDocument());
	if (!doc) return;

	CMolecularDynamicsView *view = doc->GetMainView();
	if (!view) return;

	view->SetColors();
}


void DisplayPropertyPage::OnBnClickedMfccolorbutton()
{
	SetModified();
}
