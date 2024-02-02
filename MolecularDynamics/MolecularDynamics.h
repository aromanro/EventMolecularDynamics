
// MolecularDynamics.h : main header file for the MolecularDynamics application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

#include "Options.h"
// CMolecularDynamicsApp:
// See MolecularDynamics.cpp for the implementation of this class
//

class CMolecularDynamicsApp : public CWinAppEx
{
public:
	CMolecularDynamicsApp();

// Overrides
	BOOL InitInstance() override;

// Implementation
	Options& GetOptions() { return options; }
	UINT& GetAppLook() { return m_nAppLook; }

	BOOL UseHiColorIcons() const { return m_bHiColorIcons; }

	DECLARE_MESSAGE_MAP()

private:
	void PreLoadState() override;
	void LoadCustomState() override;
	void SaveCustomState() override;

	afx_msg void OnAppAbout();
	int ExitInstance() override;

	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	Options options;

	ULONG_PTR gdiplusToken;
};

extern CMolecularDynamicsApp theApp;
