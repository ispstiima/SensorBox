#pragma once

#include <opencv2/opencv.hpp>
#include <vector>


class CMyImage
{
public:
	CMyImage();
	~CMyImage();

public:
	class CCountWindows
	{
	public:
		int CountThem();	
		std::vector<HWND> hWindows;
		std::vector<CString> WinNames;
	private:
		static BOOL CALLBACK StaticWndEnumProc(HWND hwnd, LPARAM lParam);
		BOOL WndEnumProc(HWND hwnd);
		int m_count;
	};

	struct MyFrame {
		std::string SerialNumber;
		double TimeStamp;
		cv::Mat Image;
	};

	struct MyPose {
		float T[3];
		float R[4];
		float TransfMat[16];
	} m_Pose;

	struct MyPoints {
		float x;
		float y;
		float z;
		float u;
		float v;
	};

	typedef std::vector<MyPoints> MyPointCloud;

	cv::String m_winName;
	std::vector<MyFrame> m_Frames;
	
	int maxDataSize;
	BOOL bWinOpen;

	void InitDisplay();
	void ExitDisplay();
	HWND GetWindowHandle(std::string inputWinName);
	void SetFrames(std::vector<MyFrame> inputFrames);
	void SetPose(MyPose inputPose);
	void ClearFrames();
	void ClearPose();
	void DisplayFrames(bool isRotated);
	void DisplayFrames(bool isRotated, std::vector<std::string> serials);
	
};

