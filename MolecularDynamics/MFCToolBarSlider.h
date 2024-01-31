#pragma once

#include "EmbeddedSlider.h"

// CMFCToolBarSlider command target

#define MAX_NRSTEPS 300

class CMFCToolBarSlider : public CMFCToolBarButton
{
public:
	CEmbeddedSlider slider;

	CMFCToolBarSlider();
	CMFCToolBarSlider(UINT uiId, int iImage = -1, DWORD dwStyle = 0, int iWidth = 0);
	~CMFCToolBarSlider() override;

	void OnDraw(CDC* /*pDC*/, const CRect& /*rect*/, CMFCToolBarImages* /*pImages*/, BOOL /*bHorz*/ = TRUE, BOOL /*bCustomizeMode*/ = FALSE, BOOL /*bHighlight*/ = FALSE, BOOL /*bDrawBorder*/ = TRUE, BOOL /*bGrayDisabledButtons*/ = TRUE) override;

	HWND GetHwnd() override;
	BOOL CanBeStretched() const override;
	BOOL HaveHotBorder() const override;
	void OnShow(BOOL bShow) override;
	void OnMove() override;
	void OnSize(int iSize) override;
	SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz) override;
	void OnChangeParentWnd(CWnd* pWndParent) override;

	void Serialize(CArchive& ar) override;
	void CopyFrom(const CMFCToolBarButton& src) override;


	static int GetPos(UINT uiCmd);
	static void SetPos(UINT uiCmd, int pos);

	void SetRange(int iMin, int iMax);
	void SetValue(int iValue, BOOL bNotify = TRUE);

	DECLARE_SERIAL(CMFCToolBarSlider);
};


