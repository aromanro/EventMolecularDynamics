#pragma once
#include "afxcolorbutton.h"


// DisplayPropertyPage dialog

class DisplayPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(DisplayPropertyPage)

public:
	DisplayPropertyPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAYPROPERTYPAGE };
#endif

private:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL OnApply() override;
	BOOL OnInitDialog() override;
	void ApplyValues();
	afx_msg void OnBnClickedMfccolorbutton();

	CMFCColorButton m_ColorControl1;
	CMFCColorButton m_ColorControl2;
};
