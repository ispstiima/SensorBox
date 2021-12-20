#include "pch.h"
#include "RSBagReader.h"
#include "MultiBagReaderDlg.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(CRSBagReader, CWinThread)


CRSBagReader::CRSBagReader()
	: bConversionLoop(false)
	, frameIncrement(1)
	, bConvertPose(false)
	, bConvertPngColor(false)
	, bConvertPngColorAligned(false)
	, bConvertPngDepth(false)
	, bConvertPngDepthAligned(false)
	, bConvertPngIr1(false)
	, bConvertPngIr2(false)
	, bConvertRawColor(false)
	, bConvertRawColorAligned(false)
	, bConvertRawDepth(false)
	, bConvertRawDepthAligned(false)
	, bConvertRawIr1(false)
	, bConvertRawIr2(false)
	, bConvertCsvDepthMeter(false)
	, bConvertCsvDepthMeterAligned(false)
	, bConvertPly(false)
{
	m_bAutoDelete = false;
}


CRSBagReader::~CRSBagReader()
{
}

BOOL CRSBagReader::InitInstance()
{
	return TRUE;
}

int CRSBagReader::ExitInstance()
{
	m_hThread = NULL;
	return CWinThread::ExitInstance();
}


BEGIN_MESSAGE_MAP(CRSBagReader, CWinThread)
END_MESSAGE_MAP()

BOOL CRSBagReader::Run()
{
	auto tic = std::chrono::high_resolution_clock::now();

	// Init the framesets for each pipe by reading the first elements
	// Framesets have more than one element (if multiple streams), but only the framenumber of the corresponding pipe is updated
	for (int i = 0; i < nPipes; i++) 
		myPipes[i].InitFramesets();

	// Find the maximum timestamp of the first frames of the input bag files. It is the start time tStart
	double tStart = 0, t0;
	for (int i = 0; i < nPipes; i++) {
		t0 = myPipes[i].get_timestamp();
		if (t0 > tStart)
			tStart = t0;
	}
	//tStart += 1000.0;	// Add 1 sec to be sure that all sensors were warm

	// Discard the initial frames with no correspondances before the tStart timestamp to synchronize the bag videos
	for (int i = 0; i < nPipes; i++) {
		if (myPipes[i].ReadFrameAt(tStart) == EOF) {
			bConversionLoop = false;
			break;
		}
	}

	// Read the synchronized frames using the reference timestamp of the first camera, until the end of the file
	int countFrames = 0;
	double t = tStart; // Evaluation time
	while (bConversionLoop)
	{
		// Display messages
		countFrames++;
		CString tString;
		tString.Format(L"Frame ID: %d\r\n", countFrames);
		AppendLogMessages(tString);
		for (int i = 0; i < nPipes; i++) {
			CString tString;
			tString.Format(L"Stream %d (Camera%d, %s)\tDelay from the target evaluation timestamp = %f [ms]\r\n", i, myPipes[i].get_referenceCamera(), 
				GetWChar(myPipes[i].get_streamProfile().stream_name().c_str()), myPipes[i].get_timestamp() - t);
			AppendLogMessages(tString);
			if (myPipes[i].get_timestamp() - t > 1000.0 / ((double)minFps)) {
				tString.Format(L"%s frame dropped by Camera%d!\r\n", GetWChar(myPipes[i].get_streamProfile().stream_name().c_str()), myPipes[i].get_referenceCamera());
				AppendLogMessages(tString);
			}
		}

		// Convert data
		for (int i = 0; i < Converters.size(); i++) {
			Converters[i]->set_frameset(myPipes[Converters[i]->get_referencePipe()].get_frameset(), myPipes[Converters[i]->get_referencePipe()].get_streamProfile());
			Converters[i]->set_frameID(countFrames);
			Converters[i]->start_conversion_threads();
		}
		for (int i = 0; i < Converters.size(); i++)
			Converters[i]->end_conversion_threads();

		/*double minTime = DBL_MAX;
		for (int i = 0; i < nPipes; i++) {
			double currTime = myPipes[i].get_timestamp();
			if (currTime < minTime)
				minTime = currTime;
		}
		// Set the new target evaluation time
		t = minTime + ((double) frameIncrement) * 1000.0 / ((double)minFps);*/

		// Set the new target evaluation time
		t += ((double)frameIncrement) * 1000.0 / ((double)minFps);

		// Read the new frames and checks if the playbacks have more data
		for (int i = 0; i < nPipes; i++) {
			if (myPipes[i].ReadFrameAt(t) == EOF) 
				bConversionLoop = false;

		}
		if (!bConversionLoop)
			break;
	}

	auto toc = std::chrono::high_resolution_clock::now();
	auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic);
	
	CString tString;
	tString.Format(L"Conversion completed: %d frame(s) converted in %.3f seconds\r\n", countFrames, timeElapsed.count()/1000.0);
	AppendLogMessages(tString);

	// Write the log messages in LogFile.txt
	CreateLogFile();

	// Reset as Stop button was clicked 
	ResetDialog();

	// Stop pipes
	for (int i = 0; i < nPipes; i++)
		myPipes[i].stop();

	// Exit thread
	ExitInstance();

	return TRUE;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //
// -------------------------------------------------------------------INITIALIZATION FUNCTIONS----------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

// InitBagFileNames: process the input filename, create the basename and a vector of names of bags
size_t CRSBagReader::InitBagFileNames()
{
	EmptyLogMessages();
	BaseFileName.clear();

	if (bagFileNames.size() > 0)
		bagFileNames.clear();

	if (!strstr(refCamFileName.c_str(), ".bag")) {
		refCamFileName = refCamFileName + ".bag";
		
		CString tString;
		tString.Format(L"Missing \".bag\" extension to the name of the base file. Considering %s...\r\n", GetWChar(refCamFileName.c_str()));
		AppendLogMessages(tString);
	}
		
	if (!strstr(refCamFileName.c_str(), "_IntelRealSense") && !strstr(refCamFileName.c_str(), "_SN")) {
		// Generic input files. Only 1 camera expected!
		BaseFileName = refCamFileName.substr(0, refCamFileName.length() - 4);
		bagFileNames.push_back(inputPath + refCamFileName);

		CString tString;
		tString.Format(L"Generic input bag\r\nLoading \"%s\"\r\n", GetWChar(refCamFileName.c_str()));
		AppendLogMessages(tString);
	}
	else {
		// Bag files are from SensorBox
		BaseFileName = refCamFileName.substr(0, refCamFileName.length() - 38);
		bagFileNames.push_back(inputPath + refCamFileName);

		CString tString;
		tString.Format(L"Input bag(s) from the SensorBox\r\nLoading \"%s\" as Camera0 (with reference basename)\r\n", GetWChar(refCamFileName.c_str()));
		AppendLogMessages(tString);

		int indCam = 1;
		for (auto& p : std::filesystem::directory_iterator(inputPath))
		{
			std::string tempFileName = p.path().filename().generic_string();
			if (tempFileName.find(BaseFileName) == 0 && tempFileName.find("_IntelRealSense") == BaseFileName.length()
				&& tempFileName.find("_SN") == BaseFileName.length() + 19 && tempFileName.find(".bag") != std::string::npos &&
				tempFileName.compare(refCamFileName) != 0 && tempFileName.find("_SN") == tempFileName.length() - 19)
			{
				tString.Format(L"Loading \"%s\" as Camera%d\r\n", GetWChar(tempFileName.c_str()), indCam);
				AppendLogMessages(tString);
				bagFileNames.push_back(inputPath + tempFileName);
				indCam++;
			}
		}
	}

	return bagFileNames.size();
}

BOOL CRSBagReader::InitPipes()	// TODO: Add control on pipe exceptions
{
	myPipes.clear();
	int iMinFps = -1;
	minFps = INT_MAX;
	
	// First find the number of streams in each bag file
	int *nStreams;
	nStreams = new int[bagFileNames.size()];
	for (int i = 0; i < bagFileNames.size(); i++) {
		// Define the configuration to accept bags
		rs2::config cfg;
		cfg.enable_device_from_file(bagFileNames[i]);

		// create a temporary pipe and start it
		CMyPipeline tPipe;
		tPipe.start(cfg);
		// Gather the stream profiles in the bag
		std::vector<rs2::stream_profile> streamProfiles = tPipe.get_active_profile().get_streams();
		nStreams[i] = streamProfiles.size();
		// Stop the pipe
		tPipe.stop();
	}

	// Load every bag files nStreams times to assign SEPARATELY each stream to a pipeline (no copy constructor!)
	for (int i = 0; i < bagFileNames.size(); i++) {
		for (int s = 0; s < nStreams[i]; s++) {
			// Define the configuration to accept bags
			rs2::config cfg;
			cfg.enable_device_from_file(bagFileNames[i]);

			// create a temporary pipe and start it
			CMyPipeline tPipe;
			tPipe.start(cfg);
		
			// Set the device as a playback, pause it and remove realtime playing option (fundamental!)
			rs2::playback playback = tPipe.get_device().as<rs2::playback>();
			//playback.set_real_time(false);
			playback.pause();
			

			// Gather the stream profiles (size = S) in the pipe, update the internal variables of tPipe 
			// with data from each stream profile, and push back them to the main vector of pipes (myPipes) SEPARATELY
			std::vector<rs2::stream_profile> streamProfiles = tPipe.get_active_profile().get_streams();
			tPipe.set_streamProfile(streamProfiles[s]);
			tPipe.set_bagFileName(bagFileNames[i]);
			tPipe.set_referenceCamera(i);
			myPipes.push_back(tPipe);			
		}
	}
	nPipes = myPipes.size();
	delete[] nStreams;

	// Set the minimum framerate and give notification of streams
	for (int i = 0; i < nPipes; i++) {

		std::string serial = myPipes[i].get_device().get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		std::string name = myPipes[i].get_device().get_info(RS2_CAMERA_INFO_NAME);

		CString tString;
		tString.Format(L"Stream %d - %s\tCamera%d: %s, SN %s\r\n", i, GetWChar(myPipes[i].get_streamProfile().stream_name().c_str()),
			myPipes[i].get_referenceCamera(), GetWChar(name.c_str()), GetWChar(serial.c_str()));
		AppendLogMessages(tString);

		if (myPipes[i].get_fps() < minFps) {
			minFps = myPipes[i].get_fps();
			iMinFps = i;
		}
	}
	//  Give notification of the minimum framerate
	std::string nameMin = myPipes[iMinFps].get_bagFileName().substr(inputPath.length(), myPipes[iMinFps].get_bagFileName().length() - inputPath.length());
	CString tString;
	tString.Format(L"Setting the global framerate to the minimum value %d fps (from \"%s\")\r\n", myPipes[iMinFps].get_fps(), GetWChar(nameMin.c_str()));
	AppendLogMessages(tString);
	
	return FALSE;
}

BOOL CRSBagReader::InitConverters()
{
	// Init Output Directory
	BaseDir = inputPath + BaseFileName + currentDateTime();
	CString tString;
	tString.Format(L"Destination folder: \"%s\"\r\n", GetWChar(BaseDir.c_str()));
	AppendLogMessages(tString);

	// Inizializations
	Converters.clear();
	for (int i = 0; i < nPipes; i++) {
		std::string serial = myPipes[i].get_device().get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

		if (myPipes[i].get_streamProfile().stream_type() == RS2_STREAM_COLOR) {
			if (bConvertPngColor) {
				std::string subDirectory = BaseDir + "/Color/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PNG_COLOR, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertPngColorAligned) {
				std::string subDirectory = BaseDir + "/ColorAligned/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PNG_COLOR_ALIGNED, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertRawColor) {
				std::string subDirectory = BaseDir + "/ColorRaw/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_RAW_COLOR, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertRawColorAligned) {
				std::string subDirectory = BaseDir + "/ColorRawAligned/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_RAW_COLOR_ALIGNED, subDirectory, i);
				Converters.push_back(tConverter);
			}
		}
		else if (myPipes[i].get_streamProfile().stream_type() == RS2_STREAM_DEPTH) {
			if (bConvertPngDepth) {
				std::string subDirectory = BaseDir + "/Depth/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PNG_DEPTH, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertPngDepthAligned) {
				std::string subDirectory = BaseDir + "/DepthAligned/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PNG_DEPTH_ALIGNED, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertRawDepth) {
				std::string subDirectory = BaseDir + "/DepthRaw/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_RAW_DEPTH, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertRawDepthAligned) {
				std::string subDirectory = BaseDir + "/DepthRawAligned/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_RAW_DEPTH_ALIGNED, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertCsvDepthMeter) {
				std::string subDirectory = BaseDir + "/DepthMeter/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_CSV_DEPTH_METER, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertCsvDepthMeterAligned) {
				std::string subDirectory = BaseDir + "/DepthMeterAligned/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_CSV_DEPTH_METER_ALIGNED, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertPly) {
				std::string subDirectory = BaseDir + "/PointCloud/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PLY, subDirectory, i);
				Converters.push_back(tConverter);
			}
		}
		else if (myPipes[i].get_streamProfile().stream_type() == RS2_STREAM_INFRARED && myPipes[i].get_streamProfile().stream_index() == 1) {
			if (bConvertPngIr1) {
				std::string subDirectory = BaseDir + "/InfraredLeft/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PNG_IR1, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertRawIr1) {
				std::string subDirectory = BaseDir + "/InfraredLeftRaw/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_RAW_IR1, subDirectory, i);
				Converters.push_back(tConverter);
			}
		}
		else if (myPipes[i].get_streamProfile().stream_type() == RS2_STREAM_INFRARED && myPipes[i].get_streamProfile().stream_index() == 2) {
			if (bConvertPngIr2) {
				std::string subDirectory = BaseDir + "/InfraredRight/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_PNG_IR2, subDirectory, i);
				Converters.push_back(tConverter);
			}
			if (bConvertRawIr2) {
				std::string subDirectory = BaseDir + "/InfraredRightRaw/" + serial;
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_RAW_IR2, subDirectory, i);
				Converters.push_back(tConverter);
			}
		}
		else if (myPipes[i].get_streamProfile().stream_type() == RS2_STREAM_POSE) {
			if (bConvertPose) {
				std::string PoseFileName = BaseDir + "/Pose.txt";
				std::shared_ptr<CRSConverter> tConverter;
				tConverter = std::make_shared<CRSConverter>();
				tConverter->set_converter(CONVERT_POSE, PoseFileName, i);
				Converters.push_back(tConverter);
			}
		}
	}

	// Check if converters are created
	if (Converters.size() == 0) {
		AfxMessageBox(L"Empty converters!\nPlease select an output format");
		return TRUE;
	}
	   
	return FALSE;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //
// -----------------------------------------------------------------INTERFACE/DISPLAY FUNCTIONS---------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

void CRSBagReader::SetContainer(void* Container)
{
	p_Container = Container;
}

void CRSBagReader::EnableCheckOptions()
{
	CMultiBagReaderDlg *pDlg = (CMultiBagReaderDlg *)p_Container;
	bool hasColor = false;
	bool hasDepth = false;

	for (int i = 0; i < nPipes; i++) {
		CMyPipeline myPipe;
		myPipe = myPipes[i];

		if (myPipe.get_streamProfile().stream_type() == RS2_STREAM_POSE) {
			bConvertPose = true;
			pDlg->m_checkPose.SetCheck(BST_CHECKED);
			pDlg->m_checkPose.EnableWindow(TRUE);
		}
		else if (myPipe.get_streamProfile().stream_type() == RS2_STREAM_COLOR) {
			hasColor = true;
			bConvertPngColor = true;
			pDlg->m_checkPngColor.SetCheck(BST_CHECKED);
			pDlg->m_checkPngColor.EnableWindow(TRUE);
			bConvertRawColor = false;
			pDlg->m_checkRawColor.SetCheck(BST_UNCHECKED);
			pDlg->m_checkRawColor.EnableWindow(TRUE);
			bConvertPngColorAligned = false;
			pDlg->m_checkPngColorAligned.SetCheck(BST_UNCHECKED);
			pDlg->m_checkPngColorAligned.EnableWindow(TRUE);
			bConvertRawColorAligned = false;
			pDlg->m_checkRawColorAligned.SetCheck(BST_UNCHECKED);
			pDlg->m_checkRawColorAligned.EnableWindow(TRUE);

		}
		else if (myPipe.get_streamProfile().stream_type() == RS2_STREAM_DEPTH) {
			hasDepth = true;
			bConvertPngDepth = true;
			pDlg->m_checkPngDepth.SetCheck(BST_CHECKED);
			pDlg->m_checkPngDepth.EnableWindow(TRUE);
			bConvertPngDepthAligned = false;
			pDlg->m_checkPngDepthAligned.SetCheck(BST_UNCHECKED);
			pDlg->m_checkPngDepthAligned.EnableWindow(TRUE);
			bConvertRawDepth = false;
			pDlg->m_checkRawDepth.SetCheck(BST_UNCHECKED);
			pDlg->m_checkRawDepth.EnableWindow(TRUE);
			bConvertRawDepthAligned = false;
			pDlg->m_checkRawDepthAligned.SetCheck(BST_UNCHECKED);
			pDlg->m_checkRawDepthAligned.EnableWindow(TRUE);
			bConvertCsvDepthMeter = false;
			pDlg->m_checkCsvDepthMeter.SetCheck(BST_UNCHECKED);
			pDlg->m_checkCsvDepthMeter.EnableWindow(TRUE);
			bConvertCsvDepthMeterAligned = false;
			pDlg->m_checkCsvDepthMeterAligned.SetCheck(BST_UNCHECKED);
			pDlg->m_checkCsvDepthMeterAligned.EnableWindow(TRUE);
		}
		else if (myPipe.get_streamProfile().stream_type() == RS2_STREAM_INFRARED) {
			if (myPipe.get_streamProfile().stream_index() == 1) {
				bConvertPngIr1 = false;
				pDlg->m_checkPngIr1.SetCheck(BST_UNCHECKED);
				pDlg->m_checkPngIr1.EnableWindow(TRUE);
				bConvertRawIr1 = false;
				pDlg->m_checkRawIr1.SetCheck(BST_UNCHECKED);
				pDlg->m_checkRawIr1.EnableWindow(TRUE);
			}
			if (myPipe.get_streamProfile().stream_index() == 2) {
				bConvertPngIr2 = false;
				pDlg->m_checkPngIr2.SetCheck(BST_UNCHECKED);
				pDlg->m_checkPngIr2.EnableWindow(TRUE);
				bConvertRawIr2 = false;
				pDlg->m_checkRawIr2.SetCheck(BST_UNCHECKED);
				pDlg->m_checkRawIr2.EnableWindow(TRUE);
			}
		}
	}

	if (hasColor && hasDepth) {
		bConvertPly = false;
		pDlg->m_checkPly.SetCheck(BST_UNCHECKED);
		pDlg->m_checkPly.EnableWindow(TRUE);
	}
}

void CRSBagReader::AppendLogMessages(LPCTSTR Message)
{
	CMultiBagReaderDlg *pDlg = (CMultiBagReaderDlg *)p_Container;
	
	// get the initial text length
	int nLength = pDlg->m_editLog.GetWindowTextLength();
	// put the selection at the end of text
	pDlg->m_editLog.SetSel(nLength, nLength);
	// replace the selection
	pDlg->m_editLog.ReplaceSel(Message);
}

void CRSBagReader::EmptyLogMessages()
{
	CMultiBagReaderDlg *pDlg = (CMultiBagReaderDlg *)p_Container;
	pDlg->m_editLog.SetWindowText(L"");
}

void CRSBagReader::ResetDialog()
{
	CMultiBagReaderDlg *pDlg = (CMultiBagReaderDlg *)p_Container;

	// Reset dialog
	pDlg->GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	pDlg->GetDlgItem(IDC_START)->EnableWindow(FALSE);
	pDlg->GetDlgItem(IDC_SETREFERENCE)->EnableWindow(TRUE);
	inputPath = "";
	refCamFileName = "";
	pDlg->InitializeCheckControls();
}

void CRSBagReader::CreateLogFile()
{
	CMultiBagReaderDlg *pDlg = (CMultiBagReaderDlg *)p_Container;

	//Create a log file with all messages
	std::string LogFileName = BaseDir + "/LogFile.txt";
	std::ofstream ofsLog(LogFileName);
	if (ofsLog.is_open()) {
		CString tString;
		pDlg->m_editLog.GetWindowTextW(tString);
		ofsLog << CW2A(tString);
		ofsLog.close();
	}
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //
// ------------------------------------------------------------------GENERAL PURPOSE FUNCTIONS----------------------------------------------------------------- //
// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

const wchar_t * CRSBagReader::GetWChar(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* outWChar = new wchar_t[cSize];
	size_t outSize;
	mbstowcs_s(&outSize, outWChar, cSize, c, cSize - 1);

	return outWChar;
}

const std::string CRSBagReader::currentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	localtime_s(&tstruct, &now);
	char       buf[80];
	strftime(buf, sizeof(buf), "_%Y%m%d_%H%M%S", &tstruct);
	return buf;
}
