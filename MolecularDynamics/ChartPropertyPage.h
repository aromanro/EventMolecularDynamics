#pragma once
#include <afxpropertypage.h>

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
};

