#pragma once
#include "RealSense.h"
#include "MyImage.h"


class CController : public CWinThread
{

	DECLARE_DYNCREATE(CController)

public:
	CController();
	virtual ~CController();

	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	virtual BOOL Run();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CMyImage Image;
	bool bStartSaving, bStopSaving, bIsSaving;
	bool bAcquisitionLoop, bImageRotation;
	CString m_FilePathAndName;
	int DepthW, DepthH, ColorW, ColorH, Fps, bAcqStreamFishEye, bAcqStreamIr;

	void *p_Container;
	void SetContainer(void * Container);
	CCriticalSection m_cs;
};

