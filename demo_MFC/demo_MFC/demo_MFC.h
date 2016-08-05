
// demo_MFC.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cdemo_MFCApp:
// See demo_MFC.cpp for the implementation of this class
//

class Cdemo_MFCApp : public CWinApp
{
public:
	Cdemo_MFCApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cdemo_MFCApp theApp;