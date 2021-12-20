
// MultiBagWriterROS.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <ros/ros.h>


// CMultiBagWriterROSApp:
// See MultiBagWriterROS.cpp for the implementation of this class
//

class CMultiBagWriterROSApp : public CWinApp
{
public:
	CMultiBagWriterROSApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMultiBagWriterROSApp theApp;
