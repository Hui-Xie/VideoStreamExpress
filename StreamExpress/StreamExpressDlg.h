
// StreamExpressDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include "StreamProcess.h"
#include "FrameProcess.h"
#include "FaceContainer.h"
#include "DialogTailorImage.h"

#define MIN_FACE_WIDTH 35

enum SYS_STATUS {
	WAIT,
	CAPTURE,
	STOP
};


// CStreamExpressDlg dialog
class CStreamExpressDlg : public CDialogEx
{
// Construction
public:
	CStreamExpressDlg(CWnd* pParent = NULL);	// standard constructor
	~CStreamExpressDlg();


// Dialog Data
	enum { IDD = IDD_STREAMEXPRESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support



// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP();
	
public:
	CButton m_radioIP;
	CButton m_radioWeb;
	CButton m_buttonSetFilePath;
	CButton m_buttonCapture;
	CButton m_buttonStop;
	CComboBox m_comboIntervalTime;
	CComboBox m_comboPath;
	CComboBox m_comboShowStatus;
	CEdit m_editUserName;
	CEdit m_editPassword;
	CEdit m_editPort;
	CEdit m_editFaceWidth;
	CDateTimeCtrl m_startTime;
	CDateTimeCtrl m_endTime;
	CButton m_checkSunday;
	CButton m_checkMonday;
	CButton m_checkTuesday;
	CButton m_checkWednesday;
	CButton m_checkThursday;
	CButton m_checkFriday;
	CButton m_checkSaturday;
	CButton m_checkShowStream;
	CButton m_checkShowFaceOutline;
	CButton m_checkShowSmileFrown;
	CButton m_buttonTailorImage;
	CButton m_checkCalculateEmotion;
	CIPAddressCtrl m_IPAddress;
	int m_faceWidth;
	int m_faceHeight;
	std::string m_captureWnd;
	bool m_isShowStream;
	bool m_isShowFaceOutline;
	bool m_isShowSmileFrown;
	bool m_isCalculateEmotion;
	UINT   m_saveIntervalTime;//unit millisecond
	UINT_PTR m_saveFileEventID;
	UINT_PTR m_checkSystemTimeEventID;
	SYSTEMTIME m_currentTime;
				
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedWebCamera();
	afx_msg void OnBnClickedIPCamera();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	SYS_STATUS m_sysStatus;

	FaceContainer m_faceContainer;
	StreamProcess m_streamProcess;
	FrameProcess m_frameProcess;  
	DialogTailorImage* m_tailorImageDlg;

	afx_msg void OnBnClickedCapture();
	void analyzeStream();
	void captureStreamAnalyze();
	
	void drawFaceBox(int x, int y, int width, int height);
	void autoSaveStreamOutput();
	int judgeStartEndTime();
	void processInternalMessage();

	afx_msg void OnEnChangeFaceWidth();
	afx_msg void OnClose();
	afx_msg void OnBnClickedShowStream();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeIntervalTime();
	

	afx_msg void OnBnClickedShowFaceOutline();
	afx_msg void OnBnClickedCalculateEmotion();
	afx_msg void OnBnClickedTailorImage();
	afx_msg void OnBnClickedShowSmileFrown();
};
