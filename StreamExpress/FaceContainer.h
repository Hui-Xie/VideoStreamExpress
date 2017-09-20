
#pragma once

#include "emotient.hpp"
#include "afx.h"




struct FrameProperty{
	std::string m_frameName;
	long m_frameNO;      //reserve for video
	SYSTEMTIME m_time;
	int m_millisecond; //time offset of a frame in video; reserve for video
	int m_offsetX;//for Tailor Image;
	int m_offsetY;//for Tailor Image;
};

class FaceContainer
{
private:
	std::vector<EMOTIENT::FacetSDK::Face> m_vectorFaces;
	std::vector<FrameProperty> m_vectorFrameProperty;
	CFile m_cFile;
	void printBlankComma(const int num);
	void internalPrint(const std::string& buff);

public:
	FaceContainer();
	~FaceContainer();

	void clearAllVector();
	bool isEmpty();

	void addFrameProperty(const FrameProperty frameProp);
	void addFace(const EMOTIENT::FacetSDK::Face face);
	
	void openOutputFile(std::string path);
	void openStreamOutputFile(CString path);

	void printCSVHead();
	void printAllResult();

	void printFrameProperty(const FrameProperty& frameProp);
	void printFaceSize(const EMOTIENT::FacetSDK::Face& face, int offsetX, int offsetY, bool nonBlank = true);
	void printPose(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true); //Yaw, Pitch, Roll
	void printProperty(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true);
	void printDemographic(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true);
	void printPrimaryEmotions(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true);
	void printAdvancedEmotions(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true);
	void printOverallSentiment(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true);
	void printAU(const EMOTIENT::FacetSDK::Face& face, bool nonBlank = true);

	void printNewLineChar();


	void closeOutputFile();


};

