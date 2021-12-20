#pragma once

#include <thread>
#include <string>
#include <sstream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "librealsense2/rs.hpp"
#include <filesystem>


enum conversion_type {
	CONVERT_POSE,
	CONVERT_PNG_COLOR,
	CONVERT_PNG_COLOR_ALIGNED,
	CONVERT_PNG_DEPTH,
	CONVERT_PNG_DEPTH_ALIGNED,
	CONVERT_PNG_IR1,
	CONVERT_PNG_IR2,
	CONVERT_RAW_COLOR,
	CONVERT_RAW_COLOR_ALIGNED,
	CONVERT_RAW_DEPTH,
	CONVERT_RAW_DEPTH_ALIGNED,
	CONVERT_RAW_IR1,
	CONVERT_RAW_IR2,
	CONVERT_CSV_DEPTH_METER,
	CONVERT_CSV_DEPTH_METER_ALIGNED,
	CONVERT_PLY
};

class CRSConverter
{
public:
	CRSConverter();
	virtual ~CRSConverter();

protected:
	rs2::frameset frameset;
	int iFrameInFrameset;
	int referencePipe;
	rs2::stream_profile streamProfile;

	std::string destination;
	conversion_type conversionType;
	int frameID;
	std::ofstream ofs;
	
	std::thread worker;
	rs2::colorizer colorizer;

	template <typename F> inline void start_worker(const F& f) {worker = std::thread(f);};

	rs2::align align_to_color, align_to_depth;

	
public:
	void end_conversion_threads();
	void set_converter(conversion_type cType, std::string inDestination, int idxPipe);
	//void set_frameset(rs2::frameset inFrameset, int inIFrameInFrameset);
	void set_frameset(rs2::frameset inFrameset, rs2::stream_profile inStreamProfile);
	void set_frameID(int inFrameID);

	int get_referencePipe();
	
	void start_conversion_threads();
	void convert_png();
	void convert_raw();
	void convert_csv();
	void convert_ply();
	void convert_pose();
};
