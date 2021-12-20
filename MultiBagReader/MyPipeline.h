#pragma once

#include <librealsense2\rs.hpp>


class CMyPipeline : public rs2::pipeline
{ 
protected:
	rs2::stream_profile streamProfile;
	rs2::frameset frameset;
	size_t framenumber, firstFramenumber;
	std::string bagFileName;
	int referenceCamera;
	double timestamp;
	int fps;
	
	void update_framenumber();
	void update_timestamp();	
	

public:
	CMyPipeline();
	virtual ~CMyPipeline();

	int InitFramesets();
	int ReadFrameAt(double timestamp);
	
	rs2::device get_device();
	rs2::frame get_frame();
	rs2::frameset get_frameset();
	int get_iFrameInFrameset();

	rs2::stream_profile get_streamProfile();
	std::string get_bagFileName();
	int get_referenceCamera();
	double get_timestamp();
	int get_fps();
		
	void set_streamProfile(rs2::stream_profile inStreamProfile);
	void set_bagFileName(std::string inFilename);
	void set_referenceCamera(int inRefCamera);
	void set_framenumber(size_t inFramenumber);

};

