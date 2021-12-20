// OutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultiBagWriterROS.h"
#include "OutputDlg.h"
#include "afxdialogex.h"
#include <commctrl.h>


// COutputDlg dialog

IMPLEMENT_DYNAMIC(COutputDlg, CDialogEx)

COutputDlg::COutputDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETPATHANDNAME, pParent)
{

}

COutputDlg::~COutputDlg()
{
}

void COutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FILEPATHDLG, m_FilePath);
	DDX_Text(pDX, IDC_FILENAMEDLG, m_FileName);
}


BEGIN_MESSAGE_MAP(COutputDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BROWSE, &COutputDlg::OnBnClickedBrowse)
	ON_EN_CHANGE(IDC_FILENAMEDLG, &COutputDlg::OnEnChangeFilenamedlg)
END_MESSAGE_MAP()


// COutputDlg message handlers


void COutputDlg::OnBnClickedBrowse()
{
	// Example code
	CFolderPickerDialog FolderDlg;

	FolderDlg.m_ofn.lpstrTitle = _T("Select the path of the destination folder");
	FolderDlg.m_ofn.lpstrInitialDir = m_FilePath;
	if (FolderDlg.DoModal() == IDOK) {
		m_FilePath = FolderDlg.GetPathName();   
		UpdateData(FALSE);   
	}
	else {
		AfxMessageBox(L"Destination path not selected");
	}
}


void COutputDlg::OnEnChangeFilenamedlg()
{
	UpdateData(TRUE);		
}
