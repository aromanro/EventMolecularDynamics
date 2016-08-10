#pragma once
#include "afxcolorbutton.h"


// DisplayPropertyPage dialog

class DisplayPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(DisplayPropertyPage)

public:
	DisplayPropertyPage();
	virtual ~DisplayPropertyPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAYPROPERTYPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	void ApplyValues();
	afx_msg void OnBnClickedMfccolorbutton();

	CMFCColorButton m_ColorControl1;
	CMFCColorButton m_ColorControl2;
};
