#pragma once

#include "opencv2\opencv.hpp"
#include "afxwin.h"
#include "FaceContainer.h"
#include "FrameProcess.h"



class StreamProcess
{
public:
	cv::VideoCapture m_videoCapture;
	cv::Mat m_grayMatFrame;
	cv::Mat m_matFrame;
	bool m_isIPCamera;
	//URL format:"rtsp://face@iowa:facet2015!@128.255.246.70:88/videoMain";
	std::string m_URLAddress;
	SYSTEMTIME m_frameTime;
	long m_frameNO;
	FrameProperty m_frameProperty;
	
	//offset(%) for tailoring image
	int m_leftOffset;
	int m_rightOffset;
	int m_topOffset;
	int m_bottomOffset;

	//absolute pixels value; 
	int m_offsetX;
	int m_offsetY;
	cv::Rect m_tailorRect;

	

public:
	StreamProcess();
	~StreamProcess();
	cv::Mat getGrayMatFrame();
	cv::Mat getMatFrame();

	void closeCamera();
	void openCamera();

	bool isOpened();

	void setIPCameraURL(std::string const URLAddress);
	void setWebCamera();
	
	void openWebCamera();
	void openIPCamera(); 
	
	int getFrameWidth();
	int getFrameHeight();
	cv::Rect GetTailoredSize();
	SYSTEMTIME getFrameTime();
	
	int analyzeStream(FrameProcess& frameProcess, FaceContainer& faceContainer);
	void drawFaceFigure(FaceCharacter const& faceCharacter);
	void drawSmileFrownFace(FaceCharacter const& faceCharacter, const cv::Scalar color, float const intensity);
	void drawTailorFrame();


private:
	std::string restoreLastURLAddress();
	void saveURLAddress();

};


