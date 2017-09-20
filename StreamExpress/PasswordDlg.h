#pragma once


// PasswordDlg dialog



class PasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PasswordDlg)

public:
	PasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~PasswordDlg();
	CEdit m_editPassword;


// Dialog Data
	enum { IDD = IDD_PASSWORDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
//	afx_msg void OnEnUpdateEdit1();
};
