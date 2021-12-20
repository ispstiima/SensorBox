#pragma once

#include <afxwin.h>
#include <vector>
#include <librealsense2\rs.hpp>
#include "MyPipeline.h"
#include "CRSConverter.h"


class CRSBagReader : public CWinThread
{
	DECLARE_DYNCREATE(CRSBagReader)

public:
	CRSBagReader();
	virtual ~CRSBagReader();

	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	virtual BOOL Run();

protected:
	DECLARE_MESSAGE_MAP()

public:	// List of functions
	size_t InitBagFileNames();
	BOOL InitPipes();
	BOOL InitConverters();
	void SetContainer(void* Container);
	void AppendLogMessages(LPCTSTR Message);
	void EmptyLogMessages();
	void ResetDialog();
	void CreateLogFile();
	void EnableCheckOptions();
	const std::string currentDateTime();
	const wchar_t * CRSBagReader::GetWChar(const char *c);

public:	// List of variables
	// I/O variables
	std::string inputPath, refCamFileName, BaseFileName, BaseDir;
	void *p_Container;

	// RS variables
	std::vector<CMyPipeline> myPipes;
	size_t nPipes;
	std::vector<std::string> bagFileNames;
	int minFps;

	// Conversion variables
	std::vector<std::shared_ptr<CRSConverter>> Converters;
	bool bConversionLoop;

	// Save parameters
	int frameIncrement;
	bool bConvertPose;
	bool bConvertPngColor;
	bool bConvertPngColorAligned;
	bool bConvertPngDepth;
	bool bConvertPngDepthAligned;
	bool bConvertPngIr1;
	bool bConvertPngIr2;
	bool bConvertRawColor;
	bool bConvertRawColorAligned;
	bool bConvertRawDepth;
	bool bConvertRawDepthAligned;
	bool bConvertRawIr1;
	bool bConvertRawIr2;
	bool bConvertCsvDepthMeter;
	bool bConvertCsvDepthMeterAligned;
	bool bConvertPly;
};

