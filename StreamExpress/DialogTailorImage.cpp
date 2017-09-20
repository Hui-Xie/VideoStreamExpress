// DialogTailorImage.cpp : implementation file
//

#include "stdafx.h"
#include "StreamExpress.h"
#include "DialogTailorImage.h"
#include "afxdialogex.h"


// DialogTailorImage dialog

IMPLEMENT_DYNAMIC(DialogTailorImage, CDialogEx)

DialogTailorImage::DialogTailorImage(CWnd* pParent /*=NULL*/)
	: CDialogEx(DialogTailorImage::IDD, pParent)
{
	m_bUpdatingOffset = false;
}

DialogTailorImage::~DialogTailorImage()
{
}

BOOL DialogTailorImage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == 0x1b)
		return   TRUE;
	if (pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == 0x0d)
		return   TRUE;
	return   CDialog::PreTranslateMessage(pMsg);
}

void DialogTailorImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editLeftOffset);
	DDX_Control(pDX, IDC_EDIT2, m_editTopOffset);
	DDX_Control(pDX, IDC_EDIT3, m_editRightOffset);
	DDX_Control(pDX, IDC_EDIT4, m_editBottomOffset);

}


BEGIN_MESSAGE_MAP(DialogTailorImage, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &DialogTailorImage::OnEnChangeLeftOffset)
    ON_EN_CHANGE(IDC_EDIT2, &DialogTailorImage::OnEnChangeTopOffset)
	ON_EN_CHANGE(IDC_EDIT3, &DialogTailorImage::OnEnChangeRightOffset)
	ON_EN_CHANGE(IDC_EDIT4, &DialogTailorImage::OnEnChangeBottomOffset)
END_MESSAGE_MAP()


// DialogTailorImage message handlers

void DialogTailorImage::initilizeOffset(int const leftOffset, int const rightOffset, int const topOffset, int const bottomOffset){
	
	m_leftOffset = leftOffset;
	m_rightOffset = rightOffset;
	m_topOffset = topOffset;
	m_bottomOffset = bottomOffset;
	
	updateOffsetUI();
	
}

void DialogTailorImage::updateOffsetUI(){
	CString text;
	text.Format("%d", m_leftOffset);
	m_editLeftOffset.SetWindowTextA(text);
	text.Format("%d", m_rightOffset);
	m_editRightOffset.SetWindowTextA(text);
	text.Format("%d", m_topOffset);
	m_editTopOffset.SetWindowTextA(text);
	text.Format("%d", m_bottomOffset);
	m_editBottomOffset.SetWindowTextA(text);
}

BOOL DialogTailorImage::Create(UINT nID, CWnd * pWnd, StreamProcess * streamProcess){
	m_streamProcess = streamProcess;
	return CDialog::Create(nID, pWnd);
}

bool DialogTailorImage::validateFourOffset(){
	CString text;
	m_editLeftOffset.GetWindowText(text);
	int leftOffset = atoi(text);
	m_editTopOffset.GetWindowText(text);
	int topOffset = atoi(text);
	m_editRightOffset.GetWindowText(text);
	int rightOffset = atoi(text);
	m_editBottomOffset.GetWindowText(text);
	int bottomOffset = atoi(text);

	if (leftOffset + rightOffset > 90 || topOffset + bottomOffset > 90){
		AfxMessageBox(_T("Please assure the reasonability of 4 offsets."));
		updateOffsetUI();
		return false;
	}
	else return true;
}


void DialogTailorImage::OnEnChangeLeftOffset()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (m_bUpdatingOffset) return;
	m_bUpdatingOffset = true;

	if (validateFourOffset()) {
		CString text;
		m_editLeftOffset.GetWindowText(text);
		m_leftOffset = atoi(text);
		m_streamProcess->m_leftOffset = m_leftOffset;
	}

	m_bUpdatingOffset = false;
}



void DialogTailorImage::OnEnChangeTopOffset()
{
	if (m_bUpdatingOffset) return;
	m_bUpdatingOffset = true;

	if (validateFourOffset()) {
		CString text;
		m_editTopOffset.GetWindowText(text);
		m_topOffset = atoi(text);
		m_streamProcess->m_topOffset = m_topOffset;
	}

	m_bUpdatingOffset = false;
}


void DialogTailorImage::OnEnChangeRightOffset()
{
	if (m_bUpdatingOffset) return;
	m_bUpdatingOffset = true;
	
	if (validateFourOffset()) {
		CString text;
		m_editRightOffset.GetWindowText(text);
		m_rightOffset = atoi(text);
		m_streamProcess->m_rightOffset = m_rightOffset;
	}

	m_bUpdatingOffset = false;

}

void DialogTailorImage::OnEnChangeBottomOffset()
{
	if (m_bUpdatingOffset) return;
	m_bUpdatingOffset = true;
	
	if (validateFourOffset()) {
		CString text;
		m_editBottomOffset.GetWindowText(text);
		m_bottomOffset = atoi(text);
		m_streamProcess->m_bottomOffset = m_bottomOffset;
	}

	m_bUpdatingOffset = false;

}
