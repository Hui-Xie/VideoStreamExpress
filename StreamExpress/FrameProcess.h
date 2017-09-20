

#pragma once


#include "emotient.hpp"
#include <opencv2/opencv.hpp>
#include "FaceContainer.h"

#define Facet_Dir   "C:\\facets"   //for Sheen's PC
//#define Facet_Dir   "c:\\Emotient\\sdk\\facets"    //for Lab Win 7 Server

struct FaceCharacter{
	int faceX;
	int faceY;
	int faceW;
	int faceH;
	cv::Point leftEyeLeteral;
	cv::Point leftEyePupil;
	cv::Point leftEyeMedial;
	cv::Point rightEyePupil;
	cv::Point rightEyeMedial;
	cv::Point rightEyeLeteral;
	cv::Point noseTip;

	
};


class FrameProcess{
private:
	EMOTIENT::FacetSDK::FrameAnalyzer m_frameAnalyzer;
	EMOTIENT::FacetSDK::FrameAnalysis m_frameAnalysis;
	EMOTIENT::FacetSDK::Face m_largestFace;
	std::string m_facetsDir; //facetSDK License and Config file Dir
	int m_minFaceWidth;
	bool m_initializtionSuccess;
	FaceCharacter m_faceCharacter;
	cv::Point cvPoint(EMOTIENT::FacetSDK::Point const point);
	int m_offsetX;
	int m_offsetY;

public:
	FrameProcess();
	~FrameProcess();

	void analyzeFrame(cv::Mat& grayFrame);
	int getMultiFacesFromAFrame(cv::Mat& grayFrame, FaceContainer& faceContainer);
	void baseline();
	EMOTIENT::FacetSDK::Face getLargestFace();
	void setFacetsDir(std::string dirPath);
	void setMinFaceDetectionWidth(int width);
	bool isInitializeSuccess();
	FaceCharacter& getFaceCharacter(int const offsetX, int const offsetY);
	void getOverallSentiment(cv::Scalar& color, float& intensity);
	
	
	
};



