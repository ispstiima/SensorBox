#include "stdafx.h"
#include "Controller.h"
#include "MultiBagWriterDlg.h"

IMPLEMENT_DYNCREATE(CController, CWinThread)


CController::CController()
	: bAcquisitionLoop(false)
	, bStartSaving(false)
	, bStopSaving(false)
	, bIsSaving(false)
{
	m_bAutoDelete = false;

	// Load the acquisition parameters
	DepthW = GetPrivateProfileInt(L"Depth", L"Width", 848, L".//AcqParams.ini");
	DepthH = GetPrivateProfileInt(L"Depth", L"Height", 480, L".//AcqParams.ini");
	ColorW = GetPrivateProfileInt(L"Color", L"Width", 848, L".//AcqParams.ini");
	ColorH = GetPrivateProfileInt(L"Color", L"Height", 480, L".//AcqParams.ini");
	bAcqStreamIr = GetPrivateProfileInt(L"Ir", L"bAcqStreamIr", 0, L".//AcqParams.ini");
	bAcqStreamFishEye = GetPrivateProfileInt(L"Tracking", L"bAcqStreamFishEye", 0, L".//AcqParams.ini");
}
 


CController::~CController()
{
}

BOOL CController::InitInstance()
{
	/*if (!RS.StartPipelines()) {	
		Image.maxDataSize = 2 * RS.nRGBDCameras;
		return TRUE;
	}
	else {
		m_hThread = NULL;
		bAcquisitionLoop = false;
		return CWinThread::ExitInstance();
	}*/
	return TRUE;
}

int CController::ExitInstance()
{
	if (bAcquisitionLoop) {
		bAcquisitionLoop = false;
		if (WaitForSingleObject(m_hThread, INFINITE) == WAIT_OBJECT_0)
		{
		}
	}
	m_hThread = NULL;
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CController, CWinThread)
END_MESSAGE_MAP()

BOOL CController::Run()
{
	CRealSense RS;

	// Assign acquisition parameters
	RS.m_DepthW = DepthW;	RS.m_DepthH = DepthH;
	RS.m_ColorW = ColorW;	RS.m_ColorH = ColorH;	
	RS.m_Fps = Fps;
	RS.m_bAcqStreamFishEye = bAcqStreamFishEye;
	RS.m_bAcqStreamIr = bAcqStreamIr;

	// Init RealSense workspace
	RS.StartPipelines();
	
	// Open image window
	Image.maxDataSize = 2 * RS.nRGBDCameras;
	Image.InitDisplay();

	// Create the pointer to the dialog object
	CMultiBagWriterDlg *pDlg = (CMultiBagWriterDlg *)p_Container;
	
	while (bAcquisitionLoop)
	{
		if (bStartSaving) {
			RS.m_BagBasePathAndName = m_FilePathAndName;
			RS.ReStartPipelines(bStartSaving);
			bStartSaving = false;
			bIsSaving = true;
		}
		else if (bStopSaving) {
			RS.ReStartPipelines(!bStopSaving);
			bStopSaving = false;
			bIsSaving = false;
		}

		//CSingleLock cs(&m_cs);
		m_cs.Lock();

		std::vector<CMyImage::MyFrame> FramesToPlot;
		CMyImage::MyPose currentPose = { 0, 0, 0 };

		int nFrames = RS.GetFrames(FramesToPlot);
		if (RS.idxTrackingPipe >= 0)
			RS.ComputePose(currentPose);
		m_cs.Unlock();

		Image.SetPose(currentPose);

		if (!bIsSaving) {
			if (nFrames > 0) {
				Image.SetFrames(FramesToPlot);
				Image.DisplayFrames(bImageRotation, RS.colorSerials);
			}
		
			// Clear frames
			Image.ClearFrames();
		}

	}

	// Clear pose data
	Image.ClearPose();


	// Destroy image window
	Image.ExitDisplay();

	// Close Realsense workspace
	RS.StopPipelines();

	// Exit thread
	ExitInstance();

	return TRUE;
}


void CController::SetContainer(void * Container)
{
	p_Container = Container;
}
