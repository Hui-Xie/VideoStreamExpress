
// StreamExpressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StreamExpress.h"
#include "StreamExpressDlg.h"
#include "afxdialogex.h"
#include <string>
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





CStreamExpressDlg::CStreamExpressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStreamExpressDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sysStatus = WAIT;
	m_faceWidth = MIN_FACE_WIDTH;
	m_faceHeight = MIN_FACE_WIDTH;
	m_captureWnd = "Capture@StreamExpress";
	m_saveIntervalTime = 1000 * 60 * 30; //default 30min;
	m_saveFileEventID = 973485;
	m_checkSystemTimeEventID = 902341;
	m_isShowStream = false;
	m_isCalculateEmotion = false;
	m_isShowFaceOutline = false;
	m_isShowSmileFrown = false;
	m_tailorImageDlg = NULL;
		
}

CStreamExpressDlg::~CStreamExpressDlg(){
	
	cv::destroyWindow(m_captureWnd);
	if (NULL != m_tailorImageDlg) delete m_tailorImageDlg;
}

void CStreamExpressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_buttonSetFilePath);
	DDX_Control(pDX, IDC_BUTTON2, m_buttonCapture);
	DDX_Control(pDX, IDC_BUTTON3, m_buttonStop);
	DDX_Control(pDX, IDC_COMBO1, m_comboIntervalTime);
	DDX_Control(pDX, IDC_COMBO2, m_comboPath);
	DDX_Control(pDX, IDC_COMBO3, m_comboShowStatus);
	DDX_Control(pDX, IDC_EDIT1, m_editUserName);
	DDX_Control(pDX, IDC_EDIT2, m_editPassword);
	DDX_Control(pDX, IDC_EDIT4, m_editPort);
	DDX_Control(pDX, IDC_EDIT3, m_editFaceWidth);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_startTime);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_endTime);
	DDX_Control(pDX, IDC_CHECK1, m_checkSunday);
	DDX_Control(pDX, IDC_CHECK2, m_checkMonday);
	DDX_Control(pDX, IDC_CHECK3, m_checkTuesday);
	DDX_Control(pDX, IDC_CHECK4, m_checkWednesday);
	DDX_Control(pDX, IDC_CHECK5, m_checkThursday);
	DDX_Control(pDX, IDC_CHECK6, m_checkFriday);
	DDX_Control(pDX, IDC_CHECK7, m_checkSaturday);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddress);
	DDX_Control(pDX, IDC_RADIO1, m_radioIP);
	DDX_Control(pDX, IDC_RADIO2, m_radioWeb);
	DDX_Control(pDX, IDC_CHECK8, m_checkShowStream);
	DDX_Control(pDX, IDC_CHECK9, m_checkShowFaceOutline);
	DDX_Control(pDX, IDC_CHECK11, m_checkShowSmileFrown);
	DDX_Control(pDX, IDC_CHECK10, m_checkCalculateEmotion);
	DDX_Control(pDX, IDC_BUTTON4, m_buttonTailorImage);

}

BEGIN_MESSAGE_MAP(CStreamExpressDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON3, &CStreamExpressDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_BUTTON1, &CStreamExpressDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_RADIO2, &CStreamExpressDlg::OnBnClickedWebCamera)
	ON_BN_CLICKED(IDC_RADIO1, &CStreamExpressDlg::OnBnClickedIPCamera)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON2, &CStreamExpressDlg::OnBnClickedCapture)
	ON_EN_CHANGE(IDC_EDIT3, &CStreamExpressDlg::OnEnChangeFaceWidth)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK8, &CStreamExpressDlg::OnBnClickedShowStream)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CStreamExpressDlg::OnCbnSelchangeIntervalTime)
	ON_BN_CLICKED(IDC_CHECK9, &CStreamExpressDlg::OnBnClickedShowFaceOutline)
	ON_BN_CLICKED(IDC_CHECK10, &CStreamExpressDlg::OnBnClickedCalculateEmotion)
	ON_BN_CLICKED(IDC_BUTTON4, &CStreamExpressDlg::OnBnClickedTailorImage)
	ON_BN_CLICKED(IDC_CHECK11, &CStreamExpressDlg::OnBnClickedShowSmileFrown)
END_MESSAGE_MAP()


// CStreamExpressDlg message handlers


//Cancel Return and ESC key events.
BOOL CStreamExpressDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == 0x1b)
		return   TRUE;
	if (pMsg->message == WM_KEYDOWN   &&   pMsg->wParam == 0x0d)
		return   TRUE;
	return   CDialog::PreTranslateMessage(pMsg);
}


BOOL CStreamExpressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_checkShowStream.SetCheck(BST_UNCHECKED);
	m_checkCalculateEmotion.SetCheck(BST_UNCHECKED);
	m_checkShowFaceOutline.SetCheck(BST_UNCHECKED);
	m_checkShowSmileFrown.SetCheck(BST_UNCHECKED);
	m_checkShowFaceOutline.EnableWindow(false);
	m_checkShowSmileFrown.EnableWindow(false);

	CString text;
	text.Format("%d", MIN_FACE_WIDTH);
	m_editFaceWidth.SetWindowTextA(text);//min value.
	if (m_frameProcess.isInitializeSuccess()) m_frameProcess.setMinFaceDetectionWidth(MIN_FACE_WIDTH);

	m_comboIntervalTime.SetCurSel(2);//default 30min
	OnCbnSelchangeIntervalTime();
	
	//display current path
	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);
	m_comboPath.AddString(currentDir);
	m_comboPath.SetCurSel(0);
	
	CTime time;
	CTimeSpan span(0, 8, 10, 10);//0d,8h,10m,10s
	m_startTime.GetTime(time);
	time += span;
	m_endTime.SetTime(&time);
	
	m_buttonCapture.EnableWindow(false);
	m_buttonStop.EnableWindow(false);
	
	m_comboShowStatus.SetWindowTextA("Please First Choose Camera.");
	
	PostMessage(SS_NOTIFY);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStreamExpressDlg::OnPaint()
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


void CStreamExpressDlg::OnBnClickedBrowse()
{
	BROWSEINFO   bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR   szDisplayName[MAX_PATH];
	szDisplayName[0] = '\0';
	
	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _T("Please select a folder for output file:");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
	TCHAR   szPathName[MAX_PATH];
	if (NULL != pidl)
	{
		SHGetPathFromIDList(pidl, szPathName);
		m_comboPath.DeleteString(0);
		m_comboPath.AddString(szPathName);
		m_comboPath.SetCurSel(0);

	}

}



void CStreamExpressDlg::OnBnClickedWebCamera()
{
	// TODO: Add your control notification handler code here
	m_radioIP.SetCheck(BST_UNCHECKED);
	m_buttonCapture.EnableWindow(false);
	m_streamProcess.setWebCamera();
	m_comboShowStatus.SetWindowTextA("Set Web Camera. Please Press Capture to Launch.");
	m_buttonCapture.EnableWindow(true);
}


void CStreamExpressDlg::OnBnClickedIPCamera()
{
	// TODO: Add your control notification handler code here
	m_radioWeb.SetCheck(BST_UNCHECKED);
	m_buttonCapture.EnableWindow(false);
	UpdateWindow();
			
	//Format: "rtsp://face@iowa:facet2015!@128.255.246.70:88/videoMain"
	std::string URLAddress;
	URLAddress = "rtsp://";
	BYTE nField0, nField1, nField2, nField3;

	CString portText;
	m_editPort.GetWindowText(portText);
	int port = atoi(portText);

	if (4 == m_IPAddress.GetAddress(nField0, nField1, nField2, nField3) && port > 0) {
		//get IPaddress
		char IPAddress[50];
		int n = 0;
		n = sprintf_s(IPAddress, 50, "%d.%d.%d.%d:%d/videoMain", nField0, nField1, nField2, nField3,port);
		*(IPAddress + n) = '\0';

		//if exist username
		CString text;
		m_editUserName.GetWindowTextA(text);
		if (! text.IsEmpty()) URLAddress.append(_T(text));
		text.Empty();
		
		//if exist password
		m_editPassword.GetWindowTextA(text);
		if (!text.IsEmpty()) URLAddress.append(":" + _T(text) + "@");

		URLAddress.append(IPAddress);
		m_comboShowStatus.SetWindowTextA("Set Designated IP Camera. Please Press Capture to Launch.");
	}
	else {//use default
		//URLAddress = "rtsp://face@iowa:facet2015!@128.255.246.70:88/videoMain";
		//m_comboShowStatus.SetWindowTextA("Set Default IP Camera. Please Press Capture to Launch.");
		URLAddress.clear(); 
		m_comboShowStatus.SetWindowTextA("Set Last visited IP Camera. Please Press Capture to Launch.");
	}

	m_streamProcess.setIPCameraURL(URLAddress);
	m_buttonCapture.EnableWindow(true);

}



int CStreamExpressDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1) 	return -1;
	// TODO:  Add your specialized creation code here
	
	m_checkSystemTimeEventID = SetTimer(m_checkSystemTimeEventID, 1000*60,NULL);//check every 60s.
	return 0;
}


void CStreamExpressDlg::autoSaveStreamOutput(){
	
	if (m_faceContainer.isEmpty()) return;

	m_comboShowStatus.SetWindowTextA("Camera is closed. Now Saving output ...");
	UpdateWindow();

	CString path;
	m_comboPath.GetWindowText(path);
	m_faceContainer.openStreamOutputFile(path);
	m_faceContainer.printCSVHead();
	m_faceContainer.printAllResult();
	m_faceContainer.closeOutputFile();
	m_faceContainer.clearAllVector();

	m_comboShowStatus.SetWindowTextA("Finised output in designed directory.");
	UpdateWindow();
}


void CStreamExpressDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	
	//set window in Top Left postion.
	RECT wndRect;
	GetWindowRect(&wndRect);
	SetWindowPos(&wndTopMost, 0, 0, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, SWP_SHOWWINDOW);

}


void CStreamExpressDlg::captureStreamAnalyze(){
	
	if (m_isShowStream) cv::namedWindow(m_captureWnd);
	int nFaces = 0;
	while (CAPTURE == m_sysStatus){
		cv::Mat &frame = m_streamProcess.getMatFrame();

		if (m_isShowStream) m_streamProcess.drawTailorFrame();

		if (m_isCalculateEmotion && m_frameProcess.isInitializeSuccess()) nFaces = m_streamProcess.analyzeStream(m_frameProcess, m_faceContainer);
		else nFaces = 0;
		
		if (nFaces > 0 && m_isShowStream && m_isCalculateEmotion){
			if (m_isShowFaceOutline){
				m_streamProcess.drawFaceFigure(m_frameProcess.getFaceCharacter(m_streamProcess.m_offsetX, m_streamProcess.m_offsetY));
			}
			else if (m_isShowSmileFrown){
				cv::Scalar color;
				float intensity;
				m_frameProcess.getOverallSentiment(color, intensity);
				m_streamProcess.drawSmileFrownFace(m_frameProcess.getFaceCharacter(m_streamProcess.m_offsetX, m_streamProcess.m_offsetY),
				                                	color, intensity);
			}

		}
		
		if (m_isShowStream) cv::imshow(m_captureWnd, frame);//show
				
		//cv::waitKey(15); //unnecessay as processInternalMessage() below;
		processInternalMessage();
	}
	cv::destroyWindow(m_captureWnd);
	
}


void CStreamExpressDlg::OnBnClickedCapture()
{
	m_buttonCapture.EnableWindow(false);
	m_comboShowStatus.SetWindowTextA("Opening Camera, Please Wait ...");
	UpdateWindow();

	m_sysStatus = CAPTURE;
	m_streamProcess.openCamera();

	if (m_streamProcess.isOpened()){
		m_comboShowStatus.SetWindowTextA("Now Caputuring Stream and Analyzing ...");
		m_buttonStop.EnableWindow(true);
		captureStreamAnalyze();
	}
	else{
		m_comboShowStatus.SetWindowTextA("Opening Camera Failed. Please Check ...");
		m_buttonCapture.EnableWindow(true);
		UpdateWindow();
		m_sysStatus = STOP;
	}
}

void CStreamExpressDlg::OnBnClickedStop()
{
	m_buttonStop.EnableWindow(false);
	m_sysStatus = STOP;
	m_streamProcess.closeCamera();
	cv::destroyWindow(m_captureWnd);
	autoSaveStreamOutput();
	m_buttonCapture.EnableWindow(true);
	m_comboShowStatus.SetWindowTextA("Camera was closed. Please Press Capture to Launch.");
}

void CStreamExpressDlg::drawFaceBox(int x, int y, int width, int height){
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + m_faceWidth;
	rect.bottom = y + m_faceHeight;
	//m_memoryDC.DrawEdge(&rect, EDGE_BUMP, BF_RECT);
	
}

void CStreamExpressDlg::OnEnChangeFaceWidth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString text;
	m_editFaceWidth.GetWindowText(text);
	int width = atoi(text);
	if (width < MIN_FACE_WIDTH) {
		width = MIN_FACE_WIDTH;
		text.Format("%d", MIN_FACE_WIDTH);
		m_editFaceWidth.SetWindowTextA(text);
	}
	
	m_faceWidth = width;
	m_faceHeight = width;
	if (m_frameProcess.isInitializeSuccess()) m_frameProcess.setMinFaceDetectionWidth(m_faceWidth);

}

void CStreamExpressDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	OnBnClickedStop();
	KillTimer(m_saveFileEventID);
	KillTimer(m_checkSystemTimeEventID);
	CDialogEx::OnClose();
}




//judgeStartEndTime: Judge the start or end time of event.
//return: 0:  do nothing;
//        1:  start time;
//        2:  end time;
int CStreamExpressDlg::judgeStartEndTime(){
	GetLocalTime(&m_currentTime);
	
	//Compare the day of a week
	int const dayOfWeek = m_currentTime.wDayOfWeek;
	bool isTheDayOfWeek = false;
	switch (dayOfWeek){
	case 0:
		if (BST_CHECKED == m_checkSunday.GetCheck()) isTheDayOfWeek = true;
		break;
	case 1:
		if (BST_CHECKED == m_checkMonday.GetCheck()) isTheDayOfWeek = true;
		break;
	case 2:
		if (BST_CHECKED == m_checkTuesday.GetCheck()) isTheDayOfWeek = true;
		break;
	case 3:
		if (BST_CHECKED == m_checkWednesday.GetCheck()) isTheDayOfWeek = true;
		break;
	case 4:
		if (BST_CHECKED == m_checkThursday.GetCheck()) isTheDayOfWeek = true;
		break;
	case 5:
		if (BST_CHECKED == m_checkFriday.GetCheck()) isTheDayOfWeek = true;
		break;
	case 6:
		if (BST_CHECKED == m_checkSaturday.GetCheck()) isTheDayOfWeek = true;
		break;
	default:
		return 0;
	}

	if (!isTheDayOfWeek) return 0; //unchecked any day of a week;

	//Compare the H,M;
	SYSTEMTIME startTime, endTime;
	m_startTime.GetTime(&startTime);
	m_endTime.GetTime(&endTime);
	//check every 60s. we only need judge minute.
	if (m_currentTime.wHour == startTime.wHour
		&& m_currentTime.wMinute == startTime.wMinute)
		return 1;
	else if (m_currentTime.wHour == endTime.wHour
		&& m_currentTime.wMinute == endTime.wMinute)
		return 2;
	else return 0;
}


void CStreamExpressDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_saveFileEventID) autoSaveStreamOutput();
	else if (nIDEvent == m_checkSystemTimeEventID){
		int n = judgeStartEndTime();
		switch (n){
		case 0:
			break;
		case 1: //Capture 
			PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON2, BN_CLICKED), (DWORD)m_hWnd);
			break;
		case 2: //Stop
			PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), (DWORD)m_hWnd);
			break;
		default:
			break;
		}
		
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CStreamExpressDlg::OnCbnSelchangeIntervalTime()
{
	// TODO: Add your control notification handler code here
	char text[5];
	int n = m_comboIntervalTime.GetLBText(m_comboIntervalTime.GetCurSel(), text);
	text[n] = '\0';
	int savingTimeMin = atoi(text);
	m_saveIntervalTime = 1000 * 60 * savingTimeMin;  //convert to milliseconds
	
	KillTimer(m_saveFileEventID);
	m_saveFileEventID = SetTimer(m_saveFileEventID, m_saveIntervalTime, NULL);

}

void CStreamExpressDlg::processInternalMessage(){
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
	  	DispatchMessage(&msg);
	}

}

void CStreamExpressDlg::OnBnClickedShowStream()
{
	int checkStatus = m_checkShowStream.GetCheck();
	if (BST_CHECKED == checkStatus) {
		PasswordDlg passwordDlg;
		INT_PTR nRet = -1;
		nRet = passwordDlg.DoModal();
		if (IDOK == nRet){
			if (m_isCalculateEmotion) {
				m_checkShowFaceOutline.EnableWindow(true);
				m_checkShowSmileFrown.EnableWindow(true);
			}
			m_isShowStream = true;
			cv::namedWindow(m_captureWnd);
		}
		else {
			m_checkShowStream.SetCheck(BST_UNCHECKED);
		}
	}
	else if (BST_UNCHECKED == checkStatus) {
		m_checkShowFaceOutline.EnableWindow(false);
		m_checkShowSmileFrown.EnableWindow(false);
		m_isShowStream = false;
		cv::destroyWindow(m_captureWnd);
	}
	else return;
}

void CStreamExpressDlg::OnBnClickedCalculateEmotion()
{
	int checkStatus = m_checkCalculateEmotion.GetCheck();
	if (BST_CHECKED == checkStatus) {
		m_isCalculateEmotion = true;
		if (m_isShowStream) {
			m_checkShowFaceOutline.EnableWindow(true);
			m_checkShowSmileFrown.EnableWindow(true);
		}
	}
	else if (BST_UNCHECKED == checkStatus) {
		m_isCalculateEmotion = false;
		m_isShowFaceOutline = false;
		m_isShowSmileFrown = false;
		m_checkShowFaceOutline.SetCheck(BST_UNCHECKED);
		m_checkShowSmileFrown.SetCheck(BST_UNCHECKED);
		m_checkShowFaceOutline.EnableWindow(false);
		m_checkShowSmileFrown.EnableWindow(false);
	}
	else return;

}


void CStreamExpressDlg::OnBnClickedShowFaceOutline()
{
	int checkStatus = m_checkShowFaceOutline.GetCheck();
	if (BST_CHECKED == checkStatus) {
		m_isShowFaceOutline = true;
		m_isShowSmileFrown = false;
		m_checkShowSmileFrown.EnableWindow(false);
	}
	else if (BST_UNCHECKED == checkStatus) {
		m_isShowFaceOutline = false;
		m_checkShowSmileFrown.EnableWindow(true);
	}
	else return;

}

void CStreamExpressDlg::OnBnClickedShowSmileFrown()
{
	int checkStatus = m_checkShowSmileFrown.GetCheck();
	if (BST_CHECKED == checkStatus) {
		m_isShowFaceOutline = false;
		m_isShowSmileFrown = true;
		m_checkShowFaceOutline.EnableWindow(false);
	}
	else if (BST_UNCHECKED == checkStatus) {
		m_isShowSmileFrown = false;
		m_checkShowFaceOutline.EnableWindow(true);
	}
	else return;

}



void CStreamExpressDlg::OnBnClickedTailorImage()
{
	// TODO: Add your control notification handler code here
	
	if (NULL == m_tailorImageDlg) {
		m_tailorImageDlg = new DialogTailorImage;
		m_tailorImageDlg->Create(IDD_DIALOG1, this, &m_streamProcess);
		m_tailorImageDlg->initilizeOffset(m_streamProcess.m_leftOffset, m_streamProcess.m_rightOffset,
		                                  m_streamProcess.m_topOffset, m_streamProcess.m_bottomOffset);
	}
	m_tailorImageDlg->ShowWindow(SW_SHOW);

}


