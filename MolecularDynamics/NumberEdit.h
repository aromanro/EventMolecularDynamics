#pragma once


// CNumberEdit

class CNumberEdit : public CEdit
{
	DECLARE_DYNAMIC(CNumberEdit)
	DECLARE_MESSAGE_MAP()
public:
	CNumberEdit();
	~CNumberEdit() override;

	bool allowNegative;

private:
	CString m_GoodValue;
	DWORD m_GoodSel;
	bool m_Cancelling;

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnUpdate();
	afx_msg	LRESULT OnPaste(WPARAM wParam, LPARAM lParam);
};


