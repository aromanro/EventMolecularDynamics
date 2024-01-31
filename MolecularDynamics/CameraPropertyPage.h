#pragma once
#include "afxwin.h"


// CameraPropertyPage dialog

class CameraPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CameraPropertyPage)

public:
	CameraPropertyPage();
	~CameraPropertyPage() override;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERAPROPERTYPAGE };
#endif

private:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	BOOL OnApply() override;
	void ApplyValues();

	afx_msg void OnTRBNThumbPosChangingSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	
	CStatic m_Static1;
	CStatic m_Static2;
	CStatic m_Static3;

	CSliderCtrl m_Slider1;
	CSliderCtrl m_Slider2;
	CSliderCtrl m_Slider3;
};
