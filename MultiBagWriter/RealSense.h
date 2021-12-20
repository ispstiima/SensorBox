#pragma once

#include <librealsense2/rs.hpp>     // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include "MyImage.h"


class CRealSense
{
public:
	CRealSense();
	~CRealSense();

	std::shared_ptr<rs2::pipeline> *pipes;
	//std::map<std::string, rs2::colorizer> colorizers;
	std::vector<std::string> serials, colorSerials, cameraNames;
	rs2::frameset *Framesets;
	
	int m_nDevs, idxTrackingPipe;
	int nRGBDCameras;
	CString m_BagBasePathAndName;
	int m_DepthW, m_DepthH, m_Fps, m_ColorW, m_ColorH, m_bAcqStreamIr, m_bAcqStreamFishEye;
	bool* isFramesetAcquired;

	int InitPipelines();
	int StartPipelines();
	int StartPipelines(CString SaveFileName);
	int ReStartPipelines(bool iStartSaving);
	int GetFramesets(bool isSaving);
	const wchar_t * GetWChar(const char * c);
	void ComputePose(CMyImage::MyPose & pose);
	int ReInitPipelines();
	int StopPipelines();
	void ClearFramesets();
	
};

