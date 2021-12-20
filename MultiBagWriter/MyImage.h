#pragma once

#include <librealsense2/rs.hpp>     // Include RealSense Cross Platform API
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <wtypes.h>


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

	enum ImageType
	{
		IMAGE_COLOR,
		IMAGE_DEPTH,
		IMAGE_INFRARED
	};

	struct MyFrame {
		std::string SerialNumber;
		double TimeStamp;
		cv::Mat Image;
		ImageType Type;
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

	cv::String m_winColorName, m_winDepthName;
	//std::vector<MyFrame> m_Frames;
	std::vector<rs2::frameset> m_Framesets;
	std::string destinationFolder;
	rs2::align align_to_color, align_to_depth;
	rs2::colorizer colorizer;
	int iFrame;
	bool isRotated;
		
	BOOL bWinOpen;
	int dispImageW, dispImageH, singleImageW, singleImageH, scrnW, scrnH, m_nCams;

	void InitDisplay(int nCams);
	void SetWindows();
	void RotateDisplay();
	void ExitDisplay();
	HWND GetWindowHandle(std::string inputWinName);
	//void SetFramesets(std::vector<rs2::frameset> inputFramesets);
	void SetPose(MyPose inputPose);
	void ClearPose();
	void DisplayFramesets(rs2_stream requiredStream, rs2::frameset *inFramesets, std::vector<std::string> serials);
	void SaveFrameset(rs2::frameset CurrFrameset);
	void SavePng(rs2::frameset CurrFrameset, rs2_stream stream_type, bool isAligned);
	void SaveDepthRaw(rs2::frameset CurrFrameset, bool isAligned);
	void SaveDepthCsv(rs2::frameset CurrFrameset, bool isAligned);
	void SavePly(rs2::frameset CurrFrameset);
};

