#include "stdafx.h"
#include "RealSense.h"

CRealSense::CRealSense() 
	: idxTrackingPipe(-1)
	, nRGBDCameras(0)
	, m_nDevs(0)
{
	
	rs2::context ctx;

	rs2::device_list dev_list = ctx.query_devices();
	const int nDevs = dev_list.size();
	m_nDevs = nDevs;

	int count = 0;
	for (auto dev : dev_list) {
		if (strstr(dev.get_info(RS2_CAMERA_INFO_NAME), "T265"))
			idxTrackingPipe = count;
		else {
			nRGBDCameras++;
			colorSerials.push_back(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
		}
		cameraNames.push_back(dev.get_info(RS2_CAMERA_INFO_NAME));
		serials.push_back(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
		count++;
	}
	
	isFramesetAcquired = new bool[nRGBDCameras];
	Framesets = new rs2::frameset[nRGBDCameras];
	for (int i = 0; i < nRGBDCameras; i++) {
		isFramesetAcquired[i] = false;
		rs2::frameset fs = NULL;
		Framesets[i] = fs;
	}
		

	InitPipelines();
}


CRealSense::~CRealSense()
{
	delete[] pipes;
	delete[] isFramesetAcquired;
	delete[] Framesets;
}


int CRealSense::InitPipelines()
{
	rs2::context ctx;
	pipes = new std::shared_ptr<rs2::pipeline>[m_nDevs];
	for (int idxPipe = 0; idxPipe < m_nDevs; idxPipe++)
		pipes[idxPipe] = std::make_shared<rs2::pipeline>(ctx);
	return m_nDevs;
}


int CRealSense::ReInitPipelines()
{
	StopPipelines();
	InitPipelines();
	return 0;
}


int CRealSense::StartPipelines()
{
	// Start a streaming pipe per each connected device
	for (int idx = 0; idx < m_nDevs; idx++)
	{
		rs2::config cfg;
		std::string currName = cameraNames[idx];

		if (strstr(currName.c_str(), "T265")) {
			// Create configuration 
			cfg.enable_device(serials[idx]);
			cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
			// Start pipe	
			pipes[idx]->start(cfg);
		}
		else {
			// Create configuration
			cfg.enable_device(serials[idx]);
			cfg.enable_stream(RS2_STREAM_DEPTH, m_DepthW, m_DepthH, RS2_FORMAT_Z16, m_Fps);
			cfg.enable_stream(RS2_STREAM_COLOR, m_ColorW, m_ColorH, RS2_FORMAT_RGB8, m_Fps);
			cfg.enable_stream(RS2_STREAM_INFRARED, 1, m_DepthW, m_DepthH, RS2_FORMAT_Y8, m_Fps);
			cfg.enable_stream(RS2_STREAM_INFRARED, 2, m_DepthW, m_DepthH, RS2_FORMAT_Y8, m_Fps);
			
			// Start pipe
			pipes[idx]->start(cfg);
			//colorizers[serials[idx]] = rs2::colorizer();
		}
	}
	return 0;
}


int CRealSense::StartPipelines(CString SaveFileName)
{
	for (int idx = 0; idx < m_nDevs; idx++) {
		rs2::config cfg; // Declare a new configuration

		// Setting up the name of the bags
		std::string currName = cameraNames[idx];
		std::string::iterator end_pos = remove(currName.begin(), currName.end(), ' ');
		currName.erase(end_pos, currName.end());

		cfg.enable_device(serials[idx]);
		if (strstr(currName.c_str(), "T265")) {
			if (m_bAcqStreamFishEye != 0) {
				cfg.enable_stream(RS2_STREAM_FISHEYE, 1, RS2_FORMAT_Y8);
				cfg.enable_stream(RS2_STREAM_FISHEYE, 2, RS2_FORMAT_Y8);
			}
			cfg.enable_stream(RS2_STREAM_POSE, RS2_FORMAT_6DOF);
		}
		else {
			cfg.enable_stream(RS2_STREAM_DEPTH, m_DepthW, m_DepthH, RS2_FORMAT_Z16, m_Fps);
			cfg.enable_stream(RS2_STREAM_COLOR, m_ColorW, m_ColorH, RS2_FORMAT_RGB8, m_Fps);
			if (m_bAcqStreamIr != 0) {
				cfg.enable_stream(RS2_STREAM_INFRARED, 1, m_DepthW, m_DepthH, RS2_FORMAT_Y8, m_Fps);
				cfg.enable_stream(RS2_STREAM_INFRARED, 2, m_DepthW, m_DepthH, RS2_FORMAT_Y8, m_Fps);
			}
		}
		char FullFileName[256];
		sprintf_s(FullFileName, "%S_%S_SN%S.bag", (LPCTSTR)SaveFileName, GetWChar(currName.c_str()), GetWChar(serials[idx].c_str()));
		cfg.enable_record_to_file(FullFileName);

		pipes[idx]->start(cfg); //File will be opened at this point
		//colorizers[serials[idx]] = rs2::colorizer();
	}
	return 0;
}


int CRealSense::StopPipelines()
{
	for (int idxPipe = 0; idxPipe < m_nDevs; idxPipe++)
		pipes[idxPipe]->stop();
	return 0;
}


int CRealSense::ReStartPipelines(bool iStartSaving)
{
	ReInitPipelines();
	
	if (iStartSaving)
		StartPipelines(m_BagBasePathAndName);
	else
		StartPipelines();
	
	return 0;
}


void CRealSense::ComputePose(CMyImage::MyPose & pose)
{
	// Assume there is only one tracking camera
	rs2::pipeline *trackingPipe = pipes[idxTrackingPipe].get();

	// Wait for the next set of frames from the camera
	auto frames = trackingPipe->wait_for_frames();
	// Get a frame from the pose stream
	auto f = frames.first_or_default(RS2_STREAM_POSE);
	// Cast the frame to pose_frame and get its data
	rs2_pose pose_data = f.as<rs2::pose_frame>().get_pose_data();

	rs2_vector t = pose_data.translation;
	rs2_quaternion q = pose_data.rotation;

	// Set the matrix as column-major for convenient work with OpenGL and rotate by 180 degress (by negating 1st and 3rd columns)
	pose.TransfMat[0] = -(1 - 2 * q.y*q.y - 2 * q.z*q.z);	pose.TransfMat[4] = 2 * q.x*q.y - 2 * q.z*q.w;		pose.TransfMat[8] = -(2 * q.x*q.z + 2 * q.y*q.w);		pose.TransfMat[12] = t.x;
	pose.TransfMat[1] = -(2 * q.x*q.y + 2 * q.z*q.w);		pose.TransfMat[5] = 1 - 2 * q.x*q.x - 2 * q.z*q.z;	pose.TransfMat[9] = -(2 * q.y*q.z - 2 * q.x*q.w);		pose.TransfMat[13] = t.y;
	pose.TransfMat[2] = -(2 * q.x*q.z - 2 * q.y*q.w);		pose.TransfMat[6] = 2 * q.y*q.z + 2 * q.x*q.w;		pose.TransfMat[10] = -(1 - 2 * q.x*q.x - 2 * q.y*q.y);	pose.TransfMat[14] = t.z;
	pose.TransfMat[3] = 0.0f;								pose.TransfMat[7] = 0.0f;							pose.TransfMat[11] = 0.0f;								pose.TransfMat[15] = 1.0f;

	pose.T[0] = t.x;	pose.T[1] = t.y;	pose.T[2] = t.z;
	pose.R[0] = q.w;	pose.R[1] = q.x;	pose.R[2] = q.y;	pose.R[3] = q.z;
}


int CRealSense::GetFramesets(bool bIsSaving)
{	
	for (int i = 0; i < nRGBDCameras; i++) {
		isFramesetAcquired[i] = false;
		rs2::frameset fs = NULL;
		Framesets[i] = fs;
	}

	int count = 0, nFramesets = 0;
	for (int idxPipe = 0; idxPipe < m_nDevs; idxPipe++) {
		if (idxPipe != idxTrackingPipe) {
			rs2::frameset fs;
			pipes[idxPipe]->poll_for_frames(&fs);
			if (fs) {
				Framesets[count] = fs;
				isFramesetAcquired[count] = true;
				nFramesets++;
			}
			count++;
		}
	}
	return nFramesets;
}


void CRealSense::ClearFramesets()
{
	// Clear frame data
	for (int i = 0; i < nRGBDCameras; i++) {
		rs2::frameset fs = NULL;
		Framesets[i] = fs;
	}
}

const wchar_t * CRealSense::GetWChar(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* outWChar = new wchar_t[cSize];
	size_t outSize;
	mbstowcs_s(&outSize, outWChar, cSize, c, cSize - 1);

	return outWChar;
}


