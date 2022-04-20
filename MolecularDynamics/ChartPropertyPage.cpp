#include "stdafx.h"
#include "afxdialogex.h"
#include "ChartPropertyPage.h"


#include "resource.h"

#include "MainFrm.h"
#include "MolecularDynamics.h"
#include "Options.h"

#include "MolecularDynamicsDoc.h"
#include "MolecularDynamicsView.h"


// ChartPropertyPage dialog

IMPLEMENT_DYNAMIC(ChartPropertyPage, CMFCPropertyPage)

ChartPropertyPage::ChartPropertyPage()
	: CMFCPropertyPage(IDD_CHARTPROPERTYPAGE)
{
}

ChartPropertyPage::~ChartPropertyPage()
{
}

void ChartPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ChartPropertyPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// ChartPropertyPage message handlers


BOOL ChartPropertyPage::OnApply()
{
	UpdateData();

	ApplyValues();

	return CMFCPropertyPage::OnApply();
}


BOOL ChartPropertyPage::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void ChartPropertyPage::ApplyValues()
{
	theApp.options.Save();

	CMolecularDynamicsDoc* doc = dynamic_cast<CMolecularDynamicsDoc*>(dynamic_cast<CMainFrame*>(theApp.m_pMainWnd)->GetActiveDocument());
	if (!doc) return;

	CMolecularDynamicsView* view = doc->GetMainView();
	if (!view) return;
}



