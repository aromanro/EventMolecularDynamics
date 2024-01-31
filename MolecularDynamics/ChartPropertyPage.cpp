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
	m_ShowChart = (theApp.GetOptions().showBillboard ? BST_CHECKED : BST_UNCHECKED);
	m_UseSplines = (theApp.GetOptions().useSpline ? BST_CHECKED : BST_UNCHECKED);
	m_NrBins = theApp.GetOptions().nrBins;
	m_MaxSpeed = theApp.GetOptions().maxSpeed;
	m_LineThickness = theApp.GetOptions().lineThickness;
}


void ChartPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider1);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);

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
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER1, &ChartPropertyPage::OnTRBNThumbPosChangingSlider1)
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

	m_Slider1.SetRange(0, 100);

	m_Slider1.SetPos(static_cast<int>(theApp.GetOptions().alpha));

	CString str;
	str.Format(L"%d%%", theApp.GetOptions().alpha);
	m_Static1.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void ChartPropertyPage::ApplyValues()
{
	theApp.GetOptions().showBillboard = m_ShowChart == BST_CHECKED;
	theApp.GetOptions().useSpline = m_UseSplines == BST_CHECKED;
	theApp.GetOptions().nrBins = m_NrBins;
	theApp.GetOptions().maxSpeed = m_MaxSpeed;
	theApp.GetOptions().lineThickness = m_LineThickness;
	theApp.GetOptions().alpha = static_cast<unsigned int>(m_Slider1.GetPos());

	theApp.GetOptions().Save();

	CMolecularDynamicsDoc* doc = dynamic_cast<CMolecularDynamicsDoc*>(dynamic_cast<CMainFrame*>(theApp.m_pMainWnd)->GetActiveDocument());
	if (!doc) return;

	doc->options.showBillboard = theApp.GetOptions().showBillboard;
	doc->options.useSpline = theApp.GetOptions().useSpline;
	doc->options.lineThickness = theApp.GetOptions().lineThickness;
	doc->options.alpha = theApp.GetOptions().alpha;

	std::unique_lock<std::mutex> dlock(doc->dataSection);
	doc->options.nrBins = theApp.GetOptions().nrBins;
	doc->options.maxSpeed = theApp.GetOptions().maxSpeed;
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

void ChartPropertyPage::OnTRBNThumbPosChangingSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CString str;
	str.Format(L"%d%%", pNMTPC->dwPos);
	m_Static1.SetWindowText(str);

	SetModified();
}


