
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

private:
	ULONG_PTR gdiplusToken;

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	Options options;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};

extern CMolecularDynamicsApp theApp;
