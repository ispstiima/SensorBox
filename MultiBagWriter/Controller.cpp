#include "stdafx.h"
#include "Controller.h"
#include "MultiBagWriterDlg.h"

IMPLEMENT_DYNCREATE(CController, CWinThread)


CController::CController()
	: bAcquisitionLoop(false)
	, bStartSaving(false)
	, bStopSaving(false)
	, bIsSaving(false)
	, bRotateDisplay(false)
	, bTakeSnapshot(false)
	, nSavedFrames(0)
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
		Image.maxDataSize = 4 * RS.nRGBDCameras;
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
	Image.InitDisplay(RS.nRGBDCameras);

	// Create the pointer to the dialog object
	CMultiBagWriterDlg *pDlg = (CMultiBagWriterDlg *)p_Container;

	// Create array of bools for snapshot acquisition
	bool* isSnapshotAcquired;
	isSnapshotAcquired = new bool[RS.nRGBDCameras];
	for (int i = 0; i < RS.nRGBDCameras; i++)
		isSnapshotAcquired[i] = false;
	int nSnapshots = 0;
		
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

		if (bRotateDisplay) {
			Image.RotateDisplay();
			bRotateDisplay = false;
		}

		CMyImage::MyPose currentPose = { 0, 0, 0 };

		//CSingleLock cs(&m_cs);
		m_cs.Lock();
		int nFramesets = RS.GetFramesets(bIsSaving);
		if (RS.idxTrackingPipe >= 0)
			RS.ComputePose(currentPose);
		m_cs.Unlock();

		Image.SetPose(currentPose);

		if (!bTakeSnapshot && !bIsSaving && nFramesets > 0) {
			Image.DisplayFramesets(RS2_STREAM_COLOR, RS.Framesets, RS.colorSerials);
			Image.DisplayFramesets(RS2_STREAM_DEPTH, RS.Framesets, RS.colorSerials);
		}
			
		if (bTakeSnapshot && nFramesets > 0) {
			int count = 0;
			for (int i = 0; i < RS.nRGBDCameras; i++) {
				if (RS.isFramesetAcquired[i] && !isSnapshotAcquired[i]) {
					Image.SaveFrameset(RS.Framesets[count]);
					isSnapshotAcquired[i] = true;
					nSnapshots++;
				}	
				count++;			
			}

			if (nSnapshots == RS.nRGBDCameras) {
				bTakeSnapshot = false;
				Image.iFrame++;
				nSnapshots = 0;
				for (int i = 0; i < RS.nRGBDCameras; i++)
					isSnapshotAcquired[i] = false;
			}			
		}

		// Clear framesets
		RS.ClearFramesets();
	}

	// Clear pose data
	Image.ClearPose();

	// Destroy image window
	Image.ExitDisplay();

	// Close Realsense workspace
	RS.StopPipelines();

	// Exit thread
	ExitInstance();

	delete[] isSnapshotAcquired;

	return TRUE;
}


void CController::SetContainer(void * Container)
{
	p_Container = Container;
}
