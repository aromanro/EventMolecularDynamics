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
	m_ShowChart = (theApp.options.showBillboard ? BST_CHECKED : BST_UNCHECKED);
	m_UseSplines = (theApp.options.useSpline ? BST_CHECKED : BST_UNCHECKED);
	m_NrBins = theApp.options.nrBins;
	m_MaxSpeed = theApp.options.maxSpeed;
	m_LineThickness = theApp.options.lineThickness;
}

ChartPropertyPage::~ChartPropertyPage()
{
}

void ChartPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT2, m_Edit2);

	DDX_Check(pDX, IDC_CHECK1, m_ShowChart);
	DDX_Check(pDX, IDC_CHECK2, m_UseSplines);
	DDX_Text(pDX, IDC_EDIT1, m_NrBins);
	DDX_Text(pDX, IDC_EDIT2, m_MaxSpeed);
	DDX_Text(pDX, IDC_EDIT3, m_LineThickness);

	DDV_MinMaxUInt(pDX, m_NrBins, 4, 512);
	DDV_MinMaxDouble(pDX, m_MaxSpeed, 0.01, 999999);
	DDV_MinMaxUInt(pDX, m_LineThickness, 0, 9);
}


BEGIN_MESSAGE_MAP(ChartPropertyPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_CHECK1, &ChartPropertyPage::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &ChartPropertyPage::OnBnClickedCheck1)
	ON_EN_CHANGE(IDC_EDIT1, &ChartPropertyPage::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &ChartPropertyPage::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT3, &ChartPropertyPage::OnEnChangeEdit1)
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

	m_Edit2.allowNegative = false;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void ChartPropertyPage::ApplyValues()
{
	theApp.options.showBillboard = m_ShowChart == BST_CHECKED;
	theApp.options.useSpline = m_UseSplines == BST_CHECKED;
	theApp.options.nrBins = m_NrBins;
	theApp.options.maxSpeed = m_MaxSpeed;
	theApp.options.lineThickness = m_LineThickness;

	theApp.options.Save();

	CMolecularDynamicsDoc* doc = dynamic_cast<CMolecularDynamicsDoc*>(dynamic_cast<CMainFrame*>(theApp.m_pMainWnd)->GetActiveDocument());
	if (!doc) return;

	doc->options.showBillboard = theApp.options.showBillboard;
	doc->options.useSpline = theApp.options.useSpline;
	doc->options.lineThickness = theApp.options.lineThickness;

	std::unique_lock<std::mutex> dlock(doc->dataSection);
	doc->options.nrBins = theApp.options.nrBins;
	doc->options.maxSpeed = theApp.options.maxSpeed;
	dlock.unlock();

	doc->RetrieveStatistics();
}

void ChartPropertyPage::OnBnClickedCheck1()
{
	SetModified();
}


void ChartPropertyPage::OnEnChangeEdit1()
{
	SetModified();
}
