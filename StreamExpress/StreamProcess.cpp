#include "stdafx.h"
#include "StreamProcess.h"


#define PI 3.14159265
#define KEYH 0x7E
#define KEYL 0x2A  //IP and Password Encription
#define ConfigFile "config.bin"

StreamProcess::StreamProcess()
{
	m_isIPCamera = false;
	m_frameNO = 0;

	//offset persent of tailor image
	m_leftOffset = 0;
	m_rightOffset = 0;
	m_topOffset = 0;
	m_bottomOffset = 0;

	//offset pixel number
	m_offsetX = 0;
	m_offsetY = 0;

}


StreamProcess::~StreamProcess()
{
	if (m_videoCapture.isOpened()) {
		m_videoCapture.release();
    }
	

}

void StreamProcess::closeCamera(){
	if (m_videoCapture.isOpened()) 	m_videoCapture.release();
}

void StreamProcess::openCamera(){
	if (m_videoCapture.isOpened()) m_videoCapture.release();
	
	if (m_isIPCamera && m_URLAddress.length() > 0) openIPCamera();
	else openWebCamera();
}

bool StreamProcess::isOpened(){
	return m_videoCapture.isOpened();
}


std::string StreamProcess::restoreLastURLAddress(){
	std::string fileName = ConfigFile;
	CFile encriptionFile;
	std::string urlAddress;
	urlAddress.clear();
	if (encriptionFile.Open(fileName.c_str(), CFile::modeRead | CFile::typeBinary)){
		int length = encriptionFile.GetLength();
		unsigned char *str = new unsigned char[length];
		encriptionFile.Read(str, length);
		unsigned char HighRandom = str[0] ^ KEYH;
		unsigned char LowRandom = str[1] ^ KEYL;

		//shift to left 2 bytes
		for (int i = 0; i < length-2; i++){
			str[i] = str[i + 2];
		}
		str[length - 2] = '\0';
		str[length - 1] = '\0';

		length = length - 2;

		for (int i = 0; i < length; i += 2){
			str[i] = str[i] ^ HighRandom;
			if (i+1 < length){
				str[i + 1] = str[i + 1] ^ LowRandom;
			}
		}
		urlAddress = (char*)str;
		delete[] str;
		encriptionFile.Close();
	}
	else{
		urlAddress.clear();
	}

	return urlAddress;
}


void StreamProcess::saveURLAddress(){
	std::string fileName = ConfigFile;
	//delete the old config file
	if (PathFileExists(fileName.c_str())) CFile::Remove(fileName.c_str());
		
	CFile encriptionFile(fileName.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	encriptionFile.SeekToBegin();
	
	srand((unsigned)time(NULL));
	unsigned short random = rand();
	unsigned char mask = (random>>8) ^ KEYH;
	encriptionFile.Write(&mask, 1);
	mask = (random & 0x00FF) ^ KEYL;
	encriptionFile.Write(&mask, 1);

	unsigned char HighRandom = random >> 8;
	unsigned char LowRandom = random & 0x00FF;
		
	int urlLength = (int)m_URLAddress.length();
	unsigned char* maskStr = new unsigned char[urlLength];
	for (int i = 0; i < urlLength; i += 2){
		maskStr[i] = m_URLAddress[i] ^ HighRandom;
		if (i+1 < urlLength){
			maskStr[i + 1] = m_URLAddress[i + 1] ^ LowRandom;
		}
	}
	encriptionFile.Write(maskStr, urlLength);
	encriptionFile.Flush();
	encriptionFile.Close();
	delete[] maskStr;
}


void StreamProcess::setIPCameraURL(std::string const URLAddress){
	m_isIPCamera = true;
	if (URLAddress.empty()){
		m_URLAddress = restoreLastURLAddress();
	}
	else{
		m_URLAddress = URLAddress;
	}
}

void StreamProcess::setWebCamera(){
	m_isIPCamera = false;
}


void StreamProcess::openWebCamera(){

	m_frameNO = 0;
	m_videoCapture.open(0);
	
}

void StreamProcess::openIPCamera(){

	m_frameNO = 0;
	//m_videoCapture.open("rtsp://face@iowa:facet2015!@128.255.246.70:88/videoMain"); 
	m_videoCapture.open(m_URLAddress); //if ip address is error, this fucntion will crash. it is opencv 2410's bug.
	
	if (!m_videoCapture.isOpened()){
		AfxMessageBox("IP camera is not open.");
	}
	else{
		saveURLAddress();
	}
}

cv::Mat StreamProcess::getMatFrame(){
	//cv::Mat frame;
	//m_videoCapture >> frame;
	//cvtColor(frame, m_grayMatFrame, cv::COLOR_BGR2GRAY);
	m_videoCapture >> m_matFrame;
	GetLocalTime(&m_frameTime);
	GetTailoredSize();
	m_frameNO++;
	return m_matFrame;
	
}


cv::Mat StreamProcess::getGrayMatFrame(){
	cv::Mat tailoringFrame = cv::Mat(m_matFrame, m_tailorRect);
	cv::cvtColor(tailoringFrame, m_grayMatFrame, cv::COLOR_BGR2GRAY);
	return m_grayMatFrame;

}

cv::Rect StreamProcess::GetTailoredSize(){
	m_offsetX = m_leftOffset* getFrameWidth() / 100;
	m_offsetY = m_topOffset * getFrameHeight() / 100;
	int width = getFrameWidth()*(100 - m_leftOffset - m_rightOffset) / 100;
	int height = getFrameHeight()*(100 - m_topOffset - m_bottomOffset) / 100;
	m_tailorRect = cv::Rect(m_offsetX, m_offsetY, width, height);
	return m_tailorRect;
}

void StreamProcess::drawTailorFrame(){
	const cv::Scalar whiteColor(255, 255, 255);//white
	cv::rectangle(m_matFrame, m_tailorRect, whiteColor, 1, 4, 0);
}

int StreamProcess::getFrameWidth(){

	return (int)m_videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
}

int StreamProcess::getFrameHeight(){
	
	return (int)m_videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);

}

SYSTEMTIME StreamProcess::getFrameTime(){
	return m_frameTime;
}

int StreamProcess::analyzeStream(FrameProcess& frameProcess,FaceContainer& faceContainer){
	cv::Mat &frame = getGrayMatFrame();
	int nFaces = frameProcess.getMultiFacesFromAFrame(frame, faceContainer);
	m_frameProperty.m_frameNO = m_frameNO;
	m_frameProperty.m_time = m_frameTime;
	m_frameProperty.m_offsetX = m_offsetX;
	m_frameProperty.m_offsetY = m_offsetY;
	for (int i = 0; i < nFaces; i++){
		faceContainer.addFrameProperty(m_frameProperty);
	}
	return nFaces;
}


// intensity <0  negative face;
// intensity >0  positive face;
void StreamProcess::drawSmileFrownFace(FaceCharacter const& faceCharacter, const cv::Scalar color, float const intensity){
	
	//Opencv Scalar(B,G,R)
	//opencv ellipse clockwise direction is positive
	
	float rotateTan = (faceCharacter.rightEyeLeteral.y - faceCharacter.leftEyeLeteral.y)*1.0 
		                / (faceCharacter.rightEyeLeteral.x - faceCharacter.leftEyeLeteral.x);
	double rotateAngle = atan(rotateTan) * 180 / PI;
	
	//face
	cv::Point faceCenter(faceCharacter.faceX + faceCharacter.faceW / 2, faceCharacter.faceY + faceCharacter.faceW *3/ 8);
	int faceRadius = faceCharacter.faceW/2;// 
	cv::Size faceAxes(faceRadius, faceRadius*5/4);//H:W= 5:4
	ellipse(m_matFrame, faceCenter, faceAxes, rotateAngle, 0, 360, color, -1, CV_AA, 0);
	//circle(m_matFrame, faceCenter, faceRadius, color, -1, CV_AA, 0);//-1 means fill the cricle

	//eyes and pupils
	const cv::Scalar blackColor(0, 0, 0);//black
	const cv::Scalar whiteColor(255, 255, 255);//white
	cv::Point leftEyeCenter(faceCharacter.leftEyePupil.x, faceCharacter.leftEyePupil.y);
	cv::Point rightEyeCenter(faceCharacter.rightEyePupil.x, faceCharacter.rightEyePupil.y);
	int eyeRadius = (faceCharacter.rightEyeLeteral.x - faceCharacter.rightEyeMedial.x)/3;
	int pupilRadius = eyeRadius / 4;
	circle(m_matFrame, leftEyeCenter, eyeRadius, blackColor, -1, CV_AA, 0);
	circle(m_matFrame, rightEyeCenter, eyeRadius, blackColor, -1, CV_AA, 0);
	circle(m_matFrame, leftEyeCenter, pupilRadius, whiteColor, -1, CV_AA, 0);
	circle(m_matFrame, rightEyeCenter, pupilRadius, whiteColor, -1, CV_AA, 0);

	//draw nose
	std::vector<cv::Point> pointsVector;
	const cv::Point  *points;
	pointsVector.clear();
	int const heighDifferencePupils = faceCharacter.rightEyePupil.y - faceCharacter.leftEyePupil.y;
	pointsVector.push_back(cv::Point((faceCharacter.leftEyeMedial.x + faceCharacter.rightEyeMedial.x) / 2,
	                                 (faceCharacter.leftEyeMedial.y + faceCharacter.rightEyeMedial.y) / 2));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeMedial.x, faceCharacter.noseTip.y - heighDifferencePupils/2));
	//pointsVector.push_back(cv::Point(faceCharacter.noseTip.x, faceCharacter.noseTip.y + eyeRadius));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeMedial.x, faceCharacter.noseTip.y + heighDifferencePupils / 2));
	pointsVector.push_back(cv::Point((faceCharacter.leftEyeMedial.x + faceCharacter.rightEyeMedial.x) / 2,
	                                 (faceCharacter.leftEyeMedial.y + faceCharacter.rightEyeMedial.y) / 2));
		
	points = (cv::Point*) cv::Mat(pointsVector).data;
	int nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, false, blackColor, 1, CV_AA, 0);//thickness =1;

	//mouth
	int const mouthMiddleX = (faceCharacter.leftEyeLeteral.x + faceCharacter.rightEyeLeteral.x) / 2;
	int const EyesMiddleY = (faceCharacter.leftEyeMedial.y + faceCharacter.rightEyeMedial.y) / 2;
	int const DisEyeToMouth = (faceCharacter.faceY + faceCharacter.faceH - EyesMiddleY) * 2 / 3;
	int const mouthMiddleY = EyesMiddleY + DisEyeToMouth;
	cv::Point mouthCenter(mouthMiddleX, mouthMiddleY);
	int const mouthW = faceCharacter.rightEyePupil.x - faceCharacter.leftEyePupil.x;
	cv::Size mouhtAxes(mouthW / 2, mouthW * abs(intensity) / 3);
	
	double startAngle = 0;
	double endAngle = 0;
	if (intensity < 0){ //Negative emotion
		startAngle = -10;
		endAngle = -170;
	}
	else if (intensity > 0){    //positive emotion
		startAngle = 10;
		endAngle = 170;
	}
	else {               //neutral emotion
		startAngle = 0;
		endAngle = 180;
	}
	ellipse(m_matFrame, mouthCenter, mouhtAxes, rotateAngle, startAngle, endAngle, blackColor, 1, CV_AA, 0);
}


void StreamProcess::drawFaceFigure(FaceCharacter const& faceCharacter){
	int const leftEyeW = faceCharacter.leftEyeMedial.x - faceCharacter.leftEyeLeteral.x;
	int const leftEyeH = leftEyeW / 2;
	int const rightEyeW = faceCharacter.rightEyeLeteral.x - faceCharacter.rightEyeMedial.x;
	int const rightEyeH = rightEyeW / 2;
	int const LeftEyeRadius = leftEyeH / 4;
	int const rightEyeRadius = rightEyeH / 4;

	int const mouthW = faceCharacter.rightEyePupil.x - faceCharacter.leftEyePupil.x;
	int const mouthH = (leftEyeH + rightEyeH) / 2;

	//distance form eyeMedial to Mouth Middle 
	int const EyesMiddleY = (faceCharacter.leftEyeMedial.y + faceCharacter.rightEyeMedial.y) / 2;
	int const DisEyeToMouth = (faceCharacter.faceY + faceCharacter.faceH - EyesMiddleY) *2/3;

	int const mouthLeftY = faceCharacter.leftEyePupil.y + DisEyeToMouth;
	int const mouthMiddleY = EyesMiddleY + DisEyeToMouth;
	int const mouthRightY = faceCharacter.rightEyePupil.y + DisEyeToMouth;
	int const mouthMiddleX = (faceCharacter.leftEyeLeteral.x + faceCharacter.rightEyeLeteral.x) / 2;
	
	int const noseW = faceCharacter.rightEyeMedial.x - faceCharacter.leftEyeMedial.x;

	int nPoints;
	const cv::Scalar color(0, 255, 0);//green
	std::vector<cv::Point> pointsVector;
	const cv::Point  *points;

	//draw brow
	pointsVector.clear();
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeLeteral.x - leftEyeW/4, faceCharacter.leftEyeLeteral.y - leftEyeW/5));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeLeteral.x, faceCharacter.leftEyeLeteral.y - leftEyeW * 2 / 5));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyePupil.x, faceCharacter.leftEyePupil.y - leftEyeW/2));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeMedial.x, faceCharacter.leftEyeMedial.y - leftEyeW * 2 / 5));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeMedial.x + leftEyeW/4, faceCharacter.leftEyeMedial.y - leftEyeW/5));
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, false, color, 1, CV_AA, 0);//thickness =1;  CV_AA = antialiased line.

	pointsVector.clear();
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeMedial.x - rightEyeW /4, faceCharacter.rightEyeMedial.y - rightEyeW/5));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeMedial.x, faceCharacter.rightEyeMedial.y - rightEyeW * 2 / 5));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyePupil.x, faceCharacter.rightEyePupil.y - rightEyeW / 2));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeLeteral.x, faceCharacter.rightEyeLeteral.y - rightEyeW * 2 / 5));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeLeteral.x + rightEyeW/4, faceCharacter.rightEyeLeteral.y - rightEyeW/5));
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, false, color, 1, CV_AA, 0);//thickness =1;
	
	//draw eye
	pointsVector.clear();
	pointsVector.push_back(faceCharacter.leftEyeLeteral);
	pointsVector.push_back(cv::Point((faceCharacter.leftEyeLeteral.x + faceCharacter.leftEyePupil.x) / 2, faceCharacter.leftEyePupil.y - leftEyeH/3));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyePupil.x, faceCharacter.leftEyePupil.y - leftEyeH/2));
	pointsVector.push_back(cv::Point((faceCharacter.leftEyePupil.x + faceCharacter.leftEyeMedial.x) / 2, faceCharacter.leftEyePupil.y - leftEyeH / 3));
	pointsVector.push_back(faceCharacter.leftEyeMedial);
	pointsVector.push_back(cv::Point((faceCharacter.leftEyePupil.x + faceCharacter.leftEyeMedial.x) / 2, faceCharacter.leftEyePupil.y + leftEyeH / 3));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyePupil.x, faceCharacter.leftEyePupil.y + leftEyeH/2));
	pointsVector.push_back(cv::Point((faceCharacter.leftEyeLeteral.x + faceCharacter.leftEyePupil.x) / 2, faceCharacter.leftEyePupil.y + leftEyeH / 3));
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, true, color, 1, CV_AA, 0);

	pointsVector.clear();
	pointsVector.push_back(faceCharacter.rightEyeMedial);
	pointsVector.push_back(cv::Point((faceCharacter.rightEyeMedial.x + faceCharacter.rightEyePupil.x) / 2, faceCharacter.rightEyePupil.y - rightEyeH / 3));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyePupil.x, faceCharacter.rightEyePupil.y - rightEyeH / 2));
	pointsVector.push_back(cv::Point((faceCharacter.rightEyePupil.x + faceCharacter.rightEyeLeteral.x) / 2, faceCharacter.rightEyePupil.y - rightEyeH / 3));
	pointsVector.push_back(faceCharacter.rightEyeLeteral);
	pointsVector.push_back(cv::Point((faceCharacter.rightEyeLeteral.x + faceCharacter.rightEyePupil.x) / 2, faceCharacter.rightEyePupil.y + rightEyeH / 3));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyePupil.x, faceCharacter.rightEyePupil.y + rightEyeH / 2));
	pointsVector.push_back(cv::Point((faceCharacter.rightEyePupil.x + faceCharacter.rightEyeMedial.x) / 2, faceCharacter.rightEyePupil.y + rightEyeH / 3));
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, true, color, 1, CV_AA, 0);

	//draw pupil
	cv::circle(m_matFrame, faceCharacter.leftEyePupil, LeftEyeRadius, cv::Scalar(255, 255, 255), 1, CV_AA, 0);
	cv::circle(m_matFrame, faceCharacter.rightEyePupil, rightEyeRadius, cv::Scalar(255, 255, 255), 1, CV_AA, 0);
	
	//draw nose
	pointsVector.clear();
	pointsVector.push_back(cv::Point((faceCharacter.leftEyeMedial.x + faceCharacter.rightEyeMedial.x) / 2,
		                             (faceCharacter.leftEyeMedial.y + faceCharacter.rightEyeMedial.y) / 2));
	pointsVector.push_back(faceCharacter.noseTip);
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, false, color, 1, CV_AA, 0);//thickness =1;

	//draw mouth
	pointsVector.clear();
	pointsVector.push_back(cv::Point(faceCharacter.leftEyePupil.x + LeftEyeRadius, mouthLeftY));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeMedial.x, mouthLeftY - mouthH / 3));
	pointsVector.push_back(cv::Point(mouthMiddleX, mouthMiddleY - mouthH / 2));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeMedial.x, mouthRightY - mouthH / 3));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyePupil.x - rightEyeRadius, mouthRightY));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeMedial.x, mouthRightY + mouthH / 3));
	pointsVector.push_back(cv::Point(mouthMiddleX, mouthMiddleY + mouthH / 2));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeMedial.x, mouthLeftY + mouthH / 3));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyePupil.x + LeftEyeRadius, mouthLeftY));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyePupil.x - rightEyeRadius, mouthRightY));
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, false, color, 1, CV_AA, 0);


	//draw face outline
	pointsVector.clear();
	pointsVector.push_back(cv::Point(faceCharacter.faceX, faceCharacter.leftEyeLeteral.y));
	pointsVector.push_back(cv::Point(faceCharacter.faceX, faceCharacter.faceY + faceCharacter.faceH/2));
	
	pointsVector.push_back(cv::Point(faceCharacter.faceX, faceCharacter.noseTip.y));
	pointsVector.push_back(cv::Point((faceCharacter.leftEyeLeteral.x + faceCharacter.faceX)/2, mouthLeftY));
	pointsVector.push_back(cv::Point(faceCharacter.leftEyeMedial.x, faceCharacter.faceY + faceCharacter.faceH));
	pointsVector.push_back(cv::Point(faceCharacter.rightEyeMedial.x, faceCharacter.faceY + faceCharacter.faceH));
	pointsVector.push_back(cv::Point((faceCharacter.rightEyeLeteral.x + faceCharacter.faceX + faceCharacter.faceW)/2, mouthRightY));
	pointsVector.push_back(cv::Point(faceCharacter.faceX + faceCharacter.faceW, faceCharacter.noseTip.y));

	pointsVector.push_back(cv::Point(faceCharacter.faceX + faceCharacter.faceW, faceCharacter.faceY + faceCharacter.faceH / 2));
	pointsVector.push_back(cv::Point(faceCharacter.faceX + faceCharacter.faceW, faceCharacter.rightEyeLeteral.y));
	points = (cv::Point*) cv::Mat(pointsVector).data;
	nPoints = cv::Mat(pointsVector).rows;
	cv::polylines(m_matFrame, &points, &nPoints, 1, false, color, 1, CV_AA, 0);

}

