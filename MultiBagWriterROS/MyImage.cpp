#include "stdafx.h"
#include "MyImage.h"
#include "MultiBagWriterDlg.h"
#include "resource.h"


constexpr auto WINW = 1280;
constexpr auto WINH = 960;

CMyImage::CMyImage()
	: bWinOpen(false)
	, m_Pose()
{
	m_winName = "RGB-D Images";
}


CMyImage::~CMyImage()
{
	cv::destroyAllWindows();
}

void CMyImage::InitDisplay()
{
	// open window
	cv::namedWindow(m_winName, cv::WINDOW_NORMAL);
	cv::resizeWindow(m_winName, WINW, WINH);
	cv::moveWindow(m_winName, 100, 100);
		
	//Set the icon
	HICON hIcon = (HICON)LoadImageW(NULL, L"res\\MultiBagWriterROS.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	if (hIcon) {
		HWND winHandle = GetWindowHandle(m_winName);
		if (winHandle) {
			SendMessage(winHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			DestroyIcon(hIcon);
		}
	}

	bWinOpen = true;
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


void CMyImage::ExitDisplay()
{
	// open window
	cv::destroyWindow(m_winName);
	bWinOpen = false;
}

void CMyImage::SetPose(MyPose inputPose)
{
	m_Pose = inputPose;
}

void CMyImage::SetFrames(std::vector<MyFrame> inputFrames)
{
	m_Frames = inputFrames;
}

void CMyImage::ClearFrames()
{
	// Clear frame data
	m_Frames.clear();
}


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


void CMyImage::DisplayFrames(bool isRotated)
{
	cv::Mat dispImage;
	std::vector<MyFrame> PlotFrames(m_Frames);
	
	if (isRotated)
	{
		int cols = 3;
		int rows = PlotFrames.size() / cols;
		if (rows == 0)
			rows++;
		
		int dispW = WINW / cols;
		int dispH = WINH / rows;
		char Testo[256];
		
		for (int i = 0; i < PlotFrames.size(); i++) {
			cv::Mat matTemp;
			cv::cvtColor(PlotFrames[i].Image, matTemp, cv::COLOR_BGR2RGB);
			cv::rotate(matTemp, matTemp, cv::ROTATE_90_CLOCKWISE);
			cv::resize(matTemp, matTemp, cv::Size(dispW, dispH));
			
			sprintf_s(Testo, "SN: %s", PlotFrames[i].SerialNumber.c_str());
			cv::putText(matTemp, Testo, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
			sprintf_s(Testo, "TimeStamp: %f", PlotFrames[i].TimeStamp);
			cv::putText(matTemp, Testo, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);

			PlotFrames[i].Image = matTemp;
		}
		
		std::vector<cv::Mat> dispImageTemp;
		for (int i = 0; i < PlotFrames.size(); i = i + 2) {
			cv::Mat dispImageTemp0;
			cv::vconcat(PlotFrames[i].Image, PlotFrames[i + 1].Image, dispImageTemp0);
			dispImageTemp.emplace_back(dispImageTemp0);
		}

		dispImage = dispImageTemp[0];
		for (int i = 1; i < dispImageTemp.size(); i++)
			cv::hconcat(dispImage, dispImageTemp[i], dispImage);
	}
	else
	{
		int cols = 2;
		int rows = PlotFrames.size() / cols;
		if (rows == 0)
			rows++;

		int dispW = WINW / cols;
		int dispH = WINH / rows;
		char Testo[256];

		for (int i = 0; i < PlotFrames.size(); i++) {
			cv::Mat matTemp;
			cv::cvtColor(PlotFrames[i].Image, matTemp, cv::COLOR_BGR2RGB);
			cv::resize(matTemp, matTemp, cv::Size(dispW, dispH));

			sprintf_s(Testo, "SN: %s", PlotFrames[i].SerialNumber.c_str());
			cv::putText(matTemp, Testo, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
			sprintf_s(Testo, "TimeStamp: %f", PlotFrames[i].TimeStamp);
			cv::putText(matTemp, Testo, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);

			PlotFrames[i].Image = matTemp;
		}

		std::vector<cv::Mat> dispImageTemp;
		for (int i = 0; i < PlotFrames.size(); i = i + 2) {
			cv::Mat dispImageTemp0;
			cv::hconcat(PlotFrames[i].Image, PlotFrames[i + 1].Image, dispImageTemp0);
			dispImageTemp.emplace_back(dispImageTemp0);
		}

		dispImage = dispImageTemp[0];
		for (int i = 1; i < dispImageTemp.size(); i++)
			cv::vconcat(dispImage, dispImageTemp[i], dispImage);
	}
	
	cv::imshow("RGB-D Images", dispImage);
	cv::waitKey(1);
}

void CMyImage::DisplayFrames(bool isRotated, std::vector<std::string> serials)
{
	cv::Mat dispImage;
	std::vector<MyFrame> PlotFrames(m_Frames);

	if (isRotated)
	{
		int cols = 3;
		int rows = maxDataSize / cols;
		if (rows == 0)
			rows++;

		int dispW = WINW / cols;
		int dispH = WINH / rows;
		char Testo[256];

		std::vector<MyFrame> ExtPlotFrames;
		for (int i = 0; i < serials.size(); i++) {
			bool isFound = false;
			for (int j = 0; j < PlotFrames.size(); j++) {
				if (serials[i] == PlotFrames[j].SerialNumber) {
					ExtPlotFrames.push_back(PlotFrames[j]);
					isFound = true;
				}
			}
			if (!isFound) {
				MyFrame tFrame;
				tFrame.Image = cv::Mat::zeros(cv::Size(dispW, dispH), CV_8UC3);
				tFrame.SerialNumber = serials[i];
				tFrame.TimeStamp = 0.0;
				ExtPlotFrames.push_back(tFrame);
				ExtPlotFrames.push_back(tFrame);
			}
		}

		for (int i = 0; i < ExtPlotFrames.size(); i++) {
			cv::Mat matTemp;
			int c = ExtPlotFrames[i].Image.channels();
			cv::cvtColor(ExtPlotFrames[i].Image, matTemp, cv::COLOR_BGR2RGB);
			cv::rotate(matTemp, matTemp, cv::ROTATE_90_CLOCKWISE);
			cv::resize(matTemp, matTemp, cv::Size(dispW, dispH));

			sprintf_s(Testo, "SN: %s", ExtPlotFrames[i].SerialNumber.c_str());
			cv::putText(matTemp, Testo, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
			sprintf_s(Testo, "TimeStamp: %f", ExtPlotFrames[i].TimeStamp);
			cv::putText(matTemp, Testo, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);

			ExtPlotFrames[i].Image = matTemp;
		}

		std::vector<cv::Mat> dispImageTemp;
		for (int i = 0; i < ExtPlotFrames.size(); i = i + 2) {
			cv::Mat dispImageTemp0;
			cv::vconcat(ExtPlotFrames[i].Image, ExtPlotFrames[i + 1].Image, dispImageTemp0);
			dispImageTemp.emplace_back(dispImageTemp0);
		}

		dispImage = dispImageTemp[0];
		for (int i = 1; i < dispImageTemp.size(); i++)
			cv::hconcat(dispImage, dispImageTemp[i], dispImage);
	}
	else
	{
		int cols = 2;
		int rows = maxDataSize / cols;
		if (rows == 0)
			rows++;

		int dispW = WINW / cols;
		int dispH = WINH / rows;
		char Testo[256];

		std::vector<MyFrame> ExtPlotFrames;
		for (int i = 0; i < serials.size(); i++) {
			bool isFound = false;
			for (int j = 0; j < PlotFrames.size(); j++) {
				if (serials[i] == PlotFrames[j].SerialNumber) {
					ExtPlotFrames.push_back(PlotFrames[j]);
					isFound = true;
				}
			}
			if (!isFound) {
				MyFrame tFrame;
				tFrame.Image = cv::Mat::zeros(cv::Size(dispW, dispH), CV_8UC3);
				tFrame.SerialNumber = serials[i];
				tFrame.TimeStamp = 0.0;
				ExtPlotFrames.push_back(tFrame);
				ExtPlotFrames.push_back(tFrame);
			}
		}

		for (int i = 0; i < ExtPlotFrames.size(); i++) {
			cv::Mat matTemp;
			cv::cvtColor(ExtPlotFrames[i].Image, matTemp, cv::COLOR_BGR2RGB);
			cv::resize(matTemp, matTemp, cv::Size(dispW, dispH));

			sprintf_s(Testo, "SN: %s", ExtPlotFrames[i].SerialNumber.c_str());
			cv::putText(matTemp, Testo, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
			sprintf_s(Testo, "TimeStamp: %f", ExtPlotFrames[i].TimeStamp);
			cv::putText(matTemp, Testo, cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);

			ExtPlotFrames[i].Image = matTemp;
		}

		std::vector<cv::Mat> dispImageTemp;
		for (int i = 0; i < ExtPlotFrames.size(); i = i + 2) {
			cv::Mat dispImageTemp0;
			cv::hconcat(ExtPlotFrames[i].Image, ExtPlotFrames[i + 1].Image, dispImageTemp0);
			dispImageTemp.emplace_back(dispImageTemp0);
		}

		dispImage = dispImageTemp[0];
		for (int i = 1; i < dispImageTemp.size(); i++)
			cv::vconcat(dispImage, dispImageTemp[i], dispImage);
	}

	cv::imshow("RGB-D Images", dispImage);
	cv::waitKey(1);
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