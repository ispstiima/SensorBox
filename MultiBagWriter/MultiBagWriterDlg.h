
// MultiBagWriterDlg.h : header file
//

#pragma once
#include "Controller.h"
#include "OutputDlg.h"


// CMultiBagWriterDlg dialog
class CMultiBagWriterDlg : public CDialogEx
{
// Construction
public:
	CMultiBagWriterDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MultiBagWriter_DIALOG };
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
	virtual void OnCancel();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CButton m_checkRotateImages;
	CComboBox m_comboFps;
	CString m_SnapshotPath;

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedStart();
	CController Controller;
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedStartgrab();
	afx_msg void OnBnClickedStopgrab();

	const std::string currentDateTime();
	
	afx_msg void OnBnClickedCheckrotateimages();
	afx_msg void OnCbnSelchangeCombofps();
	afx_msg void OnBnClickedChange();
	afx_msg void OnBnClickedTake();
};
