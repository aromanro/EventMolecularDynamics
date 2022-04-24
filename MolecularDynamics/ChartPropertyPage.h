#pragma once
#include <afxpropertypage.h>

#include "NumberEdit.h"

// ChartPropertyPage dialog

class ChartPropertyPage :
    public CMFCPropertyPage
{
	DECLARE_DYNAMIC(ChartPropertyPage)

public:
	ChartPropertyPage();
	virtual ~ChartPropertyPage();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHARTPROPERTYPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	void ApplyValues();

	int m_ShowChart;
	int m_UseSplines;

	unsigned int m_NrBins;
	double m_MaxSpeed;
	unsigned int m_LineThickness;

	CNumberEdit m_Edit2;
	CStatic m_Static1;
	CSliderCtrl m_Slider1;

public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnTRBNThumbPosChangingSlider1(NMHDR* pNMHDR, LRESULT* pResult);
};

