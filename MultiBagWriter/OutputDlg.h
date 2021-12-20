#pragma once


// COutputDlg dialog

class COutputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COutputDlg)

public:
	COutputDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COutputDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETPATHANDNAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_FilePath, m_FileName;
	CEdit m_EditFileName;
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnEnChangeFilenamedlg();
};
