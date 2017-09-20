#pragma once


// DialogTailorImage dialog

class DialogTailorImage : public CDialogEx
{
	DECLARE_DYNAMIC(DialogTailorImage)
	CEdit m_editLeftOffset;
	CEdit m_editTopOffset;
	CEdit m_editRightOffset;
	CEdit m_editBottomOffset;
	
	int m_leftOffset;
	int m_rightOffset;
	int m_topOffset;
	int m_bottomOffset;
	
	StreamProcess * m_streamProcess;
	bool m_bUpdatingOffset; 



public:
	DialogTailorImage(CWnd* pParent = NULL);   // standard constructor
	virtual ~DialogTailorImage();
	void initilizeOffset(int const leftOffset, int const rightOffset, int const topOffset, int const bottomOffset);
	BOOL Create(UINT nID, CWnd * pWnd, StreamProcess * streamProcess);
	void updateOffsetUI();
	BOOL PreTranslateMessage(MSG* pMsg);
	

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool validateFourOffset();
	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeLeftOffset();

	afx_msg void OnEnChangeTopOffset();
	afx_msg void OnEnChangeRightOffset();
	afx_msg void OnEnChangeBottomOffset();
};
