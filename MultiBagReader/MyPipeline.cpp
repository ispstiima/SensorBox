#include "pch.h"
#include "MyPipeline.h"


CMyPipeline::CMyPipeline()
	: framenumber(0ULL)
	, firstFramenumber(0ULL)
{
}

CMyPipeline::~CMyPipeline()
{
}


// ReadFrame: read the next frame of a bag-file-playback-pipeline
int CMyPipeline::InitFramesets()
{
	rs2::playback playback = get_device().as<rs2::playback>();
	playback.resume();
	for (auto i = 0; i < 30; ++i)
		frameset = wait_for_frames();
	playback.pause();

	update_framenumber();
	update_timestamp();
	firstFramenumber = framenumber;
	return 0;
}


int CMyPipeline::ReadFrameAt(double targetTimestamp)
{
	int nRet = 0;
	auto device = get_device();
	rs2::playback playback = device.as<rs2::playback>();

	playback.resume();

	// Keep reading the frames till the new timestamp of the frameset is >= targetTimestamp or till the file ends
	while (timestamp < targetTimestamp && nRet != EOF) {
		frameset = wait_for_frames();
		size_t newFramenumber = get_frame().get_frame_number();

		if (newFramenumber < firstFramenumber) {
 			nRet = EOF;
			break;
		}
		else {
			update_framenumber();
			update_timestamp();
		}
	}
	playback.pause();
	return nRet;

	/*if (timestamp >= targetTimestamp - 1000.0/((double)fps))
		return 0;
	else
		return EOF;*/
}

rs2::frame CMyPipeline::get_frame()
{
	rs2::frame tFrame;
	
	for (int i = 0; i < frameset.size(); i++) {
		if (frameset[i].get_profile().stream_type() == streamProfile.stream_type()) {
			if (streamProfile.stream_type() == rs2_stream::RS2_STREAM_INFRARED) {
				if (frameset[i].get_profile().stream_index() == streamProfile.stream_index())
					return tFrame = frameset[i];
			}				
			else
				return tFrame = frameset[i];
		}			
	}
	return tFrame;
}

rs2::frameset CMyPipeline::get_frameset()
{
	return frameset;
}

int CMyPipeline::get_iFrameInFrameset()
{
	for (int i = 0; i < frameset.size(); i++) {
		if (frameset[i].get_profile().stream_type() == streamProfile.stream_type() && frameset[i].get_profile().stream_index() == streamProfile.stream_index())
			return i;
	}
				
}

void CMyPipeline::update_framenumber()
{
	framenumber = get_frame().get_frame_number();
}

void CMyPipeline::update_timestamp()
{
	timestamp = get_frame().get_timestamp();
}

rs2::stream_profile CMyPipeline::get_streamProfile()
{
	return streamProfile;
}

rs2::device CMyPipeline::get_device()
{
	return get_active_profile().get_device();
}

std::string CMyPipeline::get_bagFileName()
{
	return bagFileName;
}

int CMyPipeline::get_referenceCamera()
{
	return referenceCamera;
}

double CMyPipeline::get_timestamp()
{
	return timestamp;
}

int CMyPipeline::get_fps()
{
	return fps;
}

void CMyPipeline::set_framenumber(size_t inFramenumber)
{
	framenumber = inFramenumber;
}

void CMyPipeline::set_streamProfile(rs2::stream_profile inStreamProfile)
{
	streamProfile = inStreamProfile;
	fps = inStreamProfile.fps();
}

void CMyPipeline::set_bagFileName(std::string inFilename)
{
	bagFileName = inFilename;
}

void CMyPipeline::set_referenceCamera(int inRefCamera)
{
	referenceCamera = inRefCamera;
}

