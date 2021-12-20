
// MultiBagWriterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiBagWriter.h"
#include "MultiBagWriterDlg.h"
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


// CMultiBagWriterDlg dialog



CMultiBagWriterDlg::CMultiBagWriterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MultiBagWriter_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiBagWriterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBOFPS, m_comboFps);
	DDX_Text(pDX, IDC_TX, Controller.Image.m_Pose.T[0]);
	DDX_Text(pDX, IDC_TY, Controller.Image.m_Pose.T[1]);
	DDX_Text(pDX, IDC_TZ, Controller.Image.m_Pose.T[2]);
	DDX_Text(pDX, IDC_RW, Controller.Image.m_Pose.R[0]);
	DDX_Text(pDX, IDC_RX, Controller.Image.m_Pose.R[1]);
	DDX_Text(pDX, IDC_RY, Controller.Image.m_Pose.R[2]);
	DDX_Text(pDX, IDC_RZ, Controller.Image.m_Pose.R[3]);
	DDX_Text(pDX, IDC_FILENAME, Controller.m_FilePathAndName);
	DDX_Text(pDX, IDC_SNAPNAME, m_SnapshotPath);
	DDX_Control(pDX, IDC_CHECKROTATEIMAGES, m_checkRotateImages);

}

BEGIN_MESSAGE_MAP(CMultiBagWriterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START, &CMultiBagWriterDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CMultiBagWriterDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_STARTGRAB, &CMultiBagWriterDlg::OnBnClickedStartgrab)
	ON_BN_CLICKED(IDC_STOPGRAB, &CMultiBagWriterDlg::OnBnClickedStopgrab)
	ON_BN_CLICKED(IDC_CHECKROTATEIMAGES, &CMultiBagWriterDlg::OnBnClickedCheckrotateimages)
	ON_CBN_SELCHANGE(IDC_COMBOFPS, &CMultiBagWriterDlg::OnCbnSelchangeCombofps)
	ON_BN_CLICKED(IDC_CHANGE, &CMultiBagWriterDlg::OnBnClickedChange)
	ON_BN_CLICKED(IDC_TAKE, &CMultiBagWriterDlg::OnBnClickedTake)
END_MESSAGE_MAP()


// CMultiBagWriterDlg message handlers

BOOL CMultiBagWriterDlg::OnInitDialog()
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

	// TODO: Add extra initialization here
	SetTimer(1, 30, NULL);
	Controller.SetContainer(this);

	m_checkRotateImages.SetCheck(BST_UNCHECKED);
	Controller.Image.isRotated = false;

	m_comboFps.AddString(L"30 fps");
	m_comboFps.AddString(L"15 fps");
	m_comboFps.AddString(L"6 fps");
	m_comboFps.AddString(L"5 fps");
	m_comboFps.SetCurSel(0);
	Controller.Fps = 30;

	TCHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	m_SnapshotPath = my_documents;
	Controller.Image.destinationFolder = CT2A(m_SnapshotPath);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CMultiBagWriterDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	CDialogEx::OnTimer(nIDEvent);
}


void CMultiBagWriterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMultiBagWriterDlg::OnPaint()
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
HCURSOR CMultiBagWriterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMultiBagWriterDlg::OnCancel()
{
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBOFPS)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_STARTGRAB)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOPGRAB)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TAKE)->EnableWindow(FALSE);

	if (Controller.bAcquisitionLoop) {
		Controller.bAcquisitionLoop = false;
		if (WaitForSingleObject(Controller, INFINITE) == WAIT_OBJECT_0)
		{
		}
	}
	CDialog::OnCancel();
}


void CMultiBagWriterDlg::OnBnClickedStart()
{
	Controller.bAcquisitionLoop = true;
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBOFPS)->EnableWindow(FALSE);
	Controller.CreateThread();
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_STARTGRAB)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
	GetDlgItem(IDC_TAKE)->EnableWindow(TRUE);
}


void CMultiBagWriterDlg::OnBnClickedStop()
{
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBOFPS)->EnableWindow(TRUE);
	GetDlgItem(IDC_STARTGRAB)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TAKE)->EnableWindow(FALSE);
	
	Controller.bTakeSnapshot = false;
	Controller.bAcquisitionLoop = false;
	if (WaitForSingleObject(Controller, INFINITE) == WAIT_OBJECT_0)
	{
	}
	cv::waitKey(2000);
}


void CMultiBagWriterDlg::OnBnClickedStartgrab()
{
	GetDlgItem(IDC_STARTGRAB)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOPGRAB)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECKROTATEIMAGES)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_TAKE)->EnableWindow(FALSE);
	
	COutputDlg OutputDlg;

	

	std::string strName = currentDateTime();
	char tName[80];
	strcpy_s(tName, 80, strName.c_str());
	OutputDlg.m_FileName = tName;
	
	TCHAR my_documents[MAX_PATH];
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	OutputDlg.m_FilePath = my_documents;

	if (OutputDlg.DoModal() == IDOK) {
		UpdateData(FALSE);
		int extPos = OutputDlg.m_FileName.Find(L".", OutputDlg.m_FileName.GetLength() - 4);
		if (extPos >= 0)
			Controller.m_FilePathAndName = OutputDlg.m_FilePath + "\\" + OutputDlg.m_FileName.Left(extPos);
		else
			Controller.m_FilePathAndName = OutputDlg.m_FilePath + "\\" + OutputDlg.m_FileName;
		Controller.bStartSaving = true;
	}
	else
	{
		AfxMessageBox(L"Cannot Open File");
		GetDlgItem(IDC_STARTGRAB)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOPGRAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKROTATEIMAGES)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_TAKE)->EnableWindow(TRUE);
	}
}


void CMultiBagWriterDlg::OnBnClickedStopgrab()
{
	Controller.bStopSaving = true;
	GetDlgItem(IDC_STARTGRAB)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOPGRAB)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECKROTATEIMAGES)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHANGE)->EnableWindow(TRUE);
	GetDlgItem(IDC_TAKE)->EnableWindow(TRUE);
	Controller.m_FilePathAndName.Format(L"");
}

const std::string CMultiBagWriterDlg::currentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	localtime_s(&tstruct, &now);
	char       buf[80];
	strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
	return buf;
}



void CMultiBagWriterDlg::OnBnClickedCheckrotateimages()
{
	Controller.bRotateDisplay = true;
}


void CMultiBagWriterDlg::OnCbnSelchangeCombofps()
{
	int configSel = m_comboFps.GetCurSel();
	if (configSel != LB_ERR) {
		switch (m_comboFps.GetCurSel()) {
		case 0:
			Controller.Fps = 30;
			break;
		case 1:
			Controller.Fps = 15;
			break;
		case 2:
			Controller.Fps = 6;
			break;
		case 3:
			Controller.Fps = 5;
			break;
		}
	}
	else
		Controller.Fps = 30;
}


void CMultiBagWriterDlg::OnBnClickedChange()
{
	// Example code
	CFolderPickerDialog FolderDlg;

	FolderDlg.m_ofn.lpstrTitle = _T("Select the path of the destination folder");
	FolderDlg.m_ofn.lpstrInitialDir = m_SnapshotPath;
	if (FolderDlg.DoModal() == IDOK) {
		m_SnapshotPath = FolderDlg.GetPathName();
		Controller.Image.destinationFolder = CT2A(m_SnapshotPath);
		UpdateData(FALSE);
	}
	else {
		AfxMessageBox(L"Destination path not selected");
	}
}


void CMultiBagWriterDlg::OnBnClickedTake()
{
	Controller.bTakeSnapshot = true;
}
