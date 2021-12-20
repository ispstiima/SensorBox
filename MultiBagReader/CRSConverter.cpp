#include "pch.h"
#include "CRSConverter.h"


CRSConverter::CRSConverter()
	: align_to_color(RS2_STREAM_COLOR)
	, align_to_depth(RS2_STREAM_DEPTH)
{
}

CRSConverter::~CRSConverter()
{
	if (ofs.is_open())
		ofs.close();
}


void CRSConverter::end_conversion_threads()
{
	worker.join();
}

void CRSConverter::set_converter(conversion_type cType, std::string inDestination, int idxPipe)
{
	referencePipe = idxPipe;
	destination = inDestination;
	conversionType = cType;

	if (conversionType == CONVERT_POSE) {
		// destination is a filename. Need to separate the path to create the BaseDir
		std::string BaseDir = destination.substr(0, destination.length() - 8);
		std::filesystem::create_directories(BaseDir);

		ofs.open(destination.c_str(), std::ofstream::out | std::ofstream::app);
		char firstLine[64];
		sprintf_s(firstLine, 64, "FrameID\tTimeStamp [ms]\tTx\tTy\tTz\tRw\tRx\tRy\tRz");
		ofs << firstLine << std::endl;
	}
	else // destination is a path to a directory
		std::filesystem::create_directories(inDestination);
}

/*void CRSConverter::set_frameset(rs2::frameset inFrameset, int inIFrameInFrameset)
{
	frameset = inFrameset;
	iFrameInFrameset = inIFrameInFrameset;
}*/

void CRSConverter::set_frameset(rs2::frameset inFrameset, rs2::stream_profile inStreamProfile)
{
	frameset = inFrameset;
	streamProfile = inStreamProfile;
}

void CRSConverter::set_frameID(int inFrameID)
{
	frameID = inFrameID;
}

int CRSConverter::get_referencePipe()
{
	return referencePipe;
}

void CRSConverter::start_conversion_threads()
{
	switch (conversionType)
	{
	case CONVERT_POSE:
		convert_pose();
		break;
	case CONVERT_PNG_COLOR:
		convert_png();
		break;
	case CONVERT_PNG_COLOR_ALIGNED:
		frameset = align_to_depth.process(frameset);
		convert_png();
		break;
	case CONVERT_PNG_DEPTH:
		convert_png();
		break;
	case CONVERT_PNG_DEPTH_ALIGNED:
		frameset = align_to_color.process(frameset);
		convert_png();
		break;
	case CONVERT_PNG_IR1:
		convert_png();
		break;
	case CONVERT_PNG_IR2:
		convert_png();
		break;
	case CONVERT_RAW_COLOR:
		convert_raw();
		break;
	case CONVERT_RAW_COLOR_ALIGNED:
		frameset = align_to_depth.process(frameset);
		convert_raw();
		break;
	case CONVERT_RAW_DEPTH:
		convert_raw();
		break;
	case CONVERT_RAW_DEPTH_ALIGNED:
		frameset = align_to_color.process(frameset);
		convert_raw();
		break;
	case CONVERT_RAW_IR1:
		convert_raw();
		break;
	case CONVERT_RAW_IR2:
		convert_raw();
		break;
	case CONVERT_CSV_DEPTH_METER:
		convert_csv();
		break;
	case CONVERT_CSV_DEPTH_METER_ALIGNED:
		frameset = align_to_color.process(frameset);
		convert_csv();
		break;
	case CONVERT_PLY:
		convert_ply();
		break;
	}
}

void CRSConverter::convert_png()
{
	start_worker( [this] {
		// Gather the correct frame from the frameset 
		for (int i = 0; i < frameset.size(); i++) {
			if (frameset[i].get_profile().stream_type() == streamProfile.stream_type() && frameset[i].get_profile().stream_index() == streamProfile.stream_index()) {
				iFrameInFrameset = i;
				break;
			}				
		}
		rs2::video_frame frame = frameset[iFrameInFrameset].as<rs2::video_frame>();

		if (frame) {
			// it is a depth frame, colorize it
			if (frame.get_profile().stream_type() == rs2_stream::RS2_STREAM_DEPTH) 
				frame = colorizer.process(frame);
					
			// Set the image name (destination is the path)
			char filenameC[256];
			sprintf_s(filenameC, "%s/FrameID%06d_TimeStamp%.0f.png", destination.c_str(), frameID, round(frame.get_timestamp()));
			std::string filenameS(filenameC);

			// Create image and write it with opencv
			if (frame.get_profile().stream_type() == rs2_stream::RS2_STREAM_INFRARED) {
				cv::Mat imageIR = cv::Mat(cv::Size(frame.get_width(), frame.get_height()), CV_8UC1, (void*)frame.get_data());
				cv::imwrite(filenameS, imageIR);
			}
			else {
				cv::Mat imageBGR(cv::Size(frame.get_width(), frame.get_height()), CV_8UC3, (void*)frame.get_data(), cv::Mat::AUTO_STEP), imageRGB;
				cv::cvtColor(imageBGR, imageRGB, cv::COLOR_BGR2RGB);
				cv::imwrite(filenameS, imageRGB);
			}
		}		
	});
}

void CRSConverter::convert_raw() 
{
	start_worker( [this] {
		// Gather the correct frame from the frameset
		for (int i = 0; i < frameset.size(); i++) {
			if (frameset[i].get_profile().stream_type() == streamProfile.stream_type() && frameset[i].get_profile().stream_index() == streamProfile.stream_index()) {
				iFrameInFrameset = i;
				break;
			}
		}
		rs2::video_frame frame = frameset[iFrameInFrameset].as<rs2::video_frame>();

		if (frame) {			
			// Set the name of the raw file (destination is the path)
			char filenameC[256];
			sprintf_s(filenameC, "%s/FrameID%06d_TimeStamp%.0f.raw", destination.c_str(), frameID, round(frame.get_timestamp()));

			// Initialize the rawfile and write the data
			std::ofstream fs(filenameC, std::ios::binary | std::ios::trunc);
			if (fs) { 
				fs.write(static_cast<const char *>(frame.get_data()), frame.get_stride_in_bytes() * frame.get_height());
				fs.flush();
			}
		}
	});
}

void CRSConverter::convert_csv() 
{
	start_worker([this] {
		// Gather the correct depth frame from the frameset
		for (int i = 0; i < frameset.size(); i++) {
			if (frameset[i].get_profile().stream_type() == rs2_stream::RS2_STREAM_DEPTH) {
				iFrameInFrameset = i;
				break;
			}				
		}
		rs2::depth_frame frame = frameset[iFrameInFrameset].as<rs2::depth_frame>();

		if (frame) {
			// Set the name of the csv file (destination is the path)
			char filenameC[256];
			sprintf_s(filenameC, "%s/FrameID%06d_TimeStamp%.0f.csv", destination.c_str(), frameID, round(frame.get_timestamp()));
			
			// Initialize the csv file and write the data
			std::ofstream fs(filenameC, std::ios::trunc);
			if (fs) {
				for (int y = 0; y < frame.get_height(); y++) {
					auto delim = "";
					for (int x = 0; x < frame.get_width(); x++) {
						fs << delim << frame.get_distance(x, y);
						delim = ",";
					}
					fs << '\n';
				}
				fs.flush();
			}
		}
	});
}

void CRSConverter::convert_ply()
{
	rs2::pointcloud pc; 
	start_worker( [this, pc]() mutable {	// pc is given to the worker. It makes the input mutable
		// Get both frames from the frameset
		rs2::depth_frame frameDepth = frameset.get_depth_frame();
		rs2::video_frame frameColor = frameset.get_color_frame();
		
		if (frameDepth && frameColor) {
			pc.map_to(frameColor);
			auto points = pc.calculate(frameDepth);

			// Set the image name (destination is the path)
			char filenameC[256];
			sprintf_s(filenameC, "%s/FrameID%06d_TimeStamp%.0f.ply", destination.c_str(), frameID, round(frameDepth.get_timestamp()));
			std::string filenameS(filenameC);
			
			// Export to ply with realsense library
			points.export_to_ply(filenameS, frameColor);			
		}
	});

}

void CRSConverter::convert_pose()
{
	start_worker([this] {
		if (ofs.is_open()) {

			// Get the correct frame in the frameset
			for (int i = 0; i < frameset.size(); i++) {
				if (frameset[i].get_profile().stream_type() == rs2_stream::RS2_STREAM_POSE) {
					iFrameInFrameset = i;
					break;
				}					
			}
			rs2::frame frame = frameset[iFrameInFrameset];

			// Cast the frame to pose_frame and get its data
			rs2_pose pose_data = frame.as<rs2::pose_frame>().get_pose_data();
			rs2_vector t = pose_data.translation;
			rs2_quaternion q = pose_data.rotation;

			// Write the results
			char PoseStr[256];
			sprintf_s(PoseStr, 256, "%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f", frame.get_timestamp(), t.x, t.y, t.z, q.w, q.x, q.y, q.z);
			ofs << frameID << "\t" << PoseStr << std::endl;
		}
	});
	
}

