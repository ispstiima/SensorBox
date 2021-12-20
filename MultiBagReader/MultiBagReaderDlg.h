
// MultiBagReaderDlg.h : header file
//

#pragma once
#include "RSBagReader.h"

// CMultiBagReaderDlg dialog
class CMultiBagReaderDlg : public CDialogEx
{
// Construction
public:
	CMultiBagReaderDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MULTIBAGREADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	CRSBagReader BagReader;
	CString m_FilePathAndName;
	
	CButton m_checkPose;
	CButton m_checkPngColor, m_checkPngColorAligned, m_checkPngDepth, m_checkPngDepthAligned, m_checkPngIr1, m_checkPngIr2;
	CButton m_checkRawColor, m_checkRawColorAligned, m_checkRawDepth, m_checkRawDepthAligned, m_checkRawIr1, m_checkRawIr2;
	CButton m_checkCsvDepthMeter, m_checkCsvDepthMeterAligned;
	CButton m_checkPly;
	CEdit m_editLog, m_editFilePathAndName, m_editFrameIncrement;
	CSpinButtonCtrl m_spinFrameIncrement;

	afx_msg void OnBnClickedSetreference();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	void InitializeCheckControls();
	afx_msg void OnBnClickedCheckpc();
	afx_msg void OnBnClickedCheckcsvdepth();
	afx_msg void OnBnClickedCheckcsvdepthaligned();
	afx_msg void OnBnClickedCheckpngcolor();
	afx_msg void OnBnClickedCheckpngdepth();
	afx_msg void OnBnClickedCheckpngdepthaligned();
	afx_msg void OnBnClickedCheckrawcolor();
	afx_msg void OnBnClickedCheckrawdepth();
	afx_msg void OnBnClickedCheckrawdepthaligned();
	afx_msg void OnBnClickedCheckpose();
	afx_msg void OnEnChangeEditframeincrement();
	afx_msg void OnBnClickedCheckpngir1();
	afx_msg void OnBnClickedCheckpngir2();
	afx_msg void OnBnClickedCheckrawir1();
	afx_msg void OnBnClickedCheckrawir2();
	afx_msg void OnBnClickedCheckpngcoloraligned();
	afx_msg void OnBnClickedCheckrawcoloraligned();
};
