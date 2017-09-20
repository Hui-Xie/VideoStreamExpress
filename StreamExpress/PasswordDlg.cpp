// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StreamExpress.h"
#include "PasswordDlg.h"
#include "afxdialogex.h"


// PasswordDlg dialog

#define PASSWORD "Macalester92"

IMPLEMENT_DYNAMIC(PasswordDlg, CDialogEx)

PasswordDlg::PasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(PasswordDlg::IDD, pParent)
{
	
}

PasswordDlg::~PasswordDlg()
{
}

void PasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editPassword);
}


BEGIN_MESSAGE_MAP(PasswordDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &PasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// PasswordDlg message handlers


void PasswordDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString text;
	m_editPassword.GetWindowText(text);
	if (0 != text.Compare(PASSWORD)){
		AfxMessageBox("Sorry. Invalid Password.");
		return;
	}
	else {
		CDialogEx::OnOK();
	}
}


