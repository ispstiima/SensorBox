#include "stdafx.h"
#include "MyImage.h"
#include "MultiBagWriterDlg.h"
#include "resource.h"

constexpr auto IMAGEFRAME = 50;

CMyImage::CMyImage()
	: bWinOpen(false)
	, m_Pose()
	, iFrame(1)
	, align_to_color(RS2_STREAM_COLOR)
	, align_to_depth(RS2_STREAM_DEPTH)
	, isRotated(false)
{
	m_winColorName = "RGB Images";
	m_winDepthName = "Depth Images";
}


CMyImage::~CMyImage()
{
	cv::destroyAllWindows();
}

void CMyImage::InitDisplay(int nCams)
{
	m_nCams = nCams;

	// Get Screensize
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	scrnW = desktop.right;
	scrnH = desktop.bottom;

	// open window
	cv::namedWindow(m_winColorName, cv::WINDOW_NORMAL);
	cv::namedWindow(m_winDepthName, cv::WINDOW_NORMAL);

	SetWindows();

			
	//Set the icon
	HICON hIcon = (HICON)LoadImageW(NULL, L"res\\MultiBagWriter.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	if (hIcon) {
		HWND winHandle = GetWindowHandle(m_winColorName);
		if (winHandle) {
			SendMessage(winHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			DestroyIcon(hIcon);
		}
	}

	//Set the icon
	hIcon = (HICON)LoadImageW(NULL, L"res\\MultiBagWriter.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	if (hIcon) {
		HWND winHandle = GetWindowHandle(m_winDepthName);
		if (winHandle) {
			SendMessage(winHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			DestroyIcon(hIcon);
		}
	}

	bWinOpen = true;
}

void CMyImage::SetWindows()
{
	if (isRotated) {
		singleImageH = (int)((scrnH  - 3 * IMAGEFRAME) / 2.0);
		singleImageW = (int)(singleImageH / 16.0 * 9.0);
		dispImageH = singleImageH;
		dispImageW = singleImageW * m_nCams;

		if (dispImageW > scrnW - 3 * IMAGEFRAME) {
			dispImageW = scrnW - 3 * IMAGEFRAME;
			singleImageW = (int)(double(dispImageW) / double(m_nCams));
			singleImageH = (int)(singleImageW / 9.0 * 16.0);
			dispImageH = singleImageH;
		}

		cv::resizeWindow(m_winColorName, dispImageW, dispImageH);
		cv::moveWindow(m_winColorName, IMAGEFRAME, IMAGEFRAME);

		cv::resizeWindow(m_winDepthName, dispImageW, dispImageH);
		cv::moveWindow(m_winDepthName, IMAGEFRAME, dispImageH + 2 * IMAGEFRAME);
	}
	else {
		singleImageW = (int)((scrnW - 3 * IMAGEFRAME) / 2.0);
		singleImageH = (int)(singleImageW / 16.0 * 9.0);
		dispImageH = singleImageH * m_nCams;
		dispImageW = singleImageW;

		if (dispImageH > scrnH - 2 * IMAGEFRAME) {
			dispImageH = scrnH - 2 * IMAGEFRAME;
			singleImageH = (int)(double(dispImageH) / double(m_nCams));
			singleImageW = (int)(singleImageH / 9.0 * 16.0);
			dispImageW = singleImageW;
		}

		cv::resizeWindow(m_winColorName, dispImageW, dispImageH);
		cv::moveWindow(m_winColorName, IMAGEFRAME, IMAGEFRAME);

		cv::resizeWindow(m_winDepthName, dispImageW, dispImageH);
		cv::moveWindow(m_winDepthName, dispImageW + 2 * IMAGEFRAME, IMAGEFRAME);
	}
}

HWND CMyImage::GetWindowHandle(std::string inputWinName)
{
	CCountWindows WinCounter;
	int nWindows = WinCounter.CountThem();

	int bufferLength;
	int inLength = (int)inputWinName.length() + 1;
	bufferLength = MultiByteToWideChar(CP_ACP, 0, inputWinName.c_str(), inLength, 0, 0);
	wchar_t* buffer = new wchar_t[bufferLength];
	MultiByteToWideChar(CP_ACP, 0, inputWinName.c_str(), inLength, buffer, bufferLength);
	std::wstring inputWinNameW(buffer);
	delete[] buffer;
	
	for (int i = 0; i < nWindows; i++) {
		if (WinCounter.WinNames[i].GetBuffer() == inputWinNameW)
			return WinCounter.hWindows[i];
	}
	return NULL;
}


void CMyImage::RotateDisplay()
{
	isRotated = !isRotated;
	SetWindows();
}

void CMyImage::ExitDisplay()
{
	// open window
	cv::destroyWindow(m_winColorName);
	cv::destroyWindow(m_winDepthName);
	bWinOpen = false;
}

void CMyImage::SetPose(MyPose inputPose)
{
	m_Pose = inputPose;
}

/*void CMyImage::SetFramesets(std::vector<rs2::frameset> inputFramesets)
{
	m_Framesets = inputFramesets;
}
*/

void CMyImage::ClearPose()
{
	// Clear pose data
	for (int idx = 0; idx < 3; idx++)
		m_Pose.T[idx] = 0;
	*m_Pose.T = NULL;
	for (int idx = 0; idx < 4; idx++)
		m_Pose.R[idx] = 0;
	*m_Pose.R = NULL;
	for (int idx = 0; idx < 16; idx++)
		m_Pose.TransfMat[idx] = 0;
	*m_Pose.TransfMat = NULL;
}


void CMyImage::DisplayFramesets(rs2_stream requiredStream, rs2::frameset *inFramesets, std::vector<std::string> serials)
{
	cv::Mat dispImage;
	char Testo[256];
	std::vector<cv::Mat> Images;
	std::vector<rs2::frame> newFrames;
	int nRGBDcameras = serials.size();
	rs2::colorizer colorizer = rs2::colorizer();

	for (int i = 0; i < m_nCams; i++)
		for (const rs2::frame& tFrame : inFramesets[i]) 
			if (tFrame.get_profile().stream_type() == requiredStream)
				newFrames.push_back(tFrame);

	for (int j = 0; j < serials.size(); j++) {
		bool isFound = false;
		
		for (int i = 0; i < newFrames.size(); i++) {
			MyFrame tMyFrame;
			std::string SerialNumber = rs2::sensor_from_frame(newFrames[i])->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
			
			if (SerialNumber == serials[j]) {
				double TimeStamp = newFrames[i].get_timestamp();

				const int w = newFrames[i].as<rs2::video_frame>().get_width();
				const int h = newFrames[i].as<rs2::video_frame>().get_height();

				if (requiredStream == RS2_STREAM_DEPTH)
					newFrames[i] = colorizer.process(newFrames[i]);

				cv::Mat Image = cv::Mat(cv::Size(w, h), CV_8UC3, (void*)newFrames[i].get_data(), cv::Mat::AUTO_STEP);
				if (isRotated)					
					cv::rotate(Image, Image, cv::ROTATE_90_CLOCKWISE);
				cv::resize(Image, Image, cv::Size(singleImageW, singleImageH));
				cv::cvtColor(Image, Image, cv::COLOR_BGR2RGB);
				
				sprintf_s(Testo, "SN: %s", SerialNumber.c_str());
				cv::putText(Image, Testo, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1.8);
				sprintf_s(Testo, "TimeStamp: %f", TimeStamp);
				cv::putText(Image, Testo, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1.8);

				Images.push_back(Image);
				isFound = true;
			}
		}	
		if (!isFound)
			Images.push_back(cv::Mat::zeros(cv::Size(singleImageW, singleImageH), CV_8UC3));
	}		

	dispImage = Images[0];
	for (int i = 1; i < Images.size(); i++) {
		if (isRotated)
			cv::hconcat(dispImage, Images[i], dispImage);
		else
			cv::vconcat(dispImage, Images[i], dispImage);
	}
	
	if (requiredStream == RS2_STREAM_COLOR)
		cv::imshow(m_winColorName, dispImage);
	else if (requiredStream == RS2_STREAM_DEPTH)
		cv::imshow(m_winDepthName, dispImage);
	cv::waitKey(1);

}


void CMyImage::SaveFrameset(rs2::frameset CurrFrameset)
{		
	rs2::frameset CurrA2DFrameset = align_to_depth.process(CurrFrameset);
	rs2::frameset CurrA2CFrameset = align_to_color.process(CurrFrameset);
	rs2::frame ExactFrame;


	// ----- SAVE COLOR -----
	SavePng(CurrFrameset, RS2_STREAM_COLOR, false);

	// ----- SAVE COLOR ALIGNED -----
	SavePng(CurrA2DFrameset, RS2_STREAM_COLOR, true);

	// ----- SAVE DEPTH -----
	SavePng(CurrFrameset, RS2_STREAM_DEPTH, false);
	SaveDepthRaw(CurrFrameset, false);
	//SaveDepthCsv(CurrFrameset, false);

	// ----- SAVE DEPTH ALIGNED -----
	SavePng(CurrA2CFrameset, RS2_STREAM_DEPTH, true);
	SaveDepthRaw(CurrA2CFrameset, true);
	//SaveDepthCsv(CurrA2CFrameset, true);

	// ----- SAVE IRs -----
	SavePng(CurrFrameset, RS2_STREAM_INFRARED, false);

	// ----- SAVE PC -----
	SavePly(CurrFrameset);
}

void CMyImage::SavePng(rs2::frameset CurrFrameset, rs2_stream stream_type, bool isAligned)
{
	rs2::frame ExactFrame;
	if (stream_type != RS2_STREAM_INFRARED) {
		for (const rs2::frame& f : CurrFrameset) {
			if (f.get_profile().stream_type() == stream_type) {
				ExactFrame = f;
				break;
			}
		}
	}
	else {
		for (const rs2::frame& f : CurrFrameset) {
			if (f.get_profile().stream_type() == RS2_STREAM_INFRARED && f.get_profile().stream_index() == 1) {
				ExactFrame = f;
				break;
			}
		}
	}
	
	rs2::video_frame CurrFrame = ExactFrame.as<rs2::video_frame>();
	if (CurrFrame) {
		// if depth, colorize frame
		if (stream_type == RS2_STREAM_DEPTH)
			CurrFrame = colorizer.process(CurrFrame);

		// Set the image name (destination is the path)
		char filenameC[256];
		const char* serial = rs2::sensor_from_frame(CurrFrame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		
		if (isAligned) {
			if (stream_type == RS2_STREAM_COLOR)
				sprintf_s(filenameC, "%s/SN%s_ID%03d_ColorAligned_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
			else if (stream_type == RS2_STREAM_DEPTH)
				sprintf_s(filenameC, "%s/SN%s_ID%03d_DepthAligned_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
			else //infrared left
				sprintf_s(filenameC, "%s/SN%s_ID%03d_InfraredLeftAligned_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
		}
		else {
			if (stream_type == RS2_STREAM_COLOR)
				sprintf_s(filenameC, "%s/SN%s_ID%03d_Color_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
			else if (stream_type == RS2_STREAM_DEPTH)
				sprintf_s(filenameC, "%s/SN%s_ID%03d_Depth_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
			else //infrared left
				sprintf_s(filenameC, "%s/SN%s_ID%03d_InfraredLeft_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
		}
		std::string filenameS(filenameC);
		
		// Convert OpenCV Image and write it
		cv::Mat imageBGR, imageRGB;
		if (stream_type == RS2_STREAM_INFRARED)
			imageBGR = cv::Mat(cv::Size(CurrFrame.get_width(), CurrFrame.get_height()), CV_8UC1, (void*)CurrFrame.get_data());
		else
			imageBGR = cv::Mat(cv::Size(CurrFrame.get_width(), CurrFrame.get_height()), CV_8UC3, (void*)CurrFrame.get_data(), cv::Mat::AUTO_STEP);
		cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
		cv::imwrite(filenameS, imageRGB);
	}

	if (stream_type == RS2_STREAM_INFRARED) {
		for (const rs2::frame& f : CurrFrameset) {
			if (f.get_profile().stream_type() == RS2_STREAM_INFRARED && f.get_profile().stream_index() == 2) {
				ExactFrame = f;
				break;
			}
		}
		CurrFrame = ExactFrame.as<rs2::video_frame>();
		if (CurrFrame) {
			// Set the image name (destination is the path)
			char filenameC[256];
			const char* serial = rs2::sensor_from_frame(CurrFrame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

			if (isAligned) 
				sprintf_s(filenameC, "%s/SN%s_ID%03d_InfraredRightAligned_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
			else 
				sprintf_s(filenameC, "%s/SN%s_ID%03d_InfraredRight_%.0f.png", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
			std::string filenameS(filenameC);

			// Convert OpenCV Image and write it
			cv::Mat imageBGR, imageRGB;
			imageBGR = cv::Mat(cv::Size(CurrFrame.get_width(), CurrFrame.get_height()), CV_8UC1, (void*)CurrFrame.get_data());
			cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
			cv::imwrite(filenameS, imageRGB);
		}
	}
}


void CMyImage::SaveDepthRaw(rs2::frameset CurrFrameset, bool isAligned)
{
	rs2::frame ExactFrame;
	for (const rs2::frame& f : CurrFrameset) {
		if (f.get_profile().stream_type() == RS2_STREAM_DEPTH) {
			ExactFrame = f;
			break;
		}
	}
	rs2::video_frame CurrFrame = ExactFrame.as<rs2::video_frame>();
	if (CurrFrame) {
		// Set the image name (destination is the path)
		char filenameC[256];
		const char* serial = rs2::sensor_from_frame(CurrFrame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		if (isAligned)
			sprintf_s(filenameC, "%s/SN%s_ID%03d_DepthAligned_%.0f.raw", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
		else
			sprintf_s(filenameC, "%s/SN%s_ID%03d_Depth_%.0f.raw", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));

		// Initialize the rawfile and write the data
		std::ofstream fs(filenameC, std::ios::binary | std::ios::trunc);
		if (fs) {
			fs.write(static_cast<const char*>(CurrFrame.get_data()), CurrFrame.get_stride_in_bytes() * CurrFrame.get_height());
			fs.flush();
		}
	}
}

void CMyImage::SaveDepthCsv(rs2::frameset CurrFrameset, bool isAligned)
{
	rs2::frame ExactFrame;
	for (const rs2::frame& f : CurrFrameset) {
		if (f.get_profile().stream_type() == RS2_STREAM_DEPTH) {
			ExactFrame = f;
			break;
		}
	}
	rs2::depth_frame CurrFrame = ExactFrame.as<rs2::depth_frame>();
	if (CurrFrame) {
		// Set the image name (destination is the path)
		char filenameC[256];
		const char* serial = rs2::sensor_from_frame(CurrFrame)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		if (isAligned)
			sprintf_s(filenameC, "%s/SN%s_ID%03d_DepthAligned_%.0f.csv", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));
		else
			sprintf_s(filenameC, "%s/SN%s_ID%03d_Depth_%.0f.csv", destinationFolder.c_str(), serial, iFrame, round(CurrFrame.get_timestamp()));

		// Initialize the csv file and write the data
		std::ofstream fs(filenameC, std::ios::trunc);
		if (fs) {
			for (int y = 0; y < CurrFrame.get_height(); y++) {
				auto delim = "";
				for (int x = 0; x < CurrFrame.get_width(); x++) {
					fs << delim << CurrFrame.get_distance(x, y);
					delim = ",";
				}
				fs << '\n';
			}
			fs.flush();
		}
	}
}

void CMyImage::SavePly(rs2::frameset CurrFrameset)
{
	rs2::pointcloud pc;

	// Get both frames from the frameset
	rs2::depth_frame frameDepth = CurrFrameset.get_depth_frame();
	rs2::video_frame frameColor = CurrFrameset.get_color_frame();

	if (frameDepth && frameColor) {
		pc.map_to(frameColor);
		auto points = pc.calculate(frameDepth);

		// Set the image name (destination is the path)
		char filenameC[256];
		const char* serial = rs2::sensor_from_frame(frameColor)->get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		sprintf_s(filenameC, "%s/SN%s_ID%03d_PointCloud_%.0f.ply", destinationFolder.c_str(), serial, iFrame, round(frameDepth.get_timestamp()));
		std::string filenameS(filenameC);

		// Export to ply with realsense library
		points.export_to_ply(filenameS, frameColor);
	}
}



BOOL CMyImage::CCountWindows::StaticWndEnumProc(HWND hwnd, LPARAM lParam)
{
	CCountWindows *pThis = reinterpret_cast<CCountWindows*>(lParam);
	return pThis->WndEnumProc(hwnd);
}


BOOL CMyImage::CCountWindows::WndEnumProc(HWND hwnd)
{
	m_count++;
	hWindows.push_back(hwnd);

	int length = GetWindowTextLength(hwnd);
	CString windowTitle;

	if (length > 0) {
		TCHAR* buffer;
		buffer = new TCHAR[length + 1];
		memset(buffer, 0, (length + 1) * sizeof(TCHAR));

		GetWindowText(hwnd, buffer, length + 1);
		windowTitle = buffer;
		delete[] buffer;
	}
	else
		windowTitle = L"";
	WinNames.push_back(windowTitle);

	return TRUE;
}


int CMyImage::CCountWindows::CountThem()
{
	m_count = 0;
	hWindows.clear();
	WinNames.clear();
	EnumWindows(StaticWndEnumProc, reinterpret_cast<LPARAM>(this));
	return m_count;
}