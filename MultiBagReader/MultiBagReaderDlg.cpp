
// MultiBagReaderDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MultiBagReader.h"
#include "MultiBagReaderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMultiBagReaderDlg dialog



CMultiBagReaderDlg::CMultiBagReaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MULTIBAGREADER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiBagReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECKPOSE, m_checkPose);
	DDX_Control(pDX, IDC_CHECKPNGCOLOR, m_checkPngColor);
	DDX_Control(pDX, IDC_CHECKPNGDEPTH, m_checkPngDepth);
	DDX_Control(pDX, IDC_CHECKPNGIR1, m_checkPngIr1);
	DDX_Control(pDX, IDC_CHECKPNGIR2, m_checkPngIr2);
	DDX_Control(pDX, IDC_CHECKPNGCOLORALIGNED, m_checkPngColorAligned); 
	DDX_Control(pDX, IDC_CHECKPNGDEPTHALIGNED, m_checkPngDepthAligned);
	DDX_Control(pDX, IDC_CHECKRAWCOLOR, m_checkRawColor);
	DDX_Control(pDX, IDC_CHECKRAWDEPTH, m_checkRawDepth);
	DDX_Control(pDX, IDC_CHECKRAWCOLORALIGNED, m_checkRawColorAligned);
	DDX_Control(pDX, IDC_CHECKRAWDEPTHALIGNED, m_checkRawDepthAligned);
	DDX_Control(pDX, IDC_CHECKRAWIR1, m_checkRawIr1);
	DDX_Control(pDX, IDC_CHECKRAWIR2, m_checkRawIr2);
	DDX_Control(pDX, IDC_CHECKCSVDEPTH, m_checkCsvDepthMeter);
	DDX_Control(pDX, IDC_CHECKCSVDEPTHALIGNED, m_checkCsvDepthMeterAligned);
	DDX_Control(pDX, IDC_CHECKPC, m_checkPly);
	DDX_Control(pDX, IDC_EDITLOG, m_editLog);
	DDX_Control(pDX, IDC_EDITINPUTPATH, m_editFilePathAndName);
	DDX_Control(pDX, IDC_EDITFRAMEINCREMENT, m_editFrameIncrement);
	DDX_Control(pDX, IDC_SPINFRAMEINCREMENT, m_spinFrameIncrement);
}

BEGIN_MESSAGE_MAP(CMultiBagReaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SETREFERENCE, &CMultiBagReaderDlg::OnBnClickedSetreference)
	ON_BN_CLICKED(IDC_START, &CMultiBagReaderDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CMultiBagReaderDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_CHECKPC, &CMultiBagReaderDlg::OnBnClickedCheckpc)
	ON_BN_CLICKED(IDC_CHECKCSVDEPTH, &CMultiBagReaderDlg::OnBnClickedCheckcsvdepth)
	ON_BN_CLICKED(IDC_CHECKCSVDEPTHALIGNED, &CMultiBagReaderDlg::OnBnClickedCheckcsvdepthaligned)
	ON_BN_CLICKED(IDC_CHECKPNGCOLOR, &CMultiBagReaderDlg::OnBnClickedCheckpngcolor)
	ON_BN_CLICKED(IDC_CHECKPNGDEPTH, &CMultiBagReaderDlg::OnBnClickedCheckpngdepth)
	ON_BN_CLICKED(IDC_CHECKPNGDEPTHALIGNED, &CMultiBagReaderDlg::OnBnClickedCheckpngdepthaligned)
	ON_BN_CLICKED(IDC_CHECKRAWCOLOR, &CMultiBagReaderDlg::OnBnClickedCheckrawcolor)
	ON_BN_CLICKED(IDC_CHECKRAWDEPTH, &CMultiBagReaderDlg::OnBnClickedCheckrawdepth)
	ON_BN_CLICKED(IDC_CHECKRAWDEPTHALIGNED, &CMultiBagReaderDlg::OnBnClickedCheckrawdepthaligned)
	ON_BN_CLICKED(IDC_CHECKPOSE, &CMultiBagReaderDlg::OnBnClickedCheckpose)
	ON_EN_CHANGE(IDC_EDITFRAMEINCREMENT, &CMultiBagReaderDlg::OnEnChangeEditframeincrement)
	ON_BN_CLICKED(IDC_CHECKPNGIR1, &CMultiBagReaderDlg::OnBnClickedCheckpngir1)
	ON_BN_CLICKED(IDC_CHECKPNGIR2, &CMultiBagReaderDlg::OnBnClickedCheckpngir2)
	ON_BN_CLICKED(IDC_CHECKRAWIR1, &CMultiBagReaderDlg::OnBnClickedCheckrawir1)
	ON_BN_CLICKED(IDC_CHECKRAWIR2, &CMultiBagReaderDlg::OnBnClickedCheckrawir2)
	ON_BN_CLICKED(IDC_CHECKPNGCOLORALIGNED, &CMultiBagReaderDlg::OnBnClickedCheckpngcoloraligned)
	ON_BN_CLICKED(IDC_CHECKRAWCOLORALIGNED, &CMultiBagReaderDlg::OnBnClickedCheckrawcoloraligned)
END_MESSAGE_MAP()


// CMultiBagReaderDlg message handlers

BOOL CMultiBagReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	BagReader.SetContainer(this);

	// Initialize output data and corresponding check buttons
	InitializeCheckControls();

	// Init Frame Increment
	m_editFrameIncrement.SetWindowText(L"1");
	m_spinFrameIncrement.SetRange(1, 10000);
	m_spinFrameIncrement.SetBuddy(GetDlgItem(IDC_EDITFRAMEINCREMENT));
	m_spinFrameIncrement.SetPos(BagReader.frameIncrement);

	// Set increment by 1
	UDACCEL AccellValue1;
	AccellValue1.nInc = 1;   
	m_spinFrameIncrement.SetAccel(1, &AccellValue1);

	// Increase the text limit to the maximum
	m_editLog.SetLimitText(0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMultiBagReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMultiBagReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMultiBagReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMultiBagReaderDlg::OnCancel()
{
	BagReader.AppendLogMessages(L"Exit converison program.\r\n");
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	BagReader.bConversionLoop = false;
	
	CDialog::OnCancel();
}

void CMultiBagReaderDlg::OnBnClickedSetreference()
{
	BagReader.ResetDialog();
	m_FilePathAndName.Format(L"");
	m_editFilePathAndName.SetWindowTextW(m_FilePathAndName);

	CFileDialog FileDlg(TRUE, L"", L"*.bag");
	FileDlg.m_ofn.lpstrTitle = L"Set the reference bag file";
	if (FileDlg.DoModal() == IDOK) {
		m_FilePathAndName = FileDlg.GetPathName();
		m_editFilePathAndName.SetWindowTextW(m_FilePathAndName);

		// Separate the folder path
		CString folderPath = m_FilePathAndName.Left(m_FilePathAndName.GetLength() - FileDlg.GetFileName().GetLength());
		// Convert a TCHAR string to a LPCSTR and assign to inputPath
		CT2CA pszConvertedFolder(folderPath);
		BagReader.inputPath = pszConvertedFolder;

		// Assigne the name of the refeference camera bag
		// Convert a TCHAR string to a LPCSTR and assign to refCamFileName
		CT2CA pszConvertedFile(FileDlg.GetFileName());
		BagReader.refCamFileName = pszConvertedFile;
		
		if (BagReader.InitBagFileNames() < 0) {
			AfxMessageBox(L"Cannot Open File");
			return;
		}
		if (BagReader.InitPipes()) {
			AfxMessageBox(L"Cannot Init RealSense Pipelines");
			return;
		}
		
		BagReader.EnableCheckOptions();
		
		GetDlgItem(IDC_EDITFRAMEINCREMENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPINFRAMEINCREMENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);

	}
	else
		AfxMessageBox(L"Cannot Open File");
		
}


void CMultiBagReaderDlg::OnBnClickedStart()
{
	if (BagReader.InitConverters())
		return;
	else {
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPOSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKCSVDEPTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKCSVDEPTHALIGNED)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPC)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPNGCOLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPNGCOLORALIGNED)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPNGDEPTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPNGDEPTHALIGNED)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPNGIR1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKPNGIR2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKRAWCOLOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKRAWCOLORALIGNED)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKRAWDEPTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKRAWDEPTHALIGNED)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKRAWIR1)->EnableWindow(FALSE); 
		GetDlgItem(IDC_CHECKRAWIR2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITFRAMEINCREMENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPINFRAMEINCREMENT)->EnableWindow(FALSE);

		BagReader.bConversionLoop = true;
		BagReader.CreateThread();
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_SETREFERENCE)->EnableWindow(FALSE);
	}
}


void CMultiBagReaderDlg::OnBnClickedStop()
{
	BagReader.AppendLogMessages(L"Conversion stopped by user\r\n");
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	BagReader.bConversionLoop = false;
}

void CMultiBagReaderDlg::InitializeCheckControls()
{
	BagReader.bConvertPose = false;
	BagReader.bConvertPngColor = false;
	BagReader.bConvertPngColorAligned = false;
	BagReader.bConvertPngDepth = false;
	BagReader.bConvertPngDepthAligned = false;
	BagReader.bConvertPngIr1 = false;
	BagReader.bConvertPngIr2 = false;
	BagReader.bConvertRawColor = false;
	BagReader.bConvertRawColorAligned = false;
	BagReader.bConvertRawDepth = false;
	BagReader.bConvertRawDepthAligned = false;
	BagReader.bConvertRawIr1 = false;
	BagReader.bConvertRawIr2 = false;
	BagReader.bConvertCsvDepthMeter = false;
	BagReader.bConvertCsvDepthMeterAligned = false;
	BagReader.bConvertPly = false;

	m_checkPose.SetCheck(BST_UNCHECKED);
	m_checkPngColor.SetCheck(BST_UNCHECKED);
	m_checkPngColorAligned.SetCheck(BST_UNCHECKED);
	m_checkPngDepth.SetCheck(BST_UNCHECKED);
	m_checkPngDepthAligned.SetCheck(BST_UNCHECKED);
	m_checkPngIr1.SetCheck(BST_UNCHECKED);
	m_checkPngIr2.SetCheck(BST_UNCHECKED);
	m_checkRawColor.SetCheck(BST_UNCHECKED);
	m_checkRawColorAligned.SetCheck(BST_UNCHECKED);
	m_checkRawDepth.SetCheck(BST_UNCHECKED);
	m_checkRawDepthAligned.SetCheck(BST_UNCHECKED);
	m_checkRawIr1.SetCheck(BST_UNCHECKED);
	m_checkRawIr2.SetCheck(BST_UNCHECKED);
	m_checkCsvDepthMeter.SetCheck(BST_UNCHECKED);
	m_checkCsvDepthMeterAligned.SetCheck(BST_UNCHECKED);
	m_checkPly.SetCheck(BST_UNCHECKED);
}


void CMultiBagReaderDlg::OnBnClickedCheckpc()
{
	BagReader.bConvertPly = !BagReader.bConvertPly;
}


void CMultiBagReaderDlg::OnBnClickedCheckcsvdepth()
{
	BagReader.bConvertCsvDepthMeter = !BagReader.bConvertCsvDepthMeter;
}


void CMultiBagReaderDlg::OnBnClickedCheckcsvdepthaligned()
{
	BagReader.bConvertCsvDepthMeterAligned = !BagReader.bConvertCsvDepthMeterAligned;
}


void CMultiBagReaderDlg::OnBnClickedCheckpngcolor()
{
	BagReader.bConvertPngColor = !BagReader.bConvertPngColor;
}


void CMultiBagReaderDlg::OnBnClickedCheckpngdepth()
{
	BagReader.bConvertPngDepth = !BagReader.bConvertPngDepth;
}


void CMultiBagReaderDlg::OnBnClickedCheckpngdepthaligned()
{
	BagReader.bConvertPngDepthAligned = !BagReader.bConvertPngDepthAligned;
}


void CMultiBagReaderDlg::OnBnClickedCheckrawcolor()
{
	BagReader.bConvertRawColor = !BagReader.bConvertRawColor;
}


void CMultiBagReaderDlg::OnBnClickedCheckrawdepth()
{
	BagReader.bConvertRawDepth = !BagReader.bConvertRawDepth;
}


void CMultiBagReaderDlg::OnBnClickedCheckrawdepthaligned()
{
	BagReader.bConvertRawDepthAligned = !BagReader.bConvertRawDepthAligned;
}


void CMultiBagReaderDlg::OnBnClickedCheckpose()
{
	BagReader.bConvertPose = !BagReader.bConvertPose;
}


void CMultiBagReaderDlg::OnEnChangeEditframeincrement()
{
	CString tString = L"";
	m_editFrameIncrement.GetWindowTextW(tString);

	if (tString.SpanIncluding(L"0123456789") != tString)
		m_editFrameIncrement.SetWindowText(L"");
	else {
		if (_wtoi(tString.GetBuffer()) > 10000)
			m_editFrameIncrement.SetWindowText(L"10000");
		else if (_wtoi(tString.GetBuffer()) < 1)
			m_editFrameIncrement.SetWindowText(L"1");
		BagReader.frameIncrement = _wtoi(tString.GetBuffer());
	}
}


void CMultiBagReaderDlg::OnBnClickedCheckpngir1()
{
	BagReader.bConvertPngIr1 = !BagReader.bConvertPngIr1;
}


void CMultiBagReaderDlg::OnBnClickedCheckpngir2()
{
	BagReader.bConvertPngIr2 = !BagReader.bConvertPngIr2;
}


void CMultiBagReaderDlg::OnBnClickedCheckrawir1()
{
	BagReader.bConvertRawIr1 = !BagReader.bConvertRawIr1;
}


void CMultiBagReaderDlg::OnBnClickedCheckrawir2()
{
	BagReader.bConvertRawIr2 = !BagReader.bConvertRawIr2;
}


void CMultiBagReaderDlg::OnBnClickedCheckpngcoloraligned()
{
	BagReader.bConvertPngColorAligned = !BagReader.bConvertPngColorAligned;
}


void CMultiBagReaderDlg::OnBnClickedCheckrawcoloraligned()
{
	BagReader.bConvertRawColorAligned = !BagReader.bConvertRawColorAligned;
}
